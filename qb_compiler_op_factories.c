/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Chung Leong <cleong@cal.berkeley.edu>                        |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

typedef struct qb_basic_op_factory				qb_basic_op_factory; 
typedef struct qb_float_op_factory				qb_float_op_factory; 
typedef struct qb_simple_op_factory				qb_simple_op_factory;
typedef struct qb_copy_op_factory				qb_copy_op_factory;
typedef struct qb_comparison_op_factory			qb_comparison_op_factory;
typedef struct qb_comparison_branch_op_factory	qb_comparison_branch_op_factory;
typedef struct qb_arithmetic_op_factory			qb_arithmetic_op_factory;
typedef struct qb_string_op_factory				qb_string_op_factory;
typedef struct qb_fcall_op_factory				qb_fcall_op_factory;
typedef struct qb_minmax_op_factory				qb_minmax_op_factory;
typedef struct qb_vector_op_factory				qb_vector_op_factory;
typedef struct qb_matrix_op_factory				qb_matrix_op_factory;
typedef struct qb_sampling_op_factory			qb_sampling_op_factory;

typedef qb_op * (ZEND_FASTCALL *qb_append_op_proc)(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result);

struct qb_basic_op_factory {
	qb_append_op_proc append;
	uint32_t coercion_flags;
	uint32_t result_flags;
	qb_opcode opcodes[10];
}; 

struct qb_float_op_factory {
	qb_append_op_proc append;
	uint32_t coercion_flags;
	uint32_t result_flags;
	qb_opcode opcodes[2];
}; 

struct qb_simple_op_factory {
	qb_append_op_proc append;
	uint32_t coercion_flags;
	uint32_t result_flags;
	qb_opcode opcode;
};

struct qb_copy_op_factory {
	qb_append_op_proc append;
	uint32_t coercion_flags;
	uint32_t result_flags;
	qb_opcode opcodes[10][10];
	qb_opcode vector_opcodes[3][2];
};

struct qb_comparison_op_factory {
	qb_append_op_proc append;
	uint32_t coercion_flags;
	uint32_t result_flags;
	qb_opcode opcodes[10];
	qb_comparison_branch_op_factory *branch_factory;
	qb_basic_op_factory *set_factory;
};

struct qb_comparison_branch_op_factory {
	qb_append_op_proc append;
	uint32_t coercion_flags;
	uint32_t result_flags;
	qb_opcode opcodes[10];
	qb_comparison_branch_op_factory *inverse_factory;
};

struct qb_arithmetic_op_factory {
	qb_append_op_proc append;
	uint32_t coercion_flags;
	uint32_t result_flags;
	qb_opcode regular_opcodes[10];
	qb_opcode vector_opcodes[3][2];
};

struct qb_string_op_factory {
	qb_append_op_proc append;
	uint32_t coercion_flags;
	uint32_t result_flags;
	qb_opcode opcodes[10];
	qb_opcode multidim_opcodes[10];
	qb_opcode text_opcode;
}; 

struct qb_fcall_op_factory {
	qb_append_op_proc append;
	uint32_t coercion_flags;
	uint32_t result_flags;
	qb_opcode opcode;
};

struct qb_minmax_op_factory {
	qb_append_op_proc append;
	uint32_t coercion_flags;
	uint32_t result_flags;
	qb_opcode opcodes_pair[10];
	qb_opcode opcodes_list[10];
};

struct qb_vector_op_factory {
	qb_append_op_proc append;
	uint32_t coercion_flags;
	uint32_t result_flags;
	qb_opcode opcodes_any_size[2];
	qb_opcode opcodes_fixed_size[3][2];
};

struct qb_matrix_op_factory {
	qb_append_op_proc append;
	uint32_t coercion_flags;
	uint32_t result_flags;
	qb_opcode opcodes_any_size[2];
	qb_opcode opcodes_fixed_size[3][2];
	qb_opcode opcode_3x3_padded;
};

struct qb_sampling_op_factory {
	qb_append_op_proc append;
	uint32_t coercion_flags;
	uint32_t result_flags;
	qb_opcode opcodes[2][2];
}; 

static zend_always_inline uint32_t qb_get_coercion_flags(qb_compiler_context *cxt, void *factory) {
	qb_basic_op_factory *f = factory;
	return f->coercion_flags;
}

static zend_always_inline uint32_t qb_get_result_flags(qb_compiler_context *cxt, void *factory) {
	qb_basic_op_factory *f = factory;
	return f->result_flags;
}

static uint32_t ZEND_FASTCALL qb_get_matrix_row_count(qb_compiler_context *cxt, qb_address *address) {
	if(address->dimension_count >= 2) {
		qb_address *dimension_address = address->dimension_addresses[address->dimension_count - 2];
		return VALUE(U32, dimension_address);
	}
	return 1;
}

static uint32_t ZEND_FASTCALL qb_get_matrix_column_count(qb_compiler_context *cxt, qb_address *address) {
	if(address->dimension_count >= 1) {
		qb_address *dimension_address = address->dimension_addresses[address->dimension_count - 1];
		return VALUE(U32, dimension_address);
	}
	return 1;
}

static zend_always_inline uint32_t qb_get_vector_width(qb_compiler_context *cxt, qb_address *address) {
	return qb_get_matrix_column_count(cxt, address);
}

static void ZEND_FASTCALL qb_execute_op(qb_compiler_context *cxt, qb_op *op);

static qb_op * ZEND_FASTCALL qb_create_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_basic_op_factory *f = factory;
	uint32_t initial_op_count = cxt->op_count;
	uint32_t i;
	qb_op *qop = f->append(cxt, factory, operands, operand_count, result);

#if ZEND_DEBUG
	if(cxt->stage != QB_STAGE_OPCODE_TRANSLATION && cxt->stage != QB_STAGE_VARIABLE_INITIALIZATION) {
		qb_abort("Creating opcode at the wrong stage");
	}
#endif

	for(i = 0; i < operand_count; i++) {
		qb_operand *operand = &operands[i];
		if(operand->type == QB_OPERAND_ADDRESS) {
			qb_unlock_address(cxt, operand->address);
		}
	}
	if(result && result->type == QB_OPERAND_ADDRESS) {
		if(result->address->flags & QB_ADDRESS_CONSTANT) {
			// evalulate the expression at compile-time
			qb_execute_op(cxt, qop);

			// roll back the op counter
			cxt->op_count = initial_op_count;

			// make it a NOP
			qop->opcode = QB_NOP;
		} else {
			// mark result address as writable
			qb_mark_as_writable(cxt, result->address);
		}
	}
	return qop;
}

static qb_op * ZEND_FASTCALL qb_create_nop(qb_compiler_context *cxt) {
	qb_op *qop = qb_append_op(cxt, QB_NOP, 0);
	return qop;
}

static qb_op * ZEND_FASTCALL qb_create_nullary_op(qb_compiler_context *cxt, void *factory, qb_address *result_address) {
	qb_operand result;
	result.type = (result_address) ? QB_OPERAND_ADDRESS : QB_OPERAND_NONE;
	result.address = result_address;
	return qb_create_op(cxt, factory, NULL, 0, &result);
}

static qb_op * ZEND_FASTCALL qb_create_unary_op(qb_compiler_context *cxt, void *factory, qb_address *op_address, qb_address *result_address) {
	qb_operand operands[1], result;
	operands[0].type = QB_OPERAND_ADDRESS;
	operands[0].address = op_address;
	result.type = QB_OPERAND_ADDRESS;
	result.address = result_address;
	return qb_create_op(cxt, factory, operands, 1, &result);
}

static qb_op * ZEND_FASTCALL qb_create_binary_op(qb_compiler_context *cxt, void *factory, qb_address *op1_address, qb_address *op2_address, qb_address *result_address) {
	qb_operand operands[2], result;
	operands[0].type = QB_OPERAND_ADDRESS;
	operands[0].address = op1_address;
	operands[1].type = QB_OPERAND_ADDRESS;
	operands[1].address = op2_address;
	result.type = QB_OPERAND_ADDRESS;
	result.address = result_address;
	return qb_create_op(cxt, factory, operands, 2, &result);
}

static qb_op * ZEND_FASTCALL qb_create_ternary_op(qb_compiler_context *cxt, void *factory, qb_address *op1_address, qb_address *op2_address, qb_address *op3_address, qb_address *result_address) {
	qb_operand operands[3], result;
	operands[0].type = QB_OPERAND_ADDRESS;
	operands[0].address = op1_address;
	operands[1].type = QB_OPERAND_ADDRESS;
	operands[1].address = op2_address;
	operands[2].type = QB_OPERAND_ADDRESS;
	operands[2].address = op3_address;
	result.type = QB_OPERAND_ADDRESS;
	result.address = result_address;
	return qb_create_op(cxt, factory, operands, 3, &result);
}

static qb_op * ZEND_FASTCALL qb_create_jump_op(qb_compiler_context *cxt, void *factory, uint32_t jump_target_index) {
	qb_operand operands[1];
	operands[0].type = QB_OPERAND_JUMP_TARGET;
	operands[0].jump_target_index = jump_target_index;
	return qb_create_op(cxt, factory, operands, 1, NULL);
}

static qb_op * ZEND_FASTCALL qb_create_branch_op(qb_compiler_context *cxt, void *factory, uint32_t jump_target_index1, uint32_t jump_target_index2, qb_address *condition) {
	qb_operand operands[3];
	operands[0].type = QB_OPERAND_JUMP_TARGET;
	operands[0].jump_target_index = jump_target_index1;
	operands[1].type = QB_OPERAND_JUMP_TARGET;
	operands[1].jump_target_index = jump_target_index2;
	operands[2].type = QB_OPERAND_ADDRESS;
	operands[2].address = condition;
	return qb_create_op(cxt, factory, operands, 3, NULL);
}

static qb_op * ZEND_FASTCALL qb_create_comparison_branch_op(qb_compiler_context *cxt, void *factory, uint32_t jump_target_index1, uint32_t jump_target_index2, qb_address *op1_address, qb_address *op2_address) {
	qb_operand operands[4];
	operands[0].type = QB_OPERAND_JUMP_TARGET;
	operands[0].jump_target_index = jump_target_index1;
	operands[1].type = QB_OPERAND_JUMP_TARGET;
	operands[1].jump_target_index = jump_target_index2;
	operands[2].type = QB_OPERAND_ADDRESS;
	operands[2].address = op1_address;
	operands[3].type = QB_OPERAND_ADDRESS;
	operands[3].address = op2_address;
	return qb_create_op(cxt, factory, operands, 4, NULL);
}

static uint32_t ZEND_FASTCALL qb_get_minimum_width(qb_compiler_context *cxt, qb_operand *operand) {
	qb_address *address = operand->address;
	uint32_t i, width = 0;
	for(i = 0; i < address->dimension_count; i++) {
		qb_address *array_size_address = address->array_size_addresses[i];
		if(array_size_address->flags & QB_ADDRESS_CONSTANT) {
			width = VALUE(U32, array_size_address);
			break;
		}
	}
	return width;
}

static int32_t ZEND_FASTCALL qb_vectorize_op(qb_compiler_context *cxt, qb_op *qop, qb_opcode opcodes[][2]) {
	qb_address *address = qop->operands[0].address;
	if(address->type >= QB_TYPE_F32) {
		uint32_t i, j;
		int32_t divisible[5] = { FALSE, TRUE, TRUE, TRUE, TRUE };
		for(i = 0; i < qop->operand_count; i++) {
			uint32_t width = qb_get_minimum_width(cxt, &qop->operands[i]);
			if(width) {
				divisible[2] = divisible[2] && !(width & 0x0001);
				divisible[3] = divisible[3] && !(width % 3);
				divisible[4] = divisible[4] && !(width & 0x0003);
			} else {
				return FALSE;
			}
		}
		for(j = 4; j >= 2; j--) {
			if(divisible[j]) {
				uint32_t single_opcode = opcodes[j - 2][QB_TYPE_F64 - address->type];
				int32_t multiple_operands = FALSE;
				for(i = 0; i < qop->operand_count; i++) {
					qb_address *address = qop->operands[i].address;
					if(IS_VARIABLE_LENGTH_ARRAY(address) || ARRAY_SIZE(address) > j) {
						multiple_operands = TRUE;
						break;
					}
				}
				qop->opcode = (multiple_operands) ? single_opcode + 1 : single_opcode;
				qop->flags = qb_get_op_flags(cxt, qop->opcode);
				return TRUE;
			}
		}
	}
	return FALSE;
}

