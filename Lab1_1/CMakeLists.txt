cmake_minimum_required(VERSION 3.10)
project(SineSum)

option(USE_DOUBLE "Use double instead of float" OFF)

if(USE_DOUBLE)
    add_definitions(-DUSE_DOUBLE)
endif()

add_executable(sine_sum main.c)
target_link_libraries(sine_sum m)