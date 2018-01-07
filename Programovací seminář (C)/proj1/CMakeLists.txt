cmake_minimum_required(VERSION 3.6)
project(IPS_1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c99 -pedantic")

set(SOURCE_FILES tmal.c test_tmal.c)
add_executable(IPS_1 ${SOURCE_FILES})