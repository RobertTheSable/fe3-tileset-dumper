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

void print_help(char* name)
{
    std::cout << "Usage: " << name << " <FILE> [OPTIONS]\n"
              << "Generate tilesets from an FE3 or BSFE rom.\n"
              << "<FILE> must be one of:\n"
              << " - An unheadered FE3 rom.\n"
              << " - A rom of one of the BSFE games.\n"
              << "If no options are given, a PNG of each tileset used in the ROM is created.\n"
              << "For FE3, the output file name is first chapter which uses the tileset.\n"
              << "For BSFE, the game's identifier (BSFE1, 2, etc) is used instead.\n"
              << "Supported Options:\n"
              << " --gif [framecount]    : Generate a tileset as an animated gif.\n"
              << "                       : The framecount is optional and defaults to 256.\n"
              << " --frames [framecount] : Generate a set of PNGs for each updated frame \n"
              << "                       : in the fileset.\n"
              << "                       : The files will be saved as\n"
              << "                       :   {tileset identifier}/frame-{frame}.png\n"
              << "                       : The framecount is optional and defaults to 256.\n"
              << " --static-color        : Skip updating frames when the palette changes.\n"
              << "                       : May be useful if a tileset has buggy \n"
              << "                       : color change info which would therwise produce.\n"
              << "                       : rapidly flashing tiles.\n";
}

int main(int argc, char* argv[])
{
    
    if (argc == 1) {
        print_help(argv[0]);
        return 0;
    }
    
    auto optItr = std::find_if(argv, argv+argc, [](char* arg) -> bool {
        return std::string{arg} == "--help";
    });
    if ((argv+argc) != optItr) {
        print_help(argv[0]);
        return 0;
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
    
    optItr = std::find_if(argv, argv+argc, [](char* arg) -> bool {
        return std::string{arg} == "--gif";
    });
    bool enableGifs = ((argv+argc) != optItr);
    bool enableFrames = false;
    if (!enableGifs) {
        optItr = std::find_if(argv, argv+argc, [](char* arg) -> bool {
            return std::string{arg} == "--frames";
        });
        enableFrames = ((argv+argc) != optItr);
    }
    
    int frameCount = (8 * 32);
    if ((argv+argc) != optItr) {
        ++optItr;
        if ((argv+argc) != optItr && *optItr[0] != '-') {
            try {
                std::string count{*optItr};
                frameCount = std::stoi(count);
            } catch (std::invalid_argument& e) {
                std::cerr << "Ignoring invalid frame count.\n";
            } catch (std::out_of_range& e) {
                std::cerr << "Ignoring too large frame count.\n";
            }
        }
    }
    
    optItr = std::find_if(argv, argv+argc, [](char* arg) -> bool {
        return std::string{arg} == "--static-color";
    });
    
    bool staticColor = ((argv+argc) != optItr);

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
        auto tilesetItr = chapterTilesets.find(tmp);
        if (tilesetItr == chapterTilesets.end()) {
            try {
                auto tilesAddress = romInfo.staticTiles.getAddress(romFile, tmp.tilesetIndex);
                auto tilesetAddress = romInfo.tileset.getAddress(romFile, tmp.tilesetIndex);
                auto animAddress = romInfo.animatedTiles.getAddress(romFile, tmp.tilesetIndex);
                
                Tileset tData{
                    romFile, 
                    tilesAddress, 
                    animAddress, 
                    tilesetAddress
                };
                
                tData.staticColor = staticColor;
                
                auto brightnessPtr = romInfo.brightness.getAddress(romFile, paletteIndex);
                tData.palettes = CGRam(
                    romFile,
                    romInfo.palette.getAddress(romFile, paletteIndex),
                    brightnessPtr,
                    romInfo.getBaseColors()
                );
                tData.chapters.push_back(chapter);
                
                if (enableGifs || enableFrames) {
                    writeAnim(tData, chapter, frameCount, enableGifs, isBSFE);
                } else {
                    writePNG(tData, chapter, isBSFE);
                }
                
                chapterTilesets[tmp] = std::move(tData);
            } catch (std::runtime_error &e) {
                std::cerr << e.what();
                return 7;
            }
        } else {
            tilesetItr->second.chapters.push_back(chapter);
        }
    }
    romFile.close();
    
    LunarUnloadDLL();
    return 0;
}
