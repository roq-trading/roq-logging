set(TARGET_NAME "${PROJECT_NAME}")

add_subdirectory(logging/flags)

set(SOURCES logging.cpp service.cpp tool.cpp unwind.cpp utils.cpp)

add_library("${TARGET_NAME}" SHARED ${SOURCES})

target_link_libraries(
  "${TARGET_NAME}"
  PUBLIC roq-api::roq-api
  INTERFACE fmt::fmt
  PRIVATE ${PROJECT_NAME}-flags absl::flags_parse absl::flags_usage absl::symbolize spdlog::spdlog
          ${LIBUNWIND_LIBRARIES})

target_compile_features("${TARGET_NAME}" PUBLIC cxx_std_20)

set_target_properties("${TARGET_NAME}" PROPERTIES LINK_FLAGS_RELEASE -s) # strip symbols
