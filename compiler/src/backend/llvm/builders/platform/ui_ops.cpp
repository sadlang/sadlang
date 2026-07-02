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
#include "sir_constants.h" // (م1-ج) kSadNullSentinel — حارس بانِي يُرجع لاشيء/void
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

// ─── مخطّط بنية الإغلاق المشترك بين الثانكين ───
// (AR) بنية الإغلاق على الكومة {fn_ptr@0, env_ptr@1} (كلاهما i64) — نظير emitClosureCall.
//      نُسمّي الفهرسين تفاديًا للأرقام السحريّة المكرّرة في closure/page-builder thunks.
static constexpr uint64_t kClosureFnSlot = 0;  // مؤشّر الدالّة (fn_ptr)
static constexpr uint64_t kClosureEnvSlot = 1; // مؤشّر بيئة الالتقاط (env_ptr)

// ─── جسر ردّ نداء الواجهة (thunk الإغلاق) — إصلاح الالتقاط المكسور ───────────────
// (AR) في ص، أيّ دالّة/لامدا كقيمة = بنية إغلاق {fn_ptr:i64, env_ptr:i64} على الكومة،
//      يُمرَّر مؤشّرها كـi64. لكنّ runtime الواجهة يتوقّع `cb` = void(*)(void*) ويستدعي
//      cb(data). تمرير مؤشّر البنية كـcb (السلوك القديم) = قفزةٌ لبيانات لا كود ⇒ التقاطٌ
//      مكسور. الحلّ: cb = thunk عامّ، data = مؤشّر الإغلاق؛ الـthunk يعيد بناء fn(env).
//      المعالِج بلا وسائط صريحة توقيعه void(i64 __env) (expression_functional.cpp:138).
//
// ⚠ قيد ABI حرِج: الـthunk يفترض توقيعًا حصريًّا void(i64 __env) — أي معالِجًا بلا
//   وسائط صريحة (النمط الوحيد المعنيّ في مسار المترجم؛ runtime الواجهة يستدعي cb(data)
//   بوسيطٍ واحد لا خريطة حدث — sad_ui_runtime.cpp:867/872). لو كتب مبرمج معالِجًا
//   بوسيطٍ صريح «لامدا(س) => …» فتوقيع اللامدا الفعليّ (i64 س, i64 __env) والـthunk
//   يمرّر __env في خانة «س» تاركًا __env قمامةً ⇒ سلوكٌ غير معرَّف عند قراءة الملتقَطات.
//   (تباعدٌ صامت عن المفسّر الذي يمرّر خريطة الحدث؛ قيدٌ معروف — لا حارس نحويّ هنا.)
// ⚠ قيد عمر: data = مؤشّر الإغلاق (كومة malloc) يعيش في binding العنصر ويُستدعى عند
//   النقر لاحقًا؛ لا مُحرِّر (العناصر تعيش طوال التطبيق) ⇒ لا تعليقٌ فعليّ في هذا المسار.
[[nodiscard]] static llvm::Function* getOrCreateUiClosureThunk(LLVMCodeGen& cg) {
    llvm::Module* m = cg.getModule();
    if (auto* existing = m->getFunction("__sad_ui_closure_thunk")) return existing;
    auto& ctx = *cg.context_;
    auto* voidTy = llvm::Type::getVoidTy(ctx);
    auto* i64Ty = llvm::Type::getInt64Ty(ctx);
    auto* ptrTy = llvm::PointerType::getUnqual(ctx);
    auto* ft = llvm::FunctionType::get(voidTy, {ptrTy}, false); // void thunk(void* data)
    auto* fn = llvm::Function::Create(ft, llvm::Function::InternalLinkage, "__sad_ui_closure_thunk", m);
    fn->addFnAttr(llvm::Attribute::NoUnwind);
    llvm::IRBuilder<>& b = *cg.builder_;
    auto savedBB = b.GetInsertBlock();
    auto savedIP = savedBB ? b.GetInsertPoint() : llvm::BasicBlock::iterator();
    auto* entry = llvm::BasicBlock::Create(ctx, "entry", fn);
    b.SetInsertPoint(entry);
    llvm::Value* data = fn->getArg(0); // = مؤشّر بنية الإغلاق
    // fn_ptr = closure[kClosureFnSlot]، env_ptr = closure[kClosureEnvSlot] — نظير emitClosureCall.
    llvm::Value* fnSlot = b.CreateGEP(i64Ty, data, llvm::ConstantInt::get(i64Ty, kClosureFnSlot), "fn.slot");
    llvm::Value* fnI64 = b.CreateLoad(i64Ty, fnSlot, "fn.i64");
    llvm::Value* envSlot = b.CreateGEP(i64Ty, data, llvm::ConstantInt::get(i64Ty, kClosureEnvSlot), "env.slot");
    llvm::Value* envI64 = b.CreateLoad(i64Ty, envSlot, "env.i64");
    llvm::Value* handlerFn = b.CreateIntToPtr(fnI64, ptrTy, "handler.fn");
    auto* handlerFt = llvm::FunctionType::get(voidTy, {i64Ty}, false); // void(i64 __env)
    b.CreateCall(handlerFt, handlerFn, {envI64});
    b.CreateRetVoid();
    if (savedBB) b.SetInsertPoint(savedBB, savedIP);
    return fn;
}

