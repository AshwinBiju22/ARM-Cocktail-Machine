#include "parser.h"
#include "emu_registers.h"
#include "encoder.h"
#include "ir.h"
#include "symbol_table.h"
#include <assert.h>
#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 1024

static size_t file_len = 0;

bool is_empty_line(char *str) {
	for (; *str; ++str) {
		if (!isspace(*str)) {
			return false;
		}
	}
	return true;
}

void populate_symbol_table(char *filename) {
	FILE *in = fopen(filename, "r");

	if (!in) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	char buffer[MAX_LINE_LEN];
	const char *label_pattern = "^\\s*([a-zA-Z_\\.][a-zA-Z0-9$_\\.]*):\\s*\n$";
	regex_t label_regex;
	if (regcomp(&label_regex, label_pattern, REG_EXTENDED)) {
		fprintf(stderr, "Failed to compile regex\n");
		exit(EXIT_FAILURE);
	}
	regmatch_t matches[2];
	while (fgets(buffer, sizeof(buffer), in)) {
		if (regexec(&label_regex, buffer, 2, matches, 0) == 0) { // matches labels
			int start = matches[1].rm_so;
			int end = matches[1].rm_eo;
			int len = end - start;
			char *label = malloc(len + 1);
			strncpy(label, buffer + start, len);
			label[len] = '\0';
			symbol_table_put(label, pc_load());
			free(label);
		} else if (!is_empty_line(buffer)) {
			pc_jump(1); // skips empty lines
		}
	}

	file_len = pc_load() >> 2;
	pc_jump((uint64_t)-file_len); // reset PC
	fclose(in);
	regfree(&label_regex);
}

size_t get_instr_num(void) { return file_len; }

char *remove_whitespace(const char *string) {
	const char *start = string;
	const char *end;

	while (*start && isspace(*start))
		start++;

	assert(*start != '\0'); // already have a check if not null

	end = string + strlen(string) - 1;
	while (end > start && (isspace(*end) || *end == '\0'))
		end--;

	size_t len = end - start + 1;
	char *trimmed = malloc(len + 1);
	if (!trimmed)
		return NULL;
	memcpy(trimmed, start, len);
	trimmed[len] = '\0';
	return trimmed;
}

// Converts "X_" or "W_" into Reg struct
Reg parse_register(char *token) {
	Reg r;
	r.sf = (tolower(token[0]) == 'x');
	r.reg_num = (unsigned)atoi(token + 1);
	if (strcmp(token + 1, "zr") == 0) {
		r.reg_num = 31;
	}
	if (r.reg_num == 0 && (strlen(token) == 1 || !isdigit(token[1]))) {
		r.reg_num = 32;
	}
	return r;
}

// Parses a register or immediate
Operand parse_operand(char *token, char *shift, char *shift_amount) {
	Operand op;

	switch (tolower(token[0])) {
	case '#':
		op.type = IMM;
		op.literal.type = LITERAL_INT;
		op.literal.imm = strtoull(token + 1, NULL, 0);
		break;
	case 'x':
	case 'w':
		op.type = REG;
		op.reg = parse_register(token);
		if (op.reg.reg_num < 32) {
			break; // all parsed well
		}
	default:
		op.type = IMM;
		op.literal.type = LITERAL_LABEL;
		op.literal.label = token;
	}
	op.shift = LSL;
	op.shift_amount = 0;

	char *shifts[4] = {"lsl", "lsr", "asr", "ror"};
	for (size_t i = 0; i < 4 && shift; ++i) {
		if (strcmp(shift, shifts[i]) == 0) {
			op.shift = i;
			break;
		}
	}

	if (shift_amount) {
		op.shift_amount = strtoull(shift_amount + 1, NULL, 0);
	}

	return op;
}

