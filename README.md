# roq-logging


The logging library used by all Roq developed c++ projects.

Uses

* [fmt](https://github.com/fmtlib/fmt) (MIT License)
* [spdlog](https://github.com/gabime/spdlog) (MIT License)
* [gflags](https://github.com/gflags/gflags) (BDS 3-Clause License)
* [abseil-cpp](https://github.com/abseil/abseil-cpp) (Apache 2.0 License)
* [libunwind](https://github.com/libunwind/libunwind) (MIT License)

Borrows some design ideas (primarily the macro interface) from

* [glog](https://github.com/google/glog) (BSD 3-Clause License)


## Operating Systems

* Linux


## Prerequisites

The library is designed to be compatible with the conda package manager.

This is one way to create a conda environment, install the required
packages and build the project

```
# Download Miniconda

wget -N https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh

# Install Miniconda

bash Miniconda3-latest-Linux-x86_64.sh -b -u -p ~/miniconda3

# Activate Miniconda

source ~/miniconda3/bin/activate

# Install the toolchain

conda install -y git cmake gxx_linux-64 gdb_linux-64

# Install build dependencies

conda install -y \
    --channel https://roq-trading.com/conda/stable \
    roq-oss-abseil-cpp \
    roq-oss-fmt \
    roq-oss-libunwind \
    roq-oss-spdlog \
    gflags
```


## Building

```
# Update git submodules

git submodule update --init --recursive

# Generate makefile

cmake \
    -DCMAKE_AR="$AR" \
    -DCMAKE_RANLIB="$RANLIB" \
    -DCMAKE_NM="$NM" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTING=ON \
    .

# Compile

make -j4

# Test

make test

# Install

make install
```


## Installing

A pre-compiled binary package can be downloaded from Roq Trading Solutions

```
conda install -y \
    --channel https://roq-trading.com/conda/stable \
    roq-logging
```

## Using

```
#include "roq/logging.h"

void foo(int i) {
  // log when the ROQ_v environment variable has been set to 1 (or higher)
  VLOG(1)("enter foo")

  // always log
  LOG(INFO)("foo called with i={}", i);

  // conditional logging
  LOG_IF(WARNING, i > 10)("value exceeds threshold");

  // terminate the process with a stacktrace
  LOG(FATAL)("something is wrong");
}

int main(int argc, const char *argv[]) {
  roq::Logger::initialize(argv[0]);
  foo(argc);
  roq::Logger::shutdown();
}
```


## License

The library is released under the terms of the MIT license.


## Links

* [Documentation](https://roq-trading.com/docs)
* [Contact us](mailto:info@roq-trading.com)
