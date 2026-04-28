/**
 * ==========================================================================
 * llvm_codegen_ui.cpp - ״×ˆ„״¯ LLVM IR „†״¸״§… ״§„ˆ״§״¬‡״© ״§„…ˆ״­״¯
 * LLVM IR Generation for Unified UI System (sad_ui.h)
 * ==========================================================================
 * 
 * ״§„ˆ״µ (״¹״±״¨):
 * --------------
 * ‡״°״§ ״§„…„ ˆ„‘״¯ LLVM IR „״§״³״×״¯״¹״§״¡ ״¯ˆ״§„ Runtime ˆ״§״¬‡״© ״§„…״³״×״®״¯… ״§„…ˆ״­״¯״©.
 * ״¹…„ ״¹„‰ ״¬…״¹ ״§„…†״µ״§״× (Android, Desktop, WASM) ג€” „״³ …״´״±ˆ״·״§‹ ״¨€ ENABLE_ANDROID_CODEGEN.
 * 
 * Description (English):
 * ----------------------
 * This file generates LLVM IR for calling unified UI runtime functions.
 * Works on all platforms (Android, Desktop, WASM) ג€” NOT conditional on ENABLE_ANDROID_CODEGEN.
 * 
 * ==========================================================================
 */

#include "llvm_codegen.h"
#include "builders/platform/ui_codegen.h"
#include <llvm/IR/DerivedTypes.h>

namespace Sad {
namespace LLVM {

using SIRInstruction = Compiler::SIR::SIRInstruction;
using SIROperand = Compiler::SIR::SIROperand;
using SIROpcode = Compiler::SIR::SIROpcode;

// ============================================================================
// ״¯״§„״© …״³״§״¹״¯״© „״§״³״×״¯״¹״§״¡ ״¯ˆ״§„ Runtime
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
    
