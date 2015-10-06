#ifndef CHIP_8_CPU_H
#define CHIP_8_CPU_H


#include <stdint.h>

#include <memory>

class Computer;

class CPU {
public:
    CPU(Computer *computer);

    void reset();

    void step();

    // Makes sure the next executed instruction is at address addr
    void jump_to(uint16_t addr);

    void print_state() const;

    virtual ~CPU() {};

private:
    // Not owned by this class
    Computer *computer;

    uint8_t V[16];
    uint16_t I;

    uint16_t pc;

    uint16_t stack[16];
    uint8_t stack_idx;

    uint8_t delay_timer;
    uint8_t sound_timer;

    // What tick it was when we last stepped
    uint32_t tick_of_previous_step;
};


#endif //CHIP_8_CPU_H
