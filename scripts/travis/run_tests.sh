#!/usr/bin/env bash

set -e

if [ "$#" -ne 1 ]; then
	echo "usage: unix_build.sh <build_dir_name>"
	echo "<build_dir_name>: Name of the directory created and used to build the project."
	exit 1
fi

BUILD_DIR_NAME="$1"

cd $BUILD_DIR_NAME
./output/unit_tests
cd -
