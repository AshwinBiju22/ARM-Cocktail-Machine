#include <stdlib.h>
#include <stdio.h>
#include "bit_utils.h"
#include "emu_registers.h"
#include "emu_memory.h"
#include <assert.h>

#define W32 0
#define X64 1

int main(void) {
	registers_init();

	// execute_and: X64
	register_store(1, 0xF0F0F0F0F0F0F0F0, X64);
	execute_and(0, 1, 0x0FF00FF00FF00FF0, X64);
	assert(register_load(0, X64) == 0x00F000F000F000F0);
	printf("X64 execute_and: OK\n");

	// execute_orr: X64
	register_store(1, 0xF0F0F0F0F0F0F0F0, X64);
	execute_orr(0, 1, 0x0FF00FF00FF00FF0, X64);
	assert(register_load(0, X64) == 0xFFF0FFF0FFF0FFF0);
	printf("X64 execute_orr: OK\n");

	// execute_eor: X64
	register_store(1, 0xAAAA5555AAAA5555, X64);
	execute_eor(0, 1, 0x5555AAAA5555AAAA, X64);
	assert(register_load(0, X64) == 0xFFFFFFFFFFFFFFFF);
	printf("X64 execute_eor: OK\n");

	// execute_ands: X64
	register_store(1, 0x8000000000000001, X64);
	execute_ands(0, 1, 0xFFFFFFFFFFFFFFFF, X64);
	assert(register_load(0, X64) == 0x8000000000000001);
	assert(!getZ());
	assert(getN());
	assert(!getC());
	assert(!getV());
	printf("X64 execute_ands: OK\n");

	// execute_and: W32
	register_store(1, 0xF0F0F0F0, W32);
	execute_and(0, 1, 0x0FF00FF0, W32);
	assert(register_load(0, W32) == 0x00F000F0);
	printf("W32 execute_and: OK\n");

	// execute_orr: W32
	register_store(1, 0xF0F0F0F0, W32);
	execute_orr(0, 1, 0x0FF00FF0, W32);
	assert(register_load(0, W32) == 0xFFF0FFF0);
	printf("W32 execute_orr: OK\n");

	// execute_eor: W32
	register_store(1, 0xAAAA5555, W32);
	execute_eor(0, 1, 0x5555AAAA, W32);
	assert(register_load(0, W32) == 0xFFFFFFFF);
	printf("W32 execute_eor: OK\n");

	// execute_ands: W32
	register_store(1, 0x80000001, W32);
	execute_ands(0, 1, 0xFFFFFFFF, W32);
	assert(register_load(0, W32) == 0x80000001);
	assert(!getZ());
	assert(getN());
	assert(!getC());
	assert(!getV());
	printf("W32 execute_ands: OK\n");

	registers_destroy();
	return EXIT_SUCCESS;
}

