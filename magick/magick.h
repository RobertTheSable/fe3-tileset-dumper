#ifndef FE3_MAGICK
#define FE3_MAGICK

#include "tileset.h"
#include "cgram.h"
#include "pointers.h"
#include "tileset.h"

struct MagickRenderer {
    constexpr static const int MAX_WIDTH = 32*32;
    int width, height;
    static void loadMagick(const char* path);
    void writePNG(Tileset& tileset, const snes::Chapter& chapter, bool bsfe) const;
    void writeAnim(Tileset& tileset, const snes::Chapter& chapter, int frameCount, bool gif, bool bsfe) const;
private:
    auto getImage(Tileset& tileset) const;
};
#endif
