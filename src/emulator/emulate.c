#include "bit_utils.h"
#include "emu_memory.h"
#include "emu_registers.h"
#include "fde.h"
#include "instructions.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IS_HALT(instruction) ((instruction) == 0x8a000000)

int main(int argc, char **argv) {
	assert(argc >= 2);

	registers_init();
	mem_init();

	binary_loader(argv[1]);

	setZ(1);
	setN(0);
	setC(0);
	setV(0);
	while (1) {
		uint32_t instr = fetch();
		if (IS_HALT(instr))
			break;
		decode_and_execute(instr);
	}

	FILE *out = stdout;

	if (argc == 3) {
		size_t outLen = strlen(argv[2]);
		if (outLen >= 4 && !strcmp(argv[2] + outLen - 4, ".out")) {
			out = fopen(argv[2], "w");
		} else {
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

	registers_destroy();
	mem_destroy();

	return EXIT_SUCCESS;
}
