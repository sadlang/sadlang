/**
 * =============================================================================
 * ملف: sir_to_llvm.cpp
 * الوصف: تحويل SIR إلى LLVM IR
 * المهمة: T265 - SIR to LLVM IR lowering
 * المرحلة: Phase 27 - User Story 24 (SIR Layer)
 * =============================================================================
 * 
 * 🔽 دليل المبتدئ لتحويل SIR إلى LLVM
 * ═════════════════════════════════════════
 * 
 * ما هو LLVM IR؟
 * ──────────────
 * LLVM IR هو تمثيل وسيط منخفض المستوى.
 * يُستخدم لإنشاء كود الآلة لأي منصة.
 * 
 * مثال التحويل:
 * ─────────────
 * ```
 * SIR:
 *     %0 = تخصيص(عدد32)
 *     تخزين(%0, 10)
 *     %1 = تحميل(%0)
 *     أرجع(%1)
 * 
 * LLVM IR:
 *     %0 = alloca i32
 *     store i32 10, i32* %0
 *     %1 = load i32, i32* %0
 *     ret i32 %1
 * ```
 * 
 * خطوات التحويل:
 * ─────────────
 * 1. تحويل الأنواع SIR → LLVM
 * 2. تحويل الدوال
 * 3. تحويل الكتل
 * 4. تحويل التعليمات
 * 5. إدراج كود التنظيف
 * 
 * =============================================================================
 */

#include "sir_types.h"
#include "sir_opcodes.h"
#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace sad::sir {

// =============================================================================
// مُولد LLVM IR (نصي)
// =============================================================================

/**
 * مُولد LLVM IR
 * 
 * نُنتج LLVM IR كنص يمكن تمريره لـ llc
 */
class LlvmIrGenerator {
public:
    /**
     * تحويل وحدة SIR إلى LLVM IR
     */
    std::string generate(const SirModule& module) {
        output_.str("");
        output_.clear();
        
        // تعريفات عامة
        emitHeader();
        
        // تعريف الأنواع المخصصة
        emitTypeDefinitions(module);
        
        // الدوال الخارجية
        emitExternalDeclarations();
        
        // الدوال
        for (const auto& func : module.functions) {
            emitFunction(func);
        }
        
        return output_.str();
    }
    
private:
    // إصدار الترويسة
    void emitHeader() {
        output_ << "; LLVM IR generated from Sad SIR\n";
        output_ << "; Target: x86_64-pc-windows-msvc\n\n";
        
        output_ << "target datalayout = \"e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"\n";
        output_ << "target triple = \"x86_64-pc-windows-msvc\"\n\n";
    }
    
    // إصدار تعريفات الأنواع
    void emitTypeDefinitions(const SirModule& module) {
        output_ << "; Type definitions\n";
        
        // نوع النص
        output_ << "%String = type { i8*, i64, i64 }\n";
        
        // نوع المصفوفة
        output_ << "%Array = type { i8*, i64, i64, i64 }\n";
        
        output_ << "\n";
    }
    
    // إصدار التعريفات الخارجية
    void emitExternalDeclarations() {
        output_ << "; External declarations\n";
        output_ << "declare void @sad_print(i8*)\n";
        output_ << "declare i8* @sad_alloc(i64)\n";
        output_ << "declare void @sad_free(i8*)\n";
        output_ << "declare void @sad_drop_string(%String*)\n";
        output_ << "declare %String* @sad_clone_string(%String*)\n";
        output_ << "\n";
    }
    
    // إصدار دالة
    void emitFunction(const SirFunction& func) {
        output_ << "; Function: " << func.name << "\n";
        output_ << "define " << typeToLlvm(func.returnType) << " @" 
                << func.name << "(";
        
        // المعاملات
        for (size_t i = 0; i < func.params.size(); i++) {
            if (i > 0) output_ << ", ";
            output_ << typeToLlvm(func.params[i].type) << " %" 
                    << func.params[i].name;
        }
        
        output_ << ") {\n";
        
        // الكتل
        for (const auto& block : func.blocks) {
            emitBlock(block);
        }
        
        output_ << "}\n\n";
    }
    
    // إصدار كتلة
    void emitBlock(const SirBasicBlock& block) {
        output_ << block.name << ":\n";
        
        for (const auto& inst : block.instructions) {
            output_ << "  ";
            emitInstruction(inst);
            output_ << "\n";
        }
    }
    
