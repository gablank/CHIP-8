#ifndef CHIP_8_COMPUTER_H
#define CHIP_8_COMPUTER_H


#include <stdint.h>
#include "CPU.h"
#include "Display.h"

class Computer {
public:
    Computer();

    uint8_t get_memory(uint16_t idx) const { return memory[idx]; }
    void set_memory(uint16_t idx, uint8_t val) { memory[idx] = val; }

    void run();

    bool get_pixel(int x, int y) const { return display_data[y * 64 + x]; }
    void set_pixel(int x, int y, bool val) { display_data[y * 64 + x] = val; }

    void reset();

    bool load(std::string file);

    bool get_key_state(int key) const { return keyboard[key]; }

    // Returns 0-9 for '0'-'9', 10-15 for 'a' to 'f', -1 otherwise
    int keysym_to_int(int keysym);

    virtual ~Computer() { SDL_FreeWAV(buzzer); }

    uint8_t *buzzer;
private:
    CPU cpu;
    uint8_t memory[4096];
    bool display_data[64*32];
    std::unique_ptr<Display> display;
    bool stepping;


    // true if key is down, false else
    bool keyboard[0xF];

    // Debug only. instruction should be little-endian
    void add_instruction(uint16_t idx, uint16_t instruction);
};


#endif //CHIP_8_COMPUTER_H
