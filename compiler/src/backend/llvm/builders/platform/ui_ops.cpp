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

// (AR) تصريح مُسبَق: حوّل معاملًا عدديًّا إلى float بأمان (التعريف أدناه). يُستعمَل
//      في مصانع/ضوابط عدديّة فوق سطر تعريفه، فيلزم التصريح المُسبَق.
// (EN) Forward declaration of the safe numeric→f32 cast (defined below);
//      used by numeric factories/setters above its definition.
static llvm::Value* castNumericToF32(LLVMCodeGen& cg, llvm::Value* v);

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
        llvm::ConstantPointerNull::get(ptrTy) :
        cg_.resolveOperand(inst->operands[0]);
    auto* result = emitUIRuntimeCall(cg_, "sad_text", ptrTy, {ptrTy}, {text});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiTextStyled(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    // sad_text_styled(text, fontSize, r, g, b, a) — حراسٌ ضدّ نقص الوسائط (منع انهيار)
    auto opF32 = [&](size_t i){ return inst->operands.size() > i ?
        castNumericToF32(cg_, cg_.resolveOperand(inst->operands[i])) : llvm::ConstantFP::get(f32Ty, 0.0f); };
    llvm::Value* text = !inst->operands.empty() ?
        cg_.resolveOperand(inst->operands[0]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* fontSize = opF32(1);
    llvm::Value* r = opF32(2);
    llvm::Value* g = opF32(3);
    llvm::Value* b = opF32(4);
    llvm::Value* a = opF32(5);
    auto* result = emitUIRuntimeCall(cg_, "sad_text_styled", ptrTy,
        {ptrTy, f32Ty, f32Ty, f32Ty, f32Ty, f32Ty}, {text, fontSize, r, g, b, a});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiButton(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    // sad_button(label, onTap, userData) — onTap and userData can be null
    // (AR) حارس: زر() بلا وسائط ⇒ operands فارغة (null ⇒ لا «عنوان»، يطابق المفسّر).
    llvm::Value* label = !inst->operands.empty() ?
        cg_.resolveOperand(inst->operands[0]) : llvm::ConstantPointerNull::get(ptrTy);
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
    // sad_button_variant(label, variant, r, g, b, a, onTap, userData) — حراسٌ ضدّ النقص
    auto opF32v = [&](size_t i){ return inst->operands.size() > i ?
        castNumericToF32(cg_, cg_.resolveOperand(inst->operands[i])) : llvm::ConstantFP::get(f32Ty, 0.0f); };
    llvm::Value* label = !inst->operands.empty() ?
        cg_.resolveOperand(inst->operands[0]) : llvm::ConstantPointerNull::get(ptrTy);
    // (AR) variant: تصليبٌ دفاعيّ (نظير emitUiSetPropInt) — CreateIntCast يفترض
    //      مصدرًا صحيحًا فينهار على عشريّ؛ نعالج الصحيح/العشريّ/غيرهما بأمان.
    llvm::Value* variant;
    if (inst->operands.size() > 1) {
        llvm::Value* rawV = cg_.resolveOperand(inst->operands[1]);
        variant = rawV->getType()->isIntegerTy()      ? cg_.builder_->CreateIntCast(rawV, i32Ty, false)
                : rawV->getType()->isFloatingPointTy() ? cg_.builder_->CreateFPToSI(rawV, i32Ty)
                                                       : llvm::ConstantInt::get(i32Ty, 0);
    } else {
        variant = llvm::ConstantInt::get(i32Ty, 0);
    }
    llvm::Value* r = opF32v(2);
    llvm::Value* g = opF32v(3);
    llvm::Value* b = opF32v(4);
    llvm::Value* a = opF32v(5);
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
    // (AR) حارس: زر_أيقونة() بلا وسائط ⇒ operands فارغة (منع انهيار).
    llvm::Value* icon = !inst->operands.empty() ?
        cg_.resolveOperand(inst->operands[0]) : llvm::ConstantPointerNull::get(ptrTy);
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
    // (AR) حارس: زر_عائم() / زر_عائم("x") (وسائط أقلّ من 5) ⇒ منع انهيار (كان
    //      يقرأ operands[1..4] للألوان). null/0 عند النقص.
    auto opF32f = [&](size_t i){ return inst->operands.size() > i ?
        castNumericToF32(cg_, cg_.resolveOperand(inst->operands[i])) : llvm::ConstantFP::get(f32Ty, 0.0f); };
    llvm::Value* icon = !inst->operands.empty() ?
        cg_.resolveOperand(inst->operands[0]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* r = opF32f(1);
    llvm::Value* g = opF32f(2);
    llvm::Value* b = opF32f(3);
    llvm::Value* a = opF32f(4);
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
    // (AR) حارس: حقل_نص() بلا وسائط ⇒ operands فارغة؛ نمرّر null (runtime لا يضبط
    //      «تلميح» عند null ⇒ يطابق المفسّر). بلا الحارس يقع وصولٌ خارج الحدود ⇒ انهيار.
    llvm::Value* hint = !inst->operands.empty() ?
        cg_.resolveOperand(inst->operands[0]) : llvm::ConstantPointerNull::get(ptrTy);
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
    // (AR) بعد مواءمة منزلق: الخافض يُصدر SLIDER **بلا operands** (عقدة فارغة) ثمّ
    //      SET_PROP(«قيمة») منفصلًا، فـminVal/maxVal هنا يقعان دائمًا على الافتراضيّ 0
    //      و sad_slider يُهملهما. الحارس يبقى دفاعيًّا (منع أيّ وصولٍ خارج الحدود).
    llvm::Value* minVal = !inst->operands.empty() ?
        castNumericToF32(cg_, cg_.resolveOperand(inst->operands[0])) : llvm::ConstantFP::get(f32Ty, 0.0f);
    llvm::Value* maxVal = inst->operands.size() > 1 ?
        castNumericToF32(cg_, cg_.resolveOperand(inst->operands[1])) : llvm::ConstantFP::get(f32Ty, 0.0f);
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
    // (AR) null (لا "") عند الغياب ⇒ sad_app_bar (if(title)) لا يضبط «عنوان»
    //      الفارغ ⇒ يطابق المفسّر (شريط_تطبيق() ⇒ بلا خصائص).
    llvm::Value* title = inst->operands.empty() ?
        llvm::ConstantPointerNull::get(ptrTy) :
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
    // (AR) حارس: حوار() بلا وسائط ⇒ منع انهيار. null (لا "") فيحرسه sad_dialog
    //      (if(title)) ⇒ لا يضبط «عنوان» ⇒ يطابق المفسّر (حوار() ⇒ بلا خصائص).
    llvm::Value* title = !inst->operands.empty() ?
        cg_.resolveOperand(inst->operands[0]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* msg = inst->operands.size() > 1 ?
        cg_.resolveOperand(inst->operands[1]) :
        llvm::ConstantPointerNull::get(ptrTy);
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
    llvm::Value* w = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[1]));
    llvm::Value* h = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[2]));
    return emitUIRuntimeCall(cg_, "sad_set_size", voidTy, {ptrTy, f32Ty, f32Ty}, {widget, w, h});
}

