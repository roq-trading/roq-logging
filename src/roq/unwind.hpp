/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <csignal>

namespace roq {
namespace unwind {

void print_stacktrace(int signal, siginfo_t *info);

}  // namespace unwind
}  // namespace roq
