cmake_minimum_required(VERSION 3.23)
project(AliceInterface)

set(INTERFACE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB INTERFACE_SRC ${INTERFACE_DIR}/*.cpp)

# glfw
set(GLFW_INCLUDE ${ALICE_TRACER_ROOT}/third_parties/glfw/include)
set(GLFW_LIB ${ALICE_TRACER_ROOT}/third_parties/glfw/lib/libglfw3.a)

# imgui
set(IMGUI_INCLUDE ${ALICE_TRACER_ROOT}/third_parties/imgui/)

include_directories(
        ${ALICE_TRACER_ROOT}
        ${GLFW_INCLUDE}
        ${IMGUI_INCLUDE}
        ${IMGUI_INCLUDE}/backends/
)

file(GLOB IMGUI_SRC ${ALICE_TRACER_ROOT}/third_parties/imgui/*.h
            ${ALICE_TRACER_ROOT}/third_parties/imgui/*.cpp
            ${ALICE_TRACER_ROOT}/third_parties/imgui/backends/imgui_impl_glfw.*
            ${ALICE_TRACER_ROOT}/third_parties/imgui/backends/imgui_impl_opengl3.*)

add_library(
        ${PROJECT_NAME}
        ${INTERFACE_SRC}
        ${IMGUI_SRC}
)
