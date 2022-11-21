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
              << "                       : rapidly flashing tiles.\n"
              << "                       : The framecount is optional and defaults to 256.\n"
              << " --unused-chapters     : Dump tilesets of any unused chapter indexes.\n"
              << " --width <width>       : Sets the width of the tileset image, in tiles.\n"
              << "                       : Mist be a power of 2 and less than " << MagickRenderer::MAX_WIDTH << '\n'
              << "                       : The default is 32.\n";
}

using ProcessCache = std::unordered_map<TilesetIndex, Tileset>;

struct Settings {
    snes::Rom romInfo;
    bool isBSFE = false, staticColor = false, enableGifs = false, enableFrames = false, unusedChapters = false;
    int frameCount;
    MagickRenderer r;

    bool process(snes::Chapter& chapter, std::istream& romFile, ProcessCache& cache)
    {
        auto chapterHeaderPtr = LunarSNEStoPC(0x8A8000 + (chapter.index*8), LC_LOROM, LC_NOHEADER);

        romFile.seekg(chapterHeaderPtr);
        auto tilesetIndex = romFile.get();
        auto paletteIndex = romFile.get();
        chapter.width = romFile.get();
        chapter.height = romFile.get();

        TilesetIndex tmp{tilesetIndex, paletteIndex};
        auto tilesetItr = cache.find(tmp);
        if (tilesetItr == cache.end()) {
            auto tilesAddress = romInfo.staticTiles.getAddress(romFile, tmp.tilesetIndex);
            auto tilesetAddress = romInfo.tileset.getAddress(romFile, tmp.tilesetIndex);
            auto animAddress = romInfo.animatedTiles.getAddress(romFile, tmp.tilesetIndex);
            auto brightnessPtr = romInfo.brightness.getAddress(romFile, paletteIndex);
            auto paletteAddress = romInfo.palette.getAddress(romFile, paletteIndex);

            Tileset tData{
                romFile,
                tilesAddress,
                animAddress,
                tilesetAddress
            };

            tData.staticColor = staticColor;

            tData.palettes = CGRam(
                romFile,
                paletteAddress,
                brightnessPtr,
                romInfo.getBaseColors()
            );
            tData.chapters.push_back(chapter);

            if (enableGifs || enableFrames) {
                r.writeAnim(tData, chapter, frameCount, enableGifs, isBSFE);
            } else {
                r.writePNG(tData, chapter, isBSFE);
            }

            cache[tmp] = std::move(tData);
            return true;
        } else {
            tilesetItr->second.chapters.push_back(chapter);
        }
        return false;
    }
};

struct searchOpt {
    std::string search;
    bool operator()(char* arg) const {
        return std::string{arg} == search;
    }
};

int main(int argc, char* argv[])
{
    if (argc == 1) {
        print_help(argv[0]);
        return 0;
    }
    
    auto optItr = std::find_if(argv, argv+argc, searchOpt{"--help"});
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

    MagickRenderer::loadMagick(argv[0]);
    optItr = std::find_if(argv, argv+argc, searchOpt{"--gif"});
    bool enableGifs = ((argv+argc) != optItr);
    bool enableFrames = false;
    if (!enableGifs) {
        optItr = std::find_if(argv, argv+argc, searchOpt{"--frames"});
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
    
    optItr = std::find_if(argv, argv+argc, searchOpt{"--static-color"});

    Settings s{snes::Rom(romFile)};
    s.staticColor = ((argv+argc) != optItr);
    s.enableFrames = enableFrames;
    s.enableGifs = enableGifs;
    s.isBSFE = (s.romInfo.chapters.size() == 1);
    s.frameCount = frameCount;
    s.unusedChapters = ((argv+argc) != std::find_if(argv, argv+argc, searchOpt{"--unused-chapters"}));

    optItr = std::find_if(argv, argv+argc, searchOpt{"--width"});
    if ((argv+argc) != optItr) {
        ++optItr;
        if ((argv+argc) != optItr) {
            try {
                s.r.width = std::stoi(std::string{*optItr});
                if (s.r.width != 1 && (
                        s.r.width == 0 ||
                        (s.r.width & (s.r.width - 1)) != 0 ||
                        s.r.width > MagickRenderer::MAX_WIDTH
                    )) {
                    std::cerr << "Invalid width provided.\n";
                    return 1;
                }
                s.r.height = MagickRenderer::MAX_WIDTH / s.r.width;
            } catch (std::exception& e) {
                std::cerr << "Invalid width provided.\n";
                return 1;
            }
        }
    } else {
        s.r.width = 32;
        s.r.height = 32;
    }

    ProcessCache chapterTilesets;
    for (auto& chapter: s.romInfo.chapters) {
        try {
            s.process(chapter, romFile, chapterTilesets);
        } catch (std::runtime_error &e) {
            std::cerr << e.what();
            return 7;
        }
    }
    if (s.unusedChapters && !s.romInfo.unusedChapterIndexes.empty()) {
        for (auto index: s.romInfo.unusedChapterIndexes) {
            snes::Chapter chapter{index, -1, 0, 0};
            try {
                s.process(chapter, romFile, chapterTilesets);
            } catch (std::runtime_error &e) {
                std::cerr << e.what();
                return 7;
            }
        }
    }
    romFile.close();
    if (s.romInfo.chapters.size() > 1) {
        for (auto& itr: chapterTilesets) {
            std::cout << "Tileset " << itr.first.tilesetIndex << " with palette " << itr.first.paletteIndex
                      << " is used by: ";
            for (auto& c: itr.second.chapters) {
                std::cout << snes::FE3Formatter.format(c) << ", ";
            }
            std::cout << '\n';
        }
    }
    
    LunarUnloadDLL();
    return 0;
}
