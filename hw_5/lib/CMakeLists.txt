set(LIB_SRCS ${SRC_DIR}/shmem.cpp
             ${SRC_DIR}/Semaphore.cpp)

add_library(${LIB_NAME} SHARED ${LIB_SRCS})

target_include_directories(${LIB_NAME} PUBLIC ${INC_DIR})
