#ifndef TEXTURE_HELPER_H
#define TEXTURE_HELPER_H

unsigned int gen_texture(char* texLocation);
unsigned int gen_texture_wh(char* texLocation, int* width, int* height);
unsigned int gen_texture_whc(char* texLocation, int* width, int* height, int* nrChannels);
unsigned int gen_texture_f(char* texLocation, int format);
unsigned int gen_texture_whcf(char* texLocation, int* width, int* height, int* nrChannels, int format);

#endif //TEXTURE_HELPER_H
