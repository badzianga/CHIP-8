#ifndef CHIP8_H
#define CHIP8_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

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
    bool displayUpdated;
    uint8_t V[16];
} CHIP8;

CHIP8* CreateCHIP8();
void DestroyCHIP8(CHIP8* chip8);
void RunInstruction(CHIP8* chip8);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //CHIP8_H
