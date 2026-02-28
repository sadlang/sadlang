// ============================================================================
// llvm_codegen_lowlevel.cpp — توليد LLVM IR لعمليات المكتبة المنخفضة المستوى
// LLVM IR Generation for Low-Level OS Library Operations
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 2026
// الإصدار / Version: 5.0 — دعم 19 وحدة لبرمجة أنظمة التشغيل
//
// (AR) هذا الملف يولّد LLVM IR لعمليات المكتبة المنخفضة المستوى.
//      العمليات البدائية تُترجم إلى inline assembly مباشرة.
//      العمليات العالية المستوى تُترجم إلى استدعاءات runtime C.
// (EN) This file generates LLVM IR for low-level library operations.
//      Primitive ops are translated to inline assembly directly.
//      High-level ops are translated to C runtime function calls.
// ============================================================================

#include "llvm_codegen.h"
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Intrinsics.h>

namespace Sad {
namespace LLVM {

using SIRInstruction = Compiler::SIR::SIRInstruction;
using SIROperand = Compiler::SIR::SIROperand;
using SIROpcode = Compiler::SIR::SIROpcode;

// ============================================================================
// Helper: emit a call to a freestanding runtime C function
// ============================================================================
static llvm::Value* emitRuntimeCall(
    LLVMCodeGen* cg,
    llvm::IRBuilder<>& builder,
    llvm::Module* module,
    const std::string& funcName,
    llvm::Type* retType,
    const std::vector<llvm::Type*>& argTypes,
    const std::vector<llvm::Value*>& argValues)
{
    llvm::Function* fn = module->getFunction(funcName);
    if (!fn) {
        llvm::FunctionType* ft = llvm::FunctionType::get(retType, argTypes, false);
        fn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, funcName, module);
    }
    return builder.CreateCall(fn, argValues);
}

// ============================================================================
// 15a. وحدة المعالج المتقدمة / Advanced CPU Module
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelCpuGetInfo(std::shared_ptr<SIRInstruction> inst) {
    // (AR) استدعاء دالة runtime: sad_ll_cpu_get_info() -> i64 (مؤشر لبنية CPUInfo)
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_cpu_get_info", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelCpuGetFeatures(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_cpu_get_features", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelCpuReadMSR(std::shared_ptr<SIRInstruction> inst) {
    // (AR) rdmsr — قراءة سجل نموذج محدد
    // (EN) rdmsr instruction via inline assembly: ecx=reg -> edx:eax
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* reg = resolveOperand(inst->operands[0]);
    reg = builder_->CreateIntCast(reg, llvm::Type::getInt32Ty(*context_), false);
    
    // Use inline asm: rdmsr returns edx:eax combined into i64
    auto* asmTy = llvm::FunctionType::get(i64Ty, {llvm::Type::getInt32Ty(*context_)}, false);
    auto* inlineAsm = llvm::InlineAsm::get(asmTy,
        "rdmsr; shlq $$32, %rdx; orq %rdx, %rax",
        "={rax},{ecx},~{rdx}", true);
    return builder_->CreateCall(asmTy, inlineAsm, {reg});
}

llvm::Value* LLVMCodeGen::emitLowlevelCpuWriteMSR(std::shared_ptr<SIRInstruction> inst) {
    // (AR) wrmsr — كتابة سجل نموذج محدد
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    
    llvm::Value* reg = resolveOperand(inst->operands[0]);
    llvm::Value* val = resolveOperand(inst->operands[1]);
    reg = builder_->CreateIntCast(reg, i32Ty, false);
    
    // Split value: eax = low 32, edx = high 32
    llvm::Value* lo = builder_->CreateTrunc(val, i32Ty);
    llvm::Value* hi = builder_->CreateTrunc(builder_->CreateLShr(val, 32), i32Ty);
    
    auto* asmTy = llvm::FunctionType::get(voidTy, {i32Ty, i32Ty, i32Ty}, false);
    auto* inlineAsm = llvm::InlineAsm::get(asmTy,
        "wrmsr", "{ecx},{eax},{edx}", true);
    return builder_->CreateCall(asmTy, inlineAsm, {reg, lo, hi});
}

llvm::Value* LLVMCodeGen::emitLowlevelCpuReadCR(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* crNum = resolveOperand(inst->operands[0]);
    // Call runtime — handles switch on CR number
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_read_cr", i64Ty, {i64Ty}, {crNum});
}

llvm::Value* LLVMCodeGen::emitLowlevelCpuWriteCR(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* crNum = resolveOperand(inst->operands[0]);
    llvm::Value* val = resolveOperand(inst->operands[1]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_write_cr", voidTy, {i64Ty, i64Ty}, {crNum, val});
}

llvm::Value* LLVMCodeGen::emitLowlevelCpuInvlpg(std::shared_ptr<SIRInstruction> inst) {
    // (AR) invlpg — إبطال صفحة في TLB
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* addr = resolveOperand(inst->operands[0]);
    
    auto* asmTy = llvm::FunctionType::get(voidTy, {i64Ty}, false);
    auto* inlineAsm = llvm::InlineAsm::get(asmTy,
        "invlpg ($0)", "r", true);
    return builder_->CreateCall(asmTy, inlineAsm, {addr});
}

llvm::Value* LLVMCodeGen::emitLowlevelCpuGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_cpu_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15b. وحدة GDT
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelGdtInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_gdt_init", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelGdtLoad(std::shared_ptr<SIRInstruction> inst) {
    // (AR) lgdt — تحميل جدول الواصفات العامة
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_gdt_load", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelGdtGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_gdt_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15c. وحدة الترحيل / Paging
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelPagingInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_paging_init", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelPagingMap(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* vaddr = resolveOperand(inst->operands[0]);
    llvm::Value* paddr = resolveOperand(inst->operands[1]);
    llvm::Value* flags = (inst->operands.size() > 2) ? 
        resolveOperand(inst->operands[2]) : 
        llvm::ConstantInt::get(i64Ty, 0x3); // default: present + writable
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_paging_map", voidTy, {i64Ty, i64Ty, i64Ty}, {vaddr, paddr, flags});
}

llvm::Value* LLVMCodeGen::emitLowlevelPagingUnmap(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* vaddr = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_paging_unmap", voidTy, {i64Ty}, {vaddr});
}

llvm::Value* LLVMCodeGen::emitLowlevelPagingFlushTlb(std::shared_ptr<SIRInstruction> inst) {
    // (AR) mov cr3, cr3 — إفراغ ذاكرة الترجمة بالكامل
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* asmTy = llvm::FunctionType::get(voidTy, {}, false);
    auto* inlineAsm = llvm::InlineAsm::get(asmTy,
        "movq %cr3, %rax; movq %rax, %cr3", "~{rax}", true);
    return builder_->CreateCall(asmTy, inlineAsm, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelPagingGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_paging_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15d. وحدة المقاطعات المتقدمة / Advanced Interrupts (IDT)
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelIdtInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_idt_init", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelIdtLoad(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_idt_load", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelIdtRegisterIsr(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* isrNum = resolveOperand(inst->operands[0]);
    llvm::Value* handler = resolveOperand(inst->operands[1]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_idt_register_isr", voidTy, {i64Ty, i64Ty}, {isrNum, handler});
}

llvm::Value* LLVMCodeGen::emitLowlevelIdtEnableIrq(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* irq = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_idt_enable_irq", voidTy, {i64Ty}, {irq});
}

llvm::Value* LLVMCodeGen::emitLowlevelIdtGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_idt_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15e. وحدة PCI
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelPciEnumerate(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_pci_enumerate", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelPciReadConfig(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* bus = resolveOperand(inst->operands[0]);
    llvm::Value* dev = resolveOperand(inst->operands[1]);
    llvm::Value* func = resolveOperand(inst->operands[2]);
    llvm::Value* offset = resolveOperand(inst->operands[3]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_pci_read_config", i64Ty,
        {i64Ty, i64Ty, i64Ty, i64Ty}, {bus, dev, func, offset});
}

llvm::Value* LLVMCodeGen::emitLowlevelPciWriteConfig(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* bus = resolveOperand(inst->operands[0]);
    llvm::Value* dev = resolveOperand(inst->operands[1]);
    llvm::Value* func = resolveOperand(inst->operands[2]);
    llvm::Value* offset = resolveOperand(inst->operands[3]);
    llvm::Value* value = resolveOperand(inst->operands[4]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_pci_write_config", voidTy,
        {i64Ty, i64Ty, i64Ty, i64Ty, i64Ty}, {bus, dev, func, offset, value});
}

llvm::Value* LLVMCodeGen::emitLowlevelPciGetDeviceCount(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_pci_device_count", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelPciGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_pci_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15f. وحدة DMA المتقدمة
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelDmaInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_dma_init", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelDmaTransfer(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* src = resolveOperand(inst->operands[0]);
    llvm::Value* dst = resolveOperand(inst->operands[1]);
    llvm::Value* size = resolveOperand(inst->operands[2]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_dma_transfer", voidTy, {i64Ty, i64Ty, i64Ty}, {src, dst, size});
}

llvm::Value* LLVMCodeGen::emitLowlevelDmaStatus(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_dma_status", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelDmaGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_dma_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15g. وحدة الشاشة / Framebuffer
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelFbInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* w = resolveOperand(inst->operands[0]);
    llvm::Value* h = resolveOperand(inst->operands[1]);
    llvm::Value* bpp = (inst->operands.size() > 2) ?
        resolveOperand(inst->operands[2]) :
        llvm::ConstantInt::get(i64Ty, 32);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_fb_init", voidTy, {i64Ty, i64Ty, i64Ty}, {w, h, bpp});
}

llvm::Value* LLVMCodeGen::emitLowlevelFbSetPixel(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* x = resolveOperand(inst->operands[0]);
    llvm::Value* y = resolveOperand(inst->operands[1]);
    llvm::Value* color = resolveOperand(inst->operands[2]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_fb_set_pixel", voidTy, {i64Ty, i64Ty, i64Ty}, {x, y, color});
}

llvm::Value* LLVMCodeGen::emitLowlevelFbDrawRect(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Type*> types(inst->operands.size(), i64Ty);
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) vals.push_back(resolveOperand(op));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_fb_draw_rect", voidTy, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelFbFillRect(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Type*> types(inst->operands.size(), i64Ty);
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) vals.push_back(resolveOperand(op));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_fb_fill_rect", voidTy, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelFbDrawLine(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Type*> types(inst->operands.size(), i64Ty);
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) vals.push_back(resolveOperand(op));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_fb_draw_line", voidTy, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelFbDrawString(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Type*> types(inst->operands.size(), i64Ty);
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) vals.push_back(resolveOperand(op));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_fb_draw_string", voidTy, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelFbClear(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    if (!inst->operands.empty()) {
        llvm::Value* color = resolveOperand(inst->operands[0]);
        return emitRuntimeCall(this, *builder_, module_.get(),
            "sad_ll_fb_clear", voidTy, {i64Ty}, {color});
    }
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_fb_clear_default", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelFbGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_fb_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15h. وحدة ACPI
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelAcpiInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_init", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelAcpiFindTable(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* sig = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_find_table", i64Ty, {i64Ty}, {sig});
}

llvm::Value* LLVMCodeGen::emitLowlevelAcpiShutdown(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_shutdown", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelAcpiGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15i. وحدة التزامن / Sync
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelSpinlockInit(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_spinlock_init", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelSpinlockLock(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* lock = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_spinlock_lock", voidTy, {i64Ty}, {lock});
}

llvm::Value* LLVMCodeGen::emitLowlevelSpinlockUnlock(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* lock = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_spinlock_unlock", voidTy, {i64Ty}, {lock});
}

llvm::Value* LLVMCodeGen::emitLowlevelMutexInit(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_mutex_init", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelMutexLock(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* mtx = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_mutex_lock", voidTy, {i64Ty}, {mtx});
}

