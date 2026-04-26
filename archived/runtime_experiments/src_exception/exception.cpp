// بسم الله الرحمن الرحيم
// ===================================================================
// Exception Handling System Implementation - تنفيذ نظام معالجة الاستثناءات
// Phase 5.6: Exception Handling
// ===================================================================

#include "exception/exception.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <cerrno>

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#else
#include <execinfo.h>
#include <cxxabi.h>
#include <unistd.h>
#endif

namespace sad {
namespace runtime {

// ===================================================================
// StackFrame Implementation
// ===================================================================

StackFrame::StackFrame(const std::string& function,
                       const std::string& file,
                       int line,
                       void* address)
    : function_name_(function)
    , file_name_(file)
    , line_number_(line)
    , address_(address) {
}

std::string StackFrame::to_string() const {
    std::ostringstream oss;
    
    if (!function_name_.empty()) {
        oss << function_name_;
    } else {
        oss << "<unknown function>";
    }
    
    if (!file_name_.empty()) {
        oss << " at " << file_name_;
        if (line_number_ > 0) {
            oss << ":" << line_number_;
        }
    }
    
    if (address_ != nullptr) {
        oss << " [0x" << std::hex << reinterpret_cast<uintptr_t>(address_) << "]";
    }
    
    return oss.str();
}

// ===================================================================
// StackTrace Implementation
// ===================================================================

void StackTrace::capture(int skip_frames, int max_frames) {
    frames_.clear();
    
#ifdef _WIN32
    // Windows implementation using SymFromAddr
    void* stack[64];
    HANDLE process = GetCurrentProcess();
    
    SymInitialize(process, NULL, TRUE);
    
    WORD frame_count = CaptureStackBackTrace(
        skip_frames + 1,  // Skip this function too
        max_frames,
        stack,
        NULL
    );
    
    SYMBOL_INFO* symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + 256);
    if (symbol) {
        symbol->MaxNameLen = 255;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        
        for (WORD i = 0; i < frame_count; i++) {
            DWORD64 address = (DWORD64)stack[i];
            
            if (SymFromAddr(process, address, 0, symbol)) {
                // Try to get line information
                IMAGEHLP_LINE64 line;
                line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
                DWORD displacement = 0;
                
                if (SymGetLineFromAddr64(process, address, &displacement, &line)) {
                    add_frame(symbol->Name, line.FileName, line.LineNumber, stack[i]);
                } else {
                    add_frame(symbol->Name, "", 0, stack[i]);
                }
            } else {
                add_frame("<unknown>", "", 0, stack[i]);
            }
        }
        
        free(symbol);
    }
    
    SymCleanup(process);
    
#else
    // Linux/Unix implementation using backtrace
    void* stack[64];
    int frame_count = backtrace(stack, max_frames);
    
    char** symbols = backtrace_symbols(stack, frame_count);
    if (symbols) {
        for (int i = skip_frames + 1; i < frame_count; i++) {
            std::string symbol_str(symbols[i]);
            
            // Try to demangle C++ symbols
            size_t start = symbol_str.find('(');
            size_t end = symbol_str.find('+', start);
            
            if (start != std::string::npos && end != std::string::npos) {
                std::string mangled = symbol_str.substr(start + 1, end - start - 1);
                
                int status;
                char* demangled = abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status);
                
                if (status == 0 && demangled) {
                    add_frame(demangled, "", 0, stack[i]);
                    free(demangled);
                } else {
                    add_frame(mangled, "", 0, stack[i]);
                }
            } else {
                add_frame(symbol_str, "", 0, stack[i]);
            }
        }
        
        free(symbols);
    }
#endif
}

void StackTrace::add_frame(const StackFrame& frame) {
    frames_.push_back(frame);
}

void StackTrace::add_frame(const std::string& function,
                          const std::string& file,
                          int line,
                          void* address) {
    frames_.emplace_back(function, file, line, address);
}

std::string StackTrace::to_string() const {
    std::ostringstream oss;
    oss << "Stack trace:\n";
    
    for (size_t i = 0; i < frames_.size(); i++) {
        oss << "  #" << i << " " << frames_[i].to_string() << "\n";
    }
    
    return oss.str();
}

void StackTrace::print() const {
    std::cerr << to_string();
}

// ===================================================================
// Exception Type Names
// ===================================================================

const char* get_exception_type_name(ExceptionType type) {
    switch (type) {
        case ExceptionType::RUNTIME_ERROR:      return "RuntimeError";
        case ExceptionType::LOGIC_ERROR:        return "LogicError";
        case ExceptionType::SYSTEM_ERROR:       return "SystemError";
        case ExceptionType::TYPE_ERROR:         return "TypeError";
        case ExceptionType::CAST_ERROR:         return "CastError";
        case ExceptionType::VALUE_ERROR:        return "ValueError";
        case ExceptionType::RANGE_ERROR:        return "RangeError";
        case ExceptionType::OVERFLOW_ERROR:     return "OverflowError";
        case ExceptionType::UNDERFLOW_ERROR:    return "UnderflowError";
        case ExceptionType::OUT_OF_MEMORY:      return "OutOfMemoryError";
        case ExceptionType::NULL_POINTER:       return "NullPointerError";
        case ExceptionType::INVALID_MEMORY:     return "InvalidMemoryError";
        case ExceptionType::INDEX_ERROR:        return "IndexError";
        case ExceptionType::KEY_ERROR:          return "KeyError";
        case ExceptionType::IO_ERROR:           return "IOError";
        case ExceptionType::FILE_NOT_FOUND:     return "FileNotFoundError";
        case ExceptionType::PERMISSION_DENIED:  return "PermissionDeniedError";
        case ExceptionType::DIVISION_BY_ZERO:   return "DivisionByZeroError";
        case ExceptionType::IMPORT_ERROR:       return "ImportError";
        case ExceptionType::MODULE_NOT_FOUND:   return "ModuleNotFoundError";
        case ExceptionType::ATTRIBUTE_ERROR:    return "AttributeError";
        case ExceptionType::METHOD_NOT_FOUND:   return "MethodNotFoundError";
        case ExceptionType::THREAD_ERROR:       return "ThreadError";
        case ExceptionType::DEADLOCK:           return "DeadlockError";
        case ExceptionType::CUSTOM_ERROR:       return "CustomError";
        default:                                return "UnknownError";
    }
}

