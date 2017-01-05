#include <stdio.h>
#include <stdlib.h>
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

    /* 0x4XNN: Skips instruction if VX doesn't equal NN */
    case 0x4000:
      if (chip8->registers[(chip8->opcode & 0x0F00) >> 8] != (chip8->opcode & 0x00FF)) {
        chip8->pc += 4;
      }
      else {
        chip8->pc += 2;
      }
    break;

    /* 0x5XY0: Skips instruction if VX equals VY */
    case 0x5000:
      if (chip8->registers[(chip8->opcode & 0x0F00) >> 8] == 
          chip8->registers[(chip8->opcode & 0x00F0) >> 4]) {
        chip8->pc += 4;
      }
      else {
        chip8->pc += 2;
      }
    break;

    /* 0x6XNN: Sets VX to NN */
    case 0x6000:
      chip8->registers[(chip8->opcode & 0x0F00) >> 8] = (chip8->opcode & 0x00FF);
      chip8->pc += 2;
    break;

    /* 0x7XNN: Adds NN to VX */
    case 0x7000:
      chip8->registers[(chip8->opcode & 0x0F00) >> 8] += (chip8->opcode & 0x00FF);
      chip8->pc += 2;
    break;

    case 0x8000:
      switch (chip8->opcode & 0x000F) {
        /* 0x8XY0: Sets VX to VY */
        case 0x0000:
          chip8->registers[(chip8->opcode & 0x0F00) >> 8] = chip8->registers[(chip8->opcode & 0x00F0) >> 4];
          chip8->pc += 2;
        break;

        /* 0x8XY1: Sets VX to VX || VY */
        case 0x0001:
          chip8->registers[(chip8->opcode & 0x0F00) >> 8] |= chip8->registers[(chip8->opcode & 0x00F0) >> 4];
          chip8->pc += 2;
        break;

        /* 0x8XY2: Sets VX to VX && VY */
        case 0x0002:
          chip8->registers[(chip8->opcode & 0x0F00) >> 8] &= chip8->registers[(chip8->opcode & 0x00F0) >> 4];
          chip8->pc += 2;
        break;

        /* 0x8XY3: Sets VX to VX XOR VY */
        case 0x0003:
          chip8->registers[(chip8->opcode & 0x0F00) >> 8] ^= chip8->registers[(chip8->opcode & 0x00F0) >> 4];
          chip8->pc += 2;
        break;

        /* 0x8XY4: Adds VY to VX. Sets VF to 1 if there's carry */
        case 0x0004:
          if (chip8->registers[(chip8->opcode & 0x00F0) >> 4] > 
             (0xFF) - chip8->registers[(chip8->opcode & 0x0F00) >> 8]) {
            chip8->registers[0xF] = 1; // carry flag
          }
          else {
            chip8->registers[0xF] = 0;
          }
          chip8->registers[(chip8->opcode & 0x0F00) >> 8] += chip8->registers[(chip8->opcode & 0x00F0) >> 4];
          chip8->pc += 2;
        break;

        /* 0x8XY5: Subtracts VY from VX. Sets VF to 0 if there's borrow, 1 otherwise */
        case 0x0005:
          if (chip8->registers[(chip8->opcode & 0x0F00) >> 8] >
              chip8->registers[(chip8->opcode & 0x00F0) >> 4]) {
            chip8->registers[0xF] = 1; 
          }
          else {
            chip8->registers[0xF] = 0;
          }
          chip8->registers[(chip8->opcode & 0x0F00) >> 8] -= chip8->registers[(chip8->opcode & 0x00F0) >> 4];
          chip8->pc += 2;
        break;

        /* 0x8XY6: Shifts VX right by one. Sets VF to the LSB of VX before-shift */
        case 0x0006:
          chip8->registers[0xF] = chip8->registers[(chip8->opcode & 0x0F00) >> 8] & 0x1;
          chip8->registers[(chip8->opcode & 0x0F00)] >>= 1;
          chip8->pc += 2;
        break;

        /* 0x8XY7: Sets VX to VY - VX. Sets VF to 0 if there's borrow, 1 otherwise */
        case 0x0007:
          if (chip8->registers[(chip8->opcode & 0x00F0) >> 4] >
              chip8->registers[(chip8->opcode & 0x0F00) >> 8]) {
            chip8->registers[0xF] = 1;
          }
          else {
            chip8->registers[0xF] = 0;
          }
          chip8->registers[(chip8->opcode & 0x0F00) >> 8] = chip8->registers[(chip8->opcode & 0x00F0) >> 4] -
                                                            chip8->registers[(chip8->opcode & 0x0F00) >> 8];
          chip8->pc += 2;
        break;

        /* 0x8XYE: Shifts VX left by one. Sets VF to the MSB of VX before-shift */
        case 0x000E:
          chip8->registers[0xF] = chip8->registers[(chip8->opcode & 0x0F00) >> 8] >> 7;
          chip8->registers[(chip8->opcode & 0x0F00) >> 8] <<= 1;
          chip8->pc += 2;
        break;

        default:
          printf("Unknown opcode [0x8000]: 0x%X\n", chip8->opcode);
        break;
      }
    break;

    /* 0x9XY0: Skips instruction if VX != VY */
    case 0x9000:
      if (chip8->registers[(chip8->opcode & 0x0F00) >> 8] != 
          chip8->registers[(chip8->opcode & 0x00F0) >> 4]) {
        chip8->pc += 4;
      }
      else {
        chip8->pc += 2;
      }
    break;

    /* 0xANNN: Sets I to the address NNN */
    case 0xA000:
      chip8->i_register = chip8->opcode & 0x0FFF;
      chip8->pc += 2;
    break;

    /* 0xBNNN: Jumps to the address NNN plus V0 */
    case 0xB000:
      chip8->pc = (chip8->opcode & 0x0FFF) + chip8->registers[0];
    break;

    /* 0xCXNN: Sets VX to NN && a random number */
    case 0xC000:
      chip8->registers[(chip8->opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (chip8->opcode & 0x00FF);
      chip8->pc += 2;
    break;

    /* 0xDXYN: Draws a 8xN sprite at coordinate (VX, VY). Sets VF to 1 if any screen pixels are flipped.
     * Each row of 8 pixels is read starting at memory location I.
     */
    case 0xD000: {
      uint8_t x = chip8->registers[(chip8->opcode & 0x0F00) >> 8];
      uint8_t y = chip8->registers[(chip8->opcode & 0x00F0) >> 4];

      uint8_t height = chip8->opcode & 0x000F;
      uint8_t pixel;

      chip8->registers[0xF] = 0;

      for (int y_line = 0; y_line < height; y_line++) {
        pixel = chip8->memory[chip8->i_register + y_line];
        for (int x_line = 0; x_line < 8; x_line++) {
          if ((pixel) & (0x80 >> x_line) != 0) {
            if (chip8->graphics[(x + x_line + ((y + y_line) * 64))] == 1) {
              chip8->registers[0xF] = 1;
            }
            chip8->graphics[(x + x_line + ((y + y_line) * 64))] ^= 1;
          }
        }
      }
      chip8->pc += 2;
    }

    case 0xE000: 
      switch (chip8->opcode & 0x000F) {
        /* 0xEX9E: Skips instruction if key stored in VX is pressed */
        case 0x000E:
          if (chip8->key[chip8->registers[(chip8->opcode & 0x0F00) >> 8]] != 0) {
            chip8->pc += 4;
          }
          else {
            chip8->pc += 2;
          }
        break;
      }  
    break;
  }
}