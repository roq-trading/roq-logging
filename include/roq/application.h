/* Copyright (c) 2017-2020, Hans Erik Thrane */

#pragma once

#include "roq/logging.h"

namespace roq {

class ROQ_LOGGING_PUBLIC Application {
 public:
  Application(
      int argc,
      char **argv,
      const std::string_view& description,
      const std::string_view& version);

  Application(const Application&) = delete;
  Application(Application&&) = default;

  virtual ~Application();

  int run();

 protected:
  virtual int main(int argc, char **argv) = 0;

 private:
  char **_argv;
  int _argc;
};

}  // namespace roq
