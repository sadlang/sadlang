// ============================================================================
// llvm_codegen_lowlevel.cpp ג€” ״×ˆ„״¯ LLVM IR „״¹…„״§״× ״§„…ƒ״×״¨״© ״§„…†״®״¶״© ״§„…״³״×ˆ‰
// LLVM IR Generation for Low-Level OS Library Operations
// ============================================================================
// ״§„…״₪„ / Author: Sad Compiler Team
// ״§„״×״§״±״® / Date: January 2026
// ״§„״¥״µ״¯״§״± / Version: 5.0 ג€” ״¯״¹… 19 ˆ״­״¯״© „״¨״±…״¬״© ״£†״¸…״© ״§„״×״´״÷„
//
// (AR) ‡״°״§ ״§„…„ ˆ„‘״¯ LLVM IR „״¹…„״§״× ״§„…ƒ״×״¨״© ״§„…†״®״¶״© ״§„…״³״×ˆ‰.
//      ״§„״¹…„״§״× ״§„״¨״¯״§״¦״© ״×״×״±״¬… ״¥„‰ inline assembly …״¨״§״´״±״©.
//      ״§„״¹…„״§״× ״§„״¹״§„״© ״§„…״³״×ˆ‰ ״×״×״±״¬… ״¥„‰ ״§״³״×״¯״¹״§״¡״§״× runtime C.
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
// 15a. ˆ״­״¯״© ״§„…״¹״§„״¬ ״§„…״×‚״¯…״© / Advanced CPU Module
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelCpuGetInfo(std::shared_ptr<SIRInstruction> inst) {
    // (AR) ״§״³״×״¯״¹״§״¡ ״¯״§„״© runtime: sad_ll_cpu_get_info() -> i64 (…״₪״´״± „״¨†״© CPUInfo)
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
    // (AR) rdmsr ג€” ‚״±״§״¡״© ״³״¬„ †…ˆ״°״¬ …״­״¯״¯
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
    // (AR) wrmsr ג€” ƒ״×״§״¨״© ״³״¬„ †…ˆ״°״¬ …״­״¯״¯
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
    // Call runtime ג€” handles switch on CR number
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
    // (AR) invlpg ג€” ״¥״¨״·״§„ ״µ״­״©  TLB
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
// 15b. ˆ״­״¯״© GDT
// ============================================================================

llvm::Value* LLVMCodeGen::emitLowlevelGdtInit(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    return emitRuntimeCall(this, *builder_, module_.get(),
        "sad_ll_gdt_init", voidTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitLowlevelGdtLoad(std::shared_ptr<SIRInstruction> inst) {
    // (AR) lgdt ג€” ״×״­…„ ״¬״¯ˆ„ ״§„ˆ״§״µ״§״× ״§„״¹״§…״©
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
// 15c. ˆ״­״¯״© ״§„״×״±״­„ / Paging
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
    // (AR) mov cr3, cr3 ג€” ״¥״±״§״÷ ״°״§ƒ״±״© ״§„״×״±״¬…״© ״¨״§„ƒ״§…„
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
// 15d. ˆ״­״¯״© ״§„…‚״§״·״¹״§״× ״§„…״×‚״¯…״© / Advanced Interrupts (IDT)
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
// 15e. ˆ״­״¯״© PCI
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
// 15f. ˆ״­״¯״© DMA ״§„…״×‚״¯…״©
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
// 15g. ˆ״­״¯״© ״§„״´״§״´״© / Framebuffer
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
// 15h. ˆ״­״¯״© ACPI
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
// 15i. ˆ״­״¯״© ״§„״×״²״§…† / Sync
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
// 15j. ˆ״­״¯״© ״§„…״¬״¯ˆ„ / Scheduler
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
// 15k. ˆ״­״¯״© ״§„״¥‚„״§״¹ / Boot
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
// 15l. ˆ״­״¯״© †״¸״§… ״§„…„״§״× ״§„״§״×״±״§״¶ / VFS
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
// 15m. ˆ״­״¯״© APIC
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
// 15n. ˆ״­״¯״© HPET
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
// 15o. ˆ״­״¯״© ״§״³״×״¯״¹״§״¡״§״× ״§„†״¸״§… / Syscall
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
    // (AR) syscall ג€” ״×†״° ״§״³״×״¯״¹״§״¡ †״¸״§… ״¨״§„״±‚… ˆ״§„…״¹״§…„״§״×
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
// 15p. ״¹…„״§״× ״§„״°״§ƒ״±״© ״§„…״×‚״¯…״© / Advanced Memory
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
// ״§„‚״³… 16: ״¨״±ˆ״×ˆƒˆ„ UEFI ג€” ״×ˆ„״¯ LLVM IR
// Section 16: UEFI Boot Protocol ג€” LLVM IR Generation
// ============================================================================

} // namespace LLVM
} // namespace Sad

