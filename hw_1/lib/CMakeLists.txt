add_library(${LIB_NAME} SHARED ${SRC_DIR}/process.cpp)

target_include_directories(${LIB_NAME} PUBLIC ${INC_DIR})
target_link_options(${LIB_NAME} PUBLIC -g3)
