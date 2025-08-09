#include <stdbool.h>
#include <stdint.h>

#ifndef IR_H
#define IR_H

typedef enum { REG, IMM, SHIFTED_IMM, ADDR } Operand_Type;

typedef struct {
	bool sf;		  // true for x; false for w
	unsigned reg_num; // 0 - 30 for normal, 31 for zero
} Reg;

typedef enum { LITERAL_LABEL, LITERAL_INT, LITERAL_IMM } Literal_Type;

typedef struct {
	Literal_Type type;
	union {
		uint64_t imm; // cast signed to uint64_t
		char *label;  // resolve with symbol table;
	};
} Literal;

typedef enum {
	UNSIGNED_OFFSET, // for zero unsigned offset, use 0 for offset
	PRE_INDEXED,
	POST_INDEXED,
	REGISTER_OFFSET,
	LITERAL
} Addressing_Type;

typedef struct {
	Addressing_Type type;
	Reg base; // use zero register for load from literal
	union {
		// use for first 3 addressing types, use cast for simm
		uint64_t imm_offset;
		Reg reg_offset; // use with register offset
		Literal literal;
	};
} Address;

typedef enum { LSL, LSR, ASR, ROR } Shift_Type;

typedef struct {
	Operand_Type type;
	Shift_Type shift;
	unsigned shift_amount;
	union {
		Reg reg;
		Literal literal; // use for imm and labels
	};
} Operand;

typedef enum {
	INSTR_ARITHMETIC, // all artihmetic ops other than mult
	INSTR_LOGIC,	  // all logic ops
	INSTR_MOVE,
	INSTR_MUL,
	INSTR_BRANCH,
	INSTR_TRANSFER,
	INSTR_DIRECTIVE
} Instr_Type;

typedef enum {
	EQ,
	NE,
	GE,
	LT,
	GT,
	LE,
	AL,
	NO // use to say it is not a conditional branch
} Cond;

typedef enum { AND, OR, XOR } Logical_Type;

typedef struct {
	Instr_Type type;
	union {
		struct {
			bool set_flags;
			// true for adds, subs, cmp, cmn, negs
			bool neg;
			// true for sub(s), cmp, neg(s)
			Reg rd, rn;
			// use rd = 31 for cmp, cmn
			// use rn = 31 for neg(s)
			Operand op2;
		} arithmetic;
		struct {
			bool set_flags;
			// true for tst, ands, bics
			bool neg;
			// true for bic(s), eon, orn, mvn
			Logical_Type ltype; // use in encoding directly
			Reg rd, rn;
			// use rd = 31 for tst
			// use rn = 31 for mov, mvn
			Operand op2;
		} logical;
		struct {
			bool neg;			// use true for msub and mneg; false for madd and mul
			Reg rd, ra, rn, rm; // use ra = 31 for mul and mneg
		} multiply;
		struct {
			bool neg;		// true for movn
			bool with_keep; // true for movk
			// both false for movz
			Reg rd;
			Operand op;
		} move;
		struct {
			bool store; // true for str, false for ldr
			Reg rt;
			Address address;
		} single_data_transfer;
		struct {
			bool is_cond; // true for conditional
			Operand op;
			Cond cond; // use no or keep uninitialised
		} branch;
		uint64_t directive; // use for int directive
	};
} Instr;

#endif
