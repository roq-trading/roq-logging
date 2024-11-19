/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include "roq/compat.hpp"

#include <thread>

namespace roq {
namespace logging {

extern ROQ_PUBLIC thread_local std::string message_buffer;

extern ROQ_PUBLIC size_t verbosity;
extern ROQ_PUBLIC bool terminal_color;

}  // namespace logging
}  // namespace roq
