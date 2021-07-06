/* Copyright (c) 2017-2021, Hans Erik Thrane */

#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace roq {
namespace flags {

struct Flags final {
  static std::string_view log_pattern();
  static std::chrono::seconds log_flush_every();
  static std::string_view log_path();
  static uint32_t log_max_size();
  static uint32_t log_max_files();
  static bool log_rotate_on_open();
};

}  // namespace flags
}  // namespace roq
