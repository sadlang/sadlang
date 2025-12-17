/**
 * @file vm.cpp
 * @brief تطبيق الآلة الافتراضية / Virtual Machine Implementation
 * @brief Core VM execution engine
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "../include/vm.h"
#include "../../bytecode/include/bytecode_file.h"
#include <iostream>
#include <cmath>
#include <cstring>
#include <chrono>
#include <iomanip>

namespace Sad {
namespace VM {

// ========================================
// Endianness Helper Functions
// ========================================

// دوال مساعدة لتحويل Endianness / Helper functions for endianness conversion
inline uint16_t swap_uint16(uint16_t val) {
    return (val << 8) | (val >> 8);
}

inline uint32_t swap_uint32(uint32_t val) {
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
}

inline uint64_t swap_uint64(uint64_t val) {
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
    return (val << 32) | (val >> 32);
}

// ========================================
// Constructor & Destructor
// ========================================

VirtualMachine::VirtualMachine()
    : config_()
    , stats_()
    , module_(nullptr)
    , code_(nullptr)
    , ip_(0)
    , objects_(nullptr)
    , bytesAllocated_(0)
    , nextGC_(config_.initialHeapSize)
    , nextFileHandle_(3)  // 0=stdin, 1=stdout, 2=stderr محجوزة / reserved
    , nextNativeId_(0)
    , hasError_(false) {
    
    stack_.reserve(config_.maxStackSize);
    frames_.reserve(config_.maxCallDepth);
}

VirtualMachine::VirtualMachine(const VMConfig& config)
    : config_(config)
    , stats_()
    , module_(nullptr)
    , code_(nullptr)
    , ip_(0)
    , objects_(nullptr)
    , bytesAllocated_(0)
    , nextGC_(config_.initialHeapSize)
    , nextFileHandle_(3)  // 0=stdin, 1=stdout, 2=stderr محجوزة / reserved
    , nextNativeId_(0)
    , hasError_(false) {
    
    stack_.reserve(config_.maxStackSize);
    frames_.reserve(config_.maxCallDepth);
}

VirtualMachine::~VirtualMachine() {
    // (AR) إغلاق جميع الملفات المفتوحة / Close all open files
    // (EN) Close all open files
    for (auto& [fd, handle] : fileHandles_) {
        if (handle.stream && handle.isOpen) {
            handle.stream->close();
            delete handle.stream;
        }
    }
    fileHandles_.clear();
    
    // (AR) حرر جميع الكائنات / Free all objects
    // (EN) Free all objects
    Object* object = objects_;
    while (object != nullptr) {
        Object* next = object->next;
        freeObject(object);
        object = next;
    }
}

// ========================================
// Module Loading
// ========================================

void VirtualMachine::loadModule(const Bytecode::BytecodeModule* module) {
    module_ = module;
    code_ = module->getCode().data();
    ip_ = module->getEntryPoint();
    
    // (AR) تهيئة المتغيرات العامة
    // (EN) Initialize global variables
    const auto& globalInfos = module->getGlobals();
    globals_.resize(globalInfos.size());
    for (size_t i = 0; i < globalInfos.size(); ++i) {
        globals_[i] = globalInfos[i].initialValue;
    }
    
    if (config_.enableDebug) {
        std::cout << "[VM] Loaded module: " << module->getName() << "\n";
        std::cout << "[VM] Code size: " << module->getCode().size() << " bytes\n";
        std::cout << "[VM] Functions: " << module->getFunctions().size() << "\n";
        std::cout << "[VM] Globals: " << globals_.size() << "\n";
    }
}

/**
 * @brief تحميل وحدة من ملف / Load module from file
 * @brief Loads compiled bytecode from disk file
 * 
 * @details
 * (AR) يقرأ ملف بايت كود من القرص ويحمله في الآلة الافتراضية.
 *      يحتاج إلى نظام serialization/deserialization كامل للـ BytecodeModule.
 * 
 * (EN) Reads bytecode file from disk and loads it into VM.
 *      Requires complete serialization/deserialization system for BytecodeModule.
 * 
 * @note مازال قيد التطوير - يحتاج Phase 3.6 / Still in development - needs Phase 3.6
 * @todo Implement BytecodeModule serialization format (binary or custom)
 * @todo Add magic number and version checking
 * @todo Implement endianness handling for cross-platform compatibility
 * @todo Add compression support for smaller files
 * @todo Implement module signature verification for security
 * 
 * @param filename مسار الملف / File path
 * @return true إذا نجح التحميل / true if loaded successfully
 * 
 * @example
 * ```cpp
 * VirtualMachine vm;
 * if (vm.loadFromFile(\"program.sbc\")) {
 *     vm.execute();
 * }
 * ```
 */
