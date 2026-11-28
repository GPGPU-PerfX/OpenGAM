#!/usr/bin/env bash
# Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
# SPDX-License-Identifier: Apache-2.0
#
# Run the OpenGAM tests against the existing default build directory.

set -euo pipefail

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
build_dir="$root/build"
unittests="$build_dir/gamUnittests"

usage() {
    cat <<'USAGE'
Usage: ./run_tests.sh [--gtest_filter=<pattern>] [gtest options...]

Runs build/gamUnittests against the existing default build/ tree.
Build first with ./build.sh rel, ./build.sh dbg, or ./build.sh dbg_asan.

Extra arguments are passed to the GoogleTest binary, for example:

  ./run_tests.sh --gtest_filter=ModelGetVersionTest.*
USAGE
}

fail() {
    printf 'run_tests.sh: %s\n' "$*" >&2
    exit 2
}

if [[ "${1:-}" == -h || "${1:-}" == --help ]]; then
    usage
    exit 0
fi

if [[ ! -d "$build_dir" ]]; then
    fail "no build directory at $build_dir; build first with ./build.sh dbg (or rel / dbg_asan)"
fi

if [[ ! -x "$unittests" ]]; then
    fail "no gamUnittests in $build_dir; build first with ./build.sh dbg (or rel / dbg_asan)"
fi

# Leak detection is not supported by AddressSanitizer on macOS.
if [[ "$(uname -s)" == Linux ]]; then
    export ASAN_OPTIONS=detect_leaks=1:halt_on_error=1
else
    export ASAN_OPTIONS=halt_on_error=1
fi

export UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1

"$unittests" "$@"
