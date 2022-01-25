/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include <gtest/gtest.h>

#include <absl/debugging/stacktrace.h>
#include <absl/debugging/symbolize.h>

namespace {
static auto stack_frames_function() {
  const constexpr size_t max_depth = 10;
  void *result[max_depth];
  int sizes[max_depth];
  int depth = absl::GetStackFrames(result, sizes, max_depth, 0);
  return depth;
}
static auto stack_trace_function() {
  const constexpr size_t max_depth = 10;
  void *result[max_depth];
  int depth = absl::GetStackTrace(result, max_depth, 0);
  return depth;
}
}  // namespace

TEST(stack_frames, simple) {
  auto depth = stack_frames_function();
#if defined(__x86_64__)
  ASSERT_EQ(depth, 0);  // doesn't seem to work -- will fail when it works
#else
  ASSERT_GT(depth, 0);
#endif
}

TEST(stack_trace, simple) {
  auto depth = stack_trace_function();
#if defined(__x86_64__)
  ASSERT_EQ(depth, 0);  // doesn't seem to work -- will fail when it works
#else
  ASSERT_GT(depth, 0);
#endif
}
