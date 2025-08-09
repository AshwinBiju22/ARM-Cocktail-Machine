#include "instruction_assembler.h"
#include "../shared/emu_registers.h"
#include "ir.h"
#include "symbol_table.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Data Processing
uint32_t multiply_encoder(Instr instr) {
	assert(instr.type == INSTR_MUL);
	Reg rd = instr.multiply.rd;
	Reg ra = instr.multiply.ra;
	Reg rn = instr.multiply.rn;
	Reg rm = instr.multiply.rm;

	uint32_t sf = (uint32_t)(rd.sf && ra.sf && rn.sf && rm.sf) << 31;
	uint32_t base = 0xD8 << 21; // 00 1101 1000

	return sf | base | (uint32_t)rm.reg_num << 16 | (uint32_t)instr.multiply.neg << 15 |
		   (uint32_t)ra.reg_num << 10 | (uint32_t)rn.reg_num << 5 | (uint32_t)rd.reg_num;
}

static uint32_t get_opr(Instr_Type it, Shift_Type st, bool neg) {
	switch (st) {
	case LSL:
		return it == INSTR_ARITHMETIC ? 0x8 : 0x0 + (uint32_t)neg; // 1000 / 000N Logical
	case LSR:
		return it == INSTR_ARITHMETIC ? 0xA : 0x2 + (uint32_t)neg; // 1010 / 001N Logical
	case ASR:
		return it == INSTR_ARITHMETIC ? 0xC : 0x4 + (uint32_t)neg; // 1100 / 010N Logical
	case ROR:
		assert(it != INSTR_ARITHMETIC);
		return 0x6 + (uint32_t)neg; // 011N Logical only
	default:
		printf("ERROR: Unknown shift type for opr\n");
		exit(EXIT_FAILURE);
	}
}

uint32_t logic_encoder(Instr instr) {
	assert(instr.type == INSTR_LOGIC);

	Reg rd = instr.logical.rd;
	Reg rn = instr.logical.rn;
	Operand op2 = instr.logical.op2;
	bool neg = instr.logical.neg;
	bool set_flags = instr.logical.set_flags;
	Logical_Type ltype = instr.logical.ltype;

	uint32_t sf = (rd.sf && rn.sf && op2.reg.sf) << 31;
	uint32_t opc = (ltype == AND) ? (set_flags ? 0x3 : 0x0)
								  : (ltype == OR ? 0x1 : 0x2); // XOR when ltype not AND or OR
	uint32_t base = 0x5 << 25;								   // M101, M = 0
	uint32_t opr = get_opr(instr.type, op2.shift, neg) << 21;
	uint32_t rm = op2.reg.reg_num << 16;
	uint32_t operand = op2.shift_amount << 10;

	return sf | (opc << 29) | base | opr | rm | operand | (rn.reg_num << 5) | rd.reg_num;
}

uint32_t move_encoder(Instr instr) {
	assert(instr.type == INSTR_MOVE);
	Reg rd = instr.move.rd;
	Operand op = instr.move.op;

	uint32_t sf = (uint32_t)rd.sf << 31;
	uint32_t opc = (((uint32_t)(!instr.move.neg) << 1) | (uint32_t)instr.move.with_keep) << 29;
	uint32_t base = 0x4 << 26; // 100
	uint32_t opi = 0x5 << 23;  // 101 wide mov
	uint32_t hw = (op.shift_amount / 16) << 21;
	uint32_t imm16 = op.literal.imm << 5;
	return sf | opc | base | opi | hw | imm16 | rd.reg_num;
}

static uint32_t encode_reg_a(Instr instr) {
	assert(instr.type == INSTR_ARITHMETIC);

	Reg rd = instr.arithmetic.rd;
	Reg rn = instr.arithmetic.rn;
	Operand op2 = instr.arithmetic.op2;
	bool neg = instr.arithmetic.neg;
	bool set_flags = instr.arithmetic.set_flags;

	uint32_t sf = (rd.sf && rn.sf && op2.reg.sf) << 31;
	uint32_t opc = ((uint32_t)neg << 1) | (uint32_t)set_flags;
	uint32_t base = 0x5 << 25; // M101, M = 0
	uint32_t opr = get_opr(instr.type, op2.shift, neg) << 21;
	uint32_t rm = op2.reg.reg_num << 16;
	uint32_t operand = op2.shift_amount << 10;

	return sf | (opc << 29) | base | opr | rm | operand | (rn.reg_num << 5) | rd.reg_num;
}

static uint32_t encode_imm_a(Instr instr) {
	assert(instr.type == INSTR_ARITHMETIC);
	Reg rd = instr.arithmetic.rd;
	Reg rn = instr.arithmetic.rn;
	Operand op2 = instr.arithmetic.op2;
	bool neg = instr.arithmetic.neg;
	bool set_flags = instr.arithmetic.set_flags;

	uint32_t sf = (rd.sf && rn.sf) << 31;
	uint32_t opc = (((uint32_t)neg << 1) | (uint32_t)set_flags) << 29;
	uint32_t base = 0x4 << 26; // 100

	uint32_t opi = 0x2 << 23; // 010 arithmetic
	uint32_t sh = (op2.shift_amount / 12) << 22;
	uint32_t imm12 = op2.literal.imm << 10;
	return sf | opc | base | opi | sh | imm12 | (rn.reg_num << 5) | rd.reg_num;
}

uint32_t arithmetic_encoder(Instr instr) {
	assert(instr.type == INSTR_ARITHMETIC);

	Operand op2 = instr.arithmetic.op2;
	if (op2.type == IMM || op2.type == SHIFTED_IMM) {
		return encode_imm_a(instr);
	} else if (op2.type == REG) {
		return encode_reg_a(instr);
	} else {
		printf("ERROR: Invalid operand type");
		exit(EXIT_FAILURE);
	}
}

