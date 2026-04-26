/**
 * =============================================================================
 * ملف: arm64.cpp
 * الوصف: Backend لمعالجات ARM64 (AArch64)
 * المهمة: T238 - ARM64 target backend
 * المرحلة: Phase 24 - User Story 21 (Cross-Platform Build --كل-الأهداف)
 * =============================================================================
 * 
 * 📱 دليل المبتدئ لمعمارية ARM64
 * ═══════════════════════════════
 * 
 * ما هو ARM64؟
 * ─────────────
 * معمارية معالجات موفرة للطاقة وقوية.
 * تُسمى أيضاً: AArch64, ARMv8-A
 * 
 * أين نجدها؟
 * ───────────
 * - Apple Silicon (M1, M2, M3)
 * - هواتف Android الحديثة
 * - iPhone و iPad
 * - Raspberry Pi 4/5
 * - AWS Graviton
 * 
 * المسجلات (31 مسجل عام!):
 * ──────────────────────────
 * X0-X30  - 64-bit عام
 * W0-W30  - 32-bit (نصف X)
 * SP      - مؤشر المكدس
 * PC      - عداد البرنامج
 * XZR/WZR - مسجل الصفر
 * 
 * مسجلات النقطة العائمة:
 * ───────────────────────
 * V0-V31  - 128-bit SIMD
 * D0-D31  - 64-bit double
 * S0-S31  - 32-bit float
 * 
 * اتفاقية الاستدعاء AAPCS64:
 * ───────────────────────────
 * المعطيات: X0-X7 (أو V0-V7 للعشري)
 * الإرجاع: X0 (أو V0)
 * المحفوظة: X19-X28, SP
 * 
 * ميزات ARM64:
 * ─────────────
 * ✓ كفاءة طاقة عالية
 * ✓ تعليمات ثابتة الحجم (4 bytes)
 * ✓ SIMD متقدم (NEON)
 * ✓ عنونة مرنة
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <cstdint>

namespace sad::targets {

// =============================================================================
// مسجلات ARM64
// =============================================================================

enum class ARM64Register {
    // General Purpose 64-bit (X0-X30)
    X0, X1, X2, X3, X4, X5, X6, X7,
    X8, X9, X10, X11, X12, X13, X14, X15,
    X16, X17, X18, X19, X20, X21, X22, X23,
    X24, X25, X26, X27, X28, X29, X30,
    
    // 32-bit versions (W0-W30)
    W0, W1, W2, W3, W4, W5, W6, W7,
    W8, W9, W10, W11, W12, W13, W14, W15,
    W16, W17, W18, W19, W20, W21, W22, W23,
    W24, W25, W26, W27, W28, W29, W30,
    
    // Special registers
    SP,     // Stack Pointer
    XZR,    // Zero Register (64-bit)
    WZR,    // Zero Register (32-bit)
    LR,     // Link Register (X30)
    FP,     // Frame Pointer (X29)
    
    // SIMD/FP registers
    V0, V1, V2, V3, V4, V5, V6, V7,
    V8, V9, V10, V11, V12, V13, V14, V15,
    V16, V17, V18, V19, V20, V21, V22, V23,
    V24, V25, V26, V27, V28, V29, V30, V31,
    
    // 64-bit FP (D0-D31)
    D0, D1, D2, D3, D4, D5, D6, D7,
    
    // 32-bit FP (S0-S31)
    S0, S1, S2, S3, S4, S5, S6, S7
};

// =============================================================================
// أنواع العمليات
// =============================================================================

enum class ARM64OpType {
    // نقل البيانات
    MOV,
    MOVZ,   // Move with zero
    MOVK,   // Move with keep
    MOVN,   // Move with NOT
    LDR,    // Load
    STR,    // Store
    LDP,    // Load pair
    STP,    // Store pair
    ADRP,   // Address of page
    
    // الحساب
    ADD,
    ADDS,   // Add with flags
    SUB,
    SUBS,   // Sub with flags
    MUL,
    SDIV,   // Signed divide
    UDIV,   // Unsigned divide
    MADD,   // Multiply-add
    MSUB,   // Multiply-sub
    NEG,
    
    // المنطق
    AND,
    ORR,
    EOR,    // XOR
    BIC,    // Bit clear
    LSL,    // Left shift
    LSR,    // Logical right shift
    ASR,    // Arithmetic right shift
    
    // المقارنة
    CMP,
    CMN,    // Compare negative
    TST,    // Test bits
    
    // القفز
    B,      // Branch
    BL,     // Branch with link (call)
    BR,     // Branch to register
    BLR,    // Branch with link to register
    RET,
    
    // الشرطي
    B_EQ, B_NE, B_GT, B_GE, B_LT, B_LE,
    B_HI, B_HS, B_LO, B_LS,
    
    // التحديد الشرطي
    CSEL,   // Conditional select
    CSINC,  // Conditional select increment
    CSET,   // Conditional set
    
    // SIMD
    FADD,
    FSUB,
    FMUL,
    FDIV,
    FCMP,
    FMOV,
    
    // النظام
    SVC,    // Supervisor call
    NOP,
    BRK     // Breakpoint
};

// =============================================================================
// شروط ARM64
// =============================================================================

enum class ARM64Condition {
    EQ,  // Equal
    NE,  // Not equal
    GT,  // Greater than (signed)
    GE,  // Greater or equal (signed)
    LT,  // Less than (signed)
    LE,  // Less or equal (signed)
    HI,  // Higher (unsigned >)
    HS,  // Higher or same (unsigned >=)
    LO,  // Lower (unsigned <)
    LS,  // Lower or same (unsigned <=)
    AL   // Always
};

// =============================================================================
// معامل العملية
// =============================================================================

struct ARM64Operand {
    enum Type {
        Register,
        Immediate,
        Memory,
        Label,
        Shifted,
        Extended
    } type;
    
    ARM64Register reg;
    int64_t immediate;
    std::string label;
    
    // للذاكرة: [base, #offset] أو [base, index]
    ARM64Register base;
    ARM64Register index;
    int offset;
    bool preIndex;   // [base, #offset]!
    bool postIndex;  // [base], #offset
    
    // للإزاحة
    int shift;
    enum { LSL, LSR, ASR, ROR } shiftType;
    
    /**
     * إنشاء معامل مسجل
     */
    static ARM64Operand Reg(ARM64Register r) {
        ARM64Operand op;
        op.type = Register;
        op.reg = r;
        return op;
    }
    
    /**
     * إنشاء معامل فوري
     */
    static ARM64Operand Imm(int64_t val) {
        ARM64Operand op;
        op.type = Immediate;
        op.immediate = val;
        return op;
    }
    
    /**
     * إنشاء معامل ذاكرة
     */
    static ARM64Operand Mem(ARM64Register base, int offset = 0) {
        ARM64Operand op;
        op.type = Memory;
        op.base = base;
        op.offset = offset;
        op.preIndex = false;
        op.postIndex = false;
        return op;
    }
    
    /**
     * إنشاء تسمية
     */
    static ARM64Operand Lbl(const std::string& name) {
        ARM64Operand op;
        op.type = Label;
        op.label = name;
        return op;
    }
    
    /**
     * تحويل لنص
     */
    std::string toString() const;
};

