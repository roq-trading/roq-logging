/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/utils.hpp"

#include <limits.h>
#include <unistd.h>

#include <sys/utsname.h>

namespace roq {
namespace logging {

// === IMPLEMENTATION ===

std::string get_uname_v() {
  struct utsname utsname = {};
  if (uname(&utsname) == 0)
    return {utsname.version};
  return {};
}

std::string get_uname_s() {
  struct utsname utsname = {};
  if (uname(&utsname) == 0)
    return {utsname.sysname};
  return {};
}

std::string get_uname_r() {
  struct utsname utsname = {};
  if (uname(&utsname) == 0)
    return {utsname.release};
  return {};
}

std::string get_cwd() {
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)))
    return cwd;
  return {};
}

}  // namespace logging
}  // namespace roq
