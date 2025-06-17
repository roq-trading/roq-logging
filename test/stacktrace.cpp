/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <absl/debugging/stacktrace.h>

#include <array>
#include <cstddef>

#include <catch2/catch_test_macros.hpp>

namespace {
auto stack_frames_function() {
  constexpr size_t const max_depth = 10;
  std::array<void *, max_depth> result;
  std::array<int, max_depth> sizes;
  int depth = absl::GetStackFrames(std::data(result), std::data(sizes), std::size(result), 0);
  return depth;
}

auto stack_trace_function() {
  constexpr size_t const max_depth = 10;
  std::array<void *, max_depth> result;
  int depth = absl::GetStackTrace(std::data(result), std::size(result), 0);
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

/*
TEST_CASE("stack_trace_simple", "[stack_trace]") {
  auto depth = stack_trace_function();
  REQUIRE(depth > 0);  // note! seems to work also in debug mode since abseil 20250512
}
*/
