/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include "roq/compat.hpp"

#include <string_view>

#include "roq/logging/settings.hpp"

namespace roq {
namespace logging {

struct ROQ_PUBLIC Logger final {
  //! Initialize abseil
  static void initialize_0(std::string_view const &arg0);

  explicit Logger(logging::Settings const &, bool stacktrace = true);

  Logger(Logger const &) = delete;
  Logger(Logger &&) = delete;

  ~Logger();
};

}  // namespace logging
}  // namespace roq
