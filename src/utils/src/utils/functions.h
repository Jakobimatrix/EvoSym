#ifndef UTILS_FUNCTIONS
#define UTILS_FUNCTIONS

namespace utils {
// this uses stb/stb_image which cannot be used in a .h file.
inline unsigned char* loadTexture(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);

void freeTexture(unsigned char *c);
}

#endif
