/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>

#include "roq/args/parser.hpp"

#include "roq/logging/logger.hpp"

namespace roq {

//! Convenience class to wrap \ref roq::Logger, absl::flags, etc.
struct ROQ_PUBLIC Tool {
  struct Info final {
    std::string_view description;
    std::string_view package_name = {};
    std::string_view host = {};
    std::string_view build_version = {};
    std::string_view build_number = {};
    std::string_view build_type = {};
    std::string_view git_hash = {};
    std::string_view compile_date = __DATE__;
    std::string_view compile_time = __TIME__;
  };

  Tool(args::Parser const &, logging::Settings const &, Info const &);

  Tool(Tool const &) = delete;

  virtual ~Tool() = default;

  //! The main function
  int run();

 protected:
  virtual int main(args::Parser const &) = 0;

 private:
  std::string const build_type_;
  std::string const git_hash_;
  std::string const compile_date_;
  std::string const compile_time_;
  args::Parser const &args_;
  logging::Settings const settings_;
  logging::Logger logger_;
};

}  // namespace roq
