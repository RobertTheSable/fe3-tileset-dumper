#include <windows.h>
#include <stdexcept>
#include <iomanip>
#include "LunarDLL.h"
#include "cgram.h"

void loadPalette(std::istream& file, int address, std::vector<unsigned short>& colorBuffer) 
{
    file.seekg(LunarSNEStoPC(address, LC_LOROM, LC_NOHEADER));
    for (auto& color: colorBuffer) {
        color = file.get();
        color |= (file.get() << 8);
    }
    
    if (!file) {
        throw std::runtime_error("Invalid rom palette.");
    }
}

CGRam::CGRam(std::istream& file, int index)
{   
    unsigned int brightAddressPtr = 0xCB0000 + getAddressFromFile(
        file, 
        0x89CA4E + (3*index)
    );
    
    unsigned int colorsPtr = 0xCB0000 + getAddressFromFile(
        file, 
        0x89C9EB + (3*index)
    );
    
    file.seekg(LunarSNEStoPC(colorsPtr, LC_LOROM, LC_NOHEADER));
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
    
    file.seekg(LunarSNEStoPC(0x95F800, LC_LOROM, LC_NOHEADER));
    for (int idx = 5; file && idx < 8; ++idx) {
        for (auto& color: snesColors[idx]) {
            color = file.get();
            color |= (file.get() << 8);
        }
    }
    
    if (!file) {
        throw std::runtime_error("Invalid rom palette.");
    }
    
    
    loadPalette(file, 0x94D080,  snesColors[0]);
    
    snesColors[12] = snesColors[5];
    snesColors[13] = snesColors[7];
    snesColors[14] = snesColors[6];
    
    snesColors[12].back() = snesColors[0][3];
    snesColors[13].back() = snesColors[0][3];
    snesColors[14].back() = snesColors[0][3];
    
    loadPalette(file, 0x94D000, snesColors[8]);
    
    loadPalette(file, 0x94D020, snesColors[11]);
    
    loadPalette(file, 0x94D060, snesColors[15]);
    
    snesColors[0].front() = 0;
    
    this->colors.reserve(16 * 16);
    for (auto& snesPalette: snesColors) {
        for (auto& color: snesPalette) {
            this->colors.push_back(LunarSNEStoPCRGB(color));
        }
    }
}
