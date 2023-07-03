/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/service.hpp"

#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <absl/flags/usage_config.h>

#include <cassert>

#include <ctre.hpp>

#include "roq/exceptions.hpp"

#include "roq/utils.hpp"

#include "roq/logging.hpp"

#include "roq/logging/logger.hpp"

#include "roq/logging/flags/settings.hpp"

using namespace std::literals;

namespace roq {

// === CONSTANTS ===

namespace {
constexpr auto const PATTERN = ctll::fixed_string{".*/opt/conda/.*work/(src/)?(.*)"};
// matching spdlog pattern to glog
// - %L = level (I=INFO|W=WARN|E=ERROR|C=CRITICAL)
// - %m = month (MM)
// - %d = day (DD)
// - %T = time (HH:MM:SS)
// - %f = fraction (microseconds)
// - %t = thread (int)
// - %v = message
auto const DEFAULT_LOG_PATTERN = "%L%m%d %T.%f %t %^%v%$"sv;
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

Service::Service(int argc, char **argv, Info const &info)
    : args_{initialize_flags(argc, argv, info.description, info.build_version)}, package_name_{info.package_name},
      host_{info.host}, build_version_{info.build_version}, build_number_{info.build_number},
      build_type_{info.build_type}, git_hash_{info.git_hash}, compile_date_{info.compile_date},
      compile_time_{info.compile_time}, settings_{create_settings()}, logger_{settings_} {
}

Service::~Service() {
}

int Service::run() {
  log::info("===== START ====="sv);
  log::info("package name  : {}"sv, package_name_);
  log::info("build version : {}"sv, build_version_);
  log::info("build number  : {}"sv, build_number_);
  log::info("build type    : {}"sv, build_type_);
  log::info("git hash      : {}"sv, git_hash_);
  log::info("compile time  : {} {}"sv, compile_date_, compile_time_);
  log::info("host          : {}"sv, host_);
  log::info("uname -v      : {}"sv, logging::get_uname_v());
  log::info("uname -s      : {}"sv, logging::get_uname_s());
  log::info("uname -r      : {}"sv, logging::get_uname_r());
  log::info("cwd           : {}"sv, logging::get_cwd());
  auto res = EXIT_FAILURE;
  try {
    res = main(std::size(args_), std::data(args_));
  } catch (Exception const &e) {
    log::error("Exception: {}"sv, e);
  } catch (std::exception &e) {
    log::error(R"(Exception: what="{}")"sv, e.what());
  } catch (...) {
    log::error("Exception: <unknown>"sv);
  }
  if (res != 0)
    log::warn("exit-code={}"sv, res);
  log::info("===== STOP ====="sv);
  return res;
}

}  // namespace roq
