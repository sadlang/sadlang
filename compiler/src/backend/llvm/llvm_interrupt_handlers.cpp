/**
 * =============================================================================
 * ملف: llvm_interrupt_handlers.cpp
 * الوصف: تنفيذ توليد معالجات المقاطعات عبر LLVM IRBuilder
 * المهمة: م-د02 - تحويل InterruptCodeGenerator
 * =============================================================================
 *
 * هيكل غلاف ISR المُولَّد (x86_64):
 *
 *   isr_wrapper_14:
 *     ; CPU تدفع: SS, RSP, RFLAGS, CS, RIP [, error_code]
 *     ; إذا لا يوجد error_code → ندفع 0 بديلاً
 *     push 0              ; fake error code (إذا لزم)
 *     push rax            ; حفظ السجلات العامة
 *     push rbx
 *     ...
 *     push r15
 *     
 *     mov rdi, rsp        ; مؤشر الإطار كمعامل أول
 *     call page_fault_handler  ; استدعاء المعالج
 *     
 *     pop r15             ; استعادة السجلات
 *     ...
 *     pop rax
 *     add rsp, 8          ; إزالة error code
 *     iretq               ; العودة من المقاطعة
 * =============================================================================
 */

#include "backend/llvm/llvm_interrupt_handlers.h"

#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>

