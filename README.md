# CPM - A C/C++ Package Manager

CPM is a **C/C++** **P**ackage **M**anager made for my diploma thesis.

## Installation

### Windows and Linux

Check out the latest release [here](https://github.com/YassenEfremov/cpm/releases/latest) for binaries, installers and tarballs.


### Build from source

To build the project you need [CMake](https://cmake.org/) and [ninja](https://ninja-build.org/).

After that clone the repository and execute the following commands:

```sh
cd cpm
git submodule update --init --progress
mkdir build
cd build
cmake .. -G Ninja
ninja
sudo ninja install
```

## Available packages

The packages that can be installed with `cpm` can be found [here](https://github.com/orgs/cpm-examples/repositories).