// (AR) يحوّل قيمة إغلاق (i64/ptr) إلى زوج (cb=thunk, data=مؤشّر الإغلاق). null ⇒ (null,null).
struct UiCallbackPair { llvm::Value* cb = nullptr; llvm::Value* data = nullptr; };
[[nodiscard]] static UiCallbackPair bridgeUiCallback(LLVMCodeGen& cg, llvm::Value* closureVal) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg.context_);
    auto* nullp = llvm::ConstantPointerNull::get(ptrTy);
    if (!closureVal) return { nullp, nullp };
    llvm::Value* dataPtr;
    if (closureVal->getType()->isPointerTy())
        dataPtr = closureVal;
    else if (closureVal->getType()->isIntegerTy())
        dataPtr = cg.builder_->CreateIntToPtr(closureVal, ptrTy, "cb.closure");
    else
        return { nullp, nullp }; // نوع غير متوقّع ⇒ لا ردّ نداء (كالحارس الدفاعيّ السابق)
    return { getOrCreateUiClosureThunk(cg), dataPtr };
}

// =====================================================================
// (م1-ج، توقيع البانِي) جسر ثانك باني الصفحة: يحوّل إغلاق ص (دالّة تُرجع عنصرًا)
//   إلى SadPageBuilder المكتبيّ `SadWidget(*)(void* data)`. نظير closure_thunk لكن
//   **يُرجع عنصرًا** بدل void. الثانك عامّ (thunk واحد لكلّ وحدة) يعمل لأيّ بانٍ:
//   يحمّل fn@0/env@1 (نظير emitClosureCall)، يستدعي البانِي ببروتوكول الإغلاق
//   `i64 fn(i64 __env)` ثمّ inttoptr للنتيجة ⇒ SadWidget.
// ⚠ توافق ABI الإرجاع: بانِي ص المُغلَّف قد يُرجع ptr (func-ref wrapper) أو i64
//   (lambda-conv wrapper، closure_ops.cpp:314-317)؛ كلاهما يعود في سجلّ النتيجة
//   نفسه على x64/ARM64، فقراءتُه i64 ثمّ inttoptr صحيحةٌ للحالتين (نظير بروتوكول
//   CLOSURE_CALL الذي يفترض i64). الثانك يطابق هذا البروتوكول لا توقيع البانِي.
// ⚠ عمر data (بيئة الإغلاق): يملكه مكدّس التنقّل ويحرّره بـrelease عند الإسقاط (Q5)؛
//   نمرّر release=null هنا (بيئة الإغلاق مُدارة كومةً تعيش مع البرنامج — لا مُحرِّر
//   بعدُ في المترجم، نظير قيد عمر ردّ نداء الحدث). لا تعليقٌ فعليّ في هذا المسار.
[[nodiscard]] static llvm::Function* getOrCreateUiPageBuilderThunk(LLVMCodeGen& cg) {
    llvm::Module* m = cg.getModule();
    if (auto* existing = m->getFunction("__sad_ui_page_builder_thunk")) return existing;
    auto& ctx = *cg.context_;
    auto* i64Ty = llvm::Type::getInt64Ty(ctx);
    auto* ptrTy = llvm::PointerType::getUnqual(ctx);
    auto* ft = llvm::FunctionType::get(ptrTy, {ptrTy}, false); // ptr thunk(void* data)
    auto* fn = llvm::Function::Create(ft, llvm::Function::InternalLinkage, "__sad_ui_page_builder_thunk", m);
    fn->addFnAttr(llvm::Attribute::NoUnwind);
    llvm::IRBuilder<>& b = *cg.builder_;
    auto savedBB = b.GetInsertBlock();
    auto savedIP = savedBB ? b.GetInsertPoint() : llvm::BasicBlock::iterator();
    auto* entry = llvm::BasicBlock::Create(ctx, "entry", fn);
    b.SetInsertPoint(entry);
    llvm::Value* data = fn->getArg(0); // = مؤشّر بنية الإغلاق {fn_ptr@0, env_ptr@1}
    llvm::Value* fnSlot = b.CreateGEP(i64Ty, data, llvm::ConstantInt::get(i64Ty, kClosureFnSlot), "fn.slot");
    llvm::Value* fnI64 = b.CreateLoad(i64Ty, fnSlot, "fn.i64");
    llvm::Value* envSlot = b.CreateGEP(i64Ty, data, llvm::ConstantInt::get(i64Ty, kClosureEnvSlot), "env.slot");
    llvm::Value* envI64 = b.CreateLoad(i64Ty, envSlot, "env.i64");
    llvm::Value* builderFn = b.CreateIntToPtr(fnI64, ptrTy, "builder.fn");
    // (AR) بروتوكول الإغلاق: i64 fn(i64 __env) — نظير emitClosureCall (env آخر وسيط).
    auto* builderFt = llvm::FunctionType::get(i64Ty, {i64Ty}, false);
    llvm::Value* widgetI64 = b.CreateCall(builderFt, builderFn, {envI64}, "page.i64");
    // (AR) حارس بانٍ يُرجع «لاشيء» (kSadNullSentinel) أو 0: أعِد مؤشّرًا فارغًا حقيقيًّا
    //      بدل inttoptr للحارس (= مؤشّر قمامةٍ غير-فارغ يجتاز حارس impl في sad_print_tree
    //      ⇒ انهيار). هكذا تُعرَض صفحةٌ فارغة (لا شيء) مطابقةً للمفسّر (طباعة صامتة على
    //      صفحةٍ فارغة) بدل الانهيار. (⚠ بانٍ يُرجع void حقيقيًّا — لا ارجع أصلًا — يترك
    //      سجلّ النتيجة غير معرَّف فقد يفلت من الحارس؛ ذاك سوء استخدامٍ: البانِي يجب أن
    //      يُرجع عنصرًا أو لاشيء.)
    auto* sentinel = llvm::ConstantInt::get(i64Ty, static_cast<uint64_t>(Sad::Compiler::kSadNullSentinel), /*isSigned=*/true);
    auto* zero = llvm::ConstantInt::get(i64Ty, 0);
    llvm::Value* isSent = b.CreateICmpEQ(widgetI64, sentinel, "page.is_null_sentinel");
    llvm::Value* isZero = b.CreateICmpEQ(widgetI64, zero, "page.is_zero");
    llvm::Value* isNull = b.CreateOr(isSent, isZero, "page.is_null");
    llvm::Value* asPtr = b.CreateIntToPtr(widgetI64, ptrTy, "page.widget");
    llvm::Value* nullp = llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* widgetPtr = b.CreateSelect(isNull, nullp, asPtr, "page.safe");
    b.CreateRet(widgetPtr);
    if (savedBB) b.SetInsertPoint(savedBB, savedIP);
    return fn;
}

