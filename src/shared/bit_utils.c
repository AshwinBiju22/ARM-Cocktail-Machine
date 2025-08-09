#include "bit_utils.h"
#include "emu_memory.h"
#include "emu_registers.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t set_bit(uint64_t value, unsigned pos) { return value | (1ULL << pos); }
uint64_t clear_bit(uint64_t value, unsigned pos) { return value & ~(1ULL << pos); }
uint64_t toggle_bit(uint64_t value, unsigned pos) { return value ^ (1ULL << pos); }
uint64_t get_bit(uint64_t value, unsigned pos) { return (value >> pos) & 1; }

// extracts {width} bits from bit {lsb} to bit {lsb + width - 1}
uint64_t extract_bits(uint64_t value, unsigned lsb, unsigned width) {
	if (width == 64)
		return value;
	return (value >> lsb) & ((1ULL << width) - 1);
}

uint64_t make_mask(unsigned lsb, unsigned width) {
	assert(lsb < 64);
	if (width >= 64) // need special case as shift by more than 63 is undefined
		return (uint64_t)-1 << lsb;
	return ((1ULL << width) - 1) << lsb;
}

// insert bits {field} into bits {lsb} to {lsb + width - 1} of {original}
uint64_t insert_bits(uint64_t original, uint64_t field, unsigned lsb, unsigned width) {
	uint64_t mask = make_mask(lsb, width);
	return (original & ~mask) | ((field << lsb) & mask);
}

bool is_aligned(uint64_t value, unsigned alignment) { return (value & (alignment - 1)) == 0; }

// sign-extend a {value} assuming it is {original_width} wide
uint64_t sign_extend(uint64_t value, unsigned original_width) {
	// if msb if 1 then it is negative, else it is positive so return unmodified
	if (get_bit(value, original_width - 1)) {
		uint64_t mask = make_mask(original_width, 64 - original_width);
		return value | mask;
	}
	return value;
}

// rotate {value} {shift} bits left, assuming {value} is {width} bits wide
uint64_t rotate_left(uint64_t value, unsigned shift, unsigned width) {
	// mask to remove overflowed bits
	uint64_t mask = make_mask(0, width);
	return ((value << shift) | (value >> (width - shift))) & mask;
}

// same as above but right
uint64_t rotate_right(uint64_t value, unsigned shift, unsigned width) {
	uint64_t mask = make_mask(0, width);
	return ((value >> shift) | (value << (width - shift))) & mask;
}

// unused
unsigned popcount(uint64_t value) { return __builtin_popcountll(value); }

unsigned clz(uint64_t value) { return __builtin_clzll(value); }

unsigned ctz(uint64_t value) { return __builtin_ctzll(value); }

unsigned find_msb(uint64_t value) { return 63 - __builtin_clzll(value); }

unsigned find_lsb(uint64_t value) { return __builtin_ctzll(value); }

// designed to check overflow...
bool fits_in_bits_signed(int64_t value, unsigned width) {
	return value <= (1 << (width - 1)) - 1 && value >= -(1 << (width - 1));
}

bool fits_in_bits_unsigned(uint64_t value, unsigned width) { return value <= (1 << width) - 1; }

// helper to determine width
unsigned get_width(uint64_t sf) { return sf ? 64 : 32; }

// helper for executing shifts
uint64_t apply_shift(uint64_t value, unsigned shift_type, unsigned shift_amount, bool sf) {
	switch (shift_type) {
	case 0:
		return logical_shift_left(value, shift_amount, get_width(sf));
	case 1:
		return logical_shift_right(value, shift_amount, get_width(sf));
	case 2:
		return arithmetic_shift_right(value, shift_amount, get_width(sf));
	case 3:
		return rotate_right(value, shift_amount, get_width(sf));
	default:
		exit(EXIT_FAILURE);
	}
}

// executes add instr
void add(uint64_t rd, uint64_t rn, uint64_t op, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t rn_val = register_load((unsigned)rn, sf);
	uint64_t result = rn_val + op;
	result = extract_bits(result, 0, width);
	register_store((unsigned)rd, result, sf);
}

