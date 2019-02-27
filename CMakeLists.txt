cmake_minimum_required(VERSION 3.13)
project(PlainTextToVTK C)

set(CMAKE_C_STANDARD 99)

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin )

IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
    SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer" )
ENDIF(CMAKE_BUILD_TYPE MATCHES DEBUG)


ADD_SUBDIRECTORY(src/string)
ADD_SUBDIRECTORY(src/vtk_utils)
ADD_SUBDIRECTORY(src/file_utils)

add_executable(PlainTextToVTK src/main.c)

add_dependencies(PlainTextToVTK string vtk_utils)

target_link_libraries(PlainTextToVTK string vtk_utils file_utils m)