llvm::Value* UICodeGen::emitUiSetFlex(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* flex = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[1]));
    return emitUIRuntimeCall(cg_, "sad_set_flex", voidTy, {ptrTy, f32Ty}, {widget, flex});
}

llvm::Value* UICodeGen::emitUiSetBackground(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* r = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[1]));
    llvm::Value* g = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[2]));
    llvm::Value* b = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[3]));
    llvm::Value* a = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[4]));
    return emitUIRuntimeCall(cg_, "sad_set_background", voidTy,
        {ptrTy, f32Ty, f32Ty, f32Ty, f32Ty}, {widget, r, g, b, a});
}

llvm::Value* UICodeGen::emitUiSetForeground(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* r = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[1]));
    llvm::Value* g = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[2]));
    llvm::Value* b = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[3]));
    llvm::Value* a = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[4]));
    return emitUIRuntimeCall(cg_, "sad_set_foreground", voidTy,
        {ptrTy, f32Ty, f32Ty, f32Ty, f32Ty}, {widget, r, g, b, a});
}

llvm::Value* UICodeGen::emitUiSetSpacing(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* spacing = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[1]));
    return emitUIRuntimeCall(cg_, "sad_set_spacing", voidTy, {ptrTy, f32Ty}, {widget, spacing});
}

