/* Copyright (c) 2017-2020, Hans Erik Thrane */

#include <absl/debugging/symbolize.h>

#include <gtest/gtest.h>

// SO5260907
extern int my_argc;
extern char **my_argv;

int main(int argc, char **argv) {
  ::absl::InitializeSymbolizer(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  my_argc = argc;
  my_argv = argv;
  return RUN_ALL_TESTS();
}
