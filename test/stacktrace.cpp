/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <absl/debugging/stacktrace.h>

#include <cstddef>

#include <catch2/catch_test_macros.hpp>

namespace {
static auto stack_frames_function() {
  constexpr size_t const max_depth = 10;
  void *result[max_depth];
  int sizes[max_depth];
  int depth = absl::GetStackFrames(result, sizes, max_depth, 0);
  return depth;
}
static auto stack_trace_function() {
  constexpr size_t const max_depth = 10;
  void *result[max_depth];
  int depth = absl::GetStackTrace(result, max_depth, 0);
  return depth;
}
}  // namespace

TEST_CASE("stack_frames_simple", "[stack_frames]") {
  auto depth = stack_frames_function();
#if defined(NDEBUG) || defined(__APPLE__)
  REQUIRE(depth > 0);
#else
  REQUIRE(depth == 0);  // doesn't seem to work in debug
#endif
}

TEST_CASE("stack_trace_simple", "[stack_trace]") {
  auto depth = stack_trace_function();
#if defined(NDEBUG) || defined(__APPLE__)
  REQUIRE(depth > 0);
#else
  REQUIRE(depth == 0);  // doesn't seem to work in debug
#endif
}
