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
    : _argv(argv), _argc(initialize_gflags(argc, &_argv, description, version)),
      _build_type(build_type), _git_hash(git_hash), _compile_date(compile_date),
      _compile_time(compile_time) {
  assert(argc > 0);
  auto pattern = "%v";
  Logger::initialize(argv[0], pattern);
}

Tool::~Tool() {
}

int Tool::run() {
  return main(_argc, _argv);
}

}  // namespace roq
