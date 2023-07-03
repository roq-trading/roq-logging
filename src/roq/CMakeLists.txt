set(TARGET_NAME "${PROJECT_NAME}")

add_subdirectory(logging/flags)

set(SOURCES logger.cpp service.cpp tool.cpp unwind.cpp utils.cpp)

add_library("${TARGET_NAME}" SHARED ${SOURCES})

target_link_libraries(
  "${TARGET_NAME}"
  PUBLIC roq-api::roq-api
  INTERFACE fmt::fmt
  PRIVATE ${PROJECT_NAME}-flags absl::flags_parse absl::flags_usage absl::symbolize spdlog::spdlog
          ${LIBUNWIND_LIBRARIES})

set_target_properties("${TARGET_NAME}" PROPERTIES LINK_FLAGS_RELEASE -s) # strip symbols
