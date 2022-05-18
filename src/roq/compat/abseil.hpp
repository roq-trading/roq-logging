/* Copyright (c) 2017-2022, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace roq {
namespace compat {

// some abseil-cpp functions have a signature depending on the default compiler standard version
// this library thus compiles with the default standard version (currently c++14 using clang
// on osx and c++17 using gcc on linux)

struct Abseil final {
  static void set_program_usage_message(std::string const &message);
  static void set_flags_usage_config(std::string const &version);
  static std::vector<char *> parse_command_line(int argc, char **argv);
};

}  // namespace compat
}  // namespace roq
