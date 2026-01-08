cmake_minimum_required(VERSION 3.10)
project(Pacman3D CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Pacman3D
    src/main.cpp
    src/scene.cpp
    src/pacman.cpp
    src/ghost.cpp
    src/power_pellet.cpp
    src/model.cpp
)

target_include_directories(Pacman3D PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/glfw/include
)

add_subdirectory(glfw)

find_package(OpenGL REQUIRED)

target_link_libraries(Pacman3D PRIVATE
    glfw
    OpenGL::GL
    OpenGL::GLU
    gdi32
)


set_target_properties(Pacman3D PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
)