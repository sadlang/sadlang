// ============================================================================
// llvm_codegen_lowlevel_uefi.cpp — توليد LLVM IR لعمليات UEFI و ACPI/APIC المتقدمة
// LLVM IR Generation for UEFI, Extended ACPI & APIC Operations
// ============================================================================
// (AR) هذا الملف يحتوي على:
//      - عمليات UEFI (تهيئة، ذاكرة، رسوميات، ملفات، متغيرات)
//      - عمليات ACPI المتقدمة (تأخير، مؤقتات، معالجات)
//      - عمليات APIC المتقدمة (مؤقتات، IPI، IRQ routing)
// (EN) This file contains UEFI ops, extended ACPI ops, and extended APIC ops.
// تم استخراج هذا الملف من llvm_codegen_lowlevel.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include "llvm_codegen.h"
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Intrinsics.h>

namespace Sad {
namespace LLVM {

using SIRInstruction = Compiler::SIR::SIRInstruction;
using SIROperand = Compiler::SIR::SIROperand;
using SIROpcode = Compiler::SIR::SIROpcode;

// Helper: emit a call to a freestanding runtime C function (same as in lowlevel.cpp)
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
//
// (AR) ״¬…״¹ ״¹…„״§״× UEFI ״×״×״±״¬… ״¥„‰ ״§״³״×״¯״¹״§״¡״§״× ״¯ˆ״§„ runtime ״®״§״±״¬״©
//      ״¨״§„״¨״§״¯״¦״© sad_ll_uefi_* ˆ״§„״× ״×… ״±״¨״·‡״§  ˆ‚״× ״§„״±״¨״· ״§„†‡״§״¦.
//      ״¹„‰ ״¨״¦״© UEFI ״§„״­‚‚״©״ ‡״°‡ ״§„״¯ˆ״§„ ״×״³״×״¯״¹ ״®״¯…״§״× UEFI …״¨״§״´״±״©.
//      ״¹„‰ ״¨״¦״© ״§„…״­״§ƒ״§״©״ ״×… ״±״¨״·‡״§ ״¨״×†״° ˆ‡….
//
// (EN) All UEFI operations are translated to external runtime function calls
//      prefixed with sad_ll_uefi_* which are linked at final link time.
//      On real UEFI environment, these call UEFI services directly.
//      On simulation, they link to a stub implementation.
// ============================================================================

// --- 16a. ״§„״×‡״¦״© ˆ״§„״×״­ƒ… / Initialization & Control ---

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

// --- 16b. ״¥״¯״§״±״© ״§„״°״§ƒ״±״© / Memory Services ---

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

// --- 16c. ״¨״±ˆ״×ˆƒˆ„ ״§„״±״³ˆ…״§״× GOP / Graphics Output Protocol ---

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

// --- 16d. ״®״¯…״§״× ˆ‚״× ״§„״×״´״÷„ / Runtime Services ---

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

// --- 16e. †״¸״§… ״§„…„״§״× / File System ---

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

// --- 16f. ״¨״±ˆ״×ˆƒˆ„״§״× ˆ…״¹„ˆ…״§״× / Protocols & System Info ---

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
// ״§„‚״³… 17: ACPI ״§„…ˆ״³‘״¹ / Extended ACPI
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
// ״§„‚״³… 18: APIC ״§„…ˆ״³‘״¹ / Extended APIC
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
