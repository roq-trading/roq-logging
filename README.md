# roq-logging

roq-logging is a utility library to provide a glog-like interface on top
of spdlog.


## Operating Systems

* Linux


## Prerequisites

The library is designed to be compatible with the conda package manager.

### Download Miniconda

```
wget -N https://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh
```

### Install Miniconda

```
bash Miniconda3-latest-Linux-x86_64.sh -b -u -p ~/miniconda3
```

### Activate Miniconda

```
source ~/miniconda3/bin/activate
```

### Install Toolchain

```
conda install -y git cmake gxx_linux-64 gdb_linux-64
```

### Install Package Dependencies

```
conda install -y \
    --channel https://roq-trading.com/conda/stable \
    roq-oss-abseil-cpp \
    roq-oss-fmt \
    roq-oss-spdlog \
    gflags
```


## Building

### Update Git Submodules

```
git submodule update --init --recursive
```

### Create Makefile

The default build system is CMake

```
cmake \
    -DCMAKE_AR="$AR" \
    -DCMAKE_RANLIB="$RANLIB" \
    -DCMAKE_NM="$NM" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTING=ON \
    .
```

### Compile

```
make -j4
```


### Test

```
make test
```


## Installing

```
conda install -y \
    --channel https://roq-trading.com/conda/stable \
    roq-logging
```

## Using

The interface has been inspired by glog and fmt

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
```


## License

The library is released under the terms of the MIT license.


## Links

* [Documentation](https://roq-trading.com/docs)
* [Contact us](mailto:info@roq-trading.com)
