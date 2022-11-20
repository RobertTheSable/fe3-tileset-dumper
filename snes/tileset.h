#pragma once
#include <functional>
#include <vector>
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
    std::vector<snes::Chapter> chapters;
    std::vector<char> pixmap;
    std::vector<unsigned int> tiles;
    CGRam palettes;
    Tileset();
};

template<>
struct std::hash<TilesetIndex> {
    std::size_t operator()(TilesetIndex const& s) const noexcept;
};
