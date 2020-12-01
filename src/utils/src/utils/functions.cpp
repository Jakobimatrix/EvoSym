#include "functions.h"

#include <warning_guards/warning_guards.h>

THIRD_PARTY_HEADERS_BEGIN
#include <stb/stb_image.h>
THIRD_PARTY_HEADERS_END


namespace utils {
unsigned char *loadTexture(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels) {
  return stbi_load(filename, x, y, channels_in_file, desired_channels);
}

void freeTexture(unsigned char *c) { stbi_image_free(c); }
}  // namespace utils
