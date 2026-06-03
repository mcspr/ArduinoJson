#!/usr/bin/env bash

set -e -x

root=$(git rev-parse --show-toplevel)

board=${1:?specify arduino board name}

directories=$(find "$root"/examples -maxdepth 1 -mindepth 1 -type d)
for dir in $directories ; do
  name=$(basename "$dir")
  arduino-cli compile \
    --library . \
    --warnings all \
    --fqbn "$board" \
    "$dir/${name}.ino"
done
