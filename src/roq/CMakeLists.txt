set(TARGET_NAME "${PROJECT_NAME}")

set(SOURCES logging.cpp service.cpp static.cpp tool.cpp unwind.cpp)

add_library("${TARGET_NAME}" SHARED ${SOURCES})

target_link_libraries(
  "${TARGET_NAME}"
  INTERFACE fmt::fmt
  PRIVATE roq-api::roq-api absl::flags_parse absl::flags_usage absl::symbolize spdlog::spdlog
          ${LIBUNWIND_LIBRARIES})

target_compile_features("${TARGET_NAME}" PUBLIC cxx_std_17)

set_target_properties("${TARGET_NAME}" PROPERTIES LINK_FLAGS_RELEASE -s) # strip symbols
