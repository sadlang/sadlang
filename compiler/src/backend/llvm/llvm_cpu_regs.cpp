/**
 * =============================================================================
 * ملف: llvm_cpu_regs.cpp
 * الوصف: تنفيذ قراءة/كتابة سجلات المعالج عبر LLVM IRBuilder
 * المهمة: م-د06
 * =============================================================================
 */

#include "backend/llvm/llvm_cpu_regs.h"

#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>

namespace sad {
namespace codegen {

LLVMCpuRegs::LLVMCpuRegs(llvm::Module& module)
    : m_module(module)
    , m_ctx(module.getContext()) {}

// ═══════════════════════════════════════════════════════════════════════════════
//                    مساعدات سجلات التحكم
// ═══════════════════════════════════════════════════════════════════════════════

llvm::Value* LLVMCpuRegs::emitReadCR(llvm::IRBuilder<>& builder,
                                       unsigned crNum) {
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    auto* funcTy = llvm::FunctionType::get(i64Ty, false);
    
    std::string asmStr = "mov %cr" + std::to_string(crNum) + ", $0";
    
    auto* inlineAsm = llvm::InlineAsm::get(
        funcTy, asmStr, "=r",
        true, false,
        llvm::InlineAsm::AD_ATT);
    
    auto* result = builder.CreateCall(inlineAsm);
    result->setName("cr" + std::to_string(crNum));
    return result;
}

void LLVMCpuRegs::emitWriteCR(llvm::IRBuilder<>& builder, unsigned crNum,
                                llvm::Value* value) {
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    auto* funcTy = llvm::FunctionType::get(voidTy, {i64Ty}, false);
    
    std::string asmStr = "mov $0, %cr" + std::to_string(crNum);
    
    auto* inlineAsm = llvm::InlineAsm::get(
        funcTy, asmStr, "r",
        true, false,
        llvm::InlineAsm::AD_ATT);
    
    builder.CreateCall(inlineAsm, {value});
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    سجلات التحكم
// ═══════════════════════════════════════════════════════════════════════════════

llvm::Value* LLVMCpuRegs::emitReadCR0(llvm::IRBuilder<>& builder) {
    return emitReadCR(builder, 0);
}

void LLVMCpuRegs::emitWriteCR0(llvm::IRBuilder<>& builder, llvm::Value* value) {
    emitWriteCR(builder, 0, value);
}

llvm::Value* LLVMCpuRegs::emitReadCR2(llvm::IRBuilder<>& builder) {
    return emitReadCR(builder, 2);
}

llvm::Value* LLVMCpuRegs::emitReadCR3(llvm::IRBuilder<>& builder) {
    return emitReadCR(builder, 3);
}

void LLVMCpuRegs::emitWriteCR3(llvm::IRBuilder<>& builder, llvm::Value* value) {
    emitWriteCR(builder, 3, value);
}

llvm::Value* LLVMCpuRegs::emitReadCR4(llvm::IRBuilder<>& builder) {
    return emitReadCR(builder, 4);
}

void LLVMCpuRegs::emitWriteCR4(llvm::IRBuilder<>& builder, llvm::Value* value) {
    emitWriteCR(builder, 4, value);
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    MSR
// ═══════════════════════════════════════════════════════════════════════════════

llvm::Value* LLVMCpuRegs::emitReadMSR(llvm::IRBuilder<>& builder,
                                        uint32_t msrNumber) {
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    return emitReadMSR(builder, llvm::ConstantInt::get(i32Ty, msrNumber));
}

llvm::Value* LLVMCpuRegs::emitReadMSR(llvm::IRBuilder<>& builder,
                                        llvm::Value* msrNumber) {
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    
    // rdmsr: ECX = رقم المسجل → EDX:EAX = القيمة
    // نستخدم asm مع مخرجين: EAX, EDX
    auto* retTy = llvm::StructType::get(m_ctx, {i32Ty, i32Ty});
    auto* funcTy = llvm::FunctionType::get(retTy, {i32Ty}, false);
    
    auto* inlineAsm = llvm::InlineAsm::get(
        funcTy, "rdmsr", "={eax},={edx},{ecx}",
        true, false,
        llvm::InlineAsm::AD_ATT);
    
    auto* result = builder.CreateCall(inlineAsm, {msrNumber});
    
    // دمج EDX:EAX في قيمة 64-بت
    auto* eax = builder.CreateExtractValue(result, 0, "msr_lo");
    auto* edx = builder.CreateExtractValue(result, 1, "msr_hi");
    
    auto* eax64 = builder.CreateZExt(eax, i64Ty);
    auto* edx64 = builder.CreateZExt(edx, i64Ty);
    auto* shifted = builder.CreateShl(edx64, 32);
    auto* combined = builder.CreateOr(eax64, shifted, "msr_val");
    
    return combined;
}

void LLVMCpuRegs::emitWriteMSR(llvm::IRBuilder<>& builder,
                                 uint32_t msrNumber,
                                 llvm::Value* value) {
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    emitWriteMSR(builder, llvm::ConstantInt::get(i32Ty, msrNumber), value);
}

void LLVMCpuRegs::emitWriteMSR(llvm::IRBuilder<>& builder,
                                 llvm::Value* msrNumber,
                                 llvm::Value* value) {
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    
    // تقسيم القيمة 64-بت إلى EAX و EDX
    auto* lo = builder.CreateTrunc(value, i32Ty, "msr_lo");
    auto* shifted = builder.CreateLShr(value, 32);
    auto* hi = builder.CreateTrunc(shifted, i32Ty, "msr_hi");
    
    // wrmsr: ECX = رقم المسجل, EDX:EAX = القيمة
    auto* funcTy = llvm::FunctionType::get(voidTy, {i32Ty, i32Ty, i32Ty}, false);
    
    auto* inlineAsm = llvm::InlineAsm::get(
        funcTy, "wrmsr", "{eax},{edx},{ecx}",
        true, false,
        llvm::InlineAsm::AD_ATT);
    
    builder.CreateCall(inlineAsm, {lo, hi, msrNumber});
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    RFLAGS
// ═══════════════════════════════════════════════════════════════════════════════

llvm::Value* LLVMCpuRegs::emitReadFlags(llvm::IRBuilder<>& builder) {
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    auto* funcTy = llvm::FunctionType::get(i64Ty, false);
    
    auto* inlineAsm = llvm::InlineAsm::get(
        funcTy, "pushfq; pop $0", "=r",
        true, false,
        llvm::InlineAsm::AD_ATT);
    
    auto* result = builder.CreateCall(inlineAsm);
    result->setName("rflags");
    return result;
}

void LLVMCpuRegs::emitWriteFlags(llvm::IRBuilder<>& builder,
                                   llvm::Value* value) {
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    auto* funcTy = llvm::FunctionType::get(voidTy, {i64Ty}, false);
    
    auto* inlineAsm = llvm::InlineAsm::get(
        funcTy, "push $0; popfq", "r",
        true, false,
        llvm::InlineAsm::AD_ATT);
    
    builder.CreateCall(inlineAsm, {value});
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    تعليمات خاصة
// ═══════════════════════════════════════════════════════════════════════════════

llvm::Value* LLVMCpuRegs::emitRDTSC(llvm::IRBuilder<>& builder) {
    auto* i64Ty = llvm::Type::getInt64Ty(m_ctx);
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    
    auto* retTy = llvm::StructType::get(m_ctx, {i32Ty, i32Ty});
    auto* funcTy = llvm::FunctionType::get(retTy, false);
    
    auto* inlineAsm = llvm::InlineAsm::get(
        funcTy, "rdtsc", "={eax},={edx}",
        true, false,
        llvm::InlineAsm::AD_ATT);
    
    auto* result = builder.CreateCall(inlineAsm);
    
    // دمج EDX:EAX
    auto* lo = builder.CreateExtractValue(result, 0);
    auto* hi = builder.CreateExtractValue(result, 1);
    auto* lo64 = builder.CreateZExt(lo, i64Ty);
    auto* hi64 = builder.CreateZExt(hi, i64Ty);
    auto* shifted = builder.CreateShl(hi64, 32);
    auto* tsc = builder.CreateOr(lo64, shifted, "tsc");
    
    return tsc;
}

llvm::Value* LLVMCpuRegs::emitCPUID(llvm::IRBuilder<>& builder,
                                      llvm::Value* leaf) {
    auto* i32Ty = llvm::Type::getInt32Ty(m_ctx);
    
    // CPUID: EAX = leaf → EAX, EBX, ECX, EDX
    auto* retTy = llvm::StructType::get(m_ctx, {i32Ty, i32Ty, i32Ty, i32Ty});
    auto* funcTy = llvm::FunctionType::get(retTy, {i32Ty}, false);
    
    auto* inlineAsm = llvm::InlineAsm::get(
        funcTy, "cpuid", "={eax},={ebx},={ecx},={edx},{eax}",
        true, false,
        llvm::InlineAsm::AD_ATT);
    
    auto* result = builder.CreateCall(inlineAsm, {leaf});
    result->setName("cpuid_result");
    return result;
}

void LLVMCpuRegs::emitWBINVD(llvm::IRBuilder<>& builder) {
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    auto* funcTy = llvm::FunctionType::get(voidTy, false);
    
    auto* inlineAsm = llvm::InlineAsm::get(
        funcTy, "wbinvd", "",
        true, false,
        llvm::InlineAsm::AD_ATT);
    
    builder.CreateCall(inlineAsm);
}

void LLVMCpuRegs::emitINVLPG(llvm::IRBuilder<>& builder,
                               llvm::Value* address) {
    auto* voidTy = llvm::Type::getVoidTy(m_ctx);
    auto* ptrTy = llvm::PointerType::getUnqual(m_ctx);
    auto* funcTy = llvm::FunctionType::get(voidTy, {ptrTy}, false);
    
    auto* inlineAsm = llvm::InlineAsm::get(
        funcTy, "invlpg ($0)", "r",
        true, false,
        llvm::InlineAsm::AD_ATT);
    
    builder.CreateCall(inlineAsm, {address});
}

} // namespace codegen
} // namespace sad
