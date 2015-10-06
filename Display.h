#ifndef CHIP_8_DISPLAY_H
#define CHIP_8_DISPLAY_H

#include <SDL2/SDL.h>

class Computer;

class Display {
public:
    Display(Computer *computer);
    virtual ~Display();

    virtual void blit() const;

protected:
    // Not owned by this class
    Computer *computer;
};


#endif //CHIP_8_DISPLAY_H
