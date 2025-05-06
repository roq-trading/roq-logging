/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/utils.hpp"

#include <unistd.h>

#include <sys/utsname.h>

#include <array>
#include <climits>

namespace roq {
namespace logging {

// === IMPLEMENTATION ===

std::string get_uname_v() {
  struct utsname utsname = {};
  if (::uname(&utsname) == 0) {
    return {utsname.version};
  }
  return {};
}

std::string get_uname_s() {
  struct utsname utsname = {};
  if (::uname(&utsname) == 0) {
    return {utsname.sysname};
  }
  return {};
}

std::string get_uname_r() {
  struct utsname utsname = {};
  if (::uname(&utsname) == 0) {
    return {utsname.release};
  }
  return {};
}

std::string get_cwd() {
  std::array<char, PATH_MAX> cwd = {};
  if (::getcwd(std::data(cwd), std::size(cwd)) != nullptr) {
    return {std::data(cwd)};
  }
  return {};
}

}  // namespace logging
}  // namespace roq
