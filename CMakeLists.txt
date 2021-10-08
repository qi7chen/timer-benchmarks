# Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
# Distributed under the terms and conditions of the Apache License. 
# See accompanying files LICENSE.

cmake_minimum_required(VERSION 3.5)

project (TimerBench)

set (CMAKE_CXX_STANDARD 14)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)


include_directories(src)

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

target_link_libraries(BenchTest  ${CONAN_LIBS} )
