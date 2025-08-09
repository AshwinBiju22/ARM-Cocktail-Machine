#include "instructions.h"
#include "bit_utils.h"
#include "emu_memory.h"
#include "emu_registers.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#define IS_WIDE_MOV(instr) (assert(extract_bits((instr), 23, 3) == 0x5))
#define IS_ARITHMETIC(instr) (assert(extract_bits((instr), 23, 3) == 0x2))
#define GET_OPC(instr) (extract_bits((instr), 29, 2))

static inline void parse_mov_fields(uint32_t instruction, bool *sf, uint8_t *rd, uint8_t *hw,
									uint64_t *imm16) {
	*sf = extract_bits(instruction, 31, 1);
	*rd = extract_bits(instruction, 0, 5);
	*hw = extract_bits(instruction, 21, 2);
	*imm16 = extract_bits(instruction, 5, 16);
}

// Rd := ~Op
void execute_movn(uint32_t instruction) {
	IS_WIDE_MOV(instruction);
	assert(GET_OPC(instruction) == 0x0);
	bool sf;
	uint8_t rd, hw;
	uint64_t imm16;
	parse_mov_fields(instruction, &sf, &rd, &hw, &imm16);
	uint8_t shift = hw * 16;
	uint64_t op = imm16 << shift;
	uint64_t result = ~op;
	if (!sf)
		result &= 0xFFFFFFFFULL;
	register_store(rd, result, sf);
}

// Rd := Op
void execute_movz(uint32_t instruction) {
	IS_WIDE_MOV(instruction);
	assert(GET_OPC(instruction) == 0x2);
	bool sf;
	uint8_t rd, hw;
	uint64_t imm16;
	parse_mov_fields(instruction, &sf, &rd, &hw, &imm16);
	uint8_t shift = hw * 16;
	uint64_t result = imm16 << shift;
	if (!sf)
		result &= 0xFFFFFFFFULL;
	register_store(rd, result, sf);
}

// Rd[shift + 15 : shift# := imm16 (only the bits where imm16 goes get changed)
void execute_movk(uint32_t instruction) {
	IS_WIDE_MOV(instruction);
	assert(GET_OPC(instruction) == 0x3);
	bool sf;
	uint8_t rd, hw;
	uint64_t imm16;
	parse_mov_fields(instruction, &sf, &rd, &hw, &imm16);
	uint8_t shift = hw * 16;
	uint64_t orig = register_load(rd, sf);
	if (!sf)
		orig &= 0xFFFFFFFFULL;
	uint64_t mask = ~(0xFFFFULL << shift);
	uint64_t result = (orig & mask) | (imm16 << shift);
	register_store(rd, result, sf);
}
