/* Copyright (c) 2017-2021, Hans Erik Thrane */

#include "roq/tool.h"

#include <cassert>

#include "roq/exceptions.h"

#include "roq/compat/abseil.h"

using namespace roq::literals;

namespace roq {

namespace {
static auto initialize_flags(
    int argc, char **argv, const std::string_view &description, const std::string_view &version) {
  compat::Abseil::set_program_usage_message(std::string{description});
  compat::Abseil::set_flags_usage_config(std::string{version});
  return compat::Abseil::parse_command_line(argc, argv);
}
}  // namespace

Tool::Tool(
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
  auto pattern = "%v"_sv;
  Logger::initialize(args_[0], pattern);
}

Tool::~Tool() {
}

int Tool::run() {
  auto res = EXIT_FAILURE;
  try {
    res = main(args_.size(), args_.data());
  } catch (Exception &e) {
    log::error("Exception: {}"_fmt, e);
  } catch (std::exception &e) {
    log::error(R"(Exception: what="{}")"_fmt, e.what());
  } catch (...) {
    log::error("Exception: <unknown>"_sv);
  }
  return res;
}

}  // namespace roq
