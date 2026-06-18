# libabseil - Abseil C++ Common Libraries

This is a `build2` package for the
[`abseil-cpp`](https://github.com/abseil/abseil-cpp) C++ library. Abseil is an
open-source collection of C++ library code designed to augment the C++ standard
library.

## Usage

To start using `libabseil` in your project, add the following `depends`
value to your `manifest`, adjusting the version constraint as appropriate:

```
depends: libabseil ^2025.127.2
```

Then import the library in your `buildfile`:

```
import libs = libabseil%lib{abseil}
```

## Importable targets

This package provides the following importable targets:

```
lib{abseil}
```

