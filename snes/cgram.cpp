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
}
