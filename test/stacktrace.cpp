/* Copyright (c) 2017-2020, Hans Erik Thrane */

#include <gtest/gtest.h>

#include <absl/debugging/stacktrace.h>

TEST(stacktrace, simple) {
  // XXX doesn't seem to work on x86_64...
  // void *addr[32];
  // int depth = absl::GetStackTrace(addr, std::size(addr), 0);
  // EXPECT_EQ(depth, 0);  // will fail when it works
}
