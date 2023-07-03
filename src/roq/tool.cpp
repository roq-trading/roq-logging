/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/tool.hpp"

#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <absl/flags/usage_config.h>

#include <cassert>

#include <ctre.hpp>

#include "roq/exceptions.hpp"

#include "roq/logging.hpp"

#include "roq/logging/logger.hpp"

#include "roq/logging/flags/settings.hpp"

using namespace std::literals;

namespace roq {

// === CONSTANTS ===

namespace {
constexpr auto const PATTERN = ctll::fixed_string{".*/opt/conda/.*work/(src/)?(.*)"};
auto const DEFAULT_LOG_PATTERN = "%^%v%$"sv;
}  // namespace

// === HELPERS ===

namespace {
auto initialize_flags(int argc, char **argv, std::string_view const &description, std::string_view const &version) {
  assert(argc > 0);
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
  auto result = absl::ParseCommandLine(argc, argv);
  assert(std::size(result) > 0);
  logging::Logger::initialize_0(result[0]);
  return result;
}

auto create_settings() {
  auto result = logging::flags::create_settings();
  if (std::empty(result.log.pattern))
    result.log.pattern = DEFAULT_LOG_PATTERN;
  return result;
}
}  // namespace

// === IMPLEMENTATION ===

Tool::Tool(int argc, char **argv, Info const &info)
    : args_{initialize_flags(argc, argv, info.description, info.build_version)}, build_type_{info.build_type},
      git_hash_{info.git_hash}, compile_date_{info.compile_date}, compile_time_{info.compile_time},
      settings_{create_settings()}, logger_{settings_} {
}

Tool::~Tool() {
}

int Tool::run() {
  auto res = EXIT_FAILURE;
  try {
    res = main(std::size(args_), std::data(args_));
  } catch (Exception &e) {
    log::error("Exception: {}"sv, e);
  } catch (std::exception &e) {
    log::error(R"(Exception: what="{}")"sv, e.what());
  } catch (...) {
    log::error("Exception: <unknown>"sv);
  }
  return res;
}

}  // namespace roq
