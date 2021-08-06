/* Copyright (c) 2017-2021, Hans Erik Thrane */

#pragma once

#include <fmt/format.h>

#include <string_view>
#include <tuple>

#include "roq/source_location.h"

namespace roq {

template <typename... Args>
struct format_str final {
  constexpr format_str(const fmt::format_string<Args...> &fmt, const source_location &loc = source_location::current())
      : fmt_(fmt), loc_(loc) {}

  constexpr format_str(format_str &&) = default;
  constexpr format_str(const format_str &) = delete;

  constexpr operator const fmt::format_string<Args...> &() const { return fmt_; }
  constexpr operator const source_location &() const { return loc_; }

 private:
  const fmt::format_string<Args...> fmt_;
  const source_location loc_;
};

template <typename... Args>
struct format_str_2 final {
  constexpr format_str_2(Args &&...args, const source_location &loc = source_location::current())
      : args_(std::forward<Args>(args)...), loc_(loc) {}

  constexpr format_str_2(format_str_2 &&) = default;
  constexpr format_str_2(const format_str_2 &) = delete;

  template <typename Callback, typename Level>
  void dispatch(Callback callback, Level level) {
    std::apply(callback, level, loc_, args_);
  }

 private:
  std::tuple<Args...> args_;
  const source_location loc_;
};

// template <typename... Args>
// format_str_2(Args &&...) -> format_str_2<Args...>;

}  // namespace roq
