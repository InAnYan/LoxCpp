cmake_minimum_required(VERSION 3.26)
project(LoxCpp)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-gdwarf-4)

add_library(LoxLib LoxLib/src
        LoxLib/src/Lox
        LoxLib/src/Lox/DataStructures
        LoxLib/src/Lox/DataStructures/Chunk.cpp
        LoxLib/src/Lox/DataStructures/LinesArray.cpp
        LoxLib/src/Lox/Compiler
        LoxLib/src/Lox/Compiler/Compilers/Compiler.cpp
        LoxLib/src/Lox/Compiler/Scanning/Token.cpp
        LoxLib/src/Lox/Compiler/Scanning/SourcePosition.cpp
        LoxLib/src/Lox/Compiler/Scanning/TokenType.cpp
        LoxLib/src/Lox/Compiler/Scanning/Scanner.cpp
        LoxLib/src/Lox/Stages
        LoxLib/src/Lox/Stages/ChunkOptimizer.cpp
        LoxLib/src/Lox/Runtime
        LoxLib/src/Lox/Runtime/Value.cpp
        LoxLib/src/Lox/Runtime/Nil.cpp
        LoxLib/src/Lox/Runtime/ValueType.cpp
        LoxLib/src/Lox/Interpreter
        LoxLib/src/Lox/Interpreter/VirtualMachineConfiguration.cpp
        LoxLib/src/Lox/Interpreter/Exceptions
        LoxLib/src/Lox/Interpreter/Exceptions/ZeroDivision.cpp
        LoxLib/src/Lox/Interpreter/Exceptions/StackOverflow.cpp
        LoxLib/src/Lox/Interpreter/Exceptions/StackUnderflow.cpp
        LoxLib/src/Lox/Interpreter/Exceptions/RuntimeException.cpp
        LoxLib/src/Lox/Interpreter/Exceptions/UnknownInstruction.cpp
        LoxLib/src/Lox/Interpreter/CallFrame.cpp
        LoxLib/src/Lox/Interpreter/Opcode.cpp
        LoxLib/src/Lox/Interpreter/VirtualMachine.cpp
        LoxLib/src/Lox/Util
        LoxLib/src/Lox/Util/Assert.cpp
        LoxLib/src/Lox/Stages/ChunkChecker.cpp
        LoxLib/src/Lox/Util/ChunkDumper.cpp
        LoxLib/src/Lox/Util/ChunkWriter.cpp
        LoxLib/src/Lox/Util/ChunkReader.cpp
        LoxLib/include/Lox/Stages/Exceptions/OptimizerFailure.hpp
        LoxLib/src/Lox/Stages/Exceptions/OptimizerFailure.cpp
        LoxLib/include/Lox/Runtime/ObjectType.hpp
        LoxLib/src/Lox/Runtime/ObjectType.cpp
        LoxLib/include/Lox/Runtime/Object.hpp
        LoxLib/include/Lox/Runtime/GcPtr.hpp
        LoxLib/src/Lox/Runtime/Object.cpp
        LoxLib/include/Lox/Runtime/Objects/String.hpp
        LoxLib/src/Lox/Runtime/Objects/String.cpp
        LoxLib/include/Lox/Runtime/PrintFlags.hpp
        LoxLib/include/Lox/Runtime/MemoryManager.hpp
        LoxLib/src/Lox/Runtime/MemoryManager.cpp
        LoxLib/include/Lox/Interpreter/Exceptions/WrongType.hpp
        LoxLib/include/Lox/Interpreter/Exceptions/UndefinedVariable.hpp
        LoxLib/src/Lox/Interpreter/Exceptions/UndefinedVariable.cpp
        LoxLib/include/Lox/Interpreter/Exceptions/InvalidStackAccess.hpp
        LoxLib/src/Lox/Interpreter/Exceptions/InvalidStackAccess.cpp
        LoxLib/include/Lox/Interpreter/Exceptions/All.hpp
        LoxLib/include/Lox/Runtime/Objects/Closure.hpp
        LoxLib/src/Lox/Runtime/Objects/Closure.cpp
        LoxLib/src/Lox/Compiler/Compilers/Compiler.hpp
        LoxLib/src/Lox/Compiler/Compiler.cpp
        LoxLib/src/Lox/Compiler/Parsing/Parser.hpp
        LoxLib/src/Lox/Compiler/Parsing/Parser.cpp
        LoxLib/src/Lox/Compiler/Parsing/Precedence.hpp
        LoxLib/src/Lox/Compiler/Parsing/ParserRule.hpp
        LoxLib/src/Lox/Compiler/Parsing/ParserRule.cpp
        LoxLib/include/Lox/Interpreter/Exceptions/NonCallable.hpp
        LoxLib/src/Lox/Interpreter/Exceptions/NonCallable.cpp
        LoxLib/include/Lox/Interpreter/Exceptions/WrongArgumentsCount.hpp
        LoxLib/src/Lox/Interpreter/Exceptions/WrongArgumentsCount.cpp
        LoxLib/include/Lox/Runtime/Objects/Native.hpp
        LoxLib/src/Lox/Runtime/Objects/Native.cpp
        LoxLib/include/Lox/Interpreter/StackIterator.hpp
        LoxLib/include/Lox/Util/OptionalReference.hpp
        LoxLib/include/Lox/Util/StackedBoolean.hpp
        LoxLib/src/Lox/Util/StackedBoolean.cpp
        LoxLib/include/Lox/Runtime/Objects/Upvalue.hpp
        LoxLib/src/Lox/Runtime/Objects/Upvalue.cpp
        LoxLib/include/Lox/Runtime/RootsSource.hpp
        LoxLib/include/Lox/Runtime/Objects/Class.hpp
        LoxLib/src/Lox/Runtime/Objects/Class.cpp
        LoxLib/include/Lox/Runtime/Objects/Instance.hpp
        LoxLib/src/Lox/Runtime/Objects/Instance.cpp
        LoxLib/include/Lox/Interpreter/Exceptions/UndefinedProperty.hpp
        LoxLib/src/Lox/Interpreter/Exceptions/UndefinedProperty.cpp
        LoxLib/src/Lox/Compiler/Compilers/ScopedValueChange.hpp
        LoxLib/src/Lox/Compiler/Compilers/FunctionType.hpp
        LoxLib/src/Lox/Compiler/Compilers/ClassCompiler.hpp
        LoxLib/src/Lox/Compiler/Compilers/ClassCompiler.cpp
        LoxLib/include/Lox/Runtime/Objects/BoundMethod.hpp
        LoxLib/src/Lox/Runtime/Objects/BoundMethod.cpp
)
set_target_properties(LoxLib PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(LoxLib PRIVATE LoxLib/include)

add_executable(LoxInterpreter
        LoxInterpreter/src
        LoxInterpreter/src/main.cpp
        LoxInterpreter/src/Natives.hpp
        LoxInterpreter/src/ErrorCode.hpp
        LoxInterpreter/src/Natives.cpp)
target_link_libraries(LoxInterpreter LoxLib)
set_target_properties(LoxInterpreter PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(LoxInterpreter PRIVATE LoxLib/include)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(LoxGoogleTest
        LoxGoogleTest/src/ValueTest.cpp
        LoxGoogleTest/src/ScannerTest.cpp
        LoxGoogleTest/src/CompilerTest.cpp
        LoxGoogleTest/src/CheckerTest.cpp
        LoxGoogleTest/src/ChunkRwTest.cpp
        LoxGoogleTest/src/OptimizerTest.cpp
        LoxGoogleTest/src/VmTest.cpp)

target_link_libraries(LoxGoogleTest GTest::gtest_main LoxLib)
target_include_directories(LoxGoogleTest PRIVATE LoxLib/include)

include(GoogleTest)
gtest_discover_tests(LoxGoogleTest)
