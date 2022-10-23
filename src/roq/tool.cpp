/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include "roq/tool.hpp"

#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <absl/flags/usage_config.h>

#include <cassert>

#include <ctre.hpp>

#include "roq/exceptions.hpp"

using namespace std::literals;

namespace roq {

namespace {
constexpr auto const PATTERN = ctll::fixed_string{".*/opt/conda/.*work/(src/)?(.*)"};

auto initialize_flags(int argc, char **argv, std::string_view const &description, std::string_view const &version) {
  absl::SetProgramUsageMessage(description);
  assert(!std::empty(version));
  absl::FlagsUsageConfig config{
      .contains_helpshort_flags = {},
      .contains_help_flags = [](auto) { return true; },
      .contains_helppackage_flags = {},
      .version_string = [version = std::string{version}]() { return version; },
      .normalize_filename = [](auto const &file) -> std::string {
        auto [whole, dummy, sub] = ctre::match<PATTERN>(file);
        return whole ? std::string{sub} : std::string{file};
      },
  };
  absl::SetFlagsUsageConfig(config);
  return absl::ParseCommandLine(argc, argv);
}
}  // namespace

Tool::Tool(int argc, char **argv, Info const &info)
    : args_{initialize_flags(argc, argv, info.description, info.build_version)}, build_type_{info.build_type},
      git_hash_{info.git_hash}, compile_date_{info.compile_date}, compile_time_{info.compile_time} {
  assert(std::size(args_) > 0);
  Logger::Config config{
      .pattern = "%^%v%$"sv,
      .flush_freq = {},
      .path = {},
      .max_size = {},
      .max_files = {},
      .rotate_on_open = {},
      .color = {},
  };
  Logger::initialize(args_[0], config);
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
