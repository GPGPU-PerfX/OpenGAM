#!/usr/bin/env bash
# Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
# SPDX-License-Identifier: Apache-2.0
#
# Developer build entry point for OpenGAM.
# Invoke from any directory; paths are resolved from this script's location.

set -euo pipefail

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
build_dir="$root/build"
prefix="$root/install"

usage() {
    cat <<'USAGE'
Usage: ./build.sh <mode> [test] [install] [--prefix=<absolute path>]

A fresh build tree uses the Ninja generator when `ninja` is on PATH.
An existing CMake cache keeps the generator it was created with;
run ./build.sh clean to switch.

Modes
  rel        Release build
  dbg        Debug build
  dbg_asan   Debug build with AddressSanitizer and UndefinedBehaviorSanitizer
  clean      Delete the build directory and the default ./install
             directory, then exit

Options
  test                     After a successful build, run ./run_tests.sh
  install                  After a successful build and passing tests,
                           install into the prefix
                           (default: <source>/install)
  --prefix=<path>          Override the install prefix. The path must be
                           absolute.

Arguments may appear in any order. ./run_tests.sh also runs the tests
against an existing build/ tree without rebuilding.

Examples
  ./build.sh rel
  ./build.sh dbg
  ./build.sh dbg_asan
  ./build.sh dbg test
  ./build.sh rel install
  ./build.sh dbg install --prefix=/usr/local
  ./build.sh clean
USAGE
}

fail() {
    printf 'build.sh: %s\n' "$*" >&2
    exit 2
}

mode=""
do_install=false
do_test=false

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
        test)
            do_test=true
            ;;
        install)
            do_install=true
            ;;
        --prefix=*)
            prefix="${1#--prefix=}"
            [[ -n "$prefix" ]] || fail "--prefix is empty"
            [[ "$prefix" == /* ]] || fail "--prefix must be an absolute path"
            ;;
        *)
            fail "unsupported argument: $1 (try ./build.sh --help)"
            ;;
    esac
    shift
done

[[ -n "$mode" ]] || { usage >&2; exit 2; }

if [[ "$mode" == clean ]]; then
    [[ "$do_install" == false && "$do_test" == false && "$prefix" == "$root/install" ]] ||
        fail "clean does not take test, install, or --prefix"
    default_install="$root/install"
    rm -rf -- "$build_dir"
    printf 'Removed %s\n' "$build_dir"
    if [[ -e "$default_install" || -L "$default_install" ]]; then
        rm -rf -- "$default_install"
        printf 'Removed %s\n' "$default_install"
    fi
    exit 0
fi

case "$(uname -s)" in
    Linux|Darwin) ;;
    *) fail "OpenGAM currently supports Linux and macOS only" ;;
esac

command -v cmake >/dev/null 2>&1 || fail "cmake is not on PATH"

build_type=Debug
sanitizers=OFF
case "$mode" in
    rel) build_type=Release ;;
    dbg_asan) sanitizers=ON ;;
esac

# Prefer Ninja on a fresh tree. Do not pass -G when a cache already
# exists, because CMake will refuse to change the generator in place.
# The cmake argv is kept in one non-empty array: expanding an empty
# array with "${arr[@]}" is an unbound-variable error on Bash 3.2
# (the macOS system shell) when set -u is on.
cmake_args=(
    -S "$root"
    -B "$build_dir"
    -DCMAKE_BUILD_TYPE="$build_type"
    -DCMAKE_INSTALL_PREFIX="$prefix"
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    -DOPENGAM_ENABLE_SANITIZERS="$sanitizers"
)
if [[ ! -f "$build_dir/CMakeCache.txt" ]] && command -v ninja >/dev/null 2>&1; then
    cmake_args+=(-G Ninja)
fi
cmake "${cmake_args[@]}"

cmake --build "$build_dir" --parallel

# install implies a passing test run first. GoogleTest is not installed.
if [[ "$do_test" == true || "$do_install" == true ]]; then
    "$root/run_tests.sh"
fi

if [[ "$do_install" == true ]]; then
    cmake --install "$build_dir"
    printf 'Installed to %s\n' "$prefix"
fi

if [[ "$(uname -s)" == Darwin ]]; then
    printf 'Built %s/libopengam.dylib\n' "$build_dir"
else
    printf 'Built %s/libopengam.so\n' "$build_dir"
fi
