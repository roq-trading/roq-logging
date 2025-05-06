/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/logging/factory.hpp"

#include "roq/exceptions.hpp"

#include "roq/logging/spdlog/logger.hpp"

#include "roq/logging/standard/logger.hpp"

using namespace std::literals;

namespace roq {
namespace logging {

// === IMPLEMENTATION ===

std::unique_ptr<Handler> Factory::create(std::string_view const &type, Settings const &settings) {
  if (std::empty(type) || type == "std"sv || type == "standard"sv) {
    return std::make_unique<standard::Logger>(settings);
  }
  if (type == "spdlog"sv) {
    return std::make_unique<spdlog::Logger>(settings);
  }
  throw RuntimeError{R"(Unknown logging type: "{}")"sv, type};
}

}  // namespace logging
}  // namespace roq
