cmake_minimum_required(VERSION 3.12)

# cmake

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# version (using git tag)

include(GetGitRepoVersion)

message("Using GIT_REPO_VERSION=${GIT_REPO_VERSION}")

# project

project(roq-logging VERSION ${GIT_REPO_VERSION})

# conda

if(DEFINED ENV{CONDA_PREFIX} AND NOT DEFINED ENV{CONDA_BUILD})
  message("Setting CMAKE_INSTALL_PREFIX and CMAKE_FIND_ROOT_PATH")
  set(CMAKE_INSTALL_PREFIX $ENV{CONDA_PREFIX})
  set(CMAKE_FIND_ROOT_PATH $ENV{CONDA_PREFIX})
endif()

# rpath

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# prefer static libraries

set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})

# c++

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  message("Setting default visibility to hidden")
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
  set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
endif (CMAKE_BUILD_TYPE STREQUAL "Release")

# filesystem

include(GNUInstallDirs)

# dependencies

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  find_package(Libunwind REQUIRED)
endif()

find_package(absl REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(roq-api REQUIRED)

# include

include_directories("${CMAKE_SOURCE_DIR}/include" "${CMAKE_SOURCE_DIR}/src")

# sub-projects

add_subdirectory("${CMAKE_SOURCE_DIR}/src/roq")

# install (public headers)

install(
  DIRECTORY "${CMAKE_SOURCE_DIR}/include/roq/"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/roq"
  FILES_MATCHING
  PATTERN "*.h"
  PATTERN "CMakeFiles" EXCLUDE)

# test

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(CTest)
endif()

if(BUILD_TESTING)
  include(GoogleTest)
  add_subdirectory("${CMAKE_SOURCE_DIR}/third_party/googletest" EXCLUDE_FROM_ALL)
  add_subdirectory("${CMAKE_SOURCE_DIR}/test")
endif(BUILD_TESTING)

# doxygen

option(BUILD_DOCS "Enable doxygen" OFF)

if(BUILD_DOCS)
  find_package(Doxygen)
  add_subdirectory("${CMAKE_SOURCE_DIR}/doxygen")
  add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-doxygen)
endif(BUILD_DOCS)

# install (cmake)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-config)

install(FILES "${CMAKE_SOURCE_DIR}/CHANGELOG.md"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/${PROJECT_NAME}")

set(CMAKE_LIB_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

export(
  TARGETS ${PROJECT_NAME}
  NAMESPACE ${PROJECT_NAME}::
  FILE "${CMAKE_LIB_DIR}/${PROJECT_NAME}-config.cmake")

install(
  EXPORT ${PROJECT_NAME}-config
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION "${CMAKE_LIB_DIR}")
