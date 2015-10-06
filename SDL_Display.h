#ifndef CHIP_8_SDL_DISPLAY_H
#define CHIP_8_SDL_DISPLAY_H

#include <SDL2/SDL.h>
#include "Display.h"

class SDL_Display : public Display {
public:
    SDL_Display(Computer *computer);
    virtual ~SDL_Display();

    void blit() const override;
    void set_window_size(int width, int height);

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *pixel;

    int width;
    int height;
};


#endif //CHIP_8_SDL_DISPLAY_H