static qb_op * ZEND_FASTCALL qb_append_copy_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_copy_op_factory *f = factory;
	qb_address *src_address = operands[0].address;
	qb_address *dst_address = result->address;	
	qb_opcode opcode = f->opcodes[QB_TYPE_F64 - src_address->type][QB_TYPE_F64 - dst_address->type];
	qb_op *qop = qb_append_op(cxt, opcode, 2);
	qop->operands[0] = operands[0];
	qop->operands[1] = *result;
	if(src_address->type == dst_address->type) {
		// vectorized instructions are available only for copying between variables of the same type
		qb_vectorize_op(cxt, qop, f->vector_opcodes);
	}
	return qop;
}

static qb_copy_op_factory factory_copy = {
	qb_append_copy_op,
	QB_COERCE_TO_LVALUE_TYPE,
	0,
	{
		{	QB_MOV_F64_F64,	QB_MOV_F64_F32,	QB_MOV_F64_U64,	QB_MOV_F64_S64,	QB_MOV_F64_U32,	QB_MOV_F64_S32,	QB_MOV_F64_U16,	QB_MOV_F64_S16,	QB_MOV_F64_U08,	QB_MOV_F64_S08,	},
		{	QB_MOV_F32_F64,	QB_MOV_F32_F32,	QB_MOV_F32_U64,	QB_MOV_F32_S64,	QB_MOV_F32_U32,	QB_MOV_F32_S32,	QB_MOV_F32_U16,	QB_MOV_F32_S16,	QB_MOV_F32_U08,	QB_MOV_F32_S08,	},
		{	QB_MOV_U64_F64,	QB_MOV_U64_F32,	QB_MOV_I64_I64,	QB_MOV_I64_I64,	QB_MOV_I64_I32,	QB_MOV_I64_I32,	QB_MOV_I64_I16,	QB_MOV_I64_I16,	QB_MOV_I64_I08,	QB_MOV_I64_I08,	},
		{	QB_MOV_S64_F64,	QB_MOV_S64_F32,	QB_MOV_I64_I64,	QB_MOV_I64_I64,	QB_MOV_I64_I32,	QB_MOV_I64_I32,	QB_MOV_I64_I16,	QB_MOV_I64_I16,	QB_MOV_I64_I08,	QB_MOV_I64_I08,	},
		{	QB_MOV_U32_F64,	QB_MOV_U32_F32,	QB_MOV_U32_I64,	QB_MOV_U32_I64,	QB_MOV_I32_I32,	QB_MOV_I32_I32,	QB_MOV_I32_I16,	QB_MOV_I32_I16,	QB_MOV_I32_I08,	QB_MOV_I32_I08,	},
		{	QB_MOV_S32_F64,	QB_MOV_S32_F32,	QB_MOV_S32_I64,	QB_MOV_S32_I64,	QB_MOV_I32_I32,	QB_MOV_I32_I32,	QB_MOV_I32_I16,	QB_MOV_I32_I16,	QB_MOV_I32_I08,	QB_MOV_I32_I08,	},
		{	QB_MOV_U16_F64,	QB_MOV_U16_F32,	QB_MOV_U16_I64,	QB_MOV_U16_I64,	QB_MOV_U16_I32,	QB_MOV_U16_I32,	QB_MOV_I16_I16,	QB_MOV_I16_I16,	QB_MOV_I16_I08,	QB_MOV_I16_I08,	},
		{	QB_MOV_S16_F64,	QB_MOV_S16_F32,	QB_MOV_S16_I64,	QB_MOV_S16_I64,	QB_MOV_S16_I32,	QB_MOV_S16_I32,	QB_MOV_I16_I16,	QB_MOV_I16_I16,	QB_MOV_I16_I08,	QB_MOV_I16_I08,	},
		{	QB_MOV_S08_F64,	QB_MOV_S08_F32,	QB_MOV_S08_I64,	QB_MOV_S08_I64,	QB_MOV_S08_I32,	QB_MOV_S08_I32,	QB_MOV_S08_I16,	QB_MOV_S08_I16,	QB_MOV_I08_I08,	QB_MOV_I08_I08,	},
		{	QB_MOV_U08_F64,	QB_MOV_U08_F32,	QB_MOV_U08_I64,	QB_MOV_U08_I64,	QB_MOV_U08_I32,	QB_MOV_U08_I32,	QB_MOV_U08_I16,	QB_MOV_S08_I16,	QB_MOV_I08_I08,	QB_MOV_I08_I08,	},
	},
	{
		{	QB_MOV_2X_F64_F64,	QB_MOV_2X_F32_F32,	},
		{	QB_MOV_3X_F64_F64,	QB_MOV_3X_F32_F32,	},
		{	QB_MOV_4X_F64_F64,	QB_MOV_4X_F32_F32,	},
	},
};

static qb_op * ZEND_FASTCALL qb_append_array_insert(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_basic_op_factory *f = factory;
	qb_address *address = operands[0].address;
	qb_address *offset_address = operands[1].address;
	qb_op *qop = NULL;
	qb_opcode opcode = f->opcodes[QB_TYPE_F64 - address->type];
	qop = qb_append_op(cxt, opcode, 3);
	qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[0].address = address;
	qop->operands[1].type = QB_OPERAND_ADDRESS_VAR;
	qop->operands[1].address = offset_address;
	qop->operands[2] = *result;
	return qop;
}

static qb_basic_op_factory factory_array_insert = {
	qb_append_array_insert,
	0,
	0,
	{	QB_AINS_F64_U32_F64,	QB_AINS_F32_U32_F32,	QB_AINS_I64_U32_I64,	QB_AINS_I64_U32_I64,	QB_AINS_I32_U32_I32,	QB_AINS_I32_U32_I32,	QB_AINS_I16_U32_I16,	QB_AINS_I16_U32_I16,	QB_AINS_I08_U32_I08,	QB_AINS_I08_U32_I08,	},
};

static qb_op * ZEND_FASTCALL qb_append_incdec_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_arithmetic_op_factory *f = factory;
	qb_address *address = result->address;
	qb_op *qop = NULL;
	qb_opcode opcode = f->regular_opcodes[QB_TYPE_F64 - address->type];
	qop = qb_append_op(cxt, opcode, 1);
	qop->operands[0] = *result;
	qb_vectorize_op(cxt, qop, f->vector_opcodes);
	return qop;
}

static qb_arithmetic_op_factory factory_increment = { 
	qb_append_incdec_op,
	0,
	0,
	{	QB_INC_F64,	QB_INC_F32,	QB_INC_I64,	QB_INC_I64,	QB_INC_I32,	QB_INC_I32,	QB_INC_I16,	QB_INC_I16,	QB_INC_I08,	QB_INC_I08,	},
	{
		{	QB_INC_2X_F64,	QB_INC_2X_F32,	},
		{	QB_INC_3X_F64,	QB_INC_3X_F32,	},
		{	QB_INC_4X_F64,	QB_INC_4X_F32,	},
	},
};

static qb_arithmetic_op_factory factory_decrement = { 
	qb_append_incdec_op,
	0,
	0,
	{	QB_DEC_F64,	QB_DEC_F32,	QB_DEC_I64,	QB_DEC_I64,	QB_DEC_I32,	QB_DEC_I32,	QB_DEC_I16,	QB_DEC_I16,	QB_DEC_I08,	QB_DEC_I08,	},
	{
		{	QB_DEC_2X_F64,	QB_DEC_2X_F32,	},
		{	QB_DEC_3X_F64,	QB_DEC_3X_F32,	},
		{	QB_DEC_4X_F64,	QB_DEC_4X_F32,	},
	},
};

static qb_op * ZEND_FASTCALL qb_append_unary_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_basic_op_factory *f = factory;
	qb_address *address = operands[0].address;
	qb_opcode opcode = f->opcodes[QB_TYPE_F64 - address->type];
	qb_op *qop = qb_append_op(cxt, opcode, 2);
	qop->operands[0] = operands[0];
	qop->operands[1] = *result;
	return qop;
}

static qb_basic_op_factory factory_bitwise_not = {
	qb_append_unary_op,
	QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	0,	0,	QB_BW_NOT_I64_I64,	QB_BW_NOT_I64_I64,	QB_BW_NOT_I32_I32,	QB_BW_NOT_I32_I32,	QB_BW_NOT_I16_I16,	QB_BW_NOT_I16_I16,	QB_BW_NOT_I08_I08,	QB_BW_NOT_I08_I08,	}
};

static qb_basic_op_factory factory_logical_not = {
	qb_append_unary_op,
	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	0,	0,	0,	0,	QB_NOT_I32_I32,	QB_NOT_I32_I32,	0,	0,	0,	0,	}
};

static qb_basic_op_factory factory_isset = { 
	qb_append_unary_op,
	0,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_ISSET_F64_I32_ELV ,	QB_ISSET_F32_I32_ELV,	QB_ISSET_I64_I32_ELV,	QB_ISSET_I64_I32_ELV,	QB_ISSET_I32_I32_ELV,	QB_ISSET_I32_I32_ELV,	QB_ISSET_I16_I32_ELV,	QB_ISSET_I16_I32_ELV,	QB_ISSET_I08_I32_ELV,	QB_ISSET_I08_I32_ELV,	},
};


static qb_op * ZEND_FASTCALL qb_append_nullary_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_basic_op_factory *f = factory;
	qb_address *address = result->address;
	qb_opcode opcode = f->opcodes[QB_TYPE_F64 - address->type];
	qb_op *qop = qb_append_op(cxt, opcode, 1);
	qop->operands[0] = *result;
	return qop;
}

static qb_basic_op_factory factory_unset = { 
	qb_append_nullary_op,
	0,
	0,
	{	QB_UNSET_F64 ,	QB_UNSET_F32,	QB_UNSET_I64,	QB_UNSET_I64,	QB_UNSET_I32,	QB_UNSET_I32,	QB_UNSET_I16,	QB_UNSET_I16,	QB_UNSET_I08,	QB_UNSET_I08,	},
};

static qb_op * ZEND_FASTCALL qb_append_binary_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_basic_op_factory *f = factory;
	qb_address *address1 = operands[0].address;
	qb_opcode opcode = f->opcodes[QB_TYPE_F64 - address1->type];
	qb_op *qop = qb_append_op(cxt, opcode, 3);
	qop->operands[0] = operands[0];
	qop->operands[1] = operands[1];
	qop->operands[2] = *result;
	return qop;
}

static qb_basic_op_factory factory_shift_left = { 
	qb_append_binary_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	0,	0,	QB_SHL_U64_U64_U64,	QB_SHL_S64_S64_S64,	QB_SHL_U32_U32_U32,	QB_SHL_S32_S32_S32,	QB_SHL_U16_U16_U16,	QB_SHL_S16_S16_S16,	QB_SHL_U08_U08_U08,	QB_SHL_S08_S08_S08,	},
};

static qb_basic_op_factory factory_shift_right = { 
	qb_append_binary_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	0,	0,	QB_SHR_U64_U64_U64,	QB_SHR_S64_S64_S64,	QB_SHR_U32_U32_U32,	QB_SHR_S32_S32_S32,	QB_SHR_U16_U16_U16,	QB_SHR_S16_S16_S16,	QB_SHR_U08_U08_U08,	QB_SHR_S08_S08_S08,	},
};