// (AR) هل الوسيط بانٍ (إغلاق/دالّة ص، يُحلّ إلى i64) لا لقطة عنصر (ptr)؟ تمييزٌ نظير
//      bridgeUiCallback: العنصر المبنيّ ptr، والإغلاق i64 (ptrtoint closure.alloc).
struct UiBuilderTriple { bool isBuilder = false; llvm::Value* build = nullptr; llvm::Value* data = nullptr; };
[[nodiscard]] static UiBuilderTriple bridgeUiPageBuilder(LLVMCodeGen& cg, llvm::Value* arg) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg.context_);
    if (arg && arg->getType()->isIntegerTy()) {
        llvm::Value* dataPtr = cg.builder_->CreateIntToPtr(arg, ptrTy, "nav.closure");
        return { true, getOrCreateUiPageBuilderThunk(cg), dataPtr };
    }
    return { false, nullptr, nullptr };
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
    // (AR) ردّ النداء = إغلاق (operands[1]) ⇒ جسر thunk (cb=thunk, data=مؤشّر الإغلاق).
    llvm::Value* closureVal = inst->operands.size() > 1 ? cg_.resolveOperand(inst->operands[1]) : nullptr;
    auto cbPair = bridgeUiCallback(cg_, closureVal);
    auto* result = emitUIRuntimeCall(cg_, "sad_button", ptrTy,
        {ptrTy, ptrTy, ptrTy}, {label, cbPair.cb, cbPair.data});
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
    llvm::Value* closureVal = inst->operands.size() > 6 ? cg_.resolveOperand(inst->operands[6]) : nullptr;
    auto cbPair = bridgeUiCallback(cg_, closureVal); // جسر thunk الإغلاق (إصلاح الالتقاط)
    auto* result = emitUIRuntimeCall(cg_, "sad_button_variant", ptrTy,
        {ptrTy, i32Ty, f32Ty, f32Ty, f32Ty, f32Ty, ptrTy, ptrTy},
        {label, variant, r, g, b, a, cbPair.cb, cbPair.data});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiIconButton(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    // (AR) حارس: زر_أيقونة() بلا وسائط ⇒ operands فارغة (منع انهيار).
    llvm::Value* icon = !inst->operands.empty() ?
        cg_.resolveOperand(inst->operands[0]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* closureVal = inst->operands.size() > 1 ? cg_.resolveOperand(inst->operands[1]) : nullptr;
    auto cbPair = bridgeUiCallback(cg_, closureVal); // جسر thunk الإغلاق
    auto* result = emitUIRuntimeCall(cg_, "sad_icon_button", ptrTy,
        {ptrTy, ptrTy, ptrTy}, {icon, cbPair.cb, cbPair.data});
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
    llvm::Value* closureVal = inst->operands.size() > 5 ? cg_.resolveOperand(inst->operands[5]) : nullptr;
    auto cbPair = bridgeUiCallback(cg_, closureVal); // جسر thunk الإغلاق
    auto* result = emitUIRuntimeCall(cg_, "sad_fab", ptrTy,
        {ptrTy, f32Ty, f32Ty, f32Ty, f32Ty, ptrTy, ptrTy},
        {icon, r, g, b, a, cbPair.cb, cbPair.data});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiTextField(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    // (AR) حارس: حقل_نص() بلا وسائط ⇒ operands فارغة؛ نمرّر null (runtime لا يضبط
    //      «تلميح» عند null ⇒ يطابق المفسّر). بلا الحارس يقع وصولٌ خارج الحدود ⇒ انهيار.
    llvm::Value* hint = !inst->operands.empty() ?
        cg_.resolveOperand(inst->operands[0]) : llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* closureVal = inst->operands.size() > 1 ? cg_.resolveOperand(inst->operands[1]) : nullptr;
    auto cbPair = bridgeUiCallback(cg_, closureVal); // جسر thunk الإغلاق
    auto* result = emitUIRuntimeCall(cg_, "sad_text_field", ptrTy,
        {ptrTy, ptrTy, ptrTy}, {hint, cbPair.cb, cbPair.data});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiCheckbox(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Value* closureVal = !inst->operands.empty() ? cg_.resolveOperand(inst->operands[0]) : nullptr;
    auto cbPair = bridgeUiCallback(cg_, closureVal); // جسر thunk الإغلاق
    auto* result = emitUIRuntimeCall(cg_, "sad_checkbox", ptrTy,
        {ptrTy, ptrTy}, {cbPair.cb, cbPair.data});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiSwitch(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Value* closureVal = !inst->operands.empty() ? cg_.resolveOperand(inst->operands[0]) : nullptr;
    auto cbPair = bridgeUiCallback(cg_, closureVal); // جسر thunk الإغلاق
    auto* result = emitUIRuntimeCall(cg_, "sad_switch_toggle", ptrTy,
        {ptrTy, ptrTy}, {cbPair.cb, cbPair.data});
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
    llvm::Value* closureVal = inst->operands.size() > 2 ? cg_.resolveOperand(inst->operands[2]) : nullptr;
    auto cbPair = bridgeUiCallback(cg_, closureVal); // جسر thunk الإغلاق
    auto* result = emitUIRuntimeCall(cg_, "sad_slider", ptrTy,
        {f32Ty, f32Ty, ptrTy, ptrTy}, {minVal, maxVal, cbPair.cb, cbPair.data});
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

// (AR) م-تحكّم: دوال الثيم ⇒ جسرٌ فوق حالة الثيم المكتبيّة (sad::ui::*)، نظير المفسّر.
//      تبديل/داكن/فاتح نداءات void بلا وسائط؛ هل_داكن يُرجع i1 (bool) ويُخزَّن في namedValues.
llvm::Value* UICodeGen::emitUiToggleTheme(std::shared_ptr<SIRInstruction> /*inst*/) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitUIRuntimeCall(cg_, "sad_toggle_theme", voidTy, {}, {});
}

llvm::Value* UICodeGen::emitUiDarkMode(std::shared_ptr<SIRInstruction> /*inst*/) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitUIRuntimeCall(cg_, "sad_set_dark", voidTy, {}, {});
}

llvm::Value* UICodeGen::emitUiLightMode(std::shared_ptr<SIRInstruction> /*inst*/) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitUIRuntimeCall(cg_, "sad_set_light", voidTy, {}, {});
}

llvm::Value* UICodeGen::emitUiIsDark(std::shared_ptr<SIRInstruction> inst) {
    auto* i1Ty = llvm::Type::getInt1Ty(*cg_.context_);
    auto* result = emitUIRuntimeCall(cg_, "sad_is_dark", i1Ty, {}, {});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

// (AR) مدّة الانتقال البصريّ الافتراضيّة عند حذف الوسيط (م2). ⚠ يجب أن تطابق
//      sad::ui::kDefaultTransitionSec في المكتبة (nav.h) وحلقة النافذة (sad_ui_runtime.cpp)
//      ضمانًا لتماثل السلوك بين المُدخَل من الوسيط والمُستهلَك من المكدّس.
static constexpr float kUiDefaultTransitionSec = 0.3f;

// (AR) م-تحكّم: التنقّل ⇒ جسرٌ فوق مكدّس التنقّل المكتبيّ (sad::ui::nav).
//      انتقل/استبدل يمرّران مقبض الصفحة (ptr) أو بانيًا (دالّة، م1-ج)؛ عودة/عودة_للبداية
//      بلا وسائط؛ عدد_الصفحات يُرجع i64 ويُخزَّن.
llvm::Value* UICodeGen::emitUiNavigate(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* page = inst->operands.empty()
        ? llvm::ConstantPointerNull::get(ptrTy)
        : cg_.resolveOperand(inst->operands[0]);
    // (م1-ج) دالّة/إغلاق ⇒ نموذج البانِي (تفاعليّة عبر buildCurrent)؛ عنصر ⇒ لقطة.
    auto tri = bridgeUiPageBuilder(cg_, page);
    if (tri.isBuilder) {
        auto* nullRel = llvm::ConstantPointerNull::get(ptrTy);
        return emitUIRuntimeCall(cg_, "sad_navigate_builder", voidTy,
            {ptrTy, ptrTy, ptrTy}, {tri.build, tri.data, nullRel});
    }
    return emitUIRuntimeCall(cg_, "sad_navigate", voidTy, {ptrTy}, {page});
}

llvm::Value* UICodeGen::emitUiNavBack(std::shared_ptr<SIRInstruction> /*inst*/) {
    auto* i1Ty = llvm::Type::getInt1Ty(*cg_.context_);
    // (AR) النتيجة المنطقيّة تُهمَل (عودة void في ص، كالمفسّر).
    return emitUIRuntimeCall(cg_, "sad_navigate_back", i1Ty, {}, {});
}

llvm::Value* UICodeGen::emitUiNavRoot(std::shared_ptr<SIRInstruction> /*inst*/) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitUIRuntimeCall(cg_, "sad_navigate_root", voidTy, {}, {});
}

llvm::Value* UICodeGen::emitUiReplacePage(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* page = inst->operands.empty()
        ? llvm::ConstantPointerNull::get(ptrTy)
        : cg_.resolveOperand(inst->operands[0]);
    // (م1-ج) دالّة/إغلاق ⇒ بدّل ببانٍ (تفاعليّة)؛ عنصر ⇒ لقطة.
    auto tri = bridgeUiPageBuilder(cg_, page);
    if (tri.isBuilder) {
        auto* nullRel = llvm::ConstantPointerNull::get(ptrTy);
        return emitUIRuntimeCall(cg_, "sad_replace_page_builder", voidTy,
            {ptrTy, ptrTy, ptrTy}, {tri.build, tri.data, nullRel});
    }
    return emitUIRuntimeCall(cg_, "sad_replace_page", voidTy, {ptrTy}, {page});
}

llvm::Value* UICodeGen::emitUiPageCount(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* result = emitUIRuntimeCall(cg_, "sad_page_count", i64Ty, {}, {});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

// (AR) الصفحة_الحالية() ⇒ sad_current_page(): يُرجع SadWidget (ptr) ويُخزَّن — حارس
//      بنيويّ (طباعة_شجرة(الصفحة_الحالية()) تكشف الجذر المرسوم).
llvm::Value* UICodeGen::emitUiCurrentPage(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* result = emitUIRuntimeCall(cg_, "sad_current_page", ptrTy, {}, {});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

// (م2) انتقل_بتحريك ⇒ sad_navigate_transition(page, transType, duration).
llvm::Value* UICodeGen::emitUiNavigateTransition(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* page = inst->operands.empty() ? llvm::ConstantPointerNull::get(ptrTy)
                                               : cg_.resolveOperand(inst->operands[0]);
    llvm::Value* trans = inst->operands.size() > 1 ? cg_.resolveOperand(inst->operands[1])
                                                   : llvm::ConstantPointerNull::get(ptrTy);
    if (!trans->getType()->isPointerTy()) trans = llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* dur = inst->operands.size() > 2 ? castNumericToF32(cg_, cg_.resolveOperand(inst->operands[2]))
                                                 : llvm::ConstantFP::get(f32Ty, kUiDefaultTransitionSec);
    // (م1-ج) دالّة/إغلاق ⇒ بانٍ تفاعليّ + انتقال؛ عنصر ⇒ لقطة + انتقال. بلا هذا التفريع
    //        يُمرَّر إغلاق i64 إلى مُعامل ptr ⇒ فشل تحقّق LLVM (تباعد صامت عن المفسّر الذي
    //        يقبل الدالّة عبر interpNavEntryFor). — إصلاح مراجعة Amelia (HIGH-1).
    auto tri = bridgeUiPageBuilder(cg_, page);
    if (tri.isBuilder) {
        auto* nullRel = llvm::ConstantPointerNull::get(ptrTy);
        return emitUIRuntimeCall(cg_, "sad_navigate_transition_builder", voidTy,
            {ptrTy, ptrTy, ptrTy, ptrTy, f32Ty}, {tri.build, tri.data, nullRel, trans, dur});
    }
    return emitUIRuntimeCall(cg_, "sad_navigate_transition", voidTy, {ptrTy, ptrTy, f32Ty}, {page, trans, dur});
}

// (م2) عودة_بتحريك ⇒ sad_navigate_back_transition(transType, duration). النتيجة تُهمَل.
llvm::Value* UICodeGen::emitUiBackTransition(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* i1Ty = llvm::Type::getInt1Ty(*cg_.context_);
    llvm::Value* trans = !inst->operands.empty() ? cg_.resolveOperand(inst->operands[0])
                                                 : llvm::ConstantPointerNull::get(ptrTy);
    if (!trans->getType()->isPointerTy()) trans = llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* dur = inst->operands.size() > 1 ? castNumericToF32(cg_, cg_.resolveOperand(inst->operands[1]))
                                                 : llvm::ConstantFP::get(f32Ty, kUiDefaultTransitionSec);
    return emitUIRuntimeCall(cg_, "sad_navigate_back_transition", i1Ty, {ptrTy, f32Ty}, {trans, dur});
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
    // (AR) ردّ النداء = إغلاق (operands[2]) ⇒ جسر thunk (cb=thunk, data=مؤشّر الإغلاق).
    //      يُصلح الالتقاط المكسور؛ operands[3] القديم (userData) لم يعد يُستعمَل (data من الإغلاق).
    llvm::Value* closureVal = inst->operands.size() > 2 ? cg_.resolveOperand(inst->operands[2]) : nullptr;
    auto cbPair = bridgeUiCallback(cg_, closureVal);
    return emitUIRuntimeCall(cg_, "sad_add_event", voidTy,
        {ptrTy, ptrTy, ptrTy, ptrTy}, {widget, name, cbPair.cb, cbPair.data});
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
