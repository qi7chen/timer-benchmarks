# Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
# Distributed under the terms and conditions of the Apache License. 
# See accompanying files LICENSE.

project(unittest)

file(GLOB PROJECT_HEADER_FILES *.h)
file(GLOB PROJECT_SOURCE_FILES *.cpp)

find_package(glog CONFIG REQUIRED)
find_package(benchmark CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_executable(unittest ${PROJECT_HEADER_FILES} ${PROJECT_SOURCE_FILES} )

target_link_libraries(unittest 
    libtimers
    glog::glog
    benchmark::benchmark
     GTest::gtest
)