static qb_basic_op_factory factory_bitwise_or = { 
	qb_append_binary_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	0,	0,	QB_BW_OR_I64_I64_I64,	QB_BW_OR_I64_I64_I64,	QB_BW_OR_I32_I32_I32,	QB_BW_OR_I32_I32_I32,	QB_BW_OR_I16_I16_I16,	QB_BW_OR_I16_I16_I16,	QB_BW_OR_I08_I08_I08,	QB_BW_OR_I08_I08_I08,	},
};

static qb_basic_op_factory factory_bitwise_and = { 
	qb_append_binary_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	0,	0,	QB_BW_AND_I64_I64_I64,	QB_BW_AND_I64_I64_I64,	QB_BW_AND_I32_I32_I32,	QB_BW_AND_I32_I32_I32,	QB_BW_AND_I16_I16_I16,	QB_BW_AND_I16_I16_I16,	QB_BW_AND_I08_I08_I08,	QB_BW_AND_I08_I08_I08,	},
};

static qb_basic_op_factory factory_bitwise_xor = {
	qb_append_binary_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE | QB_COERCE_TO_INTEGER,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	0,	0,	QB_BW_XOR_I64_I64_I64,	QB_BW_XOR_I64_I64_I64,	QB_BW_XOR_I32_I32_I32,	QB_BW_XOR_I32_I32_I32,	QB_BW_XOR_I16_I16_I16,	QB_BW_XOR_I16_I16_I16,	QB_BW_XOR_I08_I08_I08,	QB_BW_XOR_I08_I08_I08,	},
};

static qb_basic_op_factory factory_logical_and = {
	qb_append_binary_op,
	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	0,	0,	0,	0,	QB_AND_I32_I32_I32,	QB_AND_I32_I32_I32,	0,	0,	0,	0,	},
};

static qb_basic_op_factory factory_logical_or = {
	qb_append_binary_op,
	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	0,	0,	0,	0,	QB_OR_I32_I32_I32,	QB_OR_I32_I32_I32,	0,	0,	0,	0,	},
};

static qb_basic_op_factory factory_logical_xor = {
	qb_append_binary_op,
	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	0,	0,	0,	0,	QB_XOR_I32_I32_I32,	QB_XOR_I32_I32_I32,	0,	0,	0,	0,	},
};

static qb_op * ZEND_FASTCALL qb_append_binary_arithmetic_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_arithmetic_op_factory *f = factory;
	qb_address *address1 = operands[0].address;
	qb_opcode opcode = f->regular_opcodes[QB_TYPE_F64 - address1->type];
	qb_op *qop = qb_append_op(cxt, opcode, 3);
	qop->operands[0] = operands[0];
	qop->operands[1] = operands[1];
	qop->operands[2] = *result;
	qb_vectorize_op(cxt, qop, f->vector_opcodes);
	return qop;
}

static qb_arithmetic_op_factory factory_add = { 
	qb_append_binary_arithmetic_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_ADD_F64_F64_F64,	QB_ADD_F32_F32_F32,	QB_ADD_I64_I64_I64,	QB_ADD_I64_I64_I64,	QB_ADD_I32_I32_I32,	QB_ADD_I32_I32_I32,	QB_ADD_I16_I16_I16,	QB_ADD_I16_I16_I16,	QB_ADD_I08_I08_I08,	QB_ADD_I08_I08_I08,	},
	{
		{	QB_ADD_2X_F64_F64_F64,	QB_ADD_2X_F32_F32_F32,	},
		{	QB_ADD_3X_F64_F64_F64,	QB_ADD_3X_F32_F32_F32,	},
		{	QB_ADD_4X_F64_F64_F64,	QB_ADD_4X_F32_F32_F32,	},
	},
};

static qb_arithmetic_op_factory factory_subtract = { 
	qb_append_binary_arithmetic_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_SUB_F64_F64_F64,	QB_SUB_F32_F32_F32,	QB_SUB_I64_I64_I64,	QB_SUB_I64_I64_I64,	QB_SUB_I32_I32_I32,	QB_SUB_I32_I32_I32,	QB_SUB_I16_I16_I16,	QB_SUB_I16_I16_I16,	QB_SUB_I08_I08_I08,	QB_SUB_I08_I08_I08,	},
	{
		{	QB_SUB_2X_F64_F64_F64,	QB_SUB_2X_F32_F32_F32,	},
		{	QB_SUB_3X_F64_F64_F64,	QB_SUB_3X_F32_F32_F32,	},
		{	QB_SUB_4X_F64_F64_F64,	QB_SUB_4X_F32_F32_F32,	},
	},
};

static qb_arithmetic_op_factory factory_multiply = { 
	qb_append_binary_arithmetic_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_MUL_F64_F64_F64,	QB_MUL_F32_F32_F32,	QB_MUL_U64_U64_U64,	QB_MUL_U64_U64_U64,	QB_MUL_U32_U32_U32,	QB_MUL_U32_U32_U32,	QB_MUL_S16_S16_S16,	QB_MUL_S16_S16_S16,	QB_MUL_S08_S08_S08,	QB_MUL_S08_S08_S08,	},
	{
		{	QB_MUL_2X_F64_F64_F64,	QB_MUL_2X_F32_F32_F32,	},
		{	QB_MUL_3X_F64_F64_F64,	QB_MUL_3X_F32_F32_F32,	},
		{	QB_MUL_4X_F64_F64_F64,	QB_MUL_4X_F32_F32_F32,	},
	},
};

static qb_arithmetic_op_factory factory_divide = { 
	qb_append_binary_arithmetic_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_DIV_F64_F64_F64,	QB_DIV_F32_F32_F32,	QB_DIV_U64_U64_U64,	QB_DIV_S64_S64_S64,	QB_DIV_U32_U32_U32,	QB_DIV_S32_S32_S32,	QB_DIV_U16_U16_U16,	QB_DIV_S16_S16_S16,	QB_DIV_U08_U08_U08,	QB_DIV_S08_S08_S08,	},
	{
		{	QB_DIV_2X_F64_F64_F64,	QB_DIV_2X_F32_F32_F32,	},
		{	QB_DIV_3X_F64_F64_F64,	QB_DIV_3X_F32_F32_F32,	},
		{	QB_DIV_4X_F64_F64_F64,	QB_DIV_4X_F32_F32_F32,	},
	},
};

static qb_arithmetic_op_factory factory_modulo = { 
	qb_append_binary_arithmetic_op,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_MOD_F64_F64_F64,	QB_MOD_F32_F32_F32,	QB_MOD_U64_U64_U64,	QB_MOD_S64_S64_S64,	QB_MOD_U32_U32_U32,	QB_MOD_S32_S32_S32,	QB_MOD_U16_U16_U16,	QB_MOD_S16_S16_S16,	QB_MOD_U08_U08_U08,	QB_MOD_S08_S08_S08,	},
	{
		{	QB_MOD_2X_F64_F64_F64,	QB_MOD_2X_F32_F32_F32,	},
		{	QB_MOD_3X_F64_F64_F64,	QB_MOD_3X_F32_F32_F32,	},
		{	QB_MOD_4X_F64_F64_F64,	QB_MOD_4X_F32_F32_F32,	},
	},
};

static qb_float_op_factory factory_floor_modulo = { 
	qb_append_binary_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_LVALUE_TYPE,
	QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_MOD_FLR_F64_F64_F64,	QB_MOD_FLR_F32_F32_F32,	},
};

static qb_basic_op_factory factory_boolean = {
	qb_append_unary_op,
	0,
	QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_BOOL_F64_I32,	QB_BOOL_F32_I32,	QB_BOOL_I64_I32,	QB_BOOL_I64_I32,	QB_BOOL_I32_I32,	QB_BOOL_I32_I32,	QB_BOOL_I16_I32,	QB_BOOL_I16_I32,	QB_BOOL_I08_I32,	QB_BOOL_I08_I32,	},
};

static qb_comparison_branch_op_factory factory_branch_on_equal;
static qb_comparison_branch_op_factory factory_branch_on_not_equal;
static qb_comparison_branch_op_factory factory_branch_on_less_equal;
static qb_comparison_branch_op_factory factory_branch_on_less_than;
static qb_comparison_branch_op_factory factory_branch_on_greater_equal;
static qb_comparison_branch_op_factory factory_branch_on_greater_than;

static qb_op * ZEND_FASTCALL qb_append_comparison_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_comparison_op_factory *f = factory;
	if(IS_SCALAR(result->address)) {
		return qb_append_binary_op(cxt, factory, operands, operand_count, result);
	} else {
		return f->set_factory->append(cxt, f->set_factory, operands, operand_count, result);
	}
} 

static qb_basic_op_factory factory_set_equal = {
	qb_append_binary_op,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_EQ_SET_F64_F64_I32,	QB_EQ_SET_F32_F32_I32,	QB_EQ_SET_I64_I64_I32,	QB_EQ_SET_I64_I64_I32,	QB_EQ_SET_I32_I32_I32,	QB_EQ_SET_I32_I32_I32,	QB_EQ_SET_I16_I16_I32,	QB_EQ_SET_I16_I16_I32,	QB_EQ_SET_I08_I08_I32,	QB_EQ_SET_I08_I08_I32,	},
};

static qb_basic_op_factory factory_set_not_equal = {
	qb_append_binary_op,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_NE_SET_F64_F64_I32,	QB_NE_SET_F32_F32_I32,	QB_NE_SET_I64_I64_I32,	QB_NE_SET_I64_I64_I32,	QB_NE_SET_I32_I32_I32,	QB_NE_SET_I32_I32_I32,	QB_NE_SET_I16_I16_I32,	QB_NE_SET_I16_I16_I32,	QB_NE_SET_I08_I08_I32,	QB_NE_SET_I08_I08_I32,	},
};

static qb_basic_op_factory factory_set_less_than = {
	qb_append_binary_op,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_LT_SET_F64_F64_I32,	QB_LT_SET_F32_F32_I32,	QB_LT_SET_U64_U64_I32,	QB_LT_SET_S64_S64_I32,	QB_LT_SET_U32_U32_I32,	QB_LT_SET_S32_S32_I32,	QB_LT_SET_U16_U16_I32,	QB_LT_SET_S16_S16_I32,	QB_LT_SET_U08_U08_I32,	QB_LT_SET_S08_S08_I32,	},
};

static qb_basic_op_factory factory_set_less_equal = {
	qb_append_binary_op,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_LE_SET_F64_F64_I32,	QB_LE_SET_F32_F32_I32,	QB_LE_SET_U64_U64_I32,	QB_LE_SET_S64_S64_I32,	QB_LE_SET_U32_U32_I32,	QB_LE_SET_S32_S32_I32,	QB_LE_SET_U16_U16_I32,	QB_LE_SET_S16_S16_I32,	QB_LE_SET_U08_U08_I32,	QB_LE_SET_S08_S08_I32,	},
};

static qb_comparison_op_factory factory_equal = {
	qb_append_comparison_op,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_EQ_F64_F64_I32,	QB_EQ_F32_F32_I32,	QB_EQ_I64_I64_I32,	QB_EQ_I64_I64_I32,	QB_EQ_I32_I32_I32,	QB_EQ_I32_I32_I32,	QB_EQ_I16_I16_I32,	QB_EQ_I16_I16_I32,	QB_EQ_I08_I08_I32,	QB_EQ_I08_I08_I32,	},
	&factory_branch_on_equal,
	&factory_set_equal,
};

static qb_comparison_op_factory factory_not_equal = { 
	qb_append_comparison_op,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_NE_F64_F64_I32,	QB_NE_F32_F32_I32,	QB_NE_I64_I64_I32,	QB_NE_I64_I64_I32,	QB_NE_I32_I32_I32,	QB_NE_I32_I32_I32,	QB_NE_I16_I16_I32,	QB_NE_I16_I16_I32,	QB_NE_I08_I08_I32,	QB_NE_I08_I08_I32,	},
	&factory_branch_on_not_equal,
	&factory_set_not_equal,
};

