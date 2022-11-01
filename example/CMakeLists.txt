cmake_minimum_required(VERSION 3.0)
project(Example)

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

add_executable(${PROJECT_NAME} main.cpp)

find_library(OPENGL_LIBRARY OpenGL REQUIRED)
find_library(COCOA_LIBRARY Cocoa REQUIRED)
find_library(IOKIT_LIBRARY IOKit REQUIRED)
find_library(COREVID_LIBRARY CoreVideo REQUIRED)

target_link_libraries(
        ${PROJECT_NAME}
        ${OPENGL_LIBRARY}
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVID_LIBRARY}
        ${GLFW_LIB}
        AliceCore
        AliceInterface
)