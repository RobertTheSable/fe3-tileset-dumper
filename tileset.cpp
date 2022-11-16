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

bool operator==(Tileset const& lhs, Tileset const& rhs) {
    return (lhs.tilesetIndex == rhs.tilesetIndex) &&
        (lhs.paletteIndex == rhs.paletteIndex);
}


std::size_t std::hash<Tileset>::operator()(Tileset const& s) const noexcept 
{
    std::size_t h1 = std::hash<int>{}(s.tilesetIndex);
    std::size_t h2 = std::hash<int>{}(s.paletteIndex);
    return h1 ^ (h2 << 1);
}
