/* Copyright (c) 2017-2024, Hans Erik Thrane */

#include "roq/logging/standard/logger.hpp"

#include <iostream>

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
      std::cout << message << std::endl;
      break;
    case ERROR:
    case CRITICAL:
      std::cout << message << std::endl;
      break;
  }
}

}  // namespace standard
}  // namespace logging
}  // namespace roq