static qb_comparison_op_factory factory_less_than = {
	qb_append_comparison_op,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_LT_F64_F64_I32,	QB_LT_F32_F32_I32,	QB_LT_U64_U64_I32,	QB_LT_S64_S64_I32,	QB_LT_U32_U32_I32,	QB_LT_S32_S32_I32,	QB_LT_U16_U16_I32,	QB_LT_S16_S16_I32,	QB_LT_U08_U08_I32,	QB_LT_S08_S08_I32,	},
	&factory_branch_on_less_than,
	&factory_set_less_than,
};

static qb_comparison_op_factory factory_less_equal = {
	qb_append_comparison_op,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_LE_F64_F64_I32,	QB_LE_F32_F32_I32,	QB_LE_U64_U64_I32,	QB_LE_S64_S64_I32,	QB_LE_U32_U32_I32,	QB_LE_S32_S32_I32,	QB_LE_U16_U16_I32,	QB_LE_S16_S16_I32,	QB_LE_U08_U08_I32,	QB_LE_S08_S08_I32,	},
	&factory_branch_on_less_equal,
	&factory_set_less_equal,
};

static qb_op *ZEND_FASTCALL qb_append_binary_branch_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_basic_op_factory *f = factory;
	uint32_t qop_index = cxt->op_count;
	uint32_t jump_target1 = operands[0].jump_target_index;
	uint32_t jump_target2 = operands[1].jump_target_index;
	qb_address *address1 = operands[2].address;
	qb_opcode opcode = f->opcodes[QB_TYPE_F64 - address1->type];
	qb_op *qop = qb_append_op(cxt, opcode, 4);	
	qop->operands[0] = operands[0];
	qop->operands[1] = operands[1];
	qop->operands[2] = operands[2];
	qop->operands[3] = operands[3];
	qb_mark_jump_target(cxt, qop_index, jump_target1);
	qb_mark_jump_target(cxt, qop_index, jump_target2);
	return qop;
}

static qb_comparison_branch_op_factory factory_branch_on_equal = {
	qb_append_binary_branch_op,
	QB_COERCE_TO_HIGHEST_RANK,
	0,
	{	QB_IF_EQ_F64_F64,	QB_IF_EQ_F32_F32,	QB_IF_EQ_I64_I64,	QB_IF_EQ_I64_I64,	QB_IF_EQ_I32_I32,	QB_IF_EQ_I32_I32,	QB_IF_EQ_I16_I16,	QB_IF_EQ_I16_I16,	QB_IF_EQ_I08_I08,	QB_IF_EQ_I08_I08,	},
	&factory_branch_on_not_equal,
};

static qb_comparison_branch_op_factory factory_branch_on_not_equal = { 
	qb_append_binary_branch_op,
	QB_COERCE_TO_HIGHEST_RANK,
	0,
	{	QB_IF_NE_F64_F64,	QB_IF_NE_F32_F32,	QB_IF_NE_I64_I64,	QB_IF_NE_I64_I64,	QB_IF_NE_I32_I32,	QB_IF_NE_I32_I32,	QB_IF_NE_I16_I16,	QB_IF_NE_I16_I16,	QB_IF_NE_I08_I08,	QB_IF_NE_I08_I08,	},
	&factory_branch_on_equal,
};

static qb_comparison_branch_op_factory factory_branch_on_less_than = {
	qb_append_binary_branch_op,
	QB_COERCE_TO_HIGHEST_RANK,
	0,
	{	QB_IF_LT_F64_F64,	QB_IF_LT_F32_F32,	QB_IF_LT_U64_U64,	QB_IF_LT_S64_S64,	QB_IF_LT_U32_U32,	QB_IF_LT_S32_S32,	QB_IF_LT_U16_U16,	QB_IF_LT_S16_S16,	QB_IF_LT_U08_U08,	QB_IF_LT_S08_S08,	},
	&factory_branch_on_greater_equal,
};

static qb_comparison_branch_op_factory factory_branch_on_less_equal = {
	qb_append_binary_branch_op,
	QB_COERCE_TO_HIGHEST_RANK,
	0,
	{	QB_IF_LE_F64_F64,	QB_IF_LE_F32_F32,	QB_IF_LE_U64_U64,	QB_IF_LE_S64_S64,	QB_IF_LE_U32_U32,	QB_IF_LE_S32_S32,	QB_IF_LE_U16_U16,	QB_IF_LE_S16_S16,	QB_IF_LE_U08_U08,	QB_IF_LE_S08_S08,	},
	&factory_branch_on_greater_than,
};

static qb_comparison_branch_op_factory factory_branch_on_greater_than = {
	qb_append_binary_branch_op,
	QB_COERCE_TO_HIGHEST_RANK,
	0,
	{	QB_IF_GT_F64_F64,	QB_IF_GT_F32_F32,	QB_IF_GT_U64_U64,	QB_IF_GT_S64_S64,	QB_IF_GT_U32_U32,	QB_IF_GT_S32_S32,	QB_IF_GT_U16_U16,	QB_IF_GT_S16_S16,	QB_IF_GT_U08_U08,	QB_IF_GT_S08_S08,	},
	&factory_branch_on_less_equal,
};

static qb_comparison_branch_op_factory factory_branch_on_greater_equal = {
	qb_append_binary_branch_op,
	QB_COERCE_TO_HIGHEST_RANK,
	0,
	{	QB_IF_GE_F64_F64,	QB_IF_GE_F32_F32,	QB_IF_GE_U64_U64,	QB_IF_GE_S64_S64,	QB_IF_GE_U32_U32,	QB_IF_GE_S32_S32,	QB_IF_GE_U16_U16,	QB_IF_GE_S16_S16,	QB_IF_GE_U08_U08,	QB_IF_GE_S08_S08,	},
	&factory_branch_on_less_than,
};

static qb_op * ZEND_FASTCALL qb_append_binary_reverse_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_operand reversed_operands[2];
	reversed_operands[0] = operands[1];
	reversed_operands[1] = operands[0];
	return qb_append_binary_op(cxt, factory, reversed_operands, operand_count, result);
}

static qb_basic_op_factory factory_set_greater_equal = {
	qb_append_binary_reverse_op,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_LE_SET_F64_F64_I32,	QB_LE_SET_F32_F32_I32,	QB_LE_SET_U64_U64_I32,	QB_LE_SET_S64_S64_I32,	QB_LE_SET_U32_U32_I32,	QB_LE_SET_S32_S32_I32,	QB_LE_SET_U16_U16_I32,	QB_LE_SET_S16_S16_I32,	QB_LE_SET_U08_U08_I32,	QB_LE_SET_S08_S08_I32,	},
};

static qb_basic_op_factory factory_set_greater_than = {
	qb_append_binary_reverse_op,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_LT_SET_F64_F64_I32,	QB_LT_SET_F32_F32_I32,	QB_LT_SET_U64_U64_I32,	QB_LT_SET_S64_S64_I32,	QB_LT_SET_U32_U32_I32,	QB_LT_SET_S32_S32_I32,	QB_LT_SET_U16_U16_I32,	QB_LT_SET_S16_S16_I32,	QB_LT_SET_U08_U08_I32,	QB_LT_SET_S08_S08_I32,	},
};

static qb_basic_op_factory factory_set_not = {
	qb_append_unary_op,
	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	0,	0,	0,	0,	QB_NOT_SET_I32_I32,	QB_NOT_SET_I32_I32,	0,	0,	0,	0,	},
};

static qb_basic_op_factory factory_all = {
	qb_append_unary_op,
	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_I32,
	{	0,	0,	0,	0,	QB_ALL_I32_I32,	QB_ALL_I32_I32,	0,	0,	0,	0,	},
};

static qb_basic_op_factory factory_any = {
	qb_append_unary_op,
	QB_COERCE_TO_BOOLEAN,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	0,	0,	0,	0,	QB_ANY_I32_I32,	QB_ANY_I32_I32,	0,	0,	0,	0,	},
};

static qb_address *ZEND_FASTCALL qb_get_subarray_sizes(qb_compiler_context *cxt, qb_address *address);

static qb_op *ZEND_FASTCALL qb_append_concat_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_string_op_factory *f = factory;
	qb_address *addition = operands[0].address;
	qb_opcode opcode;
	qb_op *qop;

	if(addition->dimension_count > 1) {
		qb_address *subarray_sizes_address = qb_get_subarray_sizes(cxt, addition);
		opcode = f->multidim_opcodes[QB_TYPE_F64 - addition->type];
		qop = qb_append_op(cxt, opcode, 3);
		qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
		qop->operands[0].address = addition;
		qop->operands[1].type = QB_OPERAND_ADDRESS_ARR;
		qop->operands[1].address = subarray_sizes_address;
		qop->operands[2] = *result;
		qb_unlock_address(cxt, subarray_sizes_address);
	} else {
		if(addition->flags & QB_ADDRESS_STRING) {
			opcode = f->text_opcode;
		} else {
			opcode = f->opcodes[QB_TYPE_F64 - addition->type];
		}
		qop = qb_append_op(cxt, opcode, 2);
		qop->operands[0] = operands[0];
		qop->operands[1] = *result;
	}
	return qop;
}

static qb_string_op_factory factory_concat = {
	qb_append_concat_op,
	0,
	0,
	{	QB_CAT_F64_U08,	QB_CAT_F32_U08,	QB_CAT_U64_U08,	QB_CAT_S64_U08,	QB_CAT_U32_U08,	QB_CAT_S32_U08,	QB_CAT_U16_U08,	QB_CAT_S16_U08,	QB_CAT_U08_U08,	QB_CAT_S08_U08,	},
	{	QB_CAT_DIM_F64_U32_U08,	QB_CAT_DIM_F32_U32_U08,	QB_CAT_DIM_U64_U32_U08,	QB_CAT_DIM_S64_U32_U08,	QB_CAT_DIM_U32_U32_U08,	QB_CAT_DIM_S32_U32_U08,	QB_CAT_DIM_U16_U32_U08,	QB_CAT_DIM_S16_U32_U08,	QB_CAT_DIM_U08_U32_U08,	QB_CAT_DIM_S08_U32_U08,	},
	QB_CAT_STR_U08_U08,
};

static qb_op *ZEND_FASTCALL qb_append_print_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_string_op_factory *f = factory;
	qb_address *address = operands[0].address;
	qb_opcode opcode;
	qb_op *qop = NULL;

	if(address->dimension_count > 1) {
		qb_address *subarray_sizes_address = qb_get_subarray_sizes(cxt, address);
		opcode = f->multidim_opcodes[QB_TYPE_F64 - address->type];
		qop = qb_append_op(cxt, opcode, 2);
		qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
		qop->operands[0].address = address;
		qop->operands[1].type = QB_OPERAND_ADDRESS_ARR;
		qop->operands[1].address = subarray_sizes_address;
		qb_unlock_address(cxt, subarray_sizes_address);
	} else {
		if(address->flags & QB_ADDRESS_STRING) {
			opcode = f->text_opcode;
		} else {
			opcode = f->opcodes[QB_TYPE_F64 - address->type];
		}
		qop = qb_append_op(cxt, opcode, 1);
		qop->operands[0] = operands[0];
	}
	return qop;
}

static qb_string_op_factory factory_print = {
	qb_append_print_op,
	0,
	0,
	{	QB_PRN_F64,	QB_PRN_F32,	QB_PRN_U64,	QB_PRN_S64,	QB_PRN_U32,	QB_PRN_S32,	QB_PRN_U16,	QB_PRN_S16,	QB_PRN_U08,	QB_PRN_S08,	},
	{	QB_PRN_DIM_F64_U32,	QB_PRN_DIM_F32_U32,	QB_PRN_DIM_U64_U32,	QB_PRN_DIM_S64_U32,	QB_PRN_DIM_U32_U32,	QB_PRN_DIM_S32_U32,	QB_PRN_DIM_U16_U32,	QB_PRN_DIM_S16_U32,	QB_PRN_DIM_U08_U32,	QB_PRN_DIM_S08_U32,	},
	QB_PRN_STR_U08,
};

