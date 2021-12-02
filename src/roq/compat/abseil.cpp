/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include "roq/compat/abseil.h"

#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <absl/flags/usage_config.h>

#include <cassert>

using namespace std::literals;

namespace roq {
namespace compat {

void Abseil::set_program_usage_message(const std::string &message) {
  assert(!std::empty(message));
  absl::SetProgramUsageMessage(message);
}

void Abseil::set_flags_usage_config(const std::string &version) {
  assert(!std::empty(version));
  absl::FlagsUsageConfig config{
      .contains_helpshort_flags = {},
      .contains_help_flags = [](auto) { return true; },
      .contains_helppackage_flags = {},
      .version_string = [version]() -> std::string { return version; },
      .normalize_filename = [](const auto &file) -> std::string {
        if (file.find("roq"sv) != file.npos)
          return "roq"s;
        return std::string{file};
      },
  };
  absl::SetFlagsUsageConfig(config);
}

std::vector<char *> Abseil::parse_command_line(int argc, char **argv) {
  assert(argc > 0);
  return absl::ParseCommandLine(argc, argv);
}

}  // namespace compat
}  // namespace roq
