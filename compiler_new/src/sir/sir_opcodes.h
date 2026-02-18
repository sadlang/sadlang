/**
 * =============================================================================
 * ملف: sir_opcodes.h
 * الوصف: عمليات SIR (12 عملية ملكية)
 * المهمة: T261 - SIR opcodes (12 ownership ops)
 * المرحلة: Phase 27 - User Story 24 (SIR Layer)
 * =============================================================================
 * 
 * 🎮 دليل المبتدئ لعمليات SIR
 * ═══════════════════════════════
 * 
 * ما هي العمليات (Opcodes)؟
 * ─────────────────────────
 * العمليات هي الأوامر الأساسية في SIR.
 * كل سطر كود يُترجم إلى عملية أو أكثر.
 * 
 * العمليات الـ 12 للملكية:
 * ────────────────────────
 * 
 * 1. Alloc - تخصيص ذاكرة لمتغير جديد
 *    ```
 *    متغير س = 10
 *    →  %0 = Alloc(عدد)
 *       Store(%0, 10)
 *    ```
 * 
 * 2. Borrow - استعارة للقراءة فقط
 *    ```
 *    متغير مرجع = &س
 *    →  %1 = Borrow(%0)
 *    ```
 * 
 * 3. BorrowMut - استعارة للقراءة والكتابة
 *    ```
 *    متغير مرجع = &متغير س
 *    →  %1 = BorrowMut(%0)
 *    ```
 * 
 * 4. Move - نقل الملكية
 *    ```
 *    متغير ص = انقل س
 *    →  %1 = Move(%0)
 *    ```
 * 
 * 5. Copy - نسخ القيمة
 *    ```
 *    متغير ص = س    // لأنواع قابلة للنسخ
 *    →  %1 = Copy(%0)
 *    ```
 * 
 * 6. Drop - إسقاط (تحرير) القيمة
 *    ```
 *    نهاية          // نهاية النطاق
 *    →  Drop(%0)
 *    ```
 * 
 * 7. Clone - استنساخ عميق
 *    ```
 *    متغير ص = س.استنسخ()
 *    →  %1 = Clone(%0)
 *    ```
 * 
 * 8. EndBorrow - إنهاء الاستعارة
 *    ```
 *    // المرجع خرج من النطاق
 *    →  EndBorrow(%1)
 *    ```
 * 
 * 9. Reborrow - إعادة استعارة
 *    ```
 *    متغير مرجع2 = مرجع
 *    →  %2 = Reborrow(%1)
 *    ```
 * 
 * 10. Project - الوصول لحقل
 *     ```
 *     متغير ع = س.حقل
 *     →  %1 = Project(%0, "حقل")
 *     ```
 * 
 * 11. Deref - فك المرجع
 *     ```
 *     متغير ص = *مرجع
 *     →  %1 = Deref(%ref)
 *     ```
 * 
 * 12. Take - أخذ من حاوية
 *     ```
 *     متغير ص = مصفوفة.أخذ(0)
 *     →  %1 = Take(%arr, 0)
 *     ```
 * 
 * =============================================================================
 */

#ifndef SAD_SIR_OPCODES_H
#define SAD_SIR_OPCODES_H

#include "sir_types.h"
#include <string>
#include <vector>
#include <memory>
#include <variant>

namespace sad::sir {

// =============================================================================
// تصنيف العمليات
// =============================================================================

/**
 * تصنيف العملية
 */
enum class OpCategory {
    Ownership,      // عمليات الملكية (الـ 12)
    Memory,         // عمليات الذاكرة
    Arithmetic,     // عمليات حسابية
    Comparison,     // عمليات المقارنة
    Logic,          // عمليات منطقية
    Control,        // عمليات التحكم
    Function,       // عمليات الدوال
    Type,           // عمليات الأنواع
    Debug           // عمليات التصحيح
};

// =============================================================================
// كود العملية
// =============================================================================

/**
 * كود العملية
 */
enum class Opcode {
    // ═══════════════════════════════════════════════════════════════
    // عمليات الملكية الـ 12
    // ═══════════════════════════════════════════════════════════════
    