// executes adds instr
void adds(uint64_t rd, uint64_t rn, uint64_t op, uint64_t sf) {
	unsigned w = get_width(sf);
	uint64_t rv = register_load((unsigned)rn, sf);
	uint64_t mask = (w == 64 ? ~0ULL : ((1ULL << w) - 1));
	uint64_t a = rv & mask;
	uint64_t b = op & mask;
	uint64_t sum = a + b;
	uint64_t r = sum & mask;
	register_store((unsigned)rd, r, sf);
	setN(get_bit(r, w - 1));
	setZ(r == 0);
	setC((sum >> w) & 1);
	setV((get_bit(a, w - 1) == get_bit(b, w - 1)) && (get_bit(r, w - 1) != get_bit(a, w - 1)));
}

// executes sub instr
void sub(uint64_t rd, uint64_t rn, uint64_t op, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t rn_val = register_load((unsigned)rn, sf);
	uint64_t result = rn_val - op;
	result = extract_bits(result, 0, width);
	register_store((unsigned)rd, result, sf);
}

// executes subs instr
void subs(uint64_t rd, uint64_t rn, uint64_t op, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t rn_val = register_load((unsigned)rn, sf);
	uint64_t result = rn_val - op;
	result = extract_bits(result, 0, width);
	register_store((unsigned)rd, result, sf);

	setN(get_bit(result, width - 1));
	setZ(result == 0);
	setC(rn_val >= op);

	bool rn_sign = get_bit(rn, width - 1);
	bool op_sign = get_bit(op, width - 1);
	bool res_sign = get_bit(result, width - 1);
	bool overflow = (rn_sign != op_sign) && (res_sign != rn_sign);
	setV(overflow);
}

// executes madd instr
void madd(uint64_t rd, uint64_t rn, uint64_t rm, uint64_t ra, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t rn_val = register_load((unsigned)rn, sf);
	uint64_t rm_val = register_load((unsigned)rm, sf);
	uint64_t ra_val = register_load((unsigned)ra, sf);
	uint64_t result = ra_val + (rn_val * rm_val);
	result = extract_bits(result, 0, width);
	register_store((unsigned)rd, result, sf);
}

// executes msub instr
void msub(uint64_t rd, uint64_t rn, uint64_t rm, uint64_t ra, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t rn_val = register_load((unsigned)rn, sf);
	uint64_t rm_val = register_load((unsigned)rm, sf);
	uint64_t ra_val = register_load((unsigned)ra, sf);
	uint64_t result = ra_val - (rn_val * rm_val);
	result = extract_bits(result, 0, width);
	register_store((unsigned)rd, result, sf);
}

uint64_t ldr_uoffset(uint64_t rt, uint64_t xn, uint64_t imm12, uint64_t sf) {
	uint64_t base = register_load(xn, true);
	uint32_t uoffset = imm12 * (sf ? 8 : 4);
	uint32_t address = (uint32_t)base + uoffset;

	if (sf) {
		uint64_t value = mem_load64(address);
		register_store(rt, value, sf);
		return value;
	} else {
		uint32_t value = mem_load32(address);
		register_store(rt, (uint64_t)value, sf);
		return (uint64_t)value;
	}
}

void str_uoffset(uint64_t rt, uint64_t xn, uint64_t imm12, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t base = register_load((unsigned)xn, true);
	uint32_t uoffset = imm12 * (width / 8);
	uint32_t address = (uint32_t)base + uoffset;

	if (width == 64) {
		uint64_t value = register_load(rt, true);
		mem_store64(address, value);
	} else {
		uint32_t value = (uint32_t)register_load(rt, false);
		mem_store32(address, value);
	}
}

uint64_t ldr_preindexed(uint64_t rt, uint64_t xn, uint64_t simm9, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t baddr = register_load((unsigned)xn, true);
	uint64_t offset = sign_extend(simm9, 9);
	uint32_t address = (uint32_t)(baddr + offset);
	register_store(xn, address, true);

	if (width == 64) {
		uint64_t value = mem_load64(address);
		register_store(rt, value, true);
		return value;
	} else {
		uint32_t value = mem_load32(address);
		register_store(rt, (uint64_t)value, false);
		return (uint64_t)value;
	}
}

void str_preindexed(uint64_t rt, uint64_t xn, uint64_t simm9, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t baddr = register_load(xn, true);
	uint64_t offset = sign_extend(simm9, 9);
	uint32_t address = (uint32_t)(baddr + offset);
	register_store(xn, address, true);

	if (width == 64) {
		uint64_t value = register_load(rt, true);
		mem_store64(address, value);
	} else {
		uint32_t value = (uint32_t)register_load(rt, false);
		mem_store32(address, value);
	}
}

