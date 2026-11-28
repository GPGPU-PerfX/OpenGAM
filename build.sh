#!/usr/bin/env bash
# Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
# SPDX-License-Identifier: Apache-2.0
#
# Developer build entry point for OpenGAM.
# Invoke from any directory; paths are resolved from this script's location.

set -euo pipefail

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
build_dir="$root/build"
install_prefix="$root/local-sdk"

usage() {
    cat <<'USAGE'
Usage: ./build.sh <mode> [install] [-- <cmake configure args>...]

Modes
  rel        Release build
  dbg        Debug build
  dbg_asan   Debug build with AddressSanitizer and UndefinedBehaviorSanitizer
  clean      Delete the build directory and exit

Options
  install    After a successful build, install into the prefix
             (default: <source>/local-sdk). Override with
             -- -DCMAKE_INSTALL_PREFIX=<path>
  --         Pass the remaining arguments to the CMake configure step

Examples
  ./build.sh rel
  ./build.sh dbg
  ./build.sh dbg_asan
  ./build.sh rel install
  ./build.sh rel -- -DCMAKE_INSTALL_PREFIX=/usr/local
  ./build.sh clean
USAGE
}

fail() {
    printf 'build.sh: %s\n' "$*" >&2
    exit 2
}

mode=""
do_install=false
extra=()

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
        --)
            shift
            extra=("$@")
            break
            ;;
        *)
            fail "unsupported argument: $1 (try ./build.sh --help)"
            ;;
    esac
    shift
done

[[ -n "$mode" ]] || { usage >&2; exit 2; }

if [[ "$mode" == clean ]]; then
    [[ "$do_install" == false && ${#extra[@]} -eq 0 ]] ||
        fail "clean does not take install or CMake arguments"
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

cmake -S "$root" -B "$build_dir" \
    -DCMAKE_BUILD_TYPE="$build_type" \
    -DCMAKE_INSTALL_PREFIX="$install_prefix" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DGAM_ENABLE_SANITIZERS="$sanitizers" \
    "${extra[@]}"

cmake --build "$build_dir" --parallel

if [[ "$do_install" == true ]]; then
    cmake --install "$build_dir"
    printf 'Installed to %s\n' "$install_prefix"
fi

printf 'Built %s/libopengam.so\n' "$build_dir"
