#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "emu_registers.h"
#include <inttypes.h>

int main(int argc, char **argv) {
	registers_init();
	setN(true);
	assert(getN());
	setZ(false);
	assert(!getZ());
	assert(register_load(0, false) == 0);
	assert(register_load(0, true) == 0);
	uint64_t x = (uint64_t) -1;
	register_store(0, x, true);
	assert(register_load(0, false) == UINT32_MAX);
	assert(register_load(0, true) == UINT64_MAX);
	register_store(0, (uint64_t) -1, false);
	assert(register_load(0, false) == UINT32_MAX);
	assert(register_load(0, true) == UINT32_MAX);
	assert(register_load(31, false) == 0);
	assert(register_load(31, true) == 0);
	register_store(31, 1, false);
	assert(register_load(31, false) == 0);
	assert(pc_load() == 0);
	pc_jump(4);
	assert(pc_load() == 16);
	register_store(0, 20, false);
	pc_jump_indirect(0);
	assert(pc_load() == 20);
	setZ(true);
	pc_jump_conditional(1, 0);
	assert(pc_load() == 24);
	pc_jump_conditional(1, 1);
	assert(pc_load() == 24);
	setZ(false);
	pc_jump_conditional(1, 1);
	assert(pc_load() == 28);
	setN(false);
	setV(false);
	pc_jump_conditional(1, 10);
	assert(pc_load() == 32);
	setN(true);
	pc_jump_conditional(1, 10);
	assert(pc_load() == 32);
	pc_jump_conditional(1, 11);
	assert(pc_load() == 36);
	pc_jump_conditional(1, 12);
	assert(pc_load() == 36);
	pc_jump_conditional(1, 13);
	assert(pc_load() == 40);
	setV(true);
	pc_jump_conditional(1, 12);
	assert(pc_load() == 44);
	pc_jump_conditional(1, 13);
	assert(pc_load() == 44);
	setZ(true);
	pc_jump_conditional(1, 12);
	assert(pc_load() == 44);
	pc_jump_conditional(1, 13);
	assert(pc_load() == 48);
	return EXIT_SUCCESS;
}
