/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include <absl/debugging/stacktrace.h>
#include <absl/debugging/symbolize.h>

#include <catch2/catch_all.hpp>

namespace {
static auto stack_frames_function() {
  constexpr auto const max_depth = size_t{10};
  void *result[max_depth];
  int sizes[max_depth];
  int depth = absl::GetStackFrames(result, sizes, max_depth, 0);
  return depth;
}
static auto stack_trace_function() {
  constexpr auto const max_depth = size_t{10};
  void *result[max_depth];
  int depth = absl::GetStackTrace(result, max_depth, 0);
  return depth;
}
}  // namespace

TEST_CASE("stack_frames_simple", "[stack_frames]") {
  auto depth = stack_frames_function();
#if defined(__x86_64__) && !defined(__APPLE__)
  REQUIRE(depth == 0);  // doesn't seem to work -- will fail when it works
#else
  REQUIRE(depth > 0);
#endif
}

TEST_CASE("stack_trace_simple", "[stack_trace]") {
  auto depth = stack_trace_function();
#if defined(__x86_64__) && !defined(__APPLE__)
  REQUIRE(depth == 0);  // doesn't seem to work -- will fail when it works
#else
  REQUIRE(depth > 0);
#endif
}
