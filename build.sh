#!/usr/bin/env bash
# Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
# SPDX-License-Identifier: Apache-2.0
#
# Developer build entry point for OpenGAM.
# Invoke from any directory; paths are resolved from this script's location.

set -euo pipefail

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
build_dir="$root/build"
install_prefix="$root/install"

usage() {
    cat <<'USAGE'
Usage: ./build.sh <mode> [install] [install_prefix=<absolute path>]

A fresh build tree uses the Ninja generator when `ninja` is on PATH.
An existing CMake cache keeps the generator it was created with;
run ./build.sh clean to switch.

Modes
  rel        Release build
  dbg        Debug build
  dbg_asan   Debug build with AddressSanitizer and UndefinedBehaviorSanitizer
  clean      Delete the build directory and exit

Options
  install                  After a successful build, install into the prefix
                           (default: <source>/install)
  install_prefix=<path>    Override the install prefix. The path must be
                           absolute.

Arguments may appear in any order.

Examples
  ./build.sh rel
  ./build.sh dbg
  ./build.sh dbg_asan
  ./build.sh rel install
  ./build.sh dbg install install_prefix=/usr/local
  ./build.sh clean
USAGE
}

fail() {
    printf 'build.sh: %s\n' "$*" >&2
    exit 2
}

mode=""
do_install=false

while (($#)); do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
        rel|dbg|dbg_asan|clean)
            [[ -z "$mode" ]] || fail "choose exactly one of rel, dbg, dbg_asan, or clean"
            mode="$1"
            ;;
        install)
            do_install=true
            ;;
        install_prefix=*)
            install_prefix="${1#install_prefix=}"
            [[ -n "$install_prefix" ]] || fail "install_prefix is empty"
            [[ "$install_prefix" == /* ]] || fail "install_prefix must be an absolute path"
            ;;
        --)
            fail "CMake passthrough after -- is not supported; use install_prefix=<absolute path>"
            ;;
        *)
            fail "unsupported argument: $1 (try ./build.sh --help)"
            ;;
    esac
    shift
done

[[ -n "$mode" ]] || { usage >&2; exit 2; }

if [[ "$mode" == clean ]]; then
    [[ "$do_install" == false && "$install_prefix" == "$root/install" ]] ||
        fail "clean does not take install or install_prefix"
    rm -rf -- "$build_dir"
    printf 'Removed %s\n' "$build_dir"
    exit 0
fi

command -v cmake >/dev/null 2>&1 || fail "cmake is not on PATH"

build_type=Debug
sanitizers=OFF
case "$mode" in
    rel) build_type=Release ;;
    dbg_asan) sanitizers=ON ;;
esac

# Prefer Ninja on a fresh tree. Do not pass -G when a cache already
# exists, because CMake will refuse to change the generator in place.
generator_args=()
if [[ ! -f "$build_dir/CMakeCache.txt" ]] && command -v ninja >/dev/null 2>&1; then
    generator_args=(-G Ninja)
fi

cmake -S "$root" -B "$build_dir" \
    "${generator_args[@]}" \
    -DCMAKE_BUILD_TYPE="$build_type" \
    -DCMAKE_INSTALL_PREFIX="$install_prefix" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DGAM_ENABLE_SANITIZERS="$sanitizers"

cmake --build "$build_dir" --parallel

if [[ "$do_install" == true ]]; then
    cmake --install "$build_dir"
    printf 'Installed to %s\n' "$install_prefix"
fi

printf 'Built %s/libopengam.so\n' "$build_dir"