llvm::Value* UICodeGen::emitUiSetPadding(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* top = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[1]));
    llvm::Value* right = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[2]));
    llvm::Value* bottom = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[3]));
    llvm::Value* left = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[4]));
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
    llvm::Value* width = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[1]));
    // sad_set_border(widget, width) ג€” optional color via separate call
    return emitUIRuntimeCall(cg_, "sad_set_border", voidTy, {ptrTy, f32Ty}, {widget, width});
}

llvm::Value* UICodeGen::emitUiSetElevation(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* elev = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[1]));
    return emitUIRuntimeCall(cg_, "sad_set_elevation", voidTy, {ptrTy, f32Ty}, {widget, elev});
}

llvm::Value* UICodeGen::emitUiSetOpacity(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* opacity = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[1]));
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
    llvm::Value* w = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[1]));
    llvm::Value* h = castNumericToF32(cg_, cg_.resolveOperand(inst->operands[2]));
    return emitUIRuntimeCall(cg_, "sad_app_layout", voidTy, {ptrTy, f32Ty, f32Ty}, {app, w, h});
}

llvm::Value* UICodeGen::emitUiAppRender(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* app = cg_.resolveOperand(inst->operands[0]);
    return emitUIRuntimeCall(cg_, "sad_app_render", voidTy, {ptrTy}, {app});
}

// (AR) تشغيل_تطبيق(عنصر) ⇒ sad_app_run(root): حلقة سطح المكتب + الإرسال (جسر المكتبة).
//      نداءٌ void: لا يُخزَّن نتيجةٌ في namedValues (بخلاف مصانع العناصر التي تعيد مقبضًا).
llvm::Value* UICodeGen::emitUiAppRun(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* root = cg_.resolveOperand(inst->operands[0]);
    return emitUIRuntimeCall(cg_, "sad_app_run", voidTy, {ptrTy}, {root});
}

