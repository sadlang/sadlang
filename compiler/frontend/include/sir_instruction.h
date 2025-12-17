// ======================================================================
// sir_instruction.h - تعليمات SIR
//                    SIR Instructions
// ======================================================================
// الوصف بالعربية:
//   تعريف تعليمات SIR (Simple Intermediate Representation):
//   - SIRInstruction: تعليمة واحدة
//   - SIRBasicBlock: كتلة أساسية من التعليمات
//
// English Description:
//   Definition of SIR instructions:
//   - SIRInstruction: Single instruction
//   - SIRBasicBlock: Basic block of instructions
//
// الإصدار / Version: 1.0
// التاريخ / Date: December 2, 2025
// المرحلة / Phase: 1 - Frontend
// ======================================================================

#pragma once

#include "sir_types.h"
#include <vector>
#include <string>
#include <memory>
#include <optional>

namespace Sad {
namespace Compiler {
namespace SIR {

// Forward declarations
class SIRBasicBlock;
class SIRFunction;

// ======================================================================
// تعليمة SIR / SIR Instruction
// ======================================================================

/**
 * @brief (AR) تعليمة واحدة في SIR
 * @brief (EN) Single instruction in SIR
 * 
 * الشكل العام / General form:
 * ```
 * [label:]
 * [%result =] opcode operand1, operand2, ...
 * ```
 * 
 * أمثلة / Examples:
 * ```
 * %t1 = add.i64 %a, %b
 * store %t1, %ptr
 * br label %loop
 * loop:
 *   %t2 = load %ptr
 * ```
 */
class SIRInstruction {
public:
    SIROpcode opcode;                   ///< رمز العملية / Operation code
    std::optional<SIROperand> result;   ///< النتيجة (اختياري) / Result (optional)
    std::vector<SIROperand> operands;   ///< المعاملات / Operands
    std::string label;                  ///< تسمية (اختياري) / Label (optional)
    std::string comment;                ///< تعليق (اختياري) / Comment (optional)
    
    // Metadata for debugging and optimization
    int lineNumber = -1;                ///< رقم السطر في الكود المصدري
    bool isTerminator = false;          ///< هل هذه آخر تعليمة في الكتلة؟
    
    // ==========================================
    // Constructors
    // ==========================================
    
    /**
     * @brief (AR) منشئ افتراضي
     * @brief (EN) Default constructor
     */
    SIRInstruction() : opcode(SIROpcode::ADD_I64) {}
    
    /**
     * @brief (AR) منشئ بـ opcode فقط
     * @brief (EN) Constructor with opcode only
     */
    explicit SIRInstruction(SIROpcode op) 
        : opcode(op) {}
    
    /**
     * @brief (AR) منشئ كامل
     * @brief (EN) Full constructor
     */
    SIRInstruction(SIROpcode op, 
                   const std::optional<SIROperand>& res,
                   const std::vector<SIROperand>& ops)
        : opcode(op), result(res), operands(ops) {}
    
    // ==========================================
    // Factory Methods (دوال إنشاء سريعة)
    // ==========================================
    
    /**
     * @brief (AR) إنشاء تعليمة ثنائية (binary operation)
     * @brief (EN) Create binary operation instruction
     * 
     * @example %result = add.i64 %a, %b
     */
    static SIRInstruction Binary(SIROpcode op,
                                const SIROperand& result,
                                const SIROperand& left,
                                const SIROperand& right) {
        SIRInstruction inst(op);
        inst.result = result;
        inst.operands = {left, right};
        return inst;
    }
    
    /**
     * @brief (AR) إنشاء تعليمة أحادية (unary operation)
     * @brief (EN) Create unary operation instruction
     * 
     * @example %result = neg %a
     */
    static SIRInstruction Unary(SIROpcode op,
                               const SIROperand& result,
                               const SIROperand& operand) {
        SIRInstruction inst(op);
        inst.result = result;
        inst.operands = {operand};
        return inst;
    }
    
    /**
     * @brief (AR) إنشاء تعليمة load
     * @brief (EN) Create load instruction
     * 
     * @example %value = load ptr<%ptr>
     */
    static SIRInstruction Load(const SIROperand& result,
                              const SIROperand& address) {
        return Unary(SIROpcode::LOAD, result, address);
    }
    
