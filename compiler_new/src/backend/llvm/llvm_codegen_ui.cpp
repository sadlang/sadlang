/**
 * ==========================================================================
 * llvm_codegen_ui.cpp - توليد LLVM IR لنظام الواجهة الموحد
 * LLVM IR Generation for Unified UI System (sad_ui.h)
 * ==========================================================================
 * 
 * الوصف (عربي):
 * --------------
 * هذا الملف يُولّد LLVM IR لاستدعاء دوال Runtime واجهة المستخدم الموحدة.
 * يعمل على جميع المنصات (Android, Desktop, WASM) — ليس مشروطاً بـ ENABLE_ANDROID_CODEGEN.
 * 
 * Description (English):
 * ----------------------
 * This file generates LLVM IR for calling unified UI runtime functions.
 * Works on all platforms (Android, Desktop, WASM) — NOT conditional on ENABLE_ANDROID_CODEGEN.
 * 
 * ==========================================================================
 */

#include "llvm_codegen.h"
#include <llvm/IR/DerivedTypes.h>

namespace Sad {
namespace LLVM {

using SIRInstruction = Compiler::SIR::SIRInstruction;
using SIROperand = Compiler::SIR::SIROperand;
using SIROpcode = Compiler::SIR::SIROpcode;

// ============================================================================
// دالة مساعدة لاستدعاء دوال Runtime
// Helper to emit runtime function calls (declaration only - implemented elsewhere)
// ============================================================================

// Forward declaration - emitRuntimeCall exists in llvm_codegen_android.cpp or llvm_codegen.cpp
// We use a local helper to avoid dependencies

static llvm::Value* emitUIRuntimeCall(
    LLVMCodeGen& cg,
    const std::string& funcName,
    llvm::Type* retType,
    const std::vector<llvm::Type*>& argTypes,
    const std::vector<llvm::Value*>& argValues)
{
    // Get module and builder from cg
    llvm::Module* module = cg.getModule();
    llvm::IRBuilder<>* builder = cg.getBuilder();
    llvm::LLVMContext* context = cg.getContext();
    
    // Find or create function
    llvm::Function* fn = module->getFunction(funcName);
    if (!fn) {
        llvm::FunctionType* ft = llvm::FunctionType::get(retType, argTypes, false);
        fn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, funcName, module);
        fn->addFnAttr(llvm::Attribute::NoUnwind);
    }
    
