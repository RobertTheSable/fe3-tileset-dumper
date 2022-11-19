#include "pointers.h"

#include <stdexcept>

#include <windows.h>
#include "LunarDLL.h"

int getAddressFromFile(std::istream& file, unsigned int address)
{
    file.seekg(LunarSNEStoPC(address, LC_LOROM, LC_NOHEADER));
    int retVal = 0;
    file.read((char*)&retVal, 3);
    return retVal;
}

auto operator "" _sjs(const char* in, std::size_t len)
{
    wchar_t output[34];
    memset(output, 0, 34);
    MultiByteToWideChar(CP_UTF8, 0, in, -1, output, len);

    char output2[17] = "";
    memset(output2, ' ', 16);
    WideCharToMultiByte(932, 0, output, -1, output2, len*2, NULL, NULL);
    return std::string{output2};
}

snes::Rom::Rom(std::istream &romFile)
{
    snes::StandardHeader h;
    romFile.seekg(LunarSNEStoPC(0xFFC0, LC_LOROM, LC_NOHEADER));
    auto headerPos = romFile.tellg();
    romFile.read((char*)&h, 32);
    if (std::string(h.title, h.title+21) != "SHVC FIREEMBLEM      ") {
        romFile.seekg(headerPos);
        snes::BSNESHeader bsh;
        romFile.read((char*)&bsh, 32);
        std::string title(bsh.title, bsh.title+16);

        if (title == "パレス陥落      "_sjs) {
            animatedTiles.address = 0x89CA9B;
            tileset.address = 0x89CAAA;
            staticTiles.address = 0x89CA98;
            brightness.address = 0x89CAA1;
            palette.address = 0x89CA9E;
        } else if (title == "赤い竜騎士      "_sjs) {
            animatedTiles.address = 0x89CB20;
            tileset.address = 0x89CB2F;
            staticTiles.address = 0x89CB1D;
            brightness.address = 0x89CB26;
            palette.address = 0x89CB23;
        } else if (title == "正義の盗賊団    "_sjs) {
            animatedTiles.address = 0x89CBC7;
            tileset.address = 0x89CBD6;
            staticTiles.address = 0x89CBC4;
            brightness.address = 0x89CBCD;
            palette.address = 0x89CBCA;
        } else if (title == "始まりのとき    "_sjs) {
            animatedTiles.address = 0x89CBC9;
            staticTiles.address = 0x89CBD8;
            tileset.address = 0x89CBC6;
            brightness.address = 0x89CBCF;
            palette.address = 0x89CBCC;
        } else {
            throw std::runtime_error(
                "ROM not supported."
            );
        }

        animatedTiles.offset = 0;
        tileset.offset = 0;
        staticTiles.offset = 0;
        brightness.offset = 0;
        palette.offset = 0;
        fixedPalette = 0x99EE86;
        chapters.push_back(Chapter{1, 1, 0, 0});
    } else {
        animatedTiles = {0x89c9c7, 0x970000};
        tileset.address = 0x89CB6E;
        tileset.offset = 0xA20000;
        staticTiles.address = 0x89C9A3;
        staticTiles.offset = 0x9D0000;
        brightness.address = 0x89CA4E;
        brightness.offset = 0xCB0000;
        palette.address = 0x89C9EB;
        palette.offset = 0xCB0000;
        fixedPalette = 0x95F800;

        auto addr = LunarSNEStoPC(0x848C16, LC_LOROM, LC_NOHEADER);
        romFile.seekg(addr);

        for (int chapter = 1; chapter <= 44 ; ++chapter) {
            chapters.push_back({romFile.get(), chapter, 0, 0});
        }
    }


}

int snes::OffsetPointer::getAddress(std::istream &in, int index) const
{
    return offset + getAddressFromFile(in, address + (3 * index));
}
