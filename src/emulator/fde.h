#ifndef FDE_H
#define FDE_H

#include <stdint.h>

uint32_t fetch();
void decode_and_execute(uint32_t instr);

#endif
