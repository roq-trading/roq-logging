/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include "roq/compat.hpp"

#include <span>
#include <string_view>
#include <vector>

namespace roq {
namespace logging {
namespace flags {

// note! should only be used once
struct ROQ_PUBLIC Parser final {
  Parser(std::span<char *> const &args, std::string_view const &description, std::string_view const &version);

  Parser(int argc, char **argv, std::string_view const &description, std::string_view const &version)
      : Parser{{argv, static_cast<size_t>(argc)}, description, version} {}

  Parser(Parser const &) = delete;
  Parser(Parser &&) = delete;

  ~Parser();

  operator std::span<std::string_view>() { return args_; }

 private:
  std::vector<std::string_view> args_;
};

}  // namespace flags
}  // namespace logging
}  // namespace roq
