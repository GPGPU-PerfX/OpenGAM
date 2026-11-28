# OpenGAM

OpenGAM (Open-Source GPU Architecture Model) is an open-source GPU architecture
model. It is licensed under the Apache License 2.0.

The public C API is declared in [`include/opengam.h`](include/opengam.h). The
first exported entry point is `gamModelGetVersion`. Function-pointer types
for loading the library dynamically (for example with `dlopen`) are in
[`include/opengam_typedefs.h`](include/opengam_typedefs.h). Building the tree
produces the shared library `libopengam.so` on Linux, or `libopengam.dylib` on macOS.
Linux and macOS are the supported platforms.

## Branches

- `develop` is the default development branch and the default pull-request
  review target. Ordinary feature work lands here after review.
- `master` is the release / published-push branch. Published history on
  `master` is not rewritten. Ordinary feature PRs must not target `master`.

Releases are a periodic, explicit port of `develop` onto `master`. There is
no automatic merge from `develop` to `master`. See
[doc/branch-and-release.md](doc/branch-and-release.md).

## Dependencies

- CMake 3.16 or newer
- A C++17 compiler (GCC or Clang)
- Ninja, if you want the default generator (`build.sh` uses Ninja when
  `ninja` is on PATH)
- The official GoogleTest v1.17.0 release, checked out as a git submodule
  at `third_party/googletest`

On Ubuntu:

```bash
sudo apt-get install cmake g++ ninja-build
```

On macOS, install the Xcode Command Line Tools for the compiler, and
CMake and Ninja (for example with Homebrew: `brew install cmake ninja`).

Clone the repository with submodules:

```bash
git clone --recurse-submodules https://github.com/GPGPU-PerfX/OpenGAM.git
```

If the tree was cloned without submodules, initialize them before
configuring a build:

```bash
git submodule update --init
```

## Building

```bash
./build.sh rel
```

This configures a Release build and produces `build/libopengam.so` on Linux, or `build/libopengam.dylib` on macOS. A
fresh build tree uses Ninja when `ninja` is on PATH. GoogleTest is
compiled into the CMake build directory (`build/googletest` by default),
not into `third_party/`. Other modes:

```bash
./build.sh dbg              # Debug
./build.sh dbg_asan         # Debug with AddressSanitizer and UndefinedBehaviorSanitizer
./build.sh rel install      # Build, test, and install to ./install
./build.sh clean            # Remove the build directory and ./install
```

To install somewhere other than `./install`, pass an absolute
`--prefix`:

```bash
./build.sh rel install --prefix=/usr/local
```

`./build.sh install` does not install GoogleTest. `./build.sh --help`
lists the same options. Arguments may appear in any order. You can also
invoke CMake directly:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Testing

Build first, then run the tests against that `build/` tree:

```bash
./build.sh dbg
./run_tests.sh
```

The same sequence works after `./build.sh rel` or `./build.sh dbg_asan`.
`./run_tests.sh` does not configure or rebuild. If `build/` is missing
or has no tests, it exits with a short message to build first.
`./build.sh <mode> check` is a shorthand that builds and then runs
`./run_tests.sh`. `./build.sh <mode> install` runs the tests before
installing.

## Continuous integration

Pushes and pull requests to `develop` and `master` run GitHub Actions
on Ubuntu 24.04 and macos-latest. The workflow installs the dependencies
above, checks out the GoogleTest submodule, and runs `./build.sh rel`,
`./build.sh dbg`, and `./build.sh dbg_asan`, each followed by
`./run_tests.sh`.

## Contributing

See [AGENTS.md](AGENTS.md) for the repository layout, public ABI rules, and
the expected pull-request target.
