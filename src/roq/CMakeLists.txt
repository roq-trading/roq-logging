set(TARGET_NAME "${PROJECT_NAME}")

set(SOURCES
  application.cpp
  logging.cpp
  unwind.cpp
)

add_library("${TARGET_NAME}" SHARED
  ${SOURCES}
)

target_link_libraries("${TARGET_NAME}" INTERFACE
  fmt::fmt
  PRIVATE
    absl::symbolize
    spdlog::spdlog
    gflags
    ${LIBUNWIND_LIBRARIES}
)

set_target_properties("${TARGET_NAME}" PROPERTIES LINK_FLAGS_RELEASE -s)
