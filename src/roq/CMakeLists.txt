set(ROQ_LOGGING_SOURCES
  application.cpp
  logging.cpp
  unwind.cpp
)

add_library(roq-logging SHARED
  ${ROQ_LOGGING_SOURCES}
  "${HEADER_LIST}"
)

target_link_libraries(roq-logging
  ${LIBUNWIND_LIBS}
  absl::symbolize
  fmt::fmt
  gflags
  spdlog::spdlog
)

set_target_properties(roq-logging PROPERTIES
  PUBLIC_HEADER "${HEADER_LIST}"
)

install(TARGETS roq-logging
  EXPORT roq-logging-config
  PUBLIC_HEADER DESTINATION include/roq
)

export(TARGETS roq-logging
  NAMESPACE roq-logging::
  FILE "${CMAKE_INSTALL_LIBDIR}/cmake/roq-logging/roq-logging-config.cmake"
)

install(EXPORT roq-logging-config
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/roq-logging"
  NAMESPACE roq-logging::
)
