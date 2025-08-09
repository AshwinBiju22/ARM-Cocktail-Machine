#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdbool.h>
#include <stdint.h>

void execute_movn(uint32_t instruction);
void execute_movz(uint32_t instruction);
void execute_movk(uint32_t instruction);

#endif
