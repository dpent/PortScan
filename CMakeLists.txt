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

if(USE_GUI)
    # OpenGL
    # Choose modern GLVND libraries
    cmake_policy(SET CMP0072 NEW)
    set(OpenGL_GL_PREFERENCE GLVND)
    
    find_package(OpenGL REQUIRED)
    target_link_libraries(portscan PRIVATE OpenGL::GL)

    # GLAD
    add_library(glad external/glad/src/glad.c)
    target_include_directories(glad PUBLIC external/glad/include)
    target_link_libraries(portscan PRIVATE glad)

    # GLFW
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    add_subdirectory(external/glfw)
    target_link_libraries(portscan PRIVATE glfw)
endif()