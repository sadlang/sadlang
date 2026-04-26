/**
 * =============================================================================
 * ملف: llvm_inline_asm.cpp
 * الوصف: تنفيذ توليد تعليمات التجميع المُضمّنة عبر LLVM IRBuilder
 * المهمة: م-د03 - تحويل InlineAsmCodeGenerator
 * =============================================================================
 */

#include "backend/llvm/llvm_inline_asm.h"

#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>

namespace sad {
namespace codegen {

// ═══════════════════════════════════════════════════════════════════════════════
//                    خريطة التعليمات العربية
// ═══════════════════════════════════════════════════════════════════════════════

const std::unordered_map<std::string, ArabicAsmMapping>
LLVMInlineAsm::s_arabicMnemonics = {
    // التعليمة العربية → {x86_64, aarch64, sideEffects, volatile}
    {"\xd8\xaa\xd9\x88\xd9\x82\xd9\x81",                         // "توقف"
     {"hlt", "wfi", true, true}},
    {"\xd8\xb9\xd8\xb7\xd9\x91\xd9\x84_\xd8\xa7\xd9\x84\xd9\x85\xd9\x82\xd8\xa7\xd8\xb7\xd8\xb9\xd8\xa7\xd8\xaa", // "عطّل_المقاطعات"
     {"cli", "msr daifset, #0xf", true, true}},
    {"\xd9\x81\xd8\xb9\xd9\x91\xd9\x84_\xd8\xa7\xd9\x84\xd9\x85\xd9\x82\xd8\xa7\xd8\xb7\xd8\xb9\xd8\xa7\xd8\xaa", // "فعّل_المقاطعات"
     {"sti", "msr daifclr, #0xf", true, true}},
    {"\xd9\x84\xd8\xa7_\xd8\xb9\xd9\x85\xd9\x84\xd9\x8a\xd8\xa9", // "لا_عملية"
     {"nop", "nop", false, false}},
    {"\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xa7\xd8\xb1",       // "انتظار"
     {"pause", "yield", true, false}},
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    المُنشئ
// ═══════════════════════════════════════════════════════════════════════════════

LLVMInlineAsm::LLVMInlineAsm(llvm::Module& module)
    : m_module(module)
    , m_ctx(module.getContext()) {}

// ═══════════════════════════════════════════════════════════════════════════════
//                    توليد تعليمات التجميع
// ═══════════════════════════════════════════════════════════════════════════════

llvm::CallInst* LLVMInlineAsm::emitInlineAsm(
    llvm::IRBuilder<>& builder,
    const std::string& asmStr,
    const std::vector<AsmOperand>& operands,
    bool hasSideEffects,
    bool isAlignStack) {
    
    // بناء نوع الدالة من المعاملات
    auto* funcTy = buildAsmFuncType(operands);
    
    // بناء سلسلة القيود
    std::string constraints = buildConstraintString(operands);
    
    // إنشاء كائن InlineAsm
    auto* inlineAsm = llvm::InlineAsm::get(
        funcTy, asmStr, constraints,
        hasSideEffects, isAlignStack,
        llvm::InlineAsm::AD_ATT);
    
    // جمع قيم المدخلات
    std::vector<llvm::Value*> args;
    for (const auto& op : operands) {
        if (!op.isOutput && op.value) {
            args.push_back(op.value);
        }
    }
    
    return builder.CreateCall(inlineAsm, args);
}

llvm::CallInst* LLVMInlineAsm::emitSimpleAsm(
    llvm::IRBuilder<>& builder,
    const std::string& asmStr,
    bool hasSideEffects) {
    
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    auto* funcTy = llvm::FunctionType::get(voidTy, false);
    
    auto* inlineAsm = llvm::InlineAsm::get(
        funcTy, asmStr, "",
        hasSideEffects, false,
        llvm::InlineAsm::AD_ATT);
    
    return builder.CreateCall(inlineAsm);
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    تعليمات عربية مُعرّفة مسبقاً
// ═══════════════════════════════════════════════════════════════════════════════

llvm::CallInst* LLVMInlineAsm::emitHalt(llvm::IRBuilder<>& builder) {
    return emitSimpleAsm(builder, "hlt", true);
}

llvm::CallInst* LLVMInlineAsm::emitDisableInterrupts(llvm::IRBuilder<>& builder) {
    return emitSimpleAsm(builder, "cli", true);
}

llvm::CallInst* LLVMInlineAsm::emitEnableInterrupts(llvm::IRBuilder<>& builder) {
    return emitSimpleAsm(builder, "sti", true);
}

llvm::CallInst* LLVMInlineAsm::emitNop(llvm::IRBuilder<>& builder) {
    return emitSimpleAsm(builder, "nop", false);
}

llvm::CallInst* LLVMInlineAsm::emitPause(llvm::IRBuilder<>& builder) {
    return emitSimpleAsm(builder, "pause", true);
}

llvm::CallInst* LLVMInlineAsm::emitInvalidateTLB(
    llvm::IRBuilder<>& builder,
    llvm::Value* address) {
    
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    auto* ptrTy = llvm::PointerType::getUnqual(m_ctx);
    
    auto* funcTy = llvm::FunctionType::get(voidTy, {ptrTy}, false);
    
    auto* inlineAsm = llvm::InlineAsm::get(
        funcTy, "invlpg ($0)", "r",
        true, false,
        llvm::InlineAsm::AD_ATT);
    
    return builder.CreateCall(inlineAsm, {address});
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    ترجمة التعليمات العربية
// ═══════════════════════════════════════════════════════════════════════════════

std::string LLVMInlineAsm::translateArabicMnemonic(const std::string& arabic) const {
    auto it = s_arabicMnemonics.find(arabic);
    if (it == s_arabicMnemonics.end()) {
        return arabic;  // إعادة كما هي إذا لم تكن معروفة
    }
    
    if (m_targetArch == "aarch64" || m_targetArch == "arm64") {
        return it->second.aarch64;
    }
    return it->second.x86_64;  // الافتراضي x86_64
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    مساعدات داخلية
// ═══════════════════════════════════════════════════════════════════════════════

std::string LLVMInlineAsm::buildConstraintString(
    const std::vector<AsmOperand>& operands) const {
    
    std::string result;
    bool first = true;
    
    // المخرجات أولاً
    for (const auto& op : operands) {
        if (op.isOutput) {
            if (!first) result += ",";
            result += op.constraint;  // مثل "=r"
            first = false;
        }
    }
    
    // ثم المدخلات
    for (const auto& op : operands) {
        if (!op.isOutput) {
            if (!first) result += ",";
            result += op.constraint;  // مثل "r"
            first = false;
        }
    }
    
    return result;
}

llvm::FunctionType* LLVMInlineAsm::buildAsmFuncType(
    const std::vector<AsmOperand>& operands) const {
    
    // جمع أنواع المخرجات
    std::vector<llvm::Type*> outputTypes;
    for (const auto& op : operands) {
        if (op.isOutput && op.type) {
            outputTypes.push_back(op.type);
        }
    }
    
    // نوع الإرجاع
    llvm::Type* retTy;
    if (outputTypes.empty()) {
        retTy = llvm::Type::getVoidTy(m_ctx);
    } else if (outputTypes.size() == 1) {
        retTy = outputTypes[0];
    } else {
        retTy = llvm::StructType::get(m_ctx, outputTypes);
    }
    
    // جمع أنواع المدخلات
    std::vector<llvm::Type*> inputTypes;
    for (const auto& op : operands) {
        if (!op.isOutput && op.type) {
            inputTypes.push_back(op.type);
        }
    }
    
    return llvm::FunctionType::get(retTy, inputTypes, false);
}

} // namespace codegen
} // namespace sad
