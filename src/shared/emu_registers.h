#ifndef EMU_REGISTERS_H
#define EMU_REGISTERS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define NUM_REGISTERS 31

void registers_init(void);
void registers_destroy(void);

void setN(bool val);
void setZ(bool val);
void setC(bool val);
void setV(bool val);

bool getN(void);
bool getZ(void);
bool getC(void);
bool getV(void);

uint64_t register_load(unsigned addr, bool sf);
void register_store(unsigned addr, uint64_t value, bool sf);

void pc_jump(uint32_t offset);
void pc_jump_indirect(unsigned reg);
void pc_jump_conditional(uint32_t offset, uint8_t cond);

uint64_t pc_load(void);

void export_normal_registers(FILE *out);
void export_pc(FILE *out);
void export_pstate(FILE *out);
#endif
