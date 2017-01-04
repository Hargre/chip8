#ifndef _TEST_H_
#define _TEST_H_

void test_clear_screen(chip8_t *chip8);
void test_return(chip8_t *chip8);
void test_jump(chip8_t *chip8);
void test_call(chip8_t *chip8);
void test_skip_if_equal_byte(chip8_t *chip8);
void test_skip_if_not_equal_byte(chip8_t *chip8);
void test_skip_if_equal_register(chip8_t *chip8);
void test_set_register_byte(chip8_t *chip8);
void test_add_register_byte(chip8_t *chip8);
void test_set_register_register(chip8_t *chip8);
void test_vx_or_vy(chip8_t *chip8);
void test_vx_and_vy(chip8_t *chip8);
void test_vx_xor_vy(chip8_t *chip8);
void test_vx_plus_vy_no_carry(chip8_t *chip8);
void test_vx_plus_vy_carry(chip8_t *chip8);
void test_vx_minus_vy_no_borrow(chip8_t *chip8);
void test_vx_minus_vy_borrow(chip8_t *chip8);

#endif