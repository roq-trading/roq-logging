/* Copyright (c) 2017-2021, Hans Erik Thrane */

#include "roq/static.h"

using namespace std::literals;  // NOLINT

namespace roq {

static_assert(static_basename_string("").compare(""sv) == 0);
static_assert(static_basename_string("abc").compare("abc"sv) == 0);
static_assert(static_basename_string("abc/def").compare("def"sv) == 0);
static_assert(static_basename_string("abc/def/ghi").compare("ghi"sv) == 0);

static_assert(
    static_basename_string("abc/def").append(static_string(":123")).compare("def:123"sv) == 0);

}  // namespace roq
