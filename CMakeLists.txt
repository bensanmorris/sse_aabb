cmake_minimum_required(VERSION 3.5)

project(sse2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(SYSTEM .)
add_executable(sse2 main.cpp)
