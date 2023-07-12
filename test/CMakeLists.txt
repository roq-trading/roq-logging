set(TARGET_NAME ${PROJECT_NAME}-test)

set(SOURCES main.cpp logging.cpp regex.cpp stacktrace.cpp)

add_executable("${TARGET_NAME}" ${SOURCES})

target_link_libraries(
  ${TARGET_NAME} PRIVATE ${PROJECT_NAME} ${PROJECT_NAME}-flags roq-flags::roq-flags
                         absl::stacktrace absl::symbolize Catch2::Catch2)

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
