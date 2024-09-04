set(TARGET_NAME ${PROJECT_NAME}-test)

set(SOURCES main.cpp logging.cpp stacktrace.cpp)

add_executable(${TARGET_NAME} ${SOURCES})

target_link_libraries(${TARGET_NAME} PRIVATE ${PROJECT_NAME} ${PROJECT_NAME}-flags roq-flags::roq-flags absl::stacktrace absl::symbolize Catch2::Catch2)

if(ROQ_BUILD_TYPE STREQUAL "Release")
  set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS_RELEASE -s)
endif()

add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