    Alloc,          // 1. تخصيص: %r = Alloc(نوع)
    Borrow,         // 2. استعارة ثابتة: %r = Borrow(%v)
    BorrowMut,      // 3. استعارة متغيرة: %r = BorrowMut(%v)
    Move,           // 4. نقل: %r = Move(%v)
    Copy,           // 5. نسخ: %r = Copy(%v)
    Drop,           // 6. إسقاط: Drop(%v)
    Clone,          // 7. استنساخ: %r = Clone(%v)
    EndBorrow,      // 8. إنهاء استعارة: EndBorrow(%ref)
    Reborrow,       // 9. إعادة استعارة: %r = Reborrow(%ref)
    Project,        // 10. إسقاط حقل: %r = Project(%v, حقل)
    Deref,          // 11. فك مرجع: %r = Deref(%ref)
    Take,           // 12. أخذ: %r = Take(%container, فهرس)
    
    // ═══════════════════════════════════════════════════════════════
    // عمليات الذاكرة
    // ═══════════════════════════════════════════════════════════════
    
    Load,           // تحميل: %r = Load(%ptr)
    Store,          // تخزين: Store(%ptr, %v)
    StackAlloc,     // تخصيص مكدس: %r = StackAlloc(حجم)
    HeapAlloc,      // تخصيص كومة: %r = HeapAlloc(حجم)
    Free,           // تحرير: Free(%ptr)
    
    // ═══════════════════════════════════════════════════════════════
    // عمليات حسابية
    // ═══════════════════════════════════════════════════════════════
    
    Add,            // جمع: %r = Add(%a, %b)
    Sub,            // طرح: %r = Sub(%a, %b)
    Mul,            // ضرب: %r = Mul(%a, %b)
    Div,            // قسمة: %r = Div(%a, %b)
    Mod,            // باقي: %r = Mod(%a, %b)
    Neg,            // سالب: %r = Neg(%v)
    
    // ═══════════════════════════════════════════════════════════════
    // عمليات المقارنة
    // ═══════════════════════════════════════════════════════════════
    
    Eq,             // تساوي: %r = Eq(%a, %b)
    Ne,             // عدم تساوي: %r = Ne(%a, %b)
    Lt,             // أقل: %r = Lt(%a, %b)
    Le,             // أقل أو يساوي: %r = Le(%a, %b)
    Gt,             // أكبر: %r = Gt(%a, %b)
    Ge,             // أكبر أو يساوي: %r = Ge(%a, %b)
    
    // ═══════════════════════════════════════════════════════════════
    // عمليات منطقية
    // ═══════════════════════════════════════════════════════════════
    
    And,            // و: %r = And(%a, %b)
    Or,             // أو: %r = Or(%a, %b)
    Not,            // ليس: %r = Not(%v)
    Xor,            // حصري XOR بتّي: %r = Xor(%a, %b)
    BitAnd,         // AND بتّي: %r = BitAnd(%a, %b)
    BitOr,          // OR بتّي: %r = BitOr(%a, %b)
    Shl,            // إزاحة يسار: %r = Shl(%a, %b)
    Shr,            // إزاحة يمين: %r = Shr(%a, %b)
    
    // ═══════════════════════════════════════════════════════════════
    // عمليات التحكم
    // ═══════════════════════════════════════════════════════════════
    
    Jump,           // قفز: Jump(كتلة)
    Branch,         // تفرع: Branch(%cond, كتلة_صح, كتلة_خطأ)
    Switch,         // تبديل: Switch(%v, [حالات])
    Return,         // إرجاع: Return(%v) أو Return()
    Unreachable,    // غير قابل للوصول
    
    // ═══════════════════════════════════════════════════════════════
    // عمليات الدوال
    // ═══════════════════════════════════════════════════════════════
    
    Call,           // استدعاء: %r = Call(دالة, [معاملات])
    CallIndirect,   // استدعاء غير مباشر: %r = CallIndirect(%fn, [معاملات])
    CallMethod,     // استدعاء تابع: %r = CallMethod(%obj, تابع, [معاملات])
    
    // ═══════════════════════════════════════════════════════════════
    // عمليات الأنواع
    // ═══════════════════════════════════════════════════════════════
    
    Cast,           // تحويل: %r = Cast(%v, نوع)
    TypeCheck,      // فحص نوع: %r = TypeCheck(%v, نوع)
    Sizeof,         // حجم: %r = Sizeof(نوع)
    
