/**
 * @file bytecode.cpp
 * @brief تطبيق حاوية البايت كود / Bytecode Container Implementation
 * @brief Implementation of bytecode storage and management
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "bytecode/bytecode.h"
#include <iostream>
#include <iomanip>
#include <cstring>

namespace Sad {
namespace Compiler {
namespace Bytecode {

// ========================================
// Constant Pool Implementation
// ========================================

uint32_t ConstantPool::addInteger(int64_t value) {
    // (AR) تحقق من وجود القيمة مسبقاً
    // (EN) Check if value already exists
    auto it = intMap_.find(value);
    if (it != intMap_.end()) {
        return it->second;
    }
    
    // (AR) أضف قيمة جديدة
    // (EN) Add new value
    uint32_t index = static_cast<uint32_t>(constants_.size());
    constants_.push_back(VM::Value::Int(value));
    intMap_[value] = index;
    return index;
}

uint32_t ConstantPool::addFloat(double value) {
    auto it = floatMap_.find(value);
    if (it != floatMap_.end()) {
        return it->second;
    }
    
    uint32_t index = static_cast<uint32_t>(constants_.size());
    constants_.push_back(VM::Value::Float(value));
    floatMap_[value] = index;
    return index;
}

uint32_t ConstantPool::addString(const std::string& str) {
    auto it = stringMap_.find(str);
    if (it != stringMap_.end()) {
        return it->second;
    }
    
    uint32_t index = static_cast<uint32_t>(constants_.size());
    
    // (AR) إنشاء كائن نص
    // (EN) Create string object
    auto* strObj = new VM::StringObject();
    strObj->length = str.length();
    strObj->chars = new char[str.length() + 1];
    std::memcpy(strObj->chars, str.c_str(), str.length() + 1);
    
    // (AR) حساب hash
    // (EN) Calculate hash
    strObj->hash = std::hash<std::string>{}(str);
    
    constants_.push_back(VM::Value::String(strObj));
    stringMap_[str] = index;
    return index;
}

VM::Value ConstantPool::getConstant(uint32_t index) const {
    if (index >= constants_.size()) {
        return VM::Value::Null();
    }
    return constants_[index];
}

void ConstantPool::clear() {
    // (AR) حرر كائنات النصوص
    // (EN) Free string objects
    for (const auto& val : constants_) {
        if (val.isString()) {
            delete val.asString();
        }
    }
    
    constants_.clear();
    intMap_.clear();
    floatMap_.clear();
    stringMap_.clear();
}

// ========================================
// Bytecode Module Implementation
// ========================================

BytecodeModule::BytecodeModule(const std::string& name)
    : name_(name), entryPoint_(0) {
}

void BytecodeModule::emitByte(Byte byte) {
    code_.push_back(byte);
}

void BytecodeModule::emitBytes(const std::vector<Byte>& bytes) {
    code_.insert(code_.end(), bytes.begin(), bytes.end());
}

void BytecodeModule::emitU16(uint16_t value) {
    // Little-endian encoding
    emitByte(static_cast<Byte>(value & 0xFF));
    emitByte(static_cast<Byte>((value >> 8) & 0xFF));
}

void BytecodeModule::emitU32(uint32_t value) {
    emitByte(static_cast<Byte>(value & 0xFF));
    emitByte(static_cast<Byte>((value >> 8) & 0xFF));
    emitByte(static_cast<Byte>((value >> 16) & 0xFF));
    emitByte(static_cast<Byte>((value >> 24) & 0xFF));
}

void BytecodeModule::emitU64(uint64_t value) {
    for (int i = 0; i < 8; ++i) {
        emitByte(static_cast<Byte>((value >> (i * 8)) & 0xFF));
    }
}

void BytecodeModule::emitF64(double value) {
    uint64_t bits;
    std::memcpy(&bits, &value, sizeof(double));
    emitU64(bits);
}

void BytecodeModule::patchByte(uint32_t offset, Byte byte) {
    if (offset < code_.size()) {
        code_[offset] = byte;
    }
}

void BytecodeModule::patchU32(uint32_t offset, uint32_t value) {
    if (offset + 3 < code_.size()) {
        code_[offset] = static_cast<Byte>(value & 0xFF);
        code_[offset + 1] = static_cast<Byte>((value >> 8) & 0xFF);
        code_[offset + 2] = static_cast<Byte>((value >> 16) & 0xFF);
        code_[offset + 3] = static_cast<Byte>((value >> 24) & 0xFF);
    }
}

void BytecodeModule::addLineInfo(uint32_t lineNumber) {
    uint32_t offset = getCurrentOffset();
    
    // (AR) لا تضف إذا كان نفس السطر
    // (EN) Don't add if same line
    if (!lineInfo_.empty() && lineInfo_.back().lineNumber == lineNumber) {
        return;
    }
    
    lineInfo_.emplace_back(offset, lineNumber);
}

void BytecodeModule::addFunction(const FunctionInfo& func) {
    functions_.push_back(func);
}

void BytecodeModule::addGlobal(const GlobalInfo& global) {
    globals_.push_back(global);
}

int BytecodeModule::getFunctionIndex(const std::string& name) const {
    // (AR) البحث عن الدالة بالاسم / (EN) Search for function by name
    for (size_t i = 0; i < functions_.size(); ++i) {
        if (functions_[i].name == name) {
            return static_cast<int>(i);
        }
    }
    return -1; // (AR) لم يتم العثور / (EN) Not found
}

int BytecodeModule::getBuiltinId(const std::string& name) const {
    // (AR) البحث في خريطة الدوال المدمجة / (EN) Search builtin map
    auto it = builtinMap_.find(name);
    if (it != builtinMap_.end()) {
        return static_cast<int>(it->second);
    }
    return -1; // (AR) لم يتم العثور / (EN) Not found
}

void BytecodeModule::registerBuiltin(const std::string& name, uint32_t id) {
    // (AR) تسجيل دالة مدمجة / (EN) Register builtin function
    builtinMap_[name] = id;
}

void BytecodeModule::clear() {
    code_.clear();
    constantPool_.clear();
    functions_.clear();
    globals_.clear();
    lineInfo_.clear();
    entryPoint_ = 0;
    builtinMap_.clear();
}

void BytecodeModule::disassemble() const {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════╗\n";
    std::cout << "║  Bytecode Disassembly / تفكيك البايت كود          ║\n";
    std::cout << "╠════════════════════════════════════════════════════╣\n";
    std::cout << "║  Module: " << std::left << std::setw(39) << name_ << " ║\n";
    std::cout << "║  Code Size: " << std::setw(34) << code_.size() << " bytes ║\n";
    std::cout << "║  Constants: " << std::setw(36) << constantPool_.size() << " ║\n";
    std::cout << "║  Functions: " << std::setw(36) << functions_.size() << " ║\n";
    std::cout << "║  Globals: " << std::setw(38) << globals_.size() << " ║\n";
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
    
    // (AR) طباعة الثوابت
    // (EN) Print constants
    if (!constantPool_.getConstants().empty()) {
        std::cout << "Constants:\n";
        for (size_t i = 0; i < constantPool_.size(); ++i) {
            std::cout << "  [" << i << "] ";
            VM::printValue(constantPool_.getConstant(static_cast<uint32_t>(i)));
            std::cout << "\n";
        }
        std::cout << "\n";
    }
    
    // (AR) طباعة الدوال
    // (EN) Print functions
    if (!functions_.empty()) {
        std::cout << "Functions:\n";
        for (const auto& func : functions_) {
            std::cout << "  " << func.name << " (arity: " << func.arity 
                      << ", locals: " << func.localCount << ")\n";
        }
        std::cout << "\n";
    }
    
    // (AR) طباعة الكود
    // (EN) Print code
    std::cout << "Code:\n";
    for (uint32_t offset = 0; offset < code_.size();) {
        offset += disassembleInstruction(code_, offset, constantPool_);
    }
}

// ========================================
// Disassembler Implementation
// ========================================

int disassembleInstruction(
    const std::vector<Byte>& code,
    uint32_t offset,
    const ConstantPool& constantPool) {
    
    if (offset >= code_.size()) {
        return 0;
    }
    
    // (AR) طباعة الإزاحة
    // (EN) Print offset
    std::cout << std::setw(4) << std::setfill('0') << offset << " ";
    std::cout << std::setfill(' ');
    
    VM::Opcode op = code[offset];
    const char* name = VM::getOpcodeName(op);
    
    std::cout << std::left << std::setw(20) << name;
    
    int size = VM::getOperandSize(op);
    
    // (AR) طباعة المعاملات
    // (EN) Print operands
    if (size > 0 && offset + size < code.size()) {
        std::cout << " ";
        
        switch (size) {
            case 1: {
                uint8_t operand = code[offset + 1];
                std::cout << static_cast<int>(operand);
                break;
            }
            case 2: {
                uint16_t operand = code[offset + 1] | (code[offset + 2] << 8);
                std::cout << operand;
                break;
            }
            case 4: {
                uint32_t operand = code[offset + 1] | 
                                  (code[offset + 2] << 8) |
                                  (code[offset + 3] << 16) |
                                  (code[offset + 4] << 24);
                std::cout << operand;
                
                // (AR) إذا كانت تعليمة ثابت، اطبع القيمة
                // (EN) If constant instruction, print value
                if (op == VM::OP_PUSH_STR) {
                    std::cout << " (";
                    VM::printValue(constantPool.getConstant(operand));
                    std::cout << ")";
                }
                break;
            }
            case 8: {
                uint64_t operand = 0;
                for (int i = 0; i < 8; ++i) {
                    operand |= (static_cast<uint64_t>(code[offset + 1 + i]) << (i * 8));
                }
                
                if (op == VM::OP_PUSH_F64) {
                    double value;
                    std::memcpy(&value, &operand, sizeof(double));
                    std::cout << value;
                } else {
                    std::cout << operand;
                }
                break;
            }
        }
    }
    
    std::cout << "\n";
    
    return 1 + size;
}

void disassembleModule(const BytecodeModule& module) {
    module.disassemble();
}

} // namespace Bytecode
} // namespace Compiler
} // namespace Sad
