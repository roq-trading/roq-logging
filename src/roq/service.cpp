/* Copyright (c) 2017-2022, Hans Erik Thrane */

#include "roq/service.hpp"

#include <cassert>

#include "roq/exceptions.hpp"

#include "roq/compat/abseil.hpp"

#include "roq/utils.hpp"

using namespace std::literals;

namespace roq {

namespace {
auto initialize_flags(int argc, char **argv, const std::string_view &description, const std::string_view &version) {
  compat::Abseil::set_program_usage_message(std::string{description});
  compat::Abseil::set_flags_usage_config(std::string{version});
  return compat::Abseil::parse_command_line(argc, argv);
}
}  // namespace

Service::Service(int argc, char **argv, const Info &info)
    : args_(initialize_flags(argc, argv, info.description, info.build_version)), package_name_(info.package_name),
      host_(info.host), build_version_(info.build_version), build_number_(info.build_number),
      build_type_(info.build_type), git_hash_(info.git_hash), compile_date_(info.compile_date),
      compile_time_(info.compile_time) {
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
      .pattern = "%L%m%d %T.%f %t %g:%#] %^%v%$"sv,
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
  log::info("uname -v      : {}"sv, logging::get_uname());
  auto res = EXIT_FAILURE;
  try {
    res = main(std::size(args_), std::data(args_));
  } catch (const Exception &e) {
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
