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
#include "builders/platform/lowlevel_codegen.h"
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

llvm::Value* LowlevelCodeGen::emitLowlevelCpuGetInfo(std::shared_ptr<SIRInstruction> inst) {
    // (AR) استدعاء دالة runtime: sad_ll_cpu_get_info() -> i64 (مؤشر لبنية CPUInfo)
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_cpu_get_info", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelCpuGetFeatures(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_cpu_get_features", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelCpuReadMSR(std::shared_ptr<SIRInstruction> inst) {
    // (AR) rdmsr — قراءة سجل نموذج محدد
    // (EN) rdmsr instruction via inline assembly: ecx=reg -> edx:eax
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* reg = cg_.resolveOperand(inst->operands[0]);
    reg = cg_.builder_->CreateIntCast(reg, llvm::Type::getInt32Ty(*cg_.context_), false);
    
    // Use inline asm: rdmsr returns edx:eax combined into i64
    auto* asmTy = llvm::FunctionType::get(i64Ty, {llvm::Type::getInt32Ty(*cg_.context_)}, false);
    auto* inlineAsm = llvm::InlineAsm::get(asmTy,
        "rdmsr; shlq $$32, %rdx; orq %rdx, %rax",
        "={rax},{ecx},~{rdx}", true);
    return cg_.builder_->CreateCall(asmTy, inlineAsm, {reg});
}

llvm::Value* LowlevelCodeGen::emitLowlevelCpuWriteMSR(std::shared_ptr<SIRInstruction> inst) {
    // (AR) wrmsr — كتابة سجل نموذج محدد
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    
    llvm::Value* reg = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* val = cg_.resolveOperand(inst->operands[1]);
    reg = cg_.builder_->CreateIntCast(reg, i32Ty, false);
    
    // Split value: eax = low 32, edx = high 32
    llvm::Value* lo = cg_.builder_->CreateTrunc(val, i32Ty);
    llvm::Value* hi = cg_.builder_->CreateTrunc(cg_.builder_->CreateLShr(val, 32), i32Ty);
    
    auto* asmTy = llvm::FunctionType::get(voidTy, {i32Ty, i32Ty, i32Ty}, false);
    auto* inlineAsm = llvm::InlineAsm::get(asmTy,
        "wrmsr", "{ecx},{eax},{edx}", true);
    return cg_.builder_->CreateCall(asmTy, inlineAsm, {reg, lo, hi});
}

llvm::Value* LowlevelCodeGen::emitLowlevelCpuReadCR(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* crNum = cg_.resolveOperand(inst->operands[0]);
    // Call runtime — handles switch on CR number
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_read_cr", i64Ty, {i64Ty}, {crNum});
}

llvm::Value* LowlevelCodeGen::emitLowlevelCpuWriteCR(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* crNum = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* val = cg_.resolveOperand(inst->operands[1]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_write_cr", voidTy, {i64Ty, i64Ty}, {crNum, val});
}

llvm::Value* LowlevelCodeGen::emitLowlevelCpuInvlpg(std::shared_ptr<SIRInstruction> inst) {
    // (AR) invlpg — إبطال صفحة في TLB
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* addr = cg_.resolveOperand(inst->operands[0]);
    
    auto* asmTy = llvm::FunctionType::get(voidTy, {i64Ty}, false);
    auto* inlineAsm = llvm::InlineAsm::get(asmTy,
        "invlpg ($0)", "r", true);
    return cg_.builder_->CreateCall(asmTy, inlineAsm, {addr});
}

llvm::Value* LowlevelCodeGen::emitLowlevelCpuGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_cpu_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15b. وحدة GDT
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelGdtInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_gdt_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelGdtLoad(std::shared_ptr<SIRInstruction> inst) {
    // (AR) lgdt — تحميل جدول الواصفات العامة
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_gdt_load", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelGdtGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_gdt_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15c. وحدة الترحيل / Paging
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelPagingInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_paging_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelPagingMap(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* vaddr = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* paddr = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* flags = (inst->operands.size() > 2) ? 
        cg_.resolveOperand(inst->operands[2]) : 
        llvm::ConstantInt::get(i64Ty, 0x3); // default: present + writable
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_paging_map", voidTy, {i64Ty, i64Ty, i64Ty}, {vaddr, paddr, flags});
}

llvm::Value* LowlevelCodeGen::emitLowlevelPagingUnmap(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* vaddr = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_paging_unmap", voidTy, {i64Ty}, {vaddr});
}

llvm::Value* LowlevelCodeGen::emitLowlevelPagingFlushTlb(std::shared_ptr<SIRInstruction> inst) {
    // (AR) mov cr3, cr3 — إفراغ ذاكرة الترجمة بالكامل
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* asmTy = llvm::FunctionType::get(voidTy, {}, false);
    auto* inlineAsm = llvm::InlineAsm::get(asmTy,
        "movq %cr3, %rax; movq %rax, %cr3", "~{rax}", true);
    return cg_.builder_->CreateCall(asmTy, inlineAsm, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelPagingGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_paging_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15d. وحدة المقاطعات المتقدمة / Advanced Interrupts (IDT)
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelIdtInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_idt_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelIdtLoad(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_idt_load", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelIdtRegisterIsr(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* isrNum = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* handler = cg_.resolveOperand(inst->operands[1]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_idt_register_isr", voidTy, {i64Ty, i64Ty}, {isrNum, handler});
}

llvm::Value* LowlevelCodeGen::emitLowlevelIdtEnableIrq(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* irq = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_idt_enable_irq", voidTy, {i64Ty}, {irq});
}

llvm::Value* LowlevelCodeGen::emitLowlevelIdtGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_idt_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15e. وحدة PCI
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelPciEnumerate(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_pci_enumerate", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelPciReadConfig(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* bus = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* dev = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* func = cg_.resolveOperand(inst->operands[2]);
    llvm::Value* offset = cg_.resolveOperand(inst->operands[3]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_pci_read_config", i64Ty,
        {i64Ty, i64Ty, i64Ty, i64Ty}, {bus, dev, func, offset});
}

llvm::Value* LowlevelCodeGen::emitLowlevelPciWriteConfig(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* bus = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* dev = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* func = cg_.resolveOperand(inst->operands[2]);
    llvm::Value* offset = cg_.resolveOperand(inst->operands[3]);
    llvm::Value* value = cg_.resolveOperand(inst->operands[4]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_pci_write_config", voidTy,
        {i64Ty, i64Ty, i64Ty, i64Ty, i64Ty}, {bus, dev, func, offset, value});
}

llvm::Value* LowlevelCodeGen::emitLowlevelPciGetDeviceCount(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_pci_device_count", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelPciGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_pci_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15f. وحدة DMA المتقدمة
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelDmaInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_dma_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelDmaTransfer(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* src = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* dst = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* size = cg_.resolveOperand(inst->operands[2]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_dma_transfer", voidTy, {i64Ty, i64Ty, i64Ty}, {src, dst, size});
}

llvm::Value* LowlevelCodeGen::emitLowlevelDmaStatus(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_dma_status", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelDmaGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_dma_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15g. وحدة الشاشة / Framebuffer
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelFbInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* w = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* h = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* bpp = (inst->operands.size() > 2) ?
        cg_.resolveOperand(inst->operands[2]) :
        llvm::ConstantInt::get(i64Ty, 32);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_fb_init", voidTy, {i64Ty, i64Ty, i64Ty}, {w, h, bpp});
}

llvm::Value* LowlevelCodeGen::emitLowlevelFbSetPixel(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* x = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* y = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* color = cg_.resolveOperand(inst->operands[2]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_fb_set_pixel", voidTy, {i64Ty, i64Ty, i64Ty}, {x, y, color});
}

llvm::Value* LowlevelCodeGen::emitLowlevelFbDrawRect(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types(inst->operands.size(), i64Ty);
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) vals.push_back(cg_.resolveOperand(op));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_fb_draw_rect", voidTy, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelFbFillRect(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types(inst->operands.size(), i64Ty);
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) vals.push_back(cg_.resolveOperand(op));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_fb_fill_rect", voidTy, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelFbDrawLine(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types(inst->operands.size(), i64Ty);
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) vals.push_back(cg_.resolveOperand(op));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_fb_draw_line", voidTy, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelFbDrawString(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types(inst->operands.size(), i64Ty);
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) vals.push_back(cg_.resolveOperand(op));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_fb_draw_string", voidTy, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelFbClear(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    if (!inst->operands.empty()) {
        llvm::Value* color = cg_.resolveOperand(inst->operands[0]);
        return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
            "sad_ll_fb_clear", voidTy, {i64Ty}, {color});
    }
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_fb_clear_default", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelFbGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_fb_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15h. وحدة ACPI
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiFindTable(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* sig = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_find_table", i64Ty, {i64Ty}, {sig});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiShutdown(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_shutdown", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15i. وحدة التزامن / Sync
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelSpinlockInit(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_spinlock_init", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSpinlockLock(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* lock = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_spinlock_lock", voidTy, {i64Ty}, {lock});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSpinlockUnlock(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* lock = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_spinlock_unlock", voidTy, {i64Ty}, {lock});
}

llvm::Value* LowlevelCodeGen::emitLowlevelMutexInit(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_mutex_init", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelMutexLock(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* mtx = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_mutex_lock", voidTy, {i64Ty}, {mtx});
}

llvm::Value* LowlevelCodeGen::emitLowlevelMutexUnlock(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* mtx = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_mutex_unlock", voidTy, {i64Ty}, {mtx});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSemaphoreInit(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* count = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_semaphore_init", i64Ty, {i64Ty}, {count});
}

llvm::Value* LowlevelCodeGen::emitLowlevelBarrierInit(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* count = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_barrier_init", i64Ty, {i64Ty}, {count});
}

// ============================================================================
// 15j. وحدة المجدول / Scheduler
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelSchedInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_sched_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSchedCreateProc(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(i64Ty);
        vals.push_back(cg_.resolveOperand(op));
    }
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_sched_create_process", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelSchedCreateThread(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* name = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* entry = cg_.resolveOperand(inst->operands[1]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_sched_create_thread", i64Ty, {i64Ty, i64Ty}, {name, entry});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSchedYield(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_sched_yield", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSchedSleep(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* ms = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_sched_sleep", voidTy, {i64Ty}, {ms});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSchedGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_sched_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15k. وحدة الإقلاع / Boot
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelBootGetInfo(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_boot_info", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelBootGetMemoryMap(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_boot_memory_map", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelBootGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_boot_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15l. وحدة نظام الملفات الافتراضي / VFS
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelVfsMount(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(i64Ty);
        vals.push_back(cg_.resolveOperand(op));
    }
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_vfs_mount", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelVfsUnmount(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* mp = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_vfs_unmount", voidTy, {i64Ty}, {mp});
}

