set(TARGET_NAME ${PROJECT_NAME}-flags)

set(SOURCES flags.cpp settings.cpp)

add_library(${TARGET_NAME} SHARED ${SOURCES})

target_link_libraries(${TARGET_NAME} PRIVATE absl::flags_parse absl::flags_usage absl::symbolize)
