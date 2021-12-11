# Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
# Distributed under the terms and conditions of the Apache License. 
# See accompanying files LICENSE.

cmake_minimum_required(VERSION 3.0)

project (TimerBench)

set (CMAKE_CXX_STANDARD 11)
set (GTEST_ROOT_DIR 3rd/googletest-1.11.0/googletest)

include_directories(
    src
    ${GTEST_ROOT_DIR}
    ${GTEST_ROOT_DIR}/include
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

add_executable(TimerBench 
    ${LIB_HEADER_FILES} ${LIB_SOURCE_FILES}
    ${TEST_HEADER_FILES} ${TEST_SOURCE_FILES}
    ${GTEST_ROOT_DIR}/src/gtest-all.cc
)

if (UNIX)
    target_link_libraries(TimerBench  pthread)
endif(UNIX)
