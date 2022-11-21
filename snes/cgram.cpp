#include <windows.h>
#include <stdexcept>
#include <iomanip>
#include "LunarDLL.h"
#include "cgram.h"

CGRam::CGRam(std::istream& file, int colorPtr, int brightnessPtr, const snes::BaseColors& fixedColors)
{
    file.seekg(LunarSNEStoPC(colorPtr, LC_LOROM, LC_NOHEADER));
    std::vector<std::vector<unsigned short>> snesColors(16, std::vector<unsigned short>{});
    int palcount = 0;
    for (auto& palette: snesColors) {
        palette.reserve(16);
        int count = 0;
        while (file && count < 16) {
            unsigned short a = 0, b  = 0;
            a = file.get();
            b = file.get();
            unsigned short color = a;
            color |= (b << 8);
            ++count;
            palette.push_back(color);
        }
        ++palcount;
    }
    if (!file || palcount < 16) {
        throw std::runtime_error("Invalid tileset palette.");
    }
    
    this->colors.reserve(16 * 16);
    for (int index = 0; index < 16 ; ++index) {
        auto fixedPal = fixedColors[index];
        if (fixedPal) {
            for (auto color: *fixedPal) {
                this->colors.push_back(color);
            }
        } else {
            for (auto& color: snesColors[index]) {
                this->colors.push_back(LunarSNEStoPCRGB(color));
            }
        }
    }

    file.seekg(LunarSNEStoPC(brightnessPtr + 0x3000, LC_LOROM, LC_NOHEADER));

    for (int i = 0; i < 2 ; ++i) {
        for (auto &bPalette: brightness) {
            unsigned int bColor = file.get();
            bColor |= (file.get() << 8);
            bPalette[i] = LunarSNEStoPCRGB(bColor);
        }
    }
}

bool CGRam::update(int frame)
{
    if ((frame & 7) == 3) {
        auto tmpIndex = (brightIndex & 0x1E);
        bool wasUpdated = ((brightIndex %2) == 0);
        if (wasUpdated) {
            tmpIndex >>= 1;
            this->colors[0x37] = brightness[tmpIndex][0];
            this->colors[0x38] = brightness[tmpIndex][1];
        }

        ++brightIndex;
        return wasUpdated;
    }
    return false;
}
