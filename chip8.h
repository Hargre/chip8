#ifndef _CHIP8_H_
#define _CHIP8_H_

#include <stdint.h>

typedef struct chip8 {
  uint16_t opcode;
  uint8_t  memory[0xFFF];
  uint8_t  registers[16];

  uint16_t i_register;
  uint16_t program_counter;
  
  uint16_t stack[16];
  uint16_t stack_pointer;

  uint8_t  key[16];
  uint8_t  graphics[64 * 32];

  uint8_t  delay_timer;
  uint8_t  sound_timer;
} chip8_t;

void init(chip8_t *chip8);
void load_rom(chip8_t *chip8, const char *filename);
void emulate_cycle(chip8_t *chip8);

#endif