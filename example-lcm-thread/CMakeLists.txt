cmake_minimum_required(VERSION 2.8)
project(Thread_Test)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_MODULE_PATH /home/aillian/nanopb/nanopb-0.4.6-linux-x86/extra)
set(SOURCES 
    ${CMAKE_SOURCE_DIR}/modules/src/main.cpp)
find_package(Nanopb REQUIRED)
find_package(lcm REQUIRED)

include_directories(${NANOPB_INCLUDE_DIRS})

nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS
    ${CMAKE_SOURCE_DIR}/modules/common/data_struct/proto/data_struct.proto
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

## set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -O0")

## add_executable(app ${SOURCES} ${PROTO_SRCS} ${PROTO_HDRS})
add_executable(exe ${SOURCES} ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(exe lcm pthread)
