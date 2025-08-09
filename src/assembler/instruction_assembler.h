#include "ir.h"
#include <stdint.h>

#ifndef INSTRUCTION_ASSEMBLER_H
#define INSTRUCTION_ASSEMBLER_H

// Data Processing
uint32_t multiply_encoder(Instr instr);
uint32_t move_encoder(Instr instr);
uint32_t logic_encoder(Instr instr);
uint32_t arithmetic_encoder(Instr instr);

// Single Data Transfer
uint32_t encode_ldrstr(Instr instr);

// Branch
uint32_t encode_branch(Instr instr);

// Special Instructions / Directives
uint32_t encode_directive(Instr instr);

#endif
