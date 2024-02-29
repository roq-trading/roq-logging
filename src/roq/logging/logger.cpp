/* Copyright (c) 2017-2024, Hans Erik Thrane */

#define USE_UNWIND

#if defined(USE_UNWIND)
#define UNW_LOCAL_ONLY
#endif

#include "roq/logging/logger.hpp"

#include <absl/debugging/stacktrace.h>
#include <absl/debugging/symbolize.h>

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

// === IMPLEMENTATION ===

Logger::Logger(args::Parser const &args, logging::Settings const &settings, bool stacktrace) {
  std::string arg0{args.program_name()};
  absl::InitializeSymbolizer(arg0.c_str());
  // note! to detach from terminal: use nohup, systemd, etc.
  auto terminal = ::isatty(fileno(stdout));
  // terminal color
  if (std::empty(settings.log.color) || utils::case_insensitive_compare(settings.log.color, "auto"sv) == 0) {
    terminal_color = terminal;
  } else if (utils::case_insensitive_compare(settings.log.color, "always"sv) == 0) {
    terminal_color = true;
  } else if (utils::case_insensitive_compare(settings.log.color, "none"sv) == 0) {
    terminal_color = false;
  } else {
    fmt::print(stderr, R"(Unknown color: "{}"\n)"sv, settings.log.color);
    std::exit(EXIT_FAILURE);
  }
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

}  // namespace logging
}  // namespace roq