llvm::Value* LowlevelCodeGen::emitLowlevelVfsOpen(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(i64Ty);
        vals.push_back(cg_.resolveOperand(op));
    }
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_vfs_open", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelVfsRead(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* fd = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* size = cg_.resolveOperand(inst->operands[1]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_vfs_read", i64Ty, {i64Ty, i64Ty}, {fd, size});
}

llvm::Value* LowlevelCodeGen::emitLowlevelVfsWrite(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* fd = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* data = cg_.resolveOperand(inst->operands[1]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_vfs_write", i64Ty, {i64Ty, i64Ty}, {fd, data});
}

llvm::Value* LowlevelCodeGen::emitLowlevelVfsClose(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* fd = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_vfs_close", voidTy, {i64Ty}, {fd});
}

llvm::Value* LowlevelCodeGen::emitLowlevelVfsGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_vfs_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15m. وحدة APIC
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelApicInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicSendEoi(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_send_eoi", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicSendIpi(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* target = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* vector = cg_.resolveOperand(inst->operands[1]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_send_ipi", voidTy, {i64Ty, i64Ty}, {target, vector});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicSetTimer(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* divisor = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* count = cg_.resolveOperand(inst->operands[1]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_set_timer", voidTy, {i64Ty, i64Ty}, {divisor, count});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15n. وحدة HPET
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelHpetInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_hpet_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelHpetRead(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_hpet_read", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelHpetSleep(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* ns = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_hpet_sleep", voidTy, {i64Ty}, {ns});
}

llvm::Value* LowlevelCodeGen::emitLowlevelHpetGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_hpet_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15o. وحدة استدعاءات النظام / Syscall
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelSyscallInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_syscall_init", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSyscallRegister(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* num = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* handler = cg_.resolveOperand(inst->operands[1]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_syscall_register", voidTy, {i64Ty, i64Ty}, {num, handler});
}

