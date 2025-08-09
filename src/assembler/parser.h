#include "ir.h"
#include <stddef.h>

#ifndef PARSER_H
#define PARSER_H

void populate_symbol_table(char *filename);

uint32_t *parse_file(char *filename);

size_t get_instr_num(void);

#endif
