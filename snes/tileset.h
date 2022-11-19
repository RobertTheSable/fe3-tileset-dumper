#pragma once
#include <functional>
#include <vector>
#include <iostream>

#include <string>
#include "cgram.h"

struct Chapter {
    int index, number, width, height;
    operator std::string() const;
    friend std::ostream& operator<<(std::ostream& os, const Chapter& cp);
    friend std::wostream& operator<<(std::wostream& os, const Chapter& cp);
};

struct TilesetIndex {
    int tilesetIndex, paletteIndex;
    friend bool operator==(TilesetIndex const& lhs, TilesetIndex const& rhs);
};

struct Tileset {
    std::vector<Chapter> chapters;
    std::vector<char> pixmap;
    std::vector<unsigned int> tiles;
    CGRam palettes;
    Tileset();
};

template<>
struct std::hash<TilesetIndex> {
    std::size_t operator()(TilesetIndex const& s) const noexcept;
};