// =============================================================================
// تعليمة واحدة
// =============================================================================

struct ARM64Instruction {
    ARM64OpType op;
    std::vector<ARM64Operand> operands;
    ARM64Condition condition;
    std::string comment;
    
    ARM64Instruction() : condition(ARM64Condition::AL) {}
    
    /**
     * تحويل لنص Assembly
     */
    std::string toAsm() const;
};

// =============================================================================
// اتفاقية الاستدعاء AAPCS64
// =============================================================================

class ARM64CallingConvention {
public:
    /**
     * مسجلات المعطيات
     */
    std::vector<ARM64Register> argRegisters() const {
        return {ARM64Register::X0, ARM64Register::X1, ARM64Register::X2,
                ARM64Register::X3, ARM64Register::X4, ARM64Register::X5,
                ARM64Register::X6, ARM64Register::X7};
    }
    
    /**
     * مسجلات المعطيات العشرية
     */
    std::vector<ARM64Register> fpArgRegisters() const {
        return {ARM64Register::V0, ARM64Register::V1, ARM64Register::V2,
                ARM64Register::V3, ARM64Register::V4, ARM64Register::V5,
                ARM64Register::V6, ARM64Register::V7};
    }
    
    /**
     * مسجلات الإرجاع
     */
    std::vector<ARM64Register> returnRegisters() const {
        return {ARM64Register::X0, ARM64Register::X1};
    }
    
