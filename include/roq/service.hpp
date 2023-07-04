/* Copyright (c) 2017-2023, Hans Erik Thrane */

#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "roq/logging.hpp"  // XXX TODO remove (keeping for now to minimize downstream issues)

#include "roq/logging/logger.hpp"

namespace roq {

//! Convenience class to wrap \ref roq::Logger, absl::flags, etc.
struct ROQ_PUBLIC Service {
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

  Service(std::span<std::string_view> const &args, logging::Settings const &, Info const &);
  Service(int argc, char **argv, Info const &);  // XXX LEGACY

  Service(Service const &) = delete;
  Service(Service &&) = delete;

  virtual ~Service();

  //! The main function
  int run();

 protected:
  virtual int main(int argc, char **argv) = 0;

 private:
  std::string package_name_;
  std::string host_;
  std::string build_version_;
  std::string build_number_;
  std::string build_type_;
  std::string git_hash_;
  std::string compile_date_;
  std::string compile_time_;
  std::vector<std::string_view> args_;
  logging::Settings const settings_;
  logging::Logger logger_;
};

}  // namespace roq
