/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include <catch2/catch.hpp>

#include <regex>
#include <string>

using namespace std::literals;

TEST_CASE("regex_simple", "regex") {
  std::regex re(".*/opt/conda/.*work/(src/)?(.*)"s);
  SECTION("test 1") {
    auto path = "/home/build/opt/conda/conda-bld/roq-logging_1641837162184/work/src/roq/flags/flags.cpp"sv;
    std::cmatch match;
    std::regex_match(std::begin(path), std::end(path), match, re);
    REQUIRE(std::size(match) == 3);
    CHECK(match[2] == "roq/flags/flags.cpp"s);
  }
  SECTION("test 2") {
    auto path = "/home/build/opt/conda/conda-bld/roq-oss-abseil-cpp_1641830430974/work/absl/flags/parse.cc"s;
    std::smatch match;
    std::regex_match(path, match, re);
    REQUIRE(std::size(match) == 3);
    CHECK(match[2] == "absl/flags/parse.cc");
  }
}
