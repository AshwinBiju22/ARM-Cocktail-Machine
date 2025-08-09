#include "../shared/emu_registers.h"
#include "bin_writer.h"
#include "parser.h"
#include "symbol_table.h"
#include <assert.h>
#include <stdlib.h>

#define MAX_LINE_LEN 1024

int main(int argc, char **argv) {
	assert(argc == 3);
	assert(pc_load() == 0);

	symbol_table_create(16);
	populate_symbol_table(argv[1]);
	size_t num_instr = get_instr_num();
	uint32_t *encoded_instr = parse_file(argv[1]);
	bin_writer(encoded_instr, num_instr, argv[2]);

	symbol_table_destroy();
	free(encoded_instr);
	return EXIT_SUCCESS;
}
