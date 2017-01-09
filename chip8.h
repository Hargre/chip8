#ifndef _CHIP8_H_
#define _CHIP8_H_

#include <stdint.h>

static const uint8_t fontset[] = {
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

typedef struct chip8 {
  uint16_t opcode;
  uint8_t  memory[0xFFF];
  uint8_t  registers[16];

  uint16_t i_register;
  uint16_t pc; // Program counter
  
  uint16_t stack[16];
  uint16_t sp; // Stack pointer

  uint8_t  key[16];
  uint8_t  graphics[64 * 32];

  uint8_t  delay_timer;
  uint8_t  sound_timer;
} chip8_t;

void init(chip8_t *chip8);
void load_rom(chip8_t *chip8, const char *filename);
void emulate_cycle(chip8_t *chip8);

#endif