cmake_minimum_required(VERSION 3.10)

project(portscan)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOCKETS "src/sockets/**/*.cpp")
add_executable(portscan src/main.cpp ${SOCKETS})

if (WIN32)
    target_link_libraries(portscan PRIVATE ws2_32)
endif()

target_include_directories(portscan PRIVATE src)