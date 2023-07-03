/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/logging/flags/settings.hpp"

#include "roq/logging/flags/flags.hpp"

namespace roq {
namespace logging {
namespace flags {

// === IMPLEMENTATION ===

Settings create_settings() {
  return {
      .log{
          .pattern = Flags::log_pattern(),
          .flush_freq = Flags::log_flush_freq(),
          .path = Flags::log_path(),
          .max_size = Flags::log_max_size(),
          .max_files = Flags::log_max_files(),
          .rotate_on_open = Flags::log_rotate_on_open(),
          .color = Flags::color(),
      },
  };
}

}  // namespace flags
}  // namespace logging
}  // namespace roq
