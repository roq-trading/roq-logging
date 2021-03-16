/* Copyright (c) 2017-2021, Hans Erik Thrane */

#pragma once

#include <fmt/format.h>

#include "roq/logging.h"
#include "roq/source_location.h"

// !!! EXPERIMENTAL !!! DO NOT USE !!!
//
// logging interface using source_location and deduction guide
// references:
//   https://stackoverflow.com/a/57548488
//   https://en.cppreference.com/w/cpp/language/class_template_argument_deduction

namespace roq {
namespace log {

namespace detail {
static /*consteval*/ constexpr std::string_view basename(const std::string_view &path) noexcept {
  auto pos = path.find_last_of('/');
  return pos == path.npos ? path : path.substr(++pos);
}

template <typename... Args>
static void helper(const source_location &loc, Args &&...args) {
  /*consteval*/ auto file = basename(loc.file_name());
  auto text_1 = fmt::format("{}:{}]", file, loc.line());
  auto text_2 = fmt::format(std::forward<Args>(args)...);
  DLOG(INFO)(R"({} {})"_fmt, text_1, text_2);
}
}  // namespace detail

template <typename... Args>
struct debug {
  constexpr debug(Args &&...args, const source_location &loc = source_location::current()) {
    detail::helper(loc, std::forward<Args>(args)...);
  }
};

template <typename... Args>
debug(Args &&...) -> debug<Args...>;

}  // namespace log
}  // namespace roq
