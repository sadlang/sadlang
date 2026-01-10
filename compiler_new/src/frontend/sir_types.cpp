// ======================================================================
// sir_types.cpp - تنفيذ أنواع SIR
//                SIR Types Implementation
// ======================================================================
// الوصف بالعربية:
//   تنفيذ الدوال المساعدة لأنواع SIR
//
// English Description:
//   Implementation of SIR types helper functions
//
// الإصدار / Version: 1.0
// التاريخ / Date: December 2, 2025
// ======================================================================

#include "frontend/sir_types.h"
#include <sstream>

namespace Sad {
namespace Compiler {
namespace SIR {

// ======================================================================
// تحويل SIROpcode إلى نص / Convert SIROpcode to string
// ======================================================================

const char* sirOpcodeToString(SIROpcode opcode) {
    switch (opcode) {
        // Arithmetic
        case SIROpcode::ADD_I64:    return "add.i64";
        case SIROpcode::ADD_F64:    return "add.f64";
        case SIROpcode::SUB_I64:    return "sub.i64";
        case SIROpcode::SUB_F64:    return "sub.f64";
        case SIROpcode::MUL_I64:    return "mul.i64";
        case SIROpcode::MUL_F64:    return "mul.f64";
        case SIROpcode::DIV_I64:    return "div.i64";
        case SIROpcode::DIV_F64:    return "div.f64";
        case SIROpcode::MOD_I64:    return "mod.i64";
        case SIROpcode::NEG:        return "neg";
        
        // Bitwise
        case SIROpcode::AND:        return "and";
        case SIROpcode::OR:         return "or";
        case SIROpcode::XOR:        return "xor";
        case SIROpcode::NOT:        return "not";
        case SIROpcode::SHL:        return "shl";
        case SIROpcode::SHR:        return "shr";
        case SIROpcode::SAR:        return "sar";
        case SIROpcode::ROL:        return "rol";
        
        // Comparison
        case SIROpcode::EQ:         return "eq";
        case SIROpcode::NE:         return "ne";
        case SIROpcode::LT:         return "lt";
        case SIROpcode::LE:         return "le";
        case SIROpcode::GT:         return "gt";
        case SIROpcode::GE:         return "ge";
        
        // Control Flow
        case SIROpcode::BR:         return "br";
        case SIROpcode::BR_COND:    return "br.cond";
        case SIROpcode::RET:        return "ret";
        case SIROpcode::RET_VOID:   return "ret.void";
        case SIROpcode::CALL:       return "call";
        case SIROpcode::CALL_INDIRECT: return "call.indirect";
        case SIROpcode::SWITCH:     return "switch";
        case SIROpcode::PHI:        return "phi";
        
        // Memory
        case SIROpcode::ALLOC:      return "alloc";
        case SIROpcode::ALLOC_HEAP: return "alloc.heap";
        case SIROpcode::FREE:       return "free";
        case SIROpcode::LOAD:       return "load";
        case SIROpcode::STORE:      return "store";
        case SIROpcode::MOVE:       return "move";
        case SIROpcode::ADDR:       return "addr";
        case SIROpcode::PTR_ADD:    return "ptr.add";
        case SIROpcode::PTR_CAST:   return "ptr.cast";
        case SIROpcode::MEMCPY:     return "memcpy";
        case SIROpcode::MEMSET:     return "memset";
        
        // Array
        case SIROpcode::ARRAY_NEW:  return "array.new";
        case SIROpcode::ARRAY_GET:  return "array.get";
        case SIROpcode::ARRAY_SET:  return "array.set";
        case SIROpcode::ARRAY_LEN:  return "array.len";
        case SIROpcode::ARRAY_APPEND: return "array.append";
        case SIROpcode::ARRAY_REMOVE: return "array.remove";
        
        // String
        case SIROpcode::STRING_NEW: return "string.new";
        case SIROpcode::STRING_LEN: return "string.len";
        case SIROpcode::STRING_CONCAT: return "string.concat";
        case SIROpcode::STRING_CMP: return "string.cmp";
        case SIROpcode::STRING_SUBSTR: return "string.substr";
        case SIROpcode::STRING_FIND: return "string.find";
        case SIROpcode::STRING_REPLACE: return "string.replace";
        case SIROpcode::STRING_TO_I64: return "string.to_i64";
        
        // OOP
        case SIROpcode::OBJECT_NEW: return "object.new";
        case SIROpcode::OBJECT_GET: return "object.get";
        case SIROpcode::OBJECT_SET: return "object.set";
        case SIROpcode::OBJECT_CALL: return "object.call";
        case SIROpcode::INSTANCEOF: return "instanceof";
        case SIROpcode::OBJECT_CAST: return "object.cast";
        case SIROpcode::CLASS_DEF:  return "class.def";
        case SIROpcode::METHOD_DEF: return "method.def";
        case SIROpcode::FIELD_DEF:  return "field.def";
        case SIROpcode::CONSTRUCTOR_CALL: return "constructor.call";
        
        // Type Conversion
        case SIROpcode::I64_TO_F64: return "i64.to.f64";
        case SIROpcode::F64_TO_I64: return "f64.to.i64";
        case SIROpcode::I64_TO_BOOL: return "i64.to.bool";
        case SIROpcode::BOOL_TO_I64: return "bool.to.i64";
        case SIROpcode::I64_TO_STRING: return "i64.to.string";
        case SIROpcode::F64_TO_STRING: return "f64.to.string";
        case SIROpcode::BOOL_TO_STRING: return "bool.to.string";
        case SIROpcode::CAST:       return "cast";
        
        // Builtin
        case SIROpcode::BUILTIN_PRINT: return "builtin.print";
        case SIROpcode::BUILTIN_READ: return "builtin.read";
        case SIROpcode::BUILTIN_SQRT: return "builtin.sqrt";
        case SIROpcode::BUILTIN_POW: return "builtin.pow";
        case SIROpcode::BUILTIN_SIN: return "builtin.sin";
        case SIROpcode::BUILTIN_COS: return "builtin.cos";
        case SIROpcode::BUILTIN_TAN: return "builtin.tan";
        case SIROpcode::BUILTIN_ABS: return "builtin.abs";
        case SIROpcode::BUILTIN_FLOOR: return "builtin.floor";
        case SIROpcode::BUILTIN_CEIL: return "builtin.ceil";
        case SIROpcode::BUILTIN_ROUND: return "builtin.round";
        case SIROpcode::BUILTIN_MIN: return "builtin.min";
        case SIROpcode::BUILTIN_MAX: return "builtin.max";
        case SIROpcode::BUILTIN_ASSERT: return "builtin.assert";
        case SIROpcode::BUILTIN_DEBUG: return "builtin.debug";
        
        default:                    return "unknown";
    }
}

// ======================================================================
// عدد المعاملات / Operand count
// ======================================================================

int sirOpcodeOperandCount(SIROpcode opcode) {
    switch (opcode) {
        // Binary operations (2 operands)
        case SIROpcode::ADD_I64:
        case SIROpcode::ADD_F64:
        case SIROpcode::SUB_I64:
        case SIROpcode::SUB_F64:
        case SIROpcode::MUL_I64:
        case SIROpcode::MUL_F64:
        case SIROpcode::DIV_I64:
        case SIROpcode::DIV_F64:
        case SIROpcode::MOD_I64:
        case SIROpcode::AND:
        case SIROpcode::OR:
        case SIROpcode::XOR:
        case SIROpcode::SHL:
        case SIROpcode::SHR:
        case SIROpcode::SAR:
        case SIROpcode::ROL:
        case SIROpcode::EQ:
        case SIROpcode::NE:
        case SIROpcode::LT:
        case SIROpcode::LE:
        case SIROpcode::GT:
        case SIROpcode::GE:
        case SIROpcode::PTR_ADD:
        case SIROpcode::ARRAY_GET:
        case SIROpcode::STRING_CONCAT:
        case SIROpcode::STRING_CMP:
        case SIROpcode::OBJECT_GET:
        case SIROpcode::BUILTIN_POW:
        case SIROpcode::BUILTIN_MIN:
        case SIROpcode::BUILTIN_MAX:
            return 2;
        
        // Unary operations (1 operand)
        case SIROpcode::NEG:
        case SIROpcode::NOT:
        case SIROpcode::RET:
        case SIROpcode::BR:
        case SIROpcode::LOAD:
        case SIROpcode::MOVE:
        case SIROpcode::ADDR:
        case SIROpcode::FREE:
        case SIROpcode::ARRAY_LEN:
        case SIROpcode::STRING_LEN:
        case SIROpcode::OBJECT_NEW:
        case SIROpcode::I64_TO_F64:
        case SIROpcode::F64_TO_I64:
        case SIROpcode::I64_TO_BOOL:
        case SIROpcode::BOOL_TO_I64:
        case SIROpcode::I64_TO_STRING:
        case SIROpcode::F64_TO_STRING:
        case SIROpcode::BOOL_TO_STRING:
        case SIROpcode::BUILTIN_PRINT:
        case SIROpcode::BUILTIN_SQRT:
        case SIROpcode::BUILTIN_SIN:
        case SIROpcode::BUILTIN_COS:
        case SIROpcode::BUILTIN_TAN:
        case SIROpcode::BUILTIN_ABS:
        case SIROpcode::BUILTIN_FLOOR:
        case SIROpcode::BUILTIN_CEIL:
        case SIROpcode::BUILTIN_ROUND:
            return 1;
        
        // No operands
        case SIROpcode::RET_VOID:
        case SIROpcode::BUILTIN_READ:
            return 0;
        
        // Variable operands (return -1)
        case SIROpcode::BR_COND:
        case SIROpcode::CALL:
        case SIROpcode::CALL_INDIRECT:
        case SIROpcode::SWITCH:
        case SIROpcode::PHI:
        case SIROpcode::ALLOC:
        case SIROpcode::ALLOC_HEAP:
        case SIROpcode::STORE:
        case SIROpcode::PTR_CAST:
        case SIROpcode::MEMCPY:
        case SIROpcode::MEMSET:
        case SIROpcode::ARRAY_NEW:
        case SIROpcode::ARRAY_SET:
        case SIROpcode::ARRAY_APPEND:
        case SIROpcode::ARRAY_REMOVE:
        case SIROpcode::STRING_NEW:
        case SIROpcode::STRING_SUBSTR:
        case SIROpcode::STRING_FIND:
        case SIROpcode::STRING_REPLACE:
        case SIROpcode::STRING_TO_I64:
        case SIROpcode::OBJECT_SET:
        case SIROpcode::OBJECT_CALL:
        case SIROpcode::INSTANCEOF:
        case SIROpcode::OBJECT_CAST:
        case SIROpcode::CLASS_DEF:
        case SIROpcode::METHOD_DEF:
        case SIROpcode::FIELD_DEF:
        case SIROpcode::CONSTRUCTOR_CALL:
        case SIROpcode::CAST:
        case SIROpcode::BUILTIN_ASSERT:
        case SIROpcode::BUILTIN_DEBUG:
            return -1; // Variable number of operands
        
        default:
            return 0;
    }
}

// ======================================================================
// تحويل SIROperand إلى نص / Convert SIROperand to string
// ======================================================================

std::string SIROperand::toString() const {
    std::ostringstream oss;
    
    switch (type) {
        case SIROperandType::REGISTER:
            oss << "%" << name;
            break;
            
        case SIROperandType::CONSTANT:
            switch (dataType) {
                case SIRType::I64:
                    oss << intValue;
                    break;
                case SIRType::F64:
                    oss << floatValue;
                    break;
                case SIRType::BOOL:
                    oss << (boolValue ? "true" : "false");
                    break;
                case SIRType::STRING:
                    oss << "\"" << name << "\"";
                    break;
                default:
                    oss << "const";
                    break;
            }
            break;
            
        case SIROperandType::LABEL:
            oss << "label %" << name;
            break;
            
        case SIROperandType::GLOBAL:
            oss << "$" << name;
            break;
            
        case SIROperandType::FUNCTION:
            oss << "@" << name;
            break;
    }
    
    return oss.str();
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
