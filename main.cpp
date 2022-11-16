#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <sstream>

#include <windows.h>

#include "DLLcode/LunarDLL.h"
#include "tileset.h"
#include "cgram.h"
#include "winbmp.h"

int getAddressFromFile(std::istream& file, unsigned int address)
{
    file.seekg(LunarSNEStoPC(address, LC_LOROM, LC_NOHEADER));
    int retVal = 0;
    file.read((char*)&retVal, 3);
    return retVal;
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
    
    auto addr = LunarSNEStoPC(0x848C16, LC_LOROM, LC_NOHEADER);
    romFile.seekg(addr);
    std::vector<Chapter> chapters{};
    for (int chapter = 1; chapter <= 44 ; ++chapter) {
        chapters.push_back({romFile.get(), chapter, 0, 0});
    }
    
    std::unordered_set<Tileset> chapterTilesets;
    for (auto& chapter: chapters) {
        auto chapterHeaderPtr = LunarSNEStoPC(0x8A8000 + (chapter.index*8), LC_LOROM, LC_NOHEADER);
        
        romFile.seekg(chapterHeaderPtr);
        auto tilesetIndex = romFile.get();
        auto paletteIndex = romFile.get();
        chapter.width = romFile.get();
        chapter.height = romFile.get();
        
        Tileset tmp{tilesetIndex, paletteIndex};
        if (chapterTilesets.find(tmp) == chapterTilesets.end()) {
            tmp.chapters.push_back(chapter);
            
            auto tilesAddress = 0x9D0000 + getAddressFromFile(romFile, 0x89C9A3 + (3 * tilesetIndex));

            romFile.seekg(LunarSNEStoPC(tilesAddress, LC_LOROM, LC_NOHEADER));

            auto tileData = decompress(romFile, 0x4000, 0x4000);
            
            std::vector<char> pixmap(64*32*16, '\0');
            if (!LunarCreatePixelMap((void*)&tileData[0], (void*)&pixmap[0], 32*16, LC_4BPP)) {
                std::cerr << "Failed to create pixmap!\n";
                return 4;
            }
            
            auto tilesetAddress = 0xA20000 + getAddressFromFile(romFile, 0x89CB6E + (3 * tilesetIndex));
            auto tilesetData = getMapTileSet(romFile, tilesetAddress);
            
            CGRam palettes{romFile, paletteIndex};
            
            BitMapHolder bp{};
            if (bp.handle == nullptr) {
                std::cerr << "Failed to create DIB\n";
                return 6;
            }
            
            int x = 0, y = 0, quarter = 0;
            for (auto& tile: tilesetData) {
                int xCoord = (x * 16) + ((quarter % 2) * 8);
                int yCoord = (y * 16) + ((quarter / 2) * 8);

                LunarRender8x8(
                    bp.bitPointer, 
                    512, 
                    512,
                    xCoord, 
                    yCoord, 
                    (void*)&pixmap[0], 
                    &palettes.colors[0], 
                    tile,
                    LC_DRAW
                );
                ++quarter;
                if (quarter == 4) {
                    ++x;
                    if (x == 32) {
                        x = 0;
                        ++y;
                    }
                    quarter = 0;
                }
            }
            try {
                auto info = CreateBitmapInfoStruct(bp.handle);
                std::wostringstream filenamestrm;
                filenamestrm << chapter << L".bmp";
                CreateBMPFile(filenamestrm.str().c_str(), info, bp.handle);
            
                LocalFree(info);
            } catch (std::runtime_error& e) {
                std::cerr << e.what() << '\n';
                return 7;
            }
            
            
            chapterTilesets.insert(tmp);
        } else {
            chapterTilesets.find(tmp)->chapters.push_back(chapter);
        }
    }
    
//     int tIdx = 0;
//     for (auto tSet: chapterTilesets) {
//         std::cout << "Tileset " << ++tIdx << " has " << tSet.chapters.size() << " chapters: ";
//         for (auto c : tSet.chapters) {
//             std::cout << c << ", ";
//         }
//         std::cout << "\n";
//         
//     }
    
    LunarUnloadDLL();
    return 0;
}
