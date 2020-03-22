set(LIB_SRCS ${SRC_DIR}/base_logger.cpp
             ${SRC_DIR}/stdout_logger.cpp
             ${SRC_DIR}/stderr_logger.cpp
             ${SRC_DIR}/file_logger.cpp
             ${SRC_DIR}/log_formatter.cpp)

add_library(${LIB_NAME} SHARED ${LIB_SRCS})

target_include_directories(${LIB_NAME} PUBLIC ${INC_DIR})
target_link_options(${LIB_NAME} PUBLIC -g3)