// (AR) طباعة_شجرة(عنصر) ⇒ sad_print_tree(root): تصحيح (جسر فوق منطق المكتبة المشترك).
//      نداءٌ void: لا نتيجةٌ تُخزَّن في namedValues (كـemitUiAppRun).
llvm::Value* UICodeGen::emitUiPrintTree(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* root = cg_.resolveOperand(inst->operands[0]);
    return emitUIRuntimeCall(cg_, "sad_print_tree", voidTy, {ptrTy}, {root});
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

// ─── 21e. مصانع إضافيّة (م-مصانع) / Extra factories ───
// (AR) تُكمل تغطية المترجم لتطابق المفسّر؛ كلٌّ يُصدِر نداء runtime يُنشئ العقدة.
// (EN) Complete compiler coverage to match the interpreter; each emits a runtime
//      call that creates the corresponding IR node.

// (AR) مساعد: حوّل معاملًا عدديًّا (صحيحًا أو عشريًّا) إلى float بأمان.
//      المعاملات الصحيحة (مثل مقاس(100,50)) تحتاج SIToFP لا FPCast (الأخير
//      يُولّد fptrunc على i64 = IR غير صالح ⇒ فشل اختيار التعليمات).
// (EN) Safely cast a numeric operand (int or float) to f32. Integer literals
//      (e.g. مقاس(100,50)) need SIToFP, not FPCast (which emits an illegal
//      fptrunc on i64 → instruction-selection failure).
static llvm::Value* castNumericToF32(LLVMCodeGen& cg, llvm::Value* v) {
    auto* f32Ty = llvm::Type::getFloatTy(*cg.getContext());
    if (v->getType()->isIntegerTy())
        return cg.getBuilder()->CreateSIToFP(v, f32Ty);
    return cg.getBuilder()->CreateFPCast(v, f32Ty);
}

// (AR) مساعد محلّيّ عضو: مصنع بسيط بلا معاملات يُعيد SadWidget*.
// (EN) Member helper: a no-arg factory returning SadWidget*.
llvm::Value* UICodeGen::emitSimpleUiFactory(
    std::shared_ptr<SIRInstruction> inst, const char* runtimeName)
{
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* result = emitUIRuntimeCall(cg_, runtimeName, ptrTy, {}, {});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiImage(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Value* arg = inst->operands.empty() ?
        llvm::ConstantPointerNull::get(ptrTy) :
        cg_.resolveOperand(inst->operands[0]);
    auto* result = emitUIRuntimeCall(cg_, "sad_image", ptrTy, {ptrTy}, {arg});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiIcon(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Value* arg = inst->operands.empty() ?
        llvm::ConstantPointerNull::get(ptrTy) :
        cg_.resolveOperand(inst->operands[0]);
    auto* result = emitUIRuntimeCall(cg_, "sad_icon", ptrTy, {ptrTy}, {arg});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiTextButton(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Value* arg = inst->operands.empty() ?
        llvm::ConstantPointerNull::get(ptrTy) :
        cg_.resolveOperand(inst->operands[0]);
    auto* result = emitUIRuntimeCall(cg_, "sad_text_button", ptrTy, {ptrTy}, {arg});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiSnackbar(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Value* arg = inst->operands.empty() ?
        llvm::ConstantPointerNull::get(ptrTy) :
        cg_.resolveOperand(inst->operands[0]);
    auto* result = emitUIRuntimeCall(cg_, "sad_snackbar", ptrTy, {ptrTy}, {arg});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiTooltip(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Value* arg = inst->operands.empty() ?
        llvm::ConstantPointerNull::get(ptrTy) :
        cg_.resolveOperand(inst->operands[0]);
    auto* result = emitUIRuntimeCall(cg_, "sad_tooltip", ptrTy, {ptrTy}, {arg});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiTextArea(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Value* arg = inst->operands.empty() ?
        llvm::ConstantPointerNull::get(ptrTy) :
        cg_.resolveOperand(inst->operands[0]);
    auto* result = emitUIRuntimeCall(cg_, "sad_text_area", ptrTy, {ptrTy}, {arg});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiProgress(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    llvm::Value* value = inst->operands.empty() ?
        llvm::ConstantFP::get(f32Ty, 0.0) :
        castNumericToF32(cg_, cg_.resolveOperand(inst->operands[0]));
    auto* result = emitUIRuntimeCall(cg_, "sad_progress", ptrTy, {f32Ty}, {value});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiSizedBox(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    llvm::Value* w = !inst->operands.empty() ?
        castNumericToF32(cg_, cg_.resolveOperand(inst->operands[0])) :
        llvm::ConstantFP::get(f32Ty, 0.0);
    llvm::Value* h = inst->operands.size() > 1 ?
        castNumericToF32(cg_, cg_.resolveOperand(inst->operands[1])) :
        llvm::ConstantFP::get(f32Ty, 0.0);
    auto* result = emitUIRuntimeCall(cg_, "sad_sized_box", ptrTy, {f32Ty, f32Ty}, {w, h});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiGrid(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_grid");
}
llvm::Value* UICodeGen::emitUiCenter(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_center");
}
llvm::Value* UICodeGen::emitUiPadding(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_padding");
}
llvm::Value* UICodeGen::emitUiAlign(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_align");
}
llvm::Value* UICodeGen::emitUiExpanded(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_expanded");
}
llvm::Value* UICodeGen::emitUiFlexible(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_flexible");
}
llvm::Value* UICodeGen::emitUiWrap(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_wrap");
}
llvm::Value* UICodeGen::emitUiBox(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_box");
}
llvm::Value* UICodeGen::emitUiScrollView(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_scroll_view");
}
llvm::Value* UICodeGen::emitUiBottomNav(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_bottom_nav");
}
llvm::Value* UICodeGen::emitUiLazyColumn(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_lazy_column");
}
llvm::Value* UICodeGen::emitUiLazyRow(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_lazy_row");
}
llvm::Value* UICodeGen::emitUiListView(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_list_view");
}
llvm::Value* UICodeGen::emitUiDrawer(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_drawer");
}
llvm::Value* UICodeGen::emitUiSafeArea(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_safe_area");
}
llvm::Value* UICodeGen::emitUiSurface(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_surface");
}

// ─── 21f. أثر المعدّلات (م-أ3ر، L1): خاصّيّة عامّة بالاسم / Generic named property ───
// operands: [0]=widget, [1]=اسم الخاصيّة (نصّ), [2]=القيمة.

llvm::Value* UICodeGen::emitUiSetPropStr(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* name = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* value = cg_.resolveOperand(inst->operands[2]);
    // (AR) دفاعيّ: المسار الافتراضيّ في الخفض قد يوجّه نوعًا غير نصّيّ (مقبض/كائن)
    //      إلى هنا؛ إن لم تكن القيمة مؤشّرًا نمرّر مؤشّرًا فارغًا بدل توليد نداء
    //      غير صالح يُسقِط مُتحقِّق LLVM. (emitUIRuntimeCall لا يُكيّف الأنواع.)
    if (!value->getType()->isPointerTy())
        value = llvm::ConstantPointerNull::get(ptrTy);
    return emitUIRuntimeCall(cg_, "sad_set_prop_str", voidTy,
        {ptrTy, ptrTy, ptrTy}, {widget, name, value});
}

llvm::Value* UICodeGen::emitUiSetPropInt(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* name = cg_.resolveOperand(inst->operands[1]);
    // (AR) قيمة صحيحة ⇒ i64 (تُخزَّن int64_t كما في المفسّر، لا double).
    //      حارس: إن وصل سجلٌّ عشريّ عبر مسارٍ غير معتاد، نحوّله بـFPToSI لا IntCast
    //      (IntCast يفترض مصدرًا صحيحًا فينهار).
    llvm::Value* rawI = cg_.resolveOperand(inst->operands[2]);
    llvm::Value* value = rawI->getType()->isIntegerTy()
        ? cg_.builder_->CreateIntCast(rawI, i64Ty, true)
        : cg_.builder_->CreateFPToSI(rawI, i64Ty);
    return emitUIRuntimeCall(cg_, "sad_set_prop_int", voidTy,
        {ptrTy, ptrTy, i64Ty}, {widget, name, value});
}

llvm::Value* UICodeGen::emitUiSetPropNum(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f64Ty = llvm::Type::getDoubleTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* name = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* raw = cg_.resolveOperand(inst->operands[2]);
    // (AR) القيمة قد تكون صحيحة (i64) أو عشريّة ⇒ تحويل آمن إلى double.
    llvm::Value* value = raw->getType()->isIntegerTy()
        ? cg_.builder_->CreateSIToFP(raw, f64Ty)
        : cg_.builder_->CreateFPCast(raw, f64Ty);
    return emitUIRuntimeCall(cg_, "sad_set_prop_num", voidTy,
        {ptrTy, ptrTy, f64Ty}, {widget, name, value});
}

// (AR) L2: ربط حدث عند_* — sad_add_event(widget, name, cb, data).
//      ردّ النداء وبياناته اختياريّان (يُستبدلان بمؤشّر فارغ عند الغياب)،
//      مثل onTap في مصنع الزرّ.
llvm::Value* UICodeGen::emitUiAddEvent(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* name = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* cb = inst->operands.size() > 2
        ? cg_.resolveOperand(inst->operands[2])
        : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* data = inst->operands.size() > 3
        ? cg_.resolveOperand(inst->operands[3])
        : llvm::ConstantPointerNull::get(ptrTy);
    // (AR) دفاعيّ: ردّ النداء/البيانات يجب أن يكونا مؤشّرين (emitUIRuntimeCall لا يُكيّف).
    if (!cb->getType()->isPointerTy())   cb = llvm::ConstantPointerNull::get(ptrTy);
    if (!data->getType()->isPointerTy()) data = llvm::ConstantPointerNull::get(ptrTy);
    return emitUIRuntimeCall(cg_, "sad_add_event", voidTy,
        {ptrTy, ptrTy, ptrTy, ptrTy}, {widget, name, cb, data});
}

// ─── سلسلة التحريك (م-أ3ر، L3) ───
// (AR) كلٌّ يعمل على المجموعة النشطة من تحريكات IRNode عبر حالة محفوظة على العنصر.
llvm::Value* UICodeGen::emitUiAnimBegin(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* csv = cg_.resolveOperand(inst->operands[1]);
    if (!csv->getType()->isPointerTy()) csv = llvm::ConstantPointerNull::get(ptrTy);
    return emitUIRuntimeCall(cg_, "sad_anim_begin", voidTy,
        {ptrTy, ptrTy}, {widget, csv});
}
llvm::Value* UICodeGen::emitUiAnimDuration(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f64Ty = llvm::Type::getDoubleTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* raw = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* sec = raw->getType()->isIntegerTy()
        ? cg_.builder_->CreateSIToFP(raw, f64Ty)
        : cg_.builder_->CreateFPCast(raw, f64Ty);
    return emitUIRuntimeCall(cg_, "sad_anim_duration", voidTy,
        {ptrTy, f64Ty}, {widget, sec});
}
llvm::Value* UICodeGen::emitUiAnimEasing(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* name = cg_.resolveOperand(inst->operands[1]);
    if (!name->getType()->isPointerTy()) name = llvm::ConstantPointerNull::get(ptrTy);
    return emitUIRuntimeCall(cg_, "sad_anim_easing", voidTy,
        {ptrTy, ptrTy}, {widget, name});
}
llvm::Value* UICodeGen::emitUiAnimDelay(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f64Ty = llvm::Type::getDoubleTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* raw = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* sec = raw->getType()->isIntegerTy()
        ? cg_.builder_->CreateSIToFP(raw, f64Ty)
        : cg_.builder_->CreateFPCast(raw, f64Ty);
    return emitUIRuntimeCall(cg_, "sad_anim_delay", voidTy,
        {ptrTy, f64Ty}, {widget, sec});
}
llvm::Value* UICodeGen::emitUiAnimRepeat(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    // (AR) قد يصل وسيطٌ عشريّ (.تكرار(2.5)) — IntCast يفترض مصدرًا صحيحًا فينهار؛
    //      نحوّل العشريّ بـFPToSI كما يتسامح المفسّر (toInt).
    llvm::Value* rawN = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* count = rawN->getType()->isIntegerTy()
        ? cg_.builder_->CreateIntCast(rawN, i32Ty, true)
        : cg_.builder_->CreateFPToSI(rawN, i32Ty);
    return emitUIRuntimeCall(cg_, "sad_anim_repeat", voidTy,
        {ptrTy, i32Ty}, {widget, count});
}
llvm::Value* UICodeGen::emitUiAnimAutoReverse(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* rawR = cg_.resolveOperand(inst->operands[1]);
    // (AR) دلالةٌ منطقيّة تُطابق Value::toBool في المفسّر (value.cpp:559):
    //      صحيح ⇒ x!=0، عشريّ ⇒ x!=0.0 (فـ.عكس_تلقائي(0.4) ⇒ true كالمفسّر،
    //      لا FPToSI الذي يبتر 0.4→0 ⇒ false). مؤشّر/غيره ⇒ false دفاعيًّا.
    auto* rt = rawR->getType();
    llvm::Value* revBit =
        rt->isIntegerTy()       ? cg_.builder_->CreateICmpNE(rawR, llvm::ConstantInt::get(rt, 0))
      : rt->isFloatingPointTy() ? cg_.builder_->CreateFCmpONE(rawR, llvm::ConstantFP::get(rt, 0.0))
                                : static_cast<llvm::Value*>(llvm::ConstantInt::getFalse(*cg_.context_));
    llvm::Value* rev = cg_.builder_->CreateZExt(revBit, i32Ty);
    return emitUIRuntimeCall(cg_, "sad_anim_auto_reverse", voidTy,
        {ptrTy, i32Ty}, {widget, rev});
}

// ─── خاصّيّة متعدّدة الوسائط (م-أ3ر): دمج بفواصل عبر مُجمِّع وقت التشغيل ───
// (AR) يختار دالة الإضافة بحسب نوع الوسيط الفعليّ (نظير اختيار الرمز في L1)
//      فيُنسَّق كلٌّ كما في Value::toString داخل وقت التشغيل (مصدر واحد).
llvm::Value* UICodeGen::emitUiPropJoinAdd(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* f64Ty = llvm::Type::getDoubleTy(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    const SIROperand& valOp = inst->operands[1];
    llvm::Value* raw = cg_.resolveOperand(valOp);
    switch (valOp.dataType) {
    case SadTypeKind::String: {
        if (!raw->getType()->isPointerTy()) raw = llvm::ConstantPointerNull::get(ptrTy);
        return emitUIRuntimeCall(cg_, "sad_prop_join_add_str", voidTy, {ptrTy, ptrTy}, {widget, raw});
    }
    case SadTypeKind::Boolean: {
        llvm::Value* b = raw->getType()->isIntegerTy()
            ? cg_.builder_->CreateIntCast(raw, i32Ty, false)
            : cg_.builder_->CreateFPToSI(raw, i32Ty);
        return emitUIRuntimeCall(cg_, "sad_prop_join_add_bool", voidTy, {ptrTy, i32Ty}, {widget, b});
    }
    case SadTypeKind::Float: {
        llvm::Value* d = raw->getType()->isIntegerTy()
            ? cg_.builder_->CreateSIToFP(raw, f64Ty)
            : cg_.builder_->CreateFPCast(raw, f64Ty);
        return emitUIRuntimeCall(cg_, "sad_prop_join_add_num", voidTy, {ptrTy, f64Ty}, {widget, d});
    }
    case SadTypeKind::Integer: {
        // (AR) صحيح؛ عشريّ السجلّ ⇒ FPToSI. لو وصل مؤشّر (استنتاج خاطئ) نُمرّره نصًّا
        //      آمنًا بدل FPToSI على مؤشّر (تعليمة غير صالحة تُسقِط المُتحقِّق).
        if (raw->getType()->isIntegerTy()) {
            llvm::Value* n = cg_.builder_->CreateIntCast(raw, i64Ty, true);
            return emitUIRuntimeCall(cg_, "sad_prop_join_add_int", voidTy, {ptrTy, i64Ty}, {widget, n});
        }
        if (raw->getType()->isFloatingPointTy()) {
            llvm::Value* n = cg_.builder_->CreateFPToSI(raw, i64Ty);
            return emitUIRuntimeCall(cg_, "sad_prop_join_add_int", voidTy, {ptrTy, i64Ty}, {widget, n});
        }
        // (AR) لم يكن صحيحًا ولا عشريًّا (مؤشّر/كائن باستنتاجٍ خاطئ) ⇒ سقوطٌ **مقصود**
        //      إلى default (المسار النصّيّ الآمن) بدل FPToSI على غير عدديّ.
    }
    [[fallthrough]];
    default: {
        // (AR) نوعٌ غير متوقَّع (مؤشّر/كائن/مصفوفة) ⇒ مسارٌ نصّيٌّ آمن بحارس مؤشّر
        //      (نظير toString، يتدهور بأمان) بدل FPToSI على غير عدديّ.
        llvm::Value* s = raw->getType()->isPointerTy()
            ? raw : llvm::ConstantPointerNull::get(ptrTy);
        return emitUIRuntimeCall(cg_, "sad_prop_join_add_str", voidTy, {ptrTy, ptrTy}, {widget, s});
    }
    }
}
llvm::Value* UICodeGen::emitUiPropJoinCommit(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* name = cg_.resolveOperand(inst->operands[1]);
    if (!name->getType()->isPointerTy()) name = llvm::ConstantPointerNull::get(ptrTy);
    return emitUIRuntimeCall(cg_, "sad_prop_join_commit", voidTy, {ptrTy, ptrTy}, {widget, name});
}

llvm::Value* UICodeGen::emitUiSetPropBool(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* name = cg_.resolveOperand(inst->operands[1]);
    // (AR) حارس دفاعيّ (اتّساقًا مع emitUiSetPropInt): المسار يُصدِر هنا صحيحًا/i1
    //      دائمًا، لكن نحمي من نوع LLVM غير متوقّع لأنّ emitUIRuntimeCall لا يُكيّف
    //      الأنواع (يمرّرها مباشرة لـCreateCall) فأيّ عدم تطابقٍ يُفشِل مُتحقِّق LLVM.
    llvm::Value* rawV = cg_.resolveOperand(inst->operands[2]);
    llvm::Value* value =
        rawV->getType()->isIntegerTy()       ? cg_.builder_->CreateIntCast(rawV, i32Ty, false)
      : rawV->getType()->isFloatingPointTy() ? cg_.builder_->CreateFPToSI(rawV, i32Ty)
                                             : static_cast<llvm::Value*>(llvm::ConstantInt::get(i32Ty, 0));
    return emitUIRuntimeCall(cg_, "sad_set_prop_bool", voidTy,
        {ptrTy, ptrTy, i32Ty}, {widget, name, value});
}

} // namespace LLVM
} // namespace Sad
