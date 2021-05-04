/* Copyright (c) 2017-2021, Hans Erik Thrane */

#pragma once

#include <cassert>
#include <string_view>
#include <utility>

#include "roq/source_location.h"

#include "roq/compat.h"
#include "roq/format.h"
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
  basic_memory_view_t(value_type *buffer, size_t length)
      : iter_(buffer), begin_(buffer), end_(buffer + length) {
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
  //! Initialize the logger
  static void initialize(
      const std::string_view &arg0,
      const std::string_view &pattern = std::string_view(),
      bool stacktrace = true);

  //! Shutdown the logger
  static void shutdown();
};

}  // namespace roq

// logging interface using source_location and deduction guides
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
static void helper(roq::detail::sink_t &sink, const source_location &loc, Args &&...args) {
  auto &buffer = roq::detail::message_buffer;
  roq::detail::memory_view_t view(buffer.first, buffer.second);
  roq::format_to(std::back_inserter(view), "{}:{}] "_fmt, basename(loc.file_name()), loc.line());
  roq::format_to(std::back_inserter(view), std::forward<Args>(args)...);
  sink(view.finish());
}

template <typename... Args>
static void helper_system_error(
    roq::detail::sink_t &sink, const source_location &loc, int error, Args &&...args) {
  auto &buffer = roq::detail::message_buffer;
  roq::detail::memory_view_t view(buffer.first, buffer.second);
  roq::format_to(
      std::back_inserter(view),
      "{}:{}] {} [{}] "_fmt,
      basename(loc.file_name()),
      loc.line(),
      std::strerror(error),
      error);
  roq::format_to(std::back_inserter(view), std::forward<Args>(args)...);
  sink(view.finish());
}
}  // namespace detail

// info

template <typename... Args>
struct info {
  constexpr info(
      Args &&...args, const source_location &loc = source_location::current()) {  // NOLINT
    detail::helper(roq::detail::INFO, loc, std::forward<Args>(args)...);
  }
};

template <typename... Args>
info(Args &&...) -> info<Args...>;

// warn

template <typename... Args>
struct warn {
  constexpr warn(
      Args &&...args, const source_location &loc = source_location::current()) {  // NOLINT
    detail::helper(roq::detail::WARNING, loc, std::forward<Args>(args)...);
  }
};

template <typename... Args>
warn(Args &&...) -> warn<Args...>;

// error

template <typename... Args>
struct error {
  constexpr error(
      Args &&...args, const source_location &loc = source_location::current()) {  // NOLINT
    detail::helper(roq::detail::ERROR, loc, std::forward<Args>(args)...);
  }
};

template <typename... Args>
error(Args &&...) -> error<Args...>;

// fatal

template <typename... Args>
struct fatal {
  [[noreturn]] constexpr fatal(
      Args &&...args, const source_location &loc = source_location::current()) {  // NOLINT
    detail::helper(roq::detail::CRITICAL, loc, std::forward<Args>(args)...);
    std::abort();
  }
};

template <typename... Args>
fatal(Args &&...) -> fatal<Args...>;

// debug (only for debug build)

template <typename... Args>
struct debug {
  constexpr debug(
      Args &&...args, const source_location &loc = source_location::current()) {  // NOLINT
#if !defined(NDEBUG)
    detail::helper(roq::detail::INFO, loc, std::forward<Args>(args)...);
#endif
  }
};

template <typename... Args>
debug(Args &&...) -> debug<Args...>;

// trace_1 (only for verbosity >= 1)

template <typename... Args>
struct trace_1 {
  constexpr trace_1(
      Args &&...args, const source_location &loc = source_location::current()) {  // NOLINT
    if (ROQ_UNLIKELY(roq::detail::verbosity >= 1))
      detail::helper(roq::detail::INFO, loc, std::forward<Args>(args)...);
  }
};

template <typename... Args>
trace_1(Args &&...) -> trace_1<Args...>;

// trace_2 (only for verbosity >= 2)

template <typename... Args>
struct trace_2 {
  constexpr trace_2(
      Args &&...args, const source_location &loc = source_location::current()) {  // NOLINT
    if (ROQ_UNLIKELY(roq::detail::verbosity >= 2))
      detail::helper(roq::detail::INFO, loc, std::forward<Args>(args)...);
  }
};

template <typename... Args>
trace_2(Args &&...) -> trace_2<Args...>;

// trace_3 (only for verbosity >= 3)

template <typename... Args>
struct trace_3 {
  constexpr trace_3(
      Args &&...args, const source_location &loc = source_location::current()) {  // NOLINT
    if (ROQ_UNLIKELY(roq::detail::verbosity >= 3))
      detail::helper(roq::detail::INFO, loc, std::forward<Args>(args)...);
  }
};

template <typename... Args>
trace_3(Args &&...) -> trace_3<Args...>;

// trace_4 (only for verbosity >= 4)

template <typename... Args>
struct trace_4 {
  constexpr trace_4(
      Args &&...args, const source_location &loc = source_location::current()) {  // NOLINT
    if (ROQ_UNLIKELY(roq::detail::verbosity >= 4))
      detail::helper(roq::detail::INFO, loc, std::forward<Args>(args)...);
  }
};

template <typename... Args>
trace_4(Args &&...) -> trace_4<Args...>;

// trace_5 (only for verbosity >= 5)

template <typename... Args>
struct trace_5 {
  constexpr trace_5(
      Args &&...args, const source_location &loc = source_location::current()) {  // NOLINT
    if (ROQ_UNLIKELY(roq::detail::verbosity >= 5))
      detail::helper(roq::detail::INFO, loc, std::forward<Args>(args)...);
  }
};

template <typename... Args>
trace_5(Args &&...) -> trace_5<Args...>;

// system_error

template <typename... Args>
struct system_error {
  constexpr system_error(
      Args &&...args, const source_location &loc = source_location::current()) {  // NOLINT
    static_assert(std::is_same<std::decay<decltype(errno)>::type, int>::value);
    detail::helper_system_error(roq::detail::WARNING, loc, errno, std::forward<Args>(args)...);
  }
};

template <typename... Args>
system_error(Args &&...) -> system_error<Args...>;

}  // namespace log
}  // namespace roq
