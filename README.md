# Åtbe but C++

Compiler for the Åtbe programming language written in C++.
A reimagining of [aatbe-lang](https://github.com/chronium/aatbe-lang)

### ! Disclaimer

This is a work in progress and is not ready for use.

## Usage
```bash
Usage: lang [options] INPUT 

Positional arguments:
INPUT           Input file to be compiled [required]

Optional arguments:
-h --help       shows help message and exits [default: false]
-v --version    prints version information and exits [default: false]

```

## Building

### Requirements

- C++20 Compiler
- Meson
- Ninja
- Python 3.8+

### Building

```bash
meson setup build
meson compile -C build
```

## Running Unit tests

Running under Meson, limited output
```bash
meson test -C build
```

Running under Meson, verbose output
```bash
meson test -C build -v
```

Running with GoogleTest, verbose output

Assuming the build dir is `build`
```bash
meson compile -C build
./build/run_tests
```

# Licenses

## Åtbe compiler

The Åtbe compiler is licensed under the MIT License.

[LICENSE](LICENSE)

## argparse

[LICENSE](https://github.com/p-ranav/argparse/blob/c44dde544015f9155bc1d7981e7e9336eb15f9d7/LICENSE)

## GoogleTest

google/googletest is licensed under the

BSD 3-Clause "New" or "Revised" License

[LICENSE](https://github.com/google/googletest/blob/c0e032efe587efa99413dd9ac54c3953a7915588/LICENSE)