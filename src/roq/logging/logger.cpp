/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/logging/logger.hpp"

#include <cpptrace/cpptrace.hpp>

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
  cpptrace::generate_trace().print();
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

Logger::Logger(args::Parser const &, logging::Settings const &settings, bool stacktrace) {
  // note! to detach from terminal: use nohup, systemd, etc.
  terminal_color = [&]() -> bool {
    auto terminal = ::isatty(fileno(stdout));
    // terminal color
    if (std::empty(settings.log.color) || settings.log.color == "auto"sv) {
      return terminal != 0;
    } else if (settings.log.color == "always"sv) {
      return true;
    } else if (settings.log.color == "none"sv) {
      return false;
    } else {
      fmt::println(stderr, R"(Unknown color: "{}")"sv, settings.log.color);
      std::exit(EXIT_FAILURE);
    }
  }();
  // verbosity
  verbosity = [&]() -> uint32_t {
    auto tmp = std::getenv("ROQ_v");
    if (tmp != nullptr && std::strlen(tmp) > 0) {
      auto tmp_2 = std::atoi(tmp);
      if (tmp_2 >= 0) {
        return tmp_2;
      }
    }
    return settings.log.verbosity;
  }();
  // stacktrace
  if (stacktrace) {
    install_failure_signal_handler();
  }
}

}  // namespace logging
}  // namespace roq
