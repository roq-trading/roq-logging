/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include <string>

#include <catch2/catch_all.hpp>

#include <ctre.hpp>

using namespace std::literals;

namespace {
const constexpr auto PATTERN = ctll::fixed_string{".*/opt/conda/.*work/(src/)?(.*)"};
}

TEST_CASE("regex_simple", "[regex]") {
  {  // "test 1"
    auto path = "/home/build/opt/conda/conda-bld/roq-logging_1641837162184/work/src/roq/flags/flags.cpp"sv;
    auto [whole, dummy, sub] = ctre::match<PATTERN>(path);
    REQUIRE(whole == true);
    CHECK(sub == "roq/flags/flags.cpp"sv);
  }
  {  // "test 2"
    auto path = "/home/build/opt/conda/conda-bld/roq-oss-abseil-cpp_1641830430974/work/absl/flags/parse.cc"s;
    auto [whole, dummy, sub] = ctre::match<PATTERN>(path);
    REQUIRE(whole == true);
    CHECK(sub == "absl/flags/parse.cc"sv);
  }
  {  // "test 3"
    auto path = "parse.cc"s;
    auto [whole, dummy, sub] = ctre::match<PATTERN>(path);
    REQUIRE(whole == false);
  }
}
