cmake_minimum_required(VERSION 2.8)
project(PROJECT_NAME)
 
enable_language(C CXX)

option(debug "Use debugging flag" ON)
option(coverage "Use coverage flag" OFF)

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall")

if (debug)
     set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")
     if (coverage)
          set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
     endif()
else ()
     set(CMAKE_C_FLAGS "-Wall -O2")
endif()


include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/tests
    ${PROJECT_SOURCE_DIR}/external/googletest
    ${PROJECT_SOURCE_DIR}/external/googletest/include
    )

file(GLOB SOURCES "src/*.cpp")
file(GLOB TESTFILES "tests/*.cpp")

add_library(
    engine
    ${SOURCES}
    )

add_custom_target(
    git_update
    COMMAND git submodule init
    COMMAND git submodule update
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )

add_library(
    gtest
    ${PROJECT_SOURCE_DIR}/external/googletest/src/gtest-all.cc
    ${PROJECT_SOURCE_DIR}/external/googletest/src/gtest_main.cc
    )

add_dependencies(gtest git_update)

set_source_files_properties(${PROJECT_SOURCE_DIR}/external/googletest/src/gtest-all.cc  PROPERTIES GENERATED 1)
set_source_files_properties(${PROJECT_SOURCE_DIR}/external/googletest/src/gtest_main.cc PROPERTIES GENERATED 1)

add_executable(
    unit_tests.x
    ${TESTFILES}
    )

add_dependencies(unit_tests.x gtest)

target_link_libraries(
    unit_tests.x
    gtest
    engine
    pthread
    )

 
add_executable(main.x app/main.cpp)

add_dependencies(main.x engine)

target_link_libraries(main.x engine)


add_custom_target(
    coverage
    COMMAND cd ${PROJECT_SOURCE_DIR}/build
    COMMAND lcov --directory ${PROJECT_SOURCE_DIR}/build --capture --output-file coverage.info # capture coverage info
    COMMAND lcov --remove coverage.info 'tests/*' 'app/*' 'external/*' 'cmake/*' '/usr/*' --output-file coverage.info 
    COMMAND lcov --list coverage.info
    COMMAND genhtml coverage.info
    COMMAND google-chrome-stable index.html&
    COMMAND cd -
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )

add_dependencies(coverage unit_tests.x)

include(CTest)
enable_testing()

add_test(unit ${PROJECT_BINARY_DIR}/unit_tests)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(doc ALL
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile &> doxygen.log
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)
