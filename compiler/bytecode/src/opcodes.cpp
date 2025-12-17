/**
 * @file opcodes.cpp
 * @brief تطبيق دوال أكواد البايت كود / Bytecode Opcodes Implementation
 * @brief Implementation of opcode utility functions
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "../include/opcodes.h"
#include <unordered_map>

namespace Sad {
namespace VM {

// ========================================
// Opcode Name Mapping
// خريطة أسماء الأكواد
// ========================================

static const std::unordered_map<Opcode, const char*> opcodeNames = {
    // Stack Operations
    {OP_NOP, "OP_NOP"},
    {OP_PUSH_I8, "OP_PUSH_I8"},
    {OP_PUSH_I16, "OP_PUSH_I16"},
    {OP_PUSH_I32, "OP_PUSH_I32"},
    {OP_PUSH_I64, "OP_PUSH_I64"},
    {OP_PUSH_F64, "OP_PUSH_F64"},
    {OP_PUSH_STR, "OP_PUSH_STR"},
    {OP_PUSH_NULL, "OP_PUSH_NULL"},
    {OP_PUSH_TRUE, "OP_PUSH_TRUE"},
    {OP_PUSH_FALSE, "OP_PUSH_FALSE"},
    {OP_POP, "OP_POP"},
    {OP_DUP, "OP_DUP"},
    {OP_DUP2, "OP_DUP2"},
    {OP_SWAP, "OP_SWAP"},
    {OP_ROT, "OP_ROT"},
    {OP_OVER, "OP_OVER"},
    
    // Arithmetic
    {OP_ADD, "OP_ADD"},
    {OP_SUB, "OP_SUB"},
    {OP_MUL, "OP_MUL"},
    {OP_DIV, "OP_DIV"},
    {OP_MOD, "OP_MOD"},
    {OP_NEG, "OP_NEG"},
    {OP_ABS, "OP_ABS"},
    {OP_POW, "OP_POW"},
    {OP_SQRT, "OP_SQRT"},
    {OP_INC, "OP_INC"},
    {OP_DEC, "OP_DEC"},
    
    // Bitwise
    {OP_AND, "OP_AND"},
    {OP_OR, "OP_OR"},
    {OP_XOR, "OP_XOR"},
    {OP_NOT, "OP_NOT"},
    {OP_SHL, "OP_SHL"},
    {OP_SHR, "OP_SHR"},
    {OP_SAR, "OP_SAR"},
    
    // Comparison
    {OP_EQ, "OP_EQ"},
    {OP_NE, "OP_NE"},
    {OP_LT, "OP_LT"},
    {OP_LE, "OP_LE"},
    {OP_GT, "OP_GT"},
    {OP_GE, "OP_GE"},
    
    // Control Flow
    {OP_JMP, "OP_JMP"},
    {OP_JMP_IF, "OP_JMP_IF"},
    {OP_JMP_NOT, "OP_JMP_NOT"},
    {OP_CALL, "OP_CALL"},
    {OP_CALL_NATIVE, "OP_CALL_NATIVE"},
    {OP_RET, "OP_RET"},
    {OP_RET_VAL, "OP_RET_VAL"},
    {OP_LOOP, "OP_LOOP"},
    
    // Variables
    {OP_LOAD_LOCAL, "OP_LOAD_LOCAL"},
    {OP_STORE_LOCAL, "OP_STORE_LOCAL"},
    {OP_LOAD_GLOBAL, "OP_LOAD_GLOBAL"},
    {OP_STORE_GLOBAL, "OP_STORE_GLOBAL"},
    {OP_LOAD_CAPTURE, "OP_LOAD_CAPTURE"},
    {OP_STORE_CAPTURE, "OP_STORE_CAPTURE"},
    {OP_LOAD_UPVALUE, "OP_LOAD_UPVALUE"},
    {OP_CLOSE_UPVALUE, "OP_CLOSE_UPVALUE"},
    
    // Memory
    {OP_ALLOC, "OP_ALLOC"},
    {OP_FREE, "OP_FREE"},
    {OP_LOAD_PTR, "OP_LOAD_PTR"},
    {OP_STORE_PTR, "OP_STORE_PTR"},
    {OP_ADDR_OF, "OP_ADDR_OF"},
    {OP_DEREF, "OP_DEREF"},
    {OP_WRITE_TO, "OP_WRITE_TO"},
    
    // Arrays
    {OP_ARRAY_NEW, "OP_ARRAY_NEW"},
    {OP_ARRAY_GET, "OP_ARRAY_GET"},
    {OP_ARRAY_SET, "OP_ARRAY_SET"},
    {OP_ARRAY_LEN, "OP_ARRAY_LEN"},
    {OP_ARRAY_PUSH, "OP_ARRAY_PUSH"},
    {OP_ARRAY_POP, "OP_ARRAY_POP"},
    {OP_ARRAY_SLICE, "OP_ARRAY_SLICE"},
    
    // Objects
    {OP_OBJ_NEW, "OP_OBJ_NEW"},
    {OP_OBJ_GET, "OP_OBJ_GET"},
    {OP_OBJ_SET, "OP_OBJ_SET"},
    {OP_OBJ_HAS, "OP_OBJ_HAS"},
    {OP_OBJ_DEL, "OP_OBJ_DEL"},
    
    // Builtins
    {OP_PRINT, "OP_PRINT"},
    {OP_PRINT_AR, "OP_PRINT_AR"},
    {OP_INPUT, "OP_INPUT"},
    {OP_LEN, "OP_LEN"},
    {OP_TYPE, "OP_TYPE"},
    {OP_STR, "OP_STR"},
    {OP_INT, "OP_INT"},
    {OP_FLOAT, "OP_FLOAT"},
    {OP_BOOL, "OP_BOOL"},
    
    // Type checks
    {OP_IS_NULL, "OP_IS_NULL"},
    {OP_IS_BOOL, "OP_IS_BOOL"},
    {OP_IS_INT, "OP_IS_INT"},
    {OP_IS_FLOAT, "OP_IS_FLOAT"},
    {OP_IS_STR, "OP_IS_STR"},
    {OP_IS_ARRAY, "OP_IS_ARRAY"},
    {OP_IS_OBJ, "OP_IS_OBJ"},
    {OP_IS_PTR, "OP_IS_PTR"},
    {OP_CAST, "OP_CAST"},
    
    // Stage 1
    {OP_MALLOC, "OP_MALLOC"},
    {OP_MMAP, "OP_MMAP"},
    {OP_IO_READ, "OP_IO_READ"},
    {OP_IO_WRITE, "OP_IO_WRITE"},
    {OP_INT_GET, "OP_INT_GET"},
    {OP_INT_SET, "OP_INT_SET"},
    
    // Special
    {OP_HALT, "OP_HALT"},
    {OP_ASSERT, "OP_ASSERT"},
    {OP_DEBUG, "OP_DEBUG"},
    {OP_GC_COLLECT, "OP_GC_COLLECT"},
    {OP_IMPORT, "OP_IMPORT"},
    {OP_EXPORT, "OP_EXPORT"},
    {OP_UNREACHABLE, "OP_UNREACHABLE"},
};

const char* getOpcodeName(Opcode op) {
    auto it = opcodeNames.find(op);
    if (it != opcodeNames.end()) {
        return it->second;
    }
    return "UNKNOWN";
}

// ========================================
// Operand Sizes
// أحجام المعاملات
// ========================================

int getOperandSize(Opcode op) {
    switch (op) {
        // No operands
        case OP_NOP:
        case OP_PUSH_NULL:
        case OP_PUSH_TRUE:
        case OP_PUSH_FALSE:
        case OP_POP:
        case OP_DUP:
        case OP_DUP2:
        case OP_SWAP:
        case OP_ROT:
        case OP_OVER:
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_MOD:
        case OP_NEG:
        case OP_ABS:
        case OP_POW:
        case OP_SQRT:
        case OP_INC:
        case OP_DEC:
        case OP_AND:
        case OP_OR:
        case OP_XOR:
        case OP_NOT:
        case OP_SHL:
        case OP_SHR:
        case OP_SAR:
        case OP_EQ:
        case OP_NE:
        case OP_LT:
        case OP_LE:
        case OP_GT:
        case OP_GE:
        case OP_RET:
        case OP_RET_VAL:
        case OP_LOAD_PTR:
        case OP_STORE_PTR:
        case OP_DEREF:
        case OP_WRITE_TO:
        case OP_ARRAY_GET:
        case OP_ARRAY_SET:
        case OP_ARRAY_LEN:
        case OP_ARRAY_PUSH:
        case OP_ARRAY_POP:
        case OP_PRINT:
        case OP_PRINT_AR:
        case OP_INPUT:
        case OP_LEN:
        case OP_TYPE:
        case OP_STR:
        case OP_INT:
        case OP_FLOAT:
        case OP_BOOL:
        case OP_IS_NULL:
        case OP_IS_BOOL:
        case OP_IS_INT:
        case OP_IS_FLOAT:
        case OP_IS_STR:
        case OP_IS_ARRAY:
        case OP_IS_OBJ:
        case OP_IS_PTR:
        case OP_HALT:
        case OP_ASSERT:
        case OP_DEBUG:
        case OP_GC_COLLECT:
        case OP_UNREACHABLE:
            return 0;
        
        // 1 byte operand
        case OP_PUSH_I8:
        case OP_LOAD_LOCAL:
        case OP_STORE_LOCAL:
        case OP_LOAD_CAPTURE:
        case OP_STORE_CAPTURE:
        case OP_LOAD_UPVALUE:
        case OP_CLOSE_UPVALUE:
        case OP_CAST:
            return 1;
        
        // 2 byte operand
        case OP_PUSH_I16:
            return 2;
        
        // 4 byte operand
        case OP_PUSH_I32:
        case OP_PUSH_STR:
        case OP_JMP:
        case OP_JMP_IF:
        case OP_JMP_NOT:
        case OP_CALL:
        case OP_CALL_NATIVE:
        case OP_LOOP:
        case OP_LOAD_GLOBAL:
        case OP_STORE_GLOBAL:
        case OP_ALLOC:
        case OP_FREE:
        case OP_ADDR_OF:
        case OP_ARRAY_NEW:
        case OP_ARRAY_SLICE:
        case OP_OBJ_NEW:
        case OP_OBJ_GET:
        case OP_OBJ_SET:
        case OP_OBJ_HAS:
        case OP_OBJ_DEL:
        case OP_MALLOC:
        case OP_IO_READ:
        case OP_IO_WRITE:
        case OP_INT_GET:
        case OP_INT_SET:
        case OP_IMPORT:
        case OP_EXPORT:
            return 4;
        
        // 8 byte operand
        case OP_PUSH_I64:
        case OP_PUSH_F64:
        case OP_MMAP:
            return 8;
        
        default:
            return 0;
    }
}

} // namespace VM
} // namespace Sad
