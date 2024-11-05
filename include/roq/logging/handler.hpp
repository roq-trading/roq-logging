/* Copyright (c) 2017-2024, Hans Erik Thrane */

#pragma once

#include "roq/compat.hpp"

#include <string_view>

#include "roq/logging/level.hpp"

namespace roq {
namespace logging {

struct ROQ_PUBLIC Handler {
  Handler();

  virtual ~Handler();

  Handler(Handler &&) = default;
  Handler(Handler const &) = delete;

  virtual void operator()(Level, std::string_view const &message) = 0;

  static Handler &get_instance() { return *INSTANCE; }

 private:
  static Handler *INSTANCE;
};

}  // namespace logging
}  // namespace roq
