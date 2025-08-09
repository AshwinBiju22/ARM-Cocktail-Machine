#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#include "bit_utils.h"
#include "emu_memory.h"
#include "emu_registers.h"
#include "instructions.h"

/* immediate instruction format
sf: 31
opc: 29-30
100: 26-28
opi: 23-25
operand: 5-22
	// mov
	hw: 21-22
	imm16: 5-20
	// arith
rd: 0-4
*/

int main(void) {
	registers_init();
	mem_init();

	/* movz 64-bit: X1 := #0x1234 << 32 */
	uint32_t instr = (1u << 31)
		| (0x2u << 29)
		| (0x4u << 26)
		| (0x5u << 23)
		| (0x2u   << 21)
		| (0x1234u << 5)
		| 1u;
	execute_movz(instr);
	uint64_t val = register_load(1,true);
	assert(val == ((uint64_t)0x1234 << 32));
	printf("movz 64-bit: OK\n");

	/* movz 32-bit: X1 := #0x1234 << 16 */
	instr = (0u << 31)
		| (0x2u << 29)
		| (0x4u << 26)
		| (0x5u << 23)
		| (0x1u   << 21)
		| (0x1234u << 5)
		| 1u;
	execute_movz(instr);
	val = register_load(1,false);
	assert(val == ((uint32_t)0x1234 << 16));
	printf("movz 32-bit: OK\n");

	/* movn 64-bit: X1 := ~(#0x1234 << 32) */
	instr = (1u << 31)
		| (0x0u << 29)
		| (0x4u << 26)
		| (0x5u << 23)
		| (0x2u   << 21)
		| (0x1234u << 5)
		| 1u;
	execute_movn(instr);
	val = register_load(1,true);
	assert(val == ~((uint64_t)(0x1234) << 32));
	printf("movn 64-bit: OK\n");

	/* movk 64-bit: X1 := #0x1234 << 32) */
	instr = (1u << 31)
		| (0x0u << 29)
		| (0x4u << 26)
		| (0x5u << 23)
		| (0x2u   << 21)
		| (0x1234u << 5)
		| 1u;
	execute_movn(instr);
	val = register_load(1,true);
	assert(val == ~((uint64_t)(0x1234) << 32));
	printf("movn 64-bit: OK\n");

	/* movn 32-bit: X1 := ~(#0x1234 << 16) */
	instr = (0u << 31)
		| (0x0u << 29)
		| (0x4u << 26)
		| (0x5u << 23)
		| (0x1u   << 21)
		| (0x1234u << 5)
		| 1u;
	execute_movn(instr);
	val = register_load(1,false);
	assert(val == ~((uint32_t)(0x1234) << 16));
	printf("movn 32-bit: OK\n");
	
	/* movk 64-bit: insert imm16 at bit 32, preserving other bits */
	register_store(10, 0xAABBCCDDEEFF0011ULL, true);
	instr = (1u << 31)
		| (0x3u << 29)
		| (0x4u << 26)
		| (0x5u << 23)
		| (0x2u << 21)
		| (0x1234u << 5)
		| 10u;
	execute_movk(instr);
	val = register_load(10, true);
	assert(val == ((0xAABBCCDDEEFF0011ULL & ~(0xFFFFULL << 32)) | ((uint64_t)0x1234 << 32)));
	printf("movk 64-bit: OK\n");

	/* movk 32-bit: insert imm16 at bit 16, preserving other bits */
	register_store(11, 0xDEADBEEF, false);
	instr = (0u << 31)
		| (0x3u << 29)
		| (0x4u << 26)
		| (0x5u << 23)
		| (0x1u << 21)
		| (0x5678u << 5)
		| 11u;
	execute_movk(instr);
	val = register_load(11, false);
	assert(val == ((0xDEADBEEF & ~(0xFFFFULL << 16)) | ((uint32_t)0x5678 << 16)));
	printf("movk 32-bit: OK\n");


	registers_destroy();	
	mem_destroy();
}

