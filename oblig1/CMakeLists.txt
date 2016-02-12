cmake_minimum_required(VERSION 2.8)
project(oblig1)
 
enable_language(C)

set(CMAKE_C_FLAGS "-Wall -O2")

include_directories(
	/usr/include/malloc/malloc.h
	${PROJECT_SOURCE_DIR}/serial
	${PROJECT_SOURCE_DIR}/serial/src
	${PROJECT_SOURCE_DIR}/serial/include
	${PROJECT_SOURCE_DIR}/parallel
	${PROJECT_SOURCE_DIR}/external/simple-jpeg
	)

file(GLOB SERIAL_SOURCES "serial/src/*.c")
file(GLOB SIMPLE_JPEG_SOURCES "external/simple-jpeg/*.c")

add_library(
    simple_jpeg
    ${SIMPLE_JPEG_SOURCES}
    )


 
add_executable(
	denoise_serial
	${SERIAL_SOURCES}
	)	

add_dependencies(
	denoise_serial
	simple_jpeg
	)

target_link_libraries(denoise_serial simple_jpeg)
