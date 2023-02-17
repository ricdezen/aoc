#!/bin/bash

# Necessary for negation in ls pattern.
shopt -s extglob

# Find all executables with no extension.
cd build/bin
files=$(ls -1 20??_day_!(*.*))

# Run each one at a time.
for ex in $files; do
    # Ignore stdout but not stderr.
    ./$ex > /dev/null
    ret=$?
    if [ $ret -ne 0 ]; then
        echo "$ex had an error: $ret."
        exit $ret
    fi
    echo "$ex ran successfully."
done