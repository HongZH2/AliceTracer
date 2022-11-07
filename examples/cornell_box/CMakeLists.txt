cmake_minimum_required(VERSION 3.0)
project(Cornell_Box)

###############
# Third Parties
###############
# glfw
set(GLFW_INCLUDE ${ALICE_TRACER_ROOT}/third_parties/glfw/include)
set(GLFW_LIB ${ALICE_TRACER_ROOT}/third_parties/glfw/lib/libglfw3.a)

# imgui
set(IMGUI_INCLUDE ${ALICE_TRACER_ROOT}/third_parties/imgui/)
file(GLOB IMGUI_SRC
        ${ALICE_TRACER_ROOT}/third_parties/imgui/*.h
        ${ALICE_TRACER_ROOT}/third_parties/imgui/*.cpp
        ${ALICE_TRACER_ROOT}/third_parties/imgui/backends/imgui_impl_glfw.*
        ${ALICE_TRACER_ROOT}/third_parties/imgui/backends/imgui_impl_opengl3*)
# spdlog
set(SPDLOG_INCLUDE ${ALICE_TRACER_ROOT}/third_parties/spdlog/include)

# opengl
find_library(OPENGL_LIBRARY OpenGL REQUIRED)

#assimp
set(ASSIMP_LIB ${ALICE_TRACER_ROOT}/third_parties/assimp/lib/libassimp.dylib)

###############
# Third Parties
###############
include_directories(
        ${ALICE_TRACER_ROOT}
        ${GLFW_INCLUDE}
        ${IMGUI_INCLUDE}
        ${IMGUI_INCLUDE}/backends/
        ${SPDLOG_INCLUDE}
)

add_executable(${PROJECT_NAME} cornell_box.cpp ${IMGUI_SRC})

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
        ${ASSIMP_LIB}
        Alice_Tracer
)