#include <iostream>
#include <sstream>
#include "Computer.h"

Display::Display(Computer *computer)
        : computer(computer) {
}

Display::~Display() {
}

void Display::blit() const {
    std::ostringstream out;
    for (int i = 0; i < 64; ++i) {
        for (int j = 0; j < 32; ++j) {
            if (computer->get_pixel(i, j)) {
                out << "\u25A0";
            }
            else {
                out << " ";
            }
        }
        out << std::endl;
    }
    std::cout << out.str();
}