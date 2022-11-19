#pragma once
#include <vector>
#include <iostream>

int getAddressFromFile(std::istream& file, unsigned int address);

struct CGRam {
    std::vector<unsigned int> colors;
    CGRam()=default;
    CGRam(std::istream& file, int index);
};