llvm::Value* LLVMCodeGen::emitLowlevelMutexUnlock(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* mtx = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_mutex_unlock", voidTy, {i64Ty}, {mtx});
}

llvm::Value* LLVMCodeGen::emitLowlevelSemaphoreInit(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* count = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_semaphore_init", i64Ty, {i64Ty}, {count});
}

llvm::Value* LLVMCodeGen::emitLowlevelBarrierInit(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* count = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_barrier_init", i64Ty, {i64Ty}, {count});
}

// ============================================================================
// 15j. وحدة المجدول / Scheduler
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelSchedInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_sched_init", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelSchedCreateProc(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(i64Ty);
        vals.push_back(resolveOperand(op));
    }
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_sched_create_process", i64Ty, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelSchedCreateThread(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* name = resolveOperand(inst->operands[0]);
    llvm::Value* entry = resolveOperand(inst->operands[1]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_sched_create_thread", i64Ty, {i64Ty, i64Ty}, {name, entry});
}

llvm::Value* LLVMCodeGen::emitLowlevelSchedYield(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_sched_yield", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelSchedSleep(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* ms = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_sched_sleep", voidTy, {i64Ty}, {ms});
}

llvm::Value* LLVMCodeGen::emitLowlevelSchedGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_sched_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15k. وحدة الإقلاع / Boot
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelBootGetInfo(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_boot_info", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelBootGetMemoryMap(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_boot_memory_map", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelBootGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_boot_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15l. وحدة نظام الملفات الافتراضي / VFS
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelVfsMount(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(i64Ty);
        vals.push_back(resolveOperand(op));
    }
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_vfs_mount", i64Ty, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelVfsUnmount(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* mp = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_vfs_unmount", voidTy, {i64Ty}, {mp});
}

