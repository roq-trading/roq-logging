/* Copyright (c) 2017-2024, Hans Erik Thrane */

#pragma once

#include <signal.h>

namespace roq {
namespace unwind {

void print_stacktrace(int signal, siginfo_t *info);

}  // namespace unwind
}  // namespace roq
