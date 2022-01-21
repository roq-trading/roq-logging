/* Copyright (c) 2017-2022, Hans Erik Thrane */

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
    if (remain() > 0) [[likely]] {
      *(iter_++) = value;
    } else {
      overflow_ = true;
    }
  }
  void append(const std::string_view &text) { iter_ += text.copy(iter_, remain()); }
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
template <typename... Args>
static void helper(roq::detail::sink_t &sink, const roq::format_str<Args...> &fmt, Args &&...args) {
  using namespace std::literals;
  auto &buffer = roq::detail::message_buffer;
  roq::detail::memory_view_t view(buffer.first, buffer.second);
  fmt::format_to(std::back_inserter(view), "{}:{}] "sv, fmt.file_name_, fmt.line_);
  fmt::vformat_to(std::back_inserter(view), fmt.str_, fmt::make_format_args(std::forward<Args>(args)...));
  sink(view.finish());
}

#if !defined(NDEBUG)
template <typename... Args>
static void helper_debug(roq::detail::sink_t &sink, const roq::format_str<Args...> &fmt, Args &&...args) {
  using namespace std::literals;
  auto &buffer = roq::detail::message_buffer;
  roq::detail::memory_view_t view(buffer.first, buffer.second);
  fmt::format_to(std::back_inserter(view), "{}:{}] DEBUG: "sv, fmt.file_name_, fmt.line_);
  fmt::vformat_to(std::back_inserter(view), fmt.str_, fmt::make_format_args(std::forward<Args>(args)...));
  sink(view.finish());
}
#endif

template <typename... Args>
static void helper_system_error(
    roq::detail::sink_t &sink, int error, const roq::format_str<Args...> &fmt, Args &&...args) {
  using namespace std::literals;
  auto &buffer = roq::detail::message_buffer;
  roq::detail::memory_view_t view(buffer.first, buffer.second);
  fmt::format_to(std::back_inserter(view), "{}:{}] {} [{}] "sv, fmt.file_name_, fmt.line_, std::strerror(error), error);
  fmt::vformat_to(std::back_inserter(view), fmt.str_, fmt::make_format_args(std::forward<Args>(args)...));
  sink(view.finish());
}
}  // namespace detail

// info

template <std::size_t level = 0>
struct info final {
  template <typename... Args>
  constexpr info(const format_str<Args...> &fmt, Args &&...args) {  // NOLINT
    if constexpr (level > 0) {
      if (roq::detail::verbosity < level) [[likely]]
        return;
    }
    detail::helper(roq::detail::INFO, fmt, std::forward<Args>(args)...);
  }

  // note! experimental (conditional logging currently requires explicit level)
  template <typename... Args>
  struct when final {
    constexpr when(bool condition, const format_str<Args...> &fmt, Args &&...args) {
      if (condition) [[unlikely]]
        info<level>(fmt, std::forward<Args>(args)...);
    }
  };
};

// warn

template <std::size_t level = 0>
struct warn final {
  template <typename... Args>
  constexpr warn(const format_str<Args...> &fmt, Args &&...args) {  // NOLINT
    if constexpr (level > 0) {
      if (roq::detail::verbosity < level) [[likely]]
        return;
    }
    detail::helper(roq::detail::WARNING, fmt, std::forward<Args>(args)...);
  }

  // note! experimental (conditional logging currently requires explicit level)
  template <typename... Args>
  struct when final {
    constexpr when(bool condition, const format_str<Args...> &fmt, Args &&...args) {
      if (condition) [[unlikely]]
        warn<level>(fmt, std::forward<Args>(args)...);
    }
  };
};

// error

template <std::size_t level = 0>
struct error final {
  template <typename... Args>
  constexpr error(const format_str<Args...> &fmt, Args &&...args) {  // NOLINT
    if constexpr (level > 0) {
      if (roq::detail::verbosity < level) [[likely]]
        return;
    }
    detail::helper(roq::detail::ERROR, fmt, std::forward<Args>(args)...);
  }

  // note! experimental (conditional logging currently requires explicit level)
  template <typename... Args>
  struct when final {
    constexpr when(bool condition, const format_str<Args...> &fmt, Args &&...args) {
      if (condition) [[unlikely]]
        error<level>(fmt, std::forward<Args>(args)...);
    }
  };
};

// critical (will only abort if this is a debug build)

struct critical final {
  template <typename... Args>
  [[noreturn]] constexpr critical(const format_str<Args...> &fmt, Args &&...args) {  // NOLINT
    detail::helper(roq::detail::CRITICAL, fmt, std::forward<Args>(args)...);
#if !defined(NDEBUG)
    std::abort();
#endif
  }

  // note! experimental
  template <typename... Args>
  struct when final {
    constexpr when(bool condition, const format_str<Args...> &fmt, Args &&...args) {
      if (condition) [[unlikely]]
        critical(fmt, std::forward<Args>(args)...);
    }
  };
};

// fatal (will always abort)

struct fatal final {
  template <typename... Args>
  [[noreturn]] constexpr fatal(const format_str<Args...> &fmt, Args &&...args) {  // NOLINT
    detail::helper(roq::detail::CRITICAL, fmt, std::forward<Args>(args)...);
    std::abort();
  }

  // note! experimental
  template <typename... Args>
  struct when final {
    constexpr when(bool condition, const format_str<Args...> &fmt, Args &&...args) {
      if (condition) [[unlikely]]
        fatal(fmt, std::forward<Args>(args)...);
    }
  };
};

// debug (no-op unless this is a debug build)

template <std::size_t level = 0>
struct debug final {
  template <typename... Args>
  constexpr debug(const format_str<Args...> &fmt, Args &&...args) {  // NOLINT
#if !defined(NDEBUG)
    if constexpr (level > 0) {
      if (roq::detail::verbosity < level) [[likely]]
        return;
    }
    detail::helper_debug(roq::detail::INFO, fmt, std::forward<Args>(args)...);
#endif
  }

  // note! experimental (conditional logging currently requires explicit level)
  template <typename... Args>
  struct when final {
    constexpr when(bool condition, const format_str<Args...> &fmt, Args &&...args) {
      if (condition) [[unlikely]]
        debug<level>(fmt, std::forward<Args>(args)...);
    }
  };
};

// system_error

template <std::size_t level = 0>
struct system_error final {
  template <typename... Args>
  constexpr system_error(const format_str<Args...> &fmt, Args &&...args) {  // NOLINT
    if constexpr (level > 0) {
      if (roq::detail::verbosity < level) [[likely]]
        return;
    }
    static_assert(std::is_same<std::decay<decltype(errno)>::type, int>::value);
    detail::helper_system_error(roq::detail::WARNING, errno, fmt, std::forward<Args>(args)...);
  }

  // note! experimental (conditional logging currently requires explicit level)
  template <typename... Args>
  struct when final {
    constexpr when(bool condition, const format_str<Args...> &fmt, Args &&...args) {
      if (condition) [[unlikely]]
        system_error<level>(fmt, std::forward<Args>(args)...);
    }
  };
};

}  // namespace log
}  // namespace roq
