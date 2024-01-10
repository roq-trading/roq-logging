/* Copyright (c) 2017-2024, Hans Erik Thrane */

#pragma once

#include "roq/compat.hpp"

#include <fmt/chrono.h>
#include <fmt/compile.h>
#include <fmt/format.h>

#include <chrono>
#include <string_view>

namespace roq {
namespace logging {

namespace detail {
struct Log final {
  std::string_view pattern;
  std::chrono::nanoseconds flush_freq = {};
  std::string_view path;
  uint32_t max_size = {};
  uint32_t max_files = {};
  bool rotate_on_open = {};
  std::string_view color;
};
}  // namespace detail

struct ROQ_PUBLIC Settings {
  detail::Log log;
};

}  // namespace logging
}  // namespace roq

template <>
struct fmt::formatter<roq::logging::detail::Log> {
  constexpr auto parse(format_parse_context &context) { return std::begin(context); }
  auto format(roq::logging::detail::Log const &value, format_context &context) const {
    using namespace std::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(pattern="{}", )"
        R"(flush_freq={}, )"
        R"(path="{}", )"
        R"(max_size={}, )"
        R"(max_files={}, )"
        R"(rotate_on_open={}, )"
        R"(color="{}")"
        R"(}})"sv,
        value.pattern,
        value.flush_freq,
        value.path,
        value.max_size,
        value.max_files,
        value.rotate_on_open,
        value.color);
  }
};

template <>
struct fmt::formatter<roq::logging::Settings> {
  constexpr auto parse(format_parse_context &context) { return std::begin(context); }
  auto format(roq::logging::Settings const &value, format_context &context) const {
    using namespace std::literals;
    return fmt::format_to(
        context.out(),
        R"({{)"
        R"(log={})"
        R"(}})"sv,
        value.log);
  }
};
