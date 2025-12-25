#include <stdio.h>
#include <SDL2/SDL.h>
#include "CHIP8.h"

#define PIXEL_SIZE 16
#define WINDOW_WIDTH (DISPLAY_WIDTH * PIXEL_SIZE)
#define WINDOW_HEIGHT (DISPLAY_HEIGHT * PIXEL_SIZE)

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;

static void InitDisplay();
static void DestroyDisplay();
static void UpdateDisplay(const CHIP8* chip8);

int main(void) {
    InitDisplay();

    CHIP8* chip8 = CreateCHIP8();
    LoadProgram(chip8, "../IBM Logo.ch8");

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        RunInstruction(chip8);

        UpdateDisplay(chip8);
        SDL_Delay(16);
    }

    DestroyDisplay();
    DestroyCHIP8(chip8);

    return 0;
}

static void InitDisplay() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "[ERROR] SDL_Init error: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow(
        "CHIP-8",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_HIDDEN
    );

    if (!window) {
        printf("[ERROR] SDL_CreateWindow error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        printf("[ERROR] SDL_CreateRenderer error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_ShowWindow(window);
}

static void DestroyDisplay() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

static void UpdateDisplay(const CHIP8* chip8) {
    if (chip8->displayUpdated) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        for (int y = 0; y < DISPLAY_HEIGHT; y++) {
            for (int x = 0; x < DISPLAY_WIDTH; x++) {
                if (chip8->display[y * DISPLAY_WIDTH + x]) {
                    SDL_Rect pixel = {x * PIXEL_SIZE + 1, y * PIXEL_SIZE - 1, PIXEL_SIZE - 2, PIXEL_SIZE - 2};
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }
        SDL_RenderPresent(renderer);
    }
}
