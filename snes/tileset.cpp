#include "tileset.h"

#include <algorithm>

#include <windows.h>
#include "LunarDLL.h"

bool operator==(TilesetIndex const& lhs, TilesetIndex const& rhs) {
    return (lhs.tilesetIndex == rhs.tilesetIndex) &&
        (lhs.paletteIndex == rhs.paletteIndex);
}

auto decompress(std::istream& file, std::size_t inSize, std::size_t outSize) 
{
    std::vector<char> compBuffer(inSize, '\0'), decompBuffer(outSize, '\0');
    file.read(&compBuffer[0], inSize);
        
    LunarOpenRAMFile((void*)&compBuffer[0], LC_READONLY|LC_LOCKARRAYSIZE, inSize);
    
    auto decompSize = LunarDecompress((void*)&decompBuffer[0], 0, decompBuffer.size(), LC_LZ5, 0, nullptr);
    
    LunarCloseFile();
    
    decompBuffer.resize(decompSize);
    return decompBuffer;
}

auto getMapTileSet(std::istream& file, unsigned int address)
{
    file.seekg(LunarSNEStoPC(address, LC_LOROM, LC_NOHEADER));

    auto tileSetData = decompress(file, 0x2000, 0x2000);

    std::vector<unsigned int> formattedTileSet(0x1000, 0);
    auto tileItr = tileSetData.begin();
    for (auto& tile: formattedTileSet) {
        tile = (*tileItr++) & 0xFF;
        unsigned short tmp = *tileItr++;
        tile |= (tmp<<8); 
    }
    return formattedTileSet;
}

std::size_t std::hash<TilesetIndex>::operator()(TilesetIndex const& s) const noexcept
{
    std::size_t h1 = std::hash<int>{}(s.tilesetIndex);
    std::size_t h2 = std::hash<int>{}(s.paletteIndex);
    return h1 ^ (h2 << 1);
}

Tileset::Tileset(
    std::istream& file, 
    int staticTilesAddress, 
    int animTilesAddress, 
    int tilesetAddress
): pixmap(64*32*16, '\0'), tiles(getMapTileSet(file, tilesetAddress)) {
    file.seekg(LunarSNEStoPC(staticTilesAddress, LC_LOROM, LC_NOHEADER));

    rawTiles = decompress(file, 0x4000, 0x4000);

    if (!LunarCreatePixelMap((void*)&rawTiles[0], (void*)&pixmap[0], 32*16, LC_4BPP)) {
        throw std::runtime_error("Failed to create pixmap!");
    }
    
    file.seekg(LunarSNEStoPC(animTilesAddress, LC_LOROM, LC_NOHEADER));
    animatedTiles = std::vector<AnimBuffer>(8);
    
    for (auto& pixBuffer: animatedTiles) {
        file.read(pixBuffer.data(), 0x800);
        if (!file) {
            throw std::runtime_error("invalid address");
        }
    }
    valid = true;
}

Tileset::operator bool() const
{
    return valid;
}

bool Tileset::update(int frame)
{
    bool result = false;
    if ((frame & 7) == 2) {
        if ((tileIndex %2) == 0) {
            auto tmpIndex = (tileIndex & 0xE) >> 1;
            if (tmpIndex > 7) {
                throw std::runtime_error("Tile frame out of bounds.");
            }
            std::copy_n(
                animatedTiles[tmpIndex].rbegin(), 
                0x800, 
                rawTiles.rbegin()
            );
            if (!LunarCreatePixelMap((void*)rawTiles.data(), (void*)&pixmap[0], 32*16, LC_4BPP)) {
                throw std::runtime_error("Failed to create pixmap!");
            }
            result = true;
        }
        ++tileIndex;
    }
    if (staticColor) {
        return result;
    }
    return result || palettes.update(frame);
}
