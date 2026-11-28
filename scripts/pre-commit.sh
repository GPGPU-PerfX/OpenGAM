#!/bin/bash
# Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
# SPDX-License-Identifier: Apache-2.0
#
# Optional Git pre-commit hook for OpenGAM.
# Formats staged first-party C/C++ files with the repository .clang-format
# and re-stages them so the commit includes the formatted result.
# Files under third_party/ are left unchanged.
#
# Install once with:
#   cp scripts/pre-commit.sh .git/hooks/pre-commit && chmod +x .git/hooks/pre-commit
#
# Skip a commit with:
#   git commit --no-verify

set -e
set -o pipefail

root="$(git rev-parse --show-toplevel)"
cd "$root"

if [[ ! -f .clang-format ]]; then
    echo "pre-commit: warning: .clang-format not found at the repository root; skipping format" >&2
    exit 0
fi

if ! command -v clang-format >/dev/null 2>&1; then
    echo "pre-commit: warning: clang-format not found on PATH; skipping format" >&2
    exit 0
fi

is_cxx_source() {
    case "$1" in
        *.c|*.cc|*.cxx|*.cpp|*.c++|*.h|*.hh|*.hpp|*.hxx|*.h++)
            return 0
            ;;
        *)
            return 1
            ;;
    esac
}

formatted=0

# -z / read -d '' walks staged paths safely, including names with spaces.
# --diff-filter=ACMR skips deleted files, which cannot be formatted.
while IFS= read -r -d '' path; do
    case "$path" in
        third_party/*)
            continue
            ;;
    esac

    if ! is_cxx_source "$path"; then
        continue
    fi

    if [[ ! -f "$path" ]]; then
        continue
    fi

    echo "pre-commit: formatting $path"
    clang-format --style=file -i "$path"
    git add -- "$path"
    formatted=$((formatted + 1))
done < <(git diff --cached --name-only --diff-filter=ACMR -z)

if [[ "$formatted" -eq 0 ]]; then
    exit 0
fi

echo "pre-commit: formatted and re-staged $formatted file(s)"
exit 0
