/* Copyright (c) 2017-2025, Hans Erik Thrane */

#define USE_UNWIND

#include "roq/logging/handler.hpp"

#include <iostream>

#include "roq/exceptions.hpp"

#include "roq/logging/shared.hpp"

#include "roq/logging/standard/logger.hpp"

using namespace std::literals;

namespace roq {
namespace logging {

// === EXTERN ===

Handler *Handler::INSTANCE = nullptr;

// === HELPERS ===

namespace {
struct Default final : public Handler {
  void operator()(Level level, std::string_view const &message) override {
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
};
}  // namespace

// === CONSTANTS ===

namespace {
size_t COUNT = {};
Default DEFAULT_LOGGER;
}  // namespace

// === IMPLEMENTATION ===

Handler::Handler() {
  if (COUNT >= 2) {
    throw RuntimeError{"Logger is singleton"sv};
  }
  ++COUNT;
  INSTANCE = this;
}

Handler::~Handler() {
  switch (COUNT) {
    case 0:
      assert(false);
      return;
    case 1:
      INSTANCE = nullptr;
      break;
    case 2:
      INSTANCE = &DEFAULT_LOGGER;
      break;
    default:
      assert(false);
      break;
  }
  --COUNT;
}

}  // namespace logging
}  // namespace roq