    // (AR) دوال void لا يجب أن تحمل اسماً للنتيجة
    // (EN) void functions should not have a result name
    if (retType->isVoidTy()) {
        return builder->CreateCall(fn, argValues);
    }
    return builder->CreateCall(fn, argValues, funcName + "_result");
}

// =====================================================================
// 21. نظام الواجهة الموحد / Unified UI System (sad_ui.h)
// =====================================================================
// (AR) كل دالة مصنع تُرجع SadWidget* — مؤشر خام (opaque pointer)
// (EN) Each factory returns SadWidget* — opaque pointer
// (AR) كل دالة ضبط خصائص ترجع void
// (EN) Each property setter returns void

// ─── 21a. مصانع العناصر / Widget Factories ───

llvm::Value* LLVMCodeGen::emitUiColumn(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* result = emitUIRuntimeCall(*this, "sad_column", ptrTy, {}, {});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiRow(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* result = emitUIRuntimeCall(*this, "sad_row", ptrTy, {}, {});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiStack(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* result = emitUIRuntimeCall(*this, "sad_stack", ptrTy, {}, {});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiContainer(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* result = emitUIRuntimeCall(*this, "sad_container", ptrTy, {}, {});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiText(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::Value* text = inst->operands.empty() ?
        builder_->CreateGlobalStringPtr("", "empty_str") :
        resolveOperand(inst->operands[0]);
    auto* result = emitUIRuntimeCall(*this, "sad_text", ptrTy, {ptrTy}, {text});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiTextStyled(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* f32Ty = llvm::Type::getFloatTy(*context_);
    // sad_text_styled(text, fontSize, r, g, b, a)
    llvm::Value* text = resolveOperand(inst->operands[0]);
    llvm::Value* fontSize = builder_->CreateFPCast(resolveOperand(inst->operands[1]), f32Ty);
    llvm::Value* r = builder_->CreateFPCast(resolveOperand(inst->operands[2]), f32Ty);
    llvm::Value* g = builder_->CreateFPCast(resolveOperand(inst->operands[3]), f32Ty);
    llvm::Value* b = builder_->CreateFPCast(resolveOperand(inst->operands[4]), f32Ty);
    llvm::Value* a = builder_->CreateFPCast(resolveOperand(inst->operands[5]), f32Ty);
    auto* result = emitUIRuntimeCall(*this, "sad_text_styled", ptrTy,
        {ptrTy, f32Ty, f32Ty, f32Ty, f32Ty, f32Ty}, {text, fontSize, r, g, b, a});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiButton(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    // sad_button(label, onTap, userData) — onTap and userData can be null
    llvm::Value* label = resolveOperand(inst->operands[0]);
    llvm::Value* cb = inst->operands.size() > 1 ?
        resolveOperand(inst->operands[1]) :
        llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(*context_));
    llvm::Value* data = inst->operands.size() > 2 ?
        resolveOperand(inst->operands[2]) :
        llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(*context_));
    auto* result = emitUIRuntimeCall(*this, "sad_button", ptrTy,
        {ptrTy, ptrTy, ptrTy}, {label, cb, data});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiButtonVariant(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* f32Ty = llvm::Type::getFloatTy(*context_);
    // sad_button_variant(label, variant, r, g, b, a, onTap, userData)
    llvm::Value* label = resolveOperand(inst->operands[0]);
    llvm::Value* variant = builder_->CreateIntCast(resolveOperand(inst->operands[1]), i32Ty, false);
    llvm::Value* r = builder_->CreateFPCast(resolveOperand(inst->operands[2]), f32Ty);
    llvm::Value* g = builder_->CreateFPCast(resolveOperand(inst->operands[3]), f32Ty);
    llvm::Value* b = builder_->CreateFPCast(resolveOperand(inst->operands[4]), f32Ty);
    llvm::Value* a = builder_->CreateFPCast(resolveOperand(inst->operands[5]), f32Ty);
    llvm::Value* cb = inst->operands.size() > 6 ?
        resolveOperand(inst->operands[6]) :
        llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* data = inst->operands.size() > 7 ?
        resolveOperand(inst->operands[7]) :
        llvm::ConstantPointerNull::get(ptrTy);
    auto* result = emitUIRuntimeCall(*this, "sad_button_variant", ptrTy,
        {ptrTy, i32Ty, f32Ty, f32Ty, f32Ty, f32Ty, ptrTy, ptrTy},
        {label, variant, r, g, b, a, cb, data});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiIconButton(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::Value* icon = resolveOperand(inst->operands[0]);
    llvm::Value* cb = inst->operands.size() > 1 ?
        resolveOperand(inst->operands[1]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* data = inst->operands.size() > 2 ?
        resolveOperand(inst->operands[2]) : llvm::ConstantPointerNull::get(ptrTy);
    auto* result = emitUIRuntimeCall(*this, "sad_icon_button", ptrTy,
        {ptrTy, ptrTy, ptrTy}, {icon, cb, data});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiFab(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* f32Ty = llvm::Type::getFloatTy(*context_);
    llvm::Value* icon = resolveOperand(inst->operands[0]);
    llvm::Value* r = builder_->CreateFPCast(resolveOperand(inst->operands[1]), f32Ty);
    llvm::Value* g = builder_->CreateFPCast(resolveOperand(inst->operands[2]), f32Ty);
    llvm::Value* b = builder_->CreateFPCast(resolveOperand(inst->operands[3]), f32Ty);
    llvm::Value* a = builder_->CreateFPCast(resolveOperand(inst->operands[4]), f32Ty);
    llvm::Value* cb = inst->operands.size() > 5 ?
        resolveOperand(inst->operands[5]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* data = inst->operands.size() > 6 ?
        resolveOperand(inst->operands[6]) : llvm::ConstantPointerNull::get(ptrTy);
    auto* result = emitUIRuntimeCall(*this, "sad_fab", ptrTy,
        {ptrTy, f32Ty, f32Ty, f32Ty, f32Ty, ptrTy, ptrTy},
        {icon, r, g, b, a, cb, data});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiTextField(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::Value* hint = resolveOperand(inst->operands[0]);
    llvm::Value* cb = inst->operands.size() > 1 ?
        resolveOperand(inst->operands[1]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* data = inst->operands.size() > 2 ?
        resolveOperand(inst->operands[2]) : llvm::ConstantPointerNull::get(ptrTy);
    auto* result = emitUIRuntimeCall(*this, "sad_text_field", ptrTy,
        {ptrTy, ptrTy, ptrTy}, {hint, cb, data});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiCheckbox(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::Value* cb = !inst->operands.empty() ?
        resolveOperand(inst->operands[0]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* data = inst->operands.size() > 1 ?
        resolveOperand(inst->operands[1]) : llvm::ConstantPointerNull::get(ptrTy);
    auto* result = emitUIRuntimeCall(*this, "sad_checkbox", ptrTy,
        {ptrTy, ptrTy}, {cb, data});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiSwitch(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::Value* cb = !inst->operands.empty() ?
        resolveOperand(inst->operands[0]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* data = inst->operands.size() > 1 ?
        resolveOperand(inst->operands[1]) : llvm::ConstantPointerNull::get(ptrTy);
    auto* result = emitUIRuntimeCall(*this, "sad_switch_toggle", ptrTy,
        {ptrTy, ptrTy}, {cb, data});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiSlider(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* f32Ty = llvm::Type::getFloatTy(*context_);
    llvm::Value* minVal = builder_->CreateFPCast(resolveOperand(inst->operands[0]), f32Ty);
    llvm::Value* maxVal = builder_->CreateFPCast(resolveOperand(inst->operands[1]), f32Ty);
    llvm::Value* cb = inst->operands.size() > 2 ?
        resolveOperand(inst->operands[2]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* data = inst->operands.size() > 3 ?
        resolveOperand(inst->operands[3]) : llvm::ConstantPointerNull::get(ptrTy);
    auto* result = emitUIRuntimeCall(*this, "sad_slider", ptrTy,
        {f32Ty, f32Ty, ptrTy, ptrTy}, {minVal, maxVal, cb, data});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiCard(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* result = emitUIRuntimeCall(*this, "sad_card", ptrTy, {}, {});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiScaffold(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* result = emitUIRuntimeCall(*this, "sad_scaffold", ptrTy, {}, {});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiAppBar(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::Value* title = inst->operands.empty() ?
        builder_->CreateGlobalStringPtr("", "empty_title") :
        resolveOperand(inst->operands[0]);
    auto* result = emitUIRuntimeCall(*this, "sad_app_bar", ptrTy, {ptrTy}, {title});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiSpacer(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* result = emitUIRuntimeCall(*this, "sad_spacer", ptrTy, {}, {});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiDivider(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* result = emitUIRuntimeCall(*this, "sad_divider", ptrTy, {}, {});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiDialog(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::Value* title = resolveOperand(inst->operands[0]);
    llvm::Value* msg = inst->operands.size() > 1 ?
        resolveOperand(inst->operands[1]) :
        builder_->CreateGlobalStringPtr("", "empty_msg");
    auto* result = emitUIRuntimeCall(*this, "sad_dialog", ptrTy,
        {ptrTy, ptrTy}, {title, msg});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

// ─── 21b. إدارة الشجرة / Tree Management ───

llvm::Value* LLVMCodeGen::emitUiAddChild(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* parent = resolveOperand(inst->operands[0]);
    llvm::Value* child = resolveOperand(inst->operands[1]);
    return emitUIRuntimeCall(*this, "sad_add_child", voidTy, {ptrTy, ptrTy}, {parent, child});
}

llvm::Value* LLVMCodeGen::emitUiRemoveChild(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* parent = resolveOperand(inst->operands[0]);
    llvm::Value* child = resolveOperand(inst->operands[1]);
    return emitUIRuntimeCall(*this, "sad_remove_child", voidTy, {ptrTy, ptrTy}, {parent, child});
}

llvm::Value* LLVMCodeGen::emitUiClearChildren(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    return emitUIRuntimeCall(*this, "sad_clear_children", voidTy, {ptrTy}, {widget});
}

// ─── 21c. ضبط الخصائص / Property Setters ───

llvm::Value* LLVMCodeGen::emitUiSetText(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    llvm::Value* text = resolveOperand(inst->operands[1]);
    return emitUIRuntimeCall(*this, "sad_set_text", voidTy, {ptrTy, ptrTy}, {widget, text});
}

llvm::Value* LLVMCodeGen::emitUiSetSize(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* f32Ty = llvm::Type::getFloatTy(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    llvm::Value* w = builder_->CreateFPCast(resolveOperand(inst->operands[1]), f32Ty);
    llvm::Value* h = builder_->CreateFPCast(resolveOperand(inst->operands[2]), f32Ty);
    return emitUIRuntimeCall(*this, "sad_set_size", voidTy, {ptrTy, f32Ty, f32Ty}, {widget, w, h});
}

llvm::Value* LLVMCodeGen::emitUiSetFlex(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* f32Ty = llvm::Type::getFloatTy(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    llvm::Value* flex = builder_->CreateFPCast(resolveOperand(inst->operands[1]), f32Ty);
    return emitUIRuntimeCall(*this, "sad_set_flex", voidTy, {ptrTy, f32Ty}, {widget, flex});
}

llvm::Value* LLVMCodeGen::emitUiSetBackground(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* f32Ty = llvm::Type::getFloatTy(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    llvm::Value* r = builder_->CreateFPCast(resolveOperand(inst->operands[1]), f32Ty);
    llvm::Value* g = builder_->CreateFPCast(resolveOperand(inst->operands[2]), f32Ty);
    llvm::Value* b = builder_->CreateFPCast(resolveOperand(inst->operands[3]), f32Ty);
    llvm::Value* a = builder_->CreateFPCast(resolveOperand(inst->operands[4]), f32Ty);
    return emitUIRuntimeCall(*this, "sad_set_background", voidTy,
        {ptrTy, f32Ty, f32Ty, f32Ty, f32Ty}, {widget, r, g, b, a});
}

llvm::Value* LLVMCodeGen::emitUiSetForeground(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* f32Ty = llvm::Type::getFloatTy(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    llvm::Value* r = builder_->CreateFPCast(resolveOperand(inst->operands[1]), f32Ty);
    llvm::Value* g = builder_->CreateFPCast(resolveOperand(inst->operands[2]), f32Ty);
    llvm::Value* b = builder_->CreateFPCast(resolveOperand(inst->operands[3]), f32Ty);
    llvm::Value* a = builder_->CreateFPCast(resolveOperand(inst->operands[4]), f32Ty);
    return emitUIRuntimeCall(*this, "sad_set_foreground", voidTy,
        {ptrTy, f32Ty, f32Ty, f32Ty, f32Ty}, {widget, r, g, b, a});
}

llvm::Value* LLVMCodeGen::emitUiSetSpacing(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* f32Ty = llvm::Type::getFloatTy(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    llvm::Value* spacing = builder_->CreateFPCast(resolveOperand(inst->operands[1]), f32Ty);
    return emitUIRuntimeCall(*this, "sad_set_spacing", voidTy, {ptrTy, f32Ty}, {widget, spacing});
}

llvm::Value* LLVMCodeGen::emitUiSetPadding(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* f32Ty = llvm::Type::getFloatTy(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    llvm::Value* top = builder_->CreateFPCast(resolveOperand(inst->operands[1]), f32Ty);
    llvm::Value* right = builder_->CreateFPCast(resolveOperand(inst->operands[2]), f32Ty);
    llvm::Value* bottom = builder_->CreateFPCast(resolveOperand(inst->operands[3]), f32Ty);
    llvm::Value* left = builder_->CreateFPCast(resolveOperand(inst->operands[4]), f32Ty);
    return emitUIRuntimeCall(*this, "sad_set_padding", voidTy,
        {ptrTy, f32Ty, f32Ty, f32Ty, f32Ty}, {widget, top, right, bottom, left});
}

llvm::Value* LLVMCodeGen::emitUiSetAlignment(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    llvm::Value* mainAxis = builder_->CreateIntCast(resolveOperand(inst->operands[1]), i32Ty, false);
    llvm::Value* crossAxis = builder_->CreateIntCast(resolveOperand(inst->operands[2]), i32Ty, false);
    return emitUIRuntimeCall(*this, "sad_set_alignment", voidTy,
        {ptrTy, i32Ty, i32Ty}, {widget, mainAxis, crossAxis});
}

llvm::Value* LLVMCodeGen::emitUiSetBorder(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* f32Ty = llvm::Type::getFloatTy(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    llvm::Value* width = builder_->CreateFPCast(resolveOperand(inst->operands[1]), f32Ty);
    // sad_set_border(widget, width) — optional color via separate call
    return emitUIRuntimeCall(*this, "sad_set_border", voidTy, {ptrTy, f32Ty}, {widget, width});
}

llvm::Value* LLVMCodeGen::emitUiSetElevation(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* f32Ty = llvm::Type::getFloatTy(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    llvm::Value* elev = builder_->CreateFPCast(resolveOperand(inst->operands[1]), f32Ty);
    return emitUIRuntimeCall(*this, "sad_set_elevation", voidTy, {ptrTy, f32Ty}, {widget, elev});
}

llvm::Value* LLVMCodeGen::emitUiSetOpacity(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* f32Ty = llvm::Type::getFloatTy(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    llvm::Value* opacity = builder_->CreateFPCast(resolveOperand(inst->operands[1]), f32Ty);
    return emitUIRuntimeCall(*this, "sad_set_opacity", voidTy, {ptrTy, f32Ty}, {widget, opacity});
}

llvm::Value* LLVMCodeGen::emitUiSetVisibility(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    llvm::Value* visible = builder_->CreateIntCast(resolveOperand(inst->operands[1]), i32Ty, false);
    return emitUIRuntimeCall(*this, "sad_set_visibility", voidTy, {ptrTy, i32Ty}, {widget, visible});
}

// ─── 21d. إدارة التطبيق / App Management ───

llvm::Value* LLVMCodeGen::emitUiAppCreate(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* result = emitUIRuntimeCall(*this, "sad_app_create", ptrTy, {}, {});
    if (inst->result) context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* LLVMCodeGen::emitUiAppSetRoot(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* app = resolveOperand(inst->operands[0]);
    llvm::Value* widget = resolveOperand(inst->operands[1]);
    return emitUIRuntimeCall(*this, "sad_app_set_root", voidTy, {ptrTy, ptrTy}, {app, widget});
}

llvm::Value* LLVMCodeGen::emitUiAppLayout(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* f32Ty = llvm::Type::getFloatTy(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* app = resolveOperand(inst->operands[0]);
    llvm::Value* w = builder_->CreateFPCast(resolveOperand(inst->operands[1]), f32Ty);
    llvm::Value* h = builder_->CreateFPCast(resolveOperand(inst->operands[2]), f32Ty);
    return emitUIRuntimeCall(*this, "sad_app_layout", voidTy, {ptrTy, f32Ty, f32Ty}, {app, w, h});
}

llvm::Value* LLVMCodeGen::emitUiAppRender(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* app = resolveOperand(inst->operands[0]);
    return emitUIRuntimeCall(*this, "sad_app_render", voidTy, {ptrTy}, {app});
}

llvm::Value* LLVMCodeGen::emitUiAppDestroy(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* app = resolveOperand(inst->operands[0]);
    return emitUIRuntimeCall(*this, "sad_app_destroy", voidTy, {ptrTy}, {app});
}

llvm::Value* LLVMCodeGen::emitUiWidgetDestroy(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    return emitUIRuntimeCall(*this, "sad_widget_destroy", voidTy, {ptrTy}, {widget});
}

} // namespace LLVM
} // namespace Sad
