cmake_minimum_required(VERSION 3.0)
project(PlayGround)

###############
# Third Parties
###############
# glfw
set(GLFW_INCLUDE ${ALICE_TRACER_ROOT}/third_parties/glfw/include)
set(GLFW_LIB ${ALICE_TRACER_ROOT}/third_parties/glfw/libs/libglfw3.a)
# imgui
set(IMGUI_INCLUDE ${ALICE_TRACER_ROOT}/third_parties/imgui/)
file(GLOB IMGUI_SRC
        ${ALICE_TRACER_ROOT}/third_parties/imgui/*.h
        ${ALICE_TRACER_ROOT}/third_parties/imgui/*.cpp
        ${ALICE_TRACER_ROOT}/third_parties/imgui/backends/imgui_impl_glfw.*
        ${ALICE_TRACER_ROOT}/third_parties/imgui/backends/imgui_impl_opengl3*)
# spdlog
set(SPDLOG_INCLUDE ${ALICE_TRACER_ROOT}/third_parties/spdlog/include)

###############
# Third Parties
###############

# find OpenGL
find_package(OpenGL REQUIRED) # opengl

include_directories(
        ${ALICE_TRACER_ROOT}
        ${GLFW_INCLUDE}
        ${IMGUI_INCLUDE}
        ${IMGUI_INCLUDE}/backends/
        ${SPDLOG_INCLUDE}
        ${OPENGL_INCLUDE_DIR}
)

add_executable(${PROJECT_NAME} playground.cpp ${IMGUI_SRC})

if(APPLE)
        #assimp
        set(ASSIMP_LIB ${ALICE_TRACER_ROOT}/third_parties/assimp/mac/lib/libassimp.dylib)
        include_directories(${ALICE_TRACER_ROOT}/third_parties/assimp/mac/include)

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
elseif(UNIX)
        #assimp
        set(ASSIMP_LIB ${ALICE_TRACER_ROOT}/third_parties/assimp/linux/lib/libassimp.so)
        include_directories(${ALICE_TRACER_ROOT}/third_parties/assimp/linux/include)

        set(THREADS_PREFER_PTHREAD_FLAG ON)
        find_package(Threads REQUIRED)

        target_link_libraries(
                ${PROJECT_NAME}
                ${CMAKE_DL_LIBS}
                Threads::Threads
                ${OPENGL_LIBRARY}
                ${GLFW_LIB}
                ${ASSIMP_LIB}
                Alice_Tracer
        )
endif(APPLE)