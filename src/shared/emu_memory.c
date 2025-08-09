#include "emu_memory.h"
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 1024
#define MEM_CHECK_BOUNDS(addr, size) assert((addr) + (size) <= MEMORY_SIZE)

// using real memory to emulate memory
static uint8_t *memory = NULL;

void mem_init(void) {
	if (!memory) {
		memory = malloc(MEMORY_SIZE);
		assert(memory);
		memset(memory, 0, MEMORY_SIZE);
	}
}

void mem_destroy(void) {
	free(memory);
	memory = NULL;
}

// All this assumes Little-Endian

uint8_t mem_load8(uint32_t addr) {
	MEM_CHECK_BOUNDS(addr, 1);
	return memory[addr];
}

uint16_t mem_load16(uint32_t addr) {
	MEM_CHECK_BOUNDS(addr, 2);
	return memory[addr] | ((uint16_t)memory[addr + 1] << 8);
}

uint32_t mem_load32(uint32_t addr) {
	MEM_CHECK_BOUNDS(addr, 4);
	uint64_t to_return = 0;
	for (int i = 0; i < 4; ++i) {
		to_return |= ((uint64_t)memory[addr + i] << (i * 8));
	}
	return to_return;
}

uint64_t mem_load64(uint32_t addr) {
	MEM_CHECK_BOUNDS(addr, 8);
	uint64_t to_return = 0;
	for (int i = 0; i < 8; ++i) {
		to_return |= ((uint64_t)memory[addr + i] << (i * 8));
	}
	return to_return;
}

void mem_store8(uint32_t addr, uint8_t value) {
	MEM_CHECK_BOUNDS(addr, 1);
	memory[addr] = value;
}

void mem_store16(uint32_t addr, uint16_t value) {
	MEM_CHECK_BOUNDS(addr, 2);
	memory[addr] = value & 0xFF;
	memory[addr + 1] = (value >> 8) & 0xFF;
}

void mem_store32(uint32_t addr, uint32_t value) {
	MEM_CHECK_BOUNDS(addr, 4);
	for (int i = 0; i < 4; ++i) {
		memory[addr + i] = (value >> 8 * i) & 0xFF;
	}
}

void mem_store64(uint32_t addr, uint64_t value) {
	MEM_CHECK_BOUNDS(addr, 8);
	for (int i = 0; i < 8; ++i) {
		memory[addr + i] = (value >> 8 * i) & 0xFF;
	}
}

// debug function
void mem_dump(uint32_t addr, size_t length) {
	MEM_CHECK_BOUNDS(addr, length);
	for (size_t i = 0; i < length; ++i) {
		if (i % 16 == 0)
			printf("\n%08lx: ", addr + i);
		printf("%02x ", memory[addr + i]);
	}
	printf("\n");
}

// for exporting final state
void export_memory(FILE *out) {
	fprintf(out, "Non-Zero Memory:\n");
	for (size_t i = 0; i < MEMORY_SIZE; i += 4) {
		uint32_t memVal = mem_load32(i);
		if (memVal) {
			fprintf(out, "0x%08zx: %08" PRIx32 "\n", i, memVal);
		}
	}
}

// loads bin file into memory
void binary_loader(char *filename) {
	FILE *objcode = fopen(filename, "rb");

	if (!objcode) {
		perror("Failed to open binary file");
		return;
	}

	uint8_t buffer[BUFFERSIZE];
	size_t offset = 0;

	size_t bytes_read = fread(buffer, 1, BUFFERSIZE, objcode);
	while (bytes_read != 0) { // reads bytes until EOF
		for (size_t i = 0; i < bytes_read; ++i) {
			mem_store8(offset + i, buffer[i]); // flushes buffer into memory
		}
		offset += bytes_read;
		bytes_read = fread(buffer, 1, BUFFERSIZE, objcode);
	}
	fclose(objcode);
}
