/* Copyright (c) 2017-2021, Hans Erik Thrane */

#include "roq/service.h"

#include <cassert>

#include "roq/exceptions.h"

#include "roq/compat/abseil.h"

using namespace std::literals;

namespace roq {

namespace {
static auto initialize_flags(
    int argc, char **argv, const std::string_view &description, const std::string_view &version) {
  compat::Abseil::set_program_usage_message(std::string{description});
  compat::Abseil::set_flags_usage_config(std::string{version});
  return compat::Abseil::parse_command_line(argc, argv);
}
}  // namespace

Service::Service(
    int argc,
    char **argv,
    const std::string_view &description,
    const std::string_view &version,
    const std::string_view &build_type,
    const std::string_view &git_hash,
    const std::string_view &compile_date,
    const std::string_view &compile_time)
    : args_(initialize_flags(argc, argv, description, version)), build_type_(build_type), git_hash_(git_hash),
      compile_date_(compile_date), compile_time_(compile_time) {
  assert(args_.size() > 0);
  // matching spdlog pattern to glog
  // - %L = level (I=INFO|W=WARN|E=ERROR|C=CRITICAL)
  // - %m = month (MM)
  // - %d = day (DD)
  // - %T = time (HH:MM:SS)
  // - %f = fraction (microseconds)
  // - %t = thread (int)
  // - %v = message
  Logger::Config config{
      .pattern = "%L%m%d %T.%f %t %v"sv,
  };
  Logger::initialize(args_[0], config);
}

Service::~Service() {
}

int Service::run() {
  log::info("===== START ====="sv);
  auto res = EXIT_FAILURE;
  try {
    res = main(args_.size(), args_.data());
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
