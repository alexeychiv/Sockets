cmake_minimum_required(VERSION 3.0.0)
project(root)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_subdirectory(netlib)
add_subdirectory(udpEcho)
add_subdirectory(tcpEcho)