    // (AR) ״¯ˆ״§„ void „״§ ״¬״¨ ״£† ״×״­…„ ״§״³…״§‹ „„†״×״¬״©
    // (EN) void functions should not have a result name
    if (retType->isVoidTy()) {
        return builder->CreateCall(fn, argValues);
    }
    return builder->CreateCall(fn, argValues, funcName + "_result");
}

// =====================================================================
// 21. †״¸״§… ״§„ˆ״§״¬‡״© ״§„…ˆ״­״¯ / Unified UI System (sad_ui.h)
// =====================================================================
// (AR) ƒ„ ״¯״§„״© …״µ†״¹ ״×״±״¬״¹ SadWidget* ג€” …״₪״´״± ״®״§… (opaque pointer)
// (EN) Each factory returns SadWidget* ג€” opaque pointer
// (AR) ƒ„ ״¯״§„״© ״¶״¨״· ״®״µ״§״¦״µ ״×״±״¬״¹ void
// (EN) Each property setter returns void

// ג”€ג”€ג”€ 21a. …״µ״§†״¹ ״§„״¹†״§״µ״± / Widget Factories ג”€ג”€ג”€

llvm::Value* UICodeGen::emitUiColumn(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* result = emitUIRuntimeCall(cg_, "sad_column", ptrTy, {}, {});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiRow(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* result = emitUIRuntimeCall(cg_, "sad_row", ptrTy, {}, {});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiStack(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* result = emitUIRuntimeCall(cg_, "sad_stack", ptrTy, {}, {});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiContainer(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* result = emitUIRuntimeCall(cg_, "sad_container", ptrTy, {}, {});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiText(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Value* text = inst->operands.empty() ?
        cg_.builder_->CreateGlobalStringPtr("", "empty_str") :
        cg_.resolveOperand(inst->operands[0]);
    auto* result = emitUIRuntimeCall(cg_, "sad_text", ptrTy, {ptrTy}, {text});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiTextStyled(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    // sad_text_styled(text, fontSize, r, g, b, a)
    llvm::Value* text = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* fontSize = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[1]), f32Ty);
    llvm::Value* r = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[2]), f32Ty);
    llvm::Value* g = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[3]), f32Ty);
    llvm::Value* b = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[4]), f32Ty);
    llvm::Value* a = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[5]), f32Ty);
    auto* result = emitUIRuntimeCall(cg_, "sad_text_styled", ptrTy,
        {ptrTy, f32Ty, f32Ty, f32Ty, f32Ty, f32Ty}, {text, fontSize, r, g, b, a});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiButton(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    // sad_button(label, onTap, userData) ג€” onTap and userData can be null
    llvm::Value* label = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* cb = inst->operands.size() > 1 ?
        cg_.resolveOperand(inst->operands[1]) :
        llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(*cg_.context_));
    llvm::Value* data = inst->operands.size() > 2 ?
        cg_.resolveOperand(inst->operands[2]) :
        llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(*cg_.context_));
    auto* result = emitUIRuntimeCall(cg_, "sad_button", ptrTy,
        {ptrTy, ptrTy, ptrTy}, {label, cb, data});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiButtonVariant(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    // sad_button_variant(label, variant, r, g, b, a, onTap, userData)
    llvm::Value* label = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* variant = cg_.builder_->CreateIntCast(cg_.resolveOperand(inst->operands[1]), i32Ty, false);
    llvm::Value* r = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[2]), f32Ty);
    llvm::Value* g = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[3]), f32Ty);
    llvm::Value* b = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[4]), f32Ty);
    llvm::Value* a = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[5]), f32Ty);
    llvm::Value* cb = inst->operands.size() > 6 ?
        cg_.resolveOperand(inst->operands[6]) :
        llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* data = inst->operands.size() > 7 ?
        cg_.resolveOperand(inst->operands[7]) :
        llvm::ConstantPointerNull::get(ptrTy);
    auto* result = emitUIRuntimeCall(cg_, "sad_button_variant", ptrTy,
        {ptrTy, i32Ty, f32Ty, f32Ty, f32Ty, f32Ty, ptrTy, ptrTy},
        {label, variant, r, g, b, a, cb, data});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiIconButton(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Value* icon = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* cb = inst->operands.size() > 1 ?
        cg_.resolveOperand(inst->operands[1]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* data = inst->operands.size() > 2 ?
        cg_.resolveOperand(inst->operands[2]) : llvm::ConstantPointerNull::get(ptrTy);
    auto* result = emitUIRuntimeCall(cg_, "sad_icon_button", ptrTy,
        {ptrTy, ptrTy, ptrTy}, {icon, cb, data});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiFab(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    llvm::Value* icon = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* r = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[1]), f32Ty);
    llvm::Value* g = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[2]), f32Ty);
    llvm::Value* b = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[3]), f32Ty);
    llvm::Value* a = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[4]), f32Ty);
    llvm::Value* cb = inst->operands.size() > 5 ?
        cg_.resolveOperand(inst->operands[5]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* data = inst->operands.size() > 6 ?
        cg_.resolveOperand(inst->operands[6]) : llvm::ConstantPointerNull::get(ptrTy);
    auto* result = emitUIRuntimeCall(cg_, "sad_fab", ptrTy,
        {ptrTy, f32Ty, f32Ty, f32Ty, f32Ty, ptrTy, ptrTy},
        {icon, r, g, b, a, cb, data});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiTextField(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Value* hint = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* cb = inst->operands.size() > 1 ?
        cg_.resolveOperand(inst->operands[1]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* data = inst->operands.size() > 2 ?
        cg_.resolveOperand(inst->operands[2]) : llvm::ConstantPointerNull::get(ptrTy);
    auto* result = emitUIRuntimeCall(cg_, "sad_text_field", ptrTy,
        {ptrTy, ptrTy, ptrTy}, {hint, cb, data});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiCheckbox(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Value* cb = !inst->operands.empty() ?
        cg_.resolveOperand(inst->operands[0]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* data = inst->operands.size() > 1 ?
        cg_.resolveOperand(inst->operands[1]) : llvm::ConstantPointerNull::get(ptrTy);
    auto* result = emitUIRuntimeCall(cg_, "sad_checkbox", ptrTy,
        {ptrTy, ptrTy}, {cb, data});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiSwitch(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Value* cb = !inst->operands.empty() ?
        cg_.resolveOperand(inst->operands[0]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* data = inst->operands.size() > 1 ?
        cg_.resolveOperand(inst->operands[1]) : llvm::ConstantPointerNull::get(ptrTy);
    auto* result = emitUIRuntimeCall(cg_, "sad_switch_toggle", ptrTy,
        {ptrTy, ptrTy}, {cb, data});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiSlider(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    llvm::Value* minVal = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[0]), f32Ty);
    llvm::Value* maxVal = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[1]), f32Ty);
    llvm::Value* cb = inst->operands.size() > 2 ?
        cg_.resolveOperand(inst->operands[2]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* data = inst->operands.size() > 3 ?
        cg_.resolveOperand(inst->operands[3]) : llvm::ConstantPointerNull::get(ptrTy);
    auto* result = emitUIRuntimeCall(cg_, "sad_slider", ptrTy,
        {f32Ty, f32Ty, ptrTy, ptrTy}, {minVal, maxVal, cb, data});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiCard(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* result = emitUIRuntimeCall(cg_, "sad_card", ptrTy, {}, {});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiScaffold(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* result = emitUIRuntimeCall(cg_, "sad_scaffold", ptrTy, {}, {});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiAppBar(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Value* title = inst->operands.empty() ?
        cg_.builder_->CreateGlobalStringPtr("", "empty_title") :
        cg_.resolveOperand(inst->operands[0]);
    auto* result = emitUIRuntimeCall(cg_, "sad_app_bar", ptrTy, {ptrTy}, {title});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiSpacer(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* result = emitUIRuntimeCall(cg_, "sad_spacer", ptrTy, {}, {});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiDivider(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* result = emitUIRuntimeCall(cg_, "sad_divider", ptrTy, {}, {});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiDialog(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Value* title = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* msg = inst->operands.size() > 1 ?
        cg_.resolveOperand(inst->operands[1]) :
        cg_.builder_->CreateGlobalStringPtr("", "empty_msg");
    auto* result = emitUIRuntimeCall(cg_, "sad_dialog", ptrTy,
        {ptrTy, ptrTy}, {title, msg});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

// ג”€ג”€ג”€ 21b. ״¥״¯״§״±״© ״§„״´״¬״±״© / Tree Management ג”€ג”€ג”€

llvm::Value* UICodeGen::emitUiAddChild(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* parent = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* child = cg_.resolveOperand(inst->operands[1]);
    return emitUIRuntimeCall(cg_, "sad_add_child", voidTy, {ptrTy, ptrTy}, {parent, child});
}

llvm::Value* UICodeGen::emitUiRemoveChild(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* parent = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* child = cg_.resolveOperand(inst->operands[1]);
    return emitUIRuntimeCall(cg_, "sad_remove_child", voidTy, {ptrTy, ptrTy}, {parent, child});
}

llvm::Value* UICodeGen::emitUiClearChildren(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    return emitUIRuntimeCall(cg_, "sad_clear_children", voidTy, {ptrTy}, {widget});
}

// ג”€ג”€ג”€ 21c. ״¶״¨״· ״§„״®״µ״§״¦״µ / Property Setters ג”€ג”€ג”€

llvm::Value* UICodeGen::emitUiSetText(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* text = cg_.resolveOperand(inst->operands[1]);
    return emitUIRuntimeCall(cg_, "sad_set_text", voidTy, {ptrTy, ptrTy}, {widget, text});
}

llvm::Value* UICodeGen::emitUiSetSize(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* w = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[1]), f32Ty);
    llvm::Value* h = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[2]), f32Ty);
    return emitUIRuntimeCall(cg_, "sad_set_size", voidTy, {ptrTy, f32Ty, f32Ty}, {widget, w, h});
}

llvm::Value* UICodeGen::emitUiSetFlex(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* flex = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[1]), f32Ty);
    return emitUIRuntimeCall(cg_, "sad_set_flex", voidTy, {ptrTy, f32Ty}, {widget, flex});
}

llvm::Value* UICodeGen::emitUiSetBackground(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* r = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[1]), f32Ty);
    llvm::Value* g = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[2]), f32Ty);
    llvm::Value* b = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[3]), f32Ty);
    llvm::Value* a = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[4]), f32Ty);
    return emitUIRuntimeCall(cg_, "sad_set_background", voidTy,
        {ptrTy, f32Ty, f32Ty, f32Ty, f32Ty}, {widget, r, g, b, a});
}

