/* Copyright (c) 2017-2021, Hans Erik Thrane */

#include "roq/flags/flags.h"

#include <absl/flags/flag.h>

using namespace std::literals;
using namespace std::chrono_literals;  // NOLINT

namespace {
class TimePeriod final {
 public:
  TimePeriod() = default;

  TimePeriod(const std::chrono::nanoseconds value) : value_(absl::FromChrono(value)) {}  // NOLINT (allow implicit)

  operator const absl::Duration &() const { return value_; }

  static std::string unparse(const TimePeriod &flag) { return absl::AbslUnparseFlag(flag.value_); }

  static bool parse(absl::string_view text, TimePeriod *flag, std::string *error) {
    if (!absl::ParseFlag(text, &flag->value_, error)) {
      return false;
    }
    return true;
  }

 private:
  absl::Duration value_ = {};
};

template <typename T>
static std::string AbslUnparseFlag(T flag) {
  return T::unparse(flag);
}

template <typename T>
static bool AbslParseFlag(absl::string_view text, T *flag, std::string *error) {
  return T::parse(text, flag, error);
}
}  // namespace

ABSL_FLAG(  //
    std::string,
    log_pattern,
    {},
    "log pattern"s);

ABSL_FLAG(  //
    TimePeriod,
    log_flush_freq,
    {3s},
    "flush log every"s);

ABSL_FLAG(  //
    std::string,
    log_path,
    {},
    "log file (path)"s);

ABSL_FLAG(  //
    uint32_t,
    log_max_size,
    1073741824,  // 1GB
    "max size of log file before rotating (only if path is non-empty)"s);

ABSL_FLAG(  //
    uint32_t,
    log_max_files,
    3,
    "max number of log files to retain (only if path is non-empty)"s);

ABSL_FLAG(  //
    bool,
    log_rotate_on_open,
    true,
    "rotate log file on open? (only if path is non-empty)"s);

namespace roq {
namespace flags {

std::string_view Flags::log_pattern() {
  static const std::string result = absl::GetFlag(FLAGS_log_pattern);
  return result;
}

std::chrono::nanoseconds Flags::log_flush_freq() {
  static const std::chrono::nanoseconds result{absl::ToChronoNanoseconds(absl::GetFlag(FLAGS_log_flush_freq))};
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
