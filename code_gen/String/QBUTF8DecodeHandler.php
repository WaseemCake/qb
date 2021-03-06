<?php

class QBUTF8DecodeHandler extends QBHandler {

	public function getOperandType($i) {
		// type can be U16 for UCS-16 and U32 for UCS-32
		if($i == 1) {
			return "U08";
		} else if($i == 2) {
			return $this->operandType;
		}
	}

	public function getOperandAddressMode($i) {
		return "ARR";
	}

	
	public function getHelperFunctions() {
		$type = $this->getOperandType(2);
		$cType = $this->getOperandCType(2);
		$folder = dirname(__FILE__);
		$functions = array(
			file("$folder/bh_utf8.c", FILE_IGNORE_NEW_LINES),
			array(
				"static void ZEND_FASTCALL qb_decode_utf8_$type(qb_interpreter_context *cxt, uint8_t *utf8_chars, uint32_t utf8_length, $cType *buffer) {",
					"uint32_t codepoint, state = 0, i, j;",
					"for(i = 0, j = 0; i < utf8_length; i++) {",
						"if(!decode(&state, &codepoint, utf8_chars[i])) {",
							"buffer[j++] = codepoint;",
						"}",
					"}",
				"}",
			),
			array(
				"static uint32_t ZEND_FASTCALL qb_get_utf8_codepoint_count(qb_interpreter_context *cxt, uint8_t *utf8_chars, uint32_t utf8_length) {",
					"uint32_t codepoint, state = 0, count = 0, i;",
					"for(i = 0; i < utf8_length; i++) {",
						"if(!decode(&state, &codepoint, utf8_chars[i])) {",
							"count++;",
						"}",
					"}",
					"return count;",
				"}",
			),
		);
		return $functions;
	}	
	
	public function getResultSizePossibilities() {
		return "string_length";
	}

	public function getResultSizeCalculation() {
		return "string_length = qb_get_utf8_codepoint_count(cxt, op1_ptr, op1_count);";
	}
	
	public function getAction() {
		$type = $this->getOperandType(2);
		return "qb_decode_utf8_$type(cxt, op1_ptr, op1_count, res_ptr);";
	}
}

?>