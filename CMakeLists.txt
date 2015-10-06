cmake_minimum_required(VERSION 2.8)
project(CHIP_8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Computer.cpp Computer.h CPU.cpp CPU.h Display.cpp Display.h SDL_Display.cpp SDL_Display.h utilities.cpp utilities.h)
add_executable(CHIP_8 ${SOURCE_FILES})
target_link_libraries(CHIP_8 SDL2)