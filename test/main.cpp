/* Copyright (c) 2017-2022, Hans Erik Thrane */

#define CATCH_CONFIG_RUNNER

#include <catch2/catch.hpp>

#include <absl/debugging/symbolize.h>

// SO5260907
extern int my_argc;
extern char **my_argv;

int main(int argc, char **argv) {
  ::absl::InitializeSymbolizer(argv[0]);
  my_argc = argc;
  my_argv = argv;
  return Catch::Session().run(argc, argv);
}