static qb_op *ZEND_FASTCALL qb_append_simple_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_simple_op_factory *f = factory;
	qb_opcode opcode = f->opcode;
	uint32_t i;
	qb_op *qop = qb_append_op(cxt, opcode, operand_count);
	for(i = 0; i < operand_count; i++) {
		qop->operands[i] = operands[i];
	}
	return qop;
}

static qb_simple_op_factory factory_return = {
	qb_append_simple_op,
	0,
	0,
	QB_RET,
};

static qb_simple_op_factory factory_exit = {
	qb_append_simple_op,
	0,
	0,
	QB_EXIT_I32,
};

static qb_simple_op_factory factory_ext = {
	qb_append_simple_op,
	0,
	0,
	QB_EXT_U32,
};

static qb_op * ZEND_FASTCALL qb_append_jump_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_simple_op_factory *f = factory;
	qb_opcode opcode = f->opcode;
	uint32_t qop_index = cxt->op_count;
	uint32_t jump_target = operands[0].jump_target_index;
	qb_op *qop = qb_append_op(cxt, opcode, 1);
	qop->operands[0] = operands[0];
	qop->flags = QB_OP_JUMP;
	qb_mark_jump_target(cxt, qop_index, jump_target);
	return qop;
}

static qb_simple_op_factory factory_jump = {
	qb_append_jump_op,
	0,
	0,
	QB_JMP,
};

static qb_op * ZEND_FASTCALL qb_append_branch_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_simple_op_factory *f = factory;
	qb_opcode opcode = f->opcode;
	uint32_t qop_index = cxt->op_count;
	uint32_t jump_target1 = operands[0].jump_target_index;
	uint32_t jump_target2 = operands[1].jump_target_index;
	qb_op *qop = qb_append_op(cxt, opcode, 3);
	qop->operands[0] = operands[0];
	qop->operands[1] = operands[1];
	qop->operands[2] = operands[2];
	qb_mark_jump_target(cxt, qop_index, jump_target1);
	qb_mark_jump_target(cxt, qop_index, jump_target2);
	return qop;
}

static qb_simple_op_factory factory_branch_on_true = {
	qb_append_branch_op,
	QB_COERCE_TO_BOOLEAN,
	0,
	QB_IF_T_I32,
};

static qb_simple_op_factory factory_branch_on_false = {
	qb_append_branch_op,
	QB_COERCE_TO_BOOLEAN,
	0,
	QB_IF_F_I32,
};

static uint32_t ZEND_FASTCALL qb_import_external_symbol(qb_compiler_context *cxt, uint32_t type, const char *name, uint32_t name_len, void *pointer);

static qb_op * ZEND_FASTCALL qb_append_fcall_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_fcall_op_factory *f = factory;
	qb_operand *arguments = operands;
	uint32_t argument_count = operand_count;
	qb_op *qop;
	zend_function *zfunc = cxt->zend_function_being_called;
	uint32_t total_operand_size;
	uint32_t symbol_index, opcode = f->opcode;
	uint32_t i, j;

	// import the function
	symbol_index = qb_import_external_symbol(cxt, QB_EXT_SYM_ZEND_FUNCTION, zfunc->common.function_name, strlen(zfunc->common.function_name), zfunc);

	// figure out the length of the instruction
	// loop through the arguments and account for the return value as well
	for(i = 0, operand_count = 0; i < argument_count + 1; i++) {
		qb_address *address = (i < argument_count) ? arguments[i].address : result->address;
		// address flags and the address itself
		operand_count += 2;
		// the dimension address and array size address and their flags
		operand_count += 3 * address->dimension_count;

		// mark variables passed by-ref as writable
		if(i < zfunc->common.num_args && zfunc->common.arg_info) {
			zend_arg_info *zarg = &zfunc->common.arg_info[i];
			if(zarg->pass_by_reference) {
				qb_mark_as_writable(cxt, address);
			}
		} else {
			// not argument info--assume it's passed by ref just in case
			qb_mark_as_writable(cxt, address);
		}

		// mark all variables as exportable (since they have to be copied out and in)
		qb_mark_as_non_local(cxt, address);
	}

	// plus operand size, argument count, line number, and symbol index
	total_operand_size = sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t) + (sizeof(uint32_t) * operand_count);

	qop = qb_append_op(cxt, opcode, 3 + argument_count + 1);
	qop->operands[0].operand_size = total_operand_size;
	qop->operands[0].type = QB_OPERAND_TOTAL_LENGTH;
	qop->operands[1].argument_count = argument_count;
	qop->operands[1].type = QB_OPERAND_ARGUMENT_COUNT;
	qop->operands[2].symbol_index = symbol_index;
	qop->operands[2].type = QB_OPERAND_EXTERNAL_SYMBOL;

	// a function call can have side effects so the op cannot be removed
	qop->flags |= QB_OP_CANNOT_REMOVE;

	// put in the arguments
	for(i = 0, j = 3; i < argument_count; i++, j++) {
		qop->operands[j] = arguments[i];
	}

	// put in the return value
	qop->operands[j] = *result;
	return qop;
}

static qb_fcall_op_factory factory_fcall = {
	qb_append_fcall_op,
	0,
	0,
	QB_FCALL,
};

static qb_float_op_factory factory_sin = { 
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_SIN_F64_F64,			QB_SIN_F32_F32,	},
};

static qb_float_op_factory factory_asin = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_ASIN_F64_F64,		QB_ASIN_F32_F32,	},
};

static qb_float_op_factory factory_cos = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_COS_F64_F64,			QB_COS_F32_F32,	},
};

static qb_float_op_factory factory_acos = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_ACOS_F64_F64,		QB_ACOS_F32_F32,	},
};

static qb_float_op_factory factory_tan = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_TAN_F64_F64,			QB_TAN_F32_F32,	},
};

static qb_float_op_factory factory_atan = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_ATAN_F64_F64,		QB_ATAN_F32_F32,	},
};

static qb_float_op_factory factory_atan2 = {
	qb_append_binary_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_ATAN2_F32_F32_F32,	QB_ATAN2_F32_F32_F32,	},
};

static qb_float_op_factory factory_sinh = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_SINH_F64_F64,		QB_SINH_F32_F32,	},
};

static qb_float_op_factory factory_asinh = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_ASINH_F64_F64,		QB_ASINH_F32_F32,	},
};

static qb_float_op_factory factory_cosh = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_COSH_F64_F64,		QB_COSH_F32_F32,	},
};

static qb_float_op_factory factory_acosh = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_ACOSH_F64_F64,		QB_ACOSH_F32_F32,	},
};

static qb_float_op_factory factory_tanh = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_TANH_F64_F64,		QB_TANH_F32_F32,	},
};

static qb_float_op_factory factory_atanh = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_ATANH_F64_F64,		QB_ATANH_F32_F32,	},
};

static qb_float_op_factory factory_ceil = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_CEIL_F64_F64,		QB_CEIL_F32_F32,	},
};

static qb_float_op_factory factory_floor = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_FLOOR_F64_F64,		QB_FLOOR_F32_F32,	},
};

static qb_float_op_factory factory_fract = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_FRACT_F64_F64,		QB_FRACT_F32_F32,	},
};

static qb_op * ZEND_FASTCALL qb_append_ternary_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_basic_op_factory *f = factory;
	qb_address *address1 = operands[0].address;
	qb_opcode opcode = f->opcodes[QB_TYPE_F64 - address1->type];
	qb_op *qop = qb_append_op(cxt, opcode, 4);
	qop->operands[0] = operands[0];
	qop->operands[1] = operands[1];
	qop->operands[2] = operands[2];
	qop->operands[3] = *result;
	return qop;
}

static qb_float_op_factory factory_round = {
	qb_append_ternary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_ROUND_F64_I32_I32_F64,	QB_ROUND_F32_I32_I32_F32,	},
};

static qb_float_op_factory factory_log = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_LOG_F64_F64,			QB_LOG_F32_F32,	},
};

static qb_float_op_factory factory_log1p = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_LOG1P_F64_F64,		QB_LOG1P_F32_F32,	},
};

static qb_float_op_factory factory_log10 = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_LOG10_F64_F64,		QB_LOG10_F32_F32,	},
};

static qb_float_op_factory factory_log2 = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_LOG2_F64_F64,		QB_LOG2_F32_F32,	},
};

static qb_float_op_factory factory_exp = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_EXP_F64_F64,			QB_EXP_F32_F32,	},
};

static qb_float_op_factory factory_exp2 = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_EXP2_F64_F64,		QB_EXP2_F32_F32,	},
};

static qb_float_op_factory factory_expm1 = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_EXPM1_F64_F64,		QB_EXPM1_F32_F32,	},
};

static qb_float_op_factory factory_sqrt = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_SQRT_F64_F64,		QB_SQRT_F32_F32,	},
};

static qb_float_op_factory factory_rsqrt = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_RSQRT_F64_F64,		QB_RSQRT_F32_F32,	},
};

static qb_float_op_factory factory_pow = {
	qb_append_binary_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_POW_F64_F64_F64,		QB_POW_F32_F32_F32,	},
};

static qb_float_op_factory factory_hypot = {
	qb_append_binary_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_HYPOT_F64_F64_F64,	QB_HYPOT_F32_F32_F32,	},
};

static qb_float_op_factory factory_sign = { 
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_SIGN_F64_F64,		QB_SIGN_F32_F32,	},
};

static qb_float_op_factory factory_step = {
	qb_append_binary_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_STEP_F64_F64_F64,		QB_STEP_F32_F32_F32,	},
};

static qb_float_op_factory factory_clamp = {
	qb_append_ternary_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_CLAMP_F64_F64_F64_F64,	QB_CLAMP_F32_F32_F32_F32,	},
};

static qb_float_op_factory factory_mix = {
	qb_append_ternary_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_MIX_F64_F64_F64_F64,		QB_MIX_F32_F32_F32_F32,	},
};

static qb_float_op_factory factory_smooth_step = {
	qb_append_ternary_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_SSTEP_F64_F64_F64_F64,	QB_SSTEP_F32_F32_F32_F32,	},
};

static qb_float_op_factory factory_deg2rad = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_DEG2RAD_F64_F64,		QB_DEG2RAD_F32_F32,	},
};

static qb_float_op_factory factory_rad2deg = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_RAD2DEG_F64_F64,		QB_RAD2DEG_F32_F32,	},
};

static qb_float_op_factory factory_is_finite = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_FIN_F64_I32,		QB_FIN_F32_I32,	},
};

static qb_float_op_factory factory_is_infinite = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_INF_F64_I32,		QB_INF_F32_I32,	},
};

static qb_float_op_factory factory_is_nan = {
	qb_append_unary_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_RESULT_IS_BOOLEAN | QB_TYPE_I32,
	{	QB_NAN_F64_I32,		QB_NAN_F32_I32,	},
};	

static qb_basic_op_factory factory_abs = {
	qb_append_unary_op,
	QB_COERCE_TO_SIGNED,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_ABS_F64_F64,	QB_ABS_F32_F32,	0,	QB_ABS_S64_S64,	0,	QB_ABS_S32_S32,	0,	QB_ABS_S16_S16,	0,	QB_ABS_S08_S08,	},
};

static qb_op * ZEND_FASTCALL qb_append_minmax_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_minmax_op_factory *f = factory;
	qb_address *address1 = operands[0].address;
	qb_opcode opcode;
	qb_op *qop;

	if(operand_count == 2) {
		opcode = f->opcodes_pair[QB_TYPE_F64 - address1->type];
		qop = qb_append_op(cxt, opcode, 3);
		qop->operands[0] = operands[0];
		qop->operands[1] = operands[1];
		qop->operands[2] = *result;
	} else {
		// address1 should be an array
		opcode = f->opcodes_list[QB_TYPE_F64 - address1->type];
		qop = qb_append_op(cxt, opcode, 2);
		qop->operands[0] = operands[0];
		qop->operands[1] = *result;
	}
	return qop;
}

