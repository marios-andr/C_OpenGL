cmake_minimum_required(VERSION 3.30)
project(C_OpenGL C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Include GLFW and GLAD
find_package(OpenGL REQUIRED)
add_subdirectory(external/glfw external/cglm)  # Runs the CMakeLists of the directories.

include_directories(include external/glfw/include external/cglm/include)

# Add source files
add_executable(C_OpenGL src/main.c src/glad.c
        src/shader.c
        src/shader.h
        src/texture_helper.c
        src/texture_helper.h
)

# Link OpenGL and GLFW
target_link_libraries(C_OpenGL OpenGL::GL glfw)
