cmake_minimum_required(VERSION 3.20)

project(client)

#aux_source_directory(./source CLIENT_SRC)
file(GLOB CLIENT_SRC "./source/*.cpp" "./source/*.c")


include_directories(include ../include "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/VS/include")

add_executable(${PROJECT_NAME} ${CLIENT_SRC})


target_link_libraries(${PROJECT_NAME} PUBLIC "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/VS/lib/x64/EasyXw.lib" "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/VS/lib/x64/EasyXa.lib")