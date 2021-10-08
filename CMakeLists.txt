# Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
# Distributed under the terms and conditions of the Apache License. 
# See accompanying files LICENSE.

cmake_minimum_required(VERSION 3.5)

set (CMAKE_CXX_STANDARD 11)

### Hunter snapshot that will be used ###
include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.111.tar.gz"
    SHA1 "4189bbd869abe4eb69b5303d3f916c03d34dc154"
)

project(TimerBench)

### Download dependencies ###
hunter_add_package(glog)
hunter_add_package(benchmark)
hunter_add_package(GTest)

### Find dependencies ###
find_package(glog CONFIG REQUIRED)
find_package(benchmark CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
    src
)

if (WIN32)
    add_definitions(
        -DNOMINMAX
        -DWIN32_LEAN_AND_MEAN
        -D_WIN32_WINNT=0x0600
        -D_CRT_SECURE_NO_WARNINGS
        -D_SCL_SECURE_NO_WARNINGS
        -D_WINSOCK_DEPRECATED_NO_WARNINGS
    )
endif()

file(GLOB_RECURSE LIB_HEADER_FILES src/*.h)
file(GLOB_RECURSE LIB_SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE TEST_SOURCE_FILES test/*.cpp)

add_executable(BenchTest 
    ${LIB_HEADER_FILES} ${LIB_SOURCE_FILES}
    ${TEST_HEADER_FILES} ${TEST_SOURCE_FILES}
)

target_link_libraries(BenchTest 
    glog::glog
    benchmark::benchmark
    GTest::gtest
)
