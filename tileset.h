#pragma once
#include <functional>
#include <vector>
#include <iostream>

struct Chapter {
    int index, number, width, height;
    friend std::ostream& operator<<(std::ostream& os, const Chapter& cp);
    friend std::wostream& operator<<(std::wostream& os, const Chapter& cp);
};

struct Tileset {
    int tilesetIndex, paletteIndex;
    mutable std::vector<Chapter> chapters;
    friend bool operator==(Tileset const& lhs, Tileset const& rhs);
};

template<>
struct std::hash<Tileset> {
    std::size_t operator()(Tileset const& s) const noexcept;
};
