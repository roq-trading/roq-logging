/* Copyright (c) 2017-2020, Hans Erik Thrane */

#include "roq/service.h"

#include <absl/flags/parse.h>
#include <absl/flags/usage.h>

#include <cassert>

namespace roq {

namespace {
static auto initialize_flags(
    int argc,
    char **argv,
    const std::string_view &description,
    const std::string_view &version) {
  assert(description.length() > 0);
  assert(version.length() > 0);
  absl::SetProgramUsageMessage(description.data());
  // gflags::SetVersionString(version.data());
  return absl::ParseCommandLine(argc, argv);
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
    : args_(initialize_flags(argc, argv, description, version)),
      build_type_(build_type), git_hash_(git_hash), compile_date_(compile_date),
      compile_time_(compile_time) {
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
  Logger::initialize(args_[0], pattern);
}

Service::~Service() {
}

int Service::run() {
  LOG(INFO)("===== START =====");
  /* XXX HANS
  LOG(INFO)
  (R"(Process: )"
   R"(name="{}", version="{}", type="{}", git="{}", date="{}", time="{}")",
   absl::ProgramInvocationShortName(),
   absl::VersionString(),
   build_type_,
   git_hash_,
   compile_date_,
   compile_time_);
  */
  auto res = main(args_.size(), args_.data());
  LOG_IF(WARNING, res != 0)(R"(exit-code={})", res);
  LOG(INFO)("===== STOP =====");
  return res;
}

}  // namespace roq
