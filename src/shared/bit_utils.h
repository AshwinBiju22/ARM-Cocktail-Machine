#ifndef BIT_UTILS_H
#define BIT_UTILS_H

#include <stdbool.h>
#include <stdint.h>

uint64_t set_bit(uint64_t value, unsigned pos);
uint64_t clear_bit(uint64_t value, unsigned pos);
uint64_t toggle_bit(uint64_t value, unsigned pos);
uint64_t get_bit(uint64_t value, unsigned pos);

uint64_t extract_bits(uint64_t value, unsigned lsb, unsigned width);
uint64_t insert_bits(uint64_t original, uint64_t field, unsigned lsb, unsigned width);

uint64_t make_mask(unsigned lsb, unsigned width);
bool is_aligned(uint64_t value, unsigned alignment);

uint64_t sign_extend(uint64_t value, unsigned original_width);

uint64_t rotate_left(uint64_t value, unsigned shift, unsigned width);
uint64_t rotate_right(uint64_t value, unsigned shift, unsigned width);

unsigned popcount(uint64_t value);
unsigned clz(uint64_t value);
unsigned ctz(uint64_t value);
unsigned find_msb(uint64_t value);
unsigned find_lsb(uint64_t value);

bool fits_in_bits_signed(int64_t value, unsigned width);
bool fits_in_bits_unsigned(uint64_t value, unsigned width);

unsigned get_width(uint64_t sf);

uint64_t apply_shift(uint64_t value, unsigned shift_type, unsigned shift_amount, bool sf);

void add(uint64_t rd, uint64_t rn, uint64_t op, uint64_t sf);
void adds(uint64_t rd, uint64_t rn, uint64_t op, uint64_t sf);

void sub(uint64_t rd, uint64_t rn, uint64_t op, uint64_t sf);
void subs(uint64_t rd, uint64_t rn, uint64_t op, uint64_t sf);

void madd(uint64_t rd, uint64_t rn, uint64_t rm, uint64_t ra, uint64_t sf);
void msub(uint64_t rd, uint64_t rn, uint64_t rm, uint64_t ra, uint64_t sf);

uint64_t ldr_uoffset(uint64_t rt, uint64_t xn, uint64_t imm12, uint64_t sf);
void str_uoffset(uint64_t rt, uint64_t xn, uint64_t imm12, uint64_t sf);
uint64_t ldr_preindexed(uint64_t rt, uint64_t xn, uint64_t simm9, uint64_t sf);
void str_preindexed(uint64_t rt, uint64_t xn, uint64_t simm9, uint64_t sf);
uint64_t ldr_postindexed(uint64_t rt, uint64_t xn, uint64_t simm9, uint64_t sf);
void str_postindexed(uint64_t rt, uint64_t xn, uint64_t simm9, uint64_t sf);
uint64_t ldr_regoffset(uint64_t rt, uint64_t xn, uint64_t xm, uint64_t sf);
void str_regoffset(uint64_t rt, uint64_t xn, uint64_t xm, uint64_t sf);
uint64_t ldr_literal(uint64_t rt, uint64_t simm19, uint64_t sf);
uint64_t logical_shift_left(uint64_t value, unsigned shift, unsigned width);
uint64_t logical_shift_right(uint64_t value, unsigned shift, unsigned width);
uint64_t arithmetic_shift_right(uint64_t value, unsigned shift, unsigned width);

void execute_and(uint64_t rd, uint64_t rn, uint64_t op2, uint64_t sf);
void execute_orr(uint64_t rd, uint64_t rn, uint64_t op2, uint64_t sf);
void execute_eor(uint64_t rd, uint64_t rn, uint64_t op2, uint64_t sf);
void execute_ands(uint64_t rd, uint64_t rn, uint64_t op2, uint64_t sf);

#endif