    /**
     * المسجلات المحفوظة (callee-saved)
     */
    std::vector<ARM64Register> calleeSavedRegisters() const {
        return {ARM64Register::X19, ARM64Register::X20, ARM64Register::X21,
                ARM64Register::X22, ARM64Register::X23, ARM64Register::X24,
                ARM64Register::X25, ARM64Register::X26, ARM64Register::X27,
                ARM64Register::X28};
    }
    
    /**
     * محاذاة المكدس
     */
    int stackAlignment() const {
        return 16;
    }
};

// =============================================================================
// مولد كود ARM64
// =============================================================================

class ARM64CodeGen {
public:
    ARM64CodeGen() {}
    
    /**
     * بداية دالة
     */
    void emitFunctionPrologue(const std::string& name, int localSize = 0) {
        // التسمية
        emitLabel(name);
        
        // حفظ FP و LR
        emit(ARM64OpType::STP, {
            ARM64Operand::Reg(ARM64Register::FP),
            ARM64Operand::Reg(ARM64Register::LR),
            ARM64Operand::Mem(ARM64Register::SP, -16)
        }, "حفظ FP و LR");
        
        // تحديث SP
        instructions_.back().operands[2].preIndex = true;
        
        // إعداد FP
        emit(ARM64OpType::MOV, {
            ARM64Operand::Reg(ARM64Register::FP),
            ARM64Operand::Reg(ARM64Register::SP)
        }, "إعداد Frame Pointer");
        
        // حجز المتغيرات المحلية
        if (localSize > 0) {
            int aligned = (localSize + 15) & ~15;
            emit(ARM64OpType::SUB, {
                ARM64Operand::Reg(ARM64Register::SP),
                ARM64Operand::Reg(ARM64Register::SP),
                ARM64Operand::Imm(aligned)
            }, "حجز المتغيرات المحلية");
        }
    }
    
    /**
     * نهاية دالة
     */
    void emitFunctionEpilogue() {
        // استعادة SP من FP
        emit(ARM64OpType::MOV, {
            ARM64Operand::Reg(ARM64Register::SP),
            ARM64Operand::Reg(ARM64Register::FP)
        }, "استعادة المكدس");
        
        // استعادة FP و LR
        emit(ARM64OpType::LDP, {
            ARM64Operand::Reg(ARM64Register::FP),
            ARM64Operand::Reg(ARM64Register::LR),
            ARM64Operand::Mem(ARM64Register::SP, 16)
        }, "استعادة FP و LR");
        instructions_.back().operands[2].postIndex = true;
        
        // العودة
        emit(ARM64OpType::RET, {}, "العودة");
    }
    
    /**
     * تحميل قيمة
     */
    void emitLoad(ARM64Register dest, ARM64Register base, int offset) {
        emit(ARM64OpType::LDR, {
            ARM64Operand::Reg(dest),
            ARM64Operand::Mem(base, offset)
        });
    }
    
    /**
     * تخزين قيمة
     */
    void emitStore(ARM64Register base, int offset, ARM64Register src) {
        emit(ARM64OpType::STR, {
            ARM64Operand::Reg(src),
            ARM64Operand::Mem(base, offset)
        });
    }
    
    /**
     * نقل قيمة
     */
    void emitMove(ARM64Register dest, ARM64Register src) {
        emit(ARM64OpType::MOV, {
            ARM64Operand::Reg(dest),
            ARM64Operand::Reg(src)
        });
    }
    
    /**
     * نقل قيمة فورية
     */
    void emitMoveImm(ARM64Register dest, int64_t value) {
        if (value >= 0 && value < 65536) {
            emit(ARM64OpType::MOVZ, {
                ARM64Operand::Reg(dest),
                ARM64Operand::Imm(value)
            });
        } else {
            // تحميل متعدد المراحل
            emit(ARM64OpType::MOVZ, {
                ARM64Operand::Reg(dest),
                ARM64Operand::Imm(value & 0xFFFF)
            });
            if (value > 0xFFFF) {
                emit(ARM64OpType::MOVK, {
                    ARM64Operand::Reg(dest),
                    ARM64Operand::Imm((value >> 16) & 0xFFFF)
                });
            }
        }
    }
    
