/* Copyright (c) 2017-2020, Hans Erik Thrane */

#include "roq/service.h"

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

Service::Service(
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
  // matching spdlog pattern to glog
  // - %L = level (I=INFO|W=WARN|E=ERROR|C=CRITICAL)
  // - %m = month (MM)
  // - %d = day (DD)
  // - %T = time (HH:MM:SS)
  // - %f = fraction (microseconds)
  // - %t = thread (int)
  // - %v = message
  auto pattern = "%L%m%d %T.%f %t %v";
  Logger::initialize(argv[0], pattern);
}

Service::~Service() {
}

int Service::run() {
  LOG(INFO)("===== START =====");
  LOG(INFO)
  (R"(Process: )"
   R"(name="{}", version="{}", type="{}", git="{}", date="{}", time="{}")",
   gflags::ProgramInvocationShortName(),
   gflags::VersionString(),
   _build_type,
   _git_hash,
   _compile_date,
   _compile_time);
  auto res = main(_argc, _argv);
  LOG_IF(WARNING, res != 0)(R"(exit-code={})", res);
  LOG(INFO)("===== STOP =====");
  return res;
}

}  // namespace roq
