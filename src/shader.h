//
// Created by User on 11/4/2025.
//

#ifndef SHADER_H
#define SHADER_H

#include "shader_helper.h"

class Shader {
    unsigned int id;

    Shader(const char* vertexPath, const char* fragmentPath) {
        id = createShaderProgram(vertexPath, fragmentPath);
    }

    /**
     * Activate the shader.
     */
    void use()
    {
        glUseProgram(ID);
    }


    void setBool(const char* name, bool value) const
    {
        glUniform1i(glGetUniformLocation(id, name), (int)value);
    }

    void setInt(const char* name, int value) const
    {
        glUniform1i(glGetUniformLocation(id, name), value);
    }

    void setFloat(const char* name, float value) const
    {
        glUniform1f(glGetUniformLocation(id, name), value);
    }

};

#endif //SHADER_H
