//
// Created by User on 29/3/2025.
//

#ifndef SHADER_HELPER_H
#define SHADER_HELPER_H
#include <glad/glad.h>
#include <cglm/cglm.h>

typedef struct {
    GLuint id;
} Shader;

/**
 * Reads, compiles and links the given shader files to a new program.
 * It is the combination of the previous functions
 * @param vertexPath The path to the vertex shader.
 * @param fragmentPath The path to the fragment shader.
 * @return The ID of the shader program.
 */
GLuint create_shader_program(const char* vertexPath, const char* fragmentPath);

Shader create_shader(const char* vertexPath, const char* fragmentPath);

void shader_use(Shader shader);
void shader_delete(Shader* shader);

void shader_u1i(Shader shader, const char* name, int val);
void shader_u3f(Shader shader, const char* name, vec3 val);
void shader_uMat4f(Shader shader, const char* name, mat4 val);

#endif //SHADER_HELPER_H
