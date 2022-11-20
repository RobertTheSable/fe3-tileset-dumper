#ifndef POINTERS_H
#define POINTERS_H

#include <iostream>
#include <vector>
#include <array>
#include <optional>
#include <cstddef>
#include <cstdint>
#include "chapter.h"

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
    struct BSHeader {
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
#pragma pack(pop)

    using Palette = std::array<unsigned int, 16>;
    struct BaseColors {
        std::array<Palette, 3> palettes5_6_7;
        Palette palette0, palette8, palette11, palette15;
        std::array<Palette, 3> palettes12_13_14;
        std::optional<Palette> operator[](int n) const;
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
        const BaseColors& getBaseColors() const;
    private:
        BaseColors baseColors;
        void getChapterData(std::istream& romFile, int chapterCount);
    };
}

#endif // POINTERS_H
