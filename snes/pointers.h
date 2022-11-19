#ifndef POINTERS_H
#define POINTERS_H

#include <iostream>
#include <vector>
#include <cstddef>
#include <cstdint>

#include "tileset.h"

namespace snes {
#pragma pack(push, 1)
    struct StandardHeader {
        char title[21];
        uint8_t mapper;
        uint8_t cartType;
        uint8_t romSize;
        uint8_t ramSize;
        uint8_t destination;
        uint8_t fixed;
        uint8_t romVersion;
        uint16_t checkComplement;
        uint16_t checkSum;

    };
    struct BSNESHeader {
        char title[16];
        uint32_t allocation;
        uint16_t starts : 16;
        uint8_t day, month;
        uint8_t romInfo;
        uint8_t programType;
        uint8_t fixed;
        uint8_t romVersion;
        uint16_t checkComplement;
        uint16_t checkSum;
    };


    struct OffsetPointer {
        int address;
        int offset;
        int getAddress(std::istream& in, int index ) const;
    };

    struct Rom
    {
        OffsetPointer palette;
        OffsetPointer brightness;
        int fixedPalette;
        OffsetPointer animatedTiles;
        OffsetPointer tileset;
        OffsetPointer staticTiles;
        std::vector<Chapter> chapters;
        Rom(std::istream& romFile);
    private:
        void getChapterData(std::istream& romFile, int chapterCount);
    };


#pragma pack(pop)
}

#endif // POINTERS_H
