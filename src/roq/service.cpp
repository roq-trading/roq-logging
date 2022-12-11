/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/service.hpp"

#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <absl/flags/usage_config.h>

#include <cassert>

#include <ctre.hpp>

#include "roq/exceptions.hpp"

#include "roq/utils.hpp"

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

Service::Service(int argc, char **argv, Info const &info)
    : args_{initialize_flags(argc, argv, info.description, info.build_version)}, package_name_{info.package_name},
      host_{info.host}, build_version_{info.build_version}, build_number_{info.build_number},
      build_type_{info.build_type}, git_hash_{info.git_hash}, compile_date_{info.compile_date}, compile_time_{
                                                                                                    info.compile_time} {
  assert(std::size(args_) > 0);
  // matching spdlog pattern to glog
  // - %L = level (I=INFO|W=WARN|E=ERROR|C=CRITICAL)
  // - %m = month (MM)
  // - %d = day (DD)
  // - %T = time (HH:MM:SS)
  // - %f = fraction (microseconds)
  // - %t = thread (int)
  // - %v = message
  Logger::Config config{
      .pattern = "%L%m%d %T.%f %t %^%v%$"sv,
      .flush_freq = {},
      .path = {},
      .max_size = {},
      .max_files = {},
      .rotate_on_open = {},
      .color = {},
  };
  Logger::initialize(args_[0], config);
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
