/* Copyright (c) 2017-2021, Hans Erik Thrane */

#include "roq/tool.h"

#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <absl/flags/usage_config.h>

#include <cassert>

namespace roq {

namespace {
static char VERSION[32] = {};
static auto initialize_flags(
    int argc, char **argv, const std::string_view &description, const std::string_view &version) {
  assert(description.length() > 0);
  absl::SetProgramUsageMessage(description.data());
  assert(version.length() > 0);
  assert(version.length() < (sizeof(VERSION) - 1));
  version.copy(VERSION, sizeof(VERSION - 1));
  absl::FlagsUsageConfig config{
      .contains_helpshort_flags = {},
      .contains_help_flags = [](auto) { return true; },
      .contains_helppackage_flags = {},
      .version_string = []() { return VERSION; },
      .normalize_filename = [](const std::string_view &file) -> std::string {
        if (file.find("roq") != file.npos)
          return "roq";
        return std::string{file};
      },
  };
  return absl::ParseCommandLine(argc, argv);
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
    : args_(initialize_flags(argc, argv, description, version)), build_type_(build_type),
      git_hash_(git_hash), compile_date_(compile_date), compile_time_(compile_time) {
  assert(args_.size() > 0);
  auto pattern = "%v";
  Logger::initialize(args_[0], pattern);
}

Tool::~Tool() {
}

int Tool::run() {
  return main(args_.size(), args_.data());
}

}  // namespace roq
