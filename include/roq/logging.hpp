/* Copyright (c) 2017-2022, Hans Erik Thrane */

#pragma once

#include <fmt/format.h>

#include <cassert>
#include <chrono>
#include <functional>
#include <string_view>
#include <utility>

#include "roq/format_str.hpp"
#include "roq/source_location.hpp"

#include "roq/compat.hpp"

namespace roq {

namespace detail {
extern ROQ_PUBLIC thread_local std::pair<char *, size_t> message_buffer;
extern ROQ_PUBLIC int verbosity;

// sinks
typedef std::function<void(std::string_view const &)> sink_t;
extern ROQ_PUBLIC sink_t INFO;
extern ROQ_PUBLIC sink_t WARNING;
extern ROQ_PUBLIC sink_t ERROR;
extern ROQ_PUBLIC sink_t CRITICAL;

// memory_view to support std::back_inserter
template <typename T>
class ROQ_PUBLIC basic_memory_view_t final {
 public:
  using value_type = T;
  basic_memory_view_t(value_type *buffer, size_t length) : iter_(buffer), begin_(buffer), end_(buffer + length) {
    assert(length > 4);
  }
  size_t size() const { return iter_ - begin_; }
  size_t remain() const { return end_ - iter_ - 4; }
  void push_back(char value) {
    if (remain() > 0) [[likely]] {
      *(iter_++) = value;
    } else {
      overflow_ = true;
    }
  }
  void append(std::string_view const &text) { iter_ += text.copy(iter_, remain()); }
  std::string_view finish() {
    using namespace std::literals;
    if (overflow_) [[unlikely]] {
      assert((end_ - iter_) == 4);
      " ..."sv.copy(iter_, 4);
    }
    return std::string_view(begin_, size());
  }

 private:
  value_type *iter_;
  value_type const *begin_;
  value_type const *end_;
  bool overflow_ = false;
};

using memory_view_t = basic_memory_view_t<char>;

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
static void helper(roq::detail::sink_t &sink, roq::format_str<Args...> const &fmt, Args &&...args) {
  using namespace std::literals;
  auto &buffer = roq::detail::message_buffer;
  roq::detail::memory_view_t view(buffer.first, buffer.second);
  fmt::format_to(std::back_inserter(view), "L{} {}:{}] "sv, level, fmt.file_name_, fmt.line_);
  fmt::vformat_to(std::back_inserter(view), fmt.str_, fmt::make_format_args(std::forward<Args>(args)...));
  sink(view.finish());
}

#ifndef NDEBUG
template <size_t level, typename... Args>
static void helper_debug(roq::detail::sink_t &sink, roq::format_str<Args...> const &fmt, Args &&...args) {
  using namespace std::literals;
  auto &buffer = roq::detail::message_buffer;
  roq::detail::memory_view_t view(buffer.first, buffer.second);
  fmt::format_to(std::back_inserter(view), "L{} {}:{}] DEBUG: "sv, level, fmt.file_name_, fmt.line_);
  fmt::vformat_to(std::back_inserter(view), fmt.str_, fmt::make_format_args(std::forward<Args>(args)...));
  sink(view.finish());
}
#endif

template <size_t level, typename... Args>
static void helper_system_error(
    roq::detail::sink_t &sink, int error, roq::format_str<Args...> const &fmt, Args &&...args) {
  using namespace std::literals;
  auto &buffer = roq::detail::message_buffer;
  roq::detail::memory_view_t view(buffer.first, buffer.second);
  fmt::format_to(
      std::back_inserter(view), "L{} {}:{}] {} [{}] "sv, level, fmt.file_name_, fmt.line_, std::strerror(error), error);
  fmt::vformat_to(std::back_inserter(view), fmt.str_, fmt::make_format_args(std::forward<Args>(args)...));
  sink(view.finish());
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
  template <typename... Args>
  constexpr debug(format_str<Args...> const &fmt, Args &&...args) {  // NOLINT
#ifndef NDEBUG
    if constexpr (level > 0) {
      if (roq::detail::verbosity < level) [[likely]]
        return;
    }
    detail::helper_debug<level>(roq::detail::INFO, fmt, std::forward<Args>(args)...);
#endif
  }
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

}  // namespace log
}  // namespace roq
