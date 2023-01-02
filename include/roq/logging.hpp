/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include "roq/compat.hpp"

#include <fmt/color.h>
#include <fmt/compile.h>
#include <fmt/format.h>

#include <cassert>
#include <chrono>
#include <functional>
#include <string>
#include <string_view>
#include <utility>

#include "roq/format_str.hpp"

namespace roq {

namespace detail {
extern ROQ_PUBLIC thread_local std::string message_buffer;

extern ROQ_PUBLIC int verbosity;
extern ROQ_PUBLIC bool terminal_color;

using sink_type = std::function<void(std::string_view const &)>;

extern ROQ_PUBLIC sink_type INFO;
extern ROQ_PUBLIC sink_type WARNING;
extern ROQ_PUBLIC sink_type ERROR;
extern ROQ_PUBLIC sink_type CRITICAL;
}  // namespace detail

//! Interface to manage the lifetime of the single static logger.
struct ROQ_PUBLIC Logger final {
  //! Config
  struct Config final {
    std::string_view pattern = {};
    std::chrono::nanoseconds flush_freq = {};
    std::string_view path = {};
    size_t max_size = {};
    size_t max_files = {};
    bool rotate_on_open = {};
    std::string_view color = {};
  };

  //! Initialize the logger
  static void initialize(std::string_view const &arg0, Config const &, bool stacktrace = true);

  //! Shutdown the logger
  static void shutdown();
};

}  // namespace roq

// logging interface using source_location and deduction guides
// references:
//   https://stackoverflow.com/a/66380209

namespace roq {
namespace log {

namespace detail {
template <size_t level, typename... Args>
static void helper(roq::detail::sink_type &sink, roq::format_str<Args...> const &fmt, Args &&...args) {
  using namespace fmt::literals;
  auto &message = roq::detail::message_buffer;
#ifndef NDEBUG
  auto capacity = message.capacity();
#endif
  message.clear();  // note! capacity is in reality preserved but it is not guaranteed by the standard
#ifndef NDEBUG
  assert(capacity == message.capacity());
#endif
  fmt::format_to(std::back_inserter(message), "L{} {}:{}] "_cf, level, fmt.file_name_, fmt.line_);
  fmt::vformat_to(std::back_inserter(message), fmt.str_, fmt::make_format_args(std::forward<Args>(args)...));
  sink(message);
}

#ifndef NDEBUG
template <size_t level, typename... Args>
static void helper_debug(roq::detail::sink_type &sink, roq::format_str<Args...> const &fmt, Args &&...args) {
  using namespace fmt::literals;
  auto &message = roq::detail::message_buffer;
  message.clear();  // note! capacity is in reality preserved but it is not guaranteed by the standard
  fmt::format_to(std::back_inserter(message), "L{} {}:{}] DEBUG: "_cf, level, fmt.file_name_, fmt.line_);
  fmt::vformat_to(std::back_inserter(message), fmt.str_, fmt::make_format_args(std::forward<Args>(args)...));
  sink(message);
}
#endif

template <size_t level, typename... Args>
static void helper_system_error(
    roq::detail::sink_type &sink, int error, roq::format_str<Args...> const &fmt, Args &&...args) {
  using namespace fmt::literals;
  auto &message = roq::detail::message_buffer;
  fmt::format_to(
      std::back_inserter(message),
      "L{} {}:{}] {} [{}] "_cf,
      level,
      fmt.file_name_,
      fmt.line_,
      std::strerror(error),
      error);
  fmt::vformat_to(std::back_inserter(message), fmt.str_, fmt::make_format_args(std::forward<Args>(args)...));
  sink(message);
}
}  // namespace detail

// info

template <std::size_t level = 0>
struct info final {
  template <typename... Args>
  constexpr info(format_str<Args...> const &fmt, Args &&...args) {  // NOLINT
    if constexpr (level > 0) {
      if (roq::detail::verbosity < level) [[likely]]
        return;
    }
    detail::helper<level>(roq::detail::INFO, fmt, std::forward<Args>(args)...);
  }
};

// warn

template <std::size_t level = 0>
struct warn final {
  template <typename... Args>
  constexpr warn(format_str<Args...> const &fmt, Args &&...args) {  // NOLINT
    if constexpr (level > 0) {
      if (roq::detail::verbosity < level) [[likely]]
        return;
    }
    detail::helper<level>(roq::detail::WARNING, fmt, std::forward<Args>(args)...);
  }
};

// error

template <std::size_t level = 0>
struct error final {
  template <typename... Args>
  constexpr error(format_str<Args...> const &fmt, Args &&...args) {  // NOLINT
    if constexpr (level > 0) {
      if (roq::detail::verbosity < level) [[likely]]
        return;
    }
    detail::helper<level>(roq::detail::ERROR, fmt, std::forward<Args>(args)...);
  }
};

// critical (will only abort if this is a debug build)

#ifndef NDEBUG
template <typename... Args>
[[noreturn]] constexpr void critical(format_str<Args...> const &fmt, Args &&...args) {  // NOLINT
  detail::helper<0>(roq::detail::CRITICAL, fmt, std::forward<Args>(args)...);
  std::abort();
}
#else
template <typename... Args>
constexpr void critical(format_str<Args...> const &fmt, Args &&...args) {  // NOLINT
  detail::helper<0>(roq::detail::CRITICAL, fmt, std::forward<Args>(args)...);
}
#endif

// fatal (will always abort)

template <typename... Args>
[[noreturn]] constexpr void fatal(format_str<Args...> const &fmt, Args &&...args) {  // NOLINT
  detail::helper<0>(roq::detail::CRITICAL, fmt, std::forward<Args>(args)...);
  std::abort();
}

// debug (no-op unless this is a debug build)

template <std::size_t level = 0>
struct debug final {
#ifndef NDEBUG
  template <typename... Args>
  constexpr debug(format_str<Args...> const &fmt, Args &&...args) {  // NOLINT
    if constexpr (level > 0) {
      if (roq::detail::verbosity < level) [[likely]]
        return;
    }
    detail::helper_debug<level>(roq::detail::INFO, fmt, std::forward<Args>(args)...);
  }
#else
  template <typename... Args>
  constexpr debug(format_str<Args...> const &, Args &&...) {  // NOLINT
  }
#endif
};

// system_error

template <std::size_t level = 0>
struct system_error final {
  template <typename... Args>
  constexpr system_error(format_str<Args...> const &fmt, Args &&...args) {  // NOLINT
    if constexpr (level > 0) {
      if (roq::detail::verbosity < level) [[likely]]
        return;
    }
    static_assert(std::is_same<std::decay<decltype(errno)>::type, int>::value);
    detail::helper_system_error<level>(roq::detail::WARNING, errno, fmt, std::forward<Args>(args)...);
  }
};

// print

struct print final {
  template <typename... Args>
  constexpr print(fmt::text_style text_style, Args &&...args) {
    fmt::print(roq::detail::terminal_color ? text_style : fmt::text_style{}, std::forward<Args>(args)...);
  }
  template <typename... Args>
  constexpr print(fmt::format_string<Args...> const &format_str, Args &&...args) {
    fmt::print(format_str, std::forward<Args>(args)...);
  }
  template <typename... Args>
  constexpr print(std::FILE *file, fmt::format_string<Args...> const &format_str, Args &&...args) {
    fmt::print(file, format_str, std::forward<Args>(args)...);
  }
};

}  // namespace log
}  // namespace roq