    /**
     * جمع
     */
    void emitAdd(ARM64Register dest, ARM64Register a, ARM64Register b) {
        emit(ARM64OpType::ADD, {
            ARM64Operand::Reg(dest),
            ARM64Operand::Reg(a),
            ARM64Operand::Reg(b)
        });
    }
    
    /**
     * جمع مع فوري
     */
    void emitAddImm(ARM64Register dest, ARM64Register src, int64_t imm) {
        emit(ARM64OpType::ADD, {
            ARM64Operand::Reg(dest),
            ARM64Operand::Reg(src),
            ARM64Operand::Imm(imm)
        });
    }
    
    /**
     * طرح
     */
    void emitSub(ARM64Register dest, ARM64Register a, ARM64Register b) {
        emit(ARM64OpType::SUB, {
            ARM64Operand::Reg(dest),
            ARM64Operand::Reg(a),
            ARM64Operand::Reg(b)
        });
    }
    
    /**
     * ضرب
     */
    void emitMul(ARM64Register dest, ARM64Register a, ARM64Register b) {
        emit(ARM64OpType::MUL, {
            ARM64Operand::Reg(dest),
            ARM64Operand::Reg(a),
            ARM64Operand::Reg(b)
        });
    }
    
    /**
     * مقارنة
     */
    void emitCmp(ARM64Register a, ARM64Register b) {
        emit(ARM64OpType::CMP, {
            ARM64Operand::Reg(a),
            ARM64Operand::Reg(b)
        });
    }
    
    /**
     * قفز غير مشروط
     */
    void emitBranch(const std::string& label) {
        emit(ARM64OpType::B, {ARM64Operand::Lbl(label)});
    }
    
    /**
     * قفز مشروط
     */
    void emitBranchCond(ARM64Condition cond, const std::string& label) {
        ARM64OpType op;
        switch (cond) {
            case ARM64Condition::EQ: op = ARM64OpType::B_EQ; break;
            case ARM64Condition::NE: op = ARM64OpType::B_NE; break;
            case ARM64Condition::GT: op = ARM64OpType::B_GT; break;
            case ARM64Condition::GE: op = ARM64OpType::B_GE; break;
            case ARM64Condition::LT: op = ARM64OpType::B_LT; break;
            case ARM64Condition::LE: op = ARM64OpType::B_LE; break;
            default: op = ARM64OpType::B;
        }
        emit(op, {ARM64Operand::Lbl(label)});
    }
    
    /**
     * استدعاء دالة
     */
    void emitCall(const std::string& func) {
        emit(ARM64OpType::BL, {ARM64Operand::Lbl(func)});
    }
    
    /**
     * إصدار تسمية
     */
    void emitLabel(const std::string& name) {
        labels_.push_back(name);
        ARM64Instruction inst;
        inst.comment = "label:" + name;
        instructions_.push_back(inst);
    }
    
    /**
     * إصدار تعليمة
     */
    void emit(ARM64OpType op, std::vector<ARM64Operand> operands,
              const std::string& comment = "") {
        ARM64Instruction inst;
        inst.op = op;
        inst.operands = operands;
        inst.comment = comment;
        instructions_.push_back(inst);
    }
    
    /**
     * الحصول على الكود المُولد
     */
    std::string getAssembly() const {
        std::ostringstream ss;
        
        ss << "; كود مُولد للغة ص - ARM64 (AArch64)\n";
        ss << "; اتفاقية الاستدعاء: AAPCS64\n\n";
        
        ss << ".text\n\n";
        
        for (const auto& inst : instructions_) {
            ss << inst.toAsm() << "\n";
        }
        
        return ss.str();
    }
    
private:
    std::vector<ARM64Instruction> instructions_;
    std::vector<std::string> labels_;
};

// =============================================================================
// تحويل لنص
// =============================================================================

std::string registerToString(ARM64Register reg) {
    switch (reg) {
        case ARM64Register::X0: return "x0";
        case ARM64Register::X1: return "x1";
        case ARM64Register::X2: return "x2";
        case ARM64Register::X3: return "x3";
        case ARM64Register::X4: return "x4";
        case ARM64Register::X5: return "x5";
        case ARM64Register::X6: return "x6";
        case ARM64Register::X7: return "x7";
        case ARM64Register::X19: return "x19";
        case ARM64Register::X20: return "x20";
        case ARM64Register::X29: return "x29";
        case ARM64Register::X30: return "x30";
        case ARM64Register::SP: return "sp";
        case ARM64Register::FP: return "x29";
        case ARM64Register::LR: return "x30";
        case ARM64Register::XZR: return "xzr";
        // ... المزيد
        default: return "?";
    }
}

