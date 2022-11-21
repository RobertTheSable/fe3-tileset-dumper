#include "magick.h"

#include <iostream>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <list>
#include <fstream>

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
        image.write(getFormattedName(chapter, bsfe) + ".png");
    } catch (Magick::Exception& e) {
        throw std::runtime_error(e.what());
    }
}

void writeAnim(Tileset &tileset, const snes::Chapter &chapter, int frameCount, bool gif, bool bsfe)
{
    try {
        std::list<Magick::Image> frames;
        int firstDelay = 0;
        int delay = 1;
        for (int frameI = 0; frameI < frameCount ; ++frameI) {
            if (tileset.update(frameI)) {
                auto img = getImage(tileset);
                auto frameString = std::to_string(frameI);
                if (frameI < 100) {
                    if (frameI < 10) {
                        frameString.insert(0, "0");
                    }
                    frameString.insert(0, "0");
                }
                if (gif) {
                    if (frames.empty()) {
                        firstDelay = delay;
                    } else {
                        frames.back().animationDelay(delay*2);
                    }
                    frames.push_back(img);
                } else {
                    CreateDirectoryA(getFormattedName(chapter, bsfe).c_str(), NULL);
                    img.write(getFormattedName(chapter, bsfe) + "/frame-" + std::to_string(frameI) + ".png");
                }
                delay = 1;
            } else {
                ++delay;
            }
        }
        if (gif) {
            frames.back().animationDelay((firstDelay + delay - 1) * 2);
            // if this fails with a message about signatures, try switching your quantum depth setting.
            Magick::writeImages(
                frames.begin(), 
                frames.end(), 
                getFormattedName(chapter, bsfe) + ".gif"
            );
        }
    } catch (Magick::Exception& e) {
        throw std::runtime_error(e.what());
    }

}
