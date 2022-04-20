/* Copyright (c) 2017-2022, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "roq/logging.hpp"

namespace roq {

//! Convenience class to wrap \ref roq::Logger, absl::flags, etc.
class ROQ_PUBLIC Service {
 public:
  struct Info final {
    std::string_view description;
    std::string_view package_name = {};
    std::string_view build_version = {};
    std::string_view build_number = {};
    std::string_view build_type = {};
    std::string_view git_hash = {};
    std::string_view compile_date = __DATE__;
    std::string_view compile_time = __TIME__;
  };

  Service(int argc, char **argv, const Info &);

  Service(const Service &) = delete;
  Service(Service &&) = default;

  virtual ~Service();

  //! The main function
  int run();

 protected:
  virtual int main(int argc, char **argv) = 0;

 private:
  std::vector<char *> args_;
  std::string package_name_;
  std::string build_version_;
  std::string build_number_;
  std::string build_type_;
  std::string git_hash_;
  std::string compile_date_;
  std::string compile_time_;
};

}  // namespace roq