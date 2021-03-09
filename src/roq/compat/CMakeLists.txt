set(TARGET_NAME "${PROJECT_NAME}-compat")

set(SOURCES abseil.cpp)

add_library("${TARGET_NAME}" STATIC ${SOURCES})

target_compile_features("${TARGET_NAME}" PUBLIC cxx_std_14)
