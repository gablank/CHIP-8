#ifndef CHIP_8_UTILITIES_H
#define CHIP_8_UTILITIES_H

#include <stdint.h>
#include <stddef.h>

template<typename T>
T swap_endianness(T integral_number) {
    size_t size = sizeof(T);

    uint8_t *byte = (uint8_t *) &integral_number;
    for (size_t i = 0; i < size / 2; ++i) {
        uint8_t temp = byte[i];
        byte[i] = byte[(size-1)-i];
        byte[(size-1)-i] = temp;
    }

    return integral_number;
}

#endif //CHIP_8_UTILITIES_H
