set(LIB_SRCS ${SRC_DIR}/ip_address.cpp
             ${SRC_DIR}/socket_manager.cpp
             ${SRC_DIR}/socket.cpp
             ${SRC_DIR}/rw_socket.cpp
             ${SRC_DIR}/connection_tcp.cpp
             ${SRC_DIR}/server_tcp.cpp)

add_library(${LIB_NAME} SHARED ${LIB_SRCS})

target_include_directories(${LIB_NAME} PUBLIC ${INC_DIR})
