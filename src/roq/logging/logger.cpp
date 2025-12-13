/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/logging/logger.hpp"

#include <absl/debugging/stacktrace.h>
#include <absl/debugging/symbolize.h>

#include <fmt/format.h>

#include <execinfo.h>
#include <unistd.h>

#include <chrono>
#include <csignal>
#include <cstdlib>
#include <memory>

#include "roq/logging/shared.hpp"

using namespace std::literals;
using namespace std::chrono_literals;

namespace roq {
namespace logging {

// === CONSTANTS ===

namespace {
size_t const LENGTH_ADDR = 32;
size_t const LENGTH_NAME = 256;
}  // namespace

// === HELPERS ===

namespace {
void invoke_default_signal_handler(int signal) {
  struct sigaction action = {};
  sigemptyset(&action.sa_mask);
  action.sa_handler = SIG_DFL;
  sigaction(signal, &action, nullptr);
  kill(getpid(), signal);
}

void termination_handler(int sig, [[maybe_unused]] siginfo_t *info, void *) {
  fmt::println(stderr, "*** TERMINATION HANDLER ***"sv);
#if defined(__linux__)
  psiginfo(info, nullptr);
#endif
  std::array<void *, LENGTH_ADDR> addr;
  int depth = ::backtrace(std::data(addr), std::size(addr));
  if (depth != 0) {
    std::array<char, LENGTH_NAME> name;
    for (int i = 0; i < depth; ++i) {
      char const *symbol = "(unknown)";
      // note! this signature does not include the arguments
      // --> so we still prefer libunwind
      auto result = absl::Symbolize(addr[i], std::data(name), std::size(name));
      if (result) {
        symbol = std::data(name);
      }
      fmt::println(stderr, "[{:2}] {} {}", i, addr[i], symbol);
    }
  } else {
    fmt::println(stderr, "can't get stacktrace");
  }
  invoke_default_signal_handler(sig);
}

void install_failure_signal_handler() {
  struct sigaction action = {};
  action.sa_sigaction = termination_handler;
  action.sa_flags = SA_SIGINFO;
  sigaction(SIGABRT, &action, nullptr);
  sigaction(SIGILL, &action, nullptr);
  sigaction(SIGSEGV, &action, nullptr);
}
}  // namespace

// === IMPLEMENTATION ===

Logger::Logger(args::Parser const &args, logging::Settings const &settings, bool stacktrace) {
  std::string arg0{args.program_name()};
  absl::InitializeSymbolizer(arg0.c_str());
  // note! to detach from terminal: use nohup, systemd, etc.
  auto terminal = ::isatty(fileno(stdout));
  // terminal color
  if (std::empty(settings.log.color) || settings.log.color == "auto"sv) {
    terminal_color = terminal != 0;
  } else if (settings.log.color == "always"sv) {
    terminal_color = true;
  } else if (settings.log.color == "none"sv) {
    terminal_color = false;
  } else {
    fmt::println(stderr, R"(Unknown color: "{}")"sv, settings.log.color);
    std::exit(EXIT_FAILURE);
  }
  // verbosity
  auto verbosity_2 = std::getenv("ROQ_v");
  if (verbosity_2 != nullptr && std::strlen(verbosity_2) > 0) {
    auto tmp = std::atoi(verbosity_2);
    if (tmp >= 0) {
      verbosity = tmp;
    }
  } else {
    verbosity = settings.log.verbosity;
  }
  // stacktrace
  if (stacktrace) {
    install_failure_signal_handler();
  }
}

}  // namespace logging
}  // namespace roq
