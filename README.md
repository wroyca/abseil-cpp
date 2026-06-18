# abseil-cpp - Abseil C++ Common Libraries

This is a `build2` package repository for
[`abseil-cpp`](https://github.com/abseil/abseil-cpp), a collection of C++
library code designed to augment the C++ standard library.

This file contains setup instructions and other details that are more
appropriate for development rather than consumption. If you want to use
`abseil-cpp` in your `build2`-based project, then instead see the accompanying
[`PACKAGE-README.md`](libabseil/PACKAGE-README.md) file.

The development setup for `abseil-cpp` uses the standard `bdep`-based workflow.
For example:

```
git clone --recurse-submodules .../abseil-cpp.git
cd abseil-cpp

bdep init -C @gcc cc config.cxx=g++
bdep update
bdep test
```
