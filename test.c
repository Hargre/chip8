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

void test_set_register_byte(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x60;
  chip8->memory[0x201] = 0x10;

  emulate_cycle(chip8);

  assert(chip8->registers[0] == 0x10);
  assert(chip8->pc == 0x202);
}

void test_add_register_byte(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x70;
  chip8->memory[0x201] = 0x01;

  chip8->registers[0] = 0x01;

  emulate_cycle(chip8);

  assert(chip8->registers[0] == 0x02);
  assert(chip8->pc == 0x202);
}

void test_set_register_register(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x80;
  chip8->memory[0x201] = 0x10;

  chip8->registers[1] = 0x10;

  emulate_cycle(chip8);

  assert(chip8->registers[0] == 0x10);
  assert(chip8->pc == 0x202);
}

void test_vx_or_vy(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x80;
  chip8->memory[0x201] = 0x11;

  chip8->registers[1] = 0xFF;

  emulate_cycle(chip8);

  assert(chip8->registers[0] == 0xFF);
  assert(chip8->pc == 0x202);
}

void test_vx_and_vy(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x80;
  chip8->memory[0x201] = 0x12;

  chip8->registers[0] = 0xFF;
  chip8->registers[1] = 0x00;

  emulate_cycle(chip8);

  assert(chip8->registers[0] == 0x00);
  assert(chip8->pc == 0x202);
}

void test_vx_xor_vy(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x80;
  chip8->memory[0x201] = 0x13;

  chip8->registers[0] = 0xFF;
  chip8->registers[1] = 0xFF;

  emulate_cycle(chip8);

  assert(chip8->registers[0] == 0x00); // x XOR x == 0
  assert(chip8->pc == 0x202);
}

void test_vx_plus_vy_no_carry(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x80;
  chip8->memory[0x201] = 0x14;

  chip8->registers[0] = 0x00;
  chip8->registers[1] = 0x01;

  emulate_cycle(chip8);

  assert(chip8->registers[0] == 0x01);
  assert(chip8->registers[0xF] == 0);
  assert(chip8->pc == 0x202);
}

void test_vx_plus_vy_carry(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x80;
  chip8->memory[0x201] = 0x14;

  chip8->registers[0] = 0xEE;
  chip8->registers[1] = 0xFF;

  emulate_cycle(chip8);

  assert(chip8->registers[0xF] == 1);
  assert(chip8->pc == 0x202);
}

void test_vx_minus_vy_no_borrow(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x80;
  chip8->memory[0x201] = 0x15;

  chip8->registers[0] = 0xFF;
  chip8->registers[1] = 0x00;

  emulate_cycle(chip8);

  assert(chip8->registers[0] == 0xFF);
  assert(chip8->registers[0xF] == 1);
  assert(chip8->pc == 0x202);
}

void test_vx_minus_vy_borrow(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x80;
  chip8->memory[0x201] = 0x15;

  chip8->registers[0] = 0x00;
  chip8->registers[1] = 0xFF;

  emulate_cycle(chip8);

  assert(chip8->registers[0xF] == 0);
  assert(chip8->pc == 0x202);
}

void test_vx_right_shift(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x80;
  chip8->memory[0x201] = 0x16;

  chip8->registers[0] = 0x02;

  emulate_cycle(chip8);

  assert(chip8->registers[0xF] == 0);
  assert(chip8->registers[0] == 1);
  assert(chip8->pc == 0x202);
}

void test_vy_minus_vx_no_borrow(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x80;
  chip8->memory[0x201] = 0x17;

  chip8->registers[0] = 0x00;
  chip8->registers[1] = 0xFF;

  emulate_cycle(chip8);

  assert(chip8->registers[0xF] == 1);
  assert(chip8->registers[0] == 0xFF);
  assert(chip8->pc == 0x202);
}

void test_vy_minus_vx_borrow(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x80;
  chip8->memory[0x201] = 0x17;

  chip8->registers[0] = 0xFF;
  chip8->registers[1] = 0x00;

  emulate_cycle(chip8);

  assert(chip8->registers[0xF] == 0);
  assert(chip8->pc == 0x202);
}

void test_vx_left_shift(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x80;
  chip8->memory[0x201] = 0x1E;

  chip8->registers[0] = 0xFF;

  emulate_cycle(chip8);

  assert(chip8->registers[0xF] == 1);
  assert(chip8->registers[0] == 0xFE);
  assert(chip8->pc == 0x202);
}

