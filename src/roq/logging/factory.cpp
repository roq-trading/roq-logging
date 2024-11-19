/* Copyright (c) 2017-2024, Hans Erik Thrane */

#include "roq/logging/factory.hpp"

#include "roq/exceptions.hpp"

#include "roq/logging/spdlog/logger.hpp"

#include "roq/logging/standard/logger.hpp"

using namespace std::literals;

namespace roq {
namespace logging {

// === IMPLEMENTATION ===

std::unique_ptr<Handler> Factory::create(std::string_view const &type, Settings const &settings) {
  if (std::empty(type) || type.compare("std"sv) == 0 || type.compare("standard"sv) == 0)
    return std::make_unique<standard::Logger>(settings);
  if (type.compare("spdlog"sv) == 0)
    return std::make_unique<spdlog::Logger>(settings);
  throw RuntimeError{R"(Unknown logging type: "{}")"sv, type};
}

}  // namespace logging
}  // namespace roq
