#include <iostream>
#include <cstring>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>

#include <Lox/Lox.hpp>

#include "ErrorCode.hpp"
#include "Natives.hpp"

void PrintUsage();

int RunRepl(Lox::VirtualMachine& vm);

int RunLoxFile(Lox::VirtualMachine& vm, const std::string& path);

int RunChunkFile(Lox::VirtualMachine& vm, const std::string& path);

int CompileFile(Lox::VirtualMachine& vm, const std::string& sourcePath, const std::string& destPath);

int RunString(Lox::VirtualMachine& vm, const std::string& filePath, const std::string& input);

int RunFunction(Lox::VirtualMachine& vm, Lox::GcPtr<Lox::Closure> func);

int ReadSource(const std::string& path, std::string& buffer);

int ReadSource(std::ifstream& file, std::string& result);

int CompileString(Lox::VirtualMachine& vm, Lox::GcPtr<Lox::Closure>& func, const std::string& input,
                  const std::string& filePath);

int CatchLoxErrors(const std::function<void()>& fn);

void ErrorAndUsage(std::string_view msg);

// Yeah, const `argv`.
int main(int argc, const char* argv[])
{
    Lox::VirtualMachineConfiguration conf(std::cout, std::cin, std::cout);
    Lox::VirtualMachine vm(conf);
    Natives::AddNatives(vm);
    
    if (argc < 2)
    {
        ErrorAndUsage("not enough arguments");
        return ErrorCodeNotEnoughArguments;
    }
    
    if (strcmp(argv[1], "repl") == 0)
    {
        return RunRepl(vm);
    }
    else if (strcmp(argv[1], "run_file") == 0)
    {
        if (argc < 3)
        {
            ErrorAndUsage("not enough arguments");
            return ErrorCodeNoRunFile;
        }
        
        return RunLoxFile(vm, argv[2]);
    }
    else if (strcmp(argv[1], "run_bytecode") == 0)
    {
        if (argc < 3)
        {
            ErrorAndUsage("not enough arguments");
            return ErrorCodeNoRunFile;
        }
        
        return RunChunkFile(vm, argv[2]);
    }
    else if (strcmp(argv[1], "compile") == 0)
    {
        if (argc < 4)
        {
            ErrorAndUsage("not enough arguments");
            return ErrorCodeNoRunFile;
        }
        
        return CompileFile(vm, argv[2], argv[3]);
    }
    else
    {
        ErrorAndUsage("unknown command");
        return ErrorCodeUnknownCommand;
    }
}

int RunRepl(Lox::VirtualMachine& vm)
{
    std::cout << "Lox language bytecode interpreter written in C++." << std::endl;
    std::cout << "Write ':quit' (without quotes) to exit." << std::endl;
    
    while (true)
    {
        std::cout << "> ";
        
        std::string line;
        if (!std::getline(std::cin, line))
        {
            // QUESTION: Ok or not?
            return ErrorCodeOk;
        }
        
        if (line == ":quit")
        {
            return ErrorCodeOk;
        }
        
        RunString(vm, "stdin", line);
    }
}

int RunLoxFile(Lox::VirtualMachine& vm, const std::string& path)
{
    std::string source;
    int result = ReadSource(path, source);
    if (result != ErrorCodeOk)
    {
        return result;
    }
    
    Lox::GcPtr<Lox::Closure> func;
    
    int compileResult = CompileString(vm, func, source, path);
    if (compileResult != ErrorCodeOk)
    {
        return compileResult;
    }
    
    return RunFunction(vm, func);
}

int RunChunkFile(Lox::VirtualMachine& vm, const std::string& path)
{
    std::ifstream file(path);
    if (!file)
    {
        ErrorAndUsage("could not read file");
        return ErrorCodeCouldNotOpenFile;
    }
    
    std::unique_ptr<Lox::Chunk> chunk = Lox::ReadChunk(vm, file);
    
    if (!chunk)
    {
        ErrorAndUsage("chunk wrong file format or I/O error");
        return ErrorCodeChunkReadError;
    }
    
    Lox::GcPtr<Lox::String> name = vm.InternString("<script>");
    Lox::GcPtr<Lox::Closure> func = vm.AllocateObject<Lox::Closure>(name, 0, std::move(*chunk));
    
    return RunFunction(vm, func);
}

int CompileFile(Lox::VirtualMachine& vm, const std::string& sourcePath, const std::string& destPath)
{
    std::string source;
    
    int readResult = ReadSource(sourcePath, source);
    if (readResult != ErrorCodeOk)
    {
        return readResult;
    }
    
    Lox::GcPtr<Lox::Closure> func;
    
    int compileResult = CompileString(vm, func, source, sourcePath);
    if (compileResult != ErrorCodeOk)
    {
        return compileResult;
    }
    
    std::ofstream file(destPath);
    if (!file)
    {
        std::cout << "Error: could not write to file '" << destPath << "'" << std::endl;
        return ErrorCodeWriteChunkError;
    }
    
    Lox::ChunkWriter writer(func->GetChunk(), file);
    writer.Write();
    
    // TODO: Check for errors?
    
    return ErrorCodeOk;
}

int RunString(Lox::VirtualMachine& vm, const std::string& filePath, const std::string& input)
{
    Lox::GcPtr<Lox::Closure> func;
    
    int result = CompileString(vm, func, input, filePath);
    if (result != ErrorCodeOk)
    {
        return result;
    }
    
    return RunFunction(vm, func);
}