uint64_t ldr_postindexed(uint64_t rt, uint64_t xn, uint64_t simm9, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t baddr = register_load(xn, true);
	uint64_t offset = sign_extend(simm9, 9);
	uint32_t address = (uint32_t)baddr;
	register_store(xn, baddr + offset, true);

	if (width == 64) {
		uint64_t value = mem_load64(address);
		register_store(rt, value, true);
		return value;
	} else {
		uint32_t value = mem_load32(address);
		register_store(rt, (uint64_t)value, false);
		return (uint64_t)value;
	}
}

void str_postindexed(uint64_t rt, uint64_t xn, uint64_t simm9, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t baddr = register_load(xn, true);
	uint64_t offset = sign_extend(simm9, 9);
	uint32_t address = (uint32_t)baddr;
	register_store(xn, baddr + offset, true);

	if (width == 64) {
		uint64_t value = register_load(rt, true);
		mem_store64(address, value);
	} else {
		uint32_t value = (uint32_t)register_load(rt, false);
		mem_store32(address, value);
	}
}

uint64_t ldr_regoffset(uint64_t rt, uint64_t xn, uint64_t xm, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t baddr = register_load(xn, true);
	uint64_t offset = register_load(xm, true);
	uint32_t address = (uint32_t)(baddr + offset);

	if (width == 64) {
		uint64_t value = mem_load64(address);
		register_store(rt, value, true);
		return value;
	} else {
		uint32_t value = mem_load32(address);
		register_store(rt, (uint64_t)value, false);
		return (uint64_t)value;
	}
}

void str_regoffset(uint64_t rt, uint64_t xn, uint64_t xm, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t baddr = register_load(xn, true);
	uint64_t offset = register_load(xm, true);
	uint32_t address = (uint32_t)(baddr + offset);
	if (width == 64) {
		uint64_t value = register_load(rt, true);
		mem_store64(address, value);
	} else {
		uint32_t value = (uint32_t)register_load(rt, false);
		mem_store32(address, value);
	}
}

uint64_t ldr_literal(uint64_t rt, uint64_t simm19, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t offset = sign_extend(simm19 * 4, 21);
	uint32_t address = (uint32_t)(pc_load() + offset);

	if (width == 64) {
		uint64_t value = mem_load64(address);
		register_store(rt, value, true);
		return value;
	} else {
		uint32_t value = mem_load32(address);
		register_store(rt, (uint64_t)value, false);
		return (uint64_t)value;
	}
}

uint64_t logical_shift_left(uint64_t value, unsigned shift, unsigned width) {
	return (value << shift) & make_mask(0, width);
}

uint64_t logical_shift_right(uint64_t value, unsigned shift, unsigned width) {
	return value >> shift;
}

uint64_t arithmetic_shift_right(uint64_t value, unsigned shift, unsigned width) {
	uint64_t mask = make_mask(0, width);
	return sign_extend((value >> shift), width - shift) & mask;
}

void execute_and(uint64_t rd, uint64_t rn, uint64_t op2, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t rv = register_load(rn, sf);
	uint64_t result = rv & op2;
	result = extract_bits(result, 0, width);
	register_store(rd, result, sf);
}

void execute_orr(uint64_t rd, uint64_t rn, uint64_t op2, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t rv = register_load(rn, sf);
	uint64_t result = rv | op2;
	result = extract_bits(result, 0, width);
	register_store(rd, result, sf);
}

void execute_eor(uint64_t rd, uint64_t rn, uint64_t op2, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t rv = register_load(rn, sf);
	uint64_t result = rv ^ op2;
	result = extract_bits(result, 0, width);
	register_store(rd, result, sf);
}

void execute_ands(uint64_t rd, uint64_t rn, uint64_t op2, uint64_t sf) {
	unsigned width = get_width(sf);
	uint64_t rv = register_load(rn, sf);
	uint64_t result = rv & op2;
	result = extract_bits(result, 0, width);
	register_store(rd, result, sf);

	setN(get_bit(result, width - 1));
	setZ(result == 0);
	setC(0);
	setV(0);
}
