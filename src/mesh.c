//
// Created by User on 5/5/2025.
//

#include "mesh.h"

const Attribute ATTRIB_POSITION = { 3, GL_FLOAT };
const Attribute ATTRIB_UV = { 2, GL_FLOAT };

int gl_type_size(const GLenum type) {
    switch (type) {
        case GL_FLOAT: return sizeof(GLfloat);
        case GL_INT: return sizeof(GLint);
        case GL_UNSIGNED_INT: return sizeof(GLuint);
        case GL_SHORT: return sizeof(GLshort);
        case GL_UNSIGNED_SHORT: return sizeof(GLushort);
        case GL_BYTE: return sizeof(GLbyte);
        case GL_UNSIGNED_BYTE: return sizeof(GLubyte);
        default: return 0; // Handle unknown type safely
    }
}

Mesh mesh_init(const float* data, const unsigned int dataSize,
                const int* indices, const unsigned int indicesSize) {
    GLuint VBO, VAO, EBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    if (indicesSize > 0)
        glGenBuffers(1, &EBO);
    Mesh vao = {
        .vao = VAO,
        .vbo = VBO,
        .ebo = EBO,
        .indices = indicesSize,
    };

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);

    if (indicesSize > 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);
    }

    return vao;
}

Mesh mesh_init_ptr(const float* data, const unsigned int dataSize,
                const int* indices, const unsigned int indicesSize,
                int attribCount, GLint* attribSizes, GLsizei* attribStrides, GLsizeiptr* attribOffsets) {
    Mesh vao = mesh_init(data, dataSize, indices, indicesSize);

    for (int i = 0; i < attribCount; i++) {
        glVertexAttribPointer(i, attribSizes[i], GL_FLOAT, GL_FALSE, attribStrides[i], (void*)attribOffsets[i]);
        glEnableVertexAttribArray(i);
    }

    return vao;
}

Mesh mesh_init_attrib(float* data, unsigned int dataSize,
                int* indices, unsigned int indicesSize,
                int attribCount, Attribute* attributes) {
    Mesh vao = mesh_init(data, dataSize, indices, indicesSize);

    int stride = 0;
    for (int i = 0; i < attribCount; i++) {
        stride += attributes[i].size * sizeof(gl_type_size(attributes[i].type));
    }

    for (int i = 0; i < attribCount; i++) {
        int offset = (i > 0 ? attributes[i-1].size : 0) * sizeof(gl_type_size(attributes[i].type));
        glVertexAttribPointer(i, attributes[i].size, attributes[i].type, GL_FALSE, stride, (void*)offset);
        glEnableVertexAttribArray(i);
    }

    return vao;
}

void mesh_bind(Mesh mesh) {
    glBindVertexArray(mesh.vao);
}

void mesh_destroy(Mesh *m) {
    glDeleteBuffers(1, &m->ebo);
    glDeleteBuffers(1, &m->vbo);
    glDeleteVertexArrays(1, &m->vao);

    m->vao = m->vbo = m->ebo = 0;
    m->indices = 0;
}

Model model_init(float x, float y, float z) {
    Model m = {
        .position    = {x, y, z},
        .orientation = GLM_QUAT_IDENTITY_INIT,
        .scale       = {1.0f, 1.0f, 1.0f}
      };
    return m;
}

void model_translate(Model *m, float x, float y, float z) {
    m->position[0] += x;
    m->position[1] += y;
    m->position[2] += z;
}

void model_rotate(Model *m, float rad, float x, float y, float z) {
    glm_quatv(m->orientation, rad, (vec3){x,y,z});
}

void model_rotate_deg(Model *m, float degrees, float x, float y, float z) {
    model_rotate(m, glm_rad(degrees), x, y, z);
}

void model_add_rotation(Model *m, float rad, float x, float y, float z) {
    versor quat;
    glm_quatv(quat, rad, (vec3){x,y,z});
    glm_quat_mul(m->orientation, quat, m->orientation);
}

void model_add_rotation_deg(Model *m, float deg, float x, float y, float z) {
    model_add_rotation(m, glm_rad(deg), x, y, z);
}

void model_scale(Model *m, float x, float y, float z) {
    m->scale[0] = x;
    m->scale[1] = y;
    m->scale[2] = z;
}

void model_to_shader(Model m, Shader shader) {
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model, m.position);
    glm_quat_rotate_at(model, m.orientation, (vec3){0,0,0});
    glm_scale(model, m.scale);

    shader_uMat4f(shader, "model", model);
}

Mesh shape_square() {
    float vertices[] = {
        // positions         // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // top right
        0.5f, -0.5f, 0.0f, 1.0f, 1.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };
    Attribute attribs[] = {
        ATTRIB_POSITION,
        ATTRIB_UV,
    };

    return mesh_init_attrib(vertices, sizeof(vertices), indices, sizeof(indices), 2, attribs);
}

Mesh shape_cube() {
    float cube[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
    };
    Attribute attribs[] = {
        ATTRIB_POSITION,
        ATTRIB_UV,
    };

    return mesh_init_attrib(cube, sizeof(cube), NULL, 0, 2, attribs);
}