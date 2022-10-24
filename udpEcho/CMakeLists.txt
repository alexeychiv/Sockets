cmake_minimum_required(VERSION 3.0.0)
project(udpEcho)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}")

add_executable(udpClient udpClient.cpp)
target_link_libraries(udpClient PRIVATE netLib)

add_executable(udpEchoServer udpEchoServer.cpp)
target_link_libraries(udpEchoServer PRIVATE netLib)