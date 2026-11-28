#!/usr/bin/env bash
# Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
# SPDX-License-Identifier: Apache-2.0
#
# Run the OpenGAM tests against the existing default build directory.

set -euo pipefail

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
build_dir="$root/build"

usage() {
    cat <<'USAGE'
Usage: ./run_tests.sh

Runs the OpenGAM tests against the existing default build/ tree.
Build first with ./build.sh rel, ./build.sh dbg, or ./build.sh dbg_asan.
USAGE
}

fail() {
    printf 'run_tests.sh: %s\n' "$*" >&2
    exit 2
}

while (($#)); do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
        *)
            fail "unsupported argument: $1 (try ./run_tests.sh --help)"
            ;;
    esac
    shift
done

if [[ ! -d "$build_dir" ]]; then
    fail "no build directory at $build_dir; build first with ./build.sh dbg (or rel / dbg_asan)"
fi

if [[ ! -f "$build_dir/CTestTestfile.cmake" ]]; then
    fail "no tests in $build_dir; build first with ./build.sh dbg (or rel / dbg_asan)"
fi

cmake_path="$(command -v cmake || true)"
if [[ -n "$cmake_path" && -x "${cmake_path%/*}/ctest" ]]; then
    ctest="${cmake_path%/*}/ctest"
else
    command -v ctest >/dev/null 2>&1 || fail "ctest is not on PATH"
    ctest=ctest
fi

# Leak detection is not supported by AddressSanitizer on macOS.
if [[ "$(uname -s)" == Linux ]]; then
    export ASAN_OPTIONS=detect_leaks=1:halt_on_error=1
else
    export ASAN_OPTIONS=halt_on_error=1
fi

export UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1

"$ctest" --test-dir "$build_dir" --output-on-failure --no-tests=error --parallel
