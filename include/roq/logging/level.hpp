/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <fmt/format.h>

#include <magic_enum/magic_enum_format.hpp>

namespace roq {
namespace logging {

enum class Level {
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  CRITICAL,
};

}  // namespace logging
}  // namespace roq
