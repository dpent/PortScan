cmake_minimum_required(VERSION 3.10)

project(portscan)

set(CMAKE_CXX_STANDARD 20)

option(USE_GUI "Build GUI version" OFF)

file(GLOB_RECURSE SOCKETS "src/sockets/**/*.cpp")

if(USE_GUI)
    set(MAIN_FILE src/gui/main.cpp)
else()
    set(MAIN_FILE src/cli/main.cpp)
endif()

add_executable(portscan ${MAIN_FILE} ${SOCKETS})

target_include_directories(portscan PRIVATE src)

if (WIN32)
    target_link_libraries(portscan PRIVATE ws2_32)
endif()