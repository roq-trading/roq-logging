/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include "roq/compat/abseil.hpp"

#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <absl/flags/usage_config.h>

#include <cassert>

#if (__cplusplus >= 201703L)
#include <ctre.hpp>
#else
#include <regex>
#endif

using namespace std::literals;

namespace roq {
namespace compat {

namespace {
#if (__cplusplus >= 201703L)
const constexpr auto PATTERN = ctll::fixed_string{".*/opt/conda/.*work/(src/)?(.*)"};
#else
const std::regex REGEX(".*/opt/conda/.*work/(src/)?(.*)"s);
#endif
}  // namespace

void Abseil::set_program_usage_message(const std::string &message) {
#if (__cplusplus >= 201703L)
  assert(!std::empty(message));
#endif
  absl::SetProgramUsageMessage(message);
}

void Abseil::set_flags_usage_config(const std::string &version) {
#if (__cplusplus >= 201703L)
  assert(!std::empty(version));
#endif
  absl::FlagsUsageConfig config{
      .contains_helpshort_flags = {},
      .contains_help_flags = [](auto) { return true; },
      .contains_helppackage_flags = {},
      .version_string = [version]() -> std::string { return version; },
      .normalize_filename = [](const auto &file) -> std::string {
#if (__cplusplus >= 201703L)
        auto [whole, dummy, sub] = ctre::match<PATTERN>(file);
        return whole ? std::string{sub} : std::string{file};
#else
        std::cmatch match;
        std::regex_match(std::begin(file), std::end(file), match, REGEX);
        if (match.size() == 3)
          return std::string{match[2]};
        return std::string{file};
#endif
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
