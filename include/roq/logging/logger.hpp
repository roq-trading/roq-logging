/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include "roq/compat.hpp"

#include "roq/args/parser.hpp"

#include "roq/logging/settings.hpp"

namespace roq {
namespace logging {

// note! should only be used once
struct ROQ_PUBLIC Logger final {
  Logger(args::Parser const &, logging::Settings const &, bool stacktrace = true);

  Logger(Logger const &) = delete;
  Logger(Logger &&) = delete;

  ~Logger();
};

}  // namespace logging
}  // namespace roq
