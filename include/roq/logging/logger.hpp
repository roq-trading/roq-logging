/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include "roq/compat.hpp"

#include "roq/args/parser.hpp"

#include "roq/logging/handler.hpp"
#include "roq/logging/settings.hpp"

namespace roq {
namespace logging {

// note! should only be used once
struct ROQ_PUBLIC Logger final {
  Logger(args::Parser const &, logging::Settings const &, bool stacktrace = true);

  Logger(Logger const &) = delete;
  Logger(Logger &&) = delete;
};

}  // namespace logging
}  // namespace roq