static qb_minmax_op_factory factory_min = {
	qb_append_minmax_op,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_MIN_F64_F64_F64,	QB_MIN_F32_F32_F32,	QB_MIN_U64_U64_U64,	QB_MIN_S64_S64_S64,	QB_MIN_U32_U32_U32,	QB_MIN_S32_S32_S32,	QB_MIN_U16_U16_U16,	QB_MIN_S16_S16_S16,	QB_MIN_U08_U08_U08,		QB_MIN_S08_S08_S08,	},
	{	QB_AMIN_F64_F64,	QB_AMIN_F32_F32,	QB_AMIN_U64_U64,	QB_AMIN_S64_S64,	QB_AMIN_U32_U32,	QB_AMIN_S32_S32,	QB_AMIN_U16_U16,	QB_AMIN_S16_S16,	QB_AMIN_U08_U08,		QB_AMIN_S08_S08,	},
};

static qb_minmax_op_factory factory_max = { 
	qb_append_minmax_op,
	QB_COERCE_TO_HIGHEST_RANK,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_MAX_F64_F64_F64,	QB_MAX_F32_F32_F32,	QB_MAX_U64_U64_U64,	QB_MAX_S64_S64_S64,	QB_MAX_U32_U32_U32,	QB_MAX_S32_S32_S32,	QB_MAX_U16_U16_U16,	QB_MAX_S16_S16_S16,	QB_MAX_U08_U08_U08,		QB_MAX_S08_S08_S08,	},
	{	QB_AMAX_F64_F64,	QB_AMAX_F32_F32,	QB_AMAX_U64_U64,	QB_AMAX_S64_S64,	QB_AMAX_U32_U32,	QB_AMAX_S32_S32,	QB_AMAX_U16_U16,	QB_AMAX_S16_S16,	QB_AMAX_U08_U08,		QB_AMAX_S08_S08,	},
};

static qb_basic_op_factory factory_rand = {
	qb_append_binary_op,
	QB_COERCE_TO_LVALUE_TYPE,
	QB_TYPE_OPERAND,
	{	0 ,	0,	QB_RAND_U64_U64_U64,	QB_RAND_S64_S64_S64,	QB_RAND_U32_U32_U32,	QB_RAND_S32_S32_S32,	QB_RAND_U16_U16_U16,	QB_RAND_S16_S16_S16,	QB_RAND_U08_U08_U08,	QB_RAND_S08_S08_S08,	},
};

static qb_basic_op_factory factory_mt_rand = {
	qb_append_binary_op,
	QB_COERCE_TO_LVALUE_TYPE,
	QB_TYPE_OPERAND,
	{	0,	0,	QB_MT_RAND_U64_U64_U64,	QB_MT_RAND_S64_S64_S64,	QB_MT_RAND_U32_U32_U32,	QB_MT_RAND_S32_S32_S32,	QB_MT_RAND_U16_U16_U16,	QB_MT_RAND_S16_S16_S16,	QB_MT_RAND_U08_U08_U08,	QB_MT_RAND_S08_S08_S08,	},
};

static qb_float_op_factory factory_lcg = {
	qb_append_nullary_op,
	QB_COERCE_TO_LVALUE_TYPE,
	QB_TYPE_OPERAND,
	{	QB_LCG_F64,				QB_LCG_F32,	},
};

static qb_op * ZEND_FASTCALL qb_append_sampling_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_sampling_op_factory *f = factory;
	qb_address *image_address = operands[0].address;
	qb_address *x_address = operands[1].address;
	qb_address *y_address = operands[2].address;
	uint32_t channel_count = qb_get_vector_width(cxt, image_address);
	qb_opcode opcode = f->opcodes[channel_count - 3][QB_TYPE_F64 - image_address->type];
	qb_op *qop;

	qop = qb_append_op(cxt, opcode, 6);
	qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[0].address = image_address;
	qop->operands[1].type = QB_OPERAND_ADDRESS_VAR;
	qop->operands[1].address = image_address->dimension_addresses[1];
	qop->operands[2].type = QB_OPERAND_ADDRESS_VAR;
	qop->operands[2].address = image_address->dimension_addresses[0];
	qop->operands[3].type = QB_OPERAND_ADDRESS;
	qop->operands[3].address = x_address;
	qop->operands[4].type = QB_OPERAND_ADDRESS;
	qop->operands[4].address = y_address;
	qop->operands[5].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[5].address = result->address;
	return qop;
}

static qb_sampling_op_factory factory_sample_nearest	= { 
	qb_append_sampling_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT,
	0,
	{
		{	QB_SAMPLE_3X_NN_F64_U32_U32_F64_F64_F64,	QB_SAMPLE_3X_NN_F32_U32_U32_F32_F32_F32,	},
		{	QB_SAMPLE_4X_NN_F64_U32_U32_F64_F64_F64,	QB_SAMPLE_4X_NN_F32_U32_U32_F32_F32_F32,	},
	}
};

static qb_sampling_op_factory factory_sample_bilinear = {
	qb_append_sampling_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT,
	0,
	{
		{	QB_SAMPLE_3X_BL_F64_U32_U32_F64_F64_F64,	QB_SAMPLE_3X_BL_F32_U32_U32_F32_F32_F32,	},
		{	QB_SAMPLE_4X_BL_F64_U32_U32_F64_F64_F64,	QB_SAMPLE_4X_BL_F32_U32_U32_F32_F32_F32,	},
	}
};

static qb_op * ZEND_FASTCALL qb_append_vector_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_vector_op_factory *f = factory;
	qb_address *address1 = operands[0].address;
	qb_address *address2 = (operand_count >= 2) ? operands[1].address : NULL;
	uint32_t dimension1 = qb_get_vector_width(cxt, address1);
	qb_op *qop;
	qb_opcode opcode;
	uint32_t i;

	if(2 <= dimension1 && dimension1 <= 4) {
		opcode = f->opcodes_fixed_size[dimension1 - 2][QB_TYPE_F64 - address1->type];
	} else {
		opcode = f->opcodes_any_size[QB_TYPE_F64 - address1->type];
	}
	if(address1->dimension_count > 1 || (address2 && address2->dimension_count > 1)) {
		// handling multiple input vectors
		uint32_t op_flags = qb_get_op_flags(cxt, opcode);
		if(!(op_flags & QB_OP_MULTI_ADDRESS)) {
			opcode = opcode + 1;
		}
	}
	qop = qb_append_op(cxt, opcode, operand_count + 1);
	for(i = 0; i < operand_count; i++) {
		qop->operands[i].type = QB_OPERAND_ADDRESS_ARR;
		qop->operands[i].address = operands[i].address;
	}
	qop->operands[operand_count] = *result;
	qop->matrix_dimensions = V_DIMENSIONS(dimension1);
	return qop;
}

static qb_vector_op_factory factory_dot_product = {
	qb_append_vector_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_VECTOR_COUNT | QB_TYPE_OPERAND,
	{	QB_DOT_F64_F64_F64,		QB_DOT_F32_F32_F32,	},
	{
		{	QB_DOT_2X_F64_F64_F64,	QB_DOT_2X_F32_F32_F32,	},
		{	QB_DOT_3X_F64_F64_F64,	QB_DOT_3X_F32_F32_F32,	},
		{	QB_DOT_4X_F64_F64_F64,	QB_DOT_4X_F32_F32_F32,	},
	},
};

static qb_vector_op_factory factory_length = {
	qb_append_vector_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_VECTOR_COUNT | QB_TYPE_OPERAND,
	{	QB_LEN_F64_F64,			QB_LEN_F32_F32,	},
	{
		{	QB_LEN_2X_F64_F64,		QB_LEN_2X_F32_F32,	},
		{	QB_LEN_3X_F64_F64,		QB_LEN_3X_F32_F32,	},
		{	QB_LEN_4X_F64_F64,		QB_LEN_4X_F32_F32,	},
	},
};

static qb_vector_op_factory factory_distance = {
	qb_append_vector_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_VECTOR_COUNT | QB_TYPE_OPERAND,
	{	QB_DIS_F64_F64_F64,		QB_DIS_F32_F32_F32,	},
	{
		{	QB_DIS_2X_F64_F64_F64,	QB_DIS_2X_F32_F32_F32,	},
		{	QB_DIS_3X_F64_F64_F64,	QB_DIS_3X_F32_F32_F32,	},
		{	QB_DIS_4X_F64_F64_F64,	QB_DIS_4X_F32_F32_F32,	},
	},
};

static qb_vector_op_factory factory_normalize = {
	qb_append_vector_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_NORM_F64_F64,		QB_NORM_F32_F32,	},
	{
		{	QB_NORM_2X_F64_F64,		QB_NORM_2X_F32_F32,	},
		{	QB_NORM_3X_F64_F64,		QB_NORM_3X_F32_F32,	},
		{	QB_NORM_4X_F64_F64,		QB_NORM_4X_F32_F32,	},
	},
};

static qb_vector_op_factory factory_faceforward = { 
	qb_append_vector_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_FORE_F64_F64_F64,		QB_FORE_F32_F32_F32,	},
	{
		{	QB_FORE_2X_F64_F64_F64,		QB_FORE_2X_F32_F32_F32,	},
		{	QB_FORE_3X_F64_F64_F64,		QB_FORE_3X_F32_F32_F32,	},
		{	QB_FORE_4X_F64_F64_F64,		QB_FORE_4X_F32_F32_F32,	},
	},
};

static qb_vector_op_factory factory_reflect = { 
	qb_append_vector_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_REFL_F64_F64_F64,		QB_REFL_F32_F32_F32,	},
	{
		{	QB_REFL_2X_F64_F64_F64,		QB_REFL_2X_F32_F32_F32,	},
		{	QB_REFL_3X_F64_F64_F64,		QB_REFL_3X_F32_F32_F32,	},
		{	QB_REFL_4X_F64_F64_F64,		QB_REFL_4X_F32_F32_F32,	},
	},
};

static qb_address * ZEND_FASTCALL qb_obtain_temporary_scalar(qb_compiler_context *cxt, qb_primitive_type desired_type);

static qb_op * ZEND_FASTCALL qb_append_refract(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_vector_op_factory *f = factory;
	qb_address *address1 = operands[0].address;
	qb_address *address2 = operands[1].address;
	qb_address *eta_address = operands[2].address;
	uint32_t dimension1 = qb_get_vector_width(cxt, address1);
	qb_op *qop;
	qb_opcode opcode;

	if(2 <= dimension1 && dimension1 <= 4) {
		opcode = f->opcodes_fixed_size[dimension1 - 2][QB_TYPE_F64 - address1->type];
	} else {
		opcode = f->opcodes_any_size[QB_TYPE_F64 - address1->type];
	}
	if(address1->dimension_count > 1 || (address2 && address2->dimension_count > 1)) {
		// handling multiple input vectors
		opcode = opcode + 1;
	}
	// the third operand must be compatible with VAR address mode
	if(!IS_SCALAR_VARIABLE(eta_address)) {
		// copy it to a temp variable
		qb_address *temp_address = qb_obtain_temporary_scalar(cxt, eta_address->type);
		qb_create_unary_op(cxt, &factory_copy, eta_address, temp_address);
		temp_address = eta_address;
	}

	qop = qb_append_op(cxt, opcode, operand_count + 1);
	qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[0].address = address1;
	qop->operands[1].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[1].address = address2;
	qop->operands[2].type = QB_OPERAND_ADDRESS_VAR;
	qop->operands[2].address = eta_address;
	qop->operands[3] = *result;
	qop->matrix_dimensions = V_DIMENSIONS(dimension1);
	return qop;
}

