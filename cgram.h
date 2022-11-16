#pragma once
#include <vector>
#include <iostream>

int getAddressFromFile(std::istream& file, unsigned int address);

struct CGRam {
    std::vector<unsigned int> colors;
    CGRam(std::istream& file, int index);
};