    // ═══════════════════════════════════════════════════════════════
    // عمليات التصحيح
    // ═══════════════════════════════════════════════════════════════
    
    DebugLoc,       // موقع تصحيح: DebugLoc(سطر، عمود)
    DebugValue,     // قيمة تصحيح: DebugValue(%v, اسم)
    Assert          // تأكيد: Assert(%cond, رسالة)
};

// =============================================================================
// معاملات العمليات
// =============================================================================

/**
 * معامل ثابت
 */
struct ConstantOperand {
    std::variant<
        bool,           // منطقي
        int64_t,        // عدد صحيح
        uint64_t,       // عدد طبيعي
        double,         // عشري
        std::string     // نص
    > value;
    std::shared_ptr<SirType> type;
    
    std::string toString() const {
        return std::visit([](auto&& v) -> std::string {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, bool>) {
                return v ? "صحيح" : "خطأ";
            } else if constexpr (std::is_same_v<T, std::string>) {
                return "\"" + v + "\"";
            } else {
                return std::to_string(v);
            }
        }, value);
    }
};

/**
 * معامل قيمة (مرجع لقيمة)
 */
struct ValueOperand {
    ValueId id;
    
    std::string toString() const {
        return "%" + std::to_string(id);
    }
};

/**
 * معامل كتلة
 */
struct BlockOperand {
    BlockId id;
    
    std::string toString() const {
        return "bb" + std::to_string(id);
    }
};

/**
 * معامل دالة
 */
struct FunctionOperand {
    std::string name;
    FunctionId id;
    
    std::string toString() const {
        return "@" + name;
    }
};

/**
 * معامل حقل
 */
struct FieldOperand {
    std::string name;
    int index;
    
    std::string toString() const {
        return "." + name;
    }
};

/**
 * معامل نوع
 */
struct TypeOperand {
    std::shared_ptr<SirType> type;
    
    std::string toString() const {
        return type ? type->toString() : "?";
    }
};

/**
 * اتحاد جميع المعاملات
 */
using Operand = std::variant<
    ConstantOperand,
    ValueOperand,
    BlockOperand,
    FunctionOperand,
    FieldOperand,
    TypeOperand
>;

// =============================================================================
// تعليمة SIR
// =============================================================================

/**
 * تعليمة SIR واحدة
 */
class SirInstruction {
public:
    Opcode opcode;
    std::optional<ValueId> result;      // القيمة الناتجة (إن وجدت)
    std::vector<Operand> operands;      // المعاملات
    SourceLocation location;            // موقع الكود الأصلي
    std::string comment;                // تعليق للتوثيق
    
    // ═══════════════════════════════════════════════════════════════
    // مُنشئات سريعة لعمليات الملكية الـ 12
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * 1. تخصيص (Alloc)
     */
    static SirInstruction Alloc(ValueId result, std::shared_ptr<SirType> type) {
        SirInstruction inst;
        inst.opcode = Opcode::Alloc;
        inst.result = result;
        inst.operands.push_back(TypeOperand{type});
        inst.comment = "تخصيص " + type->toString();
        return inst;
    }
    
    /**
     * 2. استعارة ثابتة (Borrow)
     */
    static SirInstruction Borrow(ValueId result, ValueId source) {
        SirInstruction inst;
        inst.opcode = Opcode::Borrow;
        inst.result = result;
        inst.operands.push_back(ValueOperand{source});
        inst.comment = "استعارة ثابتة";
        return inst;
    }
    
    /**
     * 3. استعارة متغيرة (BorrowMut)
     */
    static SirInstruction BorrowMut(ValueId result, ValueId source) {
        SirInstruction inst;
        inst.opcode = Opcode::BorrowMut;
        inst.result = result;
        inst.operands.push_back(ValueOperand{source});
        inst.comment = "استعارة متغيرة";
        return inst;
    }
    
    /**
     * 4. نقل (Move)
     */
    static SirInstruction Move(ValueId result, ValueId source) {
        SirInstruction inst;
        inst.opcode = Opcode::Move;
        inst.result = result;
        inst.operands.push_back(ValueOperand{source});
        inst.comment = "نقل الملكية";
        return inst;
    }
    
