cmake_minimum_required(VERSION 3.30)
project(ProgTest_03)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g")

add_executable(ProgTest_03 main.cpp)
