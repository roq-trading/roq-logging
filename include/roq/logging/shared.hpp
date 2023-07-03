/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include "roq/compat.hpp"

#include <functional>
#include <string>
#include <string_view>

namespace roq {
namespace logging {

extern ROQ_PUBLIC thread_local std::string message_buffer;

extern ROQ_PUBLIC size_t verbosity;
extern ROQ_PUBLIC bool terminal_color;

using sink_type = std::function<void(std::string_view const &)>;

extern ROQ_PUBLIC sink_type INFO;
extern ROQ_PUBLIC sink_type WARNING;
extern ROQ_PUBLIC sink_type ERROR;
extern ROQ_PUBLIC sink_type CRITICAL;

}  // namespace logging
}  // namespace roq
