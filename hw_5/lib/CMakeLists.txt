set(LIB_SRCS ${SRC_DIR}/shared_memory.cpp
             ${SRC_DIR}/semaph.cpp)

add_library(${LIB_NAME} SHARED ${LIB_SRCS})

target_include_directories(${LIB_NAME} PUBLIC ${INC_DIR})
