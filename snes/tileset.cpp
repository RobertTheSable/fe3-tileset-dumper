#include "tileset.h"

std::ostream& operator<<(std::ostream& os, const Chapter& cp)
{
    if (cp.number <= 20) {
        os << "Chapter 1-" << cp.number;
    } else if (cp.number <= 40) {
        os << "Chapter 2-" << (cp.number-20);
    } else {
        os << "Chapter F-" << (cp.number-40);
    }
    
    return os;
}
std::wostream& operator<<(std::wostream& os, const Chapter& cp) 
{
    if (cp.number <= 20) {
        os << L"Chapter 1-" << cp.number;
    } else if (cp.number <= 40) {
        os << L"Chapter 2-" << (cp.number-20);
    } else {
        os << L"Chapter F-" << (cp.number-40);
    }
    
    return os;
}
Chapter::operator std::string() const {
    if (number == 0) {
        return std::string{""};
    } else if (number <= 20) {
        return std::string{"Chapter 1-"} + std::to_string(number);
    } else if (number <= 40) {
        return std::string{"Chapter 2-"} + std::to_string(number-20);
    } else {
        return std::string{"Chapter F-"} + std::to_string(number-40);
    }
}

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
