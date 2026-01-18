#ifndef TEXTURE_HELPER_H
#define TEXTURE_HELPER_H

unsigned int gen_texture(char* texLocation);
unsigned int gen_texture_wh(char* texLocation, int* width, int* height);
unsigned int gen_texture_whc(char* texLocation, int* width, int* height, int* nrChannels);
unsigned int gen_skybox_texture(char* texLocation);

unsigned int gen_texture_data(unsigned char* data, int width, int height, int nrChannels);

unsigned char* image_subregion(unsigned char* source, int width, int height, int channels, int xOffset, int yOffset, int size);

/**
 * Generate a cube map from the provided texture. The 6 faces are extrapolated from subregions of the texture.
 * @param textureLocation
 * @param width
 * @param height
 * @param nrChannels
 * @return
 */
unsigned int gen_cube_map_single(char* textureLocation, int* width, int* height, int* nrChannels);

#endif //TEXTURE_HELPER_H
