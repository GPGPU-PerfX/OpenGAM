# OpenGAM contributor guide

OpenGAM (Open-Source GPU Architecture Model) is a public, from-scratch
open-source GPU architecture model. It is licensed under the Apache
License 2.0. This file is the working guide for changes in this
repository: layout, how to build, public ABI rules, and review targets.

## Layout

- `include/opengam.h` — the public C API. All exported functions are
  declared here.
- `include/opengam_typedefs.h` — function-pointer types for loading
  those entry points dynamically (for example with `dlopen`).
- `src/opengam.cpp` — the shared-library implementation of that ABI.
- `opengam.def` — the public export list on Linux (GNU version script).
- `opengam_macos.exports` — the public export list on macOS (ld64
  exported-symbols list). Names use the leading underscore the
  Mach-O linker expects.
- `CMakeLists.txt` — builds `libopengam.so` (soname `libopengam.so.0`)
  on Linux, and `libopengam.dylib` on macOS. Linux and macOS are the
  only supported platforms for now.
- `build.sh` — the developer build entry point.
- `doc/` — project documentation, including the branch and release
  policy in `doc/branch-and-release.md`.

## Building

The supported entry point is `build.sh`, which can be run from any
directory. A fresh build tree uses the Ninja generator when `ninja`
is on PATH; `./build.sh clean` is required to switch generators.

```bash
./build.sh rel        # Release
./build.sh dbg        # Debug
./build.sh dbg_asan   # Debug with AddressSanitizer and UndefinedBehaviorSanitizer
./build.sh rel install
./build.sh clean
```

`install` places headers and the library in `./install` unless an
absolute `install_prefix=<path>` is given, for example
`./build.sh rel install install_prefix=/usr/local`. `./build.sh --help`
lists the same options. Equivalent CMake invocation:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The library is C++17, built with exceptions and RTTI disabled, and
with hidden visibility by default. Only the symbols named in
`opengam.def` or `opengam_macos.exports` are exported.

## Public ABI

- The ABI is a stable C interface. `include/opengam.h` exposes no C++
  types, and C++ exceptions must not cross the ABI boundary.
- Entry points use the `GAMAPI` calling convention, written as
  `GAMresult GAMAPI gamModelGetVersion(int *version)`. `GAMAPI` is
  empty on Linux and macOS.
- The first published entry point is `gamModelGetVersion`. It writes a
  packed version of `(1000 * major + 10 * minor)`. The current model
  version is 0.1, which is 10. A null out-pointer returns
  `GAM_ERROR_INVALID_VALUE`.
- When adding a new public function, declare it in `include/opengam.h`,
  implement it in the library, add its name to the `global:` list in
  `opengam.def` and as `_<function>` in `opengam_macos.exports`, and
  add a versioned `PFN_<function>_v<version>` typedef
  plus a `PFN_<function>` latest-version macro in
  `include/opengam_typedefs.h` in the same change. The `<version>`
  suffix is the packed model version that introduced the signature.
  Do not change the meaning of existing symbols, result codes, or the
  version encoding.
- Public comments, commit messages, and documentation must stay
  self-contained. Do not describe this tree in terms of any private
  project.

## Branches and pull requests

- Ordinary work lands on `develop` after review. `master` is the
  release branch. Do not open ordinary feature pull requests against
  `master`, and do not force-push `develop` or `master`.
- The full policy is in [doc/branch-and-release.md](doc/branch-and-release.md).
- Prefer a clear, minimal change in each pull request. Keep comments
  complete enough that the public header and export list can be read
  without the implementation in hand.
