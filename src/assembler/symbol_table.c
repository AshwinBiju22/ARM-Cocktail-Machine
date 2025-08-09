#include "symbol_table.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LOAD_FACTOR 0.75

static SymbolTable *st = NULL;

// djb2 hash function
static unsigned long hash(const char *str) {
	unsigned long hash = 5381;
	int c;
	while ((c = *str++)) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}

void symbol_table_create(size_t capacity) {
	if (!st) {
		st = (SymbolTable *)malloc(sizeof(SymbolTable));
		st->capacity = capacity;
		st->size = 0;
		st->buckets = calloc(capacity, sizeof(SymbolEntry *));
	}
}

void symbol_table_destroy(void) {
	if (st) {
		for (size_t i = 0; i < st->capacity; ++i) {
			SymbolEntry *entry = st->buckets[i];
			while (entry) {
				SymbolEntry *next = entry->next;
				free(entry->label);
				free(entry);
				entry = next;
			}
		}
		free(st->buckets);
		free(st);
	}
}

static void symbol_table_resize(void) {
	size_t new_capacity = st->capacity * 2;
	SymbolEntry **new_buckets = calloc(new_capacity, sizeof(SymbolEntry *));
	for (int i = 0; i < st->capacity; ++i) {
		SymbolEntry *entry = st->buckets[i];
		while (entry) {
			SymbolEntry *next = entry->next;
			unsigned long new_index = hash(entry->label) % new_capacity;
			entry->next = new_buckets[new_index];
			new_buckets[new_index] = entry;
			entry = next;
		}
	}
	free(st->buckets);
	st->buckets = new_buckets;
	st->capacity = new_capacity;
}

void symbol_table_put(const char *label, uint32_t address) {
	if ((float)st->size / st->capacity > MAX_LOAD_FACTOR) {
		symbol_table_resize();
	}
	unsigned long index = hash(label) % st->capacity;
	SymbolEntry *entry = st->buckets[index];
	while (entry) {
		if (strcmp(entry->label, label) == 0) {
			errno = 1;
			fprintf(stderr, "Duplicate label definition undefined\n");
			exit(EXIT_FAILURE);
			return;
		}
		entry = entry->next;
	}
	entry = malloc(sizeof(SymbolEntry));
	entry->label = malloc(strlen(label) + 1);
	strcpy(entry->label, label);
	entry->address = address;
	entry->next = st->buckets[index];
	st->buckets[index] = entry;
	st->size++;
}

uint32_t symbol_table_get(const char *label) {
	unsigned long index = hash(label) % st->capacity;
	SymbolEntry *entry = st->buckets[index];
	while (entry) {
		if (strcmp(entry->label, label) == 0)
			return entry->address;
		entry = entry->next;
	}
	errno = 1;
	fprintf(stderr, "Label not found in symbol table\n");
	exit(EXIT_FAILURE);
	return -1;
}

size_t get_symbol_table_size(void) { return st->size; }

size_t get_symbol_table_capacity(void) { return st->capacity; }
