cmake_minimum_required(VERSION 3.5)
project(sse2 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(picobench
    GIT_REPOSITORY https://github.com/iboB/picobench.git
)
if(NOT picobench_POPULATED)
    FetchContent_Populate(picobench)
    set(PICOBENCH_SRC_ROOT ${picobench_SOURCE_DIR}/include)
endif()

include_directories(${PICOBENCH_SRC_ROOT} SYSTEM .)
add_executable(sse2 main.cpp)
