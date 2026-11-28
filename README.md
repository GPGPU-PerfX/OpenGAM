# OpenGAM

OpenGAM (Open-Source GPU Architecture Model) is an open-source GPU architecture
model. It is licensed under the Apache License 2.0.

The public C API is declared in [`include/opengam.h`](include/opengam.h). The
first exported entry point is `gamModelGetVersion`. Function-pointer types
for loading the library dynamically (for example with `dlopen`) are in
[`include/opengam_typedefs.h`](include/opengam_typedefs.h). Building the tree
produces the shared library `libopengam.so`.

## Branches

- `develop` is the default development branch and the default pull-request
  review target. Ordinary feature work lands here after review.
- `master` is the release / published-push branch. Published history on
  `master` is not rewritten. Ordinary feature PRs must not target `master`.

Releases are a periodic, explicit port of `develop` onto `master`. There is
no automatic merge from `develop` to `master`. See
[doc/branch-and-release.md](doc/branch-and-release.md).

## Building

```bash
./build.sh rel
```

This configures a Release build and produces `build/libopengam.so`. Other
modes:

```bash
./build.sh dbg              # Debug
./build.sh dbg_asan         # Debug with AddressSanitizer and UndefinedBehaviorSanitizer
./build.sh rel install      # Build and install to ./install
./build.sh clean            # Remove the build directory
```

To install somewhere other than `./install`, pass an absolute
`install_prefix`:

```bash
./build.sh rel install install_prefix=/usr/local
```

`./build.sh --help` lists the same options. Arguments may appear in any
order. You can also invoke CMake directly:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Contributing

See [AGENTS.md](AGENTS.md) for the repository layout, public ABI rules, and
the expected pull-request target.
