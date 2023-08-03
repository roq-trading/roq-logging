/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/logging/factory.hpp"

#include "roq/exceptions.hpp"

#include "roq/utils/compare.hpp"

#include "roq/logging/spdlog/logger.hpp"

#include "roq/logging/standard/logger.hpp"

using namespace std::literals;

namespace roq {
namespace logging {

// === IMPLEMENTATION ===

std::unique_ptr<Handler> Factory::create(std::string_view const &type, Settings const &settings) {
  if (std::empty(type) || utils::case_insensitive_compare(type, "std"sv) == 0 ||
      utils::case_insensitive_compare(type, "standard"sv) == 0)
    return std::make_unique<standard::Logger>(settings);
  if (utils::case_insensitive_compare(type, "spdlog"sv) == 0)
    return std::make_unique<spdlog::Logger>(settings);
  throw RuntimeError{R"(Unknown logging type: "{}")"sv, type};
}

}  // namespace logging
}  // namespace roq
