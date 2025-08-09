#!/bin/bash
set -e

cd ../test
make

shopt -s nullglob
binaries=(*.out)

if [ ${#binaries[@]} -eq 0 ]; then
    echo "No tests to run."
    exit 0
fi

fail=0
for bin in "${binaries[@]}"; do
    echo "Running $bin..."
    ./"$bin" || { echo "FAIL: $bin FAILED"; fail=1; }
done

if [ $fail -eq 0 ]; then
    echo "OK: All tests passed."
else
    exit 1
fi

