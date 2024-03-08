# CPM - A C/C++ Package Manager

CPM is a **C/C++** **P**ackage **M**anager made for my diploma thesis.

## How to install

Clone the repository and execute the following commands:

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