    // إصدار تعليمة
    void emitInstruction(const SirInstruction& inst) {
        switch (inst.opcode) {
            // عمليات الملكية
            case Opcode::Alloc:
                emitAlloc(inst);
                break;
            case Opcode::Borrow:
            case Opcode::BorrowMut:
                emitBorrow(inst);
                break;
            case Opcode::Move:
            case Opcode::Copy:
                emitMoveCopy(inst);
                break;
            case Opcode::Drop:
                emitDrop(inst);
                break;
            case Opcode::Clone:
                emitClone(inst);
                break;
            case Opcode::EndBorrow:
                emitEndBorrow(inst);
                break;
            case Opcode::Deref:
                emitDeref(inst);
                break;
                
            // عمليات الذاكرة
            case Opcode::Load:
                emitLoad(inst);
                break;
            case Opcode::Store:
                emitStore(inst);
                break;
                
            // عمليات حسابية
            case Opcode::Add:
                emitBinaryOp(inst, "add");
                break;
            case Opcode::Sub:
                emitBinaryOp(inst, "sub");
                break;
            case Opcode::Mul:
                emitBinaryOp(inst, "mul");
                break;
            case Opcode::Div:
                emitBinaryOp(inst, "sdiv");
                break;
            case Opcode::Mod:
                emitBinaryOp(inst, "srem");
                break;
                
            // عمليات المقارنة
            case Opcode::Eq:
                emitComparison(inst, "eq");
                break;
            case Opcode::Ne:
                emitComparison(inst, "ne");
                break;
            case Opcode::Lt:
                emitComparison(inst, "slt");
                break;
            case Opcode::Le:
                emitComparison(inst, "sle");
                break;
            case Opcode::Gt:
                emitComparison(inst, "sgt");
                break;
            case Opcode::Ge:
                emitComparison(inst, "sge");
                break;
                
            // عمليات التحكم
            case Opcode::Jump:
                emitJump(inst);
                break;
            case Opcode::Branch:
                emitBranch(inst);
                break;
            case Opcode::Return:
                emitReturn(inst);
                break;
                
            // عمليات الدوال
            case Opcode::Call:
                emitCall(inst);
                break;
                
            default:
                output_ << "; unsupported: " << (int)inst.opcode;
                break;
        }
        
        // التعليق
        if (!inst.comment.empty()) {
            output_ << "  ; " << inst.comment;
        }
    }
    
    // إصدار تخصيص
    void emitAlloc(const SirInstruction& inst) {
        if (!inst.result) return;
        
        auto& typeOp = std::get<TypeOperand>(inst.operands[0]);
        std::string llvmType = typeToLlvm(typeOp.type);
        
        output_ << "%" << *inst.result << " = alloca " << llvmType;
    }
    
    // إصدار استعارة
    void emitBorrow(const SirInstruction& inst) {
        if (!inst.result || inst.operands.empty()) return;
        
        auto& valOp = std::get<ValueOperand>(inst.operands[0]);
        
        // الاستعارة في LLVM هي مجرد نسخ المؤشر
        output_ << "%" << *inst.result << " = bitcast i32* %" 
                << valOp.id << " to i32*";
    }
    
    // إصدار نقل/نسخ
    void emitMoveCopy(const SirInstruction& inst) {
        if (!inst.result || inst.operands.empty()) return;
        
        auto& valOp = std::get<ValueOperand>(inst.operands[0]);
        
        // في LLVM، النقل والنسخ متشابهان على مستوى IR
        output_ << "%" << *inst.result << " = load i32, i32* %"
                << valOp.id;
    }
    
    // إصدار إسقاط
    void emitDrop(const SirInstruction& inst) {
        if (inst.operands.empty()) return;
        
        // الإسقاط يعتمد على النوع
        // للأنواع البسيطة، لا شيء مطلوب
        // للأنواع المعقدة، نستدعي المُدمر
        output_ << "; drop %" << std::get<ValueOperand>(inst.operands[0]).id;
    }
    
    // إصدار استنساخ
    void emitClone(const SirInstruction& inst) {
        if (!inst.result || inst.operands.empty()) return;
        
        auto& valOp = std::get<ValueOperand>(inst.operands[0]);
        
        // للأنواع البسيطة، الاستنساخ مثل النسخ
        output_ << "%" << *inst.result << " = load i32, i32* %"
                << valOp.id;
    }
    
    // إصدار إنهاء استعارة
    void emitEndBorrow(const SirInstruction& inst) {
        // لا شيء مطلوب في LLVM
        output_ << "; end borrow";
    }
    
    // إصدار فك مرجع
    void emitDeref(const SirInstruction& inst) {
        if (!inst.result || inst.operands.empty()) return;
        
        auto& valOp = std::get<ValueOperand>(inst.operands[0]);
        
        output_ << "%" << *inst.result << " = load i32, i32* %"
                << valOp.id;
    }
    
    // إصدار تحميل
    void emitLoad(const SirInstruction& inst) {
        if (!inst.result || inst.operands.empty()) return;
        
        auto& valOp = std::get<ValueOperand>(inst.operands[0]);
        
        output_ << "%" << *inst.result << " = load i32, i32* %"
                << valOp.id;
    }
    
