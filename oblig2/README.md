[![Build Status](https://travis-ci.org/bsamseth/cpp-project.svg?branch=master)](https://travis-ci.org/bsamseth/cpp-project)
[![Coverage Status](https://coveralls.io/repos/github/bsamseth/cpp-project/badge.svg?branch=master)](https://coveralls.io/github/bsamseth/cpp-project?branch=master)

# Boiler plate for C++ projects 

This is a boiler plate for C++ projects. What you get:

- Sources, headers and mains separated in distinct folders
- Access to [Google Tests](https://github.com/google/googletest)
- Use of [CMake](https://cmake.org/) for much easier compiling
- Code documentation with [Doxygen](http://www.stack.nl/~dimitri/doxygen/)
- Continuous testing with [Travis-CI](https://travis-ci.org/)
- Code coverage with [Coveralls.io](https://coveralls.io/)

## Structure
Sources go in [src/](src/), header files in [include/](include/), main programs in [app/](app),
tests go in [tests/](tests/) (compiled in unit_tests.x by default). Due to the magic of CMake, the only thing that ever has to
be changed if you add more files, is to add a target for the executable. Examples of this is in [CMakeLists.txt](CMakeLists.txt).

Ignored files are Emacs backup files, executables (here defined as `.x` extensions) and the `build/` directory.

## Building

Build by making a build directory (i.e. `build/`), run `cmake` in that dir, and then use `make` to build the desired target.

Example:

``` bash
$ mkdir build && cd build
$ cmake .. # argument is location of CMakelists.txt
$ make
$ ./unit_tests.x
```

## Setup
When starting a new project, you probably don't want the history of this repository. To start fresh, with just the files
and no history, you simply delete the `.git/` directory and start a new one:

``` bash
$ rm -rf .git
$ git init
$ git commit -am "Added C++ Boiler Plate"
```

The result is a fresh Git repository with one commit adding all files from the boiler plate. 

## Services

If repository is activated with Travis-CI, then unit tests will be built and executed on each commit.

If repository is activated with Coveralls, then deployment to Travis will also calculate code coverage and
upload this to Coveralls.io. 


