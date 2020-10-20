/* Copyright (c) 2017-2020, Hans Erik Thrane */

#define USE_UNWIND

#if defined(USE_UNWIND)
#define UNW_LOCAL_ONLY
#endif

#include "roq/logging.h"

#include <signal.h>
#include <unistd.h>

#include <absl/debugging/stacktrace.h>
#include <absl/debugging/symbolize.h>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <memory>

#if defined(USE_UNWIND)
#include "roq/unwind.h"
#endif

namespace roq {

constexpr size_t MESSAGE_BUFFER_SIZE = 65536;

constexpr size_t SPDLOG_QUEUE_SIZE = 1024 * 1024;
constexpr size_t SPDLOG_THREAD_COUNT = 1;
constexpr size_t SPDLOG_FLUSH_SECONDS = 1;

namespace detail {
template <typename T, std::size_t alignment>
struct aligned_allocator {
  using value_type = T;

  aligned_allocator() = default;

  T *allocate(std::size_t size) {
    auto result = std::aligned_alloc(alignment, size);
    return reinterpret_cast<T *>(result);
  }
  void deallocate(T *pointer, std::size_t) noexcept { ::free(pointer); }
};

constexpr size_t PAGE_SIZE = 4096;

template <typename T>
struct page_aligned_allocator : public aligned_allocator<T, PAGE_SIZE> {};

template <typename T>
struct page_aligned_vector : public std::vector<T, page_aligned_allocator<T> > {
  using base_type = std::vector<T, page_aligned_allocator<T> >;
  using base_type::base_type;
};

thread_local page_aligned_vector<char> RAW_BUFFER(MESSAGE_BUFFER_SIZE);

thread_local std::pair<char *, size_t> message_buffer(
    &RAW_BUFFER[0], RAW_BUFFER.size());
}  // namespace detail

namespace {
inline bool likely(bool expr) {
  return __builtin_expect(expr, true);
}
}  // namespace

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
  void *addr[32];
  int depth = absl::GetStackTrace(addr, std::size(addr), 0);
  if (depth) {
    char name[1024];
    for (int i = 0; i < depth; ++i) {
      const char *symbol = "(unknown)";
      auto result = absl::Symbolize(addr[0], name, sizeof(name));
      if (result) symbol = name;
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
static spdlog::logger *spdlog_logger = nullptr;
}  // namespace

namespace detail {
int verbosity = 0;

sink_t info = [](const std::string_view &message) {
  if (likely(spdlog_logger)) spdlog_logger->info(message);
};

sink_t warning = [](const std::string_view &message) {
  if (likely(spdlog_logger)) spdlog_logger->warn(message);
};

sink_t error = [](const std::string_view &message) {
  if (likely(spdlog_logger)) spdlog_logger->error(message);
};

sink_t critical = [](const std::string_view &message) {
  if (likely(spdlog_logger)) {
    spdlog_logger->critical(message);
    spdlog_logger->flush();
  }
  std::abort();
};
}  // namespace detail

void Logger::initialize(
    const std::string_view &arg0,
    const std::string_view &pattern,
    bool stacktrace) {
  // abseil
  initialize_abseil(arg0);
  // spdlog
  auto terminal = ::isatty(fileno(stdout));
  std::shared_ptr<spdlog::logger> logger;
  if (terminal) {
    logger = spdlog::stdout_logger_st("spdlog");
  } else {
    spdlog::init_thread_pool(SPDLOG_QUEUE_SIZE, SPDLOG_THREAD_COUNT);
    spdlog::flush_every(std::chrono::seconds(SPDLOG_FLUSH_SECONDS));
    logger = spdlog::stdout_logger_st<spdlog::async_factory>("spdlog");
  }
  logger->set_pattern(std::string(pattern));
  logger->flush_on(spdlog::level::warn);
  // note! spdlog uses reference count
  spdlog_logger = logger.get();
  auto verbosity = std::getenv("ROQ_v");
  if (verbosity != nullptr && std::strlen(verbosity) > 0)
    detail::verbosity = std::atoi(verbosity);
  if (stacktrace) install_failure_signal_handler();
}

void Logger::shutdown() {
  // note! not thread-safe
  if (spdlog_logger) {
    spdlog_logger->flush();
    spdlog_logger = nullptr;
  }
  spdlog::drop_all();
}

}  // namespace roq
