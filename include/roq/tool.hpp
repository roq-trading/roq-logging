/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "roq/logging.hpp"  // XXX TODO remove (keeping for now to minimize downstream issues)

#include "roq/logging/logger.hpp"

#include "roq/logging/flags/parser.hpp"

namespace roq {

//! Convenience class to wrap \ref roq::Logger, absl::flags, etc.
struct ROQ_PUBLIC Tool {
  struct Info final {
    std::string_view description;
    std::string_view package_name;
    std::string_view host = {};
    std::string_view build_version = {};
    std::string_view build_number = {};
    std::string_view build_type = {};
    std::string_view git_hash = {};
    std::string_view compile_date = __DATE__;
    std::string_view compile_time = __TIME__;
  };

  Tool(std::span<std::string_view> const &, logging::Settings const &, Info const &);

  Tool(Tool const &) = delete;
  Tool(Tool &&) = delete;

  virtual ~Tool();

  //! The main function
  int run();

 protected:
  virtual int main(int argc, char **argv) = 0;

 private:
  std::string build_type_;
  std::string git_hash_;
  std::string compile_date_;
  std::string compile_time_;
  std::vector<std::string_view> args_;
  logging::Settings const settings_;
  logging::Logger logger_;
};

}  // namespace roq
