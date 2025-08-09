#include <stddef.h>
#include <stdint.h>

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef struct SymbolEntry {
	char *label;
	uint32_t address;
	struct SymbolEntry *next;
} SymbolEntry;

typedef struct {
	SymbolEntry **buckets;
	size_t size;
	size_t capacity;
} SymbolTable;

void symbol_table_create(size_t capacity);
void symbol_table_destroy(void);
void symbol_table_put(const char *label, uint32_t address);
uint32_t symbol_table_get(const char *label);

size_t get_symbol_table_size(void);
size_t get_symbol_table_capacity(void);

#endif
