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
set(TEST_OUT   test.out)

add_subdirectory(${LIB_DIR})

set(SERVER_SRCS ${EXM_DIR}/main.cpp)
set(TEST_SRCS ${EXM_DIR}/test.cpp)


# =============== Includes threading for test ============= #
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# =============== Server example executable =============== #
add_executable(${SERVER_OUT} ${SERVER_SRCS})
target_link_libraries(${SERVER_OUT} PUBLIC ${LIB_NAME})
target_include_directories(${SERVER_OUT} PUBLIC ${LIB_DIR})

# =============== Test example executable ================= #
add_executable(${TEST_OUT} ${TEST_SRCS})
target_link_libraries(${TEST_OUT} PUBLIC ${LIB_NAME}
                                         Threads::Threads)
target_include_directories(${SERVER_OUT} PUBLIC ${LIB_DIR})