static qb_vector_op_factory factory_refract = { 
	qb_append_refract,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_REFR_F64_F64_F64_F64,		QB_REFR_F32_F32_F32_F32,	},
	{
		{	QB_REFR_2X_F64_F64_F64_F64,		QB_REFR_2X_F32_F32_F32_F32,	},
		{	QB_REFR_3X_F64_F64_F64_F64,		QB_REFR_3X_F32_F32_F32_F32,	},
		{	QB_REFR_4X_F64_F64_F64_F64,		QB_REFR_4X_F32_F32_F32_F32,	},
	},
};

static qb_op * ZEND_FASTCALL qb_append_cross(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_basic_op_factory *f = factory;
	qb_address *address1 = operands[0].address;
	qb_address *address2 = operands[1].address;
	qb_opcode opcode = f->opcodes[QB_TYPE_F64 - address1->type];
	qb_op *qop;
	if(address1->dimension_count > 1 || address2->dimension_count > 1) {
		opcode = opcode + 1;
	}
	qop = qb_append_op(cxt, opcode, 3);
	qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[0].address = address1;
	qop->operands[1].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[1].address = address2;
	qop->operands[2] = *result;
	return qop;
}

static qb_float_op_factory factory_cross_product = {
	qb_append_cross,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_CROSS_F64_F64_F64,		QB_CROSS_F32_F32_F32,	},
};

static qb_op * ZEND_FASTCALL qb_append_matrix_matrix_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_matrix_op_factory *f = factory;
	qb_address *address1 = operands[0].address;
	qb_address *address2 = operands[1].address;
	uint32_t m1_rows = qb_get_matrix_row_count(cxt, address1);
	uint32_t m1_cols = qb_get_matrix_column_count(cxt, address1);
	uint32_t m2_rows = qb_get_matrix_row_count(cxt, address2);
	uint32_t m2_cols = qb_get_matrix_column_count(cxt, address2);
	qb_op *qop;
	qb_opcode opcode;

	if(cxt->matrix_padding && m1_rows == 3 && m1_cols == 4 && m2_rows == 3 && m2_cols == 4) {
		opcode = f->opcode_3x3_padded;
	} else if((m1_rows == m1_cols) && (m2_rows == m2_cols) && (2 <= m1_rows && m1_rows <= 4)) {
		opcode = f->opcodes_fixed_size[m1_rows - 2][QB_TYPE_F64 - address1->type];
	} else {
		opcode = f->opcodes_any_size[QB_TYPE_F64 - address1->type];
	}
	if(address1->dimension_count > 2 || address2->dimension_count > 2) {
		// handling multiple matrices or vectors
		opcode = opcode + 1;
	}
	qop = qb_append_op(cxt, opcode, 3);
	qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[0].address = operands[0].address;
	qop->operands[1].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[1].address = operands[1].address;
	qop->operands[2].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[2].address = result->address;
	qop->matrix_dimensions = MM_DIMENSIONS(m1_rows, m1_cols, m2_rows, m2_cols);
	return qop;
}	

static qb_matrix_op_factory factory_mm_multiply = { 
	qb_append_matrix_matrix_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_MM_PRODUCT | QB_TYPE_OPERAND,
	{	QB_MUL_MM_F64_F64_F64,			QB_MUL_MM_F32_F32_F32,	},
	{
		{	QB_MUL_MM_2X2_F64_F64_F64,		QB_MUL_MM_2X2_F32_F32_F32,	},
		{	QB_MUL_MM_3X3_F64_F64_F64,		QB_MUL_MM_3X3_F32_F32_F32,	},
		{	QB_MUL_MM_4X4_F64_F64_F64,		QB_MUL_MM_4X4_F32_F32_F32,	},
	},
	QB_MUL_MM_3X3P_F32_F32_F32,
};

static qb_op * ZEND_FASTCALL qb_append_matrix_vector_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_matrix_op_factory *f = factory;
	qb_address *address1 = operands[0].address;
	qb_address *address2 = operands[1].address;
	uint32_t m_rows = qb_get_matrix_row_count(cxt, address1);
	uint32_t m_cols = qb_get_matrix_column_count(cxt, address1);
	uint32_t v_width = qb_get_vector_width(cxt, address1);
	qb_op *qop;
	qb_opcode opcode;

	if(cxt->matrix_padding && m_rows == 3 && m_cols == 4) {
		opcode = f->opcode_3x3_padded;
	} else if((m_rows == m_cols) && (2 <= m_rows && m_rows <= 4)) {
		opcode = f->opcodes_fixed_size[m_rows - 2][QB_TYPE_F64 - address1->type];
	} else {
		opcode = f->opcodes_any_size[QB_TYPE_F64 - address1->type];
	}
	if(address1->dimension_count > 1 || address2->dimension_count > 2) {
		// handling multiple matrices or vectors
		opcode = opcode + 1;
	}
	qop = qb_append_op(cxt, opcode, 3);
	qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[0].address = operands[0].address;
	qop->operands[1].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[1].address = operands[1].address;
	qop->operands[2].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[2].address = result->address;
	qop->matrix_dimensions = MV_DIMENSIONS(m_rows, m_cols, v_width);
	return qop;
}	

static qb_matrix_op_factory factory_mv_multiply = { 
	qb_append_matrix_vector_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_MV_PRODUCT | QB_TYPE_OPERAND,
	{	QB_MUL_MV_F64_F64_F64,			QB_MUL_MV_F32_F32_F32,	},
	{
		{	QB_MUL_MV_2X2_F64_F64_F64,		QB_MUL_MV_2X2_F32_F32_F32,	},
		{	QB_MUL_MV_3X3_F64_F64_F64,		QB_MUL_MV_3X3_F32_F32_F32,	},
		{	QB_MUL_MV_4X4_F64_F64_F64,		QB_MUL_MV_4X4_F32_F32_F32,	},
	},
	QB_MUL_MV_3X3P_F32_F32_F32,
};

static qb_op * ZEND_FASTCALL qb_append_vector_matrix_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_matrix_op_factory *f = factory;
	qb_address *address1 = operands[0].address;
	qb_address *address2 = operands[1].address;
	uint32_t v_width = qb_get_vector_width(cxt, address1);
	uint32_t m_rows = qb_get_matrix_row_count(cxt, address2);
	uint32_t m_cols = qb_get_matrix_column_count(cxt, address2);
	qb_op *qop;
	qb_opcode opcode;

	if(cxt->matrix_padding && m_rows == 3 && m_cols == 4) {
		opcode = f->opcode_3x3_padded;
	} else if((m_rows == m_cols) && (2 <= m_rows && m_rows <= 4)) {
		opcode = f->opcodes_fixed_size[m_rows - 2][QB_TYPE_F64 - address1->type];
	} else {
		opcode = f->opcodes_any_size[QB_TYPE_F64 - address1->type];
	}
	if(address1->dimension_count > 1 || address2->dimension_count > 2) {
		opcode = opcode + 1;
	}
	qop = qb_append_op(cxt, opcode, 3);
	qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[0].address = operands[0].address;
	qop->operands[1].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[1].address = operands[1].address;
	qop->operands[2].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[2].address = result->address;
	qop->matrix_dimensions = VM_DIMENSIONS(v_width, m_rows, m_cols);
	return qop;
}	

static qb_matrix_op_factory factory_vm_multiply = { 
	qb_append_vector_matrix_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_VM_PRODUCT | QB_TYPE_OPERAND,
	{	QB_MUL_VM_F64_F64_F64,			QB_MUL_VM_F32_F32_F32,	},
	{
		{	QB_MUL_VM_2X2_F64_F64_F64,		QB_MUL_VM_2X2_F32_F32_F32,	},
		{	QB_MUL_VM_3X3_F64_F64_F64,		QB_MUL_VM_3X3_F32_F32_F32,	},
		{	QB_MUL_VM_4X4_F64_F64_F64,		QB_MUL_VM_4X4_F32_F32_F32,	},
	},
	QB_MUL_VM_3X3P_F32_F32_F32,
};

static qb_op * ZEND_FASTCALL qb_append_matrix_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_vector_op_factory *f = factory;
	qb_address *address1 = operands[0].address;
	uint32_t dimension1 = qb_get_vector_width(cxt, address1);
	qb_op *qop;
	qb_opcode opcode;
	uint32_t i;

	if(2 <= dimension1 && dimension1 <= 4) {
		opcode = f->opcodes_fixed_size[dimension1 - 2][QB_TYPE_F64 - address1->type];
	} else {
		opcode = f->opcodes_any_size[QB_TYPE_F64 - address1->type];
	}
	if(address1->dimension_count > 2) {
		// handling multiple input vectors
		uint32_t op_flags = qb_get_op_flags(cxt, opcode);
		if(!(op_flags & QB_OP_MULTI_ADDRESS)) {
			opcode = opcode + 1;
		}
	}
	qop = qb_append_op(cxt, opcode, operand_count + 1);
	for(i = 0; i < operand_count; i++) {
		qop->operands[i].type = QB_OPERAND_ADDRESS_ARR;
		qop->operands[i].address = operands[i].address;
	}
	qop->operands[operand_count] = *result;
	qop->matrix_dimensions = V_DIMENSIONS(dimension1);
	return qop;
}

static qb_matrix_op_factory factory_transpose = { 
	qb_append_matrix_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	0,			0,	},
	{
		{	QB_MTRAN_2X_F64_F64,		QB_MTRAN_2X_F32_F32,	},
		{	QB_MTRAN_3X_F64_F64,		QB_MTRAN_3X_F32_F32,	},
		{	QB_MTRAN_4X_F64_F64,		QB_MTRAN_4X_F32_F32,	},
	},
	0,
};

static qb_matrix_op_factory factory_inverse = { 
	qb_append_matrix_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	0,			0,	},
	{
		{	QB_MINV_2X_F64_F64,		QB_MINV_2X_F32_F32,	},
		{	QB_MINV_3X_F64_F64,		QB_MINV_3X_F32_F32,	},
		{	QB_MINV_4X_F64_F64,		QB_MINV_4X_F32_F32,	},
	},
	0,
};

static qb_matrix_op_factory factory_determinant = { 
	qb_append_matrix_op,
	QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_MATRIX_COUNT | QB_TYPE_OPERAND,
	{	0,			0,	},
	{
		{	QB_MDET_2X_F64_F64,		QB_MDET_2X_F32_F32,	},
		{	QB_MDET_3X_F64_F64,		QB_MDET_3X_F32_F32,	},
		{	QB_MDET_4X_F64_F64,		QB_MDET_4X_F32_F32,	},
	},
	0,
};

static qb_op * ZEND_FASTCALL qb_append_utf8_decode(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_basic_op_factory *f = factory;
	qb_address *address = result->address;
	qb_opcode opcode = f->opcodes[QB_TYPE_F64 - address->type];
	qb_op *qop = qb_append_op(cxt, opcode, 2);
	qop->operands[0] = operands[0];
	qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[1] = *result;
	qop->operands[1].type = QB_OPERAND_ADDRESS_ARR;
	return qop;
}

static qb_basic_op_factory factory_utf8_decode = {
	qb_append_utf8_decode,
	0,
	0,
	{	0, 0, 0, 0, QB_UTF8_DEC_U08_U32,	QB_UTF8_DEC_U08_U32,	QB_UTF8_DEC_U08_U16,	QB_UTF8_DEC_U08_U16,	},
};

static qb_op * ZEND_FASTCALL qb_append_utf8_encode(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_basic_op_factory *f = factory;
	qb_address *address = operands[0].address;
	qb_opcode opcode = f->opcodes[QB_TYPE_F64 - address->type];
	qb_op *qop = qb_append_op(cxt, opcode, 2);
	qop->operands[0] = operands[0];
	qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[1] = *result;
	qop->operands[1].type = QB_OPERAND_ADDRESS_ARR;
	return qop;
}

