#!/usr/bin/env bash

set -e

if [ "$#" -ne 1 ]; then
	echo "usage: unix_build.sh <build_dir_name>"
	echo "<build_dir_name>: Name of the directory created and used to build the project."
	exit 1
fi

BUILD_DIR="$1"
GENERATOR="Ninja"
CURRENT_DIR="$(pwd)"

mkdir -p "$BUILD_DIR" && cd "$BUILD_DIR"
cmake .. -G "$GENERATOR" -DCMAKE_BUILD_TYPE=$CONFIG -DCMAKE_PREFIX_PATH=/usr/local/opt/llvm/ -DTRAVIS_BUILD_NUMBER=$TRAVIS_BUILD_NUMBER
cmake --build ./ --clean-first --target all
cd "$CURRENT_DIR"

