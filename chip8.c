#include <stdio.h>
#include <string.h>

#include "chip8.h"

void init(chip8_t *chip8) {
  chip8->program_counter = 0x200; // Program initial address
  chip8->opcode          = 0;
  chip8->i_register      = 0;
  chip8->stack_pointer   = 0;

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