Instr parseLine(char *line) {
	Instr instr;

	// Tokenise
	char *tokens[6] = {"", "", "", "", "", ""};
	int count = 0;

	char *token = strtok(line, " ");

	while (count < 6) {
		if (token == NULL) {
			break;
		}

		tokens[count] = token;
		count++;

		token = strtok(NULL, ", ");
	}

	char *opcode = tokens[0];
	// Arithmetic Parsing
	if (strcmp(opcode, "add") == 0 || strcmp(opcode, "adds") == 0 || strcmp(opcode, "sub") == 0 ||
		strcmp(opcode, "subs") == 0) {
		instr.type = INSTR_ARITHMETIC;
		instr.arithmetic.rd = parse_register(tokens[1]);
		instr.arithmetic.rn = parse_register(tokens[2]);
		instr.arithmetic.op2 = parse_operand(tokens[3], tokens[4], tokens[5]);
		instr.arithmetic.set_flags = (tokens[0][3] == 's');
		instr.arithmetic.neg = (tokens[0][0] == 's');

	} else if (strcmp(opcode, "cmp") == 0 || strcmp(opcode, "cmn") == 0) {
		instr.type = INSTR_ARITHMETIC;
		instr.arithmetic.rn = parse_register(tokens[1]);

		if (tolower(tokens[1][0]) == 'x') {
			instr.arithmetic.rd = parse_register("x31");
		} else {
			instr.arithmetic.rd = parse_register("w31");
		}

		instr.arithmetic.op2 = parse_operand(tokens[2], tokens[3], tokens[4]);
		instr.arithmetic.set_flags = true;
		instr.arithmetic.neg = (opcode[2] == 'p');

	} else if (strcmp(opcode, "neg") == 0 || strcmp(opcode, "negs") == 0) {
		instr.type = INSTR_ARITHMETIC;
		instr.arithmetic.rd = parse_register(tokens[1]);

		if (tolower(tokens[1][0]) == 'x') {
			instr.arithmetic.rn = parse_register("x31");
		} else {
			instr.arithmetic.rn = parse_register("w31");
		}

		instr.arithmetic.op2 = parse_operand(tokens[2], tokens[3], tokens[4]);
		instr.arithmetic.set_flags = (opcode[3] == 's');
		instr.arithmetic.neg = true;

		// Logical Parsing
	} else if (strcmp(opcode, "and") == 0 || strcmp(opcode, "ands") == 0 ||
			   strcmp(opcode, "bic") == 0 || strcmp(opcode, "bics") == 0 ||
			   strcmp(opcode, "eor") == 0 || strcmp(opcode, "eon") == 0 ||
			   strcmp(opcode, "orr") == 0 || strcmp(opcode, "orn") == 0) {
		instr.type = INSTR_LOGIC;
		instr.logical.rd = parse_register(tokens[1]);
		instr.logical.rn = parse_register(tokens[2]);
		instr.logical.op2 = parse_operand(tokens[3], tokens[4], tokens[5]);
		if (opcode[0] == 'e') {
			instr.logical.ltype = XOR;
		} else if (opcode[0] == 'o') {
			instr.logical.ltype = OR;
		} else {
			instr.logical.ltype = AND;
		}
		instr.logical.set_flags = (opcode[3] == 's');
		instr.logical.neg = (opcode[0] == 'b' || opcode[2] == 'n');

	} else if (strcmp(opcode, "tst") == 0) {
		instr.type = INSTR_LOGIC;
		if (tolower(tokens[1][0]) == 'x') {
			instr.logical.rd = parse_register("x31");
		} else {
			instr.logical.rd = parse_register("w31");
		}
		instr.logical.rn = parse_register(tokens[1]);
		instr.logical.op2 = parse_operand(tokens[2], tokens[3], tokens[4]);
		instr.logical.ltype = AND;
		instr.logical.set_flags = true;
		instr.logical.neg = false;

	} else if (strcmp(opcode, "mvn") == 0) {
		instr.type = INSTR_LOGIC;
		if (tolower(tokens[1][0]) == 'x') {
			instr.arithmetic.rn = parse_register("x31");
		} else {
			instr.arithmetic.rn = parse_register("w31");
		}
		instr.logical.rd = parse_register(tokens[1]);
		instr.logical.op2 = parse_operand(tokens[2], tokens[3], tokens[4]);
		instr.logical.ltype = OR;
		instr.logical.set_flags = false;
		instr.logical.neg = true;

	} else if (strcmp(opcode, "mov") == 0) {
		instr.type = INSTR_LOGIC;
		if (tolower(tokens[1][0]) == 'x') {
			instr.logical.rn = parse_register("x31");
		} else {
			instr.logical.rn = parse_register("w31");
		}
		instr.logical.rd = parse_register(tokens[1]);
		instr.logical.op2 = parse_operand(tokens[2], tokens[3], tokens[4]);
		instr.logical.ltype = OR;
		instr.logical.set_flags = false;
		instr.logical.neg = false;

		// Wide Move Parsing
	} else if (strcmp(opcode, "movn") == 0 || strcmp(opcode, "movk") == 0 ||
			   strcmp(opcode, "movz") == 0) {
		instr.type = INSTR_MOVE;
		instr.move.rd = parse_register(tokens[1]);
		instr.move.op = parse_operand(tokens[2], tokens[3], tokens[4]);

		if (tokens[3] && strncmp(tokens[3], "lsl", 3) == 0) {
			instr.move.op.shift_amount = strtoull(tokens[3] + 5, NULL, 0);
		}

		instr.move.neg = (tokens[0][3] == 'n');
		instr.move.with_keep = (tokens[0][3] == 'k');

		// Multiplication Parsing
	} else if (strcmp(opcode, "madd") == 0 || strcmp(opcode, "msub") == 0) {
		instr.type = INSTR_MUL;
		instr.multiply.rd = parse_register(tokens[1]);
		instr.multiply.rn = parse_register(tokens[2]);
		instr.multiply.rm = parse_register(tokens[3]);
		instr.multiply.ra = parse_register(tokens[4]);
		instr.multiply.neg = (tokens[0][1] == 's');

	} else if (strcmp(opcode, "mul") == 0 || strcmp(opcode, "mneg") == 0) {
		instr.type = INSTR_MUL;
		instr.multiply.rd = parse_register(tokens[1]);
		instr.multiply.rn = parse_register(tokens[2]);
		instr.multiply.rm = parse_register(tokens[3]);
		if (tolower(tokens[1][0]) == 'x') {
			instr.multiply.ra = parse_register("x31");
		} else {
			instr.multiply.ra = parse_register("w31");
		}
		instr.multiply.neg = (tokens[0][1] == 'n');

		// Branching Parsing
	} else if (strcmp(opcode, "br") == 0) {
		instr.type = INSTR_BRANCH;
		instr.branch.op = parse_operand(tokens[1], tokens[2], tokens[3]);
		instr.branch.is_cond = false;
		instr.branch.cond = NO;

	} else if (strncmp(opcode, "b.", 2) == 0) {
		instr.type = INSTR_BRANCH;
		instr.branch.is_cond = true;

		if (strcmp(opcode, "b.eq") == 0) {
			instr.branch.cond = EQ;
		} else if (strcmp(opcode, "b.ne") == 0) {
			instr.branch.cond = NE;
		} else if (strcmp(opcode, "b.ge") == 0) {
			instr.branch.cond = GE;
		} else if (strcmp(opcode, "b.lt") == 0) {
			instr.branch.cond = LT;
		} else if (strcmp(opcode, "b.gt") == 0) {
			instr.branch.cond = GT;
		} else if (strcmp(opcode, "b.le") == 0) {
			instr.branch.cond = LE;
		} else {
			instr.branch.cond = AL;
		}

		instr.branch.op = parse_operand(tokens[1], tokens[2], tokens[3]);

	} else if (strcmp(opcode, "b") == 0) {
		instr.type = INSTR_BRANCH;
		instr.branch.op = parse_operand(tokens[1], tokens[2], tokens[3]);
		instr.branch.is_cond = false;
		instr.branch.cond = NO;

		// Single Data Transfer Parsing
	} else if (strcmp(opcode, "ldr") == 0 || strcmp(opcode, "str") == 0) {
		instr.type = INSTR_TRANSFER;
		instr.single_data_transfer.store = (opcode[0] == 's');
		instr.single_data_transfer.rt = parse_register(tokens[1]);

		Address addr;
		addr.base.sf = false;
		addr.base.reg_num = 0;
		addr.type = UNSIGNED_OFFSET;
		addr.imm_offset = 0;
		if (tokens[2][0] == '[') {
			addr.base = parse_register(tokens[2] + 1);
			if (tokens[2][strlen(tokens[2]) - 1] == ']') {
				if (strcmp(tokens[3], "")) { // Post-Indexed
					addr.type = POST_INDEXED;
					addr.imm_offset = (uint64_t)strtoll(tokens[3] + 1, NULL, 0);
				} else { // Zero Unsigned Offset
					addr.type = UNSIGNED_OFFSET;
					addr.imm_offset = 0;
				}
			} else {
				if (tokens[3][strlen(tokens[3]) - 1] == '!') { // Pre-Indexed
					addr.type = PRE_INDEXED;
					addr.imm_offset = (uint64_t)strtoll(tokens[3] + 1, NULL, 0);
				} else {
					if (tokens[3][0] == '#') {
						addr.type = UNSIGNED_OFFSET;
						addr.imm_offset = strtoll(tokens[3] + 1, NULL, 0);
					} else {
						addr.type = REGISTER_OFFSET;
						addr.reg_offset = parse_register(tokens[3]);
					}
				}
			}
		} else {
			// Literal: label or #imm
			addr.type = LITERAL;
			addr.base.sf = true;
			addr.base.reg_num = 31;

			if (tokens[2][0] == '#') {
				addr.literal.type = LITERAL_INT;
				addr.literal.imm = strtoull(tokens[2] + 1, NULL, 0);
			} else {
				addr.literal.type = LITERAL_LABEL;
				addr.literal.label = strdup(tokens[2]);
			}
		}

		instr.single_data_transfer.address = addr;

		// Directive Parsing
	} else if (strcmp(opcode, ".int") == 0) {
		instr.type = INSTR_DIRECTIVE;
		instr.directive = strtoull(tokens[1], NULL, 0);

	} else {
		printf("Unknown instruction: %s\n", tokens[0]);
	}

	return instr;
}