    /**
     * 5. نسخ (Copy)
     */
    static SirInstruction Copy(ValueId result, ValueId source) {
        SirInstruction inst;
        inst.opcode = Opcode::Copy;
        inst.result = result;
        inst.operands.push_back(ValueOperand{source});
        inst.comment = "نسخ القيمة";
        return inst;
    }
    
    /**
     * 6. إسقاط (Drop)
     */
    static SirInstruction Drop(ValueId value) {
        SirInstruction inst;
        inst.opcode = Opcode::Drop;
        inst.operands.push_back(ValueOperand{value});
        inst.comment = "إسقاط (تحرير)";
        return inst;
    }
    
    /**
     * 7. استنساخ (Clone)
     */
    static SirInstruction Clone(ValueId result, ValueId source) {
        SirInstruction inst;
        inst.opcode = Opcode::Clone;
        inst.result = result;
        inst.operands.push_back(ValueOperand{source});
        inst.comment = "استنساخ عميق";
        return inst;
    }
    
    /**
     * 8. إنهاء استعارة (EndBorrow)
     */
    static SirInstruction EndBorrow(ValueId borrow) {
        SirInstruction inst;
        inst.opcode = Opcode::EndBorrow;
        inst.operands.push_back(ValueOperand{borrow});
        inst.comment = "إنهاء الاستعارة";
        return inst;
    }
    
    /**
     * 9. إعادة استعارة (Reborrow)
     */
    static SirInstruction Reborrow(ValueId result, ValueId source) {
        SirInstruction inst;
        inst.opcode = Opcode::Reborrow;
        inst.result = result;
        inst.operands.push_back(ValueOperand{source});
        inst.comment = "إعادة استعارة";
        return inst;
    }
    
    /**
     * 10. إسقاط حقل (Project)
     */
    static SirInstruction Project(ValueId result, ValueId source, 
                                   const std::string& fieldName, int fieldIndex) {
        SirInstruction inst;
        inst.opcode = Opcode::Project;
        inst.result = result;
        inst.operands.push_back(ValueOperand{source});
        inst.operands.push_back(FieldOperand{fieldName, fieldIndex});
        inst.comment = "الوصول للحقل " + fieldName;
        return inst;
    }
    
    /**
     * 11. فك مرجع (Deref)
     */
    static SirInstruction Deref(ValueId result, ValueId ref) {
        SirInstruction inst;
        inst.opcode = Opcode::Deref;
        inst.result = result;
        inst.operands.push_back(ValueOperand{ref});
        inst.comment = "فك المرجع";
        return inst;
    }
    
    /**
     * 12. أخذ (Take)
     */
    static SirInstruction Take(ValueId result, ValueId container, int index) {
        SirInstruction inst;
        inst.opcode = Opcode::Take;
        inst.result = result;
        inst.operands.push_back(ValueOperand{container});
        inst.operands.push_back(ConstantOperand{static_cast<int64_t>(index), SirType::Int32()});
        inst.comment = "أخذ العنصر " + std::to_string(index);
        return inst;
    }
    
    // ═══════════════════════════════════════════════════════════════
    // مُنشئات أخرى
    // ═══════════════════════════════════════════════════════════════
    
    static SirInstruction Store(ValueId ptr, ValueId value) {
        SirInstruction inst;
        inst.opcode = Opcode::Store;
        inst.operands.push_back(ValueOperand{ptr});
        inst.operands.push_back(ValueOperand{value});
        inst.comment = "تخزين";
        return inst;
    }
    
    static SirInstruction Load(ValueId result, ValueId ptr) {
        SirInstruction inst;
        inst.opcode = Opcode::Load;
        inst.result = result;
        inst.operands.push_back(ValueOperand{ptr});
        inst.comment = "تحميل";
        return inst;
    }
    
    static SirInstruction Add(ValueId result, ValueId a, ValueId b) {
        SirInstruction inst;
        inst.opcode = Opcode::Add;
        inst.result = result;
        inst.operands.push_back(ValueOperand{a});
        inst.operands.push_back(ValueOperand{b});
        return inst;
    }
    
    static SirInstruction Return(std::optional<ValueId> value = std::nullopt) {
        SirInstruction inst;
        inst.opcode = Opcode::Return;
        if (value) {
            inst.operands.push_back(ValueOperand{*value});
        }
        return inst;
    }
    
