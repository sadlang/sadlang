/**
 * @file emitter.h
 * @brief مُصدِر البايت كود / Bytecode Emitter
 * @brief Translates SIR to bytecode
 * 
 * @details
 * (AR) يترجم التمثيل الوسيط SIR إلى بايت كود قابل للتنفيذ.
 *      يدير الثوابت، التسميات، والقفزات.
 * 
 * (EN) Translates SIR intermediate representation to executable bytecode.
 *      Manages constants, labels, and jumps.
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 * @phase Phase 3: Bytecode Backend
 */

#pragma once

#include "bytecode.h"
#include "../../frontend/include/sir_module.h"
#include "../../frontend/include/sir_instruction.h"
#include <unordered_map>
#include <string>
#include <memory>

namespace Sad {
namespace Compiler {
namespace Bytecode {

// ========================================
// Label Information
// معلومات التسمية
// ========================================

/**
 * @brief معلومات التسمية / Label Info
 * @brief Information about a label for jumps
 */
struct LabelInfo {
    std::string name;                  ///< الاسم / Name
    uint32_t offset;                   ///< الإزاحة / Offset (-1 if not resolved)
    std::vector<uint32_t> references;  ///< المراجع / References to patch
    bool resolved;                     ///< هل مُحلَّلة؟ / Is resolved?
    
    LabelInfo() : offset(0xFFFFFFFF), resolved(false) {}
};

// ========================================
// Variable Information
// معلومات المتغير
// ========================================

/**
 * @brief معلومات المتغير / Variable Info
 * @brief Maps variable name to local/global index
 */
struct VariableInfo {
    std::string name;      ///< الاسم / Name
    uint32_t index;        ///< الفهرس / Index
    bool isGlobal;         ///< هل عام؟ / Is global?
    bool isCaptured;       ///< هل محتجز؟ / Is captured?
    
    VariableInfo() : index(0), isGlobal(false), isCaptured(false) {}
};

// ========================================
// Bytecode Emitter
// مُصدِر البايت كود
// ========================================

/**
 * @brief مُصدِر البايت كود / Bytecode Emitter
 * @brief Main class for SIR → Bytecode translation
 * 
 * @details
 * (AR) يترجم وحدة SIR كاملة إلى بايت كود قابل للتنفيذ.
 *      يدير السياق، المتغيرات، والتسميات.
 * 
 * (EN) Translates complete SIR module to executable bytecode.
 *      Manages context, variables, and labels.
 * 
 * @example
 * ```cpp
 * BytecodeEmitter emitter;
 * auto bytecode = emitter.emit(sirModule);
 * bytecode->disassemble();
 * ```
 */
class BytecodeEmitter {
public:
    /**
     * @brief Constructor / البناء
     */
    BytecodeEmitter();
    
    /**
     * @brief إصدار بايت كود من SIR / Emit bytecode from SIR
     * @brief Main entry point for translation
     * 
     * @param module وحدة SIR / SIR module
     * @return وحدة البايت كود / Bytecode module
     * 
     * @details
     * (AR) يترجم وحدة SIR كاملة بما في ذلك جميع الدوال والمتغيرات العامة.
     * (EN) Translates complete SIR module including all functions and globals.
     */
    std::unique_ptr<BytecodeModule> emit(std::shared_ptr<SIR::SIRModule> module);
    
    /**
     * @brief إصدار دالة / Emit function
     */
    void emitFunction(SIR::SIRFunction* function);
    
    /**
     * @brief تمكين/تعطيل التحسين / Enable/disable optimization
     */
    void setOptimize(bool optimize) { optimize_ = optimize; }
    
    /**
     * @brief تمكين/تعطيل وضع التصحيح / Enable/disable debug mode
     */
    void setDebugMode(bool debug) { debugMode_ = debug; }

private:
    // ========================================
    // Instruction Emission
    // إصدار التعليمات
    // ========================================
    