    /**
     * @brief (AR) إنشاء تعليمة store
     * @brief (EN) Create store instruction
     * 
     * @example store %value, %ptr
     */
    static SIRInstruction Store(const SIROperand& value,
                               const SIROperand& address) {
        SIRInstruction inst(SIROpcode::STORE);
        inst.operands = {value, address};
        return inst;
    }
    
    /**
     * @brief (AR) إنشاء تعليمة alloc
     * @brief (EN) Create alloc instruction
     * 
     * @example %ptr = alloc i64
     */
    static SIRInstruction Alloc(const SIROperand& result,
                               SIRType type,
                               const SIROperand& size) {
        SIRInstruction inst(SIROpcode::ALLOC);
        inst.result = result;
        inst.operands = {size};
        return inst;
    }
    
    /**
     * @brief (AR) إنشاء تعليمة branch غير مشروط
     * @brief (EN) Create unconditional branch
     * 
     * @example br label %target
     */
    static SIRInstruction Branch(const SIROperand& target) {
        SIRInstruction inst(SIROpcode::BR);
        inst.operands = {target};
        inst.isTerminator = true;
        return inst;
    }
    
    /**
     * @brief (AR) إنشاء تعليمة branch مشروط
     * @brief (EN) Create conditional branch
     * 
     * @example br %cond, label %then, label %else
     */
    static SIRInstruction BranchCond(const SIROperand& condition,
                                    const SIROperand& thenLabel,
                                    const SIROperand& elseLabel) {
        SIRInstruction inst(SIROpcode::BR_COND);
        inst.operands = {condition, thenLabel, elseLabel};
        inst.isTerminator = true;
        return inst;
    }
    
    /**
     * @brief (AR) إنشاء تعليمة return
     * @brief (EN) Create return instruction
     * 
     * @example ret i64 %value
     */
    static SIRInstruction Return(const SIROperand& value) {
        SIRInstruction inst(SIROpcode::RET);
        inst.operands = {value};
        inst.isTerminator = true;
        return inst;
    }
    
    /**
     * @brief (AR) إنشاء تعليمة return void
     * @brief (EN) Create return void instruction
     * 
     * @example ret void
     */
    static SIRInstruction ReturnVoid() {
        SIRInstruction inst(SIROpcode::RET_VOID);
        inst.isTerminator = true;
        return inst;
    }
    
    /**
     * @brief (AR) إنشاء تعليمة call
     * @brief (EN) Create call instruction
     * 
     * @example %result = call @functionName(%arg1, %arg2)
     */
    static SIRInstruction Call(const SIROperand& result,
                              const SIROperand& function,
                              const std::vector<SIROperand>& args) {
        SIRInstruction inst(SIROpcode::CALL);
        inst.result = result;
        inst.operands.push_back(function);
        inst.operands.insert(inst.operands.end(), args.begin(), args.end());
        return inst;
    }
    
    /**
     * @brief (AR) إنشاء تعليمة call بدون نتيجة
     * @brief (EN) Create call instruction without result
     * 
     * @example call @functionName(%arg1, %arg2)
     */
    static SIRInstruction CallVoid(const SIROperand& function,
                                  const std::vector<SIROperand>& args) {
        SIRInstruction inst(SIROpcode::CALL);
        inst.operands.push_back(function);
        inst.operands.insert(inst.operands.end(), args.begin(), args.end());
        return inst;
    }
    
    /**
     * @brief (AR) إنشاء تعليمة PHI (لـ SSA form)
     * @brief (EN) Create PHI instruction (for SSA form)
     * 
     * @example %result = phi i64 [%val1, %bb1], [%val2, %bb2]
     */
    static SIRInstruction Phi(const SIROperand& result,
                             const std::vector<std::pair<SIROperand, SIROperand>>& incoming) {
        SIRInstruction inst(SIROpcode::PHI);
        inst.result = result;
        for (const auto& [value, block] : incoming) {
            inst.operands.push_back(value);
            inst.operands.push_back(block);
        }
        return inst;
    }
    
    // ==========================================
    // Utility Methods
    // ==========================================
    
    /**
     * @brief (AR) هل التعليمة terminator؟ (آخر تعليمة في الكتلة)
     * @brief (EN) Is this instruction a terminator?
     */
    bool isTerminatorInst() const {
        return isTerminator || 
               opcode == SIROpcode::BR || 
               opcode == SIROpcode::BR_COND ||
               opcode == SIROpcode::RET ||
               opcode == SIROpcode::RET_VOID ||
               opcode == SIROpcode::SWITCH;
    }
    
