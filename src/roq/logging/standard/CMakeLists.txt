set(TARGET_NAME ${PROJECT_NAME}-standard)

set(SOURCES logger.cpp)

add_library(${TARGET_NAME} OBJECT ${SOURCES})

target_link_libraries(${TARGET_NAME})
