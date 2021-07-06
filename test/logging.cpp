/* Copyright (c) 2017-2021, Hans Erik Thrane */

#include <gtest/gtest.h>

#include "roq/logging.h"

// SO5260907
int my_argc;
char **my_argv;

using namespace roq;

TEST(logging, no_init_before) {
  log::info("info"_sv);
  log::error("error"_sv);
  EXPECT_TRUE(true);
}

TEST(logging, start_stop) {
  assert(my_argc > 0);
  Logger::Config config{};
  Logger::initialize(my_argv[0], config);
  Logger::shutdown();
}

TEST(logging, no_init_after) {
  log::info("info"_sv);
  log::error("error"_sv);
  EXPECT_TRUE(true);
}
