cmake_minimum_required(VERSION 3.10)

project(prog)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-g3")
set(CMAKE_CXX_FLAGS_RELEASE "-o3")

set(PRJ_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(INC_DIR ${PRJ_DIR}/include)
set(SRC_DIR ${PRJ_DIR}/src)
set(LIB_DIR ${PRJ_DIR}/lib)

set(LIB_NAME Process)
set(TARGET_NAME prog.out)

add_subdirectory(${LIB_DIR})

set(SRCS ${SRC_DIR}/main.cpp)

add_executable(${TARGET_NAME} ${SRCS})

target_link_libraries(${TARGET_NAME} PUBLIC ${LIB_NAME})
target_include_directories(${TARGET_NAME} PUBLIC ${LIB_DIR})
