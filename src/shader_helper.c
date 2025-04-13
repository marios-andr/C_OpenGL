#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include "shader_helper.h"


char* loadShaderSource(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        printf("Error opening file: %s\n", path);
        return NULL;
    }

    // Move file pointer to the end to get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(size + 1);
    if (!buffer) {
        printf("Error: Memory allocation failed\n");
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0';  // Null-terminate the string

    fclose(file);
    return buffer;
}

GLuint compileShaderSource(const GLenum type, const char* source) {
    int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    checkCompileErrors(shader, type);
    return shader;
}

GLuint linkProgram(const GLuint vertexShader, const GLuint fragmentShader) {
    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    checkLinkingErrors(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    const char* vertexSource = loadShaderSource(vertexPath);
    const char* fragmentSource = loadShaderSource(fragmentPath);
    const GLuint vertexShader = compileShaderSource(GL_VERTEX_SHADER, vertexSource);
    const GLuint fragmentShader = compileShaderSource(GL_FRAGMENT_SHADER, fragmentSource);
    return linkProgram(vertexShader, fragmentShader);
}

void checkCompileErrors(const GLuint shader, const GLenum type) {
    int success;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        printf("ERROR::PROGRAM_COMPILATION_ERROR of type: %s \n %s \n -- --------------------------------------------------- -- \n", glGetString(type), infoLog);
    }
}

void checkLinkingErrors(const GLuint program) {
    int success;
    char infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        printf("ERROR::SHADER_LINKING_ERROR of type: PROGRAM \n %s \n -- --------------------------------------------------- -- \n", infoLog);
    }
}