static qb_basic_op_factory factory_utf8_encode = {
	qb_append_utf8_encode,
	0,
	0,
	{	0, 0, 0, 0, QB_UTF8_ENC_U32_U08,	QB_UTF8_ENC_U32_U08,	QB_UTF8_ENC_U16_U08,	QB_UTF8_ENC_U16_U08,	},
};

static qb_op * ZEND_FASTCALL qb_append_pack(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_basic_op_factory *f = factory;
	qb_address *address = operands[0].address;
	qb_opcode opcode = f->opcodes[QB_TYPE_F64 - address->type];
	qb_op *qop = qb_append_op(cxt, opcode, 2);
	qop->operands[0] = operands[0];
	qop->operands[1] = *result;
	// operand is always an array and should not be involved in the address mode resolution process
	qop->operands[1].type = QB_OPERAND_ADDRESS_ARR;
	return qop;
}

static qb_basic_op_factory factory_pack_le = { 
	qb_append_pack,
	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	{	QB_PACK_LE_F64_U08,	QB_PACK_LE_F32_U08,	QB_PACK_LE_I64_U08,	QB_PACK_LE_I64_U08,	QB_PACK_LE_I32_U08,	QB_PACK_LE_I32_U08,	QB_PACK_LE_I16_U08,	QB_PACK_LE_I16_U08,	},
};

static qb_basic_op_factory factory_pack_be = {
	qb_append_pack,
	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	{	QB_PACK_BE_F64_U08 ,	QB_PACK_BE_F32_U08,	QB_PACK_BE_I64_U08,	QB_PACK_BE_I64_U08,	QB_PACK_BE_I32_U08,	QB_PACK_BE_I32_U08,	QB_PACK_BE_I16_U08,	QB_PACK_BE_I16_U08,	},
};

static qb_op * ZEND_FASTCALL qb_append_unpack(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_basic_op_factory *f = factory;
	qb_address *address = result->address;
	qb_opcode opcode = f->opcodes[QB_TYPE_F64 - address->type];
	qb_op *qop = qb_append_op(cxt, opcode, 2);
	qop->operands[0] = operands[0];
	qop->operands[0].type = QB_OPERAND_ADDRESS_ARR;
	qop->operands[1] = *result;
	return qop;
}

static qb_basic_op_factory factory_unpack_le = {
	qb_append_unpack,
	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	{	QB_UNPACK_LE_U08_F64,	QB_UNPACK_LE_U08_F32,	QB_UNPACK_LE_U08_I64,	QB_UNPACK_LE_U08_I64,	QB_UNPACK_LE_U08_I32,	QB_UNPACK_LE_U08_I32,	QB_UNPACK_LE_U08_I16,	QB_UNPACK_LE_U08_I16	},
};

static qb_basic_op_factory factory_unpack_be = {
	qb_append_unpack,
	0,
	QB_RESULT_FROM_PURE_FUNCTION,
	{	QB_UNPACK_BE_U08_F64,	QB_UNPACK_BE_U08_F32,	QB_UNPACK_BE_U08_I64,	QB_UNPACK_BE_U08_I64,	QB_UNPACK_BE_U08_I32,	QB_UNPACK_BE_U08_I32,	QB_UNPACK_BE_U08_I16,	QB_UNPACK_BE_U08_I16	},
};

static qb_basic_op_factory factory_sort = {
	qb_append_nullary_op,
	0,
	0,
	{	QB_SORT_F64 ,	QB_SORT_F32,	QB_SORT_U64,	QB_SORT_S64,	QB_SORT_U32,	QB_SORT_S32,	QB_SORT_U16,	QB_SORT_S16,	QB_SORT_U08,	QB_SORT_S08	},
};

static qb_basic_op_factory factory_rsort = {
	qb_append_nullary_op,
	0,
	0,
	{	QB_RSORT_F64,	QB_RSORT_F32,	QB_RSORT_U64,	QB_RSORT_S64,	QB_RSORT_U32,	QB_RSORT_S32,	QB_RSORT_U16,	QB_RSORT_S16,	QB_RSORT_U08,	QB_RSORT_S08	},
};

static qb_basic_op_factory factory_array_sum = { 
	qb_append_unary_op,
	0,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_ASUM_F64_F64,	QB_ASUM_F32_F32,	QB_ASUM_U64_U64,	QB_ASUM_S64_S64,	QB_ASUM_U32_U32,	QB_ASUM_S32_S32,	QB_ASUM_U16_U16,	QB_ASUM_S16_S16,	QB_ASUM_U08_U08,	QB_ASUM_S08_S08	},
};

static qb_basic_op_factory factory_array_product = {
	qb_append_unary_op,
	0,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_APROD_F64_F64,	QB_APROD_F32_F32,	QB_APROD_U64_U64,	QB_APROD_S64_S64,	QB_APROD_U32_U32,	QB_APROD_S32_S32,	QB_APROD_U16_U16,	QB_APROD_S16_S16,	QB_APROD_U08_U08,	QB_APROD_S08_S08	},
};

static qb_basic_op_factory factory_array_search = { 
	qb_append_binary_op,
	QB_COERCE_TO_SECOND_OPERAND_TYPE,
	QB_TYPE_I32,
	{	QB_AFIND_IDX_F64_F64_I32,	QB_AFIND_IDX_F32_F32_I32,	QB_AFIND_IDX_I64_I64_I32,	QB_AFIND_IDX_I64_I64_I32,	QB_AFIND_IDX_I32_I32_I32,	QB_AFIND_IDX_I32_I32_I32,	QB_AFIND_IDX_I16_I16_I32,	QB_AFIND_IDX_I16_I16_I32,	QB_AFIND_IDX_I08_I08_I32,	QB_AFIND_IDX_I08_I08_I32	},
};

static qb_basic_op_factory factory_in_array = {
	qb_append_binary_op,
	QB_COERCE_TO_SECOND_OPERAND_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_I32,
	{	QB_AFIND_F64_F64_I32,	QB_AFIND_F32_F32_I32,	QB_AFIND_I64_I64_I32,	QB_AFIND_I64_I64_I32,	QB_AFIND_I32_I32_I32,	QB_AFIND_I32_I32_I32,	QB_AFIND_I16_I16_I32,	QB_AFIND_I16_I16_I32,	QB_AFIND_I08_I08_I32,	QB_AFIND_I08_I08_I32	},
};

static qb_basic_op_factory factory_subarray_pos = {
	qb_append_ternary_op,
	QB_COERCE_TO_FIRST_OPERAND_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_I32,
	{	QB_APOS_F64_F64_I32_I32,	QB_APOS_F32_F32_I32_I32,	QB_APOS_I64_I64_I32_I32,	QB_APOS_I64_I64_I32_I32,	QB_APOS_I32_I32_I32_I32,	QB_APOS_I32_I32_I32_I32,	QB_APOS_I16_I16_I32_I32,	QB_APOS_I16_I16_I32_I32,	QB_APOS_I08_I08_I32_I32,	QB_APOS_I08_I08_I32_I32	},
};

static qb_basic_op_factory factory_subarray_rpos = {
	qb_append_ternary_op,
	QB_COERCE_TO_FIRST_OPERAND_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_I32,
	{	QB_ARPOS_F64_F64_I32_I32,	QB_ARPOS_F32_F32_I32_I32,	QB_ARPOS_I64_I64_I32_I32,	QB_ARPOS_I64_I64_I32_I32,	QB_ARPOS_I32_I32_I32_I32,	QB_ARPOS_I32_I32_I32_I32,	QB_ARPOS_I16_I16_I32_I32,	QB_ARPOS_I16_I16_I32_I32,	QB_ARPOS_I08_I08_I32_I32,	QB_ARPOS_I08_I08_I32_I32	},
};

static qb_basic_op_factory factory_array_reverse = {
	qb_append_ternary_op,
	QB_COERCE_TO_FIRST_OPERAND_TYPE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_TYPE_OPERAND,
	{	QB_AREV_F64_U32_U32_F64,	QB_AREV_F32_U32_U32_F32,	QB_AREV_I64_U32_U32_I64,	QB_AREV_I64_U32_U32_I64,	QB_AREV_I32_U32_U32_I32,	QB_AREV_I32_U32_U32_I32,	QB_AREV_I16_U32_U32_I16,	QB_AREV_I16_U32_U32_I16,	QB_AREV_I08_U32_U32_I08,	QB_AREV_I08_U32_U32_I08		},
};

static qb_op * ZEND_FASTCALL qb_append_complex_number_op(qb_compiler_context *cxt, void *factory, qb_operand *operands, uint32_t operand_count, qb_operand *result) {
	qb_basic_op_factory *f = factory;
	qb_address *address1 = operands[0].address;
	qb_opcode opcode = f->opcodes[QB_TYPE_F64 - address1->type];
	qb_op *qop;
	uint32_t i;
	for(i = 0; i < operand_count; i++) {
		qb_address *address = operands[i].address;
		if(address->dimension_count > 1) {
			opcode = opcode + 1;
			break;
		}
	}
	qop = qb_append_op(cxt, opcode, operand_count + 1);
	for(i = 0; i < operand_count; i++) {
		qb_address *address = operands[i].address;
		qop->operands[i].type = QB_OPERAND_ADDRESS_ARR;
		qop->operands[i].address = address;
	}
	qop->operands[operand_count] = *result;
	return qop;
}

static qb_float_op_factory factory_complex_abs = {
	qb_append_complex_number_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_VECTOR_COUNT | QB_TYPE_OPERAND,
	{	QB_CABS_F64_F64,	QB_CABS_F32_F32	},
};

static qb_float_op_factory factory_complex_arg = {
	qb_append_complex_number_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_VECTOR_COUNT | QB_TYPE_OPERAND,
	{	QB_CARG_F64_F64,	QB_CARG_F32_F32	},
};


static qb_float_op_factory factory_complex_multiply = {
	qb_append_complex_number_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_CMUL_F64_F64_F64,	QB_CMUL_F32_F32_F32	},
};

static qb_float_op_factory factory_complex_divide = {
	qb_append_complex_number_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_CDIV_F64_F64_F64,	QB_CDIV_F32_F32_F32	},
};

static qb_float_op_factory factory_complex_exp = {
	qb_append_complex_number_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_CEXP_F64_F64,	QB_CEXP_F32_F32	},
};

static qb_float_op_factory factory_complex_log = {
	qb_append_complex_number_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_CLOG_F64_F64,	QB_CLOG_F32_F32	},
};

static qb_float_op_factory factory_complex_sqrt = {
	qb_append_complex_number_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_CSQRT_F64_F64,	QB_CSQRT_F32_F32	},
};

static qb_float_op_factory factory_complex_pow = {
	qb_append_complex_number_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_CPOW_F64_F64_F64,	QB_CPOW_F32_F32_F32	},
};

static qb_float_op_factory factory_complex_sin = {
	qb_append_complex_number_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_CSIN_F64_F64,	QB_CSIN_F32_F32	},
};

static qb_float_op_factory factory_complex_cos = {
	qb_append_complex_number_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_CCOS_F64_F64,	QB_CCOS_F32_F32	},
};

static qb_float_op_factory factory_complex_tan = {
	qb_append_complex_number_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_CTAN_F64_F64,	QB_CTAN_F32_F32	},
};

static qb_float_op_factory factory_complex_sinh = {
	qb_append_complex_number_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_CSINH_F64_F64,	QB_CSINH_F32_F32	},
};

static qb_float_op_factory factory_complex_cosh = {
	qb_append_complex_number_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_CCOSH_F64_F64,	QB_CCOSH_F32_F32	},
};

static qb_float_op_factory factory_complex_tanh = {
	qb_append_complex_number_op,
	QB_COERCE_TO_HIGHEST_RANK | QB_COERCE_TO_FLOATING_POINT | QB_COERCE_TO_INTEGER_TO_DOUBLE,
	QB_RESULT_FROM_PURE_FUNCTION | QB_RESULT_SIZE_OPERAND | QB_TYPE_OPERAND,
	{	QB_CTANH_F64_F64,	QB_CTANH_F32_F32	},
};

