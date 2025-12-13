/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include "roq/compat.hpp"

#include "roq/args/parser.hpp"

#include "roq/logging/settings.hpp"

namespace roq {
namespace logging {
namespace flags {

struct ROQ_PUBLIC Settings final : public roq::logging::Settings {
  explicit Settings(args::Parser const &);
};

}  // namespace flags
}  // namespace logging
}  // namespace roq
