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
static bool HandleEvents(CHIP8* chip8);
static void UpdateDisplay(const CHIP8* chip8);

int main(void) {
    InitDisplay();

    CHIP8* chip8 = CreateCHIP8();
    LoadProgram(chip8, "../roms/IBM Logo.ch8");

    bool running = true;

    while (running) {
        running = HandleEvents(chip8);

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

static bool HandleEvents(CHIP8* chip8) {
    static SDL_Event event;
    bool running = true;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_1: chip8->keys[ 0] = true; break;
                case SDL_SCANCODE_2: chip8->keys[ 1] = true; break;
                case SDL_SCANCODE_3: chip8->keys[ 2] = true; break;
                case SDL_SCANCODE_4: chip8->keys[ 3] = true; break;
                case SDL_SCANCODE_Q: chip8->keys[ 4] = true; break;
                case SDL_SCANCODE_W: chip8->keys[ 5] = true; break;
                case SDL_SCANCODE_E: chip8->keys[ 6] = true; break;
                case SDL_SCANCODE_R: chip8->keys[ 7] = true; break;
                case SDL_SCANCODE_A: chip8->keys[ 8] = true; break;
                case SDL_SCANCODE_S: chip8->keys[ 9] = true; break;
                case SDL_SCANCODE_D: chip8->keys[10] = true; break;
                case SDL_SCANCODE_F: chip8->keys[11] = true; break;
                case SDL_SCANCODE_Z: chip8->keys[12] = true; break;
                case SDL_SCANCODE_X: chip8->keys[13] = true; break;
                case SDL_SCANCODE_C: chip8->keys[14] = true; break;
                case SDL_SCANCODE_V: chip8->keys[15] = true; break;
                default: break;
            }
        }
        else if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_1: chip8->keys[ 0] = false; break;
                case SDL_SCANCODE_2: chip8->keys[ 1] = false; break;
                case SDL_SCANCODE_3: chip8->keys[ 2] = false; break;
                case SDL_SCANCODE_4: chip8->keys[ 3] = false; break;
                case SDL_SCANCODE_Q: chip8->keys[ 4] = false; break;
                case SDL_SCANCODE_W: chip8->keys[ 5] = false; break;
                case SDL_SCANCODE_E: chip8->keys[ 6] = false; break;
                case SDL_SCANCODE_R: chip8->keys[ 7] = false; break;
                case SDL_SCANCODE_A: chip8->keys[ 8] = false; break;
                case SDL_SCANCODE_S: chip8->keys[ 9] = false; break;
                case SDL_SCANCODE_D: chip8->keys[10] = false; break;
                case SDL_SCANCODE_F: chip8->keys[11] = false; break;
                case SDL_SCANCODE_Z: chip8->keys[12] = false; break;
                case SDL_SCANCODE_X: chip8->keys[13] = false; break;
                case SDL_SCANCODE_C: chip8->keys[14] = false; break;
                case SDL_SCANCODE_V: chip8->keys[15] = false; break;
                default: break;
            }
        }
    }
    return running;
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
