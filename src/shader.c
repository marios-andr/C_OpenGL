#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <cglm/cglm.h>
#include "shader.h"

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

/**
 * Reads the file at the given path and returns its contents.
 * @param path A string of the path of the shader file.
 * @return The file pointed to by the path
 */
char* loadShaderSource(const char* path) {
    FILE* file = fopen(path, "rb");
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

GLuint create_shader_program(const char* vertexPath, const char* fragmentPath) {
    char* vertexSource = loadShaderSource(vertexPath);
    char* fragmentSource = loadShaderSource(fragmentPath);
    const GLuint vertexShader = compileShaderSource(GL_VERTEX_SHADER, vertexSource);
    const GLuint fragmentShader = compileShaderSource(GL_FRAGMENT_SHADER, fragmentSource);

    free(vertexSource);
    free(fragmentSource);

    return linkProgram(vertexShader, fragmentShader);
}

Shader create_shader(const char* vertexPath, const char* fragmentPath) {
    GLuint program = create_shader_program(vertexPath, fragmentPath);
    Shader shader = {.id = program};
    return shader;
}

void shader_use(Shader shader) {
    glUseProgram(shader.id);
}

void shader_delete(Shader* shader) {
    glDeleteProgram(shader->id);
    shader->id = 0;
}

void shader_u1i(Shader shader, const char* name, int val) {
    glUniform1i(glGetUniformLocation(shader.id, name), val);
}

void shader_uMat4f(Shader shader, const char* name, mat4 val) {
    glUniformMatrix4fv(glGetUniformLocation(shader.id, name), 1, GL_FALSE, (float*)val);
}