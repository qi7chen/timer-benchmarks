project(unittest)

set(GTEST_ROOT_DIR ${THIRDPARTY_LIB_DIR}/googletest)

include_directories(
     ${GTEST_ROOT_DIR}
     ${GTEST_ROOT_DIR}/include
)

add_definitions(
    -DGTEST_STDLIB_CXX11=1
    -DGTEST_LANG_CXX11=1
    -DGTEST_HAS_TR1_TUPLE=0
    -DGTEST_HAS_STD_TUPLE_=1
)

file(GLOB PROJECT_HEADER_FILES *.h)
file(GLOB PROJECT_SOURCE_FILES *.cpp)

if (WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")
  add_executable(unittest WIN32 ${PROJECT_HEADER_FILES} ${PROJECT_SOURCE_FILES} ${GTEST_ROOT_DIR}/src/gtest-all.cc)
else()
  add_executable(unittest ${PROJECT_HEADER_FILES} ${PROJECT_SOURCE_FILES} ${GTEST_ROOT_DIR}/src/gtest-all.cc)
endif()

target_link_libraries(unittest libtimers)
