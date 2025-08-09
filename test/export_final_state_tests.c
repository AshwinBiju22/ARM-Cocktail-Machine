#include <stdlib.h>
#include "emu_memory.h"
#include <stdio.h>
#include "bit_utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include "emu_registers.h"
#include <string.h>

#define IS_HALT(instruction) ((instruction) == 0x8a000000)

int main(int argc, char **argv) {
	// if (is_halt(instruction)) break;
	assert( argc >= 2 );
	// Add Michael's file import stuff here
	
	mem_init();
	registers_init();
	mem_store8(0, 109);
	mem_store64(0x222, 1);
	mem_store32(0xFF, 0xFFFFAAAA);
	setN(true);
	setC(true);
	register_store(5, 0xFFFF, false);
	register_store(2, 0xABCDEF, true);
	register_store(30, 0x222, false);
	pc_jump(100);	
	//
	// Main loop goes here
	//
	
	FILE* out = stdout;	

	if (argc == 3) {
		size_t outLen = strlen(argv[2]);
		if (outLen >= 4 && !strcmp(argv[2] + outLen - 4, ".out")) {
			out = fopen(argv[2], "w");
		}
		else {
			char *new = malloc(outLen + 5);
			strcpy(new, argv[2]);
			strcat(new, ".out");
			out = fopen(new, "w");
			free(new);
		}
		if (out == NULL) {
			return EXIT_FAILURE;
		}
	}
	
	export_normal_registers(out);
	export_pc(out);
	export_pstate(out);
	export_memory(out);
	
	if (out != stdout) {
		fclose(out);
	}
	mem_destroy();
	registers_destroy();
	return EXIT_SUCCESS;
	
}
