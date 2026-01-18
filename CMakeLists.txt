cmake_minimum_required(VERSION 3.30)
project(COpenGLLib C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

add_subdirectory(external/glfw)  # Runs the CMakeLists of the directories.
add_subdirectory(external/cglm)

set(ENGINE_SOURCES
        src/glad.c
        src/shader.c
        src/texture_helper.c
        src/mesh.c
        src/camera.c
        include/texture.h
)

add_library(COpenGLLib ${ENGINE_SOURCES})

target_include_directories(COpenGLLib
        PUBLIC
        include
        external/glfw/include
        external/cglm/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(COpenGLLib PUBLIC OpenGL::GL glfw cglm)

project(COpenGLTest C CXX)
add_executable(COpenGLTest src/test/main.c)
target_link_libraries(COpenGLTest COpenGLLib)

project(BlackHole C CXX)
add_executable(BlackHole src/blackhole/main.c)
target_link_libraries(BlackHole COpenGLLib)