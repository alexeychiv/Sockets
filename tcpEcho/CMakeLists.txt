cmake_minimum_required(VERSION 3.0.0)
project(tcpEcho)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}")

add_executable(tcpClient tcpClient.cpp)
target_link_libraries(tcpClient PRIVATE netLib)

add_executable(tcpEchoServer tcpEchoServer.cpp)
target_link_libraries(tcpEchoServer PRIVATE netLib)