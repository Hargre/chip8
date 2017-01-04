#ifndef _CHIP8_H_
#define _CHIP8_H_

#include <stdint.h>

struct chip8_cpu {
  int16_t opcode;
  int8_t  memory[0xFFF];
  int8_t  registers[16];

  int16_t i_register;
  int16_t program_counter;
  
  int16_t stack[16];
  int16_t stack_pointer;

  int8_t  key[16];
  int8_t  graphics[32 * 64];
  
  int8_t  delay_timer;
  int8_t  sound_timer;
}

#endif