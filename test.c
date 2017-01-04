#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "chip8.h"
#include "test.h"

void test_clear_screen(chip8_t *chip8) {
  int i = 0;

  init(chip8);

  chip8->memory[0x200] = 0x00;
  chip8->memory[0x201] = 0xE0;

  emulate_cycle(chip8);

  while (i < sizeof(chip8->graphics) / sizeof(chip8->graphics[0])) {
    assert(chip8->graphics[i++] == 0);
  }

  assert(chip8->pc == 0x202);
}

void test_return(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x00;
  chip8->memory[0x201] = 0xEE;

  chip8->stack[0] = 0x200;
  chip8->sp = 1;

  emulate_cycle(chip8);

  assert(chip8->pc == 0x202);
  assert(chip8->sp == 0);
}

void test_jump(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x15;
  chip8->memory[0x201] = 0x45;

  emulate_cycle(chip8);

  assert(chip8->pc == 0x545);
}

void test_call(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x25;
  chip8->memory[0x201] = 0x45;

  emulate_cycle(chip8);

  assert(chip8->stack[0] == 0x200);
  assert(chip8->sp == 1);
  assert(chip8->pc == 0x545);
}

void test_skip_if_equal_byte(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x32;
  chip8->memory[0x201] = 0xAA;

  chip8->registers[2]  = 0xAA;

  emulate_cycle(chip8);

  assert(chip8->pc == 0x204);
}

void test_skip_if_not_equal_byte(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x42;
  chip8->memory[0x201] = 0xAA;

  chip8->registers[2]  = 0xBB;

  emulate_cycle(chip8);

  assert(chip8->pc == 0x204);
}

void test_skip_if_equal_register(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x50;
  chip8->memory[0x201] = 0x10;

  chip8->registers[0] = 0xAA;
  chip8->registers[1] = 0xAA;

  emulate_cycle(chip8);

  assert(chip8->pc == 0x204);
}

int main() {
  chip8_t *chip8;
  chip8 = malloc(sizeof(chip8_t));

  test_clear_screen(chip8);
  test_return(chip8);
  test_jump(chip8);
  test_skip_if_equal_byte(chip8);
  test_skip_if_not_equal_byte(chip8);
  test_skip_if_equal_register(chip8);
}