    static SirInstruction Call(std::optional<ValueId> result, 
                               const std::string& funcName,
                               FunctionId funcId,
                               const std::vector<ValueId>& args) {
        SirInstruction inst;
        inst.opcode = Opcode::Call;
        inst.result = result;
        inst.operands.push_back(FunctionOperand{funcName, funcId});
        for (auto arg : args) {
            inst.operands.push_back(ValueOperand{arg});
        }
        return inst;
    }
    
    static SirInstruction Branch(ValueId cond, BlockId trueBlock, BlockId falseBlock) {
        SirInstruction inst;
        inst.opcode = Opcode::Branch;
        inst.operands.push_back(ValueOperand{cond});
        inst.operands.push_back(BlockOperand{trueBlock});
        inst.operands.push_back(BlockOperand{falseBlock});
        return inst;
    }
    
    static SirInstruction Jump(BlockId target) {
        SirInstruction inst;
        inst.opcode = Opcode::Jump;
        inst.operands.push_back(BlockOperand{target});
        return inst;
    }
    
    // ═══════════════════════════════════════════════════════════════
    // تمثيل نصي
    // ═══════════════════════════════════════════════════════════════
    
    std::string toString() const {
        std::string str;
        
        // النتيجة
        if (result) {
            str += "%" + std::to_string(*result) + " = ";
        }
        
        // اسم العملية
        str += opcodeToString(opcode);
        
        // المعاملات
        if (!operands.empty()) {
            str += "(";
            for (size_t i = 0; i < operands.size(); i++) {
                if (i > 0) str += ", ";
                str += operandToString(operands[i]);
            }
            str += ")";
        }
        
        // التعليق
        if (!comment.empty()) {
            str += "  ; " + comment;
        }
        
        return str;
    }
    
private:
    static std::string opcodeToString(Opcode op) {
        switch (op) {
            // عمليات الملكية
            case Opcode::Alloc: return "تخصيص";
            case Opcode::Borrow: return "استعارة";
            case Opcode::BorrowMut: return "استعارة_متغيرة";
            case Opcode::Move: return "نقل";
            case Opcode::Copy: return "نسخ";
            case Opcode::Drop: return "إسقاط";
            case Opcode::Clone: return "استنساخ";
            case Opcode::EndBorrow: return "إنهاء_استعارة";
            case Opcode::Reborrow: return "إعادة_استعارة";
            case Opcode::Project: return "حقل";
            case Opcode::Deref: return "فك_مرجع";
            case Opcode::Take: return "أخذ";
            
            // عمليات الذاكرة
            case Opcode::Load: return "تحميل";
            case Opcode::Store: return "تخزين";
            case Opcode::StackAlloc: return "تخصيص_مكدس";
            case Opcode::HeapAlloc: return "تخصيص_كومة";
            case Opcode::Free: return "تحرير";
            
            // عمليات حسابية
            case Opcode::Add: return "جمع";
            case Opcode::Sub: return "طرح";
            case Opcode::Mul: return "ضرب";
            case Opcode::Div: return "قسمة";
            case Opcode::Mod: return "باقي";
            case Opcode::Neg: return "سالب";
            
            // عمليات المقارنة
            case Opcode::Eq: return "يساوي";
            case Opcode::Ne: return "لا_يساوي";
            case Opcode::Lt: return "أقل";
            case Opcode::Le: return "أقل_أو_يساوي";
            case Opcode::Gt: return "أكبر";
            case Opcode::Ge: return "أكبر_أو_يساوي";
            
            // عمليات منطقية وبتية
            case Opcode::And: return "و";
            case Opcode::Or: return "أو";
            case Opcode::Not: return "ليس";
            case Opcode::Xor: return "حصري";
            case Opcode::BitAnd: return "بت_و";
            case Opcode::BitOr: return "بت_أو";
            case Opcode::Shl: return "إزاحة_يسار";
            case Opcode::Shr: return "إزاحة_يمين";
            
            // عمليات التحكم
            case Opcode::Jump: return "قفز";
            case Opcode::Branch: return "تفرع";
            case Opcode::Switch: return "تبديل";
            case Opcode::Return: return "أرجع";
            case Opcode::Unreachable: return "غير_قابل_للوصول";
            
            // عمليات الدوال
            case Opcode::Call: return "استدعاء";
            case Opcode::CallIndirect: return "استدعاء_غير_مباشر";
            case Opcode::CallMethod: return "استدعاء_تابع";
            
            // عمليات الأنواع
            case Opcode::Cast: return "تحويل";
            case Opcode::TypeCheck: return "فحص_نوع";
            case Opcode::Sizeof: return "حجم";
            
            // عمليات التصحيح
            case Opcode::DebugLoc: return "موقع_تصحيح";
            case Opcode::DebugValue: return "قيمة_تصحيح";
            case Opcode::Assert: return "تأكيد";
            
            default: return "مجهول";
        }
    }
    
