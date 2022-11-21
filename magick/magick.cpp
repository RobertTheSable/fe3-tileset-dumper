#include "magick.h"

#include <iostream>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <list>
#include <fstream>

#define MAGICKCORE_QUANTUM_DEPTH 8
#define MAGICKCORE_HDRI_ENABLE 0

#ifdef __MINGW32__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <Magick++.h>
#ifdef __MINGW32__
#pragma GCC diagnostic pop
#endif

#include <windows.h>

#include "LunarDLL.h"

void loadMagick(const char* path)
{
    Magick::InitializeMagick(path);
}

auto getImage(Tileset& tileset)
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

inline std::string getFormattedName(const snes::Chapter& chapter, bool bsfe)
{
    if (bsfe) {
        return snes::BSFEFormatter.format(chapter);
    } else {
        return snes::FE3Formatter.format(chapter);
    }
}

void writePNG(Tileset& tileset, const snes::Chapter& chapter, bool bsfe)
{
    try {
        auto image = getImage(tileset);
        if (bsfe) {
            image.write(snes::BSFEFormatter.format(chapter) + ".png");
        } else {
            image.write(snes::FE3Formatter.format(chapter) + ".png");
        }
    } catch (Magick::Exception& e) {
        throw std::runtime_error(e.what());
    }
}

void writeGif(Tileset &tileset, const snes::Chapter &chapter, bool bsfe)
{
    try {
        std::list<Magick::Image> frames;
        for (int frameI = 0; frameI < (8 * 32) ; ++frameI) {
            int delay = 1;
            if (tileset.palettes.update(frameI)) {
                auto img = getImage(tileset);
                auto frameString = std::to_string(frameI);
                if (frameI < 100) {
                    if (frameI < 10) {
                        frameString.insert(0, "0");
                    }
                    frameString.insert(0, "0");
                }
//                img.magick("gif");
                // gifs can't do 60fps
                // a delay of to produces 50 fps, which is good enough
//                img.animationDelay(2 * delay);
                img.image()->signature = MagickSignature;
                frames.push_back(img);
                delay = 1;
//                CreateDirectoryA(getFormattedName(chapter, bsfe).c_str(), NULL);
//                img.write(getFormattedName(chapter, bsfe) + "/frame" + frameString + ".png");
            } else {
                ++delay;
            }
        }
        Magick::Image output("512x512", "white");
        output.magick("gif");
        output.image()->signature = MagickSignature;
        Magick::appendImages(&output, frames.begin(), frames.end());
        output.write(getFormattedName(chapter, bsfe) + ".gif");
    } catch (Magick::Exception& e) {
        throw std::runtime_error(e.what());
    }

}
