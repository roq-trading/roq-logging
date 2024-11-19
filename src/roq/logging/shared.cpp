/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/logging/shared.hpp"

namespace roq {
namespace logging {

// === EXTERN ===

thread_local std::string message_buffer;

size_t verbosity = 0;
bool terminal_color = true;

}  // namespace logging
}  // namespace roq
