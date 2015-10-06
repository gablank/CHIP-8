#include <iostream>
#include "Computer.h"
#include "SDL_Display.h"
#include <fstream>
#include <iomanip>


void fill_audio(void *udata, uint8_t *stream, int len) {
    if (len == 0) {
        return;
    }
    std::cout << "In callback" << std::endl;

    Computer *_this = (Computer *) udata;

    for (int i = 0; i < len; ++i) {
        stream[i] = _this->buzzer[0];
    }
}

Computer::Computer()
        : cpu(CPU(this)),
          stepping(false)
{
    display = std::unique_ptr<Display>(new SDL_Display(this));
//    SDL_Init(SDL_INIT_AUDIO);
//    SDL_AudioDeviceID audio_device;
//    SDL_AudioSpec want, have;
//    want.freq = 48000;
//    want.format = AUDIO_F32;
//    want.channels = 2;
//    want.samples = 4096;
//    want.callback = fill_audio;
//    want.userdata = this;
//    audio_device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
//    std::cout << have.freq << std::endl;
//    std::cout << have.format << std::endl;
//    std::cout << have.channels << std::endl;
//    std::cout << have.samples << std::endl;
//    std::cout << have.callback << std::endl;
//    uint32_t buzzer_length;
//    if (SDL_LoadWAV("buzzer.wav", &have, &buzzer, &buzzer_length) == NULL) {
//        std::cerr << "Could not open buzzer.wav: " << SDL_GetError() << std::endl;
//    }
//    SDL_PauseAudioDevice(audio_device, 0);
//    SDL_Delay(5000);
//    SDL_CloseAudioDevice(audio_device);

    reset();

    uint16_t idx = 0x200;
    // This program displays the numbers from 0-f on the screen in a loop.
    // Waits for the user to press 0 at f before looping.
//    /* 0x200 */ add_instruction(idx, 0x60ff); // load 0xff into V0
//    /* 0x202 */ add_instruction(idx += 2, 0xf015); // set delay timer to 0x120 (just for testing)
//    /* 0x204 */ add_instruction(idx += 2, 0x6000); // load 0 into V0
//    /* 0x206 */ add_instruction(idx += 2, 0x610f); // load 15 into V1
//    /* 0x208 */ add_instruction(idx += 2, 0x620a); // load 10 into V2
//    /* 0x20a */ add_instruction(idx += 2, 0xF029); // I = addr of sprite for digit in V0
//    /* 0x20c */ add_instruction(idx += 2, 0x0000); // Clear the display
//    /* 0x20e */ add_instruction(idx += 2, 0xD125); // Draw the digit from I
//    /* 0x210 */ add_instruction(idx += 2, 0x7001); // Add 1 to V0
//    /* 0x212 */ add_instruction(idx += 2, 0x3010); // Compare to 0x10, if equal, we skip the next instruction
//    /* 0x214 */ add_instruction(idx += 2, 0x120a); // Loop forever
//    /* 0x216 */ add_instruction(idx += 2, 0x6000); // Set V0 to 0
//    /* 0x218 */ add_instruction(idx += 2, 0xE09E); // Skip next instruction if 0 is pressed
//    /* 0x21a */ add_instruction(idx += 2, 0x1218); // Check for 0 pressed
//    /* 0x21c */ add_instruction(idx += 2, 0x120a); // Loop forever


    // Writes 0xff in decimal to the screen
    // registers:
    // v0-v2: general purpose
    // v3: x coordinate
    // v4: y coordinate
    // v5: constant (5)
    /* 0x200 */ add_instruction(idx,      0x60ff); // load 0xff into V0
    /* 0x202 */ add_instruction(idx += 2, 0x630a); // load 0x0a into V3
    /* 0x204 */ add_instruction(idx += 2, 0x640a); // load 0x0a into V4
    /* 0x206 */ add_instruction(idx += 2, 0x6505); // load 0x05 into V5
    /* 0x208 */ add_instruction(idx += 2, 0xa400); // set I = 0x400
    /* 0x20a */ add_instruction(idx += 2, 0xf033); // store BCD of V0 in I, I+1, I+2
    /* 0x20c */ add_instruction(idx += 2, 0xf265); // load I, I+1, I+2 into V0, V1, V2

    /* 0x20e */ add_instruction(idx += 2, 0xf029); // I = addr of sprite for digit in V0
    /* 0x210 */ add_instruction(idx += 2, 0xd345); // Draw the digit from I

    /* 0x212 */ add_instruction(idx += 2, 0x8354); // Add V5 to V3

    /* 0x214 */ add_instruction(idx += 2, 0xf129); // I = addr of sprite for digit in V1
    /* 0x216 */ add_instruction(idx += 2, 0xd345); // Draw the digit from I

    /* 0x218 */ add_instruction(idx += 2, 0x8354); // Add V5 to V3

    /* 0x21a */ add_instruction(idx += 2, 0xf229); // I = addr of sprite for digit in V2
    /* 0x21c */ add_instruction(idx += 2, 0xd345); // Draw the digit from I

    /* 0x21e */ add_instruction(idx += 2, 0x121e); // Loop forever here
}

