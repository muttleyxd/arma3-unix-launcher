#!/usr/bin/env bash
# Builds the unit tests with clang source-based coverage, runs them and prints an llvm-cov report
# for src/common, src/arma3-unix-launcher-library and the app sources covered by the widget tests.
# Run inside the Nix devShell.
#
# Usage: coverage.sh [--min PERCENT FILE...]
#   --min PERCENT FILE...  also fail unless each FILE (path relative to the repo root) has at
#                          least PERCENT line coverage, e.g. --min 80 src/common/mod_search.cpp

set -u

SOURCE_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD_DIR="$SOURCE_DIR/build-coverage"
PROFILE_DIR="$BUILD_DIR/profiles"

MIN_PERCENT=""
REQUIRED_FILES=()
if [ "${1:-}" = "--min" ]; then
    MIN_PERCENT="${2:?--min needs a percentage}"
    shift 2
    REQUIRED_FILES=("$@")
fi

fail()
{
    echo "FAIL: $1"
    exit 1
}

command -v llvm-cov > /dev/null || fail "llvm-cov not found, run inside the devShell (nix develop)"

cmake -S "$SOURCE_DIR" -B "$BUILD_DIR" -DRUN_TESTS=ON -DA3UL_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug > /dev/null ||
    fail "configure"
cmake --build "$BUILD_DIR" --parallel > /dev/null || fail "build"

rm -rf "$PROFILE_DIR"
LLVM_PROFILE_FILE="$PROFILE_DIR/%p-%m.profraw" ctest --test-dir "$BUILD_DIR" --output-on-failure > /dev/null ||
    fail "tests"
llvm-profdata merge -sparse "$PROFILE_DIR"/*.profraw -o "$BUILD_DIR/coverage.profdata" || fail "llvm-profdata"

OBJECTS=()
while IFS= read -r test_binary; do
    OBJECTS+=(-object "$test_binary")
done < <(find "$BUILD_DIR/tests" -type f -perm -u+x -name 'test_unit_*')
[ ${#OBJECTS[@]} -gt 0 ] || fail "no test binaries found"

REPORT="$(llvm-cov report "${OBJECTS[@]:1}" -instr-profile="$BUILD_DIR/coverage.profdata" \
                     "$SOURCE_DIR/src/common" "$SOURCE_DIR/src/arma3-unix-launcher-library" \
                     "$SOURCE_DIR/src/arma3-unix-launcher")" ||
    fail "llvm-cov report"
echo "$REPORT"

STATUS=0
for file in "${REQUIRED_FILES[@]}"; do
    # Report paths are relative to the sources' common prefix (e.g. common/x.cpp for src/common/x.cpp).
    # Columns: Filename Regions Missed Cover Functions Missed Executed Lines Missed Cover ...
    line_cover="$(awk -v f="/$file" '{ n = length($1) }
                                     n > 0 && substr(f, length(f) - n) == "/" $1 { print $10 }' <<< "$REPORT" |
                  head -n 1)"
    if [ -z "$line_cover" ]; then
        echo "FAIL: $file not in the coverage report"
        STATUS=1
    elif awk -v c="${line_cover%\%}" -v m="$MIN_PERCENT" 'BEGIN { exit !(c < m) }'; then
        echo "FAIL: $file line coverage $line_cover < $MIN_PERCENT%"
        STATUS=1
    else
        echo "PASS: $file line coverage $line_cover"
    fi
done
exit $STATUS
