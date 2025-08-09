#include "encoder.h"
#include "instruction_assembler.h"
#include "ir.h"
#include <inttypes.h>
// add includes for all encoders here

typedef uint32_t (*encoding_func)(Instr);

uint32_t dummy(Instr i) { return 0; }

static encoding_func encoders[7] = {&arithmetic_encoder, &logic_encoder, &move_encoder,
									&multiply_encoder,	 &encode_branch, &encode_ldrstr,
									&encode_directive};
// arithmetic, logical, move, mul, branch, transfer, directive

uint32_t encode(Instr instruction) { return encoders[instruction.type](instruction); }
