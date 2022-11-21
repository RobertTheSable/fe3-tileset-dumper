#include "pointers.h"

#include <stdexcept>

#include <windows.h>
#include "LunarDLL.h"


void loadPalette(std::istream& file, snes::Palette& colorBuffer, int address = -1)
{
    if (address != -1) {
        file.seekg(LunarSNEStoPC(address, LC_LOROM, LC_NOHEADER));
    }
    for (auto& color: colorBuffer) {
        unsigned int tmpColor = file.get();
        tmpColor |= (file.get() << 8);
        color = LunarSNEStoPCRGB(tmpColor);
    }

    if (!file) {
        throw std::runtime_error("Invalid rom palette.");
    }
}

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
        snes::BSHeader bsh;
        romFile.read((char*)&bsh, 32);
        std::string title(bsh.title, bsh.title+16);
        int number = 0;

        if (title == "パレス陥落      "_sjs) {
            animatedTiles.address = 0x89CA9B;
            tileset.address = 0x89CAAA;
            staticTiles.address = 0x89CA98;
            brightness.address = 0x89CAA1;
            palette.address = 0x89CA9E;
            number = 1;
        } else if (title == "赤い竜騎士      "_sjs) {
            animatedTiles.address = 0x89CB20;
            tileset.address = 0x89CB2F;
            staticTiles.address = 0x89CB1D;
            brightness.address = 0x89CB26;
            palette.address = 0x89CB23;
            number = 2;
        } else if (title == "正義の盗賊団    "_sjs) {
            animatedTiles.address = 0x89CBC7;
            tileset.address = 0x89CBD6;
            staticTiles.address = 0x89CBC4;
            brightness.address = 0x89CBCD;
            palette.address = 0x89CBCA;
            number = 3;
        } else if (title == "始まりのとき    "_sjs) {
            animatedTiles.address = 0x89CBC9;
            staticTiles.address = 0x89CBD8;
            tileset.address = 0x89CBC6;
            brightness.address = 0x89CBCF;
            palette.address = 0x89CBCC;
            number = 4;
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
        chapters.push_back(Chapter{1, number, 0, 0});
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

        addr = LunarSNEStoPC(fixedPalette, LC_LOROM, LC_NOHEADER);
        romFile.seekg(addr);
        for (auto& pal: this->baseColors.palettes5_6_7) {
            loadPalette(romFile, pal);
        }

        loadPalette(romFile, baseColors.palette0, 0x94D080);

        baseColors.palettes12_13_14[0] = baseColors.palettes5_6_7[0];
        baseColors.palettes12_13_14[1] = baseColors.palettes5_6_7[2];
        baseColors.palettes12_13_14[2] = baseColors.palettes5_6_7[1];
//        snesColors[12] = snesColors[5];
//        snesColors[13] = snesColors[7];
//        snesColors[14] = snesColors[6];

        baseColors.palettes12_13_14[0].back() = baseColors.palette0[3];
        baseColors.palettes12_13_14[1].back() = baseColors.palette0[3];
        baseColors.palettes12_13_14[2].back() = baseColors.palette0[3];
//        snesColors[12].back() = snesColors[0][3];
//        snesColors[13].back() = snesColors[0][3];
//        snesColors[14].back() = snesColors[0][3];

        loadPalette(romFile, baseColors.palette8, 0x94D000);

        loadPalette(romFile, baseColors.palette11, 0x94D020);

        loadPalette(romFile, baseColors.palette15, 0x94D060);

        baseColors.palette0[0] = 0;
    }
}

const snes::BaseColors &snes::Rom::getBaseColors() const
{
    return baseColors;
}

int snes::OffsetPointer::getAddress(std::istream &in, int index) const
{
    return offset + getAddressFromFile(in, address + (3 * index));
}

std::optional<snes::Palette> snes::BaseColors::operator[](int n) const
{
    if (n == 0) {
        return this->palette0;
    } else if (n >= 5 && n <= 7) {
        return this->palettes5_6_7[n-5];
    } else if (n == 8) {
        return this->palette8;
    } else if (n == 11) {
        return this->palette11;
    } else if (n >= 12 && n <= 14) {
        return this->palettes12_13_14[n-12];
    } else if (n == 15) {
        return this->palette15;
    }
    return std::nullopt;
}
