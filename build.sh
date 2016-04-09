#! /bin/bash

if [[ $# -eq 0 ]]; then
    echo "Usage: ./build dir_to_build [run_tests]"
    echo "Script to build the project in a given dir."
    echo "First argument is target dir to build, which "
    echo "is assumed to contain CMakeLists.txt."
    echo "Second arg is to indicate that 'unit_tests.x' should be run."
    echo "Any second argument will trigger tests to be run."
    exit 0
fi

cd "$1"
mkdir -p build && cd build
rm -rf *
cmake ..
make

if [[ $# -gt 1 ]]; then
    ./unit_tests.x
fi

cd ../../