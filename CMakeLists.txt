cmake_minimum_required(VERSION 3.23)
project(Alice_Tracer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/libs/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(ALICE_TRACER_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(utils)
add_subdirectory(core)
add_subdirectory(interface)
add_subdirectory(example)