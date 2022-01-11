/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include <gtest/gtest.h>

#include <regex>
#include <string>

using namespace std::literals;

TEST(regex, simple) {
  std::regex re(".*/opt/conda/.*work/(.*)"s);
  {
    SCOPED_TRACE("test 1");
    auto path = "/home/build/opt/conda/conda-bld/roq-logging_1641837162184/work/src/roq/flags/flags.cpp"sv;
    std::cmatch match;
    std::regex_match(std::begin(path), std::end(path), match, re);
    ASSERT_EQ(std::size(match), 2);
    EXPECT_EQ(match[1], "src/roq/flags/flags.cpp"s);
  }
  {
    SCOPED_TRACE("test 2");
    auto path = "/home/build/opt/conda/conda-bld/roq-oss-abseil-cpp_1641830430974/work/absl/flags/parse.cc"s;
    std::smatch match;
    std::regex_match(path, match, re);
    ASSERT_EQ(std::size(match), 2);
    EXPECT_EQ(match[1], "absl/flags/parse.cc");
  }
}