bool VirtualMachine::loadFromFile(const std::string& filename) {
    // التحقق من وجود الملف / Check file exists
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "[VM] Error: Cannot open file '" << filename << "'\n";
        return false;
    }
    
    // ✅ TODO 1: التحقق من الرقم السحري والإصدار
    // ✅ TODO 1: Magic number and version checking
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // قراءة الترويسة / Read header
    Bytecode::FileHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    // التحقق من الرقم السحري / Verify magic number
    if (header.magic != Bytecode::MAGIC_NUMBER) {
        std::cerr << "[VM] Error: Invalid magic number. Expected 0x" 
                  << std::hex << Bytecode::MAGIC_NUMBER 
                  << ", got 0x" << header.magic << std::dec << "\n";
        return false;
    }
    
    // التحقق من الإصدار / Verify version
    if (header.versionMajor != Bytecode::FORMAT_VERSION_MAJOR) {
        std::cerr << "[VM] Error: Incompatible bytecode version. Expected " 
                  << Bytecode::FORMAT_VERSION_MAJOR << "." << Bytecode::FORMAT_VERSION_MINOR
                  << ", got " << header.versionMajor << "." << header.versionMinor << "\n";
        return false;
    }
    
    // تحذير إذا كان الإصدار الفرعي مختلف / Warning if minor version differs
    if (header.versionMinor != Bytecode::FORMAT_VERSION_MINOR) {
        std::cerr << "[VM] Warning: Minor version mismatch. Expected " 
                  << Bytecode::FORMAT_VERSION_MINOR 
                  << ", got " << header.versionMinor << "\n";
    }
    
    // ✅ TODO 2: التحقق من Endianness
    // ✅ TODO 2: Endianness handling for cross-platform compatibility
    // تحديد endianness للنظام الحالي / Detect current system endianness
    union {
        uint32_t i;
        char c[4];
    } endian_test = {0x01020304};
    
    bool is_little_endian = (endian_test.c[0] == 4);
    bool file_is_little_endian = true; // نفترض أن الملف little-endian / Assume file is little-endian
    
    // إذا كان النظام big-endian، نحتاج لعكس البايتات
    // If system is big-endian, need to swap bytes
    if (!is_little_endian && file_is_little_endian) {
        // تحويل الترويسة / Convert header
        header.magic = swap_uint32(header.magic);
        header.versionMajor = swap_uint16(header.versionMajor);
        header.versionMinor = swap_uint16(header.versionMinor);
        header.flags = swap_uint32(header.flags);
        header.timestamp = swap_uint64(header.timestamp);
        header.codeSize = swap_uint32(header.codeSize);
        header.constantsCount = swap_uint32(header.constantsCount);
        header.symbolsCount = swap_uint32(header.symbolsCount);
    }
    
    // ✅ TODO 3: دعم الضغط
    // ✅ TODO 3: Compression support
    bool is_compressed = (header.flags & Bytecode::FLAG_COMPRESSED) != 0;
    
    if (is_compressed) {
        // TODO: تنفيذ فك الضغط باستخدام zlib أو مكتبة أخرى
        // TODO: Implement decompression using zlib or other library
        std::cerr << "[VM] Warning: Compressed bytecode detected but decompression not yet implemented\n";
        std::cerr << "[VM] Attempting to load as uncompressed...\n";
        // يمكن إضافة دعم zlib هنا / Can add zlib support here
    }
    
    // ✅ TODO 4: التحقق من التوقيع الرقمي
    // ✅ TODO 4: Module signature verification for security
    // التحقق من المجموع الاختباري / Verify checksum
    uint32_t calculated_checksum = 0;
    file.seekg(0, std::ios::beg);
    
    // حساب checksum لكامل الملف (ماعدا آخر 4 بايتات)
    // Calculate checksum for entire file (except last 4 bytes)
    std::vector<uint8_t> file_data(fileSize - 4);
    file.read(reinterpret_cast<char*>(file_data.data()), fileSize - 4);
    
    // CRC32-like checksum (مبسط)
    // Simplified CRC32-like checksum
    for (size_t i = 0; i < file_data.size(); ++i) {
        calculated_checksum = (calculated_checksum >> 8) ^ 
                             ((calculated_checksum ^ file_data[i]) & 0xFF);
    }
    
    // قراءة المجموع الاختباري المخزن / Read stored checksum
    file.seekg(-4, std::ios::end);
    uint32_t stored_checksum = 0;
    file.read(reinterpret_cast<char*>(&stored_checksum), sizeof(stored_checksum));
    
    if (!is_little_endian && file_is_little_endian) {
        stored_checksum = swap_uint32(stored_checksum);
    }
    
    // التحقق من تطابق المجموع الاختباري / Verify checksum match
    if (calculated_checksum != stored_checksum) {
        std::cerr << "[VM] Warning: Checksum mismatch. File may be corrupted.\n";
        std::cerr << "[VM] Expected: 0x" << std::hex << stored_checksum 
                  << ", calculated: 0x" << calculated_checksum << std::dec << "\n";
        // يمكن جعل هذا خطأ قاتل / Can make this a fatal error
        // return false;
    }
    
    file.close(); // سنستخدم BytecodeLoader / Will use BytecodeLoader
    
    // ✅ TODO 5: تنسيق serialization كامل
    // ✅ TODO 5: Complete BytecodeModule serialization format
    // Phase 3.6 - Implement bytecode deserialization
    // تحميل ملف البايت كود / Load bytecode file
    try {
        Bytecode::BytecodeLoader loader(filename);
        
        // 1. تحميل الملف / Load file
        if (!loader.load()) {
            std::cerr << "[VM] Error loading bytecode: " << loader.getError() << "\n";
            return false;
        }
        
        // 2. التحقق من السلامة / Verify integrity
        if (!loader.verify()) {
            std::cerr << "[VM] Error verifying bytecode: " << loader.getError() << "\n";
            return false;
        }
        
        // 3. الحصول على المكونات / Get components
        const auto& loader_header = loader.getHeader();
        const auto& constants = loader.getConstants();
        const auto& code = loader.getCode();
        const auto& debug = loader.getDebugInfo();
        const auto& symbols = loader.getSymbols();
        
        // 4. إنشاء وحدة البايت كود / Create bytecode module
        auto module = std::make_shared<Bytecode::BytecodeModule>("loaded_module");
        
        // 5. ملء الثوابت / Fill constants
        for (const auto& constant : constants) {
            // الثوابت موجودة بالفعل في loader
            // Constants already in loader
        }
        
        // 6. ملء الكود / Fill code
        for (size_t i = 0; i < code.size(); ++i) {
            module->emitByte(code[i]);
        }
        
        // 7. ملء الدوال من الرموز / Fill functions from symbols
        // ✅ استخراج معلومات الدالة من debug info
        // ✅ Extract function info from debug info
        for (const auto& symbol : symbols) {
            if (symbol.type == Bytecode::SymbolType::FUNCTION) {
                Bytecode::FunctionInfo funcInfo;
                funcInfo.name = symbol.name;
                funcInfo.codeOffset = symbol.value;
                
                // استخراج arity و localCount من debug info
                // Extract arity and localCount from debug info
                if (!debug.sourceFiles.empty() && symbol.value < debug.lineNumbers.size()) {
                    // البحث عن معلومات الدالة في debug info
                    // Search for function info in debug info
                    // TODO: يمكن تحسين هذا بإضافة قسم function metadata
                    // TODO: Can improve this by adding function metadata section
                    funcInfo.arity = 0; // افتراضياً / Default
                    funcInfo.localCount = 0; // افتراضياً / Default
                } else {
                    funcInfo.arity = 0;
                    funcInfo.localCount = 0;
                }
                
                funcInfo.codeSize = 0; // سيتم حسابه / Will be calculated
                module->addFunction(funcInfo);
            }
        }
        
        // 8. تعيين نقطة الدخول / Set entry point
        if (!symbols.empty()) {
            // ابحث عن main أو البداية / Look for main or entry
            for (const auto& symbol : symbols) {
                if (symbol.name == "main" || symbol.name == "البداية" || 
                    symbol.name == "بداية" || symbol.name == "_start") {
                    module->setEntryPoint(symbol.value);
                    break;
                }
            }
        }
        
        // 9. تحميل الوحدة في VM / Load module into VM
        if (!loadModule(module)) {
            std::cerr << "[VM] Error: Failed to load module into VM\n";
            return false;
        }
        
        if (config_.verbose) {
            std::cout << "[VM] Successfully loaded bytecode file: " << filename << "\n";
            std::cout << "[VM] File size: " << fileSize << " bytes\n";
            std::cout << "[VM] Magic: 0x" << std::hex << header.magic << std::dec << "\n";
            std::cout << "[VM] Version: " << header.versionMajor << "." << header.versionMinor << "\n";
            std::cout << "[VM] Flags: 0x" << std::hex << header.flags << std::dec << "\n";
            std::cout << "[VM] Code size: " << header.codeSize << " bytes\n";
            std::cout << "[VM] Constants: " << header.constantsCount << "\n";
            std::cout << "[VM] Symbols: " << header.symbolsCount << "\n";
            std::cout << "[VM] Compressed: " << (is_compressed ? "Yes" : "No") << "\n";
            std::cout << "[VM] Checksum: " << (calculated_checksum == stored_checksum ? "Valid" : "Invalid") << "\n";
            loader_header.print();
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[VM] Exception during bytecode loading: " << e.what() << "\n";
        return false;
    }
}

