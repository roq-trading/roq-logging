# Change Log

All notable changes will be documented in this file.

## Head

## 1.1.0 &ndash; 2025-11-22

## 1.0.9 &ndash; 2025-09-26

## 1.0.8 &ndash; 2025-08-16

## 1.0.7 &ndash; 2025-07-02

## 1.0.6 &ndash; 2025-05-16


### Changed

* Use Abseil for stacktrace (#495)

## 1.0.5 &ndash; 2025-03-26

### Changed

* Upgrade the `format_str` helper class to work with `fmt 11.1` (#488)

## 1.0.4 &ndash; 2024-12-30

### Added

* Support `log_verbosity` command-line flag as lower priority alternative to the `ROQ_v` environment variable

## 1.0.3 &ndash; 2024-11-26

### Fixed

* Hide (singleton) instance variable from `logging::Handler` public interface.

## 1.0.2 &ndash; 2024-07-14

## 1.0.1 &ndash; 2024-04-14

## 1.0.0 &ndash; 2024-03-16

## 0.9.9 &ndash; 2024-01-28

## 0.9.8 &ndash; 2023-11-20

## 0.9.7 &ndash; 2023-09-18

### Changed

* Support custom log handler

## 0.9.6 &ndash; 2023-07-22

### Changed

* Use `Settings` (#356)

## 0.9.5 &ndash; 2023-06-12

## 0.9.4 &ndash; 2023-05-04

## 0.9.3 &ndash; 2023-03-20

### Changed

* The `--color` flag now has "auto" as default value (#318)

## 0.9.2 &ndash; 2023-02-22

## 0.9.1 &ndash; 2023-01-12

### Changed

* Use `std::string` (thread-local storage) for buffering

## 0.9.0 &ndash; 2022-12-22

## 0.8.9 &ndash; 2022-11-14

## 0.8.8 &ndash; 2022-10-04

## 0.8.7 &ndash; 2022-08-22

## 0.8.6 &ndash; 2022-07-18

### Added

* Authentication now includes kernel name + release (#260)
* Flag to control terminal color output (#245)

## 0.8.5 &ndash; 2022-06-06

### Changed

* Use color output when attached to terminal

## 0.8.4 &ndash; 2022-05-14

### Changed                                                                                                                

* Services will log `$HOST` and `uname -v` (#217)
* `log::<...>::when` was a mistake / reverting

## 0.8.3 &ndash; 2022-03-22

### Changed                                                                                                                

* Rename headers to .hpp (#195)
* Conda packaging should pin versions to match x.x.x (#189)

## 0.8.2 &ndash; 2022-02-18

### Changed                                                                                                                

* Always log packaging and build information (#159)

### Added

* Conditional logging (experimental)
                                                                                                                           
## 0.8.1 &ndash; 2022-01-16

### Changed                                                                                                                
                                                                                                                           
* Upgrade to C++20 (#158)

## 0.8.0 &ndash; 2022-01-12

### Changed

* Re-introduce Abseil Flags path, but exclude build prefix

## 0.7.9 &ndash; 2021-12-08

## 0.7.8 &ndash; 2021-11-02

### Changed

* Remove custom literals (#110)

## 0.7.7 &ndash; 2021-09-20

## 0.7.6 &ndash; 2021-09-02

## 0.7.5 &ndash; 2021-08-08

### Changed

* The `--log_flush_freq` (duration) has replaced `--log_flush_every_secs` (int)

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
