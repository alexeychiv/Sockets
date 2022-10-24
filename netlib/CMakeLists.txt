cmake_minimum_required(VERSION 3.0.0)
project(netLib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")

add_library(netLib STATIC
    Network.cpp
    Address.cpp
    Socket.cpp
)

if(WIN32)
    target_link_libraries(netLib ws2_32)
endif()

target_include_directories(netLib PUBLIC .)

target_compile_definitions(netLib 
    PUBLIC "$<$<CONFIG:DEBUG>:NET_DEBUG>"
    PUBLIC "NET_VERBOSE"
)