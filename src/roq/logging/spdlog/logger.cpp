/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/logging/spdlog/logger.hpp"

#include <unistd.h>

#include <spdlog/async.h>
#include <spdlog/spdlog.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>

#include "roq/logging/shared.hpp"

using namespace std::literals;

namespace roq {
namespace logging {
namespace spdlog {

// === CONSTANTS ===

namespace {
auto const SPDLOG_QUEUE_SIZE = 1048576uz;
auto const SPDLOG_THREAD_COUNT = 1uz;
}  // namespace

// === IMPLEMENTATION ===

Logger::Logger(Settings const &settings) {
  auto terminal = ::isatty(fileno(stdout));
  // note! non-interactive sessions are asynchronous
  auto interactive = std::empty(settings.log.path) && terminal != 0;
  if (!interactive) {
    ::spdlog::init_thread_pool(SPDLOG_QUEUE_SIZE, SPDLOG_THREAD_COUNT);
    if (settings.log.flush_freq.count() != 0) {
      ::spdlog::flush_every(std::chrono::duration_cast<std::chrono::seconds>(settings.log.flush_freq));
    }
  }
  std::shared_ptr<::spdlog::logger> out;
  std::shared_ptr<::spdlog::logger> err;
  if (std::empty(settings.log.path)) {
    if (terminal != 0) {
      // note! almost similar to stdout/stderr, only using spdlog for buffering
      if (terminal_color) {
        out = ::spdlog::stdout_color_mt("spdlog_out"s);
        {
          auto color_sink = static_cast<::spdlog::sinks::stdout_color_sink_mt *>((*out).sinks()[0].get());
          (*color_sink).set_color(::spdlog::level::debug, "\e[1;94m"sv);  // blue
          (*color_sink).set_color(::spdlog::level::info, "\e[0;37m"sv);   // grey
          (*color_sink).set_color(::spdlog::level::warn, "\e[1;92m"sv);   // green
        }
        err = ::spdlog::stderr_color_mt("spdlog_err"s);
        {
          auto color_sink = static_cast<::spdlog::sinks::stdout_color_sink_mt *>((*err).sinks()[0].get());
          (*color_sink).set_color(::spdlog::level::err, "\e[0;101m"sv);       // red background
          (*color_sink).set_color(::spdlog::level::critical, "\e[0;101m"sv);  // red background
        }
      } else {
        out = ::spdlog::stdout_logger_st("spdlog_out"s);
        err = ::spdlog::stderr_logger_st("spdlog_err"s);
      }
    } else {
      out = ::spdlog::stdout_logger_st<::spdlog::async_factory>("spdlog"s);
    }
  } else {
    out = ::spdlog::rotating_logger_st<::spdlog::async_factory>(
        "spdlog"s, std::string{settings.log.path}, settings.log.max_size, settings.log.max_files, settings.log.rotate_on_open);
  }
  if (!std::empty(settings.log.pattern)) {
    (*out).set_pattern(std::string{settings.log.pattern});
  }
  (*out).flush_on(::spdlog::level::warn);
  // note! async logging does not use a dedicated err stream
  // reason: avoid potential timing issues when interleaving two streams
  if (err) {
    if (!std::empty(settings.log.pattern)) {
      (*err).set_pattern(std::string{settings.log.pattern});
    }
    (*err).flush_on(::spdlog::level::warn);
  }
  // note! spdlog uses reference count
  out_ = out.get();
#ifndef NDEBUG
  (*out_).set_level(::spdlog::level::debug);
#endif
  if (err) {
    err_ = err.get();
#ifndef NDEBUG
    (*err).set_level(::spdlog::level::debug);
#endif
  } else {
    err_ = out_;
  }
  auto message = fmt::format("logging: {}"sv, interactive ? "sync"sv : "async"sv);
  (*out_).log(::spdlog::level::info, message);
}

Logger::~Logger() {
  try {
    // note! not thread-safe
    if (out_ != nullptr) {
      (*out_).flush();
    }
    if (err_ != nullptr) {
      (*err_).flush();
    }
    ::spdlog::drop_all();
  } catch (...) {
    // note! silent
  }
}

void Logger::operator()(Level level, std::string_view const &message) {
  switch (level) {
    using enum Level;
    case DEBUG:
      (*out_).log(::spdlog::level::debug, message);
      break;
    case INFO:
      (*out_).log(::spdlog::level::info, message);
      break;
    case WARNING:
      (*out_).log(::spdlog::level::warn, message);
      break;
    case ERROR:
      (*err_).log(::spdlog::level::err, message);
      break;
    case CRITICAL:
      (*err_).log(::spdlog::level::critical, message);
      break;
  }
}

}  // namespace spdlog
}  // namespace logging
}  // namespace roq