uint32_t *parse_file(char *filename) {
	assert(pc_load() == 0);
	FILE *in = fopen(filename, "r");
	if (!in) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	uint32_t *encoded = malloc(file_len * sizeof(uint32_t));
	char buffer[MAX_LINE_LEN];
	const char *label_pattern = "^\\s*([a-zA-Z_\\.][a-zA-Z0-9$_\\.]*):\\s*\n$";
	regex_t label_regex;
	if (regcomp(&label_regex, label_pattern, REG_EXTENDED)) {
		fprintf(stderr, "Failed to compile regex\n");
		exit(EXIT_FAILURE);
	}
	regmatch_t matches[2];
	size_t i = 0;
	while (fgets(buffer, sizeof(buffer), in)) {
		if (regexec(&label_regex, buffer, 2, matches, 0) && !is_empty_line(buffer)) {
			if (i >= file_len) {
				fprintf(stderr, "Line number out of bounds for expected file length");
				exit(1);
			}

			char *cleaned_line = remove_whitespace(buffer);
			Instr instr = parseLine(cleaned_line);
			encoded[i] = encode(instr);
			pc_jump(1);
			i++;
			free(cleaned_line);
		}
	}

	fclose(in);
	assert(pc_load() >> 2 == file_len);
	regfree(&label_regex);
	return encoded;
}
