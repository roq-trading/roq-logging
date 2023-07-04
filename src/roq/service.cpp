/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/service.hpp"

#include <cassert>

#include "roq/exceptions.hpp"

#include "roq/utils.hpp"

#include "roq/logging.hpp"

#include "roq/logging/logger.hpp"

#include "roq/logging/flags/parser.hpp"
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
auto const DEFAULT_LOG_PATTERN = "%L%m%d %T.%f %t %^%v%$"sv;
}  // namespace

// === HELPERS ===

namespace {
template <typename R>
R create_args(auto argc, auto argv, auto &info) {
  using result_type = std::remove_cvref<R>::type;
  logging::flags::Parser parser{{argv, static_cast<size_t>(argc)}, info.description, info.build_version};
  auto tmp = static_cast<std::span<std::string_view>>(parser);
  result_type result;
  for (auto &item : tmp)
    result.emplace_back(item);
  return result;
}

auto create_settings(auto &settings) {
  auto result = settings;
  if (std::empty(result.log.pattern))
    result.log.pattern = DEFAULT_LOG_PATTERN;
  return result;
}

auto create_settings_LEGACY() {  // XXX LEGACY
  auto settings = logging::flags::create_settings();
  return create_settings(settings);
}
}  // namespace

// === IMPLEMENTATION ===

Service::Service(std::span<std::string_view> const &args, logging::Settings const &settings, Info const &info)
    : package_name_{info.package_name}, host_{info.host}, build_version_{info.build_version},
      build_number_{info.build_number}, build_type_{info.build_type}, git_hash_{info.git_hash},
      compile_date_{info.compile_date}, compile_time_{info.compile_time}, args_{std::begin(args), std::end(args)},
      settings_{create_settings(settings)}, logger_{args_, settings_} {
}

Service::Service(int argc, char **argv, Info const &info)  // XXX LEGACY
    : package_name_{info.package_name}, host_{info.host}, build_version_{info.build_version},
      build_number_{info.build_number}, build_type_{info.build_type}, git_hash_{info.git_hash},
      compile_date_{info.compile_date}, compile_time_{info.compile_time},
      args_{create_args<decltype(args_)>(argc, argv, info)}, settings_{create_settings_LEGACY()},
      logger_{args_, settings_} {
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
    // XXX LEGACY
    std::vector<char *> tmp;
    for (auto &item : args_)
      tmp.emplace_back(const_cast<char *>(std::data(item)));
    res = main(std::size(tmp), std::data(tmp));
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
