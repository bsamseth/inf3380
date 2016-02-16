cmake_minimum_required(VERSION 2.8)
project(oblig1)
 
enable_language(C)

option(debug "debug" OFF)

if (debug)
     set(CMAKE_C_FLAGS "-Wall -g")
else ()
     set(CMAKE_C_FLAGS "-Wall -O2")
endif()

find_package(MPI)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})


include_directories(
	/usr/include/malloc/malloc.h
	${PROJECT_SOURCE_DIR}/common
	${PROJECT_SOURCE_DIR}/serial
	${PROJECT_SOURCE_DIR}/parallel
	${PROJECT_SOURCE_DIR}/external/simple-jpeg
	)

file(GLOB SERIAL_SOURCES "serial/*.c")
file(GLOB PARALLEL_SOURCES "parallel/*.c")
file(GLOB COMMON_SOURCES "common/*.c")
file(GLOB SIMPLE_JPEG_SOURCES "external/simple-jpeg/*.c")

add_library(
	simple_jpeg
    	${SIMPLE_JPEG_SOURCES}
    	)	

add_library(
	functions
	${COMMON_SOURCES}
	)

 
add_executable(
	denoise_serial
	${SERIAL_SOURCES}
	)

add_executable(
	denoise_parallel
	${PARALLEL_SOURCES}
	)
	
add_dependencies(
	denoise_serial
	simple_jpeg
	functions
	)

add_dependencies(
	denoise_parallel
	simple_jpeg
	functions
	)

target_link_libraries(denoise_serial simple_jpeg functions)

target_link_libraries(denoise_parallel simple_jpeg functions ${MPI_C_LIBRARIES})


add_custom_target(
    delivery
    COMMAND tar cvzf inf3380_oblig1.tar.gz ${PROJECT_SOURCE_DIR}/*
    )
