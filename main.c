#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 4096
#define STACK_SIZE 16

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

#define FONT_ADDR 0x050
#define PROGRAM_ADDR 0x200

typedef struct CHIP8 {
    uint8_t memory[MEMORY_SIZE];
    bool display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    uint16_t PC;
    uint16_t I;
    uint16_t stack[16];
    uint8_t SP;
    uint8_t delayTimer;
    uint8_t soundTimer;
    uint8_t V[16];
} CHIP8;

static void InitFont(CHIP8* chip8) {
    const uint8_t fontData[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    memcpy(&chip8->memory[FONT_ADDR], fontData, sizeof(fontData));
}

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

static void ClearScreen(CHIP8* chip8) {
    memset(chip8->display, 0, sizeof(chip8->display));
}

static void Jump(CHIP8* chip8, uint16_t NNN) {
    chip8->PC = NNN;
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
}

static void RunInstruction(CHIP8* chip8) {
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
            }
        } break;
        case 0x1: {
            Jump(chip8, NNN);
        } break;
        case 0x6: {
            SetRegisterVX(chip8, X, NN);
        } break;
        case 0x7: {
            AddToRegisterVX(chip8, X, NN);
        } break;
        case 0xA: {
            SetIndexRegister(chip8, NNN);
        } break;
        case 0xD: {
            Draw(chip8, X, Y, N);
        } break;
        default: break;
    }

    fprintf(stderr, "[ERROR] Unknown instruction: 0x%X\n", opcode);
    exit(1);
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

int main(void) {
    CHIP8* chip8 = CreateCHIP8();

    for (;;) {
        RunInstruction(chip8);
        break;
    }

    DestroyCHIP8(chip8);
    return 0;
}
