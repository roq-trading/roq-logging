set(TARGET_NAME "${PROJECT_NAME}")

set(SOURCES
  application.cpp
  logging.cpp
  unwind.cpp
)

add_library("${TARGET_NAME}" SHARED
  ${SOURCES}
)

target_link_libraries("${TARGET_NAME}"
  absl::symbolize
  fmt::fmt
  spdlog::spdlog
  gflags
  ${LIBUNWIND_LIBRARIES}
)