// May throw Lox exceptions.
void OptimizeFunc(Lox::VirtualMachine& vm, Lox::GcPtr<Lox::Closure> func);

int RunFunction(Lox::VirtualMachine& vm, Lox::GcPtr<Lox::Closure> func)
{
    Lox::ChunkChecker checker(func->GetChunk());
    if (!checker.Check())
    {
        return ErrorCodeChunkCheckFailed;
    }
    
    int optimizeResult = CatchLoxErrors([&vm, func]
                                        {
                                            OptimizeFunc(vm, func);
                                        });
    if (optimizeResult != ErrorCodeOk)
    {
        return optimizeResult;
    }
    
    return CatchLoxErrors([&vm, func]
                          {
                              vm.RunScript(func);
                          });
}

void OptimizeFunc(Lox::VirtualMachine& vm, Lox::GcPtr<Lox::Closure> func)
{
    Lox::ChunkOptimizer optimizer(vm, func->GetChunk());
    optimizer.Optimize();
    
    /*
    Lox::ChunkDumper dumper(func->GetChunk(), func->GetName()->GetCppString(),
                            std::cout);
    std::cout << "After optimizing:" << std::endl;
    dumper.Dump();
     */
}

int ReadSource(const std::string& path, std::string& buffer)
{
    std::ifstream file(path);
    if (!file)
    {
        ErrorAndUsage("could not read file");
        return ErrorCodeCouldNotOpenFile;
    }
    
    return ReadSource(file, buffer);
}

int ReadSource(std::ifstream& file, std::string& result)
{
    std::stringstream ss;
    ss << file.rdbuf();
    
    // TODO: Check for I/O error?
    
    result = ss.str();
    
    return ErrorCodeOk;
}

int CompileString(Lox::VirtualMachine& vm, Lox::GcPtr<Lox::Closure>& func, const std::string& input,
                  const std::string& filePath)
{
    class BasicErrorReporter final : public Lox::CompilerErrorReporter
    {
    public:
        void Error(Lox::SourcePosition pos, std::string_view msg) override
        {
            std::cerr << "Error [" << pos << "]: " << msg << std::endl;
        }
    }; // class BasicErrorReporter
    
    BasicErrorReporter errorReporter;
    
    Lox::GcPtr<Lox::Closure> script = Lox::Compile(vm, errorReporter, filePath, input);
    
    if (script.IsNullptr())
    {
        std::cerr << "Error: had errors while compiling. Exiting." << std::endl;
        return ErrorCodeCompileError;
    }
    
    func = script;
    
    return ErrorCodeOk;
}

int CatchLoxErrors(const std::function<void()>& fn)
{
    try
    {
        fn();
        return ErrorCodeOk;
    }
    catch (const Lox::Exceptions::StackOverflow& e)
    {
        std::cerr << e << std::endl;
        return ErrorCodeStackOverflow;
    }
    catch (const Lox::Exceptions::StackUnderflow& e)
    {
        std::cerr << e << std::endl;
        return ErrorCodeStackUnderflow;
    }
    catch (const Lox::Exceptions::UnknownInstruction& e)
    {
        std::cerr << e << std::endl;
        return ErrorCodeUnknownInstruction;
    }
    catch (const Lox::Exceptions::ZeroDivision& e)
    {
        std::cerr << e << std::endl;
        return ErrorCodeZeroDivision;
    }
    catch (const Lox::Exceptions::WrongTypeValue& e)
    {
        std::cerr << e << std::endl;
        return ErrorCodeWrongType;
    }
    catch (const Lox::Exceptions::UndefinedVariable& e)
    {
        std::cerr << e << std::endl;
        return ErrorCodeUndefinedVariable;
    }
    catch (const Lox::Exceptions::OptimizerFailure& e)
    {
        std::cerr << e << std::endl;
        return ErrorCodeOptimizerFailure;
    }
    catch (const Lox::Exceptions::NonCallable& e)
    {
        std::cerr << e << std::endl;
        return ErrorCodeNonCallable;
    }
    catch (const Lox::Exceptions::WrongArgumentsCount& e)
    {
        std::cerr << e << std::endl;
        return ErrorCodeRuntimeWrongArgumentsCount;
    }
    catch (const Lox::Exceptions::UndefinedProperty& e)
    {
        std::cerr << e << std::endl;
        return ErrorCodeUndefinedProperty;
    }
    catch (const Lox::Exceptions::RuntimeException& e)
    {
        std::cerr << e << std::endl;
        return ErrorCodeGenericRuntimeError;
    }
}

void PrintUsage()
{
    std::cerr << "Usage: lox command [file]" << std::endl;
    std::cerr << "Where:" << std::endl;
    std::cerr << "  command - one of { repl, run_file, run_bytecode, compile }" << std::endl;
    std::cerr << "  file - path to file" << std::endl;
    std::cerr << "When: command = compile:" << std::endl;
    std::cerr << "  Usage: lox compile source destination" << std::endl;
    std::cerr << "    Where:" << std::endl;
    std::cerr << "      source - path to lox source file" << std::endl;
    std::cerr << "      destination - path to bytecode file" << std::endl;
}

void ErrorAndUsage(std::string_view msg)
{
    std::cerr << "Error: " << msg << "." << std::endl;
    PrintUsage();
}
