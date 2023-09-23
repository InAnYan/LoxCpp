#ifndef LOX_INTERPRETER_ERROR_CODE_HPP
#define LOX_INTERPRETER_ERROR_CODE_HPP

enum ErrorCode
{
    ErrorCodeNotEnoughArguments = 100,
    ErrorCodeNoRunFile,
    ErrorCodeUnknownCommand,
    
    ErrorCodeCouldNotOpenFile,
    
    ErrorCodeChunkReadError,
    ErrorCodeWriteChunkError,
    
    ErrorCodeCompileError,
    ErrorCodeChunkCheckFailed,
    
    ErrorCodeOptimizerFailure,
    
    ErrorCodeStackOverflow,
    ErrorCodeStackUnderflow,
    ErrorCodeUnknownInstruction,
    ErrorCodeZeroDivision,
    ErrorCodeWrongType,
    ErrorCodeUndefinedVariable,
    ErrorCodeNonCallable,
    ErrorCodeRuntimeWrongArgumentsCount,
    ErrorCodeUndefinedProperty,
    ErrorCodeGenericRuntimeError,
    
    ErrorCodeOk = 0,
}; // enum ErrorCode

#endif // LOX_INTERPRETER_ERROR_CODE_HPP
