cmake_minimum_required(VERSION 3.10)

project(prog)

set(CMAKE_CXX_STANDARD 17)
set(PRJ_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(INC_DIR ${PRJ_DIR}/include)
set(SRC_DIR ${PRJ_DIR}/src)

set(SRCS ${SRC_DIR}/main.cpp
         ${SRC_DIR}/process.cpp)

add_executable(prog.out ${SRCS})

target_include_directories(prog.out PUBLIC ${INC_DIR})
