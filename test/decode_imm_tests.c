#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "bit_utils.h"
#include "emu_registers.h"
#include "emu_memory.h"
#include "fde.h"
#include "instructions.h"

#define BUILD_ARITH_IMM(sf,opc,imm12,rn,rd) \
	(((sf)   << 31) | ((opc) << 29) | (4u << 26) | (2u << 23) | ((imm12) << 10) | ((rn) << 5) | (rd))
#define BUILD_WIDE_MOV(sf,opc,hw,imm16,rd) \
	(((sf)   << 31) | ((opc) << 29) | (4u << 26) | (5u << 23) | ((hw) << 21) | ((imm16) << 5) | (rd))

int main(void) {
	registers_init();
	mem_init();

	uint32_t instr;
	uint64_t v;

	// Arithmetic immediate
	register_store(1, 1, true);
	instr = BUILD_ARITH_IMM(1, 0x0, 5, 1, 0);
	decode_and_execute(instr);
	v = register_load(0, true);
	assert(v == 1 + 5);
	printf("add_imm_64: OK\n");

	register_store(2, 10, false);
	instr = BUILD_ARITH_IMM(0, 0x0, 7, 2, 3);
	decode_and_execute(instr);
	v = register_load(3, false);
	assert(v == 10 + 7);
	printf("add_imm_32: OK\n");

	register_store(4, 20, true);
	instr = BUILD_ARITH_IMM(1, 0x1, 3, 4, 5);
	decode_and_execute(instr);
	v = register_load(5, true);
	assert(v == 20 + 3);
	printf("adds_imm_64: OK\n");

	register_store(6, 30, false);
	instr = BUILD_ARITH_IMM(0, 0x1, 8, 6, 7);
	decode_and_execute(instr);
	v = register_load(7, false);
	assert(v == 30 + 8);
	printf("adds_imm_32: OK\n");

	register_store(8, 40, true);
	instr = BUILD_ARITH_IMM(1, 0x2, 15, 8, 9);
	decode_and_execute(instr);
	v = register_load(9, true);
	assert(v == 40 - 15);
	printf("sub_imm_64: OK\n");

	register_store(10, 50, false);
	instr = BUILD_ARITH_IMM(0, 0x2, 20, 10, 11);
	decode_and_execute(instr);
	v = register_load(11, false);
	assert(v == 50 - 20);
	printf("sub_imm_32: OK\n");

	register_store(12, 60, true);
	instr = BUILD_ARITH_IMM(1, 0x3, 25, 12, 13);
	decode_and_execute(instr);
	v = register_load(13, true);
	assert(v == 60 - 25);
	printf("subs_imm_64: OK\n");

	register_store(14, 70, false);
	instr = BUILD_ARITH_IMM(0, 0x3, 30, 14, 15);
	decode_and_execute(instr);
	v = register_load(15, false);
	assert(v == 70 - 30);
	printf("subs_imm_32: OK\n");

	// Wide move immediate
	instr = BUILD_WIDE_MOV(1, 0x0, 0, 0x1234, 16);  // movn hw=0 (<<0)
	decode_and_execute(instr);
	v = register_load(16, true);
	assert(v == ~((uint64_t)0x1234 << 0));
	printf("movn_64_hw0: OK\n");

	instr = BUILD_WIDE_MOV(0, 0x0, 1, 0x1234, 17);  // movn hw=1 (<<16)
	decode_and_execute(instr);
	v = register_load(17, false);
	assert(v == ~((uint32_t)0x1234 << 16));
	printf("movn_32_hw1: OK\n");

	instr = BUILD_WIDE_MOV(1, 0x2, 2, 0x5678, 18);  // movz hw=2 (<<32)
	decode_and_execute(instr);
	v = register_load(18, true);
	assert(v == ((uint64_t)0x5678 << 32));
	printf("movz_64_hw2: OK\n");

	instr = BUILD_WIDE_MOV(0, 0x2, 0, 0x5678, 19);  // movz hw=0 (<<0)
	decode_and_execute(instr);
	v = register_load(19, false);
	assert(v == ((uint32_t)0x5678 << 0));
	printf("movz_32_hw0: OK\n");

	register_store(20, 0xAABBCCDDEEFF0011ULL, true);
	instr = BUILD_WIDE_MOV(1, 0x3, 2, 0xABCD, 20);  // movk hw=2 (<<32)
	decode_and_execute(instr);
	v = register_load(20, true);
	assert(v == ((0xAABBCCDDEEFF0011ULL & ~(0xFFFFULL<<32)) | ((uint64_t)0xABCD << 32)));
	printf("movk_64_hw2: OK\n");

	register_store(21, 0xDEADBEEFul, false);
	instr = BUILD_WIDE_MOV(0, 0x3, 1, 0x5678, 21);  // movk hw=1 (<<16)
	decode_and_execute(instr);
	v = register_load(21, false);
	assert(v == ((0xDEADBEEFul & ~(0xFFFFUL<<16)) | ((uint32_t)0x5678 << 16)));
	printf("movk_32_hw1: OK\n");

	registers_destroy();
	mem_destroy();
	return EXIT_SUCCESS;
}

