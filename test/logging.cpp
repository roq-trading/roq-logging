/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include <catch2/catch.hpp>

#include "roq/logging.h"

// SO5260907
int my_argc;
char **my_argv;

using namespace roq;

using namespace std::literals;

TEST_CASE("logging_no_init_before", "logging") {
  log::info("info"sv);
  log::error("error"sv);
  CHECK(true == true);
}

TEST_CASE("logging_start_stop", "logging") {
  assert(my_argc > 0);
  Logger::Config config{};
  Logger::initialize(my_argv[0], config);
  Logger::shutdown();
}

TEST_CASE("logging_no_init_after", "logging") {
  log::info("info"sv);
  log::error("error"sv);
  CHECK(true == true);
}

TEST_CASE("logging_conditional", "logging") {
  // log::info::when(false, "{}"sv, 123);
  // log::info::when(true, "{}"sv, 123);
  log::info<0>::when(false, "{}"sv, 123);
  log::info<0>::when(true, "{}"sv, 123);
  // log::fatal::when(false, "{}"sv, 123);
}
