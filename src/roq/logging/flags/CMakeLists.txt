set(TARGET_NAME "${PROJECT_NAME}-flags")

set(SOURCES flags.cpp)

add_library("${TARGET_NAME}" OBJECT ${SOURCES})

target_link_libraries("${TARGET_NAME}" absl::flags)
