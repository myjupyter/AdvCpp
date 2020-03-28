cmake_minimum_required(VERSION 3.10)

project(prog)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -o3")

set(PRJ_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(INC_DIR ${PRJ_DIR}/include)
set(SRC_DIR ${PRJ_DIR}/src)
set(LIB_DIR ${PRJ_DIR}/lib)

set(LIB_NAME Tcp)
set(TARGET_NAME prog.out)

add_subdirectory(${LIB_DIR})

set(SRCS ${SRC_DIR}/main.cpp)

add_executable(${TARGET_NAME} ${SRCS})

#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)

target_link_libraries(${TARGET_NAME} LINK_PUBLIC ${LIB_NAME})
    #  Threads::Threads)
target_include_directories(${TARGET_NAME} PUBLIC ${LIB_DIR})
