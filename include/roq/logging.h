/* Copyright (c) 2017-2020, Hans Erik Thrane */

#pragma once

#if defined(__GNUC__) || defined(__clang__)
#define ROQ_LOGGING_PUBLIC __attribute__((visibility("default")))
#else
#define ROQ_LOGGING_PUBLIC
#endif

#include <fmt/format.h>

#include <functional>
#include <string_view>
#include <utility>

#include "roq/static.h"

namespace roq {

namespace detail {
extern ROQ_LOGGING_PUBLIC thread_local std::pair<char *, size_t> message_buffer;
extern ROQ_LOGGING_PUBLIC bool append_newline;
extern ROQ_LOGGING_PUBLIC int verbosity;
// sinks
typedef std::function<void(const std::string_view&)> sink_t;
extern ROQ_LOGGING_PUBLIC sink_t info;
extern ROQ_LOGGING_PUBLIC sink_t warning;
extern ROQ_LOGGING_PUBLIC sink_t error;
extern ROQ_LOGGING_PUBLIC sink_t critical;
// support std::back_inserter
template <typename T>
class ROQ_LOGGING_PUBLIC basic_memory_view_t final {
 public:
  using value_type = T;
  inline basic_memory_view_t(value_type *buffer, size_t length)
      : _iter(buffer),
        _begin(buffer),
        _end(buffer + length) {
  }
  inline operator std::string_view() const {
    return std::string_view(_begin, size());
  }
  inline size_t size() const {
    return _iter - _begin;
  }
  inline size_t remain() const {
    return _end - _iter;
  }
  inline void push_back(char value) {
    if (_iter < _end)
      *(_iter++) = value;
    // note! silently drop if the buffer is full
  }
  inline void append(const std::string_view& text) {
    _iter += text.copy(_iter, remain());
  }

 private:
  value_type *_iter;
  const value_type *_begin;
  const value_type *_end;
};

using memory_view_t = basic_memory_view_t<char>;

class ROQ_LOGGING_PUBLIC LogMessage final {
 public:
  inline LogMessage(sink_t& sink, const std::string_view& prefix)
      : _sink(sink),
        _memory_view(message_buffer.first, message_buffer.second) {
    _memory_view.append(prefix);
  }

  LogMessage(const LogMessage&) = delete;
  LogMessage(LogMessage&&) = delete;

  inline ~LogMessage() {
    try {
      _sink(_memory_view);
    } catch (...) {
    }
  }
  inline void operator()(const std::string_view& format) {
    _memory_view.append(format);
  }
  template <typename... Args>
  inline void operator()(
      const std::string_view& format,
      Args&&... args) {
    fmt::format_to(
        std::back_inserter(_memory_view),
        format,
        std::forward<Args>(args)...);
  }

 private:
  sink_t& _sink;
  memory_view_t _memory_view;
};

class ROQ_LOGGING_PUBLIC ErrnoLogMessage final {
 public:
  inline ErrnoLogMessage(sink_t& sink, const std::string_view& prefix)
      : _sink(sink),
        _memory_view(message_buffer.first, message_buffer.second),
        _errnum(errno) {
    _memory_view.append(prefix);
  }

  ErrnoLogMessage(const ErrnoLogMessage&) = delete;
  ErrnoLogMessage(ErrnoLogMessage&&) = delete;

  inline ~ErrnoLogMessage() {
    try {
      fmt::format_to(
          std::back_inserter(_memory_view),
          R"(: {} [{}])",
          std::strerror(_errnum),
          _errnum);
      _memory_view.push_back('\0');
      _sink(_memory_view);
    } catch (...) {
    }
  }
  inline void operator()(const std::string_view& format) {
    _memory_view.append(format);
  }
  template <typename... Args>
  inline void operator()(
      const std::string_view& format,
      Args&&... args) {
    fmt::format_to(
        std::back_inserter(_memory_view),
        format,
        std::forward<Args>(args)...);
  }

 private:
  sink_t& _sink;
  memory_view_t _memory_view;
  int _errnum;
};

class ROQ_LOGGING_PUBLIC NullLogMessage final {
 public:
  inline NullLogMessage(sink_t&, const std::string_view&) {
  }

  NullLogMessage(const NullLogMessage&) = delete;
  NullLogMessage(NullLogMessage&&) = delete;

  inline void operator()(const std::string_view&) {
  }
  template <typename... Args>
  inline void operator()(
      const std::string_view& format,
      Args&&... args) {
  }
};

}  // namespace detail

//! Interface to manage the lifetime of the single static logger.
struct ROQ_LOGGING_PUBLIC Logger final {
  //! Initialize the logger
  static void initialize(
      const std::string_view& arg0,
      const std::string_view& pattern = std::string_view(),
      bool stacktrace = true);

  //! Shutdown the logger
  static void shutdown();
};

}  // namespace roq

// Convert number to string (SO2670816)
#define STRINGIFY(number) STRINGIFY_HELPER(number)
#define STRINGIFY_HELPER(number) #number

// Raw logging interface
#define RAW_LOG(logger, sink) \
  logger(sink, \
      ::roq::static_basename_string(__FILE__).append( \
          ::roq::static_string(":" STRINGIFY(__LINE__) "] ")).data())

// Sink selectors
#define LOG_INFO(logger) RAW_LOG(logger, ::roq::detail::info)
#define LOG_WARNING(logger) RAW_LOG(logger, ::roq::detail::warning)
#define LOG_ERROR(logger) RAW_LOG(logger, ::roq::detail::error)
// XXX SO26888805 [[noreturn]]
#define LOG_FATAL(logger) RAW_LOG(logger, ::roq::detail::critical)

// The main logging interface (level in {INFO|WARNING|ERROR|FATAL})
#define LOG(level) LOG_ ## level(::roq::detail::LogMessage)

// Conditional logging
#define LOG_IF(level, condition) \
    !(condition) \
    ? (void)(0) \
    : LOG(level)

// System error logging
#define PLOG(level) LOG_ ## level(::roq::detail::ErrnoLogMessage)

// Verbose logging
#define VLOG(n) LOG_IF(INFO, (n) <= ::roq::detail::verbosity)

// Debug logging
#if defined(NDEBUG)
#define DLOG(level) LOG_ ## level(::roq::detail::NullLogMessage)
#define DLOG_IF(level, condition) LOG_ ## level(::roq::detail::NullLogMessage)
#else
#define DLOG(level) LOG(level)
#define DLOG_IF(level, condition) LOG_IF(level, condition)
#endif