// Single Data Transfer Instructions

uint32_t encode_ldrstr(Instr instr) {
	assert((instr.type) == INSTR_TRANSFER);

	Reg rt = instr.single_data_transfer.rt;
	Address address = instr.single_data_transfer.address;
	bool store = instr.single_data_transfer.store;

	uint32_t encoded = 0;
	encoded |= 0x3 << 27;			  // 11
	encoded |= (rt.sf ? 1 : 0) << 30; // SF
	encoded |= rt.reg_num & 0x1F;	  // Rt

	switch (address.type) {
	case UNSIGNED_OFFSET:
		encoded |= (store ? 0 : 1) << 22; // L
		encoded |= 0x5 << 29;			  // 101 (0 is where sf would be)
		encoded |= 1 << 24;				  // U = 1
		encoded |= ((address.imm_offset / (rt.sf ? 8 : 4)) & 0xFFF)
				   << 10;							   // imm12 = imm / 8 or imm / 4
		encoded |= (address.base.reg_num & 0x1F) << 5; // Xn
		break;
	case PRE_INDEXED:
		encoded |= (store ? 0 : 1) << 22;			   // L
		encoded |= 0x5 << 29;						   // 101 (same as uoff)
		encoded |= (address.imm_offset & 0x1FF) << 12; // simm9
		encoded |= 0x3 << 10;						   // I = 1
		encoded |= (address.base.reg_num & 0x1F) << 5; // Xn
		break;
	case POST_INDEXED:
		encoded |= (store ? 0 : 1) << 22;			   // L
		encoded |= 0x5 << 29;						   // 101
		encoded |= (address.imm_offset & 0x1FF) << 12; // simm9
		encoded |= 1 << 10;							   // I = 0
		encoded |= (address.base.reg_num & 0x1F) << 5; // Xn
		break;
	case REGISTER_OFFSET:
		encoded |= (store ? 0 : 1) << 22;					  // L
		encoded |= 0x5 << 29;								  // 101
		encoded |= 1 << 21;									  // 1 at msb of offset field
		encoded |= (address.reg_offset.reg_num & 0x1F) << 16; // Xm
		encoded |= 0x1A << 10;								  // 11010
		encoded |= (address.base.reg_num & 0x1F) << 5;		  // Xn
		break;
	case LITERAL: {
		Literal literal = address.literal;
		int64_t offset;
		switch (literal.type) {
		case LITERAL_LABEL:
			offset = (int64_t)symbol_table_get(literal.label) - pc_load();
			break;
		case LITERAL_INT:
			offset = (int64_t)literal.imm - pc_load();
			break;
		case LITERAL_IMM:
			offset = (int64_t)literal.imm;
			break;
		default:
			printf("Error: Unsupported literal type\n");
			exit(EXIT_FAILURE);
		}
		if ((offset & 0x3) != 0) {
			printf("Error: offset not 4-byte aligned\n");
			exit(EXIT_FAILURE);
		}
		if (offset < -0xFFFFF || offset > 0xFFFFF) {
			printf("Error: address not within +-1MB range\n");
			exit(EXIT_FAILURE);
		}
		uint32_t simm19 = (offset / 4) & 0x7FFFF;
		encoded |= simm19 << 5;
		break;
	}
	default:
		printf("Error: Unsupported addressing mode\n");
		exit(EXIT_FAILURE);
	}
	return encoded;
}

// Branching Instructions

uint32_t encode_branch(Instr instr) {
	assert(instr.type == INSTR_BRANCH);

	bool is_cond = instr.branch.is_cond;
	Operand op = instr.branch.op;
	Cond cond = instr.branch.cond;

	uint32_t encoded = 0;

	if (!is_cond) {
		switch (op.type) {
		case REG:
			encoded |= 0xD61F << 16;				 // 1101011000011111
			encoded |= (op.reg.reg_num & 0x1F) << 5; // Xn
			break;
		case IMM: { // assuming only literal label
			int64_t offset = (int64_t)symbol_table_get(op.literal.label) - pc_load();
			encoded |= 0x5 << 26;				 // 101
			encoded |= (offset / 4) & 0x3FFFFFF; // simm26
			break;
		}
		default:
			printf("Error: Invalid branch operand type\n");
			exit(EXIT_FAILURE);
		}
	} else {
		if (!(op.type == IMM &&
			  op.literal.type == LITERAL_LABEL)) { // spec assumes only these types in branch anyway
			printf("Error: Invalid branch operand/literal type\n");
			exit(EXIT_FAILURE);
		} else {
			int64_t offset = (int64_t)symbol_table_get(op.literal.label) - pc_load();
			encoded |= 0x15 << 26;					  // 10101
			encoded |= ((offset / 4) & 0x7FFFF) << 5; // simm19
			switch (cond) {
			case EQ:
				break; // 0000
			case NE:
				encoded |= 0x1;
				break; // 0001
			case GE:
				encoded |= 0xA;
				break; // 1010
			case LT:
				encoded |= 0xB;
				break; // 1011
			case GT:
				encoded |= 0xC;
				break; // 1100
			case LE:
				encoded |= 0xD;
				break; // 1101
			case AL:
				encoded |= 0xE;
				break; // 1110
			// NO: shouldn't happen since is_cond has been checked to be false
			default:
				printf("Error: Invalid branch condition\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	return encoded;
}

// Special Instructions / Directives

// The only directive we implement is .int
uint32_t encode_directive(Instr instr) { return (uint32_t)(instr.directive & 0xFFFFFFFF); }
