# Change Log

All notable changes will be documented in this file.

## Head

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
