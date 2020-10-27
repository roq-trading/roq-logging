/* Copyright (c) 2017-2020, Hans Erik Thrane */

#include "roq/tool.h"

#include <gflags/gflags.h>

#include <cassert>

namespace roq {

namespace {
static int initialize_gflags(
    int argc,
    char ***argv,
    const std::string_view &description,
    const std::string_view &version) {
  assert(description.length() > 0);
  assert(version.length() > 0);
  gflags::SetUsageMessage(description.data());
  gflags::SetVersionString(version.data());
  gflags::ParseCommandLineFlags(&argc, argv, true);
  gflags::ShutDownCommandLineFlags();
  return argc;
}
}  // namespace

Tool::Tool(
    int argc,
    char **argv,
    const std::string_view &description,
    const std::string_view &version,
    const std::string_view &build_type,
    const std::string_view &git_hash,
    const std::string_view &compile_date,
    const std::string_view &compile_time)
    : argv_(argv), argc_(initialize_gflags(argc, &argv_, description, version)),
      build_type_(build_type), git_hash_(git_hash), compile_date_(compile_date),
      compile_time_(compile_time) {
  assert(argc_ > 0);
  auto pattern = "%v";
  Logger::initialize(argv_[0], pattern);
}

Tool::~Tool() {
}

int Tool::run() {
  return main(argc_, argv_);
}

}  // namespace roq
