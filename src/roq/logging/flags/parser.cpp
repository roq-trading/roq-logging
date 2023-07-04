/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/logging/flags/parser.hpp"

#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <absl/flags/usage_config.h>

#include <ctre.hpp>

namespace roq {
namespace logging {
namespace flags {

// === CONSTANTS ===

namespace {
constexpr auto const PATTERN = ctll::fixed_string{".*/opt/conda/.*work/(src/)?(.*)"};
}

// === HELPER ===

namespace {
template <typename R>
R initialize(auto &args, auto &description, auto &version) {
  using result_type = std::remove_cvref<R>::type;
  assert(!std::empty(args));
  absl::SetProgramUsageMessage(description);
  assert(!std::empty(version));
  auto version_string = [version = std::string{version}]() { return version; };
  auto normalize_filename = [](auto const &file) -> std::string {
    auto [whole, dummy, sub] = ctre::match<PATTERN>(file);
    return whole ? std::string{sub} : std::string{file};
  };
  auto config = absl::FlagsUsageConfig{
      .contains_helpshort_flags = {},
      .contains_help_flags = [](auto) { return true; },
      .contains_helppackage_flags = {},
      .version_string = version_string,
      .normalize_filename = normalize_filename,
  };
  absl::SetFlagsUsageConfig(config);
  auto tmp = absl::ParseCommandLine(std::size(args), std::data(args));
  result_type result;
  for (auto &item : tmp)
    result.emplace_back(item);
  return result;
}
}  // namespace

// === IMPLEMENTATION ===

Parser::Parser(std::span<char *> const &args, std::string_view const &description, std::string_view const &version)
    : args_{initialize<decltype(args_)>(args, description, version)} {
}

Parser::~Parser() {
}

}  // namespace flags
}  // namespace logging
}  // namespace roq
