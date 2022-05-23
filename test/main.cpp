/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include <absl/debugging/symbolize.h>

#define CATCH_CONFIG_RUNNER

#include <catch2/catch_all.hpp>

// SO5260907
extern int my_argc;
extern char **my_argv;

int main(int argc, char **argv) {
  ::absl::InitializeSymbolizer(argv[0]);
  my_argc = argc;
  my_argv = argv;
  return Catch::Session().run(argc, argv);
}
