set(TARGET_NAME ${PROJECT_NAME})

add_subdirectory(logging)

set(SOURCES
    logging/factory.cpp
    logging/handler.cpp
    logging/logger.cpp
    logging/shared.cpp
    service.cpp
    tool.cpp
    utils.cpp)

add_library(${TARGET_NAME} SHARED ${SOURCES})

target_link_libraries(
  ${TARGET_NAME}
  INTERFACE roq-api::roq-api magic_enum::magic_enum
  PUBLIC fmt::fmt
  PRIVATE ${PROJECT_NAME}-flags ${PROJECT_NAME}-spdlog ${PROJECT_NAME}-standard absl::symbolize spdlog::spdlog)

if(ROQ_BUILD_TYPE STREQUAL "Release")
  set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS_RELEASE -s)
endif()
