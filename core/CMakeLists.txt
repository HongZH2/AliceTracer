cmake_minimum_required(VERSION 3.23)
project(AliceCore)
set(CMAKE_CXX_STANDARD 17)

set(CORE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB CORE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

include_directories(
        ${ALICE_TRACER_ROOT}
)

add_library(${PROJECT_NAME} ${CORE_SRC})