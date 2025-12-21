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

    DestroyCHIP8(chip8);
    return 0;
}
