#include "Computer.h"

int main(int argc, char **argv) {
    Computer computer;

    if (argc > 1) {
        computer.load(argv[1]);
    }

    computer.run();

    return 0;
}