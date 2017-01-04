#include <stdio.h>
#include <string.h>

#include "chip8.h"

void init(chip8_t *chip8) {
  chip8->pc         = 0x200; // Program initial address
  chip8->opcode     = 0;
  chip8->i_register = 0;
  chip8->sp         = 0;

  memset(chip8->memory,    0, sizeof(chip8->memory));
  memset(chip8->registers, 0, sizeof(chip8->registers));
  memset(chip8->stack,     0, sizeof(chip8->stack));
  memset(chip8->key,       0, sizeof(chip8->key));
  memset(chip8->graphics,  0, sizeof(chip8->graphics));
}

void load_rom(chip8_t *chip8, const char *filename) {
  long size;

  init(chip8);

  FILE *rom = fopen(filename, "rb");

  fseek(rom, 0, SEEK_END);
  size = ftell(rom);
  rewind(rom);

  fread(&chip8->memory[0x200], size, 1, rom);
  fclose(rom);
}

void emulate_cycle(chip8_t *chip8) {
  /* Fetch 
   * Get two bytes from memory, combine them into opcode word 
   */
  chip8->opcode = chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc + 1];

  /* Decode 
   * Reads first four bits from opcode, switchs to corresponding instruction
   */
  switch (chip8->opcode & 0xF000) {
    case 0x0000:
      switch (chip8->opcode & 0x000F) {
        /* 0x00E0: Clears the screen */
        case 0x0000: 
          memset(chip8->graphics, 0, sizeof(chip8->graphics));
          chip8->pc += 2;
        break;

        /* 0x00EE: Returns from a subroutine */
        case 0x000E:
          chip8->sp--;
          chip8->pc = chip8->stack[chip8->sp];
          chip8->pc += 2;
        break;

        default:
          printf("Unknown opcode [0x0000]: 0x%X\n", chip8->opcode);
        break;
      }
    break;

    /* 0x1NNN: Jumps to address NNN */
    case 0x1000:
      chip8->pc = chip8->opcode & 0x0FFF;
    break;

    /* 0x2NNN: Calls subroutine at NNN */
    case 0x2000:
      chip8->stack[chip8->sp] = chip8->pc;
      chip8->sp++;
      chip8->pc = chip8->opcode & 0x0FFF;
    break;

    /* 0x3XNN: Skips instruction if VX equals NN */
    case 0x3000:
      if (chip8->registers[(chip8->opcode & 0x0F00) >> 8] == (chip8->opcode & 0x00FF)) {
        chip8->pc += 4;
      }
      else {
        chip8->pc += 2;
      }
    break;
  }
}