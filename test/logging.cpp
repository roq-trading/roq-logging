/* Copyright (c) 2017-2020, Hans Erik Thrane */

#include <gtest/gtest.h>

#include "roq/logging.h"

// SO5260907
int my_argc;
char **my_argv;

using namespace roq;  // NOLINT

TEST(logging, start_stop) {
  assert(my_argc > 0);
  Logger::initialize(my_argv[0]);
  Logger::shutdown();
}
