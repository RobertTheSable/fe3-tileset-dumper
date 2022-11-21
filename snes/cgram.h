#pragma once
#include <vector>
#include <array>
#include <iostream>

#include "pointers.h"

int getAddressFromFile(std::istream& file, unsigned int address);

struct CGRam {
    std::vector<unsigned int> colors;
    CGRam()=default;
    CGRam(std::istream& file, int colorPtr, int brightnessPtr, const snes::BaseColors& fixedColors);
    bool update(int frame);
private:
    int brightIndex;
    std::array<unsigned int[2], 16> brightness;
};
