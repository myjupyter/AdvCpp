cmake_minimum_required(VERSION 3.17)

project(prog)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -DDEBUG -g3")
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

# =============== Includes threading for test ============= #
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# =============== Server example executable =============== #
add_executable(${SERVER_OUT} ${SERVER_SRCS})
target_link_libraries(${SERVER_OUT} PUBLIC ${LIB_NAME}
                                           Threads::Threads)
target_include_directories(${SERVER_OUT} PUBLIC ${LIB_DIR} )

# ==================== Http Packet  ======================= #
add_executable(coro.out ${EXM_DIR}/coro_test.cpp)
target_link_libraries(coro.out PUBLIC ${LIB_NAME}
                                      Threads::Threads)
target_include_directories(coro.out PUBLIC ${LIB_DIR})

# ==================== Http Packet  ======================= #
add_executable(http_packet.out ${EXM_DIR}/test_http_packet.cpp)
target_link_libraries(http_packet.out PUBLIC ${LIB_NAME}
                                             Threads::Threads)
target_include_directories(http_packet.out PUBLIC ${LIB_DIR})

# ==================== Threads  ======================= #
add_executable(threads.out ${EXM_DIR}/threads.cpp)
target_link_libraries(threads.out PUBLIC ${LIB_NAME}
                                         Threads::Threads)
target_include_directories(threads.out PUBLIC ${LIB_DIR})

# ==================== Threads  ======================= #
add_executable(res.out ${EXM_DIR}/resource.cpp)
target_link_libraries(res.out PUBLIC ${LIB_NAME}
                                     Threads::Threads)
target_include_directories(res.out PUBLIC ${LIB_DIR})