    /**
     * @brief إصدار كتلة أساسية / Emit basic block
     */
    void emitBlock(SIR::SIRBasicBlock* block);
    
    /**
     * @brief إصدار تعليمة / Emit instruction
     */
    void emitInstruction(SIR::SIRInstruction* inst);
    
    /**
     * @brief إصدار عملية حسابية / Emit arithmetic operation
     */
    void emitArithmetic(SIR::SIRInstruction* inst);
    
    /**
     * @brief إصدار عملية بتية / Emit bitwise operation
     */
    void emitBitwise(SIR::SIRInstruction* inst);
    
    /**
     * @brief إصدار مقارنة / Emit comparison
     */
    void emitComparison(SIR::SIRInstruction* inst);
    
    /**
     * @brief إصدار قفز / Emit jump
     */
    void emitJump(SIR::SIRInstruction* inst);
    
    /**
     * @brief إصدار استدعاء / Emit call
     */
    void emitCall(SIR::SIRInstruction* inst);
    
    /**
     * @brief إصدار عودة / Emit return
     */
    void emitReturn(SIR::SIRInstruction* inst);
    
    /**
     * @brief إصدار تحميل / Emit load
     */
    void emitLoad(SIR::SIRInstruction* inst);
    
    /**
     * @brief إصدار تخزين / Emit store
     */
    void emitStore(SIR::SIRInstruction* inst);
    
    /**
     * @brief إصدار عملية ذاكرة / Emit memory operation
     */
    void emitMemory(SIR::SIRInstruction* inst);
    
    /**
     * @brief إصدار عملية مصفوفة / Emit array operation
     */
    void emitArray(SIR::SIRInstruction* inst);
    
    /**
     * @brief إصدار دالة مدمجة / Emit builtin call
     */
    void emitBuiltin(SIR::SIRInstruction* inst);
    
    // ========================================
    // Operand Handling
    // معالجة المعاملات
    // ========================================
    
    /**
     * @brief إصدار معامل / Emit operand
     * @brief Push operand value onto stack
     */
    void emitOperand(const SIR::SIROperand& operand);
    
    /**
     * @brief إصدار نتيجة / Emit result storage
     * @brief Store top of stack to result location
     */
    void emitResult(const SIR::SIROperand& result);
    
    // ========================================
    // Label Management
    // إدارة التسميات
    // ========================================
    
    /**
     * @brief تعريف تسمية / Define label
     */
    void defineLabel(const std::string& name);
    
    /**
     * @brief إصدار قفز إلى تسمية / Emit jump to label
     */
    void emitJumpToLabel(VM::Opcode jumpOp, const std::string& label);
    
    /**
     * @brief حل جميع التسميات / Resolve all labels
     */
    void resolveLabels();
    
    // ========================================
    // Variable Management
    // إدارة المتغيرات
    // ========================================
    
    /**
     * @brief تسجيل متغير محلي / Register local variable
     */
    uint32_t registerLocal(const std::string& name);
    
    /**
     * @brief تسجيل متغير عام / Register global variable
     */
    uint32_t registerGlobal(const std::string& name);
    
    /**
     * @brief الحصول على فهرس متغير / Get variable index
     */
    uint32_t getVariableIndex(const std::string& name);
    
    /**
     * @brief هل المتغير عام؟ / Is variable global?
     */
    bool isGlobal(const std::string& name);
    
    // ========================================
    // Register Management
    // إدارة السجلات
    // ========================================
    
    /**
     * @brief تخصيص سجل مؤقت / Allocate temporary register
     */
    std::string allocTemp();
    
    /**
     * @brief تحرير سجل مؤقت / Free temporary register
     */
    void freeTemp(const std::string& reg);
    
    // ========================================
    // Optimization
    // التحسين
    // ========================================
    
    /**
     * @brief تحسين بسيط / Simple optimization pipeline
     */
    void optimize();
    