void test_skip_vx_not_equal_vy(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0x90;
  chip8->memory[0x201] = 0x10;

  chip8->registers[0] = 0xAA;
  chip8->registers[1] = 0xBB;

  emulate_cycle(chip8);

  assert(chip8->pc == 0x204);
}

void test_set_i(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0xAA;
  chip8->memory[0x201] = 0xAA;

  emulate_cycle(chip8);
  /* I register should have address 0xAAA */

  assert(chip8->i_register == 0xAAA);
  assert(chip8->pc == 0x202);
}

void test_jump_plus_v0(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0xB0;
  chip8->memory[0x201] = 0x00;

  chip8->registers[0] = 0x01;

  emulate_cycle(chip8);

  assert(chip8->pc == 0x0001);
}

void test_draw(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0xD0;
  chip8->memory[0x201] = 0x01;

  chip8->memory[chip8->i_register] = 0xFF; 

  emulate_cycle(chip8);

  for (int i = 0; i < 8; i++) {
    assert(chip8->graphics[i] == 1);
  }
  assert(chip8->pc == 0x202);
}

void test_skip_if_key_pressed(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0xE0;
  chip8->memory[0x201] = 0x9E;

  chip8->key[0] = 1;

  emulate_cycle(chip8);

  assert(chip8->pc == 0x204);
}

void test_skip_if_key_not_pressed(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0xE0;
  chip8->memory[0x201] = 0xA1;

  chip8->key[0] = 0;

  emulate_cycle(chip8);

  assert(chip8->pc == 0x204);
}

void test_set_vx_to_delay_timer(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0xF0;
  chip8->memory[0x201] = 0x07;

  chip8->delay_timer = 0x0A;

  emulate_cycle(chip8);

  assert(chip8->registers[0] == 0x0A);
  assert(chip8->pc == 0x202);
}

void test_wait_for_key(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0xF0;
  chip8->memory[0x201] = 0x0A;

  chip8->key[1] = 1;

  emulate_cycle(chip8);

  assert(chip8->registers[0] == 1);
  assert(chip8->pc == 0x202);

  chip8->key[1] = 0;
  chip8->pc = 0x200;

  emulate_cycle(chip8);

  /* no key pressed; should maintain state */
  assert(chip8->pc == 0x200);
}

void test_set_delay_timer_to_vx(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0xF0;
  chip8->memory[0x201] = 0x15;

  chip8->registers[0] = 0xA;

  emulate_cycle(chip8);

  /* value minus one, since one cycle passed */
  assert(chip8->delay_timer == (0xA - 1));
  assert(chip8->pc == 0x202);
}

void test_set_sound_timer_to_vx(chip8_t *chip8) {
  init(chip8);

  chip8->memory[0x200] = 0xF0;
  chip8->memory[0x201] = 0x18;

  chip8->registers[0] = 0xA;

  emulate_cycle(chip8);

  /* value minus one, since one cycle passed */
  assert(chip8->sound_timer == (0xA - 1));
  assert(chip8->pc == 0x202);
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
  test_set_register_byte(chip8);
  test_add_register_byte(chip8);
  test_set_register_register(chip8);
  test_vx_or_vy(chip8);
  test_vx_and_vy(chip8);
  test_vx_xor_vy(chip8);
  test_vx_plus_vy_no_carry(chip8);
  test_vx_plus_vy_carry(chip8);
  test_vx_minus_vy_no_borrow(chip8);
  test_vx_minus_vy_borrow(chip8);
  test_vx_right_shift(chip8);
  test_vy_minus_vx_no_borrow(chip8);
  test_vy_minus_vx_borrow(chip8);
  test_vx_left_shift(chip8);
  test_skip_vx_not_equal_vy(chip8);
  test_set_i(chip8);
  test_jump_plus_v0(chip8);
  test_draw(chip8);
  test_skip_if_key_pressed(chip8);
  test_skip_if_key_not_pressed(chip8);
  test_set_vx_to_delay_timer(chip8);
  test_wait_for_key(chip8);
  test_set_delay_timer_to_vx(chip8);
  test_set_sound_timer_to_vx(chip8);
}
