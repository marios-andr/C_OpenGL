#include <glad/glad.h>
#include "texture_helper.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

unsigned int gen_texture(char* texLocation) {
    int width, height;
    return gen_texture_wh(texLocation, &width, &height);
}

unsigned int gen_texture_wh(char* texLocation, int* width, int* height) {
    int nrChannels;
    return gen_texture_whc(texLocation, width, height, &nrChannels);
}

unsigned int gen_texture_whc(char* texLocation, int* width, int* height, int* nrChannels) {
    return gen_texture_whcf(texLocation, width, height, nrChannels, GL_RGB);
}

unsigned int gen_texture_f(char* texLocation, GLint format) {
    int width, height, nrChannels;
    return gen_texture_whcf(texLocation, &width, &height, &nrChannels, format);
}

unsigned int gen_texture_whcf(char* texLocation, int* width, int* height, int* nrChannels, GLint format) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture
    unsigned char *data = stbi_load(texLocation, width, height, nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, format, *width, *height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        printf("Failed to load texture\n");
    }
    stbi_image_free(data);

    return texture;
}