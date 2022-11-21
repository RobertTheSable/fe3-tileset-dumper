#ifndef CHAPTER_H
#define CHAPTER_H
#include <iostream>
#include <string>
#include <array>

namespace snes {

struct Chapter {
    int index, number, width, height;
};

template<int n>
struct ChapterFormatter {
    int limits[n];
    std::string labels[n];
    std::string format(const Chapter& c) const {
        for (int i = 0; i < n ; ++i) {
            if (c.number > limits[i]) {
                return labels[i] + std::to_string(c.number - limits[i]);
            }
        }
        return std::string{"Debug Chapter"};
    }
};

const ChapterFormatter<3> FE3Formatter{{40, 20, 0}, {"Chapter F-", "Chapter 2-" , "Chapter 1-"}};
const ChapterFormatter<1> BSFEFormatter{{0}, {"BSFE"}};

}

#endif // CHAPTER_H
