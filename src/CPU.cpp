#include <iostream>
#include "CPU.h"
#include "Computer.h"

CPU::CPU(Computer *computer)
        : computer(computer)
{
    reset();
}

void CPU::reset() {
    pc = 0x200;

    for (int i = 0; i < 0xf; ++i) {
        V[i] = 0;
    }

    I = 0;

    for (int i = 0; i < 0xf; ++i) {
        stack[i] = 0;
    }
    stack_idx = 0;

    delay_timer = 0;
    sound_timer = 0;

    tick_of_previous_step = 0;
}

void CPU::step() {
    uint32_t cur_tick = SDL_GetTicks();
    // Don't step more than once each tick
    if (cur_tick == tick_of_previous_step) {
        return;
    }

    // Fetch upper part of opcode: 0xff00
    uint8_t op_msb = computer->get_memory(pc);

    // Fetch lower part of opcode: 0x00ff
    uint8_t op_lsb = computer->get_memory(pc + (uint16_t) 1);

    // Merge the parts together to form the entire opcode: 0xffff
    uint16_t opcode = op_msb << 8 | op_lsb;

//    std::cout << "Executing instruction: 0x" << std::hex << (int) opcode << std::endl;

    uint8_t x = (uint8_t) ((opcode >> 8) & 0xf);
    uint8_t y = (uint8_t) ((opcode >> 4) & 0xf);
    uint16_t nnn = (uint16_t) (opcode & 0xfff);
    uint8_t nn = (uint8_t) (opcode & 0xff);
    uint8_t n = (uint8_t) (opcode & 0xf);

    // Group the opcodes by the first 4 bits
    switch ((opcode & 0xf000) >> 12) {
        case 0x0: {
            switch (opcode & 0x000f) {
                // Clear the display
                case 0x0: {
                    for (int x = 0; x < 64; ++x) {
                        for (int y = 0; y < 32; ++y) {
                            computer->set_pixel(x, y, false);
                        }
                    }
                    break;
                }
                // Return from procedure
                case 0xE: {
                    pc = stack[stack_idx];
                    --stack_idx;
                    break;
                }
                default: {
                    std::cerr << "Error: Unknown opcode: 0x" << std::hex << opcode << std::endl;
                    exit(1);
                }
            }
            break;
        }
        // 0x1NNN: Jump to address at NNN
        case 0x1: {
            jump_to(nnn);
            break;
        }
        // 0x2NNN: Call subroutine at NNN
        case 0x2: {
            ++stack_idx;
            stack[stack_idx] = pc;
            jump_to(nnn);
            break;
        }
        // 0x3XNN: Skip next instruction if Vx equals NN
        case 0x3: {
            if (V[x] == nn) {
                pc += 2;
            }
            break;
        }
        // 0x4XNN: Skip next instruction if Vx doesn't equal NN
        case 0x4: {
            if (V[x] != nn) {
                pc += 2;
            }
            break;
        }
        // 0x5XY0: Skips next instruction if Vx equals Vy
        case 0x5: {
            if (V[x] == V[y]) {
                pc += 2;
            }
            break;
        }
        // 0x6XNN: Set Vx to NN
        case 0x6: {
            V[x] = nn;
            break;
        }
        // 0x7XNN: Add NN to Vx
        case 0x7: {
            V[x] += nn;
            break;
        }
//            8XY0	Sets VX to the value of VY.
//            8XY1	Sets VX to VX or VY.
//            8XY2	Sets VX to VX and VY.
//            8XY3	Sets VX to VX xor VY.
//            8XY4	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
//            8XY5	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
//            8XY6	Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.[2]
//            8XY7	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
//            8XYE	Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.[2]
        case 0x8: {
            switch (opcode & 0x000f) {
                case 0: {
                    V[x] = V[y];
                    break;
                }
                case 1: {
                    V[x] = V[y] | V[x];
                    break;
                }
                case 2: {
                    V[x] = V[y] & V[x];
                    break;
                }
                case 3: {
                    V[x] = V[y] ^ V[x];
                    break;
                }
                case 4: {
                    uint16_t total = V[x] + V[y];
                    // Check for overflow (bit number 9 will be set)
                    V[0xF] = 0;
                    if (total & 0x0100) {
                        V[0xF] = 1;
                    }
                    V[x] = (uint8_t) total;
                    break;
                }
                case 5: {
                    int16_t total = V[x] - V[y];
                    // Check for underflow (bit number 9 will be set)
                    V[0xF] = 1;
                    if (total < 0) {
                        V[0xF] = 0;
                    }
                    V[x] = (uint8_t) total;
                    break;
                }
                case 6: {
                    uint8_t least_significant = V[x] & (uint8_t) 0x1;
                    V[0xF] = least_significant;
                    V[x] >>= 1;
                    break;
                }
                case 7: {
                    int16_t total = V[y] - V[x];
                    // Check for overflow (bit number 9 will be set)
                    V[0xF] = 1;
                    if (total < 0) {
                        V[0xF] = 0;
                    }
                    V[x] = (uint8_t) total;
                    break;
                }
                case 0xE: {
                    uint8_t most_significant = V[x] & (uint8_t) 0x80;
                    V[0xF] = most_significant;
                    V[x] <<= 1;
                    break;
                }
                default: {
                    std::cerr << "Error: Unknown opcode: 0x" << std::hex << opcode << std::endl;
                    exit(1);
                }
            }
            break;
        }
        // 0x9XY0: Skips the next instruction if Vx does not equal to Vy
        case 0x9: {
            if (V[x] != V[y]) {
                pc += 2;
            }
            break;
        }
        // 0xANNN: Sets I to NNN
        case 0xA: {
            I = nnn;
            break;
        }
        // 0xBNNN: Jumps to the address NNN + V0
        case 0xB: {
            jump_to(nnn + V[0]);
            break;
        }
        // 0xCXNN: Sets Vx to the result of a bitwise and operation on a random number and NN
        case 0xC: {
            uint8_t random_number = rand() % 256;

            V[x] = random_number & nn;

            break;
        }
        // 0xDxyn: Draw sprite from I in memory to Vx, Vy on the screen. Width 8, height n
        case 0xD: {
            V[0xF] = 0;

            for (uint8_t j = 0; j < n; ++j) {
                uint8_t sprite_row = computer->get_memory(I + j);
                for (int i = 0; i < 8; ++i) {
                    int px = (V[x]+i) % 64;
                    int py = (V[y]+j) % 32;
                    bool cur = computer->get_pixel(px, py);
                    bool spr = false;
                    if (sprite_row & (0x80 >> i)) {
                        spr = true;
                    }

                    // If pixel was set and will be toggled off, we set Vf
                    if (cur && spr) {
                        V[0xF] = 1;
                    }

                    computer->set_pixel(px, py, cur ^ spr);
                }
            }
            break;
        }
        case 0xE: {
            switch (opcode & 0x00ff) {
                // Skip next instruction if the key with the value of V[x] is pressed
                case 0x9E: {
                    if (computer->get_key_state(V[x])) {
                        pc += 2;
                    }

                    break;
                }
                // Skip next instruction if the key with the value of V[x] is not pressed
                case 0xA1: {
                    if (!computer->get_key_state(V[x])) {
                        pc += 2;
                    }

                    break;
                }

                default: {
                    std::cerr << "Error: Unknown opcode: 0x" << std::hex << opcode << std::endl;
                    exit(1);
                }
            }

            break;
        }
        case 0xF: {
            switch (opcode & 0x00ff) {
                // Set Vx to the value of the delay timer
                case 0x07: {
                    V[x] = delay_timer;
                    break;
                }
                // Wait for a key press (all execution is halted until we get a keypress)
                // Store the value of the key pressed in Vx
                case 0x0A: {
                    SDL_Event e;

                    bool wait = true;
                    while (wait) {
                        SDL_PollEvent(&e);

                        switch (e.type) {
                            case SDL_QUIT: {
                                SDL_PushEvent(&e);
                                return;
                            }
                            case SDL_KEYDOWN: {
                                int key = computer->keysym_to_int(e.key.keysym.sym);

                                if (key != -1) {
                                    V[x] = (uint8_t) key;
                                }

                                wait = false;
                                break;
                            }

                            default:
                                ;
                        }
                    }
                }
                // Set delay timer equal to Vx
                case 0x15: {
                    delay_timer = V[x];
                    break;
                }
                // Set sound timer equal to Vx
                case 0x18: {
                    sound_timer = V[x];
                    break;
                }
                // Increment I by Vx
                case 0x1E: {
                    I = I + V[x];
                    break;
                }
                // Set I location of the sprite for digit Vx
                case 0x29: {
                    uint16_t digit_num = V[x];
                    I = (uint16_t) 0x050 + (uint16_t) 5 * digit_num;
                    break;
                }
                // Store BCD representation of the number in Vx into memory starting at I
                // Hundreds in I, tens in I+1, ones in I+2
                case 0x33: {
                    uint8_t ones = (uint8_t) (V[x] % 10);
                    uint8_t tens = ((uint8_t) (V[x] % 100) - ones) / 10;
                    uint8_t hundreds = ((uint8_t) (V[x] % 1000) - tens - ones) / 100;
                    computer->set_memory(I, hundreds);
                    computer->set_memory(I+1, tens);
                    computer->set_memory(I+2, ones);

                    break;
                }
                // Store the registers V0 *through* Vx into memory locations starting at I
                case 0x55: {
                    for (int i = 0; i <= x; ++i) {
                        computer->set_memory(I+i, V[i]);
                    }
                    break;
                }
                // Load the registers V0 *through* Vx from memory locations starting at I
                case 0x65: {
                    for (int i = 0; i <= x; ++i) {
                        V[i] = computer->get_memory(I+i);
                    }
                    break;
                }
            }
            break;
        }
        default: {
            std::cerr << "Error: Unknown opcode: 0x" << std::hex << opcode << std::endl;
            exit(1);
        }
    }

    // These should count down at 60Hz
    if (cur_tick % 16 == 0) {
        if (delay_timer > 0) {
            --delay_timer;
        }
        if (sound_timer > 0) {
            --sound_timer;
        }
    }

    // Instructions use two bytes, so we must increment this twice
    pc += 2;

    tick_of_previous_step = cur_tick;
}

void CPU::jump_to(uint16_t addr) {
    pc = addr - (uint16_t) 2;
}

void CPU::print_state() const {
    std::cout << "CPU state:" << std::endl;
    std::cout << "pc == 0x" << std::hex << (int)pc << std::endl;
    std::cout << "I == 0x" << std::hex << (int) I << std::endl;
    for (int i = 0; i <= 0xF; ++i) {
        if (V[i] != 0) {
            std::cout << "V[" << i << "] == 0x" << std::hex << (int)V[i] << std::endl;
        }
    }
    std::cout << "Delay timer == 0x" << std::hex << (int)delay_timer << std::endl;
    std::cout << "Sound timer == 0x" << std::hex << (int)sound_timer << std::endl;
}