    /**
     * @brief تحسينات peephole / Peephole optimizations
     */
    void peepholeOptimize();
    
    /**
     * @brief دمج التسلسلات المتكررة / Merge duplicate sequences
     */
    void mergeDuplicates();
    
    /**
     * @brief دمج التعليمات / Merge instructions (deprecated)
     */
    bool mergeInstructions(uint32_t offset);
    
    /**
     * @brief إزالة التعليمات الميتة / Remove dead instructions
     */
    void removeDeadCode();
    
    // ========================================
    // Optimization Helpers
    // دوال مساعدة للتحسين
    // ========================================
    
    /**
     * @brief حساب حجم التعليمة / Get instruction size in bytes
     */
    uint32_t getInstructionSize(VM::Opcode op) const;
    
    /**
     * @brief هل التعليمة قفزة؟ / Is instruction a jump?
     */
    bool isJumpInstruction(VM::Opcode op) const;
    
    /**
     * @brief هل التعليمة قفزة غير شرطية؟ / Is instruction unconditional jump?
     */
    bool isUnconditionalJump(VM::Opcode op) const;
    
    /**
     * @brief استخراج هدف القفزة / Extract jump target
     */
    uint32_t extractJumpTarget(const std::vector<Byte>& code, uint32_t offset, VM::Opcode op) const;
    
    /**
     * @brief تحديث أهداف القفز / Update jump targets
     */
    void updateJumpTargets(std::vector<Byte>& code, const std::unordered_map<uint32_t, uint32_t>& offsetMap);
    
    // ========================================
    // Utilities
    // أدوات مساعدة
    // ========================================
    
    /**
     * @brief طباعة رسالة تصحيح / Print debug message
     */
    void debug(const std::string& message);
    
    /**
     * @brief طباعة خطأ / Print error
     */
    void error(const std::string& message);

private:
    std::unique_ptr<BytecodeModule> module_;          ///< الوحدة الحالية / Current module
    SIR::SIRFunction* currentFunction_;               ///< الدالة الحالية / Current function
    std::unordered_map<std::string, LabelInfo> labels_; ///< خريطة التسميات / Label map
    std::unordered_map<std::string, VariableInfo> variables_; ///< خريطة المتغيرات / Variable map
    std::unordered_map<std::string, uint32_t> registerMap_; ///< خريطة السجلات / Register map
    uint32_t localCount_;                             ///< عدد المتغيرات المحلية / Local count
    uint32_t tempCounter_;                            ///< عداد المتغيرات المؤقتة / Temp counter
    bool optimize_;                                   ///< هل نحسّن؟ / Optimize?
    bool debugMode_;                                  ///< وضع التصحيح / Debug mode
    int errorCount_;                                  ///< عدد الأخطاء / Error count
};

// ========================================
// Helper Functions
// دوال مساعدة
// ========================================

/**
 * @brief تحويل SIROpcode إلى Bytecode Opcode
 * @brief Convert SIR opcode to bytecode opcode
 */
VM::Opcode sirToBytecodeOp(SIR::SIROpcode sirOp);

/**
 * @brief هل العملية حسابية؟ / Is operation arithmetic?
 */
bool isArithmeticOp(SIR::SIROpcode op);

/**
 * @brief هل العملية بتية؟ / Is operation bitwise?
 */
bool isBitwiseOp(SIR::SIROpcode op);

/**
 * @brief هل العملية مقارنة؟ / Is operation comparison?
 */
bool isComparisonOp(SIR::SIROpcode op);

/**
 * @brief هل العملية قفز؟ / Is operation jump?
 */
bool isJumpOp(SIR::SIROpcode op);

/**
 * @brief هل العملية ذاكرة؟ / Is operation memory?
 */
bool isMemoryOp(SIR::SIROpcode op);

} // namespace Bytecode
} // namespace Compiler
} // namespace Sad
