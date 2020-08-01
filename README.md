# roq-logging

Logging utilities.


## Operating Systems

* Linux


## Library/Package Dependencies

* [fmt](https://github.com/fmtlib/fmt) (MIT License)
* [spdlog](https://github.com/gabime/spdlog) (MIT License)
* [gflags](https://github.com/gflags/gflags) (BDS 3-Clause License)
* [Abseil](https://github.com/abseil/abseil-cpp) (Apache 2.0 License)
* [libunwind](https://github.com/libunwind/libunwind) (MIT License)

Optional

* [Google Test](https://github.com/google/googletest) (BSD 3-Clause License)


## Prerequisites

The project is designed to be compatible with the conda package manager.

```bash
wget -N https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh

bash Miniconda3-latest-Linux-x86_64.sh -b -u -p ~/miniconda3

source ~/miniconda3/bin/activate

conda install -y \
    git \
    cmake \
    gxx_linux-64

conda install -y --channel https://roq-trading.com/conda/stable \
    roq-oss-abseil-cpp \
    roq-oss-fmt \
    roq-oss-libunwind \
    roq-oss-spdlog \
    gflags
```


## Building

```bash
git submodule update --init --recursive

cmake .

make -j4

make test
```


## Installing

A pre-compiled binary package can be downloaded from Roq's conda package
repository

```bash
conda install -y --channel https://roq-trading.com/conda/stable \
    roq-logging
```

## Using

```cpp
#include <cstdlib>

#include "roq/logging.h"

void foo(int i) {
  // only log when the ROQ_v environment variable has been set to 1 (or higher)
  VLOG(1)("enter foo")

  // always log
  LOG(INFO)("foo called with i={}", i);

  // conditional log a warning
  LOG_IF(WARNING, i > 10)("value exceeds threshold");

  // terminate the process with a stacktrace
  LOG(FATAL)("something is wrong");
}

int main(int argc, const char *argv[]) {
  roq::Logger::initialize(argv[0]);
  foo(argc);
  roq::Logger::shutdown();
  return EXIT_SUCCESS;
}
```


## Acknowledgements

The interface is somewhat inspired by

* [glog](https://github.com/google/glog) (BSD 3-Clause License)


## License

The project is released under the terms of the MIT license.


## Links

* [Documentation](https://roq-trading.com/docs)
* [Contact us](mailto:info@roq-trading.com)
