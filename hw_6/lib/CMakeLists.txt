set(LIB_SRCS ${SRC_DIR}/tcp/ip_address.cpp
             ${SRC_DIR}/tcp/socket_manager.cpp
             ${SRC_DIR}/tcp/socket.cpp
             ${SRC_DIR}/tcp/rw_socket.cpp
             ${SRC_DIR}/tcp/connection_tcp.cpp
             ${SRC_DIR}/tcp/server_tcp.cpp
             ${SRC_DIR}/tcp/base_service.cpp
             ${SRC_DIR}/tcp/client_tcp.cpp
           
             ${SRC_DIR}/http/http_server.cpp  
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

             ${SRC_DIR}/resource/resource_manager.cpp
             )
             

add_library(${LIB_NAME} SHARED ${LIB_SRCS})

target_include_directories(${LIB_NAME} PUBLIC ${INC_DIR}
                                              ${INC_DIR}/tcp
                                              ${INC_DIR}/http        
                                              ${INC_DIR}/exceptions
                                              ${INC_DIR}/coro
                                              ${INC_DIR}/log
                                              ${INC_DIR}/resource
                                              )
