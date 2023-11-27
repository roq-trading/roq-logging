/* Copyright (c) 2017-2023, Hans Erik Thrane */

#include "roq/tool.hpp"

#include <cassert>

#include "roq/exceptions.hpp"

#include "roq/logging.hpp"

#include "roq/logging/logger.hpp"

using namespace std::literals;

namespace roq {

// === CONSTANTS ===

namespace {
auto const DEFAULT_LOG_PATTERN = "%^%v%$"sv;  // XXX TODO spdlog specific
}  // namespace

// === HELPERS ===

namespace {
auto create_settings(auto &settings) {
  auto result = settings;
  if (std::empty(result.log.pattern))
    result.log.pattern = DEFAULT_LOG_PATTERN;
  return result;
}
}  // namespace

// === IMPLEMENTATION ===

Tool::Tool(args::Parser const &args, logging::Settings const &settings, Info const &info)
    : build_type_{info.build_type}, git_hash_{info.git_hash}, compile_date_{info.compile_date},
      compile_time_{info.compile_time}, args_{args}, settings_{create_settings(settings)}, logger_{args_, settings_} {
}

Tool::~Tool() {
}

int Tool::run() {
  auto res = EXIT_FAILURE;
  try {
    res = main(args_);
  } catch (SystemError const &e) {
    log::error("Exception: {}, error_code={}"sv, e, e.code().value());
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
