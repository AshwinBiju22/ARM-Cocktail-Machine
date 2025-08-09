#include <stdlib.h>
#include "emu_memory.h"
#include <stdio.h>
#include "bit_utils.h"
#include <assert.h>

int main(int argc, char **argv) {
	uint64_t val = 0;
	val = 0;
	val = set_bit(val, 3);
	assert(val == 0x8);
	val = set_bit(val, 1);
	assert(val == 0xA);
	val = clear_bit(val, 1);
	assert(val == 0x8);
	val = toggle_bit(val, 2);
	assert(val == 0xC);
	val = toggle_bit(val, 3);
	assert(val == 0x4);
	assert(get_bit(val, 3) == 0);
	assert(get_bit(val, 2) == 1);
	assert(get_bit(val, 1) == 0);

	// --- Bit extraction ---
	val = 0x3FC;
	assert(extract_bits(val, 0, 12) == 0x3FC);
	assert(extract_bits(val, 2, 12) == 0xFF);
	assert(extract_bits(val, 12, 5) == 0);
	assert(extract_bits(val, 1, 12) == 0x1FE);

	// --- Bit insertion ---
	val = insert_bits(0xFFFFFFFFFFFFFFFF, 0x0, 4, 4);
		// val = 0xFFFFFFFFFFFFFF0F
	assert(extract_bits(val, 4, 4) == 0x0);
	assert(extract_bits(val, 0, 4) == 0xF);
	val = insert_bits(0, 0xA, 0, 4);
		// val = 0x0A
		// = 0x00001010
	assert(val == 0xA);
	val = insert_bits(val, 0x3, 4, 2);
		// val = 0x00111010
	assert(val == 0x3A);

	// --- Mask ---
	assert(make_mask(0, 5) == 0x1F);
	assert(make_mask(4, 3) == 0x70);
	assert(make_mask(60, 4) == 0xF000000000000000);

	// --- Alignment ---
	assert(is_aligned(0x1000, 0x1000) == true);
	assert(is_aligned(0x1001, 0x1000) == false);
	assert(is_aligned(0x800, 0x400) == true);

	// --- Rotate ---
		// 0b1001 = 0b0011
	assert(rotate_left(0x9, 1, 4) == 0x3);
	assert(rotate_right(0x9, 1, 4) == 0xC);

	// --- Sign extension ---
	assert(sign_extend(0x7F, 8) == 0x7F);
	assert(sign_extend(0xFF, 8) == -1);
	assert(sign_extend(0x800000, 24) == -0x800000);
	assert(sign_extend(0x400000, 24) == 0x400000);

	// --- Population count and zero count ---
	assert(popcount(0xF0F0F0F0F0F0F0F0) == 32);
	assert(clz(1ULL << 63) == 0);
	assert(clz(1ULL) == 63);
	assert(ctz(1ULL) == 0);
	assert(ctz(1ULL << 63) == 63);

	// --- Find MSB/LSB ---
	assert(find_msb(0x8000000000000000ULL) == 63);
	assert(find_msb(0x10) == 4);
	assert(find_lsb(0x8000000000000000ULL) == 63);
	assert(find_lsb(0x10) == 4);

	// --- Fit in width ---
	assert(fits_in_bits_unsigned(15, 4) == true);
	assert(fits_in_bits_unsigned(16, 4) == false);
	assert(fits_in_bits_signed(7, 4) == true);
	assert(fits_in_bits_signed(-8, 4) == true);
	assert(fits_in_bits_signed(-9, 4) == false);

	// --- LSL ---
	assert(logical_shift_left(0, 3, 10) == 0);
	assert(logical_shift_left(3, 1, 3) == 6);
	assert(logical_shift_left(3, 1, 2) == 2);
	assert(logical_shift_left(3, 4, 10) == 48);
	// --- LSR ---
	assert(logical_shift_right(1, 1, 10) == 0);
	assert(logical_shift_right(2, 1, 10) == 1);
	assert(logical_shift_right(3, 1, 10) == 1);
	assert(logical_shift_right(8, 2, 10) == 2);
	assert(logical_shift_right(2, 1, 2) == 1);
	// --- ASR ---
	assert(arithmetic_shift_right(1, 1, 10) == 0);
	assert(arithmetic_shift_right(2, 1, 2) == 3);
	assert(arithmetic_shift_right(2, 1, 3) == 1);
	return EXIT_SUCCESS;
}
