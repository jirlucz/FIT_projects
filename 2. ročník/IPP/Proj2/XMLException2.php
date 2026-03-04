<?php


namespace IPP\Student;

use IPP\Core\ReturnCode;
use IPP\Core\Exception\IPPException;
use Throwable;

/**
 * Exception for an invalid source XML
 */
class XMLBadStructException extends IPPException
{
    public function __construct(string $message = "Invalid XML structure", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::INVALID_SOURCE_STRUCTURE, $previous, false);
    }
}

class SemanticErrorRedefinitionException extends IPPException
{
    public function __construct(string $message = "Semantic error, redefinition of variable", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::SEMANTIC_ERROR, $previous, false);
    }
}

class SemanticErrorIncompatibleException extends IPPException
{
    public function __construct(string $message = "Semantic error, incompatible type and value", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::SEMANTIC_ERROR, $previous, false);
    }
}

class SemanticErrorNotDefinedException extends IPPException
{
    public function __construct(string $message = "Semantic error, variable is undefined", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::SEMANTIC_ERROR, $previous, false);
    }
}

class SemanticErrorWrongOperandException extends IPPException
{
    public function __construct(string $message = "Semantic error, wrong operand type", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::OPERAND_TYPE_ERROR, $previous, false);
    }
}



class ZeroDivException extends IPPException
{
    public function __construct(string $message = "Division by zero was detected", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::OPERAND_VALUE_ERROR, $previous, false);
    }
}

class FrameAccessErrorException extends IPPException
{
    public function __construct(string $message = "Error while accessing data frame", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::FRAME_ACCESS_ERROR, $previous, false);
    }
}

class LabelNameException extends IPPException
{
    public function __construct(string $message = "Error, found two labels with the same name", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::SEMANTIC_ERROR, $previous, false);
    }
}

class LabelNotFoundException extends IPPException
{
    public function __construct(string $message = "Error, label name not found", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::SEMANTIC_ERROR, $previous, false);
    }
}

class CantCompareException extends IPPException
{
    public function __construct(string $message = "Error, types cant be compared", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::OPERAND_TYPE_ERROR, $previous, false);
    }
}

class EmptyCallStackException extends IPPException
{
    public function __construct(string $message = "Error, reading from empty call stack", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::VALUE_ERROR, $previous, false);
    }
}

class EmptyDataStackException extends IPPException
{
    public function __construct(string $message = "Error, reading from empty data stack", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::VALUE_ERROR, $previous, false);
    }
}

class StringOperationNotUnicodeValException extends IPPException
{
    public function __construct(string $message = "Error, while converting int to char", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::STRING_OPERATION_ERROR, $previous, false);
    }
}

class StrToCharNotNumException extends IPPException
{
    public function __construct(string $message = "Error, given position index is not a number", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::STRING_OPERATION_ERROR, $previous, false);
    }
}

class StrToCharNotStringException extends IPPException
{
    public function __construct(string $message = "Error, given string has no string value", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::STRING_OPERATION_ERROR, $previous, false);
    }
}

class StrToCharOutOfIndexException extends IPPException
{
    public function __construct(string $message = "Error, out of index access", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::STRING_OPERATION_ERROR, $previous, false);
    }
}

class StrToCharFunctionErrorException extends IPPException
{
    public function __construct(string $message = "Error, while converting char to int value", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::STRING_OPERATION_ERROR, $previous, false);
    }
}

class UnknownTypeException extends IPPException
{
    public function __construct(string $message = "Error, unknown type has been found", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::SEMANTIC_ERROR, $previous, false);
    }
}

class ExitCodeErrorException extends IPPException
{
    public function __construct(string $message = "Error, incompatible value in function exit", ?Throwable $previous = null)
    {
        parent::__construct($message, ReturnCode::OPERAND_VALUE_ERROR, $previous, false);
    }
}