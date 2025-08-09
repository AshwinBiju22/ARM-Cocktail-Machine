#ifndef EMU_MEMORY_H
#define EMU_MEMORY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define MEMORY_SIZE (1 << 21) // 1 MiB

void mem_init(void);

void mem_destroy(void);

uint8_t mem_load8(uint32_t addr);
uint16_t mem_load16(uint32_t addr);
uint32_t mem_load32(uint32_t addr);
uint64_t mem_load64(uint32_t addr);

void mem_store8(uint32_t addr, uint8_t value);
void mem_store16(uint32_t addr, uint16_t value);
void mem_store32(uint32_t addr, uint32_t value);
void mem_store64(uint32_t addr, uint64_t value);

void mem_dump(uint32_t addr, size_t length);

void export_memory(FILE *out);

void binary_loader(char *filename);

#endif
