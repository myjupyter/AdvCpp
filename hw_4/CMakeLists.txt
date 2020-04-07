cmake_minimum_required(VERSION 3.17)

project(prog)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -o3")

set(PRJ_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(INC_DIR ${PRJ_DIR}/include)
set(SRC_DIR ${PRJ_DIR}/src)
set(LIB_DIR ${PRJ_DIR}/lib)
set(EXM_DIR ${PRJ_DIR}/example)

set(LIB_NAME   Network)
set(SERVER_OUT server.out)

add_subdirectory(${LIB_DIR})

set(SERVER_SRCS ${EXM_DIR}/main.cpp)

# =============== Server example executable =============== #
add_executable(${SERVER_OUT} ${SERVER_SRCS})
target_link_libraries(${SERVER_OUT} PUBLIC ${LIB_NAME})
target_include_directories(${SERVER_OUT} PUBLIC ${LIB_DIR})

