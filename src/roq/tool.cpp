/* Copyright (c) 2017-2021, Hans Erik Thrane */

#include "roq/tool.h"

#include <cassert>

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
    : args_(initialize_flags(argc, argv, description, version)), build_type_(build_type),
      git_hash_(git_hash), compile_date_(compile_date), compile_time_(compile_time) {
  assert(args_.size() > 0u);
  auto pattern = "%v"_sv;
  Logger::initialize(args_[0u], pattern);
}

Tool::~Tool() {
}

int Tool::run() {
  return main(args_.size(), args_.data());
}

}  // namespace roq