    /**
     * @brief (AR) هل التعليمة لها نتيجة؟
     * @brief (EN) Does this instruction have a result?
     */
    bool hasResult() const {
        return result.has_value();
    }
    
    /**
     * @brief (AR) هل التعليمة لها تسمية؟
     * @brief (EN) Does this instruction have a label?
     */
    bool hasLabel() const {
        return !label.empty();
    }
    
    /**
     * @brief (AR) تحويل التعليمة إلى نص
     * @brief (EN) Convert instruction to string
     * 
     * @return نص بتنسيق SIR / String in SIR format
     * 
     * @example
     * ```
     * %t1 = add.i64 5, 3
     * store %t1, %ptr
     * br label %loop
     * ```
     */
    std::string toString() const;
    
    /**
     * @brief (AR) طباعة التعليمة (للتنقيح)
     * @brief (EN) Print instruction (for debugging)
     */
    void print() const;
};

// ======================================================================
// كتلة أساسية / Basic Block
// ======================================================================

/**
 * @brief (AR) كتلة أساسية من التعليمات
 * @brief (EN) Basic block of instructions
 * 
 * الكتلة الأساسية (Basic Block) هي مجموعة من التعليمات:
 * A basic block is a sequence of instructions:
 * - تبدأ بتسمية (label)
 * - تنتهي بـ terminator (br, ret, switch)
 * - لا يوجد قفز في الوسط
 * 
 * @example
 * ```
 * entry:
 *   %t1 = add.i64 5, 3
 *   %t2 = mul.i64 %t1, 2
 *   br label %loop
 * 
 * loop:
 *   %t3 = load %ptr
 *   br.cond %t3, label %then, label %else
 * ```
 */
class SIRBasicBlock {
public:
    std::string name;                           ///< اسم الكتلة / Block name
    std::vector<SIRInstruction> instructions;   ///< التعليمات / Instructions
    
    // للتحليل والتحسين / For analysis and optimization
    std::vector<SIRBasicBlock*> predecessors;   ///< الكتل السابقة / Predecessor blocks
    std::vector<SIRBasicBlock*> successors;     ///< الكتل التالية / Successor blocks
    
    SIRFunction* parent = nullptr;              ///< الدالة الأم / Parent function
    
    // ==========================================
    // Constructors
    // ==========================================
    
    /**
     * @brief (AR) منشئ افتراضي
     * @brief (EN) Default constructor
     */
    SIRBasicBlock() = default;
    
    /**
     * @brief (AR) منشئ باسم
     * @brief (EN) Constructor with name
     */
    explicit SIRBasicBlock(const std::string& blockName) 
        : name(blockName) {}
    
    // ==========================================
    // Methods
    // ==========================================
    
    /**
     * @brief (AR) إضافة تعليمة
     * @brief (EN) Add instruction
     */
    void addInstruction(const SIRInstruction& inst) {
        instructions.push_back(inst);
    }
    
    /**
     * @brief (AR) الحصول على آخر تعليمة (terminator)
     * @brief (EN) Get last instruction (terminator)
     */
    SIRInstruction* getTerminator() {
        if (instructions.empty()) return nullptr;
        return instructions.back().isTerminatorInst() ? &instructions.back() : nullptr;
    }
    
    /**
     * @brief (AR) هل الكتلة فارغة؟
     * @brief (EN) Is block empty?
     */
    bool empty() const {
        return instructions.empty();
    }
    
    /**
     * @brief (AR) عدد التعليمات
     * @brief (EN) Number of instructions
     */
    size_t size() const {
        return instructions.size();
    }
    
    /**
     * @brief (AR) إضافة كتلة سابقة
     * @brief (EN) Add predecessor block
     */
    void addPredecessor(SIRBasicBlock* pred) {
        predecessors.push_back(pred);
    }
    
    /**
     * @brief (AR) إضافة كتلة تالية
     * @brief (EN) Add successor block
     */
    void addSuccessor(SIRBasicBlock* succ) {
        successors.push_back(succ);
    }
    
    /**
     * @brief (AR) تحويل الكتلة إلى نص
     * @brief (EN) Convert block to string
     */
    std::string toString() const;
    
    /**
     * @brief (AR) طباعة الكتلة (للتنقيح)
     * @brief (EN) Print block (for debugging)
     */
    void print() const;
};

} // namespace SIR
} // namespace Compiler
} // namespace Sad
