/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <spdlog/logger.h>

#include "roq/logging/handler.hpp"
#include "roq/logging/settings.hpp"

namespace roq {
namespace logging {
namespace spdlog {

struct Logger final : public Handler {
  explicit Logger(Settings const &);

  ~Logger() override;

 protected:
  void operator()(Level, std::string_view const &message) override;

 private:
  ::spdlog::logger *out_ = nullptr;
  ::spdlog::logger *err_ = nullptr;
};

}  // namespace spdlog
}  // namespace logging
}  // namespace roq
