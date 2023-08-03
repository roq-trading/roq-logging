/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include <spdlog/logger.h>

#include "roq/logging/handler.hpp"
#include "roq/logging/settings.hpp"

namespace roq {
namespace logging {
namespace standard {

struct Logger final : public Handler {
  explicit Logger(Settings const &);

 protected:
  void operator()(Level, std::string_view const &message) override;
};

}  // namespace standard
}  // namespace logging
}  // namespace roq