void Computer::run() {
    SDL_Event e;

    bool run = true;
    while (run) {
//        cpu.print_state();

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                run = false;
            }

            else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                bool state = e.type == SDL_KEYDOWN;

                int key = keysym_to_int(e.key.keysym.sym);

                if (key != -1) {
                    keyboard[key] = state;
                }
            }
        }

        cpu.step();

        display->blit();

        while (stepping) {
            if (SDL_PollEvent(&e) != 0) {
                SDL_PushEvent(&e);

                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        run = false;
                    }
                    break;
                }
            }
        }
    }
}

void Computer::reset() {
    // Reset the display
    for (int i = 0; i < 64*32; ++i) {
        display_data[i] = false;
    }

    // Create the predefined hex digits, starting at 0x050 and ending at 0x0A0
    int idx = 0x050;

    // 0
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b10010000;
    memory[idx++] = 0b10010000;
    memory[idx++] = 0b10010000;
    memory[idx++] = 0b11110000;
    // 1
    memory[idx++] = 0b00100000;
    memory[idx++] = 0b01100000;
    memory[idx++] = 0b00100000;
    memory[idx++] = 0b00100000;
    memory[idx++] = 0b01110000;
    // 2
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b00010000;
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b10000000;
    memory[idx++] = 0b11110000;
    // 3
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b00010000;
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b00010000;
    memory[idx++] = 0b11110000;
    // 4
    memory[idx++] = 0b10010000;
    memory[idx++] = 0b10010000;
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b00010000;
    memory[idx++] = 0b00010000;
    // 5
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b10000000;
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b00010000;
    memory[idx++] = 0b11110000;
    // 6
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b10000000;
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b10010000;
    memory[idx++] = 0b11110000;
    // 7
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b00010000;
    memory[idx++] = 0b00100000;
    memory[idx++] = 0b01000000;
    memory[idx++] = 0b01000000;
    // 8
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b10010000;
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b10010000;
    memory[idx++] = 0b11110000;
    // 9
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b10010000;
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b00010000;
    memory[idx++] = 0b11110000;
    // A
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b10010000;
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b10010000;
    memory[idx++] = 0b10010000;
    // B
    memory[idx++] = 0b11100000;
    memory[idx++] = 0b10010000;
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b10010000;
    memory[idx++] = 0b11100000;
    // C
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b10000000;
    memory[idx++] = 0b10000000;
    memory[idx++] = 0b10000000;
    memory[idx++] = 0b11110000;
    // D
    memory[idx++] = 0b11100000;
    memory[idx++] = 0b10010000;
    memory[idx++] = 0b10010000;
    memory[idx++] = 0b10010000;
    memory[idx++] = 0b11100000;
    // E
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b10000000;
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b10000000;
    memory[idx++] = 0b11110000;
    // F
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b10000000;
    memory[idx++] = 0b11110000;
    memory[idx++] = 0b10000000;
    memory[idx++] = 0b10000000;

    // Reset keyboard state
    for (int i = 0; i < 0xF; ++i) {
        keyboard[i] = false;
    }
}

bool Computer::load(std::string file) {
    std::ifstream in;

    try {
        // Open for reading, in binary, at the end (to tell the file size)
        in.open(file, std::ios_base::in | std::ios_base::binary | std::ios_base::ate);
    }
    catch (std::exception e) {
        std::cerr << "Unable to open file: " << file << std::endl;
        return false;
    }

    if (in) {
        in.seekg(0, std::ios_base::end);
        size_t length = in.tellg();
        in.seekg(0, std::ios_base::beg);

        // Load the program starting at address 0x200
        in.read((char *) &(memory[0x200]), length);

        /*uint8_t *instruction = (uint8_t *) &memory[0];
        for (int i = 0x200; i < 0x300; ++i) {
            std::cout << std::hex << "0x" << i << " == 0x" << std::setfill('0') << std::setw(2) << (int)instruction[i] << std::setfill('0') << std::setw(2)  << (int)instruction[i+1] << std::endl;
            instruction += 2;
        }*/

        cpu.reset();

        return true;
    }

    std::cerr << "Unable to open the file!" << std::endl;

    return false;
}

// Here is the keyboard mapping
int Computer::keysym_to_int(int keysym) {
    switch (keysym) {
        case SDLK_0:
            return 0;
        case SDLK_1:
            return 1;
        case SDLK_2:
            return 2;
        case SDLK_3:
            return 3;
        case SDLK_4:
            return 'c'-'a'+10;
        case SDLK_q:
            return 4;
        case SDLK_w:
            return 5;
        case SDLK_e:
            return 6;
        case SDLK_r:
            return 'd'-'a'+10;
        case SDLK_a:
            return 7;
        case SDLK_s:
            return 8;
        case SDLK_d:
            return 9;
        case SDLK_f:
            return 'e'-'a'+10;
        case SDLK_z:
            return 'a'-'a'+10;
        case SDLK_x:
            return 0;
        case SDLK_c:
            return 'b'-'a'+10;
        case SDLK_v:
            return 'f'-'a'+10;
        default:
            return -1;
    }
}

void Computer::add_instruction(uint16_t idx, uint16_t instruction) {
    // instruction: lsB | msB
    // we need: msB | lsB
    uint8_t *byte = (uint8_t *) &instruction;
    // Add the msb first
    memory[idx] = byte[1];
    memory[idx+1] = byte[0];
}