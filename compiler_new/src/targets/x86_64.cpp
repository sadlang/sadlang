/**
 * =============================================================================
 * ملف: x86_64.cpp
 * الوصف: Backend لمعالجات x86_64 (Intel/AMD 64-bit)
 * المهمة: T237 - x86_64 target backend
 * المرحلة: Phase 24 - User Story 21 (Cross-Platform Build --كل-الأهداف)
 * =============================================================================
 * 
 * 💻 دليل المبتدئ لمعمارية x86_64
 * ════════════════════════════════
 * 
 * ما هو x86_64؟
 * ──────────────
 * معمارية المعالجات الأكثر شيوعاً لأجهزة الكمبيوتر.
 * تُسمى أيضاً: AMD64, x64, Intel 64
 * 
 * أين نجدها؟
 * ───────────
 * - معظم أجهزة الكمبيوتر المكتبية
 * - معظم الخوادم
 * - اللابتوبات غير Mac M1/M2/M3
 * 
 * المسجلات (Registers):
 * ──────────────────────
 * المسجلات = ذاكرة فائقة السرعة داخل المعالج
 * 
 * عامة (16 مسجل):
 *   RAX, RBX, RCX, RDX  - العمليات الحسابية
 *   RSI, RDI           - مصدر ووجهة النسخ
 *   RBP                - قاعدة المكدس
 *   RSP                - مؤشر المكدس
 *   R8-R15             - إضافية
 * 
 * SSE/AVX (للعمليات العشرية):
 *   XMM0-XMM15         - 128-bit
 *   YMM0-YMM15         - 256-bit (AVX)
 *   ZMM0-ZMM31         - 512-bit (AVX-512)
 * 
 * اتفاقية الاستدعاء:
 * ───────────────────
 * Windows:
 *   المعطيات: RCX, RDX, R8, R9, ثم المكدس
 *   الإرجاع: RAX
 * 
 * Linux/Mac (System V):
 *   المعطيات: RDI, RSI, RDX, RCX, R8, R9
 *   الإرجاع: RAX, RDX
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
// المسجلات
// =============================================================================

enum class X86Register {
    // General Purpose 64-bit
    RAX, RBX, RCX, RDX,
    RSI, RDI, RBP, RSP,
    R8, R9, R10, R11,
    R12, R13, R14, R15,
    
    // 32-bit versions
    EAX, EBX, ECX, EDX,
    ESI, EDI, EBP, ESP,
    R8D, R9D, R10D, R11D,
    R12D, R13D, R14D, R15D,
    
    // 16-bit versions
    AX, BX, CX, DX,
    SI, DI, BP, SP,
    
    // 8-bit versions
    AL, AH, BL, BH,
    CL, CH, DL, DH,
    
    // SSE/AVX
    XMM0, XMM1, XMM2, XMM3,
    XMM4, XMM5, XMM6, XMM7,
    XMM8, XMM9, XMM10, XMM11,
    XMM12, XMM13, XMM14, XMM15,
    
    // Flags
    RFLAGS,
    
    // Instruction Pointer
    RIP
};

// =============================================================================
// أنواع العمليات
// =============================================================================

enum class X86OpType {
    // نقل البيانات
    MOV,
    MOVZX,
    MOVSX,
    LEA,
    PUSH,
    POP,
    
    // الحساب
    ADD,
    SUB,
    MUL,
    IMUL,
    DIV,
    IDIV,
    INC,
    DEC,
    NEG,
    
    // المنطق
    AND,
    OR,
    XOR,
    NOT,
    SHL,
    SHR,
    SAR,
    
    // المقارنة
    CMP,
    TEST,
    
    // القفز
    JMP,
    JE, JNE,
    JG, JGE, JL, JLE,
    JA, JAE, JB, JBE,
    JZ, JNZ,
    
    // الاستدعاء
    CALL,
    RET,
    
    // SSE
    MOVSS,
    MOVSD,
    ADDSS,
    SUBSS,
    MULSS,
    DIVSS,
    
    // النظام
    SYSCALL,
    NOP,
    INT
};

// =============================================================================
// معامل العملية
// =============================================================================

struct X86Operand {
    enum Type {
        Register,
        Immediate,
        Memory,
        Label
    } type;
    
    X86Register reg;
    int64_t immediate;
    std::string label;
    
    // للذاكرة: [base + index * scale + displacement]
    X86Register base;
    X86Register index;
    int scale;
    int displacement;
    
    /**
     * إنشاء معامل مسجل
     */
    static X86Operand Reg(X86Register r) {
        X86Operand op;
        op.type = Register;
        op.reg = r;
        return op;
    }
    
    /**
     * إنشاء معامل فوري
     */
    static X86Operand Imm(int64_t val) {
        X86Operand op;
        op.type = Immediate;
        op.immediate = val;
        return op;
    }
    
    /**
     * إنشاء معامل ذاكرة
     */
    static X86Operand Mem(X86Register base, int disp = 0) {
        X86Operand op;
        op.type = Memory;
        op.base = base;
        op.displacement = disp;
        op.scale = 1;
        return op;
    }
    
    /**
     * إنشاء تسمية
     */
    static X86Operand Lbl(const std::string& name) {
        X86Operand op;
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

struct X86Instruction {
    X86OpType op;
    std::vector<X86Operand> operands;
    std::string comment;
    
    /**
     * تحويل لنص Assembly
     */
    std::string toAsm() const;
};

// =============================================================================
// اتفاقية الاستدعاء
// =============================================================================

enum class CallingConvention {
    SystemV,    // Linux, macOS, BSD
    Win64,      // Windows x64
    Cdecl,      // C calling convention (32-bit)
    Stdcall     // Windows stdcall
};

class X86CallingConvention {
public:
    X86CallingConvention(CallingConvention conv = CallingConvention::SystemV)
        : convention_(conv) {
        setupRegisters();
    }
    
    /**
     * مسجلات المعطيات
     */
    const std::vector<X86Register>& argRegisters() const {
        return argRegs_;
    }
    
    /**
     * مسجلات الإرجاع
     */
    const std::vector<X86Register>& returnRegisters() const {
        return returnRegs_;
    }
    
    /**
     * المسجلات المحفوظة (callee-saved)
     */
    const std::vector<X86Register>& calleeSavedRegisters() const {
        return calleeSaved_;
    }
    
    /**
     * محاذاة المكدس
     */
    int stackAlignment() const {
        return 16;  // دائماً 16 bytes في x86_64
    }
    
    /**
     * منطقة الظل (Shadow Space)
     */
    int shadowSpace() const {
        return convention_ == CallingConvention::Win64 ? 32 : 0;
    }
    
private:
    CallingConvention convention_;
    std::vector<X86Register> argRegs_;
    std::vector<X86Register> returnRegs_;
    std::vector<X86Register> calleeSaved_;
    
    void setupRegisters() {
        if (convention_ == CallingConvention::Win64) {
            // Windows x64
            argRegs_ = {X86Register::RCX, X86Register::RDX, 
                       X86Register::R8, X86Register::R9};
            returnRegs_ = {X86Register::RAX};
            calleeSaved_ = {X86Register::RBX, X86Register::RBP,
                          X86Register::RDI, X86Register::RSI,
                          X86Register::R12, X86Register::R13,
                          X86Register::R14, X86Register::R15};
        } else {
            // System V (Linux, macOS)
            argRegs_ = {X86Register::RDI, X86Register::RSI,
                       X86Register::RDX, X86Register::RCX,
                       X86Register::R8, X86Register::R9};
            returnRegs_ = {X86Register::RAX, X86Register::RDX};
            calleeSaved_ = {X86Register::RBX, X86Register::RBP,
                          X86Register::R12, X86Register::R13,
                          X86Register::R14, X86Register::R15};
        }
    }
};

// =============================================================================
// مولد كود x86_64
// =============================================================================

class X86_64CodeGen {
public:
    X86_64CodeGen(CallingConvention cc = CallingConvention::SystemV)
        : callConv_(cc) {}
    
    /**
     * بداية دالة
     */
    void emitFunctionPrologue(const std::string& name, int localSize = 0) {
        // التسمية
        emitLabel(name);
        
        // حفظ RBP
        emit(X86OpType::PUSH, {X86Operand::Reg(X86Register::RBP)}, "حفظ قاعدة المكدس");
        emit(X86OpType::MOV, {X86Operand::Reg(X86Register::RBP), 
                             X86Operand::Reg(X86Register::RSP)}, "إعداد الإطار");
        
        // حجز المتغيرات المحلية
        if (localSize > 0) {
            // محاذاة 16
            int aligned = (localSize + 15) & ~15;
            emit(X86OpType::SUB, {X86Operand::Reg(X86Register::RSP),
                                 X86Operand::Imm(aligned)}, "حجز المتغيرات المحلية");
        }
        
        // Shadow space for Windows
        if (callConv_.shadowSpace() > 0) {
            emit(X86OpType::SUB, {X86Operand::Reg(X86Register::RSP),
                                 X86Operand::Imm(callConv_.shadowSpace())}, "Shadow space");
        }
    }
    
    /**
     * نهاية دالة
     */
    void emitFunctionEpilogue() {
        emit(X86OpType::MOV, {X86Operand::Reg(X86Register::RSP),
                             X86Operand::Reg(X86Register::RBP)}, "استعادة المكدس");
        emit(X86OpType::POP, {X86Operand::Reg(X86Register::RBP)}, "استعادة القاعدة");
        emit(X86OpType::RET, {}, "العودة");
    }
    
    /**
     * تحميل قيمة في مسجل
     */
    void emitLoad(X86Register dest, X86Register base, int offset) {
        emit(X86OpType::MOV, {X86Operand::Reg(dest), 
                             X86Operand::Mem(base, offset)});
    }
    
    /**
     * تخزين قيمة من مسجل
     */
    void emitStore(X86Register base, int offset, X86Register src) {
        emit(X86OpType::MOV, {X86Operand::Mem(base, offset),
                             X86Operand::Reg(src)});
    }
    
    /**
     * جمع
     */
    void emitAdd(X86Register dest, X86Register src) {
        emit(X86OpType::ADD, {X86Operand::Reg(dest), X86Operand::Reg(src)});
    }
    
    /**
     * طرح
     */
    void emitSub(X86Register dest, X86Register src) {
        emit(X86OpType::SUB, {X86Operand::Reg(dest), X86Operand::Reg(src)});
    }
    
    /**
     * ضرب
     */
    void emitMul(X86Register dest, X86Register src) {
        emit(X86OpType::IMUL, {X86Operand::Reg(dest), X86Operand::Reg(src)});
    }
    
    /**
     * مقارنة
     */
    void emitCmp(X86Register a, X86Register b) {
        emit(X86OpType::CMP, {X86Operand::Reg(a), X86Operand::Reg(b)});
    }
    
    /**
     * قفز مشروط
     */
    void emitJump(X86OpType jumpOp, const std::string& label) {
        emit(jumpOp, {X86Operand::Lbl(label)});
    }
    
    /**
     * استدعاء دالة
     */
    void emitCall(const std::string& func) {
        emit(X86OpType::CALL, {X86Operand::Lbl(func)});
    }
    
    /**
     * إصدار تسمية
     */
    void emitLabel(const std::string& name) {
        labels_.push_back(name);
        instructions_.push_back({{}, {}, "label:" + name});
    }
    
    /**
     * إصدار تعليمة
     */
    void emit(X86OpType op, std::vector<X86Operand> operands, 
              const std::string& comment = "") {
        X86Instruction inst;
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
        
        ss << "; كود مُولد للغة ص - x86_64\n";
        ss << "; اتفاقية الاستدعاء: " << (callConv_.shadowSpace() > 0 ? "Win64" : "System V") << "\n\n";
        
        ss << "section .text\n\n";
        
        for (const auto& inst : instructions_) {
            ss << inst.toAsm() << "\n";
        }
        
        return ss.str();
    }
    
private:
    X86CallingConvention callConv_;
    std::vector<X86Instruction> instructions_;
    std::vector<std::string> labels_;
};

// =============================================================================
// تحويل لنص
// =============================================================================

std::string registerToString(X86Register reg) {
    switch (reg) {
        case X86Register::RAX: return "rax";
        case X86Register::RBX: return "rbx";
        case X86Register::RCX: return "rcx";
        case X86Register::RDX: return "rdx";
        case X86Register::RSI: return "rsi";
        case X86Register::RDI: return "rdi";
        case X86Register::RBP: return "rbp";
        case X86Register::RSP: return "rsp";
        case X86Register::R8:  return "r8";
        case X86Register::R9:  return "r9";
        case X86Register::R10: return "r10";
        case X86Register::R11: return "r11";
        case X86Register::R12: return "r12";
        case X86Register::R13: return "r13";
        case X86Register::R14: return "r14";
        case X86Register::R15: return "r15";
        
        case X86Register::EAX: return "eax";
        case X86Register::EBX: return "ebx";
        case X86Register::ECX: return "ecx";
        case X86Register::EDX: return "edx";
        
        case X86Register::XMM0: return "xmm0";
        case X86Register::XMM1: return "xmm1";
        // ... المزيد
        
        default: return "?";
    }
}

std::string opToString(X86OpType op) {
    switch (op) {
        case X86OpType::MOV: return "mov";
        case X86OpType::LEA: return "lea";
        case X86OpType::PUSH: return "push";
        case X86OpType::POP: return "pop";
        case X86OpType::ADD: return "add";
        case X86OpType::SUB: return "sub";
        case X86OpType::IMUL: return "imul";
        case X86OpType::CMP: return "cmp";
        case X86OpType::JMP: return "jmp";
        case X86OpType::JE: return "je";
        case X86OpType::JNE: return "jne";
        case X86OpType::JG: return "jg";
        case X86OpType::JL: return "jl";
        case X86OpType::CALL: return "call";
        case X86OpType::RET: return "ret";
        case X86OpType::NOP: return "nop";
        default: return "?";
    }
}

std::string X86Operand::toString() const {
    switch (type) {
        case Register:
            return registerToString(reg);
        case Immediate:
            return std::to_string(immediate);
        case Memory:
            if (displacement != 0) {
                return "[" + registerToString(base) + " + " + std::to_string(displacement) + "]";
            }
            return "[" + registerToString(base) + "]";
        case Label:
            return label;
        default:
            return "?";
    }
}

std::string X86Instruction::toAsm() const {
    std::ostringstream ss;
    
    // تسمية؟
    if (comment.find("label:") == 0) {
        return comment.substr(6) + ":";
    }
    
    // التعليمة
    ss << "    " << opToString(op);
    
    for (size_t i = 0; i < operands.size(); i++) {
        if (i == 0) ss << " ";
        else ss << ", ";
        ss << operands[i].toString();
    }
    
    // التعليق
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

struct SadX86CodeGen {
    sad::targets::X86_64CodeGen* gen;
};

SadX86CodeGen* sad_x86_codegen_new(int windows) {
    auto cc = windows ? sad::targets::CallingConvention::Win64 
                      : sad::targets::CallingConvention::SystemV;
    auto* ctx = new (std::nothrow) SadX86CodeGen();
    if (!ctx) return nullptr;
    ctx->gen = new (std::nothrow) sad::targets::X86_64CodeGen(cc);
    if (!ctx->gen) { delete ctx; return nullptr; }
    return ctx;
}

void sad_x86_emit_function(SadX86CodeGen* ctx, const char* name) {
    ctx->gen->emitFunctionPrologue(name);
}

const char* sad_x86_get_asm(SadX86CodeGen* ctx) {
    static std::string result;
    result = ctx->gen->getAssembly();
    return result.c_str();
}

void sad_x86_codegen_free(SadX86CodeGen* ctx) {
    if (ctx) {
        delete ctx->gen;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef X86_64_TEST

#include <cassert>

void testCodeGeneration() {
    std::cout << "🔧 اختبار توليد الكود...\n";
    
    sad::targets::X86_64CodeGen gen;
    
    gen.emitFunctionPrologue("main", 16);
    gen.emit(sad::targets::X86OpType::MOV, {
        sad::targets::X86Operand::Reg(sad::targets::X86Register::RAX),
        sad::targets::X86Operand::Imm(42)
    }, "تحميل القيمة 42");
    gen.emitFunctionEpilogue();
    
    std::string asm_code = gen.getAssembly();
    
    assert(asm_code.find("main:") != std::string::npos);
    assert(asm_code.find("push") != std::string::npos);
    assert(asm_code.find("ret") != std::string::npos);
    
    std::cout << "   ✅ نجح\n";
}

void testCallingConvention() {
    std::cout << "📞 اختبار اتفاقية الاستدعاء...\n";
    
    sad::targets::X86CallingConvention sysv(sad::targets::CallingConvention::SystemV);
    assert(sysv.argRegisters()[0] == sad::targets::X86Register::RDI);
    assert(sysv.shadowSpace() == 0);
    
    sad::targets::X86CallingConvention win64(sad::targets::CallingConvention::Win64);
    assert(win64.argRegisters()[0] == sad::targets::X86Register::RCX);
    assert(win64.shadowSpace() == 32);
    
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات Backend x86_64\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testCodeGeneration();
    testCallingConvention();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // X86_64_TEST
