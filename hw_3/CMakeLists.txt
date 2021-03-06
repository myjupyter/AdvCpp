cmake_minimum_required(VERSION 3.10)

project(prog)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -o3")

set(PRJ_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(INC_DIR ${PRJ_DIR}/include)
set(SRC_DIR ${PRJ_DIR}/src)
set(LIB_DIR ${PRJ_DIR}/lib)
set(EXM_DIR ${PRJ_DIR}/example)

set(LIB_NAME   Tcp)
set(CLIENT_OUT client.out)
set(SERVER_OUT server.out)

add_subdirectory(${LIB_DIR})

set(CLIENT_SRCS ${EXM_DIR}/client.cpp)
set(SERVER_SRCS ${EXM_DIR}/server.cpp)

# =============== Client example executable =============== #
add_executable(${CLIENT_OUT} ${CLIENT_SRCS})
target_link_libraries(${CLIENT_OUT} PUBLIC ${LIB_NAME})
target_include_directories(${CLIENT_OUT} PUBLIC ${LIB_DIR})

# =============== Server example executable =============== #
add_executable(${SERVER_OUT} ${SERVER_SRCS})
target_link_libraries(${SERVER_OUT} PUBLIC ${LIB_NAME})
target_include_directories(${SERVER_OUT} PUBLIC ${LIB_DIR})
