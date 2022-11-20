#include "tileset.h"

bool operator==(TilesetIndex const& lhs, TilesetIndex const& rhs) {
    return (lhs.tilesetIndex == rhs.tilesetIndex) &&
        (lhs.paletteIndex == rhs.paletteIndex);
}


std::size_t std::hash<TilesetIndex>::operator()(TilesetIndex const& s) const noexcept
{
    std::size_t h1 = std::hash<int>{}(s.tilesetIndex);
    std::size_t h2 = std::hash<int>{}(s.paletteIndex);
    return h1 ^ (h2 << 1);
}

Tileset::Tileset(): pixmap(64*32*16, '\0')
{
}
