/* Copyright (c) 2017-2021, Hans Erik Thrane */

#pragma once

// objectives:
// - require explicit identification of format strings
// - (eventually) make compile-time validation of format strings possible

#include <string_view>

#include "roq/source_location.h"

namespace roq {

struct format_str final {
  template <typename T>
  constexpr format_str(const T &fmt, const source_location &loc = source_location::current()) : fmt_(fmt), loc_(loc) {
    // note! this check ensures T doesn't require run-time computation of length
    static_assert(
        std::is_same<T, std::string_view>::value || std::is_same<T, std::string>::value,
        "you should use string_view (for performance reasons)");
  }

  constexpr format_str(format_str &&) = default;
  constexpr format_str(const format_str &) = delete;

  constexpr operator const std::string_view &() const { return fmt_; }
  constexpr operator const source_location &() const { return loc_; }

 private:
  const std::string_view fmt_;
  const source_location loc_;
};

}  // namespace roq
