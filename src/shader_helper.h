//
// Created by User on 29/3/2025.
//

#ifndef SHADER_HELPER_H
#define SHADER_HELPER_H
#include <glad/glad.h>


void checkCompileErrors(GLuint shader, GLenum type);
void checkLinkingErrors(GLuint program);

/**
 * Reads the file at the given path and returns its contents.
 * @param path A string of the path of the shader file.
 * @return The file pointed to by the path
 */
char* loadShaderSource(const char* path);
GLuint compileShaderSource(const GLenum type, const char* source);
GLuint linkProgram(const GLuint vertexShader, const GLuint fragmentShader);

/**
 * Reads, compiles and links the given shader files to a new program.
 * It is the combination of the previous functions
 * @param vertexPath The path to the vertex shader.
 * @param fragmentPath The path to the fragment shader.
 * @return The ID of the shader program.
 */
GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath);
void checkCompileErrors(const GLuint shader, const GLenum type);
void checkLinkingErrors(const GLuint program);

#endif //SHADER_HELPER_H
