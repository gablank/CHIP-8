cmake_minimum_required(VERSION 2.8)
project(CHIP_8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        src/main.cpp
        src/Computer.cpp
        src/Computer.h
        src/CPU.cpp
        src/CPU.h
        src/Display.cpp
        src/Display.h
        src/SDL_Display.cpp
        src/SDL_Display.h
        src/utilities.cpp
        src/utilities.h
)
add_executable(CHIP_8 ${SOURCE_FILES})
target_link_libraries(CHIP_8 SDL2)