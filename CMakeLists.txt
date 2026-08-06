# Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
# Distributed under the terms and conditions of the Apache License.
# See accompanying files LICENSE.

cmake_minimum_required(VERSION 3.0)

project (TimerBench)

set (CMAKE_CXX_STANDARD 11)
set (GOOGLETEST_PATH ${CMAKE_SOURCE_DIR}/3rd/googletest-1.12.1/googletest)
set (GTEST_ROOT_DIR ${GOOGLETEST_PATH})
set (GBENCH_ROOT_DIR ${CMAKE_SOURCE_DIR}/3rd/benchmark-1.8.2)

# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack_size,16777216")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,stack-size=8192")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -fsanitize=address")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fsanitize=address")

include_directories(
    src
    ${GTEST_ROOT_DIR}
    ${GTEST_ROOT_DIR}/include
    ${GBENCH_ROOT_DIR}/include
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

add_subdirectory(${GBENCH_ROOT_DIR})

add_executable(TimerBench
    ${LIB_HEADER_FILES} ${LIB_SOURCE_FILES}
    ${TEST_HEADER_FILES} ${TEST_SOURCE_FILES}
    ${GTEST_ROOT_DIR}/src/gtest-all.cc
)

target_link_libraries(TimerBench benchmark::benchmark)

if (UNIX)
    target_link_libraries(TimerBench  pthread)
endif(UNIX)
