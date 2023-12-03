/* Copyright (c) 2017-2024, Hans Erik Thrane */

#pragma once

#include "roq/compat.hpp"

#include <string_view>

#include "roq/logging/handler.hpp"
#include "roq/logging/settings.hpp"

namespace roq {
namespace logging {

struct ROQ_PUBLIC Factory final {
  static std::unique_ptr<Handler> create(std::string_view const &type, Settings const &);
};

}  // namespace logging
}  // namespace roq
