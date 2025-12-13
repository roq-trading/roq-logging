/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/logging/standard/logger.hpp"

#include <fmt/format.h>

#include "roq/logging/shared.hpp"

using namespace std::literals;

namespace roq {
namespace logging {
namespace standard {

// === IMPLEMENTATION ===

Logger::Logger(Settings const &) {
}

void Logger::operator()(Level level, std::string_view const &message) {
  switch (level) {
    using enum Level;
    case DEBUG:
    case INFO:
    case WARNING:
      fmt::println("{}"sv, message);
      break;
    case ERROR:
    case CRITICAL:
      fmt::println("{}"sv, message);
      break;
  }
}

}  // namespace standard
}  // namespace logging
}  // namespace roq
