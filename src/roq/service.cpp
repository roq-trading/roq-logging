/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/service.hpp"

#include <unistd.h>

#include <cassert>

#include "roq/exceptions.hpp"

#include "roq/utils.hpp"

#include "roq/logging.hpp"

#include "roq/logging/factory.hpp"
#include "roq/logging/logger.hpp"

#include "roq/logging/flags/settings.hpp"

using namespace std::literals;

namespace roq {

// === CONSTANTS ===

namespace {
// matching spdlog pattern to glog
// - %L = level (I=INFO|W=WARN|E=ERROR|C=CRITICAL)
// - %m = month (MM)
// - %d = day (DD)
// - %T = time (HH:MM:SS)
// - %f = fraction (microseconds)
// - %t = thread (int)
// - %v = message
auto const DEFAULT_LOG_PATTERN = "%L%m%d %T.%f %t %^%v%$"sv;  // XXX TODO spdlog specific
}  // namespace

// === HELPERS ===

namespace {
auto create_settings(auto &settings) {
  auto result = settings;
  if (std::empty(result.log.pattern)) {
    result.log.pattern = DEFAULT_LOG_PATTERN;
  }
  return result;
}
}  // namespace

// === IMPLEMENTATION ===

Service::Service(args::Parser const &args, logging::Settings const &settings, Info const &info)
    : package_name_{info.package_name}, host_{info.host}, build_version_{info.build_version}, build_number_{info.build_number}, build_type_{info.build_type},
      git_hash_{info.git_hash}, compile_date_{info.compile_date}, compile_time_{info.compile_time}, args_{args}, settings_{create_settings(settings)},
      handler_2_{logging::Factory::create("spdlog"sv, settings_)}, handler_{*handler_2_}, logger_{args_, settings_} {
}

Service::Service(args::Parser const &args, logging::Settings const &settings, logging::Handler &handler, Info const &info)
    : package_name_{info.package_name}, host_{info.host}, build_version_{info.build_version}, build_number_{info.build_number}, build_type_{info.build_type},
      git_hash_{info.git_hash}, compile_date_{info.compile_date}, compile_time_{info.compile_time}, args_{args}, settings_{create_settings(settings)},
      handler_{handler}, logger_{args_, settings_} {
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
  log::info("pid           : {}"sv, ::getpid());
  auto res = EXIT_FAILURE;
  try {
    res = main(args_);
  } catch (SystemError const &e) {
    log::error("Exception: {}"sv, e);
  } catch (Exception const &e) {
    log::error("Exception: {}"sv, e);
  } catch (std::exception const &e) {
    log::error(R"(Exception: what="{}")"sv, e.what());
  } catch (...) {
    log::error("Exception: <unknown>"sv);
  }
  if (res != 0) {
    log::warn("exit-code={}"sv, res);
  }
  log::info("===== STOP ====="sv);
  return res;
}
}  // namespace roq
