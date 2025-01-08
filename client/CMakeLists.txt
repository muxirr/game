cmake_minimum_required(VERSION 3.20)

project(client)

#aux_source_directory(./source CLIENT_SRC)
file(GLOB CLIENT_SRC "./source/*.cpp" "./source/*.c")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
set(VS_DIR "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/VS")

add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)

include_directories(include ../include ${VS_DIR}/include)

add_executable(${PROJECT_NAME} WIN32 ${CLIENT_SRC})

target_link_libraries(${PROJECT_NAME} PUBLIC ${VS_DIR}/lib/x64/EasyXw.lib)