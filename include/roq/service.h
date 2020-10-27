/* Copyright (c) 2017-2020, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>

#include "roq/logging.h"

namespace roq {

//! Convenience class to wrap \ref roq::Logger, gflags, etc.
class ROQ_LOGGING_PUBLIC Service {
 public:
  Service(
      int argc,
      char **argv,
      const std::string_view &description,
      const std::string_view &version,
      const std::string_view &build_type = std::string_view(),
      const std::string_view &git_hash = std::string_view(),
      const std::string_view &compile_date = __DATE__,
      const std::string_view &compile_time = __TIME__);

  Service(const Service &) = delete;
  Service(Service &&) = default;

  virtual ~Service();

  //! The main function
  int run();

 protected:
  virtual int main(int argc, char **argv) = 0;

 private:
  char **argv_;
  int argc_;
  std::string build_type_;
  std::string git_hash_;
  std::string compile_date_;
  std::string compile_time_;
};

}  // namespace roq
