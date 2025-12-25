cmake_minimum_required(VERSION 4.0)
project(CHIP_8 C)

set(CMAKE_C_STANDARD 23)

find_package(SDL2 REQUIRED)

add_executable(CHIP_8 main.c
        src/CHIP8.c
        include/CHIP8.h)

target_include_directories(CHIP_8 PUBLIC include)
target_link_libraries(CHIP_8 SDL2::SDL2)
