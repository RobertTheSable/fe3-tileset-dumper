#pragma once
#include <functional>
#include <vector>
#include <array>
#include <iostream>

#include <string>
#include "chapter.h"
#include "cgram.h"
#include "pointers.h"

struct TilesetIndex {
    int tilesetIndex, paletteIndex;
    friend bool operator==(TilesetIndex const& lhs, TilesetIndex const& rhs);
};

struct Tileset {
    using AnimBuffer = std::array<char, 0x800>;
    std::vector<snes::Chapter> chapters;
    std::vector<char> rawTiles, pixmap;
    std::vector<unsigned int> tiles;
    std::vector<AnimBuffer> animatedTiles;
    bool update(int frame);
    CGRam palettes;
    Tileset()=default;
    Tileset(Tileset&&)=default;
    Tileset& operator=(Tileset&&)=default;
    Tileset(
        std::istream& file, 
        int staticTilesAddress, 
        int animTilesAddress, 
        int tilesetAddress
    );
    operator bool() const;
    bool staticColor = false;
private:
    unsigned int tileIndex = 0;
    bool valid = false;
};

template<>
struct std::hash<TilesetIndex> {
    std::size_t operator()(TilesetIndex const& s) const noexcept;
};
