set(TARGET_NAME "${PROJECT_NAME}-test")

set(SOURCES
  main.cpp
  logging.cpp
  stacktrace.cpp
)

add_executable("${TARGET_NAME}" ${SOURCES})

# target

target_link_libraries("${TARGET_NAME}"
  ${PROJECT_NAME}
  absl::stacktrace
  absl::symbolize
  gtest_main
)

# run

add_test(NAME "${TARGET_NAME}"
  COMMAND "${TARGET_NAME}")
