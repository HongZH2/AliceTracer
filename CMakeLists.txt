cmake_minimum_required(VERSION 3.23)
project(Alice_Tracer)
set(CMAKE_CXX_STANDARD 17)

set(alice_tracer ${CMAKE_CURRENT_SOURCE_DIR})
set(utils_src ${CMAKE_CURRENT_SOURCE_DIR}/utils/src)

include_directories(
        ${alice_tracer}
)

add_executable(AliceTracer main.cpp utils/include/alice_math.h core/include/camera.h core/include/color.h utils/include/alice_file.h core/include/tone_mapping.h core/include/ray.h utils/include/alice_image.h core/include/brdf.h core/include/material.h core/include/intersection_solver.h utils/include/alice_threads.h utils/include/alice_common.h)
