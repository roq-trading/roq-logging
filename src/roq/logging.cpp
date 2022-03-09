/* Copyright (c) 2017-2022, Hans Erik Thrane */

#define USE_UNWIND

#if defined(USE_UNWIND)
#define UNW_LOCAL_ONLY
#endif

#include "roq/logging.hpp"

#include <absl/debugging/stacktrace.h>
#include <absl/debugging/symbolize.h>

#include <spdlog/async.h>
#include <spdlog/spdlog.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>

#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>

#if defined(USE_UNWIND)
#include "roq/unwind.hpp"
#endif

#include "roq/flags.hpp"

using namespace std::literals;         // NOLINT
using namespace std::chrono_literals;  // NOLINT

namespace roq {

namespace {
const size_t MESSAGE_BUFFER_SIZE = 65536;
const size_t SPDLOG_QUEUE_SIZE = 1024 * 1024;
const size_t SPDLOG_THREAD_COUNT = 1;

auto merge_config(const Logger::Config &config) {
  decltype(config) result{
      .pattern = std::empty(config.pattern) ? Flags::log_pattern() : config.pattern,
      .flush_freq = config.flush_freq.count() == 0 ? Flags::log_flush_freq() : config.flush_freq,
      .path = std::empty(config.path) ? Flags::log_path() : config.path,
      .max_size = config.max_size == 0 ? Flags::log_max_size() : config.max_size,
      .max_files = config.max_files == 0 ? Flags::log_max_files() : config.max_files,
      .rotate_on_open = !config.rotate_on_open ? Flags::log_rotate_on_open() : config.rotate_on_open,
  };
  return result;
}
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

thread_local std::pair<char *, size_t> message_buffer(std::data(RAW_BUFFER), std::size(RAW_BUFFER));
}  // namespace detail

namespace {
void initialize_abseil(const std::string_view &arg0) {
  std::string tmp(arg0);
  absl::InitializeSymbolizer(tmp.c_str());
}
}  // namespace

namespace {
void invoke_default_signal_handler(int signal) {
  struct sigaction sa = {};
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = SIG_DFL;
  sigaction(signal, &sa, nullptr);
  kill(getpid(), signal);
}

void termination_handler(int sig, siginfo_t *info, void *) {
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

void install_failure_signal_handler() {
  struct sigaction sa = {};
  sa.sa_sigaction = termination_handler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGABRT, &sa, nullptr);
  sigaction(SIGILL, &sa, nullptr);
  sigaction(SIGSEGV, &sa, nullptr);
}
}  // namespace

namespace {
spdlog::logger *SPDLOG_OUT = nullptr;
spdlog::logger *SPDLOG_ERR = nullptr;
}  // namespace

namespace detail {
int verbosity = 0;

sink_t INFO = [](const std::string_view &message) {
  if (SPDLOG_OUT) [[likely]] {
    SPDLOG_OUT->log(spdlog::source_loc{}, spdlog::level::info, message);
  } else {
    std::cout << message << std::endl;
  }
};

sink_t WARNING = [](const std::string_view &message) {
  if (SPDLOG_OUT) [[likely]] {
    SPDLOG_OUT->log(spdlog::source_loc{}, spdlog::level::warn, message);
  } else {
    std::cout << message << std::endl;
  }
};

sink_t ERROR = [](const std::string_view &message) {
  if (SPDLOG_ERR) [[likely]] {
    SPDLOG_ERR->log(spdlog::source_loc{}, spdlog::level::err, message);
  } else {
    std::cerr << message << std::endl;
  }
};

sink_t CRITICAL = [](const std::string_view &message) {
  if (SPDLOG_ERR) [[likely]] {
    SPDLOG_ERR->log(spdlog::source_loc{}, spdlog::level::critical, message);
    SPDLOG_ERR->flush();
  } else {
    std::cerr << message << std::endl;
  }
};
}  // namespace detail

void Logger::initialize(const std::string_view &arg0, const Config &config, bool stacktrace) {
  // abseil:
  initialize_abseil(arg0);
  // spdlog:
  auto final_config = merge_config(config);
  // note! to detach from terminal: use nohup, systemd, etc.
  auto terminal = ::isatty(fileno(stdout));
  // note! non-interactive sessions are asynchronous
  auto interactive = std::empty(final_config.path) && terminal;
  if (!interactive) {
    spdlog::init_thread_pool(SPDLOG_QUEUE_SIZE, SPDLOG_THREAD_COUNT);
    if (final_config.flush_freq.count())
      spdlog::flush_every(std::chrono::duration_cast<std::chrono::seconds>(final_config.flush_freq));
  }
  std::shared_ptr<spdlog::logger> out, err;
  if (std::empty(final_config.path)) {
    if (terminal) {
      // note! almost similar to stdout/stderr, only using spdlog for buffering
      out = spdlog::stdout_color_mt("spdlog_out"s);
      err = spdlog::stderr_color_mt("spdlog_err"s);
    } else {
      out = spdlog::stdout_logger_st<spdlog::async_factory>("spdlog"s);
    }
  } else {
    out = spdlog::rotating_logger_st<spdlog::async_factory>(
        "spdlog",
        std::string{final_config.path},
        final_config.max_size,
        final_config.max_files,
        final_config.rotate_on_open);
  }
  if (!std::empty(final_config.pattern))
    out->set_pattern(std::string{final_config.pattern});
  out->flush_on(spdlog::level::warn);
  // note! async logging does not use a dedicated err stream
  // reason: avoid potential timing issues when interleaving two streams
  if (err) {
    if (!std::empty(final_config.pattern))
      err->set_pattern(std::string{final_config.pattern});
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