llvm::Value* UICodeGen::emitUiSetForeground(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* r = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[1]), f32Ty);
    llvm::Value* g = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[2]), f32Ty);
    llvm::Value* b = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[3]), f32Ty);
    llvm::Value* a = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[4]), f32Ty);
    return emitUIRuntimeCall(cg_, "sad_set_foreground", voidTy,
        {ptrTy, f32Ty, f32Ty, f32Ty, f32Ty}, {widget, r, g, b, a});
}

llvm::Value* UICodeGen::emitUiSetSpacing(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* spacing = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[1]), f32Ty);
    return emitUIRuntimeCall(cg_, "sad_set_spacing", voidTy, {ptrTy, f32Ty}, {widget, spacing});
}

llvm::Value* UICodeGen::emitUiSetPadding(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* top = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[1]), f32Ty);
    llvm::Value* right = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[2]), f32Ty);
    llvm::Value* bottom = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[3]), f32Ty);
    llvm::Value* left = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[4]), f32Ty);
    return emitUIRuntimeCall(cg_, "sad_set_padding", voidTy,
        {ptrTy, f32Ty, f32Ty, f32Ty, f32Ty}, {widget, top, right, bottom, left});
}

llvm::Value* UICodeGen::emitUiSetAlignment(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* mainAxis = cg_.builder_->CreateIntCast(cg_.resolveOperand(inst->operands[1]), i32Ty, false);
    llvm::Value* crossAxis = cg_.builder_->CreateIntCast(cg_.resolveOperand(inst->operands[2]), i32Ty, false);
    return emitUIRuntimeCall(cg_, "sad_set_alignment", voidTy,
        {ptrTy, i32Ty, i32Ty}, {widget, mainAxis, crossAxis});
}

