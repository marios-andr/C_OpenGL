//
// Created by User on 5/5/2025.
//

#ifndef MESH_H
#define MESH_H

#include <cglm/cglm.h>
#include <glad/glad.h>

#include "shader.h"

typedef struct {
    vec3 position;
    versor orientation; //quaternion
    vec3 scale;
} Model;

typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLsizei indices;
} Mesh;

typedef struct {
    GLint size;
    GLenum type;
} Attribute;

extern const Attribute ATTRIB_POSITION;
extern const Attribute ATTRIB_UV;

Mesh mesh_init_attrib(float* data, unsigned int dataSize,
                int* indices, unsigned int indicesSize,
                int attribCount, Attribute* attributes);

Mesh mesh_init_ptr(const float* data, unsigned int dataSize,
                const int* indices, unsigned int indicesSize,
                int attribCount, GLint* attribSizes, GLsizei* attribStrides, GLsizeiptr* attribOffsets);

void mesh_bind(Mesh mesh);

void mesh_destroy(Mesh *m);

Model model_init(float x, float y, float z);

void model_translate(Model *m, float x, float y, float z);

void model_rotate(Model *m, float rad, float x, float y, float z);

void model_rotate_deg(Model *m, float degrees, float x, float y, float z);

void model_add_rotation(Model *m, float rad, float x, float y, float z);

void model_add_rotation_deg(Model *m, float deg, float x, float y, float z);

void model_scale(Model *m, float x, float y, float z);

void model_to_shader(Model m, Shader shader);

Mesh shape_square();

Mesh shape_cube();

Mesh shape_skybox();
#endif //MESH_H