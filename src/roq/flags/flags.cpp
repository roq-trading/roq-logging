/* Copyright (c) 2017-2021, Hans Erik Thrane */

#include "roq/flags/flags.h"

#include <absl/flags/flag.h>

#include "roq/literals.h"

using namespace roq::literals;

ABSL_FLAG(  //
    std::string,
    log_pattern,
    {},
    "log pattern"_s);

ABSL_FLAG(  //
    uint32_t,
    log_flush_every_secs,
    3,
    "flush log every (seconds)"_s);

ABSL_FLAG(  //
    std::string,
    log_path,
    {},
    "log file (path)"_s);

ABSL_FLAG(  //
    uint32_t,
    log_max_size,
    1073741824,  // 1GB
    "max size of log file before rotating (only if path is non-empty)"_s);

ABSL_FLAG(  //
    uint32_t,
    log_max_files,
    3,
    "max number of log files to retain (only if path is non-empty)"_s);

ABSL_FLAG(  //
    bool,
    log_rotate_on_open,
    false,
    "rotate log file on open? (only if path is non-empty)"_s);

namespace roq {
namespace flags {

std::string_view Flags::log_pattern() {
  static const std::string result = absl::GetFlag(FLAGS_log_pattern);
  return result;
}

std::chrono::seconds Flags::log_flush_every() {
  static const std::chrono::seconds result{absl::GetFlag(FLAGS_log_flush_every_secs)};
  return result;
}

std::string_view Flags::log_path() {
  static const std::string result = absl::GetFlag(FLAGS_log_path);
  return result;
}

uint32_t Flags::log_max_size() {
  static const uint32_t result = absl::GetFlag(FLAGS_log_max_size);
  return result;
}

uint32_t Flags::log_max_files() {
  static const uint32_t result = absl::GetFlag(FLAGS_log_max_files);
  return result;
}

bool Flags::log_rotate_on_open() {
  static const bool result = absl::GetFlag(FLAGS_log_rotate_on_open);
  return result;
}

}  // namespace flags
}  // namespace roq