std::string opToString(ARM64OpType op) {
    switch (op) {
        case ARM64OpType::MOV: return "mov";
        case ARM64OpType::MOVZ: return "movz";
        case ARM64OpType::MOVK: return "movk";
        case ARM64OpType::LDR: return "ldr";
        case ARM64OpType::STR: return "str";
        case ARM64OpType::LDP: return "ldp";
        case ARM64OpType::STP: return "stp";
        case ARM64OpType::ADD: return "add";
        case ARM64OpType::SUB: return "sub";
        case ARM64OpType::MUL: return "mul";
        case ARM64OpType::CMP: return "cmp";
        case ARM64OpType::B: return "b";
        case ARM64OpType::BL: return "bl";
        case ARM64OpType::RET: return "ret";
        case ARM64OpType::B_EQ: return "b.eq";
        case ARM64OpType::B_NE: return "b.ne";
        case ARM64OpType::B_GT: return "b.gt";
        case ARM64OpType::B_LT: return "b.lt";
        default: return "?";
    }
}

std::string ARM64Operand::toString() const {
    switch (type) {
        case Register:
            return registerToString(reg);
        case Immediate:
            return "#" + std::to_string(immediate);
        case Memory:
            if (preIndex) {
                return "[" + registerToString(base) + ", #" + std::to_string(offset) + "]!";
            } else if (postIndex) {
                return "[" + registerToString(base) + "], #" + std::to_string(offset);
            } else if (offset != 0) {
                return "[" + registerToString(base) + ", #" + std::to_string(offset) + "]";
            }
            return "[" + registerToString(base) + "]";
        case Label:
            return label;
        default:
            return "?";
    }
}

std::string ARM64Instruction::toAsm() const {
    // تسمية؟
    if (comment.find("label:") == 0) {
        return comment.substr(6) + ":";
    }
    
    std::ostringstream ss;
    ss << "    " << opToString(op);
    
    for (size_t i = 0; i < operands.size(); i++) {
        if (i == 0) ss << " ";
        else ss << ", ";
        ss << operands[i].toString();
    }
    
    if (!comment.empty()) {
        ss << "    ; " << comment;
    }
    
    return ss.str();
}

} // namespace sad::targets

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadARM64CodeGen {
    sad::targets::ARM64CodeGen* gen;
};

SadARM64CodeGen* sad_arm64_codegen_new() {
    auto* ctx = new SadARM64CodeGen();
    ctx->gen = new sad::targets::ARM64CodeGen();
    return ctx;
}

void sad_arm64_emit_function(SadARM64CodeGen* ctx, const char* name) {
    ctx->gen->emitFunctionPrologue(name);
}

const char* sad_arm64_get_asm(SadARM64CodeGen* ctx) {
    static std::string result;
    result = ctx->gen->getAssembly();
    return result.c_str();
}

void sad_arm64_codegen_free(SadARM64CodeGen* ctx) {
    if (ctx) {
        delete ctx->gen;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef ARM64_TEST

#include <cassert>

void testCodeGeneration() {
    std::cout << "🔧 اختبار توليد كود ARM64...\n";
    
    sad::targets::ARM64CodeGen gen;
    
    gen.emitFunctionPrologue("main", 32);
    gen.emitMoveImm(sad::targets::ARM64Register::X0, 42);
    gen.emitFunctionEpilogue();
    
    std::string asm_code = gen.getAssembly();
    
    assert(asm_code.find("main:") != std::string::npos);
    assert(asm_code.find("stp") != std::string::npos);
    assert(asm_code.find("ret") != std::string::npos);
    
    std::cout << "   ✅ نجح\n";
}

void testCallingConvention() {
    std::cout << "📞 اختبار AAPCS64...\n";
    
    sad::targets::ARM64CallingConvention cc;
    
    assert(cc.argRegisters()[0] == sad::targets::ARM64Register::X0);
    assert(cc.stackAlignment() == 16);
    assert(cc.calleeSavedRegisters().size() >= 10);
    
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات Backend ARM64\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testCodeGeneration();
    testCallingConvention();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // ARM64_TEST