    static std::string operandToString(const Operand& op) {
        return std::visit([](auto&& o) { return o.toString(); }, op);
    }
};

// =============================================================================
// كتلة أساسية
// =============================================================================

/**
 * كتلة أساسية (Basic Block)
 * 
 * مجموعة من التعليمات تُنفذ بالتتابع
 */
class SirBasicBlock {
public:
    BlockId id;
    std::string name;
    std::vector<SirInstruction> instructions;
    std::vector<BlockId> predecessors;  // الكتل السابقة
    std::vector<BlockId> successors;    // الكتل اللاحقة
    
    void addInstruction(const SirInstruction& inst) {
        instructions.push_back(inst);
    }
    
    bool isEmpty() const {
        return instructions.empty();
    }
    
    bool hasTerminator() const {
        if (instructions.empty()) return false;
        auto op = instructions.back().opcode;
        return op == Opcode::Return || 
               op == Opcode::Jump || 
               op == Opcode::Branch ||
               op == Opcode::Unreachable;
    }
    
    std::string toString() const {
        std::string str = name + ":\n";
        for (const auto& inst : instructions) {
            str += "  " + inst.toString() + "\n";
        }
        return str;
    }
};

// =============================================================================
// دالة SIR
// =============================================================================

/**
 * دالة SIR
 */
class SirFunction {
public:
    FunctionId id;
    std::string name;
    std::vector<SirParameter> params;
    std::shared_ptr<SirType> returnType;
    std::vector<SirBasicBlock> blocks;
    std::map<ValueId, SirValue> localValues;
    SourceLocation location;
    
    // إضافة كتلة
    BlockId addBlock(const std::string& name = "") {
        BlockId bid = blocks.size();
        SirBasicBlock block;
        block.id = bid;
        block.name = name.empty() ? "bb" + std::to_string(bid) : name;
        blocks.push_back(block);
        return bid;
    }
    
    // الحصول على كتلة
    SirBasicBlock* getBlock(BlockId id) {
        if (id < blocks.size()) {
            return &blocks[id];
        }
        return nullptr;
    }
    
    // كتلة الدخول
    SirBasicBlock* entryBlock() {
        return blocks.empty() ? nullptr : &blocks[0];
    }
    
    std::string toString() const {
        std::string str = "دالة " + name + "(";
        
        for (size_t i = 0; i < params.size(); i++) {
            if (i > 0) str += "، ";
            str += params[i].name + ": " + params[i].type->toString();
        }
        
        str += ")";
        if (returnType && returnType->kind != TypeKind::Void) {
            str += " -> " + returnType->toString();
        }
        str += " {\n";
        
        for (const auto& block : blocks) {
            str += block.toString();
        }
        
        str += "}\n";
        return str;
    }
};

// =============================================================================
// وحدة SIR
// =============================================================================

/**
 * وحدة SIR (ملف كامل)
 */
class SirModule {
public:
    std::string name;
    std::vector<SirFunction> functions;
    std::map<std::string, std::shared_ptr<SirType>> types;
    std::map<std::string, ConstantOperand> constants;
    
    // إضافة دالة
    FunctionId addFunction(const SirFunction& func) {
        FunctionId fid = functions.size();
        functions.push_back(func);
        functions.back().id = fid;
        return fid;
    }
    
    // البحث عن دالة
    SirFunction* getFunction(const std::string& name) {
        for (auto& f : functions) {
            if (f.name == name) return &f;
        }
        return nullptr;
    }
    
    std::string toString() const {
        std::string str = "; وحدة: " + name + "\n\n";
        
        for (const auto& func : functions) {
            str += func.toString() + "\n";
        }
        
        return str;
    }
};

} // namespace sad::sir

#endif // SAD_SIR_OPCODES_H
