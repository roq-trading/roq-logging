/* Copyright (c) 2017-2021, Hans Erik Thrane */

#define USE_UNWIND

#if defined(USE_UNWIND)
#define UNW_LOCAL_ONLY
#endif

#include "roq/logging.h"

#include <absl/debugging/stacktrace.h>
#include <absl/debugging/symbolize.h>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>

#if defined(USE_UNWIND)
#include "roq/unwind.h"
#endif

using namespace std::literals;         // NOLINT
using namespace std::chrono_literals;  // NOLINT

namespace roq {

namespace {
static const auto MESSAGE_BUFFER_SIZE = 65536;
static const auto SPDLOG_QUEUE_SIZE = 1024 * 1024;
static const auto SPDLOG_THREAD_COUNT = 1;
static const auto SPDLOG_FLUSH_EVERY = 1s;
}  // namespace

namespace detail {
template <typename T, std::size_t alignment>
struct aligned_allocator {
  using value_type = T;

  aligned_allocator() = default;

  T *allocate(std::size_t size) {
#if __APPLE__
    void *result;
    if (posix_memalign(&result, alignment, size) != 0)
      throw std::bad_alloc();
#else
    auto result = std::aligned_alloc(alignment, size);
#endif
    return reinterpret_cast<T *>(result);
  }
  void deallocate(T *pointer, std::size_t) noexcept { ::free(pointer); }
};

template <typename T>
struct page_aligned_allocator : public aligned_allocator<T, ROQ_PAGE_SIZE> {};

template <typename T>
struct page_aligned_vector : public std::vector<T, page_aligned_allocator<T> > {
  using base_type = std::vector<T, page_aligned_allocator<T> >;
  using base_type::base_type;
};

thread_local page_aligned_vector<char> RAW_BUFFER(MESSAGE_BUFFER_SIZE);

thread_local std::pair<char *, size_t> message_buffer(&RAW_BUFFER[0], RAW_BUFFER.size());
}  // namespace detail

namespace {
static void initialize_abseil(const std::string_view &arg0) {
  std::string tmp(arg0);
  absl::InitializeSymbolizer(tmp.c_str());
}
}  // namespace

namespace {
static void invoke_default_signal_handler(int signal) {
  struct sigaction sa = {};
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = SIG_DFL;
  sigaction(signal, &sa, nullptr);
  kill(getpid(), signal);
}

static void termination_handler(int sig, siginfo_t *info, void *) {
  fprintf(stderr, "*** TERMINATION HANDLER ***\n");
#if defined(USE_UNWIND)
  unwind::print_stacktrace(sig, info);
#else
#if defined(__linux__)
  psiginfo(info, nullptr);
#endif
  std::array<void *, 32> addr;
  int depth = ::backtrace(std::data(addr), std::size(addr));
  if (depth) {
    std::array<char, 256> name;
    for (int i = 0; i < depth; ++i) {
      char const *symbol = "(unknown)";
      // note! this signature does not include the arguments
      // --> so we still prefer libunwind
      auto result = absl::Symbolize(addr[i], std::data(name), std::size(name));
      if (result)
        symbol = std::data(name);
      fprintf(stderr, "[%2d] %p %s\n", i, addr[i], symbol);
    }
  } else {
    fprintf(stderr, "can't get stacktrace\n");
  }
#endif
  invoke_default_signal_handler(sig);
}

static void install_failure_signal_handler() {
  struct sigaction sa = {};
  sa.sa_sigaction = termination_handler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGABRT, &sa, nullptr);
  sigaction(SIGILL, &sa, nullptr);
  sigaction(SIGSEGV, &sa, nullptr);
}
}  // namespace

namespace {
static spdlog::logger *SPDLOG_OUT = nullptr;
static spdlog::logger *SPDLOG_ERR = nullptr;
}  // namespace

namespace detail {
int verbosity = 0;

sink_t INFO = [](const std::string_view &message) {
  if (ROQ_LIKELY(SPDLOG_OUT)) {
    SPDLOG_OUT->info(message);
  } else {
    std::cout << message << std::endl;
  }
};

sink_t WARNING = [](const std::string_view &message) {
  if (ROQ_LIKELY(SPDLOG_OUT)) {
    SPDLOG_OUT->warn(message);
  } else {
    std::cout << message << std::endl;
  }
};

sink_t ERROR = [](const std::string_view &message) {
  if (ROQ_LIKELY(SPDLOG_ERR)) {
    SPDLOG_ERR->error(message);
  } else {
    std::cerr << message << std::endl;
  }
};

sink_t CRITICAL = [](const std::string_view &message) {
  if (ROQ_LIKELY(SPDLOG_ERR)) {
    SPDLOG_ERR->critical(message);
    SPDLOG_ERR->flush();
  } else {
    std::cerr << message << std::endl;
  }
};
}  // namespace detail

void Logger::initialize(const std::string_view &arg0, const std::string_view &pattern, bool stacktrace) {
  // abseil
  initialize_abseil(arg0);
  // spdlog
  auto terminal = ::isatty(fileno(stdout));
  std::shared_ptr<spdlog::logger> out, err;
  if (terminal) {
    // note! almost similar to std::cout/std::cerr, only using spdlog for buffering
    out = spdlog::stdout_color_mt("spdlog_out"s);
    err = spdlog::stderr_color_mt("spdlog_err"s);
  } else {
    spdlog::init_thread_pool(SPDLOG_QUEUE_SIZE, SPDLOG_THREAD_COUNT);
    spdlog::flush_every(std::chrono::seconds(SPDLOG_FLUSH_EVERY));
    out = spdlog::stdout_logger_st<spdlog::async_factory>("spdlog"s);
    // note! no dedicated err stream when running as sevice
    // reason: avoid potential timing issues when interleaving two streams
  }
  out->set_pattern(std::string(pattern));
  out->flush_on(spdlog::level::warn);
  if (err) {
    err->set_pattern(std::string(pattern));
    err->flush_on(spdlog::level::warn);
  }
  // note! spdlog uses reference count
  SPDLOG_OUT = out.get();
  SPDLOG_ERR = err ? err.get() : SPDLOG_OUT;
  auto verbosity = std::getenv("ROQ_v");
  if (verbosity != nullptr && std::strlen(verbosity) > 0)
    detail::verbosity = std::atoi(verbosity);
  if (stacktrace)
    install_failure_signal_handler();
}

void Logger::shutdown() {
  // note! not thread-safe
  if (SPDLOG_OUT) {
    SPDLOG_OUT->flush();
    SPDLOG_OUT = nullptr;
  }
  if (SPDLOG_ERR) {
    SPDLOG_ERR->flush();
    SPDLOG_ERR = nullptr;
  }
  spdlog::drop_all();
}

}  // namespace roq
