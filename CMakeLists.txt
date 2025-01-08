cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
add_definitions(-DUNICODE -D_UNICODE)

add_subdirectory(./server)
add_subdirectory(./client)
