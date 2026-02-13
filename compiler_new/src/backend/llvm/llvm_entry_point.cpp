/**
 * =============================================================================
 * ملف: llvm_entry_point.cpp
 * الوصف: تنفيذ توليد نقطة دخول النواة باستخدام LLVM IRBuilder
 * المهمة: م-د01 - تحويل EntryPointCodeGenerator
 * =============================================================================
 *
 * IR المُولَّد لـ _start (x86_64):
 *
 *   define void @_start() naked noreturn {
 *   entry:
 *     ; تصفير BSS
 *     call void @llvm.memset.p0.i64(ptr @__bss_start, i8 0, i64 bss_size, i1 false)
 *
 *     ; تهيئة المكدس
 *     call void asm "mov $0, %rsp", "r"(i64 stack_top)
 *
 *     ; استدعاء دالة النواة
 *     call void @kernel_main()
 *
 *     ; halt loop (إذا رجعت)
 *     call void asm sideeffect "cli", ""()
 *   halt:
 *     call void asm sideeffect "hlt", ""()
 *     br label %halt
 *   }
 * =============================================================================
 */

#include "backend/llvm/llvm_entry_point.h"

#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/Constants.h>

namespace sad {
namespace codegen {

LLVMEntryPoint::LLVMEntryPoint(llvm::Module& module)
    : m_module(module)
    , m_ctx(module.getContext()) {}

// ═══════════════════════════════════════════════════════════════════════════════
//                           التوليد الرئيسي
// ═══════════════════════════════════════════════════════════════════════════════

llvm::Function* LLVMEntryPoint::generate(const EntryPointConfig& config) {
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    auto* funcTy = llvm::FunctionType::get(voidTy, false);
    
    // إنشاء دالة _start
    auto* startFunc = llvm::Function::Create(
        funcTy, llvm::Function::ExternalLinkage,
        config.entryPointName, m_module);
    
    // سمات الدالة
    if (config.isNaked) {
        startFunc->addFnAttr(llvm::Attribute::Naked);
    }
    startFunc->addFnAttr(llvm::Attribute::NoReturn);
    startFunc->addFnAttr(llvm::Attribute::NoUnwind);
    startFunc->setDoesNotReturn();
    
    // ─── إنشاء الكتل الأساسية ─────────────────────────────────────
    auto* entryBB = llvm::BasicBlock::Create(m_ctx, "entry", startFunc);
    auto* haltBB = llvm::BasicBlock::Create(m_ctx, "halt", startFunc);
    
    llvm::IRBuilder<> builder(entryBB);
    
    // ─── 1. تصفير BSS ──────────────────────────────────────────────
    if (config.clearBSS) {
        emitBSSClear(builder);
    }
    
    // ─── 2. تهيئة المكدس ────────────────────────────────────────────
    if (config.initStack) {
        emitStackInit(builder, config.stackTop);
    }
    
    // ─── 3. استدعاء دالة النواة ──────────────────────────────────────
    emitKernelCall(builder, config.kernelFunctionName);
    
    // ─── 4. انتقال إلى halt loop ─────────────────────────────────────
    builder.CreateBr(haltBB);
    
    // ─── كتلة Halt ──────────────────────────────────────────────────
    builder.SetInsertPoint(haltBB);
    emitHaltLoop(builder);
    builder.CreateBr(haltBB); // حلقة لانهائية
    
    return startFunc;
}

llvm::Function* LLVMEntryPoint::generateFromAttribute(
    const std::string& kernelFunc, uint64_t stackSize) {
    
    EntryPointConfig config;
    config.kernelFunctionName = kernelFunc;
    
    if (stackSize > 0) {
        config.stackTop = stackSize;
    }
    
    return generate(config);
}

// ═══════════════════════════════════════════════════════════════════════════════
//                           الأجزاء الفرعية
// ═══════════════════════════════════════════════════════════════════════════════

void LLVMEntryPoint::emitBSSClear(llvm::IRBuilder<>& builder) {
    auto* i8Ty = llvm::Type::getInt8Ty(m_ctx);
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    auto* i8PtrTy = llvm::PointerType::get(i8Ty, 0);
    
    // رموز خارجية من سكربت الربط
    auto* bssStart = getOrCreateExternSymbol("__bss_start", i8Ty);
    auto* bssEnd = getOrCreateExternSymbol("__bss_end", i8Ty);
    
    // حساب حجم BSS
    auto* startPtr = builder.CreatePtrToInt(bssStart, i64Ty, "bss_start_int");
    auto* endPtr = builder.CreatePtrToInt(bssEnd, i64Ty, "bss_end_int");
    auto* bssSize = builder.CreateSub(endPtr, startPtr, "bss_size");
    
    // استدعاء llvm.memset لتصفير BSS
    builder.CreateMemSet(
        bssStart,                                    // الوجهة
        llvm::ConstantInt::get(i8Ty, 0),            // القيمة (صفر)
        bssSize,                                     // الحجم
        llvm::MaybeAlign(1));                        // المحاذاة
}

void LLVMEntryPoint::emitStackInit(llvm::IRBuilder<>& builder, uint64_t stackTop) {
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    
    // تعليمة تجميع لضبط rsp
    auto* asmTy = llvm::FunctionType::get(voidTy, {i64Ty}, false);
    auto* setRsp = llvm::InlineAsm::get(
        asmTy, "mov $0, %rsp", "r", true);
    
    auto* stackVal = llvm::ConstantInt::get(i64Ty, stackTop);
    builder.CreateCall(setRsp, {stackVal});
}

void LLVMEntryPoint::emitKernelCall(llvm::IRBuilder<>& builder,
                                     const std::string& funcName) {
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    
    // البحث عن الدالة أو إعلانها
    auto* kernelFunc = m_module.getFunction(funcName);
    if (!kernelFunc) {
        auto* funcTy = llvm::FunctionType::get(voidTy, false);
        kernelFunc = llvm::Function::Create(
            funcTy, llvm::Function::ExternalLinkage,
            funcName, m_module);
    }
    
    builder.CreateCall(kernelFunc);
}

void LLVMEntryPoint::emitHaltLoop(llvm::IRBuilder<>& builder) {
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    
    // cli — تعطيل المقاطعات
    auto* cliTy = llvm::FunctionType::get(voidTy, false);
    auto* cliAsm = llvm::InlineAsm::get(cliTy, "cli", "", true);
    builder.CreateCall(cliAsm);
    
    // hlt — إيقاف المعالج
    auto* hltAsm = llvm::InlineAsm::get(cliTy, "hlt", "", true);
    builder.CreateCall(hltAsm);
}

llvm::GlobalVariable* LLVMEntryPoint::getOrCreateExternSymbol(
    const std::string& name, llvm::Type* type) {
    
    auto* existing = m_module.getGlobalVariable(name);
    if (existing) return existing;
    
    return new llvm::GlobalVariable(
        m_module, type, false,
        llvm::GlobalVariable::ExternalLinkage,
        nullptr, name);
}

} // namespace codegen
} // namespace sad
