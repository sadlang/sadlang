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
#include <iostream>

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
        valueTypes_.clear();
        
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
    // (AR) خريطة أنواع القيم — تتبع نوع كل ValueId أثناء التوليد
    // (EN) Value type map — tracks type of each ValueId during generation
    std::map<size_t, std::shared_ptr<SirType>> valueTypes_;
    // (AR) الحصول على نوع LLVM لقيمة معينة / (EN) Get LLVM type for a value
    std::string getValueType(size_t id) {
        auto it = valueTypes_.find(id);
        if (it != valueTypes_.end() && it->second) {
            return typeToLlvm(it->second);
        }
        return "i32"; // fallback
    }
    
    // (AR) تسجيل نوع النتيجة / (EN) Register result type
    void recordType(const SirInstruction& inst) {
        if (inst.result && inst.resultType) {
            valueTypes_[*inst.result] = inst.resultType;
        }
    }
    
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
        // (AR) تسجيل نوع النتيجة في الخريطة / (EN) Record result type
        recordType(inst);
        
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
                
            // ═══════════════════════════════════════════════════════════════
            // العمليات منخفضة المستوى (نظام التشغيل)
            // ═══════════════════════════════════════════════════════════════
            
            // --- الذاكرة المباشرة ---
            case Opcode::RawLoad8:
                emitRawLoad(inst, "i8");
                break;
            case Opcode::RawLoad16:
                emitRawLoad(inst, "i16");
                break;
            case Opcode::RawLoad32:
                emitRawLoad(inst, "i32");
                break;
            case Opcode::RawLoad64:
                emitRawLoad(inst, "i64");
                break;
            case Opcode::RawStore8:
                emitRawStore(inst, "i8");
                break;
            case Opcode::RawStore16:
                emitRawStore(inst, "i16");
                break;
            case Opcode::RawStore32:
                emitRawStore(inst, "i32");
                break;
            case Opcode::RawStore64:
                emitRawStore(inst, "i64");
                break;
                
            // --- منافذ I/O ---
            case Opcode::PortIn8:
                emitPortIn(inst, "i8");
                break;
            case Opcode::PortIn16:
                emitPortIn(inst, "i16");
                break;
            case Opcode::PortIn32:
                emitPortIn(inst, "i32");
                break;
            case Opcode::PortOut8:
                emitPortOut(inst, "i8");
                break;
            case Opcode::PortOut16:
                emitPortOut(inst, "i16");
                break;
            case Opcode::PortOut32:
                emitPortOut(inst, "i32");
                break;
                
            // --- التحكم بالمعالج ---
            case Opcode::Cli:
                emitInlineAsm("cli", "", "", false);
                break;
            case Opcode::Sti:
                emitInlineAsm("sti", "", "", false);
                break;
            case Opcode::Hlt:
                emitInlineAsm("hlt", "", "", false);
                break;
            case Opcode::Pause:
                emitInlineAsm("pause", "", "", false);
                break;
            case Opcode::Nop:
                emitInlineAsm("nop", "", "", false);
                break;
                
            // --- سجلات النظام ---
            case Opcode::ReadCr0:
                emitReadCr(inst, 0);
                break;
            case Opcode::ReadCr2:
                emitReadCr(inst, 2);
                break;
            case Opcode::ReadCr3:
                emitReadCr(inst, 3);
                break;
            case Opcode::ReadCr4:
                emitReadCr(inst, 4);
                break;
            case Opcode::WriteCr0:
                emitWriteCr(inst, 0);
                break;
            case Opcode::WriteCr3:
                emitWriteCr(inst, 3);
                break;
            case Opcode::WriteCr4:
                emitWriteCr(inst, 4);
                break;
                
            // --- العمليات الذرية ---
            case Opcode::AtomicLoad:
                emitAtomicLoad(inst);
                break;
            case Opcode::AtomicStore:
                emitAtomicStore(inst);
                break;
            case Opcode::AtomicAdd:
                emitAtomicRmw(inst, "add");
                break;
            case Opcode::AtomicSub:
                emitAtomicRmw(inst, "sub");
                break;
            case Opcode::AtomicExchange:
                emitAtomicRmw(inst, "xchg");
                break;
            case Opcode::AtomicCmpXchg:
                emitAtomicCmpXchg(inst);
                break;
                
            // --- حواجز الذاكرة ---
            case Opcode::MemFence:
                output_ << "fence seq_cst";
                break;
            case Opcode::LoadFence:
                output_ << "fence acquire";
                break;
            case Opcode::StoreFence:
                output_ << "fence release";
                break;
                
            // --- المقاطعات ---
            case Opcode::Int:
                emitInt(inst);
                break;
                
            // --- الذاكرة الافتراضية ---
            case Opcode::Invlpg:
                emitInvlpg(inst);
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
        std::string valType = getValueType(valOp.id);
        
        // الاستعارة في LLVM هي مجرد نسخ المؤشر
        output_ << "%" << *inst.result << " = bitcast " << valType << "* %" 
                << valOp.id << " to " << valType << "*";
    }
    
    // إصدار نقل/نسخ
    void emitMoveCopy(const SirInstruction& inst) {
        if (!inst.result || inst.operands.empty()) return;
        
        auto& valOp = std::get<ValueOperand>(inst.operands[0]);
        std::string valType = getValueType(valOp.id);
        
        // في LLVM، النقل والنسخ متشابهان على مستوى IR
        output_ << "%" << *inst.result << " = load " << valType << ", " << valType << "* %"
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
        std::string valType = getValueType(valOp.id);
        
        // للأنواع البسيطة، الاستنساخ مثل النسخ
        output_ << "%" << *inst.result << " = load " << valType << ", " << valType << "* %"
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
        std::string valType = inst.resultType ? typeToLlvm(inst.resultType) : getValueType(valOp.id);
        
        output_ << "%" << *inst.result << " = load " << valType << ", " << valType << "* %"
                << valOp.id;
    }
    
    // إصدار تحميل
    void emitLoad(const SirInstruction& inst) {
        if (!inst.result || inst.operands.empty()) return;
        
        auto& valOp = std::get<ValueOperand>(inst.operands[0]);
        std::string valType = inst.resultType ? typeToLlvm(inst.resultType) : getValueType(valOp.id);
        
        output_ << "%" << *inst.result << " = load " << valType << ", " << valType << "* %"
                << valOp.id;
    }
    
    // إصدار تخزين
    void emitStore(const SirInstruction& inst) {
        if (inst.operands.size() < 2) return;
        
        auto& ptrOp = std::get<ValueOperand>(inst.operands[0]);
        auto& valOp = std::get<ValueOperand>(inst.operands[1]);
        std::string valType = getValueType(valOp.id);
        
        output_ << "store " << valType << " %" << valOp.id << ", " << valType << "* %" << ptrOp.id;
    }
    
    // إصدار عملية ثنائية
    void emitBinaryOp(const SirInstruction& inst, const std::string& op) {
        if (!inst.result || inst.operands.size() < 2) return;
        
        auto& lhs = std::get<ValueOperand>(inst.operands[0]);
        auto& rhs = std::get<ValueOperand>(inst.operands[1]);
        std::string valType = inst.resultType ? typeToLlvm(inst.resultType) : getValueType(lhs.id);
        
        // (AR) استخدام fadd/fsub/fmul/fdiv للأنواع العشرية
        // (EN) Use fadd/fsub/fmul/fdiv for floating point types
        std::string llvmOp = op;
        if (valType == "float" || valType == "double") {
            if (op == "add") llvmOp = "fadd";
            else if (op == "sub") llvmOp = "fsub";
            else if (op == "mul") llvmOp = "fmul";
            else if (op == "sdiv") llvmOp = "fdiv";
            else if (op == "srem") llvmOp = "frem";
        }
        
        output_ << "%" << *inst.result << " = " << llvmOp << " " << valType << " %"
                << lhs.id << ", %" << rhs.id;
    }
    
    // إصدار مقارنة
    void emitComparison(const SirInstruction& inst, const std::string& pred) {
        if (!inst.result || inst.operands.size() < 2) return;
        
        auto& lhs = std::get<ValueOperand>(inst.operands[0]);
        auto& rhs = std::get<ValueOperand>(inst.operands[1]);
        std::string valType = getValueType(lhs.id);
        
        // (AR) استخدام fcmp للأنواع العشرية مع محددات o (ordered)
        // (EN) Use fcmp for floating types with ordered predicates
        if (valType == "float" || valType == "double") {
            std::string fpred = "o" + pred; // oeq, one, olt, ole, ogt, oge
            output_ << "%" << *inst.result << " = fcmp " << fpred << " " << valType << " %"
                    << lhs.id << ", %" << rhs.id;
        } else {
            output_ << "%" << *inst.result << " = icmp " << pred << " " << valType << " %"
                    << lhs.id << ", %" << rhs.id;
        }
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
            std::string valType = inst.resultType ? typeToLlvm(inst.resultType) : getValueType(val.id);
            output_ << "ret " << valType << " %" << val.id;
        }
    }
    
    // إصدار استدعاء
    void emitCall(const SirInstruction& inst) {
        if (inst.operands.empty()) return;
        
        auto& func = std::get<FunctionOperand>(inst.operands[0]);
        std::string retType = inst.resultType ? typeToLlvm(inst.resultType) : "i32";
        
        if (inst.result) {
            output_ << "%" << *inst.result << " = ";
        }
        
        output_ << "call " << retType << " @" << func.name << "(";
        
        for (size_t i = 1; i < inst.operands.size(); i++) {
            if (i > 1) output_ << ", ";
            auto& arg = std::get<ValueOperand>(inst.operands[i]);
            std::string argType = getValueType(arg.id);
            output_ << argType << " %" << arg.id;
        }
        
        output_ << ")";
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // العمليات منخفضة المستوى (Low-level operations for OS development)
    // ═══════════════════════════════════════════════════════════════════════════
    
    // --- قراءة ذاكرة مباشرة ---
    void emitRawLoad(const SirInstruction& inst, const std::string& llvmType) {
        if (!inst.result || inst.operands.empty()) return;
        
        auto& addr = std::get<ValueOperand>(inst.operands[0]);
        // تحويل العنوان إلى مؤشر ثم القراءة منه بدون التحقق
        output_ << "%" << *inst.result << ".ptr = inttoptr i64 %" << addr.id 
                << " to " << llvmType << "*\n  ";
        output_ << "%" << *inst.result << " = load volatile " << llvmType 
                << ", " << llvmType << "* %" << *inst.result << ".ptr";
    }
    
    // --- كتابة ذاكرة مباشرة ---
    void emitRawStore(const SirInstruction& inst, const std::string& llvmType) {
        if (inst.operands.size() < 2) return;
        
        auto& addr = std::get<ValueOperand>(inst.operands[0]);
        auto& val = std::get<ValueOperand>(inst.operands[1]);
        // تحويل العنوان إلى مؤشر ثم الكتابة إليه
        output_ << "%tmp.ptr." << addr.id << " = inttoptr i64 %" << addr.id 
                << " to " << llvmType << "*\n  ";
        output_ << "store volatile " << llvmType << " %" << val.id 
                << ", " << llvmType << "* %tmp.ptr." << addr.id;
    }
    
    // --- قراءة من منفذ I/O ---
    void emitPortIn(const SirInstruction& inst, const std::string& llvmType) {
        if (!inst.result || inst.operands.empty()) return;
        
        auto& port = std::get<ValueOperand>(inst.operands[0]);
        std::string instr;
        if (llvmType == "i8") instr = "inb";
        else if (llvmType == "i16") instr = "inw";
        else instr = "inl";
        
        // inline assembly للقراءة من المنفذ
        output_ << "%" << *inst.result << " = call " << llvmType 
                << " asm sideeffect \"" << instr << " %dx, %"
                << (llvmType == "i8" ? "al" : (llvmType == "i16" ? "ax" : "eax"))
                << "\", \"={" << (llvmType == "i8" ? "al" : (llvmType == "i16" ? "ax" : "eax"))
                << "},{dx}\"(i16 %" << port.id << ")";
    }
    
    // --- كتابة إلى منفذ I/O ---
    void emitPortOut(const SirInstruction& inst, const std::string& llvmType) {
        if (inst.operands.size() < 2) return;
        
        auto& port = std::get<ValueOperand>(inst.operands[0]);
        auto& val = std::get<ValueOperand>(inst.operands[1]);
        std::string instr;
        if (llvmType == "i8") instr = "outb";
        else if (llvmType == "i16") instr = "outw";
        else instr = "outl";
        
        output_ << "call void asm sideeffect \"" << instr << " %"
                << (llvmType == "i8" ? "al" : (llvmType == "i16" ? "ax" : "eax"))
                << ", %dx\", \"{" << (llvmType == "i8" ? "al" : (llvmType == "i16" ? "ax" : "eax"))
                << "},{dx}\"(" << llvmType << " %" << val.id << ", i16 %" << port.id << ")";
    }
    
    // --- تعليمة inline assembly عامة ---
    void emitInlineAsm(const std::string& instr, const std::string& constraints,
                       const std::string& args, bool hasSideEffects = true) {
        output_ << "call void asm ";
        if (hasSideEffects) output_ << "sideeffect ";
        output_ << "\"" << instr << "\", \"" << constraints << "\"(" << args << ")";
    }
    
    // --- قراءة سجل تحكم ---
    void emitReadCr(const SirInstruction& inst, int crNum) {
        if (!inst.result) return;
        
        output_ << "%" << *inst.result << " = call i64 asm sideeffect \"mov %cr" 
                << crNum << ", $0\", \"=r\"()";
    }
    
    // --- كتابة سجل تحكم ---
    void emitWriteCr(const SirInstruction& inst, int crNum) {
        if (inst.operands.empty()) return;
        
        auto& val = std::get<ValueOperand>(inst.operands[0]);
        output_ << "call void asm sideeffect \"mov $0, %cr" << crNum 
                << "\", \"r\"(i64 %" << val.id << ")";
    }
    
    // --- تحميل ذري ---
    void emitAtomicLoad(const SirInstruction& inst) {
        if (!inst.result || inst.operands.empty()) return;
        
        auto& ptr = std::get<ValueOperand>(inst.operands[0]);
        std::string valType = inst.resultType ? typeToLlvm(inst.resultType) : "i64";
        
        output_ << "%" << *inst.result << " = load atomic " << valType 
                << ", " << valType << "* %" << ptr.id << " seq_cst";
    }
    
    // --- تخزين ذري ---
    void emitAtomicStore(const SirInstruction& inst) {
        if (inst.operands.size() < 2) return;
        
        auto& ptr = std::get<ValueOperand>(inst.operands[0]);
        auto& val = std::get<ValueOperand>(inst.operands[1]);
        std::string valType = getValueType(val.id);
        
        output_ << "store atomic " << valType << " %" << val.id 
                << ", " << valType << "* %" << ptr.id << " seq_cst";
    }
    
    // --- عملية ذرية قراءة-تعديل-كتابة ---
    void emitAtomicRmw(const SirInstruction& inst, const std::string& op) {
        if (!inst.result || inst.operands.size() < 2) return;
        
        auto& ptr = std::get<ValueOperand>(inst.operands[0]);
        auto& val = std::get<ValueOperand>(inst.operands[1]);
        std::string valType = inst.resultType ? typeToLlvm(inst.resultType) : "i64";
        
        output_ << "%" << *inst.result << " = atomicrmw " << op << " " 
                << valType << "* %" << ptr.id << ", " << valType 
                << " %" << val.id << " seq_cst";
    }
    
    // --- مقارنة وتبديل ذري ---
    void emitAtomicCmpXchg(const SirInstruction& inst) {
        if (!inst.result || inst.operands.size() < 3) return;
        
        auto& ptr = std::get<ValueOperand>(inst.operands[0]);
        auto& expected = std::get<ValueOperand>(inst.operands[1]);
        auto& desired = std::get<ValueOperand>(inst.operands[2]);
        std::string valType = getValueType(expected.id);
        
        output_ << "%" << *inst.result << ".pair = cmpxchg " << valType 
                << "* %" << ptr.id << ", " << valType << " %" << expected.id 
                << ", " << valType << " %" << desired.id << " seq_cst seq_cst\n  ";
        output_ << "%" << *inst.result << " = extractvalue {" << valType 
                << ", i1} %" << *inst.result << ".pair, 0";
    }
    
    // --- استدعاء مقاطعة ---
    void emitInt(const SirInstruction& inst) {
        if (inst.operands.empty()) return;
        
        auto& vec = std::get<ConstantOperand>(inst.operands[0]);
        int64_t vecNum = std::get<int64_t>(vec.value);
        output_ << "call void asm sideeffect \"int $$" << vecNum << "\", \"\"()";
    }
    
    // --- إبطال صفحة TLB ---
    void emitInvlpg(const SirInstruction& inst) {
        if (inst.operands.empty()) return;
        
        auto& addr = std::get<ValueOperand>(inst.operands[0]);
        output_ << "call void asm sideeffect \"invlpg ($0)\", \"r\"(i64 %" << addr.id << ")";
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    
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
                // (AR) نوع SIR غير معروف — تحذير + fallback إلى i32
                // (EN) Unknown SIR type kind — warn + fallback to i32
                std::cerr << "[sadc تحذير] نوع SIR غير معروف في typeToLlvm: "
                          << static_cast<int>(type->kind) << " — استخدام i32" << std::endl;
                return "i32";
        }
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
    thread_local std::string result;
    
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
