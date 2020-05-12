set(LIB_SRCS ${SRC_DIR}/ip_address.cpp
             ${SRC_DIR}/socket_manager.cpp
             ${SRC_DIR}/socket.cpp
             ${SRC_DIR}/rw_socket.cpp
             ${SRC_DIR}/connection_tcp.cpp
             ${SRC_DIR}/server_tcp.cpp
             ${SRC_DIR}/base_service.cpp
             ${SRC_DIR}/client_tcp.cpp
             ${SRC_DIR}/http_server.cpp
             
             ${SRC_DIR}/http/http_packet.cpp

             ${SRC_DIR}/coro/coro.cpp
            
             ${SRC_DIR}/log/base_logger.cpp
             ${SRC_DIR}/log/stdout_logger.cpp
             ${SRC_DIR}/log/stderr_logger.cpp
             ${SRC_DIR}/log/file_logger.cpp
             ${SRC_DIR}/log/logger.cpp
             ${SRC_DIR}/log/log_formatter.cpp
             ${SRC_DIR}/log/log_color_formatter.cpp
             ${SRC_DIR}/log/global_log_func.cpp
            )
             

add_library(${LIB_NAME} SHARED ${LIB_SRCS})

target_include_directories(${LIB_NAME} PUBLIC ${INC_DIR}
                                              ${INC_DIR}/http        
                                              ${INC_DIR}/exceptions
                                              ${INC_DIR}/coro
                                              ${INC_DIR}/log
                                              )
