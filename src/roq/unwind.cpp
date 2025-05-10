/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/unwind.hpp"

#define UNW_LOCAL_ONLY
#include <libunwind.h>

#include <cxxabi.h>

#include <array>
#include <cstdio>
#include <cstdlib>

#include <fmt/format.h>

using namespace std::literals;

namespace roq {
namespace unwind {

// === CONSTANTS ===

namespace {
auto const PROC_NAME_LENGTH = 1024UZ;
std::array<char, PROC_NAME_LENGTH> proc_name = {};
constexpr int width = (2 * sizeof(void *)) + 2;
}  // namespace

// === IMPLEMENTATION ===

void print_stacktrace([[maybe_unused]] int signal, [[maybe_unused]] siginfo_t *info) {
#if defined(__linux__)
  psiginfo(info, nullptr);
#endif
  unw_context_t context;
  if (unw_getcontext(&context) != 0) {
    fmt::println(stderr, "{}"sv, "Unable to initialize libunwind context."sv);
    return;
  }
  unw_cursor_t cursor;
  if (unw_init_local(&cursor, &context) < 0) {
    fmt::println(stderr, "{}"sv, "Unable to initialize libunwind cursor."sv);
    return;
  }
  int status;
  for (int index = 0;; ++index) {
    status = unw_step(&cursor);
    if (status == 0) {  // done
      break;
    }
    if (status < 0) {  // failure
      fmt::println(stderr, "{}"sv, "Unable to step libunwind cursor."sv);
      return;
    }
    unw_word_t ip_register = 0;
    if (unw_get_reg(&cursor, UNW_REG_IP, &ip_register) < 0) {
      fmt::println(stderr, "{}"sv, "Unable to get libunwind ip register."sv);
    }
    unw_word_t offp;
    status = unw_get_proc_name(&cursor, std::data(proc_name), std::size(proc_name), &offp);
    char const *name = "<unknown>";
    char *demangled_name = nullptr;
    if (status < 0) {
      if (status != UNW_ENOINFO) {
        fmt::println(stderr, "{}"sv, "Unable to get libunwind proc_name."sv);
      }
    } else {
      name = std::data(proc_name);
      demangled_name = abi::__cxa_demangle(std::data(proc_name), nullptr, nullptr, &status);
      if (status == 0) {
        name = demangled_name;
      }
    }
    fmt::println(stderr, "[{:2}] {:#{}x} {}"sv, index, width, ip_register, name);
    if (demangled_name != nullptr) {
      free(demangled_name);
    }
  }
}

}  // namespace unwind
}  // namespace roq
