<?php

class QBATanhHandler extends QBHandler {

	protected function getScalarExpression() {
		$cType = $this->getOperandCType($this->srcCount + 1);
		return "res = ($cType) atanh(op1);";

	}
}

?>