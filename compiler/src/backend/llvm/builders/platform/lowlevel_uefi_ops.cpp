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
#include "builders/platform/lowlevel_codegen.h"
#include "sad_dyn_repr.h" // (AR) جدولُ التحويلِ الوحيد: coerceToParamType
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
    // (AR) [عقد وقت التشغيل الحرّ — نظيرُ lowlevel_ops] لاءِمْ كلَّ وسيطٍ لنوعِ
    //      معاملِه المصرَّح؛ التوثيقُ والقياسُ هناك (تعارضُ i64/i32 يُسقط verifyModule،
    //      و%SadDyn يُفَكُّ بوسمِه). جدولُ التحويلِ واحدٌ: `coerceToParamType`.
    // (EN) [Freestanding runtime contract — mirror of lowlevel_ops] coerce each
    //      argument to its declared parameter type through the single cast table.
    std::vector<llvm::Value*> coerced;
    coerced.reserve(argValues.size());
    llvm::FunctionType* ft = fn->getFunctionType();
    for (size_t i = 0; i < argValues.size(); ++i) {
        llvm::Value* v = argValues[i];
        // (AR) الملاءمةُ **ضيّقةٌ عمدًا**: توسيعُ/قصُّ عرضٍ بين عددَين، أو فكُّ قيمةٍ
        //      موسومةٍ %SadDyn إلى نوعِ المعامل. وما عدا ذلك يُترَك كما هو ليردَّه
        //      المُصادِق: ملاءمةٌ واسعةٌ كانت تحوّلُ عددًا إلى مؤشّرٍ بـinttoptr
        //      فتقبلُ `uefi_قراءة_متغير(5)` صامتةً وتسلّمُ النواةَ مؤشّرًا برّيًّا
        //      قيمتُه 5 — بناءٌ أخضرُ مكانَ خطأِ توقيعٍ كان يُسقطه verifyModule.
        // (EN) Deliberately narrow: integer width adjustment, or unboxing a tagged
        //      %SadDyn. Anything else is left for the verifier to reject — a wide
        //      coercion silently turned an integer into a pointer (inttoptr).
        const bool coercible =
            v && i < ft->getNumParams() &&
            (isSadDyn(v) ||
             (v->getType()->isIntegerTy() && ft->getParamType(i)->isIntegerTy()));
        coerced.push_back(coercible ? coerceToParamType(*cg, v, ft->getParamType(i)) : v);
    }
    return builder.CreateCall(fn, coerced);
}
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

