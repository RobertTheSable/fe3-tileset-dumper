#ifndef FE3_MAGICK
#define FE3_MAGICK

#include "tileset.h"
#include "cgram.h"
#include "pointers.h"
#include "tileset.h"

void loadMagick(const char* path);
void writePNG(Tileset& tileset, const snes::Chapter& defaultChapter, bool bsfe = false);

#endif
