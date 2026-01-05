// ============================================================================
// sir_instruction.cpp - تنفيذ تعليمات SIR
//                       SIR Instructions Implementation
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 5, 2026
// الإصدار / Version: 1.0
//
// ملاحظة: هذا الملف ينفذ دوال toString() و print() لـ SIRInstruction و SIRBasicBlock
// Note: This file implements toString() and print() for SIRInstruction and SIRBasicBlock
// ============================================================================

#include "sir_instruction.h"
#include <sstream>
#include <iostream>

namespace Sad {
namespace Compiler {
namespace SIR {

// ============================================================================
// SIRInstruction::toString - تحويل التعليمة إلى نص
// ============================================================================
// مصدر التعريف / Source: sir_instruction.h:318
// التوقيع / Signature: std::string toString() const;
// ============================================================================
std::string SIRInstruction::toString() const {
    std::ostringstream oss;
    
    // (AR) إضافة التسمية إن وجدت / (EN) Add label if present
    if (!label.empty()) {
        oss << label << ":\n    ";
    }
    
    // (AR) إضافة النتيجة إن وجدت / (EN) Add result if present
    if (result.has_value()) {
        oss << result->toString() << " = ";
    }
    
    // (AR) إضافة رمز العملية / (EN) Add opcode
    oss << sirOpcodeToString(opcode);
    
    // (AR) إضافة المعاملات / (EN) Add operands
    if (!operands.empty()) {
        oss << " ";
        for (size_t i = 0; i < operands.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << operands[i].toString();
        }
    }
    
    // (AR) إضافة التعليق إن وجد / (EN) Add comment if present
    if (!comment.empty()) {
        oss << "  ; " << comment;
    }
    
    return oss.str();
}

// ============================================================================
// SIRInstruction::print - طباعة التعليمة للتنقيح
// ============================================================================
// مصدر التعريف / Source: sir_instruction.h:324
// التوقيع / Signature: void print() const;
// ============================================================================
void SIRInstruction::print() const {
    std::cout << toString() << std::endl;
}

// ============================================================================
// SIRBasicBlock::toString - تحويل الكتلة إلى نص
// ============================================================================
// مصدر التعريف / Source: sir_instruction.h:438
// التوقيع / Signature: std::string toString() const;
// ============================================================================
std::string SIRBasicBlock::toString() const {
    std::ostringstream oss;
    
    // (AR) اسم الكتلة / (EN) Block name
    oss << name << ":\n";
    
    // (AR) التعليمات / (EN) Instructions
    for (const auto& inst : instructions) {
        oss << "    " << inst.toString() << "\n";
    }
    
    return oss.str();
}

// ============================================================================
// SIRBasicBlock::print - طباعة الكتلة للتنقيح
// ============================================================================
// مصدر التعريف / Source: sir_instruction.h:444
// التوقيع / Signature: void print() const;
// ============================================================================
void SIRBasicBlock::print() const {
    std::cout << toString() << std::endl;
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
