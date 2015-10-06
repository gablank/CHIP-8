#include "SDL_Display.h"
#include <iostream>
#include "Computer.h"


SDL_Display::SDL_Display(Computer *computer)
        : Display(computer),
          width(640),
          height(320)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "Unable to initialize video: " << SDL_GetError() << std::endl;
        exit(1);
    }

    window = SDL_CreateWindow("CHIP-8 Emulator",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height, SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        std::cerr << "Unable to create window: " << SDL_GetError() << std::endl;
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr) {
        std::cerr << "Unable to get renderer: " << SDL_GetError() << std::endl;
        exit(1);
    }
}


SDL_Display::~SDL_Display() {

    SDL_Quit();
}


void SDL_Display::blit() const {
    // Clear the entire screen to black
    SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xff);
    SDL_RenderClear(renderer);

    // We draw all the pixels in white
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

    SDL_Rect pixel;
    pixel.w = width / 64;
    pixel.h = height / 32;
    for (int x = 0; x < 64; ++x) {
        for (int y = 0; y < 32; ++y) {
            if (computer->get_pixel(x, y)) {
                pixel.x = x*pixel.w;
                pixel.y = y*pixel.h;
//                std::cout << "Pixel size: " << "(" << pixel.w << ", " << pixel.h << ", " << pixel.x << ", " << pixel.y << ")" << std::endl;
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }

    SDL_RenderPresent(renderer);
}


void SDL_Display::set_window_size(int width, int height) {
    this->width = width;
    this->height = height;

    SDL_SetWindowSize(window, width, height);

    blit();
}