llvm::Value* UICodeGen::emitUiSetBorder(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* width = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[1]), f32Ty);
    // sad_set_border(widget, width) ג€” optional color via separate call
    return emitUIRuntimeCall(cg_, "sad_set_border", voidTy, {ptrTy, f32Ty}, {widget, width});
}

llvm::Value* UICodeGen::emitUiSetElevation(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* elev = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[1]), f32Ty);
    return emitUIRuntimeCall(cg_, "sad_set_elevation", voidTy, {ptrTy, f32Ty}, {widget, elev});
}

llvm::Value* UICodeGen::emitUiSetOpacity(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* opacity = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[1]), f32Ty);
    return emitUIRuntimeCall(cg_, "sad_set_opacity", voidTy, {ptrTy, f32Ty}, {widget, opacity});
}

llvm::Value* UICodeGen::emitUiSetVisibility(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* visible = cg_.builder_->CreateIntCast(cg_.resolveOperand(inst->operands[1]), i32Ty, false);
    return emitUIRuntimeCall(cg_, "sad_set_visibility", voidTy, {ptrTy, i32Ty}, {widget, visible});
}

// ג”€ג”€ג”€ 21d. ״¥״¯״§״±״© ״§„״×״·״¨‚ / App Management ג”€ג”€ג”€

llvm::Value* UICodeGen::emitUiAppCreate(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* result = emitUIRuntimeCall(cg_, "sad_app_create", ptrTy, {}, {});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiAppSetRoot(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* app = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[1]);
    return emitUIRuntimeCall(cg_, "sad_app_set_root", voidTy, {ptrTy, ptrTy}, {app, widget});
}

llvm::Value* UICodeGen::emitUiAppLayout(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* app = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* w = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[1]), f32Ty);
    llvm::Value* h = cg_.builder_->CreateFPCast(cg_.resolveOperand(inst->operands[2]), f32Ty);
    return emitUIRuntimeCall(cg_, "sad_app_layout", voidTy, {ptrTy, f32Ty, f32Ty}, {app, w, h});
}

llvm::Value* UICodeGen::emitUiAppRender(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* app = cg_.resolveOperand(inst->operands[0]);
    return emitUIRuntimeCall(cg_, "sad_app_render", voidTy, {ptrTy}, {app});
}

llvm::Value* UICodeGen::emitUiAppDestroy(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* app = cg_.resolveOperand(inst->operands[0]);
    return emitUIRuntimeCall(cg_, "sad_app_destroy", voidTy, {ptrTy}, {app});
}

llvm::Value* UICodeGen::emitUiWidgetDestroy(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    return emitUIRuntimeCall(cg_, "sad_widget_destroy", voidTy, {ptrTy}, {widget});
}

} // namespace LLVM
} // namespace Sad
