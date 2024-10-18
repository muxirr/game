cmake_minimum_required(VERSION 3.20)

project(server)


file(GLOB SERVER_SRC "./source/*.cpp" "./source/*.c")

message(STATUS "server source files: ${SERVER_SRC}")

include_directories(include ../include)

add_executable(${PROJECT_NAME} ${SERVER_SRC})

