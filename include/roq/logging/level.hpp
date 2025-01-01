/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <fmt/core.h>

#include <magic_enum/magic_enum_format.hpp>

namespace roq {
namespace logging {

enum class Level {
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  CRITICAL,
};

}  // namespace logging
}  // namespace roq

template <>
struct fmt::formatter<roq::logging::Level> {
  constexpr auto parse(format_parse_context &context) { return std::begin(context); }
  auto format(roq::logging::Level const &value, format_context &context) const {
    using namespace std::literals;
    return fmt::format_to(context.out(), "{}"sv, value);
  }
};
