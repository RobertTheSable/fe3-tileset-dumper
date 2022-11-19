#include "magick.h"

#include <iostream>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <fstream>

#define MAGICKCORE_QUANTUM_DEPTH 8
#define MAGICKCORE_HDRI_ENABLE 0
#include <Magick++.h>

#include <windows.h>

#include "LunarDLL.h"

void loadMagick(const char* path)
{
    Magick::InitializeMagick(path);
}

auto getImage(Tileset& tileset, const Chapter& chapter)
{
    std::vector<unsigned int> bytes(512*512, 0);

    int x = 0, y = 0, quarter = 0;
    for (auto& tile: tileset.tiles) {
        int xCoord = (x * 16) + ((quarter % 2) * 8);
        int yCoord = (y * 16) + ((quarter / 2) * 8);

        LunarRender8x8(
            &bytes[0],
            512,
            512,
            xCoord,
            yCoord,
            (void*)&tileset.pixmap[0],
            &tileset.palettes.colors[0],
            tile,
            LC_DRAW
        );
        ++quarter;
        if (quarter == 4) {
            ++x;
            if (x == 32) {
                x = 0;
                ++y;
            }
            quarter = 0;
        }
    }

    // may break if sizof(int) > 4
    // but Lunar probably would too
    // O = opacity, where 0 = opaque
    // 'A' treats 0 as transparent
    Magick::Image image(512, 512, "BGRO", Magick::StorageType::CharPixel, (unsigned char*)&bytes[0]);
    return image;
}

void writePNG(Tileset& tileset, const Chapter& chapter)
{

    try {
        auto image = getImage(tileset, chapter);
        image.write((std::string)chapter + ".png");
    } catch (Magick::Exception& e) {
        throw std::runtime_error(e.what());
    }\
}
