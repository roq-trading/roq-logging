/* Copyright (c) 2017-2024, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/flags/args.hpp"

#include "roq/logging.hpp"

#include "roq/logging/logger.hpp"

#include "roq/logging/flags/settings.hpp"

#include "./shared.hpp"

using namespace roq;
using namespace roq::logging;

using namespace std::literals;

TEST_CASE("logging_no_init_before", "[logging]") {
  log::info("info"sv);
  log::error("error"sv);
  CHECK(true == true);
}

TEST_CASE("logging_start_stop", "[logging]") {
  assert(my_argc > 0);
  roq::flags::Args args{my_argc, my_argv, "test"sv, "test"sv};
  logging::flags::Settings settings{args};
  logging::Logger logger{args, settings};
}

TEST_CASE("logging_no_init_after", "[logging]") {
  log::info("info"sv);
  log::error("error"sv);
  CHECK(true == true);
}
