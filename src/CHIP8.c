#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CHIP8.h"

// STACK OPERATIONS ----------------------------------------------------------------------------------------------------

static void StackPush(CHIP8* chip8, uint16_t value) {
    if (chip8->SP == STACK_SIZE - 1) {
        fprintf(stderr, "[ERROR] Stack overflow\n");
        exit(1);
    }
    chip8->stack[chip8->SP++] = value;
}

static uint16_t StackPop(CHIP8* chip8) {
    if (chip8->SP == 0) {
        fprintf(stderr, "[ERROR] Stack underflow\n");
        exit(1);
    }
    return chip8->stack[--chip8->SP];
}

// INSTRUCTIONS --------------------------------------------------------------------------------------------------------

static void ClearScreen(CHIP8* chip8) {
    memset(chip8->display, 0, sizeof(chip8->display));
    chip8->displayUpdated = true;
}

static void Jump(CHIP8* chip8, uint16_t NNN) {
    chip8->PC = NNN;
}

static void StartSubroutine(CHIP8* chip8, uint16_t NNN) {
    StackPush(chip8, chip8->PC);
    chip8->PC = NNN;
}

static void EndSubroutine(CHIP8* chip8) {
    chip8->PC = StackPop(chip8);
}

static void SkipXEqual(CHIP8* chip8, uint8_t X, uint16_t NN) {
    if (chip8->V[X] == NN) chip8->PC += 2;
}

static void SkipXNotEqual(CHIP8* chip8, uint8_t X, uint16_t NN) {
    if (chip8->V[X] != NN) chip8->PC += 2;
}

static void SkipXYEqual(CHIP8* chip8, uint16_t X, uint16_t Y) {
    if (chip8->V[X] == chip8->V[Y]) chip8->PC += 2;
}

static void SkipXYNotEqual(CHIP8* chip8, uint16_t X, uint16_t Y) {
    if (chip8->V[X] != chip8->V[Y]) chip8->PC += 2;
}

static void SetRegisterVX(CHIP8* chip8, uint8_t X, uint8_t NN) {
    chip8->V[X] = NN;
}

static void AddToRegisterVX(CHIP8* chip8, uint8_t X, uint8_t NN) {
    chip8->V[X] += NN;
}

static void SetIndexRegister(CHIP8* chip8, uint16_t NNN) {
    chip8->I = NNN;
}

static void Draw(CHIP8* chip8, uint8_t X, uint8_t Y, uint8_t N) {
    const uint8_t* sprite = chip8->memory + chip8->I;
    const uint8_t xStart = chip8->V[X] & 63;
    uint8_t yPos = chip8->V[Y] & 31;
    chip8->V[0xF] = 0;

    for (uint8_t row = 0; row < N; ++row) {
        const uint8_t spriteByte = sprite[row];
        uint8_t xPos = xStart;
        for (uint8_t col = 0; col < 8; ++col) {
            const uint8_t spritePixel = spriteByte & (0x80 >> col);
            bool* screenPixel = &chip8->display[yPos * DISPLAY_WIDTH + xPos];
            if (spritePixel) {
                if (*screenPixel) {
                    chip8->V[0xF] = 1;
                }
                *screenPixel ^= 0xFF;
            }
            if (++xPos >= DISPLAY_WIDTH) break;
        }
        if (++yPos >= DISPLAY_HEIGHT) break;
    }
    chip8->displayUpdated = true;
}

// CHIP-8 --------------------------------------------------------------------------------------------------------------

static void InitFont(CHIP8* chip8) {
    const uint8_t fontData[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
        0x20, 0x60, 0x20, 0x20, 0x70,  // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
        0xF0, 0x80, 0xF0, 0x80, 0x80,  // F
    };
    memcpy(&chip8->memory[FONT_ADDR], fontData, sizeof(fontData));
}

CHIP8* CreateCHIP8() {
    CHIP8* chip8 = calloc(1, sizeof(CHIP8));
    if (chip8 == NULL) {
        fprintf(stderr, "[ERROR] Not enough memory\n");
        exit(1);
    }

    InitFont(chip8);

    return chip8;
}

void DestroyCHIP8(CHIP8* chip8) {
    free(chip8);
}

void LoadProgram(CHIP8* chip8, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "[ERROR] Failed to open file: %s\n", filename);
        exit(1);
    }

    const size_t bytesRead = fread(
        &chip8->memory[PROGRAM_ADDR],
        sizeof(uint8_t),
        MEMORY_SIZE - PROGRAM_ADDR,
        file
    );

    if (bytesRead == 0 && ferror(file)) {
        fprintf(stderr, "[ERROR] Failed to read file");
        fclose(file);
        exit(1);
    }

    fclose(file);

    chip8->PC = PROGRAM_ADDR;
}

void RunInstruction(CHIP8* chip8) {
    chip8->displayUpdated = false;

    // fetch
    const uint8_t hi = chip8->memory[chip8->PC++];
    const uint8_t lo = chip8->memory[chip8->PC++];
    const uint16_t opcode = hi << 8 | lo;

    // decode
    const uint8_t op = (opcode & 0xF000) >> 12;
    const uint8_t X = (opcode & 0x0F00) >> 8;
    const uint8_t Y = (opcode & 0x00F0) >> 4;
    const uint8_t N = (opcode & 0x000F);
    const uint8_t NN = (opcode & 0x00FF);
    const uint16_t NNN = (opcode & 0x0FFF);

    // execute
    switch (op) {
        case 0x0: {
            if (opcode == 0x00E0) {
                ClearScreen(chip8);
                return;
            }
            if (opcode == 0x00EE) {
                EndSubroutine(chip8);
                return;
            }
        } break;
        case 0x1: {
            Jump(chip8, NNN);
        } return;
        case 0x2: {
            StartSubroutine(chip8, NNN);
        } return;
        case 0x3: {
            SkipXEqual(chip8, X, NN);
        } return;
        case 0x4: {
            SkipXNotEqual(chip8, X, NN);
        } return;
        case 0x5: {
            SkipXYEqual(chip8, X, Y);
        } return;
        case 0x6: {
            SetRegisterVX(chip8, X, NN);
        } return;
        case 0x7: {
            AddToRegisterVX(chip8, X, NN);
        } return;
        case 0x9: {
            SkipXYNotEqual(chip8, X, Y);
        } return;
        case 0xA: {
            SetIndexRegister(chip8, NNN);
        } return;
        case 0xD: {
            Draw(chip8, X, Y, N);
        } return;
        default: break;
    }

    fprintf(stderr, "[ERROR] Unknown instruction: 0x%X\n", opcode);
    exit(1);
}
