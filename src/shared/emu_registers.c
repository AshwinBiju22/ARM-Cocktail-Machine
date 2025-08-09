#include "emu_registers.h"
#include "bit_utils.h"
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define REGISTER_CHECK_BOUNDS(addr) assert((addr) <= NUM_REGISTERS)

// using array of uint64_t to represent the registers
static uint64_t *registers = NULL;
// using bitmask to represent pstate, technically only lower nibble needed
static uint8_t pstate = 0;
static uint64_t pc = 0;

void registers_init(void) {
	if (!registers) {
		registers = calloc(NUM_REGISTERS, sizeof(uint64_t));
		assert(registers);
	}
}

void registers_destroy(void) {
	if (registers) {
		free(registers);
		registers = NULL;
	}
}

// all helper functions to set and clear PSTATE
void setN(bool val) {
	if (val) {
		pstate = set_bit(pstate, 3);
	} else {
		pstate = clear_bit(pstate, 3);
	}
}

void setZ(bool val) {
	if (val) {
		pstate = set_bit(pstate, 2);
	} else {
		pstate = clear_bit(pstate, 2);
	}
}

void setC(bool val) {
	if (val) {
		pstate = set_bit(pstate, 1);
	} else {
		pstate = clear_bit(pstate, 1);
	}
}

void setV(bool val) {
	if (val) {
		pstate = set_bit(pstate, 0);
	} else {
		pstate = clear_bit(pstate, 0);
	}
}

bool getN(void) { return get_bit(pstate, 3); }

bool getZ(void) { return get_bit(pstate, 2); }

bool getC(void) { return get_bit(pstate, 1); }

bool getV(void) { return get_bit(pstate, 0); }

uint64_t register_load(unsigned addr, bool sf) {
	REGISTER_CHECK_BOUNDS(addr);
	if (addr == NUM_REGISTERS)
		return 0;
	if (!sf) { // masks out top 32 bits, when in 32 bit mode, returning only bottom 32 bits
		return registers[addr] & 0xFFFFFFFF;
	}
	return registers[addr];
}

void register_store(unsigned addr, uint64_t value, bool sf) {
	REGISTER_CHECK_BOUNDS(addr);
	if (addr != NUM_REGISTERS) {
		if (!sf) {
			registers[addr] = value & 0xFFFFFFFF;
		} else {
			registers[addr] = value;
		}
	}
}

// as pc is 4-byte aligned, each pc jump is left-shifted twice
void pc_jump(uint32_t offset) { pc += sign_extend((uint64_t)offset << 2, 34); }

void pc_jump_indirect(unsigned reg) { pc = register_load(reg, 0); }

void pc_jump_conditional(uint32_t offset, uint8_t cond) {
	switch (cond >> 1) { // use this as the LSB is whether to negate cond
	case 0:
		if ((cond & 1) ^ getZ())
			pc += sign_extend(offset << 2, 21);
		else
			pc_jump(1);
		break;
	case 5:
		if ((getN() == getV()) ^ (cond & 1))
			pc += sign_extend(offset << 2, 21);
		else
			pc_jump(1);
		break;
	case 6:
		if (((getN() == getV()) && !getZ()) ^ (cond & 1))
			pc += sign_extend(offset << 2, 21);
		else
			pc_jump(1);
		break;
	case 7:
		pc += sign_extend(offset << 2, 21);
		break;
	default:
		assert(false);
	}
}

// allows read-only access to pc
uint64_t pc_load() { return pc; }

// to export final state
void export_normal_registers(FILE *out) {
	int zero_padding = log10(NUM_REGISTERS) + 1;
	fprintf(out, "Registers:\n");
	for (size_t i = 0; i < NUM_REGISTERS; ++i) {
		fprintf(out, "X%0*zd = %016" PRIx64 "\n", zero_padding, i, register_load(i, true));
	}
}

// to export final state
void export_pc(FILE *out) { fprintf(out, "PC = %016" PRIx64 "\n", pc_load()); }

// to export final state
void export_pstate(FILE *out) {
	fprintf(out, "PSTATE : ");
	if (getN()) {
		fprintf(out, "N");
	} else {
		fprintf(out, "-");
	}
	if (getZ()) {
		fprintf(out, "Z");
	} else {
		fprintf(out, "-");
	}
	if (getC()) {
		fprintf(out, "C");
	} else {
		fprintf(out, "-");
	}
	if (getV()) {
		fprintf(out, "V\n");
	} else {
		fprintf(out, "-\n");
	}
}