llvm::Value* LLVMCodeGen::emitLowlevelVfsOpen(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(i64Ty);
        vals.push_back(resolveOperand(op));
    }
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_vfs_open", i64Ty, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelVfsRead(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* fd = resolveOperand(inst->operands[0]);
    llvm::Value* size = resolveOperand(inst->operands[1]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_vfs_read", i64Ty, {i64Ty, i64Ty}, {fd, size});
}

llvm::Value* LLVMCodeGen::emitLowlevelVfsWrite(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* fd = resolveOperand(inst->operands[0]);
    llvm::Value* data = resolveOperand(inst->operands[1]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_vfs_write", i64Ty, {i64Ty, i64Ty}, {fd, data});
}

llvm::Value* LLVMCodeGen::emitLowlevelVfsClose(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* fd = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_vfs_close", voidTy, {i64Ty}, {fd});
}

llvm::Value* LLVMCodeGen::emitLowlevelVfsGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_vfs_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15m. وحدة APIC
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelApicInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_init", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicSendEoi(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_send_eoi", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicSendIpi(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* target = resolveOperand(inst->operands[0]);
    llvm::Value* vector = resolveOperand(inst->operands[1]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_send_ipi", voidTy, {i64Ty, i64Ty}, {target, vector});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicSetTimer(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* divisor = resolveOperand(inst->operands[0]);
    llvm::Value* count = resolveOperand(inst->operands[1]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_set_timer", voidTy, {i64Ty, i64Ty}, {divisor, count});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15n. وحدة HPET
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelHpetInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_hpet_init", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelHpetRead(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_hpet_read", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelHpetSleep(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* ns = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_hpet_sleep", voidTy, {i64Ty}, {ns});
}

llvm::Value* LLVMCodeGen::emitLowlevelHpetGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_hpet_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15o. وحدة استدعاءات النظام / Syscall
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelSyscallInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_syscall_init", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelSyscallRegister(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* num = resolveOperand(inst->operands[0]);
    llvm::Value* handler = resolveOperand(inst->operands[1]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_syscall_register", voidTy, {i64Ty, i64Ty}, {num, handler});
}

llvm::Value* LLVMCodeGen::emitLowlevelSyscallInvoke(std::shared_ptr<SIRInstruction> inst) {
    // (AR) syscall — تنفيذ استدعاء نظام بالرقم والمعاملات
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(i64Ty);
        vals.push_back(resolveOperand(op));
    }
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_syscall_invoke", i64Ty, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelSyscallGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_syscall_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15p. عمليات الذاكرة المتقدمة / Advanced Memory
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelMemAllocPhys(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* size = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_mem_alloc_phys", i64Ty, {i64Ty}, {size});
}

llvm::Value* LLVMCodeGen::emitLowlevelMemFreePhys(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* addr = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_mem_free_phys", voidTy, {i64Ty}, {addr});
}

llvm::Value* LLVMCodeGen::emitLowlevelMemMapRegion(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(i64Ty);
        vals.push_back(resolveOperand(op));
    }
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_mem_map_region", voidTy, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelMemGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_mem_report", i8PtrTy, {}, {});
}

// ============================================================================
// القسم 16: بروتوكول UEFI — توليد LLVM IR
// Section 16: UEFI Boot Protocol — LLVM IR Generation
// ============================================================================
//
// (AR) جميع عمليات UEFI تُترجم إلى استدعاءات دوال runtime خارجية
//      بالبادئة sad_ll_uefi_* والتي يتم ربطها في وقت الربط النهائي.
//      على بيئة UEFI الحقيقية، هذه الدوال تستدعي خدمات UEFI مباشرة.
//      على بيئة المحاكاة، يتم ربطها بتنفيذ وهمي.
//
// (EN) All UEFI operations are translated to external runtime function calls
//      prefixed with sad_ll_uefi_* which are linked at final link time.
//      On real UEFI environment, these call UEFI services directly.
//      On simulation, they link to a stub implementation.
// ============================================================================

// --- 16a. التهيئة والتحكم / Initialization & Control ---

llvm::Value* LLVMCodeGen::emitLowlevelUefiInit(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Type*> types = {i64Ty, i64Ty};
    std::vector<llvm::Value*> vals;
    if (inst->operands.size() >= 2) {
        vals.push_back(resolveOperand(inst->operands[0]));
        vals.push_back(resolveOperand(inst->operands[1]));
    } else {
        vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
        vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    }
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_init", i64Ty, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiExitBootServices(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_exit_boot_services", i64Ty, {i64Ty}, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiIsInitialized(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_is_initialized", i32Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiBsExited(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_bs_exited", i32Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiResetSystem(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::ConstantInt::get(i32Ty, 0));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_reset_system", voidTy, {i32Ty}, vals);
}

// --- 16b. إدارة الذاكرة / Memory Services ---

llvm::Value* LLVMCodeGen::emitLowlevelUefiAllocPages(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    std::vector<llvm::Type*> types = {i32Ty, i32Ty, i64Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(3)); i++)
        vals.push_back(resolveOperand(inst->operands[i]));
    while (vals.size() < 3) vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_alloc_pages", i64Ty, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiFreePages(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Type*> types = {i64Ty, i64Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(2)); i++)
        vals.push_back(resolveOperand(inst->operands[i]));
    while (vals.size() < 2) vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_free_pages", i64Ty, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiAllocPool(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    std::vector<llvm::Type*> types = {i32Ty, i64Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(2)); i++)
        vals.push_back(resolveOperand(inst->operands[i]));
    while (vals.size() < 2) vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_alloc_pool", i64Ty, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiFreePool(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_free_pool", i64Ty, {i64Ty}, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiGetMemoryMap(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_get_memory_map", i8PtrTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiGetMemmapKey(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_get_memmap_key", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiTotalMemory(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_total_memory", i64Ty, {}, {});
}

// --- 16c. بروتوكول الرسوميات GOP / Graphics Output Protocol ---

llvm::Value* LLVMCodeGen::emitLowlevelUefiInitGop(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_init_gop", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiSetGopMode(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::ConstantInt::get(i32Ty, 0));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_set_gop_mode", i64Ty, {i32Ty}, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiQueryGopMode(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::ConstantInt::get(i32Ty, 0));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_query_gop_mode", i8PtrTy, {i32Ty}, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiGopModeCount(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_gop_mode_count", i32Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiCurrentGopMode(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_current_gop_mode", i32Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiFramebufferBase(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_framebuffer_base", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiFramebufferSize(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_framebuffer_size", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiFillScreen(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    std::vector<llvm::Type*> types = {i32Ty, i32Ty, i32Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(3)); i++)
        vals.push_back(resolveOperand(inst->operands[i]));
    while (vals.size() < 3) vals.push_back(llvm::ConstantInt::get(i32Ty, 0));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_fill_screen", i64Ty, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiDrawRect(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    std::vector<llvm::Type*> types = {i32Ty, i32Ty, i32Ty, i32Ty, i32Ty, i32Ty, i32Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(7)); i++)
        vals.push_back(resolveOperand(inst->operands[i]));
    while (vals.size() < 7) vals.push_back(llvm::ConstantInt::get(i32Ty, 0));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_draw_rect", i64Ty, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiGopBlt(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    // buffer, op, srcX, srcY, dstX, dstY, width, height
    std::vector<llvm::Type*> types = {i8PtrTy, i32Ty, i32Ty, i32Ty, i32Ty, i32Ty, i32Ty, i32Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(8)); i++)
        vals.push_back(resolveOperand(inst->operands[i]));
    while (vals.size() < 8) vals.push_back(llvm::ConstantInt::get(i32Ty, 0));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_gop_blt", i64Ty, types, vals);
}

// --- 16d. خدمات وقت التشغيل / Runtime Services ---

llvm::Value* LLVMCodeGen::emitLowlevelUefiGetTime(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_get_time", i8PtrTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiSetTime(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(llvm::Type::getInt32Ty(*context_));
        vals.push_back(resolveOperand(op));
    }
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_set_time", i64Ty, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiGetVariable(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::Constant::getNullValue(i8PtrTy));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_get_variable", i8PtrTy, {i8PtrTy}, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiSetVariable(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    std::vector<llvm::Type*> types = {i8PtrTy, i8PtrTy, i32Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(3)); i++)
        vals.push_back(resolveOperand(inst->operands[i]));
    while (vals.size() < 3) vals.push_back(llvm::ConstantInt::get(i32Ty, 7));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_set_variable", i64Ty, types, vals);
}

// --- 16e. نظام الملفات / File System ---

llvm::Value* LLVMCodeGen::emitLowlevelUefiOpenVolume(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_open_volume", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiOpenFile(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    std::vector<llvm::Type*> types = {i64Ty, i8PtrTy, i64Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(3)); i++)
        vals.push_back(resolveOperand(inst->operands[i]));
    while (vals.size() < 3) vals.push_back(llvm::ConstantInt::get(i64Ty, 1));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_open_file", i64Ty, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiReadFile(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Type*> types = {i64Ty, i64Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(2)); i++)
        vals.push_back(resolveOperand(inst->operands[i]));
    while (vals.size() < 2) vals.push_back(llvm::ConstantInt::get(i64Ty, 4096));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_read_file", i8PtrTy, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiWriteFile(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    std::vector<llvm::Type*> types = {i64Ty, i8PtrTy, i64Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(3)); i++)
        vals.push_back(resolveOperand(inst->operands[i]));
    while (vals.size() < 3) vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_write_file", i64Ty, types, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiCloseFile(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_close_file", i64Ty, {i64Ty}, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiFileInfo(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_file_info", i8PtrTy, {i64Ty}, vals);
}

// --- 16f. بروتوكولات ومعلومات / Protocols & System Info ---

llvm::Value* LLVMCodeGen::emitLowlevelUefiLocateProtocol(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::Constant::getNullValue(i8PtrTy));
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_locate_protocol", i64Ty, {i8PtrTy}, vals);
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiRevision(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_revision", i8PtrTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiVendor(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_vendor", i8PtrTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiFwRevision(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_fw_revision", i32Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelUefiReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_uefi_report", i8PtrTy, {}, {});
}

// ============================================================================
// القسم 17: ACPI الموسّع / Extended ACPI
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelAcpiInitFull(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_init_full", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelAcpiInitRsdp(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* addr = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_init_rsdp", voidTy, {i64Ty}, {addr});
}

llvm::Value* LLVMCodeGen::emitLowlevelAcpiEnable(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_enable", i32Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelAcpiDisable(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_disable", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelAcpiIsInitialized(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_is_initialized", i32Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelAcpiVersion(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_version", i32Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelAcpiReboot(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_reboot", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelAcpiSleep(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* state = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_sleep", i32Ty, {i32Ty}, {state});
}

llvm::Value* LLVMCodeGen::emitLowlevelAcpiDelayUs(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* us = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_delay_us", voidTy, {i32Ty}, {us});
}

llvm::Value* LLVMCodeGen::emitLowlevelAcpiReadPmTimer(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_read_pm_timer", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelAcpiIsPm32bit(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_is_pm_32bit", i32Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelAcpiProcessorCount(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_processor_count", i32Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelAcpiLocalApicAddr(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_local_apic_addr", i64Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelAcpiEcamBase(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* seg = inst->operands.size() > 0 ? resolveOperand(inst->operands[0]) : llvm::ConstantInt::get(i32Ty, 0);
    auto* bus = inst->operands.size() > 1 ? resolveOperand(inst->operands[1]) : llvm::ConstantInt::get(i32Ty, 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_acpi_ecam_base", i64Ty, {i32Ty, i32Ty}, {seg, bus});
}

// ============================================================================
// القسم 18: APIC الموسّع / Extended APIC
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelApicSupported(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_supported", i32Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicX2Supported(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_x2_supported", i32Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicId(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_id", i32Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicIoCount(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_io_count", i32Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicInitTimer(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* vec = inst->operands.size() > 0 ? resolveOperand(inst->operands[0]) : llvm::ConstantInt::get(i32Ty, 32);
    auto* mode = inst->operands.size() > 1 ? resolveOperand(inst->operands[1]) : llvm::ConstantInt::get(i32Ty, 0);
    auto* div = inst->operands.size() > 2 ? resolveOperand(inst->operands[2]) : llvm::ConstantInt::get(i32Ty, 3);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_init_timer", voidTy, {i32Ty, i32Ty, i32Ty}, {vec, mode, div});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicStartTimer(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* count = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_start_timer", voidTy, {i32Ty}, {count});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicStopTimer(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_stop_timer", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicTimerCount(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_timer_count", i32Ty, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicCalibrate(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* hz = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_calibrate", i32Ty, {i32Ty}, {hz});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicSetPriority(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* prio = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_set_priority", voidTy, {i32Ty}, {prio});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicDisablePic(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_disable_pic", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicMaskIrq(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* irq = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_mask_irq", voidTy, {i32Ty}, {irq});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicUnmaskIrq(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* irq = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_unmask_irq", voidTy, {i32Ty}, {irq});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicRouteIrq(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* irq = resolveOperand(inst->operands[0]);
    auto* vec = resolveOperand(inst->operands[1]);
    auto* dest = resolveOperand(inst->operands[2]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_route_irq", voidTy, {i32Ty, i32Ty, i32Ty}, {irq, vec, dest});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicSendIpiAll(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* vec = resolveOperand(inst->operands[0]);
    auto* self = inst->operands.size() > 1 ? resolveOperand(inst->operands[1]) : llvm::ConstantInt::get(i32Ty, 0);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_send_ipi_all", voidTy, {i32Ty, i32Ty}, {vec, self});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicSendInitIpi(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* dest = resolveOperand(inst->operands[0]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_send_init_ipi", voidTy, {i32Ty}, {dest});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicSendSipi(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* dest = resolveOperand(inst->operands[0]);
    auto* page = resolveOperand(inst->operands[1]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_send_sipi", voidTy, {i32Ty, i32Ty}, {dest, page});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicWaitDelivery(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_wait_delivery", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelApicInitIo(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* id = resolveOperand(inst->operands[0]);
    auto* base = resolveOperand(inst->operands[1]);
    auto* gsib = resolveOperand(inst->operands[2]);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_apic_init_io", i32Ty, {i32Ty, i64Ty, i32Ty}, {id, base, gsib});
}

} // namespace LLVM
} // namespace Sad