namespace sad {
namespace codegen {

// المقاطعات التي تدفع كود خطأ تلقائياً (x86_64)
static const uint8_t ERROR_CODE_VECTORS[] = {
    8,   // Double Fault
    10,  // Invalid TSS
    11,  // Segment Not Present
    12,  // Stack-Segment Fault
    13,  // General Protection Fault
    14,  // Page Fault
    17,  // Alignment Check
    21,  // Control Protection Exception
    29,  // VMM Communication Exception
    30,  // Security Exception
};

LLVMInterruptHandlers::LLVMInterruptHandlers(llvm::Module& module)
    : m_module(module)
    , m_ctx(module.getContext()) {}

// ═══════════════════════════════════════════════════════════════════════════════
//                           تسجيل وتوليد
// ═══════════════════════════════════════════════════════════════════════════════

void LLVMInterruptHandlers::registerHandler(uint8_t vector,
                                            const std::string& handlerFunc) {
    IDTEntry entry;
    entry.vector = vector;
    entry.handlerName = handlerFunc;
    entry.hasErrorCode = vectorHasErrorCode(vector);
    entry.isTrampoline = true;
    
    m_entries.push_back(entry);
}

void LLVMInterruptHandlers::generateAll() {
    // توليد كل الأغلفة
    for (const auto& entry : m_entries) {
        auto* wrapper = generateISRWrapper(entry);
        m_wrappers[entry.vector] = wrapper;
    }
    
    // توليد جدول IDT
    generateIDTTable();
}

// ═══════════════════════════════════════════════════════════════════════════════
//                           بنية إطار المقاطعة
// ═══════════════════════════════════════════════════════════════════════════════

llvm::StructType* LLVMInterruptHandlers::getInterruptFrameType() {
    if (m_frameType) return m_frameType;
    
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    
    // إطار المقاطعة: 15 سجل عام + error_code + RIP + CS + RFLAGS + RSP + SS
    std::vector<llvm::Type*> fields;
    
    // السجلات العامة (r15 → rax) — بترتيب الاستعادة
    for (int i = 0; i < InterruptFrameLayout::NUM_GP_REGS; i++) {
        fields.push_back(i64Ty); // r15, r14, ..., rbx, rax
    }
    
    // كود الخطأ (أو 0 إذا لا يوجد)
    fields.push_back(i64Ty); // error_code
    
    // ما تدفعه CPU
    fields.push_back(i64Ty); // RIP
    fields.push_back(i64Ty); // CS
    fields.push_back(i64Ty); // RFLAGS
    fields.push_back(i64Ty); // RSP
    fields.push_back(i64Ty); // SS
    
    m_frameType = llvm::StructType::create(m_ctx, fields, "InterruptFrame");
    return m_frameType;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                           توليد غلاف ISR
// ═══════════════════════════════════════════════════════════════════════════════

llvm::Function* LLVMInterruptHandlers::generateISRWrapper(const IDTEntry& entry) {
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    auto* funcTy = llvm::FunctionType::get(voidTy, false);
    
    std::string wrapperName = "isr_wrapper_" + std::to_string(entry.vector);
    auto* wrapper = llvm::Function::Create(
        funcTy, llvm::Function::ExternalLinkage,
        wrapperName, m_module);
    
    // ISR wrapper يجب أن يكون naked
    wrapper->addFnAttr(llvm::Attribute::Naked);
    wrapper->addFnAttr(llvm::Attribute::NoUnwind);
    wrapper->setSection(".text.isr");
    
    auto* bb = llvm::BasicBlock::Create(m_ctx, "entry", wrapper);
    llvm::IRBuilder<> builder(bb);
    
    // ─── بناء تعليمات التجميع الكاملة ────────────────────────────────
    // نستخدم كتلة asm واحدة كبيرة لأن ISR wrapper دقيق جداً
    
    std::string asmStr;
    
    // 1. دفع error code وهمي (إذا لا تدفعه CPU)
    if (!entry.hasErrorCode) {
        asmStr += "push $$0\n";
    }
    
    // 2. حفظ جميع السجلات العامة
    asmStr += "push %rax\n"
              "push %rbx\n"
              "push %rcx\n"
              "push %rdx\n"
              "push %rsi\n"
              "push %rdi\n"
              "push %rbp\n"
              "push %r8\n"
              "push %r9\n"
              "push %r10\n"
              "push %r11\n"
              "push %r12\n"
              "push %r13\n"
              "push %r14\n"
              "push %r15\n";
    
    // 3. تمرير مؤشر الإطار كمعامل (rdi = مؤشر المكدس)
    asmStr += "mov %rsp, %rdi\n";
    
    // 4. استدعاء المعالج الحقيقي
    asmStr += "call " + entry.handlerName + "\n";
    
    // 5. استعادة السجلات
    asmStr += "pop %r15\n"
              "pop %r14\n"
              "pop %r13\n"
              "pop %r12\n"
              "pop %r11\n"
              "pop %r10\n"
              "pop %r9\n"
              "pop %r8\n"
              "pop %rbp\n"
              "pop %rdi\n"
              "pop %rsi\n"
              "pop %rdx\n"
              "pop %rcx\n"
              "pop %rbx\n"
              "pop %rax\n";
    
    // 6. إزالة error code + iretq
    asmStr += "add $$8, %rsp\n"
              "iretq\n";
    
    auto* asmTy = llvm::FunctionType::get(voidTy, false);
    auto* inlineAsm = llvm::InlineAsm::get(
        asmTy, asmStr, "", true, false,
        llvm::InlineAsm::AD_ATT);
    
    builder.CreateCall(inlineAsm);
    builder.CreateUnreachable();
    
    return wrapper;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                           جدول IDT
// ═══════════════════════════════════════════════════════════════════════════════

llvm::GlobalVariable* LLVMInterruptHandlers::generateIDTTable() {
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    
    // IDT entry struct (x86_64): 16 bytes
    // offset_low(16) | selector(16) | ist(3) | reserved(5) | type(4) | zero(1) | dpl(2) | present(1)
    // | offset_mid(16) | offset_high(32) | reserved(32)
    auto* i16Ty = llvm::Type::getInt16Ty(m_ctx);
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    auto* i8Ty = llvm::Type::getInt8Ty(m_ctx);
    
    // بنية مُدخل IDT كـ [2 x i64] (16 بايت)
    auto* entryTy = llvm::ArrayType::get(i64Ty, 2);
    
    // جدول 256 مُدخل
    auto* tableTy = llvm::ArrayType::get(entryTy, 256);
    
    // قيم صفرية كافتراضي (المقاطعات غير المُسجّلة تبقى فارغة)
    auto* zeroEntry = llvm::ConstantAggregateZero::get(entryTy);
    
    std::vector<llvm::Constant*> entries(256, zeroEntry);
    
    // ملء المُدخلات المُسجّلة
    // ملاحظة: القيم الفعلية للعناوين تُحسب وقت الربط
    // هنا نُخزّن مؤشر الدالة كقيمة
    for (const auto& [vector, wrapper] : m_wrappers) {
        auto* funcAddr = llvm::ConstantExpr::getPtrToInt(wrapper, i64Ty);
        
        // تجزئة العنوان في هيكل IDT entry
        // low_word (bits 15:0), selector (0x08 = code segment), type_attr (0x8E = interrupt gate, present)
        // mid_word (bits 31:16), high_dword (bits 63:32)
        
        // نُخزّن كـ [funcAddr, metadata] — يُحلّ وقت الربط
        auto* metadata = llvm::ConstantInt::get(i64Ty, 0x008E0008ULL); // type + selector
        
        auto* entryVal = llvm::ConstantArray::get(
            entryTy, {funcAddr, metadata});
        entries[vector] = entryVal;
    }
    
    auto* tableInit = llvm::ConstantArray::get(tableTy, entries);
    
    auto* idtTable = new llvm::GlobalVariable(
        m_module, tableTy, true,
        llvm::GlobalVariable::ExternalLinkage,
        tableInit, "sad_idt_table");
    
    idtTable->setAlignment(llvm::Align(16));
    idtTable->setSection(".data.idt");
    
    return idtTable;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                           مساعدات
// ═══════════════════════════════════════════════════════════════════════════════

bool LLVMInterruptHandlers::vectorHasErrorCode(uint8_t vector) {
    for (auto v : ERROR_CODE_VECTORS) {
        if (v == vector) return true;
    }
    return false;
}

} // namespace codegen
} // namespace sad
