set(TARGET_NAME ${PROJECT_NAME}-spdlog)

set(SOURCES logger.cpp)

add_library(${TARGET_NAME} OBJECT ${SOURCES})

target_link_libraries(${TARGET_NAME} PRIVATE spdlog::spdlog)
