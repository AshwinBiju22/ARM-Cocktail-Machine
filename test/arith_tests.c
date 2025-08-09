#include <stdlib.h>
#include <stdio.h>
#include "bit_utils.h"
#include "emu_registers.h"
#include "emu_memory.h"
#include <assert.h>
#include "emu_registers.h"

#define W32 0
#define X64 1

int main(void) {
	registers_init();

	printf("/*  Arithmetic Tests  */\n");

	/* get_width: X64 W32 */
	uint64_t sf = 1;
	assert(get_width(sf) == (unsigned) 64);
	sf = 0;
	assert(get_width(sf) == (unsigned) 32);

	/* add_imm: X64 */
	register_store(1, 10, X64);
	add_imm(0, 1, 5, X64);
	assert(register_load(0, X64) == 15);
	printf("X64: add_imm passed\n");

	/* adds_imm: X64 */
	register_store(2, 0, X64);
	adds_imm(3, 2, 1, X64);
	assert(register_load(3, X64) == 1);
	assert(!getZ());
	assert(!getN());
	assert(!getC());
	assert(!getV());
	printf("X64: adds_imm passed\n");

	/* sub_imm: X64 */
	register_store(1, 10, X64);
	sub_imm(0, 1, 5, X64);
	assert(register_load(0, X64) == 5);
	printf("X64: sub_imm passed\n");
	
	/* subs_imm: X64 */
	register_store(2, 2, X64);
	subs_imm(3, 2, 1, X64);
	assert(register_load(3, X64) == 1);
	assert(!getZ());
	assert(!getN());
	assert(getC());
	assert(!getV());
	printf("X64: subs_imm passed\n");

	/* add_reg: X64 */
	register_store(1, 5, X64);
	register_store(2, 7, X64);
	add_reg(0, 1, 2, X64);
	assert(register_load(0, X64) == 12);
	printf("X64: add_reg passed\n");

	/* adds_reg: X64 */
	register_store(1, 5, X64);
	register_store(2, 7, X64);
	adds_reg(0, 1, 2, X64);
	assert(register_load(0, X64) == 12);
	assert(!getZ());
	assert(!getN());
	assert(!getC());
	assert(!getV());
	printf("X64: adds_reg passed\n");
	
	/* sub_reg: X64 */
	register_store(1, 19, X64);
	register_store(2, 7, X64);
	sub_reg(0, 1, 2, X64);
	assert(register_load(0, X64) == 12);
	printf("X64: sub_reg passed\n");

	/* subs_reg: X64 */
	register_store(1, 19, X64);
	register_store(2, 7, X64);
	subs_reg(0, 1, 2, X64);
	assert(register_load(0, X64) == 12);
	assert(!getZ());
	assert(!getN());
	assert(getC());
	assert(!getV());
	printf("X64: subs_reg passed\n");


	/* madd: X64 */
	register_store(1, 2, X64);
	register_store(2, 3, X64);
	register_store(3, 4, X64);
	madd(0, 1, 2, 3, X64);
	assert(register_load(0, X64) == 10);
	printf("X64: madd passed\n");
	

	/* msub: X64 */
	register_store(1, 2, X64);
	register_store(2, 3, X64);
	register_store(3, 5, X64);
	msub(0, 1, 2, 3, X64);
	assert(register_load(0, X64) == -1);
	printf("X64: msub passed\n");

	/* add_imm: W32 */
	register_store(1, 10, W32);
	add_imm(0, 1, 5, W32);
	assert(register_load(0, W32) == 15);
	printf("W32: add_imm passed\n");

	/* adds_imm: W32 */
	register_store(2, 0, W32);
	adds_imm(3, 2, 1, W32);
	assert(register_load(3, W32) == 1);
	assert(!getZ());
	assert(!getN());
	assert(!getC());
	assert(!getV());
	printf("W32: adds_imm passed\n");

	/* sub_imm: W32 */
	register_store(1, 10, W32);
	sub_imm(0, 1, 5, W32);
	assert(register_load(0, W32) == 5);
	printf("W32: sub_imm passed\n");
	
	/* subs_imm: W32 */
	register_store(2, 2, W32);
	subs_imm(3, 2, 1, W32);
	assert(register_load(3, W32) == 1);
	assert(!getZ());
	assert(!getN());
	assert(getC());
	assert(!getV());
	printf("W32: subs_imm passed\n");

	/* add_reg: W32 */
	register_store(1, 5, W32);
	register_store(2, 7, W32);
	add_reg(0, 1, 2, W32);
	assert(register_load(0, W32) == 12);
	printf("W32: add_reg passed\n");

	/* adds_reg: W32 */
	register_store(1, 5, W32);
	register_store(2, 7, W32);
	adds_reg(0, 1, 2, W32);
	assert(register_load(0, W32) == 12);
	assert(!getZ());
	assert(!getN());
	assert(!getC());
	assert(!getV());
	printf("W32: adds_reg passed\n");
	
	/* sub_reg: W32 */
	register_store(1, 19, W32);
	register_store(2, 7, W32);
	sub_reg(0, 1, 2, W32);
	assert(register_load(0, W32) == 12);
	printf("W32: sub_reg passed\n");

	/* subs_reg: W32 */
	register_store(1, 19, W32);
	register_store(2, 7, W32);
	subs_reg(0, 1, 2, W32);
	assert(register_load(0, W32) == 12);
	assert(!getZ());
	assert(!getN());
	assert(getC());
	assert(!getV());
	printf("W32: subs_reg passed\n");

	/* madd: W32 */
	register_store(1, 2, W32);
	register_store(2, 3, W32);
	register_store(3, 4, W32);
	madd(0, 1, 2, 3, W32);
	assert(register_load(0, W32) == 10);
	printf("W32: madd passed\n");
	

	/* msub: W32 */
	register_store(1, 2, W32);
	register_store(2, 3, W32);
	register_store(3, 5, W32);
	msub(0, 1, 2, 3, W32);
	assert(sign_extend(register_load(0, W32), 32) == -1); // Sign extended since output is negative
	printf("W32: msub passed\n");

	registers_destroy();

	return EXIT_SUCCESS;
}
