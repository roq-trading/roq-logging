/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include "roq/utils.hpp"

#include <sys/utsname.h>

namespace roq {
namespace logging {

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

}  // namespace logging
}  // namespace roq
