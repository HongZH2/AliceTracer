cmake_minimum_required(VERSION 3.23)
project(Example)

include_directories(
        ${ALICE_TRACER_ROOT}
)
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(
        ${PROJECT_NAME}
        AliceCore
)