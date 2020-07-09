cmake_minimum_required(VERSION 3.12)

project(roq-logging)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if(DEFINED ENV{CONDA_PREFIX})
  set(CMAKE_INSTALL_PREFIX $ENV{CONDA_PREFIX})
  set(CMAKE_FIND_ROOT_PATH $ENV{CONDA_PREFIX})
endif()

project(${PACKAGE_NAME} CXX)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

include(GNUInstallDirs)

find_package(Threads REQUIRED)
find_package(Libunwind REQUIRED)

find_package(absl REQUIRED)
find_package(fmt REQUIRED)
find_package(gflags REQUIRED)
find_package(spdlog REQUIRED)

include_directories(
  include
  ${LIBUNWIND_INCLUDE_DIR}
)

file(GLOB HEADER_LIST CONFIGURE_DEPENDS "include/roq/*.h")

add_subdirectory(src/roq)

option(BUILD_TEST "Enable test" OFF)

if(BUILD_TEST)
add_subdirectory(third_party/googletest EXCLUDE_FROM_ALL)
enable_testing()
  # include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include(GoogleTest)
add_subdirectory(test)
endif(BUILD_TEST)
