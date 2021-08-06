/* Copyright (c) 2017-2021, Hans Erik Thrane */

#pragma once

#include <fmt/format.h>

#include <cassert>
#include <chrono>
#include <functional>
#include <string_view>
#include <utility>

#include "roq/format_str.h"
#include "roq/source_location.h"

#include "roq/compat.h"
#include "roq/literals.h"

namespace roq {

namespace detail {
extern ROQ_PUBLIC thread_local std::pair<char *, size_t> message_buffer;
extern ROQ_PUBLIC int verbosity;

// sinks
typedef std::function<void(const std::string_view &)> sink_t;
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
    if (ROQ_LIKELY(remain() > 0))
      *(iter_++) = value;
    else
      overflow_ = true;
  }
  void append(const std::string_view &text) { iter_ += text.copy(iter_, remain()); }
  std::string_view finish() {
    if (ROQ_UNLIKELY(overflow_)) {
      assert((end_ - iter_) == 4);
      " ..."_sv.copy(iter_, 4);
    }
    return std::string_view(begin_, size());
  }

 private:
  value_type *iter_;
  const value_type *begin_;
  const value_type *end_;
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
  static void initialize(const std::string_view &arg0, const Config &, bool stacktrace = true);

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
static /*consteval*/ constexpr std::string_view basename(const std::string_view &path) noexcept {
  auto pos = path.find_last_of('/');
  return pos == path.npos ? path : path.substr(++pos);
}

template <typename... Args>
static void helper(
    roq::detail::sink_t &sink, const source_location &loc, const fmt::format_string<Args...> &fmt, Args &&...args) {
  auto &buffer = roq::detail::message_buffer;
  roq::detail::memory_view_t view(buffer.first, buffer.second);
  fmt::format_to(std::back_inserter(view), "{}:{}] "_sv, basename(loc.file_name()), loc.line());
  fmt::format_to(std::back_inserter(view), fmt, std::forward<Args>(args)...);
  sink(view.finish());
}

#if !defined(NDEBUG)
template <typename... Args>
static void helper_debug(
    roq::detail::sink_t &sink, const source_location &loc, const fmt::format_string<Args...> &fmt, Args &&...args) {
  auto &buffer = roq::detail::message_buffer;
  roq::detail::memory_view_t view(buffer.first, buffer.second);
  fmt::format_to(std::back_inserter(view), "{}:{}] DEBUG: "_sv, basename(loc.file_name()), loc.line());
  fmt::format_to(std::back_inserter(view), fmt, std::forward<Args>(args)...);
  sink(view.finish());
}
#endif

template <typename... Args>
static void helper_system_error(
    roq::detail::sink_t &sink,
    const source_location &loc,
    const fmt::format_string<Args...> &fmt,
    int error,
    Args &&...args) {
  auto &buffer = roq::detail::message_buffer;
  roq::detail::memory_view_t view(buffer.first, buffer.second);
  fmt::format_to(
      std::back_inserter(view),
      "{}:{}] {} [{}] "_sv,
      basename(loc.file_name()),
      loc.line(),
      std::strerror(error),
      error);
  fmt::format_to(std::back_inserter(view), fmt, std::forward<Args>(args)...);
  sink(view.finish());
}
}  // namespace detail

// info

template <std::size_t level = 0, typename... Args>
struct info {
  constexpr info(
      const fmt::format_string<Args...> &fmt, Args &&...args, const source_location &loc = source_location::current()) {
    if constexpr (level > 0) {
      if (ROQ_LIKELY(roq::detail::verbosity < level))
        return;
    }
    detail::helper(roq::detail::INFO, loc, fmt, std::forward<Args>(args)...);
  }
};

template <std::size_t level = 0, typename... Args>
info(const fmt::format_string<Args...> &, Args &&...) -> info<level, Args...>;

// warn

template <std::size_t level = 0, typename... Args>
struct warn {
  constexpr warn(
      const fmt::format_string<Args...> &fmt, Args &&...args, const source_location &loc = source_location::current()) {
    if constexpr (level > 0) {
      if (ROQ_LIKELY(roq::detail::verbosity < level))
        return;
    }
    detail::helper(roq::detail::WARNING, loc, fmt, std::forward<Args>(args)...);
  }
};

template <std::size_t level = 0, typename... Args>
warn(const fmt::format_string<Args...> &, Args &&...) -> warn<level, Args...>;

// error

template <std::size_t level = 0, typename... Args>
struct error {
  constexpr error(
      const fmt::format_string<Args...> &fmt, Args &&...args, const source_location &loc = source_location::current()) {
    if constexpr (level > 0) {
      if (ROQ_LIKELY(roq::detail::verbosity < level))
        return;
    }
    detail::helper(roq::detail::ERROR, loc, fmt, std::forward<Args>(args)...);
  }
};

template <std::size_t level = 0, typename... Args>
error(const fmt::format_string<Args...> &, Args &&...) -> error<level, Args...>;

// critical (will only abort if this is a debug build)

template <typename... Args>
struct critical {
  [[noreturn]] constexpr critical(
      const fmt::format_string<Args...> &fmt, Args &&...args, const source_location &loc = source_location::current()) {
    detail::helper(roq::detail::CRITICAL, loc, fmt, std::forward<Args>(args)...);
#if !defined(NDEBUG)
    std::abort();
#endif
  }
};

template <typename... Args>
critical(const fmt::format_string<Args...> &, Args &&...) -> critical<Args...>;

// fatal (will always abort)

template <typename... Args>
struct fatal {
  [[noreturn]] constexpr fatal(
      const fmt::format_string<Args...> &fmt, Args &&...args, const source_location &loc = source_location::current()) {
    detail::helper(roq::detail::CRITICAL, loc, fmt, std::forward<Args>(args)...);
    std::abort();
  }
};

template <typename... Args>
fatal(const fmt::format_string<Args...> &, Args &&...) -> fatal<Args...>;

// debug (no-op unless this is a debug build)

template <std::size_t level = 0, typename... Args>
struct debug {
  constexpr debug(
      const fmt::format_string<Args...> &fmt, Args &&...args, const source_location &loc = source_location::current()) {
#if !defined(NDEBUG)
    if constexpr (level > 0) {
      if (ROQ_LIKELY(roq::detail::verbosity < level))
        return;
    }
    detail::helper_debug(roq::detail::INFO, loc, fmt, std::forward<Args>(args)...);
#endif
  }
};
// system_error

template <std::size_t level = 0, typename... Args>
struct system_error {
  constexpr system_error(
      const fmt::format_string<Args...> &fmt, Args &&...args, const source_location &loc = source_location::current()) {
    if constexpr (level > 0) {
      if (ROQ_LIKELY(roq::detail::verbosity < level))
        return;
    }
    static_assert(std::is_same<std::decay<decltype(errno)>::type, int>::value);
    detail::helper_system_error(roq::detail::WARNING, loc, fmt, errno, std::forward<Args>(args)...);
  }
};

}  // namespace log
}  // namespace roq
