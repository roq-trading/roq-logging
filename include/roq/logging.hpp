/* Copyright (c) 2017-2024, Hans Erik Thrane */

#pragma once

#include <fmt/color.h>
#include <fmt/format.h>

#include <cassert>

#include "roq/format_str.hpp"

#include "roq/logging/handler.hpp"
#include "roq/logging/shared.hpp"

namespace roq {

namespace log {

namespace detail {
template <size_t level, typename... Args>
static void helper(roq::logging::Level log_level, roq::format_str<Args...> const &fmt, Args &&...args) {
  using namespace std::literals;
  auto &message = roq::logging::message_buffer;
#ifndef NDEBUG
  auto capacity = message.capacity();
#endif
  message.clear();  // note! capacity is in reality preserved but it is not guaranteed by the standard
#ifndef NDEBUG
  assert(capacity == message.capacity());
#endif
  fmt::format_to(std::back_inserter(message), "L{} {}:{}] "sv, level, fmt.file_name, fmt.line);
  fmt::vformat_to(std::back_inserter(message), fmt.str, fmt::make_format_args(args...));
  (*roq::logging::Handler::INSTANCE)(log_level, message);
}

#ifndef NDEBUG
template <size_t level, typename... Args>
static void helper_debug(roq::logging::Level log_level, roq::format_str<Args...> const &fmt, Args &&...args) {
  using namespace std::literals;
  auto &message = roq::logging::message_buffer;
  message.clear();  // note! capacity is in reality preserved but it is not guaranteed by the standard
  fmt::format_to(std::back_inserter(message), "L{} {}:{}] DEBUG: "sv, level, fmt.file_name, fmt.line);
  fmt::vformat_to(std::back_inserter(message), fmt.str, fmt::make_format_args(args...));
  (*roq::logging::Handler::INSTANCE)(log_level, message);
}
#endif

template <size_t level, typename... Args>
static void helper_system_error(roq::logging::Level log_level, int error, roq::format_str<Args...> const &fmt, Args &&...args) {
  using namespace std::literals;
  auto &message = roq::logging::message_buffer;
  message.clear();
  fmt::format_to(std::back_inserter(message), "L{} {}:{}] {} [{}] "sv, level, fmt.file_name, fmt.line, std::strerror(error), error);
  fmt::vformat_to(std::back_inserter(message), fmt.str, fmt::make_format_args(args...));
  (*roq::logging::Handler::INSTANCE)(log_level, message);
}
}  // namespace detail

// info

template <std::size_t level = 0>
struct info final {
  template <typename... Args>
  constexpr info(format_str<Args...> const &fmt, Args &&...args) {
    if constexpr (level > 0) {
      if (roq::logging::verbosity < level) [[likely]]
        return;
    }
    detail::helper<level>(roq::logging::Level::INFO, fmt, std::forward<Args>(args)...);
  }
};

// warn

template <std::size_t level = 0>
struct warn final {
  template <typename... Args>
  constexpr warn(format_str<Args...> const &fmt, Args &&...args) {
    if constexpr (level > 0) {
      if (roq::logging::verbosity < level) [[likely]]
        return;
    }
    detail::helper<level>(roq::logging::Level::WARNING, fmt, std::forward<Args>(args)...);
  }
};

// error

template <std::size_t level = 0>
struct error final {
  template <typename... Args>
  constexpr error(format_str<Args...> const &fmt, Args &&...args) {
    if constexpr (level > 0) {
      if (roq::logging::verbosity < level) [[likely]]
        return;
    }
    detail::helper<level>(roq::logging::Level::ERROR, fmt, std::forward<Args>(args)...);
  }
};

// critical (will only abort if this is a debug build)

#ifndef NDEBUG
template <typename... Args>
[[noreturn]] constexpr void critical(format_str<Args...> const &fmt, Args &&...args) {
  detail::helper<0>(roq::logging::Level::CRITICAL, fmt, std::forward<Args>(args)...);
  std::abort();
}
#else
template <typename... Args>
constexpr void critical(format_str<Args...> const &fmt, Args &&...args) {
  detail::helper<0>(roq::logging::Level::CRITICAL, fmt, std::forward<Args>(args)...);
}
#endif

// fatal (will always abort)

template <typename... Args>
[[noreturn]] constexpr void fatal(format_str<Args...> const &fmt, Args &&...args) {
  detail::helper<0>(roq::logging::Level::CRITICAL, fmt, std::forward<Args>(args)...);
  std::abort();
}

// debug (no-op unless this is a debug build)

template <std::size_t level = 0>
struct debug final {
#ifndef NDEBUG
  template <typename... Args>
  constexpr debug(format_str<Args...> const &fmt, Args &&...args) {
    if constexpr (level > 0) {
      if (roq::logging::verbosity < level) [[likely]]
        return;
    }
    detail::helper_debug<level>(roq::logging::Level::DEBUG, fmt, std::forward<Args>(args)...);
  }
#else
  template <typename... Args>
  constexpr debug(format_str<Args...> const &, Args &&...) {}
#endif
};

// system_error

template <std::size_t level = 0>
struct system_error final {
  template <typename... Args>
  constexpr system_error(format_str<Args...> const &fmt, Args &&...args) {
    if constexpr (level > 0) {
      if (roq::logging::verbosity < level) [[likely]]
        return;
    }
    static_assert(std::is_same<std::decay<decltype(errno)>::type, int>::value);
    detail::helper_system_error<level>(roq::logging::Level::WARNING, errno, fmt, std::forward<Args>(args)...);
  }
};

}  // namespace log

struct print final {
  template <typename... Args>
  constexpr print(fmt::text_style text_style, Args &&...args) {
    fmt::print(roq::logging::terminal_color ? text_style : fmt::text_style{}, std::forward<Args>(args)...);
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

}  // namespace roq