    // إصدار تخزين
    void emitStore(const SirInstruction& inst) {
        if (inst.operands.size() < 2) return;
        
        auto& ptrOp = std::get<ValueOperand>(inst.operands[0]);
        auto& valOp = std::get<ValueOperand>(inst.operands[1]);
        
        output_ << "store i32 %" << valOp.id << ", i32* %" << ptrOp.id;
    }
    
    // إصدار عملية ثنائية
    void emitBinaryOp(const SirInstruction& inst, const std::string& op) {
        if (!inst.result || inst.operands.size() < 2) return;
        
        auto& lhs = std::get<ValueOperand>(inst.operands[0]);
        auto& rhs = std::get<ValueOperand>(inst.operands[1]);
        
        output_ << "%" << *inst.result << " = " << op << " i32 %"
                << lhs.id << ", %" << rhs.id;
    }
    
    // إصدار مقارنة
    void emitComparison(const SirInstruction& inst, const std::string& pred) {
        if (!inst.result || inst.operands.size() < 2) return;
        
        auto& lhs = std::get<ValueOperand>(inst.operands[0]);
        auto& rhs = std::get<ValueOperand>(inst.operands[1]);
        
        output_ << "%" << *inst.result << " = icmp " << pred << " i32 %"
                << lhs.id << ", %" << rhs.id;
    }
    
    // إصدار قفز
    void emitJump(const SirInstruction& inst) {
        if (inst.operands.empty()) return;
        
        auto& block = std::get<BlockOperand>(inst.operands[0]);
        
        output_ << "br label %bb" << block.id;
    }
    
    // إصدار تفرع
    void emitBranch(const SirInstruction& inst) {
        if (inst.operands.size() < 3) return;
        
        auto& cond = std::get<ValueOperand>(inst.operands[0]);
        auto& trueBlock = std::get<BlockOperand>(inst.operands[1]);
        auto& falseBlock = std::get<BlockOperand>(inst.operands[2]);
        
        output_ << "br i1 %" << cond.id << ", label %bb" << trueBlock.id
                << ", label %bb" << falseBlock.id;
    }
    
    // إصدار إرجاع
    void emitReturn(const SirInstruction& inst) {
        if (inst.operands.empty()) {
            output_ << "ret void";
        } else {
            auto& val = std::get<ValueOperand>(inst.operands[0]);
            output_ << "ret i32 %" << val.id;
        }
    }
    
    // إصدار استدعاء
    void emitCall(const SirInstruction& inst) {
        if (inst.operands.empty()) return;
        
        auto& func = std::get<FunctionOperand>(inst.operands[0]);
        
        if (inst.result) {
            output_ << "%" << *inst.result << " = ";
        }
        
        output_ << "call i32 @" << func.name << "(";
        
        for (size_t i = 1; i < inst.operands.size(); i++) {
            if (i > 1) output_ << ", ";
            auto& arg = std::get<ValueOperand>(inst.operands[i]);
            output_ << "i32 %" << arg.id;
        }
        
        output_ << ")";
    }
    
    // تحويل نوع SIR إلى نوع LLVM
    std::string typeToLlvm(const std::shared_ptr<SirType>& type) {
        if (!type) return "i32";
        
        switch (type->kind) {
            case TypeKind::Void: return "void";
            case TypeKind::Bool: return "i1";
            case TypeKind::Int8: return "i8";
            case TypeKind::Int16: return "i16";
            case TypeKind::Int32: return "i32";
            case TypeKind::Int64: return "i64";
            case TypeKind::UInt8: return "i8";
            case TypeKind::UInt16: return "i16";
            case TypeKind::UInt32: return "i32";
            case TypeKind::UInt64: return "i64";
            case TypeKind::Float32: return "float";
            case TypeKind::Float64: return "double";
            case TypeKind::String: return "%String*";
            case TypeKind::Reference:
            case TypeKind::MutableRef:
                if (!type->params.empty()) {
                    return typeToLlvm(type->params[0]) + "*";
                }
                return "i8*";
            default:
                return "i32";
        }
    }
    
    std::ostringstream output_;
};

} // namespace sad::sir

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadSirToLlvm {
    sad::sir::LlvmIrGenerator* generator;
};

SadSirToLlvm* sad_sir_to_llvm_new() {
    auto* ctx = new SadSirToLlvm();
    ctx->generator = new sad::sir::LlvmIrGenerator();
    return ctx;
}

const char* sad_sir_to_llvm_generate(SadSirToLlvm* ctx, void* module) {
    static std::string result;
    
    if (!ctx || !module) {
        result = "";
        return result.c_str();
    }
    
    auto* sirModule = static_cast<sad::sir::SirModule*>(module);
    result = ctx->generator->generate(*sirModule);
    return result.c_str();
}

void sad_sir_to_llvm_free(SadSirToLlvm* ctx) {
    if (ctx) {
        delete ctx->generator;
        delete ctx;
    }
}

} // extern "C"