llvm::Value* LowlevelCodeGen::emitLowlevelSyscallInvoke(std::shared_ptr<SIRInstruction> inst) {
    // (AR) syscall — تنفيذ استدعاء نظام بالرقم والمعاملات
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(i64Ty);
        vals.push_back(cg_.resolveOperand(op));
    }
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_syscall_invoke", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelSyscallGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_syscall_report", i8PtrTy, {}, {});
}

// ============================================================================
// 15p. عمليات الذاكرة المتقدمة / Advanced Memory
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelMemAllocPhys(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* size = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_mem_alloc_phys", i64Ty, {i64Ty}, {size});
}

llvm::Value* LowlevelCodeGen::emitLowlevelMemFreePhys(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    llvm::Value* addr = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_mem_free_phys", voidTy, {i64Ty}, {addr});
}

llvm::Value* LowlevelCodeGen::emitLowlevelMemMapRegion(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(i64Ty);
        vals.push_back(cg_.resolveOperand(op));
    }
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_mem_map_region", voidTy, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelMemGetReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_mem_report", i8PtrTy, {}, {});
}

// ============================================================================
// القسم 16: بروتوكول UEFI — توليد LLVM IR
// Section 16: UEFI Boot Protocol — LLVM IR Generation
// ============================================================================

} // namespace LLVM
} // namespace Sad

