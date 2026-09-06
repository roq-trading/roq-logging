/* Copyright (c) 2017-2026, Hans Erik Thrane */

#define CATCH_CONFIG_RUNNER

#include <catch2/catch_session.hpp>

#include "./shared.hpp"

int my_argc;
char **my_argv;

int main(int argc, char **argv) {
  my_argc = argc;
  my_argv = argv;
  return Catch::Session{}.run(argc, argv);
}
