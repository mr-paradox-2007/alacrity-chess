cmake_minimum_required(VERSION 3.20)
project(WorkingAlacityChess)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Raylib
add_subdirectory(../raylib_native raylib)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Client executable
file(GLOB CLIENT_SOURCES "source/client/*.cpp")
add_executable(chess_client ${CLIENT_SOURCES})
target_link_libraries(chess_client raylib)

# Server executable
file(GLOB SERVER_SOURCES "source/server/*.cpp")
add_executable(chess_server ${SERVER_SOURCES})
target_link_libraries(chess_server pthread)


