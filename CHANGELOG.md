# Change Log

All notable changes will be documented in this file.

## Head

## 0.7.4 &ndash; 2021-07-20

## 0.7.3 &ndash; 2021-07-06

### Added

* The `format_str` class has been moved from the roq-api library
* Flags to allow logging to file (with automatic rotation)

### Changed

* Automatic capture of `source_location` is now managed by the `format_str` class
* All loggers, except for critical and fatal, now accept a template argument for the log level

### Removed

* The `trace_n` loggers

## 0.7.2 &ndash; 2021-06-20

### Added

* `log::critical` to only abort if debug build (`log::fatal` is too strict in some cases)

### Changed

* `log::debug` will now prefix all messages with 'DEBUG:' to make it easier to manage and
  remove log statements as they become "stale"

## 0.7.1 &ndash; 2021-05-30

### Changed

* Dropped requirement for calling `Logger::initialize(...)`

  * Default is now to use `std::cout` and `std::cerr`

* `Service` and `Tool` will now catch and log exceptions at top-level main wrapper

## 0.7.0 &ndash; 2021-04-15

### Changed

* Replaced macro-based interface with a pure C++ based interface

## 0.6.1 &ndash; 2021-02-19

### Changed

* Use string literals
* Enforce use of `roq::format_str` or the `_fmt` literal
* Now depends on `roq-api`

## 0.6.0 &ndash; 2021-02-02

### Changed

* Now using `absl::flags` (instead of gflags)

## 0.5.0 &ndash; 2020-12-04

## 0.4.5 &ndash; 2020-11-09

### Changed

* Application has been replaced with Service and Tool

  * Service logging includes process information, start, stop, timestamp,
    thread id, etc.
  * Tool logging is minimal

## 0.4.4 &ndash; 2020-09-20

## 0.4.3 &ndash; 2020-09-02

### Added

* Application will now log process information on startup

## 0.4.2 &ndash; 2020-07-27

### Removed

* Automake support

## 0.4.1 &ndash; 2020-07-17

### Changed

* CMake is now the default build system for all Roq solutions

## 0.4.0 &ndash; 2020-06-30

## 0.3.9 &ndash; 2020-06-09

### Added

* Interface and implementation moved out of `roq-api`

## 0.3.8 &ndash; 2020-06-06
