cmake_minimum_required(VERSION 3.10)
project(Heat2D)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost REQUIRED COMPONENTS program_options)

if (CMAKE_CXX_COMPILER MATCHES "pgcc|pgc\\+\\+")
    message(STATUS "Using PGI compiler")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -acc -Minfo=all")
endif()

add_executable(heat2d single.cpp)

target_link_libraries(heat2d Boost::program_options)

include_directories(${Boost_INCLUDE_DIRS})