#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <unordered_map>

#include <windows.h>

#include "LunarDLL.h"
#include "tileset.h"
#include "cgram.h"
#include "magick.h"
#include "pointers.h"

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

int main(int argc, char* argv[])
{
    if (argc == 1) {
        return 1;
    }
    std::ifstream romFile(argv[1], std::ios_base::binary|std::ios_base::in);
    if (!romFile) {
        std::cerr << "Counldn't open rom.\n";
        return 2;
    }

    if (!LunarLoadDLL())
    {
        std::cerr << "Could not load Lunar Compress.dll.\n";
        return 3;
    }
    loadMagick(argv[0]);

    auto romInfo = snes::Rom(romFile);

    std::unordered_map<TilesetIndex, Tileset> chapterTilesets;
    bool isBSFE = (romInfo.chapters.size() == 1);
    for (auto& chapter: romInfo.chapters) {
        auto chapterHeaderPtr = LunarSNEStoPC(0x8A8000 + (chapter.index*8), LC_LOROM, LC_NOHEADER);
        
        romFile.seekg(chapterHeaderPtr);
        auto tilesetIndex = romFile.get();
        auto paletteIndex = romFile.get();
        chapter.width = romFile.get();
        chapter.height = romFile.get();
        
        TilesetIndex tmp{tilesetIndex, paletteIndex};
        if (chapterTilesets.find(tmp) == chapterTilesets.end()) {
            Tileset tData{};
            auto brightnessPtr = romInfo.brightness.getAddress(romFile, paletteIndex);
            tData.palettes = CGRam(
                    romFile,
                    romInfo.palette.getAddress(romFile, paletteIndex),
                    brightnessPtr,
                    romInfo.getBaseColors()
            );
            tData.chapters.push_back(chapter);
            
            auto tilesAddress = romInfo.staticTiles.getAddress(romFile, tmp.tilesetIndex);

            romFile.seekg(LunarSNEStoPC(tilesAddress, LC_LOROM, LC_NOHEADER));

            auto tileData = decompress(romFile, 0x4000, 0x4000);

            if (!LunarCreatePixelMap((void*)&tileData[0], (void*)&tData.pixmap[0], 32*16, LC_4BPP)) {
                std::cerr << "Failed to create pixmap!\n";
                return 4;
            }
            
            auto tilesetAddress = romInfo.tileset.getAddress(romFile, tilesetIndex);
            tData.tiles = getMapTileSet(romFile, tilesetAddress);

            try {
                writeGif(tData, chapter, isBSFE);
            } catch (std::runtime_error &e) {
                std::cerr << e.what();
                return 7;
            }
            chapterTilesets[tmp] = tData;;
        } else {
            chapterTilesets[tmp].chapters.push_back(chapter);
        }
    }
    
    LunarUnloadDLL();
    return 0;
}
