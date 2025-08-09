#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "symbol_table.h"

int main(void) {
	
	symbol_table_create(16);
	assert(get_symbol_table_capacity() == 16);
	assert(get_symbol_table_size() == 0);

	symbol_table_put("label1", 10);
	assert(get_symbol_table_capacity() == 16);
	assert(get_symbol_table_size() == 1);
	assert(symbol_table_get("label1") == 10);

	
	symbol_table_put("label2", 15);
	assert(get_symbol_table_capacity() == 16);
	assert(get_symbol_table_size() == 2);
	assert(symbol_table_get("label2") == 15);

	char label[8];
	for (int i = 0; i < 12; ++i) {
		snprintf(label, sizeof(label), "label%d", i+3);
		symbol_table_put(label, 16+i);
	}
	assert(get_symbol_table_size() == 14);
	assert(get_symbol_table_capacity() == 32);
		
	// the below strings have the same djb2 hash
	symbol_table_put("heliotropes", 100);
	symbol_table_put("neurospora", 101);
	assert(get_symbol_table_size() == 16);
	assert(symbol_table_get("heliotropes") == 100);
	assert(symbol_table_get("neurospora") == 101);
	
	symbol_table_destroy();
	return EXIT_SUCCESS;
}
