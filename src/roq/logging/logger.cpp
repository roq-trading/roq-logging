/* Copyright (c) 2017-2023, Hans Erik Thrane */

#define USE_UNWIND

#if defined(USE_UNWIND)
#define UNW_LOCAL_ONLY
#endif

#include "roq/logging/logger.hpp"

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

#include "roq/utils/compare.hpp"

#include "roq/logging/shared.hpp"

using namespace std::literals;
using namespace std::chrono_literals;  // NOLINT

namespace roq {
namespace logging {

// === CONSTANTS ===

namespace {
constexpr auto const SPDLOG_QUEUE_SIZE = size_t{1048576};
constexpr auto const SPDLOG_THREAD_COUNT = size_t{1};
}  // namespace

// === HELPERS ===

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
      auto result = absl::Symbolize{addr[i], std::data(name), std::size(name)};
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

// === GLOBAL ===

namespace {
spdlog::logger *SPDLOG_OUT = nullptr;
spdlog::logger *SPDLOG_ERR = nullptr;
}  // namespace

// === EXTERN ===

sink_type INFO = [](std::string_view const &message) {
  if (SPDLOG_OUT) [[likely]] {
    (*SPDLOG_OUT).log(spdlog::level::info, message);
  } else {
    std::cout << message << std::endl;
  }
};

sink_type WARNING = [](std::string_view const &message) {
  if (SPDLOG_OUT) [[likely]] {
    (*SPDLOG_OUT).log(spdlog::level::warn, message);
  } else {
    std::cout << message << std::endl;
  }
};

sink_type ERROR = [](std::string_view const &message) {
  if (SPDLOG_ERR) [[likely]] {
    (*SPDLOG_ERR).log(spdlog::level::err, message);
  } else {
    std::cerr << message << std::endl;
  }
};

sink_type CRITICAL = [](std::string_view const &message) {
  if (SPDLOG_ERR) [[likely]] {
    (*SPDLOG_ERR).log(spdlog::level::critical, message);
    (*SPDLOG_ERR).flush();
  } else {
    std::cerr << message << std::endl;
  }
};

// === IMPLEMENTATION ===

Logger::Logger(std::span<std::string_view> const &args, logging::Settings const &settings, bool stacktrace) {
  if (!std::empty(args)) {
    std::string arg0{args[0]};
    absl::InitializeSymbolizer(arg0.c_str());
  }
  // note! to detach from terminal: use nohup, systemd, etc.
  auto terminal = ::isatty(fileno(stdout));
  // terminal color
  if (utils::case_insensitive_compare(settings.log.color, "always"sv) == 0) {
    terminal_color = true;
  } else if (utils::case_insensitive_compare(settings.log.color, "auto"sv) == 0) {
    terminal_color = terminal;
  } else if (utils::case_insensitive_compare(settings.log.color, "none"sv) == 0) {
    terminal_color = false;
  } else {
    fmt::print(stderr, R"(Unknown color: "{}"\n)"sv, settings.log.color);
    std::exit(EXIT_FAILURE);
  }
  // note! non-interactive sessions are asynchronous
  auto interactive = std::empty(settings.log.path) && terminal;
  if (!interactive) {
    spdlog::init_thread_pool(SPDLOG_QUEUE_SIZE, SPDLOG_THREAD_COUNT);
    if (settings.log.flush_freq.count())
      spdlog::flush_every(std::chrono::duration_cast<std::chrono::seconds>(settings.log.flush_freq));
  }
  std::shared_ptr<spdlog::logger> out, err;
  if (std::empty(settings.log.path)) {
    if (terminal) {
      // note! almost similar to stdout/stderr, only using spdlog for buffering
      if (terminal_color) {
        out = spdlog::stdout_color_mt("spdlog_out"s);
        {
          auto color_sink = static_cast<spdlog::sinks::stdout_color_sink_mt *>((*out).sinks()[0].get());
          (*color_sink).set_color(spdlog::level::info, (*color_sink).white);
          (*color_sink).set_color(spdlog::level::warn, "\033[1m\033[32m"sv);  // bold green
        }
        err = spdlog::stderr_color_mt("spdlog_err"s);
        {
          auto color_sink = static_cast<spdlog::sinks::stdout_color_sink_mt *>((*err).sinks()[0].get());
          (*color_sink).set_color(spdlog::level::err, (*color_sink).red_bold);
          (*color_sink).set_color(spdlog::level::critical, (*color_sink).red_bold);
        }
      } else {
        out = spdlog::stdout_logger_st("spdlog_out"s);
        err = spdlog::stderr_logger_st("spdlog_err"s);
      }
    } else {
      out = spdlog::stdout_logger_st<spdlog::async_factory>("spdlog"s);
    }
  } else {
    out = spdlog::rotating_logger_st<spdlog::async_factory>(
        "spdlog"s,
        std::string{settings.log.path},
        settings.log.max_size,
        settings.log.max_files,
        settings.log.rotate_on_open);
  }
  if (!std::empty(settings.log.pattern))
    (*out).set_pattern(std::string{settings.log.pattern});
  (*out).flush_on(spdlog::level::warn);
  // note! async logging does not use a dedicated err stream
  // reason: avoid potential timing issues when interleaving two streams
  if (err) {
    if (!std::empty(settings.log.pattern))
      (*err).set_pattern(std::string{settings.log.pattern});
    (*err).flush_on(spdlog::level::warn);
  }
  // note! spdlog uses reference count
  SPDLOG_OUT = out.get();
  SPDLOG_ERR = err ? err.get() : SPDLOG_OUT;
  // verbosity
  auto verbosity_2 = std::getenv("ROQ_v");
  if (verbosity_2 != nullptr && std::strlen(verbosity_2) > 0) {
    auto tmp = std::atoi(verbosity_2);
    if (tmp >= 0)
      verbosity = tmp;
  }
  // stacktrace
  if (stacktrace)
    install_failure_signal_handler();
}

Logger::~Logger() {
  try {
    // note! not thread-safe
    if (SPDLOG_OUT) {
      (*SPDLOG_OUT).flush();
      SPDLOG_OUT = nullptr;
    }
    if (SPDLOG_ERR) {
      (*SPDLOG_ERR).flush();
      SPDLOG_ERR = nullptr;
    }
    spdlog::drop_all();
  } catch (...) {
    // note! silent
  }
}

}  // namespace logging
}  // namespace roq