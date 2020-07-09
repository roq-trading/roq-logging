set(ROQ_LOGGING_TEST_SOURCES
  main.cpp
  logging.cpp
  stacktrace.cpp
)

add_executable(roq-logging-test
  ${ROQ_LOGGING_TEST_SOURCES}
)

target_link_libraries(roq-logging-test
  roq-logging
  absl::stacktrace
  absl::symbolize
  gtest_main
)

add_test(NAME roq-logging-test COMMAND roq-logging-test)