llvm::Value* LowlevelCodeGen::emitLowlevelUefiInit(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types = {i64Ty, i64Ty};
    std::vector<llvm::Value*> vals;
    if (inst->operands.size() >= 2) {
        vals.push_back(cg_.resolveOperand(inst->operands[0]));
        vals.push_back(cg_.resolveOperand(inst->operands[1]));
    } else {
        vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
        vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    }
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_init", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiExitBootServices(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(cg_.resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_exit_boot_services", i64Ty, {i64Ty}, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiIsInitialized(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_is_initialized", i32Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiBsExited(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_bs_exited", i32Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiResetSystem(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(cg_.resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::ConstantInt::get(i32Ty, 0));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_reset_system", voidTy, {i32Ty}, vals);
}

// --- 16b. إدارة الذاكرة / Memory Services ---

llvm::Value* LowlevelCodeGen::emitLowlevelUefiAllocPages(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    std::vector<llvm::Type*> types = {i32Ty, i32Ty, i64Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(3)); i++)
        vals.push_back(cg_.resolveOperand(inst->operands[i]));
    while (vals.size() < 3) vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_alloc_pages", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiFreePages(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types = {i64Ty, i64Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(2)); i++)
        vals.push_back(cg_.resolveOperand(inst->operands[i]));
    while (vals.size() < 2) vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_free_pages", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiAllocPool(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    std::vector<llvm::Type*> types = {i32Ty, i64Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(2)); i++)
        vals.push_back(cg_.resolveOperand(inst->operands[i]));
    while (vals.size() < 2) vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_alloc_pool", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiFreePool(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(cg_.resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_free_pool", i64Ty, {i64Ty}, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiGetMemoryMap(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_get_memory_map", i8PtrTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiGetMemmapKey(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_get_memmap_key", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiTotalMemory(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_total_memory", i64Ty, {}, {});
}

// --- 16c. بروتوكول الرسوميات GOP / Graphics Output Protocol ---

llvm::Value* LowlevelCodeGen::emitLowlevelUefiInitGop(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_init_gop", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiSetGopMode(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(cg_.resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::ConstantInt::get(i32Ty, 0));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_set_gop_mode", i64Ty, {i32Ty}, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiQueryGopMode(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(cg_.resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::ConstantInt::get(i32Ty, 0));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_query_gop_mode", i8PtrTy, {i32Ty}, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiGopModeCount(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_gop_mode_count", i32Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiCurrentGopMode(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_current_gop_mode", i32Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiFramebufferBase(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_framebuffer_base", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiFramebufferSize(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_framebuffer_size", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiFillScreen(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    std::vector<llvm::Type*> types = {i32Ty, i32Ty, i32Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(3)); i++)
        vals.push_back(cg_.resolveOperand(inst->operands[i]));
    while (vals.size() < 3) vals.push_back(llvm::ConstantInt::get(i32Ty, 0));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_fill_screen", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiDrawRect(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    std::vector<llvm::Type*> types = {i32Ty, i32Ty, i32Ty, i32Ty, i32Ty, i32Ty, i32Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(7)); i++)
        vals.push_back(cg_.resolveOperand(inst->operands[i]));
    while (vals.size() < 7) vals.push_back(llvm::ConstantInt::get(i32Ty, 0));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_draw_rect", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiGopBlt(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    // buffer, op, srcX, srcY, dstX, dstY, width, height
    std::vector<llvm::Type*> types = {i8PtrTy, i32Ty, i32Ty, i32Ty, i32Ty, i32Ty, i32Ty, i32Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(8)); i++)
        vals.push_back(cg_.resolveOperand(inst->operands[i]));
    while (vals.size() < 8) vals.push_back(llvm::ConstantInt::get(i32Ty, 0));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_gop_blt", i64Ty, types, vals);
}

// --- 16d. خدمات وقت التشغيل / Runtime Services ---

llvm::Value* LowlevelCodeGen::emitLowlevelUefiGetTime(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_get_time", i8PtrTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiSetTime(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types;
    std::vector<llvm::Value*> vals;
    for (auto& op : inst->operands) {
        types.push_back(llvm::Type::getInt32Ty(*cg_.context_));
        vals.push_back(cg_.resolveOperand(op));
    }
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_set_time", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiGetVariable(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(cg_.resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::Constant::getNullValue(i8PtrTy));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_get_variable", i8PtrTy, {i8PtrTy}, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiSetVariable(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    std::vector<llvm::Type*> types = {i8PtrTy, i8PtrTy, i32Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(3)); i++)
        vals.push_back(cg_.resolveOperand(inst->operands[i]));
    while (vals.size() < 3) vals.push_back(llvm::ConstantInt::get(i32Ty, 7));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_set_variable", i64Ty, types, vals);
}

// --- 16e. نظام الملفات / File System ---

llvm::Value* LowlevelCodeGen::emitLowlevelUefiOpenVolume(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_open_volume", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiOpenFile(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    std::vector<llvm::Type*> types = {i64Ty, i8PtrTy, i64Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(3)); i++)
        vals.push_back(cg_.resolveOperand(inst->operands[i]));
    while (vals.size() < 3) vals.push_back(llvm::ConstantInt::get(i64Ty, 1));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_open_file", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiReadFile(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Type*> types = {i64Ty, i64Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(2)); i++)
        vals.push_back(cg_.resolveOperand(inst->operands[i]));
    while (vals.size() < 2) vals.push_back(llvm::ConstantInt::get(i64Ty, 4096));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_read_file", i8PtrTy, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiWriteFile(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    std::vector<llvm::Type*> types = {i64Ty, i8PtrTy, i64Ty};
    std::vector<llvm::Value*> vals;
    for (size_t i = 0; i < std::min(inst->operands.size(), size_t(3)); i++)
        vals.push_back(cg_.resolveOperand(inst->operands[i]));
    while (vals.size() < 3) vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_write_file", i64Ty, types, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiCloseFile(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(cg_.resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_close_file", i64Ty, {i64Ty}, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiFileInfo(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(cg_.resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::ConstantInt::get(i64Ty, 0));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_file_info", i8PtrTy, {i64Ty}, vals);
}

// --- 16f. بروتوكولات ومعلومات / Protocols & System Info ---

llvm::Value* LowlevelCodeGen::emitLowlevelUefiLocateProtocol(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    std::vector<llvm::Value*> vals;
    if (!inst->operands.empty()) vals.push_back(cg_.resolveOperand(inst->operands[0]));
    else vals.push_back(llvm::Constant::getNullValue(i8PtrTy));
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_locate_protocol", i64Ty, {i8PtrTy}, vals);
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiRevision(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_revision", i8PtrTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiVendor(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_vendor", i8PtrTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiFwRevision(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_fw_revision", i32Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelUefiReport(std::shared_ptr<SIRInstruction> inst) {
    auto* i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_uefi_report", i8PtrTy, {}, {});
}

// ============================================================================
// القسم 17: ACPI الموسّع / Extended ACPI
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiInitFull(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_init_full", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiInitRsdp(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* addr = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_init_rsdp", voidTy, {i64Ty}, {addr});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiEnable(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_enable", i32Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiDisable(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_disable", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiIsInitialized(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_is_initialized", i32Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiVersion(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_version", i32Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiReboot(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_reboot", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiSleep(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* state = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_sleep", i32Ty, {i32Ty}, {state});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiDelayUs(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* us = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_delay_us", voidTy, {i32Ty}, {us});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiReadPmTimer(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_read_pm_timer", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiIsPm32bit(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_is_pm_32bit", i32Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiProcessorCount(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_processor_count", i32Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiLocalApicAddr(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_local_apic_addr", i64Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelAcpiEcamBase(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* seg = inst->operands.size() > 0 ? cg_.resolveOperand(inst->operands[0]) : llvm::ConstantInt::get(i32Ty, 0);
    auto* bus = inst->operands.size() > 1 ? cg_.resolveOperand(inst->operands[1]) : llvm::ConstantInt::get(i32Ty, 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_acpi_ecam_base", i64Ty, {i32Ty, i32Ty}, {seg, bus});
}

// ============================================================================
// القسم 18: APIC الموسّع / Extended APIC
// ============================================================================

llvm::Value* LowlevelCodeGen::emitLowlevelApicSupported(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_supported", i32Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicX2Supported(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_x2_supported", i32Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicId(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_id", i32Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicIoCount(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_io_count", i32Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicInitTimer(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* vec = inst->operands.size() > 0 ? cg_.resolveOperand(inst->operands[0]) : llvm::ConstantInt::get(i32Ty, 32);
    auto* mode = inst->operands.size() > 1 ? cg_.resolveOperand(inst->operands[1]) : llvm::ConstantInt::get(i32Ty, 0);
    auto* div = inst->operands.size() > 2 ? cg_.resolveOperand(inst->operands[2]) : llvm::ConstantInt::get(i32Ty, 3);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_init_timer", voidTy, {i32Ty, i32Ty, i32Ty}, {vec, mode, div});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicStartTimer(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* count = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_start_timer", voidTy, {i32Ty}, {count});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicStopTimer(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_stop_timer", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicTimerCount(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_timer_count", i32Ty, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicCalibrate(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* hz = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_calibrate", i32Ty, {i32Ty}, {hz});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicSetPriority(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* prio = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_set_priority", voidTy, {i32Ty}, {prio});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicDisablePic(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_disable_pic", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicMaskIrq(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* irq = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_mask_irq", voidTy, {i32Ty}, {irq});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicUnmaskIrq(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* irq = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_unmask_irq", voidTy, {i32Ty}, {irq});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicRouteIrq(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* irq = cg_.resolveOperand(inst->operands[0]);
    auto* vec = cg_.resolveOperand(inst->operands[1]);
    auto* dest = cg_.resolveOperand(inst->operands[2]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_route_irq", voidTy, {i32Ty, i32Ty, i32Ty}, {irq, vec, dest});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicSendIpiAll(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* vec = cg_.resolveOperand(inst->operands[0]);
    auto* self = inst->operands.size() > 1 ? cg_.resolveOperand(inst->operands[1]) : llvm::ConstantInt::get(i32Ty, 0);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_send_ipi_all", voidTy, {i32Ty, i32Ty}, {vec, self});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicSendInitIpi(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* dest = cg_.resolveOperand(inst->operands[0]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_send_init_ipi", voidTy, {i32Ty}, {dest});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicSendSipi(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* dest = cg_.resolveOperand(inst->operands[0]);
    auto* page = cg_.resolveOperand(inst->operands[1]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_send_sipi", voidTy, {i32Ty, i32Ty}, {dest, page});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicWaitDelivery(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_wait_delivery", voidTy, {}, {});
}

llvm::Value* LowlevelCodeGen::emitLowlevelApicInitIo(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* id = cg_.resolveOperand(inst->operands[0]);
    auto* base = cg_.resolveOperand(inst->operands[1]);
    auto* gsib = cg_.resolveOperand(inst->operands[2]);
    return emitRuntimeCall(&cg_, *cg_.builder_, cg_.module_.get(),
        "sad_ll_apic_init_io", i32Ty, {i32Ty, i64Ty, i32Ty}, {id, base, gsib});
}


} // namespace LLVM
} // namespace Sad