// ===================================================================
// Exception Implementation
// ===================================================================

Exception::Exception(ExceptionType type,
                     const std::string& message,
                     const std::string& file,
                     int line)
    : type_(type)
    , message_(message)
    , file_(file)
    , line_(line) {
    // Capture stack trace
    stack_trace_.capture(2);  // Skip constructor frames
}

const char* Exception::what() const noexcept {
    if (cached_what_.empty()) {
        cached_what_ = to_string();
    }
    return cached_what_.c_str();
}

std::string Exception::to_string() const {
    std::ostringstream oss;
    
    oss << get_type_name() << ": " << message_;
    
    if (!file_.empty()) {
        oss << "\n  at " << file_;
        if (line_ > 0) {
            oss << ":" << line_;
        }
    }
    
    return oss.str();
}

void Exception::print() const {
    std::cerr << to_string() << std::endl;
    
    if (!stack_trace_.get_frames().empty()) {
        std::cerr << "\n" << stack_trace_.to_string();
    }
    
    // Print cause chain
    if (cause_) {
        std::cerr << "\nCaused by:\n";
        cause_->print();
    }
}

// ===================================================================
// ExceptionHandler Implementation
// ===================================================================

// Thread-local exception context
thread_local ExceptionContext ExceptionHandler::context_;

ExceptionHandler& ExceptionHandler::get_instance() {
    static ExceptionHandler instance;
    return instance;
}

void ExceptionHandler::throw_exception(std::shared_ptr<Exception> exception) {
    context_.current_exception = exception;
    throw *exception;
}

void ExceptionHandler::throw_exception(Exception* exception) {
    throw_exception(std::shared_ptr<Exception>(exception));
}

void ExceptionHandler::rethrow() {
    if (context_.current_exception) {
        throw *context_.current_exception;
    } else {
        throw RuntimeError("No exception to rethrow");
    }
}

std::shared_ptr<Exception> ExceptionHandler::get_current_exception() const {
    return context_.current_exception;
}

bool ExceptionHandler::has_current_exception() const {
    return context_.current_exception != nullptr;
}

void ExceptionHandler::add_cleanup_handler(std::function<void()> handler) {
    context_.cleanup_handlers.push_back(handler);
}

void ExceptionHandler::execute_cleanup_handlers() {
    // Execute in reverse order (LIFO)
    for (auto it = context_.cleanup_handlers.rbegin();
         it != context_.cleanup_handlers.rend();
         ++it) {
        try {
            (*it)();
        } catch (...) {
            // Ignore exceptions in cleanup handlers
        }
    }
    context_.cleanup_handlers.clear();
}

void ExceptionHandler::clear_exception() {
    context_.current_exception = nullptr;
    context_.in_catch_block = false;
    context_.in_finally_block = false;
}

ExceptionContext& ExceptionHandler::get_context() {
    return context_;
}

// ===================================================================
// Utility Functions
// ===================================================================

std::shared_ptr<Exception> create_exception_from_errno(int error_code,
                                                        const std::string& context) {
    std::string message = context.empty() ? "" : context + ": ";
    message += std::strerror(error_code);
    
    // Map errno to exception types
    switch (error_code) {
        case ENOMEM:
            return std::make_shared<OutOfMemoryError>(message);
        case ENOENT:
            return std::make_shared<FileNotFoundError>(message);
        case EACCES:
        case EPERM:
            return std::make_shared<IOError>(message);
        default:
            return std::make_shared<RuntimeError>(message);
    }
}

void print_exception(const Exception& ex, bool include_stack_trace) {
    std::cerr << ex.get_type_name() << ": " << ex.get_message() << std::endl;
    
    if (!ex.get_file().empty()) {
        std::cerr << "  at " << ex.get_file();
        if (ex.get_line() > 0) {
            std::cerr << ":" << ex.get_line();
        }
        std::cerr << std::endl;
    }
    
    if (include_stack_trace && !ex.get_stack_trace().get_frames().empty()) {
        std::cerr << "\n" << ex.get_stack_trace().to_string();
    }
    
    // Print cause chain
    auto cause = ex.get_cause();
    if (cause) {
        std::cerr << "\nCaused by:\n";
        print_exception(*cause, include_stack_trace);
    }
}

std::vector<std::shared_ptr<Exception>> get_exception_chain(const Exception& ex) {
    std::vector<std::shared_ptr<Exception>> chain;
    
    // Add the main exception (create a copy)
    chain.push_back(std::make_shared<Exception>(ex));
    
    // Follow the cause chain
    auto current = ex.get_cause();
    while (current) {
        chain.push_back(current);
        current = current->get_cause();
    }
    
    return chain;
}

} // namespace runtime
} // namespace sad
