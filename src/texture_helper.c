#include <glad/glad.h>
#include "texture_helper.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../external/glfw/src/internal.h"
#include "stb/stb_image.h"

typedef unsigned char* Image;

GLint get_image_format(int nrChannels) {
    switch (nrChannels) {
        case 1: return GL_RED;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: return GL_RGB;
    }
}

unsigned int gen_texture(char* texLocation) {
    int width, height;
    return gen_texture_wh(texLocation, &width, &height);
}

unsigned int gen_texture_wh(char* texLocation, int* width, int* height) {
    int nrChannels;
    return gen_texture_whc(texLocation, width, height, &nrChannels);
}

unsigned int gen_texture_whc(char* texLocation, int* width, int* height, int* nrChannels) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture
    Image data = stbi_load(texLocation, width, height, nrChannels, 0);
    if (data) {
        GLint format = get_image_format(*nrChannels);

        glTexImage2D(GL_TEXTURE_2D, 0, format, *width, *height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        printf("Failed to load texture\n");
    }
    stbi_image_free(data);

    return texture;
}

unsigned int gen_texture_data(Image data, int width, int height, int nrChannels) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLint format = get_image_format(nrChannels);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    return texture;
}

Image image_subregion(Image source, int width, int height, int channels, int xOffset, int yOffset, int size) {
    int N = size * size * channels;
    Image subregion = (Image) malloc(N * sizeof(unsigned char));
    if (!subregion) return NULL;

    if (xOffset + size > width || yOffset + size > height) {
        return NULL;
    }

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int dst = (y*size + x) * channels; //subregion pixel at (x,y)
            int src = ((y+yOffset)*width + x+xOffset) * channels; //source pixel at (x,y)

            for (int c = 0; c < channels; c++) {
                subregion[dst+c] = source[src+c];
            }
        }
    }

    return subregion;
}

unsigned int gen_cube_map_single(char* textureLocation, int* width, int* height, int* nrChannels) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    //"../resources/galaxy.jpg"
    Image data = stbi_load(textureLocation, width, height, nrChannels, 0);
    if (!data) {
        printf("Failed to load texture\n");
        return 0;
    }

    int newWidth = *width / 4;
    int newHeight = *height / 3;
    int size = min(newWidth, newHeight);
    size = (int)pow((int)sqrt(size), 2);
    Image faces[6];
    /*right*/ faces[0] = image_subregion(data, *width, *height, *nrChannels, newWidth * 2, newHeight, size);
    /*left*/ faces[1] = image_subregion(data, *width, *height, *nrChannels, 0, newHeight, size);
    /*top*/ faces[2] = image_subregion(data, *width, *height, *nrChannels, newWidth, 0, size);
    /*bottom*/ faces[3] = image_subregion(data, *width, *height, *nrChannels, newWidth, newHeight * 2, size);
    /*front*/ faces[4] = image_subregion(data, *width, *height, *nrChannels, newWidth, newHeight, size);
    /*back*/ faces[5] = image_subregion(data, *width, *height, *nrChannels, newWidth * 3, newHeight, size);



    GLint format = get_image_format(*nrChannels);
    for (int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0, format, size, size, 0, format, GL_UNSIGNED_BYTE, faces[i]);
        free(faces[i]);
    }

    stbi_image_free(data);

    return texture;
}