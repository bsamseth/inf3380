cmake_minimum_required(VERSION 2.8)
project(matrixMultiply)

enable_language(C)
find_package(MPI)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

option(debug "Use debugging flag" ON)
option(coverage "Use coverage flag" OFF)

set(CMAKE_C_FLAGS "-Wall -std=c99")

if (debug)
     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
     if (coverage)
          set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
     endif()
else ()
     set(CMAKE_C_FLAGS "-Wall -O2")
endif()


include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/tests
    )

file(GLOB SOURCES "src/*.c")
file(GLOB TESTFILES "tests/*.c")

add_library(engine ${SOURCES})

add_executable(main.x app/main.c)
target_link_libraries(main.x engine ${MPI_C_LIBRARIES})

add_executable(unit_tests.x ${TESTFILES})
target_link_libraries(unit_tests.x engine)


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
