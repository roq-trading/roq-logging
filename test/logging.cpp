/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/logging.hpp"

#include "roq/logging/logger.hpp"

#include "roq/logging/flags/settings.hpp"

#include "./shared.hpp"

using namespace roq;

using namespace std::literals;

TEST_CASE("logging_no_init_before", "[logging]") {
  log::info("info"sv);
  log::error("error"sv);
  CHECK(true == true);
}

TEST_CASE("logging_start_stop", "[logging]") {
  assert(my_argc > 0);
  logging::Logger::initialize_0(my_argv[0]);
  auto settings = logging::flags::create_settings();
  logging::Logger logger{settings};
}

TEST_CASE("logging_no_init_after", "[logging]") {
  log::info("info"sv);
  log::error("error"sv);
  CHECK(true == true);
}
