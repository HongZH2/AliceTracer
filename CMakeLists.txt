cmake_minimum_required(VERSION 3.20)
project(Alice_Tracer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/libs/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(ALICE_TRACER_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

###############
# Third Parties
###############
# glfw
set(GLFW_INCLUDE ${ALICE_TRACER_ROOT}/third_parties/glfw/include)

# imgui
set(IMGUI_INCLUDE ${ALICE_TRACER_ROOT}/third_parties/imgui/)

# spdlog
set(SPDLOG_INCLUDE ${ALICE_TRACER_ROOT}/third_parties/spdlog/include)

# opengl
find_library(OPENGL_LIBRARY OpenGL REQUIRED)

# assimp
set(ASSIMP_INCLUDE ${ALICE_TRACER_ROOT}/third_parties/assimp/include)

###############
# Alice Tracer
###############
# Alice Tracer include
include_directories(
        ${ALICE_TRACER_ROOT}
        ${IMGUI_INCLUDE}
        ${IMGUI_INCLUDE}/backends/
        ${GLFW_INCLUDE}
        ${SPDLOG_INCLUDE}
        ${OPENGL_INCLUDE_DIR}
        ${ASSIMP_INCLUDE})

# Alice Tracer sources
file(GLOB ALICE_TRACER_SRC
        ${ALICE_TRACER_ROOT}/core/src/*.cpp
        ${ALICE_TRACER_ROOT}/interface/src/*.cpp)

message(STATUS ${ALICE_TRACER_SRC})

add_library(${PROJECT_NAME}
        ${ALICE_TRACER_SRC}
        )

add_subdirectory(examples)