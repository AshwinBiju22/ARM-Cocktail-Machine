#include "fde.h"
#include "bit_utils.h"
#include "emu_memory.h"
#include "emu_registers.h"
#include "instructions.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t fetch() {
	uint32_t instruction = mem_load32((uint32_t)pc_load());
	return instruction;
}

void decode_and_execute(uint32_t instr) {
	uint32_t op0 = extract_bits(instr, 25, 4);

	switch (op0) {
	case 0x8: // 1000 - DP (Immediate)
	case 0x9: // 1001 - DP (Immediate)
	{
		// DP (Immediate)
		uint32_t opi = extract_bits(instr, 23, 3);
		switch (opi) {
		case 0x2: // 010 - Arithmetic
		{
			bool sh = extract_bits(instr, 22, 1);
			uint32_t opc = extract_bits(instr, 29, 2);
			uint64_t rd = extract_bits(instr, 0, 5);
			uint64_t sf = extract_bits(instr, 31, 1);
			uint64_t rn = extract_bits(instr, 5, 5);
			uint64_t imm12 = extract_bits(instr, 10, 12);
			if (sh)
				imm12 = (imm12 << 12);
			if (opc == 0x0)
				add(rd, rn, imm12, sf);
			else if (opc == 0x1)
				adds(rd, rn, imm12, sf);
			else if (opc == 0x2)
				sub(rd, rn, imm12, sf);
			else if (opc == 0x3)
				subs(rd, rn, imm12, sf);
			else {
				printf("ERROR: Unknown opc for Arithmetic instruction\n");
				return;
			}
			break;
		}
		case 0x5: // 101 - Wide Mov
		{
			uint32_t opc = extract_bits(instr, 29, 2);
			if (opc == 0x0)
				execute_movn(instr);
			else if (opc == 0x2)
				execute_movz(instr);
			else if (opc == 0x3)
				execute_movk(instr);
			else {
				printf("ERROR: Unknown opc for Wide Mov instruction.\n");
				return;
			}
			break;
		}
		default: {
			printf("ERROR: Unknown opi for DP Immediate instruction.\n");
			return;
		}
		}
		pc_jump(1);
		break;
	}

	case 0x5: // 0101 - DP (Register)
	case 0xD: // 1101 - DP (Register)
	{
		uint64_t rd = extract_bits(instr, 0, 5);
		uint64_t rn = extract_bits(instr, 5, 5);
		uint64_t rm = extract_bits(instr, 16, 5);
		uint64_t sf = extract_bits(instr, 31, 1);
		uint64_t opc = extract_bits(instr, 29, 2);
		uint64_t operand = extract_bits(instr, 10, 6);
		uint32_t opr = extract_bits(instr, 21, 4);
		uint32_t M = extract_bits(instr, 28, 1);

		if (M == 0x0 && (opr & 0x9) == 0x8) {
			// Arithmetic
			uint32_t shift = extract_bits(instr, 22, 2);
			// uint32_t is_arithmetic = extract_bits(instr, 24, 1);
			uint64_t amount = operand;
			uint64_t rmv = register_load(rm, sf);
			rmv = shift == 0   ? logical_shift_left(rmv, amount, get_width(sf))
				  : shift == 1 ? logical_shift_right(rmv, amount, get_width(sf))
				  : shift == 2 ? arithmetic_shift_right(rmv, amount, get_width(sf))
							   : rmv;
			switch (opc) {
			case 0:
				add(rd, rn, rmv, sf);
				break;
			case 1:
				adds(rd, rn, rmv, sf);
				break;
			case 2:
				sub(rd, rn, rmv, sf);
				break;
			case 3:
				subs(rd, rn, rmv, sf);
				break;
			default:
				printf("ERROR: Unknown opc in Arithmetic.\n");
				return;
			}

		} else if (M == 0x0 && (opr & 0x8) == 0x0) {
			uint64_t shift = extract_bits(instr, 22, 2);
			uint64_t amount = operand;
			bool nflag = opr & 0x1;
			uint64_t rmv = register_load(rm, sf);
			uint64_t base = shift == 0	 ? logical_shift_left(rmv, amount, get_width(sf))
							: shift == 1 ? logical_shift_right(rmv, amount, get_width(sf))
							: shift == 2 ? arithmetic_shift_right(rmv, amount, get_width(sf))
										 : rotate_right(rmv, amount, get_width(sf));
			uint64_t op2 = nflag ? (~base & make_mask(0, get_width(sf))) : base;
			switch (opc) {
			case 0:
				execute_and(rd, rn, op2, sf);
				break; // AND or BIC
			case 1:
				execute_orr(rd, rn, op2, sf);
				break; // ORR or ORN
			case 2:
				execute_eor(rd, rn, op2, sf);
				break; // EOR or EON
			case 3:
				execute_ands(rd, rn, op2, sf);
				break; // ANDS or BICS
			default:
				printf("ERROR: Unknown opc in Logical.\n");
				return;
			}
		}

		else if (M == 0x1 && opr == 0x8) {
			// Multiply
			uint64_t ra = extract_bits(operand, 0, 5);
			uint64_t sub = extract_bits(operand, 5, 1);
			if (sub == 0)
				madd(rd, rn, rm, ra, sf);
			else
				msub(rd, rn, rm, ra, sf);
		} else {
			printf("ERROR: Unknown DP Register instruction.\n");
			return;
		}
		pc_jump(1);
		break;
	}

	case 0x4: // 0100 - Load/Store
	case 0x6: // 0110 - Load/Store
	case 0xC: // 1100 - Load/Store
	case 0xE: // 1110 - Load/Store
	{
		uint64_t rt = extract_bits(instr, 0, 5);
		uint64_t sf = extract_bits(instr, 30, 1);
		uint64_t M = extract_bits(instr, 31, 1);
		if (M) {
			uint64_t xn = extract_bits(instr, 5, 5);
			uint64_t L = extract_bits(instr, 22, 1);
			uint64_t U = extract_bits(instr, 24, 1);
			if (U) {
				uint64_t imm12 = extract_bits(instr, 10, 12);
				if (L)
					ldr_uoffset(rt, xn, imm12, sf);
				else
					str_uoffset(rt, xn, imm12, sf);
			} else {
				uint64_t offsetM = extract_bits(instr, 21, 1);
				if (offsetM) {
					uint64_t xm = extract_bits(instr, 16, 5);
					if (L)
						ldr_regoffset(rt, xn, xm, sf);
					else
						str_regoffset(rt, xn, xm, sf);
				} else {
					uint64_t simm9 = extract_bits(instr, 12, 9);
					uint64_t I = extract_bits(instr, 11, 1);
					if (I) {
						if (L)
							ldr_preindexed(rt, xn, simm9, sf);
						else
							str_preindexed(rt, xn, simm9, sf);
					} else {
						if (L)
							ldr_postindexed(rt, xn, simm9, sf);
						else
							str_postindexed(rt, xn, simm9, sf);
					}
				}
			}
		} else {
			uint64_t simm19 = extract_bits(instr, 5, 19);
			ldr_literal(rt, simm19, sf);
		}
		pc_jump(1);
		break;
	}
	case 0xA: // 1010 - Branch
	case 0xB: // 1011 - Branch
	{		  // Branch
		uint64_t branch_type = extract_bits(instr, 29, 3);
		if (branch_type == 0x0) {
			// Unconditional Branch
			uint64_t simm26 = extract_bits(instr, 0, 26);
			simm26 = sign_extend(simm26, 26);
			pc_jump(simm26);
			break;

		} else if (branch_type == 0x6) {
			// Register Branch
			uint64_t xn = extract_bits(instr, 5, 5);
			pc_jump_indirect(xn);
			break;

		} else {
			// Conditional Branch
			uint64_t simm19 = extract_bits(instr, 5, 19);
			simm19 = sign_extend(simm19, 19);
			uint64_t cond = extract_bits(instr, 0, 4);
			pc_jump_conditional(simm19, cond);
			break;
		}
		break;
	}
	default:
		// Unknown
		printf("ERROR: Unknown instruction.\n");
		break;
	}
}
