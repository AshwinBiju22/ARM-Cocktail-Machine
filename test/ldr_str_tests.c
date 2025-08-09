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
	mem_init();

	printf("/*  Load/Store Tests  */\n");
    
	/* ldr_uoffset: X64 */
	register_store(1, 200, X64);
	mem_store64(264, 0x1122334455667788);
	assert(ldr_uoffset(0, 1, 8, X64) == 0x1122334455667788);
	printf("X64: ldr_uoffset passed\n");

	/* str_uoffset: X64 */
	register_store(1, 400, X64);
	register_store(0, 0xAABBCCDDEEFF0011, X64);
	str_uoffset(0, 1, 5, X64);
	assert(mem_load64(440) == 0xAABBCCDDEEFF0011);
	printf("X64: str_uoffset passed\n");
	

	/* ldr_preindexed: X64 */
	register_store(1, 600, X64);
	mem_store64(608, 0xDEADBEEFCAFEBABE);
	assert(ldr_preindexed(0, 1, 8, X64) == 0xDEADBEEFCAFEBABE);
	assert(register_load(1, X64) == 608);
	printf("X64: ldr_preindexed passed\n");
	
	/* str_preindexed: X64 */
	register_store(1, 800, X64);
	register_store(0, 0x123456789ABCDEF0, X64);
	str_preindexed(0, 1, 16, X64); 
	assert(mem_load64(816) == 0x123456789ABCDEF0);
	assert(register_load(1, X64) == 816);
	printf("X64: str_preindexed passed\n");

	/* ldr_postindexed: X64 */
	register_store(1, 1000, X64);
	mem_store64(1000, 0xBEEFDEADCAFEBABE);
	assert(ldr_postindexed(0, 1, 12, X64) == 0xBEEFDEADCAFEBABE);
	assert(register_load(1, X64) == 1012);
	printf("X64: ldr_postindexed passed\n");

	/* str_postindexed: X64 */
	register_store(1, 1200, X64);
	register_store(0, 0xCAFEBABECAFEBABE, X64);
	str_postindexed(0, 1, 24, X64);
	assert(mem_load64(1200) == 0xCAFEBABECAFEBABE);
	assert(register_load(1, X64) == 1224);
	printf("X64: str_postindexed passed\n");
	
	/* ldr_regoffset: X64 */
	register_store(1, 1400, X64);
	register_store(2, 20, X64);
	mem_store64(1420, 0xDEAD1111BEEF2222);
	assert(ldr_regoffset(0, 1, 2, X64) == 0xDEAD1111BEEF2222);
	printf("X64: ldr_regoffset passed\n");

	/* str_regoffset: X64 */
	register_store(1, 1600, X64);
	register_store(2, 32, X64);
	register_store(0, 0x4444555566667777, X64);
	str_regoffset(0, 1, 2, X64);
	assert(mem_load64(1632) == 0x4444555566667777);
	printf("X64: str_regoffset passed\n");

	/* ldr_literal: X64 */
	uint64_t pc_x = pc_load();
	mem_store64(pc_x + 12, 0xAABBCCDDEEFF0011);  
	assert(ldr_literal(0, 3, X64) == 0xAABBCCDDEEFF0011);
	printf("X64: ldr_literal passed\n");	

	/* ldr_uoffset: W32 */
	register_store(1, 100, X64);
	mem_store32(116, 0xCAFEBABE);
	assert(ldr_uoffset(0, 1, 4, W32) == 0xCAFEBABE);
	printf("W32: ldr_uoffset passed\n");

	/* str_uoffset: W32 */
	register_store(1, 300, X64);
	register_store(0, 0x12345678, W32);
	str_uoffset(0, 1, 2, W32); 
	assert(mem_load32(308) == 0x12345678);
	printf("W32: str_uoffset passed\n");

	/* ldr_preindexed: W32 */
	register_store(1, 500, X64);
	mem_store32(508, 0xABCD1234);  
	assert(ldr_preindexed(0, 1, 8, W32) == 0xABCD1234);
	assert(register_load(1, X64) == 508);  
	printf("W32: ldr_preindexed passed\n");
	
	/* str_preindexed: W32 */
	register_store(1, 700, X64);
	register_store(0, 0x55667788, W32);
	str_preindexed(0, 1, 12, W32); 
	assert(mem_load32(712) == 0x55667788);
	assert(register_load(1, X64) == 712);
	printf("W32: str_preindexed passed\n");

	/* ldr_postindexed: W32 */
	register_store(1, 900, X64);
	mem_store32(900, 0xDEADBEEF);
	assert(ldr_postindexed(0, 1, 4, W32) == 0xDEADBEEF);
	assert(register_load(1, X64) == 904);
	printf("W32: ldr_postindexed passed\n");


	/* str_postindexed: W32 */
	register_store(1, 1100, X64);
	register_store(0, 0xFACEFACE, W32);
	str_postindexed(0, 1, 16, W32);
	assert(mem_load32(1100) == 0xFACEFACE);
	assert(register_load(1, X64) == 1116);
	printf("W32: str_postindexed passed\n");
	
	/* ldr_regoffset: W32 */
	register_store(1, 1300, X64); 
	register_store(2, 12, X64);  
	mem_store32(1312, 0x11112222);
	assert(ldr_regoffset(0, 1, 2, W32) == 0x11112222);
	printf("W32: ldr_regoffset passed\n");

	/* str_regoffset: W32 */
	register_store(1, 1500, X64);
	register_store(2, 16, X64);
	register_store(0, 0x77778888, W32);
	str_regoffset(0, 1, 2, W32);
	assert(mem_load32(1516) == 0x77778888);
	printf("W32: str_regoffset passed\n");

	/* ldr_literal: W32 */
	uint64_t pc_w = pc_load();
	mem_store32(pc_w + 4, 0xFEEDBEEF);
	assert(ldr_literal(0, 1, W32) == 0xFEEDBEEF);
	printf("W32: ldr_literal passed\n");

	registers_destroy();
	mem_destroy();
	
	return EXIT_SUCCESS;
}