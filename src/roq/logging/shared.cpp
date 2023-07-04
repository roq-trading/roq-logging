/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/logging/shared.hpp"

namespace roq {
namespace logging {

// === EXTERN ===

thread_local std::string message_buffer;

size_t verbosity = 0;
bool terminal_color = true;

/*
sink_type INFO = [](std::string_view const &message) {
  if (SPDLOG_OUT) [[likely]] {
    (*SPDLOG_OUT).log(spdlog::level::info, message);
  } else {
    std::cout << message << std::endl;
  }
};

sink_type WARNING = [](std::string_view const &message) {
  if (SPDLOG_OUT) [[likely]] {
    (*SPDLOG_OUT).log(spdlog::level::warn, message);
  } else {
    std::cout << message << std::endl;
  }
};

sink_type ERROR = [](std::string_view const &message) {
  if (SPDLOG_ERR) [[likely]] {
    (*SPDLOG_ERR).log(spdlog::level::err, message);
  } else {
    std::cerr << message << std::endl;
  }
};

sink_type CRITICAL = [](std::string_view const &message) {
  if (SPDLOG_ERR) [[likely]] {
    (*SPDLOG_ERR).log(spdlog::level::critical, message);
    (*SPDLOG_ERR).flush();
  } else {
    std::cerr << message << std::endl;
  }
};
*/

}  // namespace logging
}  // namespace roq