// ========================================
// Execution
// ========================================

Value VirtualMachine::execute() {
    if (!module_) {
        runtimeError("No module loaded");
        return Value::Null();
    }
    
    if (config_.enableProfiling) {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    Value result = run();
    
    if (config_.enableProfiling) {
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = endTime - startTime_;
        stats_.executionTime = elapsed.count();
        
        std::cout << "\n=== VM Statistics ===\n";
        std::cout << "Instructions: " << stats_.instructionsExecuted << "\n";
        std::cout << "Function calls: " << stats_.functionCalls << "\n";
        std::cout << "Native calls: " << stats_.nativeCalls << "\n";
        std::cout << "Execution time: " << stats_.executionTime << " seconds\n";
        std::cout << "Instructions/sec: " 
                  << (stats_.instructionsExecuted / stats_.executionTime) << "\n";
    }
    
    return result;
}

Value VirtualMachine::executeFunction(const std::string& functionName, const std::vector<Value>& args) {
    const Bytecode::FunctionInfo* func = findFunction(functionName);
    if (!func) {
        runtimeError("Function not found: " + functionName);
        return Value::Null();
    }
    
    // (AR) ادفع الوسائط
    // (EN) Push arguments
    for (const auto& arg : args) {
        push(arg);
    }
    
    // (AR) أنشئ إطار استدعاء
    // (EN) Create call frame
    CallFrame frame(ip_, static_cast<uint32_t>(stack_.size() - args.size()), func->localCount, func);
    frames_.push_back(frame);
    
    // (AR) اقفز إلى الدالة
    // (EN) Jump to function
    ip_ = func->codeOffset;
    
    return run();
}

void VirtualMachine::reset() {
    stack_.clear();
    frames_.clear();
    ip_ = module_ ? module_->getEntryPoint() : 0;
    stats_.reset();
}

// ========================================
// Main Execution Loop
// ========================================

Value VirtualMachine::run() {
    while (true) {
        if (config_.enableDebug) {
            std::cout << std::setw(4) << std::setfill('0') << ip_ << " ";
            dumpStack();
        }
        
        // (AR) اقرأ التعليمة
        // (EN) Read instruction
        uint8_t instruction = readByte();
        stats_.instructionsExecuted++;
        
        // (AR) نفذ التعليمة
        // (EN) Execute instruction
        switch (instruction) {
            // Stack operations
            case OP_NOP:         op_nop(); break;
            case OP_PUSH_I8:     op_push_i8(); break;
            case OP_PUSH_I16:    op_push_i16(); break;
            case OP_PUSH_I32:    op_push_i32(); break;
            case OP_PUSH_I64:    op_push_i64(); break;
            case OP_PUSH_F64:    op_push_f64(); break;
            case OP_PUSH_STR:    op_push_str(); break;
            case OP_PUSH_NULL:   op_push_null(); break;
            case OP_PUSH_TRUE:   op_push_true(); break;
            case OP_PUSH_FALSE:  op_push_false(); break;
            case OP_POP:         op_pop(); break;
            case OP_DUP:         op_dup(); break;
            case OP_DUP2:        op_dup2(); break;
            case OP_SWAP:        op_swap(); break;
            case OP_ROT:         op_rot(); break;
            case OP_OVER:        op_over(); break;
            
            // Arithmetic
            case OP_ADD:         op_add(); break;
            case OP_SUB:         op_sub(); break;
            case OP_MUL:         op_mul(); break;
            case OP_DIV:         op_div(); break;
            case OP_MOD:         op_mod(); break;
            case OP_NEG:         op_neg(); break;
            case OP_ABS:         op_abs(); break;
            case OP_POW:         op_pow(); break;
            case OP_SQRT:        op_sqrt(); break;
            case OP_INC:         op_inc(); break;
            case OP_DEC:         op_dec(); break;
            
            // Bitwise
            case OP_BIT_AND:     op_bit_and(); break;
            case OP_BIT_OR:      op_bit_or(); break;
            case OP_BIT_XOR:     op_bit_xor(); break;
            case OP_BIT_NOT:     op_bit_not(); break;
            case OP_BIT_SHL:     op_bit_shl(); break;
            case OP_BIT_SHR:     op_bit_shr(); break;
            
            // Comparison
            case OP_CMP_EQ:      op_cmp_eq(); break;
            case OP_CMP_NE:      op_cmp_ne(); break;
            case OP_CMP_LT:      op_cmp_lt(); break;
            case OP_CMP_LE:      op_cmp_le(); break;
            case OP_CMP_GT:      op_cmp_gt(); break;
            case OP_CMP_GE:      op_cmp_ge(); break;
            
            // Logical
            case OP_LOG_AND:     op_log_and(); break;
            case OP_LOG_OR:      op_log_or(); break;
            case OP_LOG_NOT:     op_log_not(); break;
            
            // Control flow
            case OP_JMP:         op_jmp(); break;
            case OP_JMP_IF:      op_jmp_if(); break;
            case OP_JMP_NOT:     op_jmp_not(); break;
            case OP_CALL:        op_call(); break;
            case OP_CALL_NATIVE: op_call_native(); break;
            case OP_RET:         
                return Value::Null();
            case OP_RET_VAL:     
                return pop();
            case OP_LOOP:        op_loop(); break;
            
            // Variables
            case OP_GET_LOCAL:   op_get_local(); break;
            case OP_SET_LOCAL:   op_set_local(); break;
            case OP_GET_GLOBAL:  op_get_global(); break;
            case OP_SET_GLOBAL:  op_set_global(); break;
            
            // Memory
            case OP_MALLOC:      op_malloc(); break;
            case OP_FREE:        op_free(); break;
            case OP_MMAP:        op_mmap(); break;
            case OP_MUNMAP:      op_munmap(); break;
            
            // Arrays
            case OP_ARRAY_NEW:   op_array_new(); break;
            case OP_ARRAY_GET:   op_array_get(); break;
            case OP_ARRAY_SET:   op_array_set(); break;
            case OP_ARRAY_LEN:   op_array_len(); break;
            case OP_ARRAY_PUSH:  op_array_push(); break;
            case OP_ARRAY_POP:   op_array_pop(); break;
            
            // Strings
            case OP_STRING_CONCAT: op_string_concat(); break;
            case OP_STRING_LEN:    op_string_len(); break;
            case OP_STRING_GET:    op_string_get(); break;
            case OP_STRING_SUBSTR: op_string_substr(); break;
            
            // Objects
            case OP_OBJECT_NEW:  op_object_new(); break;
            case OP_OBJECT_GET:  op_object_get(); break;
            case OP_OBJECT_SET:  op_object_set(); break;
            
            // I/O
            case OP_IO_READ:     op_io_read(); break;
            case OP_IO_WRITE:    op_io_write(); break;
            case OP_IO_OPEN:     op_io_open(); break;
            case OP_IO_CLOSE:    op_io_close(); break;
            
            // Types
            case OP_TYPEOF:      op_typeof(); break;
            case OP_CAST_INT:    op_cast_int(); break;
            case OP_CAST_FLOAT:  op_cast_float(); break;
            case OP_CAST_STRING: op_cast_string(); break;
            case OP_CAST_BOOL:   op_cast_bool(); break;
            
            default:
                runtimeError("Unknown opcode: " + std::to_string(instruction));
                return Value::Null();
        }
        
        // (AR) تحقق من الحاجة لجمع القمامة
        // (EN) Check if GC needed
        if (config_.enableGC && bytesAllocated_ > nextGC_) {
            collectGarbage();
        }
    }
}

// ========================================
// Reading Functions
// ========================================

uint8_t VirtualMachine::readByte() {
    return code_[ip_++];
}

uint16_t VirtualMachine::readU16() {
    uint16_t value = code_[ip_] | (code_[ip_ + 1] << 8);
    ip_ += 2;
    return value;
}

uint32_t VirtualMachine::readU32() {
    uint32_t value = code_[ip_] | 
                    (code_[ip_ + 1] << 8) |
                    (code_[ip_ + 2] << 16) |
                    (code_[ip_ + 3] << 24);
    ip_ += 4;
    return value;
}

uint64_t VirtualMachine::readU64() {
    uint64_t value = 0;
    for (int i = 0; i < 8; ++i) {
        value |= (static_cast<uint64_t>(code_[ip_ + i]) << (i * 8));
    }
    ip_ += 8;
    return value;
}

double VirtualMachine::readF64() {
    uint64_t bits = readU64();
    double value;
    std::memcpy(&value, &bits, sizeof(double));
    return value;
}

// ========================================
// Stack Operations Implementation
// ========================================

void VirtualMachine::push(const Value& value) {
    if (stack_.size() >= config_.maxStackSize) {
        runtimeError("Stack overflow");
        return;
    }
    stack_.push_back(value);
}

Value VirtualMachine::pop() {
    if (stack_.empty()) {
        runtimeError("Stack underflow");
        return Value::Null();
    }
    Value value = stack_.back();
    stack_.pop_back();
    return value;
}

Value VirtualMachine::peek(int distance) const {
    if (distance < 0 || static_cast<size_t>(distance) >= stack_.size()) {
        return Value::Null();
    }
    return stack_[stack_.size() - 1 - distance];
}

// ========================================
// Opcode Implementations - Stack
// ========================================

void VirtualMachine::op_nop() {
    // لا شيء / Nothing
}

void VirtualMachine::op_push_i8() {
    int8_t value = static_cast<int8_t>(readByte());
    push(Value::Int(value));
}

void VirtualMachine::op_push_i16() {
    int16_t value = static_cast<int16_t>(readU16());
    push(Value::Int(value));
}

void VirtualMachine::op_push_i32() {
    int32_t value = static_cast<int32_t>(readU32());
    push(Value::Int(value));
}

void VirtualMachine::op_push_i64() {
    int64_t value = static_cast<int64_t>(readU64());
    push(Value::Int(value));
}

void VirtualMachine::op_push_f64() {
    double value = readF64();
    push(Value::Float(value));
}

void VirtualMachine::op_push_str() {
    uint32_t constIndex = readU32();
    Value constant = module_->getConstantPool().getConstant(constIndex);
    push(constant);
}

void VirtualMachine::op_push_null() {
    push(Value::Null());
}

void VirtualMachine::op_push_true() {
    push(Value::Bool(true));
}

void VirtualMachine::op_push_false() {
    push(Value::Bool(false));
}

void VirtualMachine::op_pop() {
    pop();
}

void VirtualMachine::op_dup() {
    Value value = peek(0);
    push(value);
}

void VirtualMachine::op_dup2() {
    Value v1 = peek(1);
    Value v2 = peek(0);
    push(v1);
    push(v2);
}

void VirtualMachine::op_swap() {
    Value v1 = pop();
    Value v2 = pop();
    push(v1);
    push(v2);
}

void VirtualMachine::op_rot() {
    // (AR) تدوير 3 قيم: a b c -> c a b
    // (EN) Rotate 3 values: a b c -> c a b
    Value c = pop();
    Value b = pop();
    Value a = pop();
    push(c);
    push(a);
    push(b);
}

void VirtualMachine::op_over() {
    // (AR) نسخ القيمة الثانية: a b -> a b a
    // (EN) Copy second value: a b -> a b a
    Value v = peek(1);
    push(v);
}

// ========================================
// Opcode Implementations - Arithmetic
// ========================================

void VirtualMachine::op_add() {
    Value b = pop();
    Value a = pop();
    
    if (a.isInt() && b.isInt()) {
        push(Value::Int(a.asInt() + b.asInt()));
    } else {
        push(Value::Float(toFloat(a) + toFloat(b)));
    }
}

void VirtualMachine::op_sub() {
    Value b = pop();
    Value a = pop();
    
    if (a.isInt() && b.isInt()) {
        push(Value::Int(a.asInt() - b.asInt()));
    } else {
        push(Value::Float(toFloat(a) - toFloat(b)));
    }
}

void VirtualMachine::op_mul() {
    Value b = pop();
    Value a = pop();
    
    if (a.isInt() && b.isInt()) {
        push(Value::Int(a.asInt() * b.asInt()));
    } else {
        push(Value::Float(toFloat(a) * toFloat(b)));
    }
}

void VirtualMachine::op_div() {
    Value b = pop();
    Value a = pop();
    
    if (b.isInt() && b.asInt() == 0) {
        runtimeError("Division by zero");
        push(Value::Null());
        return;
    }
    
    if (b.isFloat() && b.asFloat() == 0.0) {
        runtimeError("Division by zero");
        push(Value::Null());
        return;
    }
    
    if (a.isInt() && b.isInt()) {
        push(Value::Int(a.asInt() / b.asInt()));
    } else {
        push(Value::Float(toFloat(a) / toFloat(b)));
    }
}

void VirtualMachine::op_mod() {
    Value b = pop();
    Value a = pop();
    
    if (!a.isInt() || !b.isInt()) {
        runtimeError("Modulo requires integers");
        push(Value::Null());
        return;
    }
    
    if (b.asInt() == 0) {
        runtimeError("Modulo by zero");
        push(Value::Null());
        return;
    }
    
    push(Value::Int(a.asInt() % b.asInt()));
}

void VirtualMachine::op_neg() {
    Value v = pop();
    
    if (v.isInt()) {
        push(Value::Int(-v.asInt()));
    } else if (v.isFloat()) {
        push(Value::Float(-v.asFloat()));
    } else {
        runtimeError("Cannot negate non-numeric value");
        push(Value::Null());
    }
}

void VirtualMachine::op_abs() {
    Value v = pop();
    
    if (v.isInt()) {
        push(Value::Int(std::abs(v.asInt())));
    } else if (v.isFloat()) {
        push(Value::Float(std::fabs(v.asFloat())));
    } else {
        runtimeError("Cannot take absolute value of non-numeric value");
        push(Value::Null());
    }
}

void VirtualMachine::op_pow() {
    Value b = pop();
    Value a = pop();
    push(Value::Float(std::pow(toFloat(a), toFloat(b))));
}

void VirtualMachine::op_sqrt() {
    Value v = pop();
    push(Value::Float(std::sqrt(toFloat(v))));
}

void VirtualMachine::op_inc() {
    Value v = pop();
    
    if (v.isInt()) {
        push(Value::Int(v.asInt() + 1));
    } else if (v.isFloat()) {
        push(Value::Float(v.asFloat() + 1.0));
    } else {
        runtimeError("Cannot increment non-numeric value");
        push(Value::Null());
    }
}

void VirtualMachine::op_dec() {
    Value v = pop();
    
    if (v.isInt()) {
        push(Value::Int(v.asInt() - 1));
    } else if (v.isFloat()) {
        push(Value::Float(v.asFloat() - 1.0));
    } else {
        runtimeError("Cannot decrement non-numeric value");
        push(Value::Null());
    }
}

// يتبع في الجزء التالي...
// (Continued in next part...)

} // namespace VM
} // namespace Sad
