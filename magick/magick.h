#ifndef FE3_MAGICK
#define FE3_MAGICK

#include "tileset.h"
#include "cgram.h"

void loadMagick(const char* path);
void writePNG(Tileset& tileset, const Chapter& defaultChapter);

#endif
