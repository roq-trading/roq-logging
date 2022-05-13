/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include "roq/utils.hpp"

#include <sys/utsname.h>

namespace roq {
namespace logging {

std::string get_uname() {
  struct utsname utsname = {};
  if (uname(&utsname) == 0)
    return {utsname.version};
  return {};
}

}  // namespace logging
}  // namespace roq
