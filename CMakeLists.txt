cmake_minimum_required(VERSION 3.12)

# cmake

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# version (using git tag)

include(GetGitRepoVersion)

# project

project(roq-logging VERSION ${GIT_REPO_VERSION})

# conda

if(DEFINED ENV{CONDA_PREFIX})
  set(CMAKE_INSTALL_PREFIX $ENV{CONDA_PREFIX})
  set(CMAKE_FIND_ROOT_PATH $ENV{CONDA_PREFIX})
endif()

# rpath

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# c++

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# filesystem

include(GNUInstallDirs)

# dependencies

find_package(Libunwind REQUIRED)
find_package(absl REQUIRED)
find_package(fmt REQUIRED)
find_package(gflags REQUIRED)
find_package(spdlog REQUIRED)

# include

include_directories(
  "${CMAKE_SOURCE_DIR}/include"
  "${CMAKE_SOURCE_DIR}/src")

# headers (public)

file(GLOB HEADERS CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/include/roq/*.h")

install(FILES ${HEADERS}
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/roq")

# sub-projects

add_subdirectory("${CMAKE_SOURCE_DIR}/src/roq")

# test

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(CTest)
endif()

if(BUILD_TESTING)
  include(GoogleTest)
  add_subdirectory("${CMAKE_SOURCE_DIR}/third_party/googletest" EXCLUDE_FROM_ALL)
  add_subdirectory("${CMAKE_SOURCE_DIR}/test")
endif(BUILD_TESTING)

# config

install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}-config
)

set(ROQ_LOGGING_CMAKE_DIR
  "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

export(TARGETS ${PROJECT_NAME}
  NAMESPACE ${PROJECT_NAME}::
  FILE "${ROQ_LOGGING_CMAKE_DIR}/${PROJECT_NAME}-config.cmake"
)

install(EXPORT ${PROJECT_NAME}-config
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION "${ROQ_LOGGING_CMAKE_DIR}"
)
