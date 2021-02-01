# roq-logging

Logging utilities.


## Operating Systems

* Linux


## Library/Package Dependencies

* [fmt](https://github.com/fmtlib/fmt) (MIT License)
* [spdlog](https://github.com/gabime/spdlog) (MIT License)
* [Abseil](https://github.com/abseil/abseil-cpp) (Apache 2.0 License)
* [libunwind](https://github.com/libunwind/libunwind) (MIT License)

Optional

* [Google Test](https://github.com/google/googletest) (BSD 3-Clause License)


## Prerequisites

The project is designed to be compatible with the conda package manager.

```bash
wget -N https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-x86_64.sh

bash Miniforge3-Linux-x86_64.sh -b -u -p ~/conda

source ~/conda/bin/activate

conda install -y \
    git \
    cmake \
    gxx_linux-64 \
    abseil-cpp \
    fmt \
    spdlog

conda install -y --channel https://roq-trading.com/conda/stable \
    roq-oss-libunwind
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

* [Roq GmbH](https://roq-trading.com/)
* [Documentation](https://roq-trading.com/docs/)
* [Contact us](mailto:info@roq-trading.com)
