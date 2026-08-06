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
#include "builders/platform/ui_codegen.h"
#include "sad_dyn_repr.h" // (AR) فكّ القيمة الديناميّة %SadDyn (unpackDouble/isSadDyn)
// (AR) تصنيفُ المقبضِ المُبهَمِ — التعريفُ نفسُه الذي يستعملُه مسارا الخفضِ الأماميّان،
//      فلا يتباعدُ حكمُ «ما لا يجوزُ أن يُقرأَ نصًّا» بين الطبقتَين.
#include "builders/ui_prop_lowering.h"
#include "sir_constants.h" // (م1-ج) kSadNullSentinel — حارس بانِي يُرجع لاشيء/void
#include "sad_event_layout_generated.h" // (② rfcs#46) تخطيط «حدث» المولَّد (SAD_EVENT_FIELDS/POD/الاسم)
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

// (AR) إكراه وسيطٍ واحد إلى نوع معامل الدالّة المُعلَن — الحارس الجذريّ الموحَّد ضدّ
//      «Calling a function with a bad signature!». مدمجات الواجهة تُعلِن معاملاتها
//      بأنواع صريحة (غالبًا ptr للعناصر/النصوص)، لكنّ resolveOperand قد يُعيد قيمةً
//      بنوعٍ مختلف (i64) حين يتدفّق العنصر عبر خانةٍ عدديّة: معامل دالّة بلا نوع، أو
//      استنتاج إرجاعٍ افتراضيّه i64 في وحدةٍ مستورَدة، أو مؤقّت لُوِّن ptrtoint. تمرير
//      i64 لمعاملٍ ptr يُفشل verifyModule. هذا الإكراه (نظير حلقة تحويل الوسائط في
//      cf_branch_call.cpp، وتعميمٌ لـcoerceUiChildToPtr من sad_add_child إلى كلّ
//      نقطة الاختناق) يضمن تطابق التوقيع دائمًا. inttoptr على عنوانٍ ليس عنصرًا
//      يحرسه runtime الواجهة وقت التشغيل (غير مُسجَّل في g_widgets) بأمان.
// (EN) Coerce one arg to the callee's declared param type — the unified root guard
//      against "Calling a function with a bad signature!". UI builtins declare their
//      params with explicit types (usually ptr for widgets/strings), but resolveOperand
//      may hand back a differently-typed value (i64) when a widget flows through an
//      integer slot: an untyped function param, an imported-module return whose inferred
//      type defaults to i64, or a ptrtoint'd temporary. Passing i64 to a ptr param fails
//      verifyModule. This coercion (mirroring the arg-conversion loop in cf_branch_call.cpp,
//      generalizing coerceUiChildToPtr from sad_add_child to the single choke point)
//      keeps the signature matched. inttoptr of a non-widget address is guarded safely by
//      the UI runtime at run time (absent from g_widgets).
static llvm::Value* coerceUiArgToParam(LLVMCodeGen& cg, llvm::Value* v, llvm::Type* want) {
    if (!v || !want) return v;
    llvm::Type* have = v->getType();
    if (have == want) return v;
    llvm::IRBuilder<>* b = cg.getBuilder();
    // (AR) عدد ⇄ مؤشّر (الحالة الأكثر شيوعًا: عنصر مرّ عبر خانة i64)
    if (want->isPointerTy() && have->isIntegerTy())
        return b->CreateIntToPtr(v, want);
    if (want->isIntegerTy() && have->isPointerTy())
        return b->CreatePtrToInt(v, want);
    // (AR) عرض صحيح مختلف (i1/i32/i64 …): المنطقيّ i1 يُوسَّع بالأصفار (وإلّا
    //      صار «صحيح» = -1 بدل 1)؛ أعداد ص موقَّعة فتُوسَّع بالإشارة.
    // (EN) Differing int width: a boolean (i1) must zero-extend (else true→-1,
    //      not 1); Sad integers are signed, so sign-extend the rest.
    if (want->isIntegerTy() && have->isIntegerTy())
        return b->CreateIntCast(v, want, /*isSigned=*/!have->isIntegerTy(1));
    // (AR) عشريّ من صحيح / بين عرضَي عشريّ
    if (want->isFloatingPointTy() && have->isIntegerTy())
        return b->CreateSIToFP(v, want);
    if (want->isFloatingPointTy() && have->isFloatingPointTy())
        return b->CreateFPCast(v, want);
    if (want->isIntegerTy() && have->isFloatingPointTy())
        return b->CreateFPToSI(v, want);
    // (AR) مؤشّر ⇄ مؤشّر (opaque) متطابق فعليًّا؛ أنواع أخرى نادرة تُترك — verifyModule الحارس النهائيّ.
    return v;
}

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

    // (AR) إكراه كلّ وسيط إلى نوع معامل الدالّة الفعليّ قبل الاستدعاء — يمنع
    //      «bad signature» مهما كان نوع القيمة الذي أعاده resolveOperand. نعتمد
    //      توقيع الدالّة الفعليّ (قد تكون أُعلنت سابقًا) لا argTypes فقط.
    // (EN) Coerce every arg to the callee's actual param type before the call —
    //      prevents "bad signature" whatever type resolveOperand returned. We use the
    //      function's actual signature (it may have been declared earlier), not argTypes.
    llvm::FunctionType* fnTy = fn->getFunctionType();
    std::vector<llvm::Value*> coerced;
    coerced.reserve(argValues.size());
    for (size_t i = 0; i < argValues.size(); ++i) {
        llvm::Type* want = (i < fnTy->getNumParams()) ? fnTy->getParamType(i) : nullptr;
        coerced.push_back(want ? coerceUiArgToParam(cg, argValues[i], want) : argValues[i]);
    }

    // (AR) دوال void لا يجب أن تحمل اسماً للنتيجة
    // (EN) void functions should not have a result name
    if (retType->isVoidTy()) {
        return builder->CreateCall(fn, coerced);
    }
    return builder->CreateCall(fn, coerced, funcName + "_result");
}

// (AR) تحصين نوع معاملات إدارة الشجرة (add/remove/clear child): باني الحاويات
//      يُصدر sad_add_child لكلّ وسيطٍ نوعه Pointer في SIR، لكنّ بعض تلك القيم تُلوَّن
//      قيمةً عدديّة في LLVM (مثل &عدد ⇒ ptrtoint i64). تمريرها لدالّةٍ معامِلها ptr
//      يُفشل verifyModule. نُكرِهها إلى ptr (inttoptr) فيصحّ التوقيع؛ والقيمة الناتجة
//      (عنوانٌ ليس عنصرًا) يتجاهلها حارس sad_add_child وقت التشغيل (غير مُسجَّل في
//      g_widgets) بأمان — فلا verifyModule ولا UB. العناصر الحقيقيّة أصلًا ptr (لا أثر).
static llvm::Value* coerceUiChildToPtr(LLVMCodeGen& cg, llvm::Value* v, llvm::Type* ptrTy) {
    if (!v) return v;
    llvm::Type* t = v->getType();
    if (t->isPointerTy()) return v;
    if (t->isIntegerTy()) return cg.getBuilder()->CreateIntToPtr(v, ptrTy);
    return v; // أنواع أخرى نادرة تُترك؛ verifyModule يبقى الحارس النهائيّ
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

// ─── (② rfcs#46) جسر ردّ نداء الحدث ذي بيانات الحدث ─────────────────────────────
// (AR) خلافًا للثانك أعلاه (معالِج بلا وسائط)، هذا المسار يبني بنية «حدث» بلغة ص من
//   SadEventPod الذي يملؤه وقت التشغيل (sad_ui_runtime.cpp: sadFillEventPod)، ثمّ
//   يمرّرها للمعالِج ذي المعامل الواحد `دالة(حدث ح)`. التوقيع الجديد للجسر يطابق
//   SadEventCallback = void(*)(void* data, const void* pod) تمامًا (لا اتّكال على
//   تجاهل وسيطٍ زائد عبر cdecl). نوعان: صفريّ الأريّة (يتجاهل pod) وذو بنية.

// (AR) نوع POD الجسر بلغة LLVM — بترتيب SAD_EVENT_FIELDS وبنوع كلّ خانة ABI. كلّ
//   الخانات 8 بايت على x86_64/ARM64 (double/i64/مؤشّر)، لكنّنا نبني النوع صراحةً
//   بأعضاءٍ منمَّطة ونفهرس بـGEP عضويّ فيصحّ التخطيط بلا افتراض حجمٍ موحَّد.
[[nodiscard]] static llvm::StructType* getOrCreateSadEventPodType(LLVMCodeGen& cg) {
    if (auto* existing = llvm::StructType::getTypeByName(*cg.context_, "__sad_event_pod"))
        return existing;
    auto& ctx = *cg.context_;
    auto* i64Ty = llvm::Type::getInt64Ty(ctx);
    auto* f64Ty = llvm::Type::getDoubleTy(ctx);
    auto* ptrTy = llvm::PointerType::getUnqual(ctx);
    std::vector<llvm::Type*> members;
    members.reserve(::Sad::Types::EventLayout::SAD_EVENT_FIELDS.size());
    for (const auto& f : ::Sad::Types::EventLayout::SAD_EVENT_FIELDS) {
        switch (f.abi) {
        case ::Sad::Types::EventLayout::AbiSlot::F64: members.push_back(f64Ty); break;
        case ::Sad::Types::EventLayout::AbiSlot::PTR: members.push_back(ptrTy); break;
        case ::Sad::Types::EventLayout::AbiSlot::I64:
        default: members.push_back(i64Ty); break;
        }
    }
    return llvm::StructType::create(ctx, members, "__sad_event_pod"); // (خ-17) اسم محجوز بادئة __sad
}

// (AR) ثانك الحدث: void thunk(void* data, void* pod) — يبني بنية «حدث» من pod ويمرّرها.
// (م-7) classStructTypes["حدث"] مضمونٌ دائمًا: sir_builder_module.cpp يُسجّل صنف «حدث»
//   في كلّ وحدة قبل أيّ معالجة، وpreprocessClasses يبني نوعه. فرعُ الغياب أدناه دفاعيّ
//   بحت (لا يُبلَغ عمليًّا)؛ لو بلغ لَمرّر مؤشّرًا لاغيًا — لكنّه غير قابل للوصول بالبناء.
[[nodiscard]] static llvm::Function* getOrCreateUiEventThunk(LLVMCodeGen& cg) {
    llvm::Module* m = cg.getModule();
    if (auto* existing = m->getFunction("__sad_ui_event_thunk")) return existing;
    auto& ctx = *cg.context_;
    auto* voidTy = llvm::Type::getVoidTy(ctx);
    auto* i64Ty = llvm::Type::getInt64Ty(ctx);
    auto* f64Ty = llvm::Type::getDoubleTy(ctx);
    auto* ptrTy = llvm::PointerType::getUnqual(ctx);
    auto* ft = llvm::FunctionType::get(voidTy, {ptrTy, ptrTy}, false); // void(void* data, void* pod)
    auto* fn = llvm::Function::Create(ft, llvm::Function::InternalLinkage, "__sad_ui_event_thunk", m);
    fn->addFnAttr(llvm::Attribute::NoUnwind);
    llvm::IRBuilder<>& b = *cg.builder_;
    auto savedBB = b.GetInsertBlock();
    auto savedIP = savedBB ? b.GetInsertPoint() : llvm::BasicBlock::iterator();
    auto* entry = llvm::BasicBlock::Create(ctx, "entry", fn);
    b.SetInsertPoint(entry);

    llvm::Value* data = fn->getArg(0);  // مؤشّر بنية الإغلاق {fn@0, env@1}
    llvm::Value* podRaw = fn->getArg(1); // مؤشّر SadEventPod

    // ─── استخراج fn/env من الإغلاق (نظير الثانك الصفريّ) ───
    llvm::Value* fnSlot = b.CreateGEP(i64Ty, data, llvm::ConstantInt::get(i64Ty, kClosureFnSlot), "fn.slot");
    llvm::Value* fnI64 = b.CreateLoad(i64Ty, fnSlot, "fn.i64");
    llvm::Value* envSlot = b.CreateGEP(i64Ty, data, llvm::ConstantInt::get(i64Ty, kClosureEnvSlot), "env.slot");
    llvm::Value* envI64 = b.CreateLoad(i64Ty, envSlot, "env.i64");
    llvm::Value* handlerFn = b.CreateIntToPtr(fnI64, ptrTy, "handler.fn");

    // ─── بناء بنية «حدث» على المكدّس وملؤها من POD ───
    const std::string eventClass(::Sad::Types::EventLayout::SAD_EVENT_STRUCT_NAME);
    auto stIt = cg.context_info_.classStructTypes.find(eventClass);
    llvm::Value* eventPtr = llvm::ConstantPointerNull::get(ptrTy);
    if (stIt != cg.context_info_.classStructTypes.end() && stIt->second) {
        llvm::StructType* eventTy = stIt->second;
        llvm::StructType* podTy = getOrCreateSadEventPodType(cg);
        llvm::Value* evt = b.CreateAlloca(eventTy, nullptr, "event.obj");
        // (AR) الحقل 0 = مؤشّر vtable — بنية «حدث» بلا دوال، فنصفّره (لا يُقرأ في الوصول).
        llvm::Value* vptr = b.CreateStructGEP(eventTy, evt, 0, "event.vptr");
        b.CreateStore(llvm::ConstantPointerNull::get(ptrTy), vptr);
        // (AR) الحقول 1..N بترتيب SAD_EVENT_FIELDS نفسه (= ترتيب classFieldNames["حدث"]).
        const auto& fields = ::Sad::Types::EventLayout::SAD_EVENT_FIELDS;
        for (unsigned i = 0; i < fields.size(); ++i) {
            const auto& fdesc = fields[i];
            llvm::Value* podSlot = b.CreateStructGEP(podTy, podRaw, i, "pod.f");
            unsigned structIdx = i + 1; // (AR) +1 لتخطّي vtable
            llvm::Value* dstSlot = b.CreateStructGEP(eventTy, evt, structIdx, "event.f");
            using K = ::Sad::Types::SadTypeKind;
            switch (fdesc.kind) {
            case K::Float: {
                llvm::Value* v = b.CreateLoad(f64Ty, podSlot, "f.f64");
                b.CreateStore(v, dstSlot);
                break;
            }
            case K::Boolean: {
                // (AR) POD يخزّن i64 (0/1)؛ حقل الصنف i1 ⇒ نُقارن ≠0 (نظير toBool).
                llvm::Value* raw = b.CreateLoad(i64Ty, podSlot, "f.i64");
                llvm::Value* bit = b.CreateICmpNE(raw, llvm::ConstantInt::get(i64Ty, 0), "f.bool");
                b.CreateStore(bit, dstSlot);
                break;
            }
            case K::String: {
                // (إصلاح ع-2) POD يحمل const char* من EventData::<نصّ>.c_str() الذي يتدلّى
                //   برجوع dispatchCompiledEvent. نستدعي sad_event_dup_str لأخذ نسخةٍ مملوكة
                //   (مُسرَّبة كنموذج نصوص ص) فيبقى ح.قيمة/ح.اسم_المفتاح صالحًا لو خزّنه المعالِج
                //   بعد العودة. الفارغ يعيد "" ساكنًا (بلا تخصيص) فلا تسرّب لكلّ إطار سحب.
                llvm::Value* raw = b.CreateLoad(ptrTy, podSlot, "f.ptr");
                auto dupFn = m->getOrInsertFunction(
                    "sad_event_dup_str",
                    llvm::FunctionType::get(ptrTy, {ptrTy}, false));
                llvm::Value* owned = b.CreateCall(dupFn, {raw}, "f.str.owned");
                b.CreateStore(owned, dstSlot);
                break;
            }
            case K::Integer:
            default: {
                llvm::Value* v = b.CreateLoad(i64Ty, podSlot, "f.i64");
                b.CreateStore(v, dstSlot);
                break;
            }
            }
        }
        eventPtr = evt;
    }

    // ─── نداء المعالِج عبر مُغلِّف الإغلاق: void(i64 حدث, i64 __env) ───
    // (AR) بروتوكول الإغلاق موحَّد على i64: مُغلِّفُ func-ref يُعلَن (i64,i64) ويُعيد
    //   inttoptr للوسيط الأوّل ليقرأ الحقول (كما في جسم المعالِج المولَّد: inttoptr ثمّ
    //   GEP على class.حدث). لذا نمرّر مؤشّر البنية كـi64 (ptrtoint) لا كـptr.
    llvm::Value* eventI64 = b.CreatePtrToInt(eventPtr, i64Ty, "event.i64");
    auto* handlerFt = llvm::FunctionType::get(voidTy, {i64Ty, i64Ty}, false);
    b.CreateCall(handlerFt, handlerFn, {eventI64, envI64});
    b.CreateRetVoid();
    if (savedBB) b.SetInsertPoint(savedBB, savedIP);
    return fn;
}

// (AR) ثانك حدثٍ صفريّ الأريّة: void thunk(void* data, void* pod) — يتجاهل pod ويستدعي
//   المعالِج بلا وسيطٍ صريح (نظير getOrCreateUiClosureThunk لكن بتوقيع SadEventCallback).
//   لازمٌ كي يطابق نوعُ مؤشّر الدالّة SadEventCallback حتّى للمعالِج الصفريّ (وقت التشغيل
//   يستدعي cb(data, &pod) دائمًا)، فلا نتّكل على تجاهل وسيطٍ زائد عبر اصطلاح cdecl.
[[nodiscard]] static llvm::Function* getOrCreateUiEventThunkZeroArg(LLVMCodeGen& cg) {
    llvm::Module* m = cg.getModule();
    if (auto* existing = m->getFunction("__sad_ui_event_thunk_void")) return existing;
    auto& ctx = *cg.context_;
    auto* voidTy = llvm::Type::getVoidTy(ctx);
    auto* i64Ty = llvm::Type::getInt64Ty(ctx);
    auto* ptrTy = llvm::PointerType::getUnqual(ctx);
    auto* ft = llvm::FunctionType::get(voidTy, {ptrTy, ptrTy}, false);
    auto* fn = llvm::Function::Create(ft, llvm::Function::InternalLinkage, "__sad_ui_event_thunk_void", m);
    fn->addFnAttr(llvm::Attribute::NoUnwind);
    llvm::IRBuilder<>& b = *cg.builder_;
    auto savedBB = b.GetInsertBlock();
    auto savedIP = savedBB ? b.GetInsertPoint() : llvm::BasicBlock::iterator();
    auto* entry = llvm::BasicBlock::Create(ctx, "entry", fn);
    b.SetInsertPoint(entry);
    llvm::Value* data = fn->getArg(0);
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

// (AR) يختار ثانك الحدث بحسب أريّة المعالِج الصريحة، المُبصومة في الواجهة الأماميّة
//   في comment=«ui-evt:N:الاسم» (N ∈ {0,1}). N≥1 ⇒ ثانك بنية «حدث»؛ 0 ⇒ الثانك
//   الصفريّ. لا نُعيد الحساب عبر getFunction (يكسره تصدير @رمز، ويلتبس __env اللامدا).
[[nodiscard]] static llvm::Function* selectUiEventThunk(LLVMCodeGen& cg, const std::string& comment) {
    unsigned sadArity = 0;
    const std::string prefix = "ui-evt:";
    if (comment.rfind(prefix, 0) == 0) {
        // (AR) الصيغة: ui-evt:<رقم>:<اسم>. نقرأ الرقم بين النقطتين.
        std::size_t p = prefix.size();
        std::size_t colon = comment.find(':', p);
        std::string numStr = comment.substr(p, colon == std::string::npos ? std::string::npos : colon - p);
        if (!numStr.empty() && numStr.find_first_not_of("0123456789") == std::string::npos)
            sadArity = static_cast<unsigned>(std::stoul(numStr));
    }
    return sadArity >= 1 ? getOrCreateUiEventThunk(cg)
                         : getOrCreateUiEventThunkZeroArg(cg);
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
// 21. نظام الواجهة الموحد / Unified UI System (sad_ui.h)
// =====================================================================
// (AR) كل دالة مصنع تُرجع SadWidget* — مؤشر خام (opaque pointer)
// (EN) Each factory returns SadWidget* — opaque pointer
// (AR) كل دالة ضبط خصائص ترجع void
// (EN) Each property setter returns void

// ─── 21a. مصانع العناصر / Widget Factories ───

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

// ─── 21b. إدارة الشجرة / Tree Management ───

llvm::Value* UICodeGen::emitUiAddChild(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* parent = coerceUiChildToPtr(cg_, cg_.resolveOperand(inst->operands[0]), ptrTy);
    llvm::Value* child = coerceUiChildToPtr(cg_, cg_.resolveOperand(inst->operands[1]), ptrTy);
    return emitUIRuntimeCall(cg_, "sad_add_child", voidTy, {ptrTy, ptrTy}, {parent, child});
}

llvm::Value* UICodeGen::emitUiRemoveChild(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* parent = coerceUiChildToPtr(cg_, cg_.resolveOperand(inst->operands[0]), ptrTy);
    llvm::Value* child = coerceUiChildToPtr(cg_, cg_.resolveOperand(inst->operands[1]), ptrTy);
    return emitUIRuntimeCall(cg_, "sad_remove_child", voidTy, {ptrTy, ptrTy}, {parent, child});
}

llvm::Value* UICodeGen::emitUiClearChildren(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = coerceUiChildToPtr(cg_, cg_.resolveOperand(inst->operands[0]), ptrTy);
    return emitUIRuntimeCall(cg_, "sad_clear_children", voidTy, {ptrTy}, {widget});
}

// ─── 21c. ضبط الخصائص / Property Setters ───

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
    // sad_set_border(widget, width) — optional color via separate call
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

// ─── 21d. إدارة التطبيق / App Management ───

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

// (AR) تشغيل_تطبيق(عنصر|دالّة) ⇒ حلقة سطح المكتب + الإرسال (جسر المكتبة). نداءٌ void:
//      لا يُخزَّن نتيجةٌ في namedValues (بخلاف مصانع العناصر التي تعيد مقبضًا).
//      (م1-ب) دالّة/إغلاق ⇒ نموذج البانِي (sad_app_run_builder): يُعاد استدعاؤه دوريًّا
//      (نبضةُ ثانية) فتُحدَّث المشاهد الزمنيّة حيًّا (الساعة)؛ عنصرٌ ⇒ لقطة ثابتة
//      (sad_app_run، توافقٌ خلفيّ). التمييزُ بنوع المعامل نظير emitUiNavigate تمامًا.
llvm::Value* UICodeGen::emitUiAppRun(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* root = cg_.resolveOperand(inst->operands[0]);
    auto tri = bridgeUiPageBuilder(cg_, root);
    if (tri.isBuilder) {
        auto* nullRel = llvm::ConstantPointerNull::get(ptrTy);
        return emitUIRuntimeCall(cg_, "sad_app_run_builder", voidTy,
            {ptrTy, ptrTy, ptrTy}, {tri.build, tri.data, nullRel});
    }
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

// (AR) التقط_مفتاح() ⇒ sad_next_key() يعيد int (رمز مفتاح منتظِر أو 0). i32 من
//      الرنتايم يُمَدّ إشارةً إلى i64 (نوع Integer في ص). لا وسائط.
llvm::Value* UICodeGen::emitUiNextKey(std::shared_ptr<SIRInstruction> inst) {
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* raw = emitUIRuntimeCall(cg_, "sad_next_key", i32Ty, {}, {});
    auto* result = cg_.builder_->CreateSExt(raw, i64Ty, "next_key.i64");
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

// ─── إكمال corui: الانتقال الكامل + الحالة + النافذة + توليد الويب ──────────────
// (إكمال) انتقل_بتحريك_كامل ⇒ sad_navigate_exit_transition(page, entry, exit, dur).
//   نظير emitUiNavigateTransition لكن بنوعَي انتقال (دخول+خروج). بانٍ/إغلاق ⇒ صيغة البانِي.
llvm::Value* UICodeGen::emitUiNavigateExitTransition(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* f32Ty = llvm::Type::getFloatTy(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* page = inst->operands.empty() ? llvm::ConstantPointerNull::get(ptrTy)
                                               : cg_.resolveOperand(inst->operands[0]);
    llvm::Value* entry = inst->operands.size() > 1 ? cg_.resolveOperand(inst->operands[1])
                                                   : llvm::ConstantPointerNull::get(ptrTy);
    if (!entry->getType()->isPointerTy()) entry = llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* exit = inst->operands.size() > 2 ? cg_.resolveOperand(inst->operands[2])
                                                  : llvm::ConstantPointerNull::get(ptrTy);
    if (!exit->getType()->isPointerTy()) exit = llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* dur = inst->operands.size() > 3 ? castNumericToF32(cg_, cg_.resolveOperand(inst->operands[3]))
                                                 : llvm::ConstantFP::get(f32Ty, kUiDefaultTransitionSec);
    // (م1-ج، HIGH-1) دالّة/إغلاق ⇒ بانٍ تفاعليّ؛ عنصر ⇒ لقطة (بلا هذا التفريع يُمرَّر
    //   إغلاق i64 لمُعامل ptr ⇒ فشل تحقّق LLVM؛ نظير emitUiNavigateTransition).
    auto tri = bridgeUiPageBuilder(cg_, page);
    if (tri.isBuilder) {
        auto* nullRel = llvm::ConstantPointerNull::get(ptrTy);
        return emitUIRuntimeCall(cg_, "sad_navigate_exit_transition_builder", voidTy,
            {ptrTy, ptrTy, ptrTy, ptrTy, ptrTy, f32Ty}, {tri.build, tri.data, nullRel, entry, exit, dur});
    }
    return emitUIRuntimeCall(cg_, "sad_navigate_exit_transition", voidTy,
        {ptrTy, ptrTy, ptrTy, f32Ty}, {page, entry, exit, dur});
}

// (إكمال) تحديث_حالة() ⇒ sad_update_state(): يعلّم مكدّس التنقّل dirty (إعادة رسم). بلا وسائط.
llvm::Value* UICodeGen::emitUiUpdateState(std::shared_ptr<SIRInstruction> /*inst*/) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitUIRuntimeCall(cg_, "sad_update_state", voidTy, {}, {});
}

// (rfcs#51) أوقف_الانتشار() ⇒ sad_stop_propagation(): يوقف انتشار الحدث الجاري. بلا وسائط.
llvm::Value* UICodeGen::emitUiStopPropagation(std::shared_ptr<SIRInstruction> /*inst*/) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitUIRuntimeCall(cg_, "sad_stop_propagation", voidTy, {}, {});
}

// (إكمال) عين_الحالة(دالّة؟) ⇒ نداء دالّة التحديث تزامنيًّا عبر ثانك الإغلاق ثمّ sad_update_state.
//   نظير المفسّر (يستدعي الدالّة أوّلًا ثمّ rebuildUI). المُعامل موجودٌ فقط إن كان دالّة (حرسه
//   الخافض)؛ غيابه ⇒ إعادة رسمٍ فقط. الثانك void(void* data) ينفّذ دالّة التحديث فورًا.
llvm::Value* UICodeGen::emitUiSetState(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    if (!inst->operands.empty()) {
        llvm::Value* fn = cg_.resolveOperand(inst->operands[0]);
        auto pair = bridgeUiCallback(cg_, fn); // {cb=ثانك الإغلاق (Function*)، data=مؤشّر الإغلاق}
        if (auto* thunk = llvm::dyn_cast_or_null<llvm::Function>(pair.cb))
            cg_.builder_->CreateCall(thunk, {pair.data}); // نداءٌ تزامنيّ لدالّة التحديث
    }
    return emitUIRuntimeCall(cg_, "sad_update_state", voidTy, {}, {});
}

// (إكمال) عنوان_النافذة(نص) ⇒ sad_set_window_title(title): جسرٌ فوق المتحكّم المشترك.
llvm::Value* UICodeGen::emitUiSetTitle(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* title = inst->operands.empty() ? llvm::ConstantPointerNull::get(ptrTy)
                                                : cg_.resolveOperand(inst->operands[0]);
    if (!title->getType()->isPointerTy()) title = llvm::ConstantPointerNull::get(ptrTy);
    return emitUIRuntimeCall(cg_, "sad_set_window_title", voidTy, {ptrTy}, {title});
}

// (إكمال) أغلق_النافذة() ⇒ sad_close_window(): جسرٌ فوق المتحكّم المشترك. بلا وسائط.
llvm::Value* UICodeGen::emitUiCloseWindow(std::shared_ptr<SIRInstruction> /*inst*/) {
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    return emitUIRuntimeCall(cg_, "sad_close_window", voidTy, {}, {});
}

// (إكمال) أبوابُ توليدِ كودِ المنصّة: توليد_ويب/أندرويد/آي_أو_إس/ماك(عنصر, اسم؟)
//   ⇒ sad_generate_*(root, name) → char* (String). أربعتُها جسمٌ واحدٌ لا يفترق إلّا
//   في اسمِ دالّةِ وقتِ التشغيل، وكلُّها تنتهي إلى مولّدِ المكتبةِ نفسِه الذي يستدعيه
//   المفسّر ⇒ تكافؤٌ بايتيّ.
//   بانٍ (دالّة تُرجع عنصرًا) ⇒ نبنيه أوّلًا لعنصرٍ عبر ثانك البانِي (نظير buildCurrent)
//   ثمّ نولّد؛ عنصر (لقطة) ⇒ يُمرَّر مباشرة ⇒ يقبل توليد_ويب(دالّة) كالمفسّر. النتيجة نصّ.
llvm::Value* UICodeGen::emitUiGenPlatform(std::shared_ptr<SIRInstruction> inst,
                                          const char* runtimeName) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    llvm::Value* root = inst->operands.empty() ? llvm::ConstantPointerNull::get(ptrTy)
                                               : cg_.resolveOperand(inst->operands[0]);
    auto tri = bridgeUiPageBuilder(cg_, root);
    if (tri.isBuilder) {
        // (AR) استدعِ ثانك البانِي ⇒ SadWidget ptr (يعيد مؤشّرًا فارغًا آمنًا إن أرجع البانِي لاشيء).
        auto* builderFn = llvm::cast<llvm::Function>(tri.build);
        root = cg_.builder_->CreateCall(builderFn, {tri.data}, "genweb.page");
    }
    if (!root->getType()->isPointerTy()) root = llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* title = inst->operands.size() > 1 ? cg_.resolveOperand(inst->operands[1])
                                                   : llvm::ConstantPointerNull::get(ptrTy);
    if (!title->getType()->isPointerTy()) title = llvm::ConstantPointerNull::get(ptrTy);
    auto* result = emitUIRuntimeCall(cg_, runtimeName, ptrTy, {ptrTy, ptrTy}, {root, title});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
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
// (AR) المصنعُ العامّ: العاملُ الأوّلُ ثابتٌ صحيحٌ = ترتيبُ العقدةِ في مصدرِ
//   الحقيقة، فيُمرَّر إلى sad_widget_create. أوپكودٌ واحدٌ يكفي فهرسَ العناصرِ كلَّه
//   بدلَ دالّةِ ABI وأوپكودٍ لكلِّ عنصر.
llvm::Value* UICodeGen::emitUiWidgetByType(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
    llvm::Value* nodeType = inst->operands.empty()
        ? llvm::ConstantInt::get(i32Ty, 0)
        : cg_.resolveOperand(inst->operands[0]);
    if (nodeType->getType() != i32Ty) {
        nodeType = cg_.builder_->CreateIntCast(nodeType, i32Ty, /*isSigned=*/true);
    }
    auto* result = emitUIRuntimeCall(cg_, "sad_widget_create", ptrTy, {i32Ty}, {nodeType});
    if (inst->result) cg_.context_info_.namedValues[inst->result->name] = result;
    return result;
}

llvm::Value* UICodeGen::emitUiSpinBox(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_spin_box");
}
llvm::Value* UICodeGen::emitUiGroupBox(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_group_box");
}
llvm::Value* UICodeGen::emitUiSpinner(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_spinner");
}
llvm::Value* UICodeGen::emitUiStatusBar(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_status_bar");
}
llvm::Value* UICodeGen::emitUiWindow(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_window");
}
llvm::Value* UICodeGen::emitUiTitleBar(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_title_bar");
}
llvm::Value* UICodeGen::emitUiScrollBar(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_scroll_bar");
}
llvm::Value* UICodeGen::emitUiTaskbar(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_taskbar");
}
llvm::Value* UICodeGen::emitUiStartMenu(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_start_menu");
}
llvm::Value* UICodeGen::emitUiSystemTray(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_system_tray");
}
llvm::Value* UICodeGen::emitUiMenu(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_menu");
}
llvm::Value* UICodeGen::emitUiToolbar(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_toolbar");
}
llvm::Value* UICodeGen::emitUiSegmentedControl(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_segmented_control");
}
llvm::Value* UICodeGen::emitUiTreeView(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_tree_view");
}
llvm::Value* UICodeGen::emitUiSplitView(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_split_view");
}
llvm::Value* UICodeGen::emitUiDataTable(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_data_table");
}
llvm::Value* UICodeGen::emitUiBreadcrumb(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_breadcrumb");
}
llvm::Value* UICodeGen::emitUiPicker(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_picker");
}
llvm::Value* UICodeGen::emitUiDatePicker(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_date_picker");
}
llvm::Value* UICodeGen::emitUiStepper(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_stepper");
}
llvm::Value* UICodeGen::emitUiRatingBar(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_rating_bar");
}
llvm::Value* UICodeGen::emitUiAlert(std::shared_ptr<SIRInstruction> inst) {
    return emitSimpleUiFactory(inst, "sad_alert");
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
    // (AR) القيمة قد تكون صحيحة (i64) أو عشريّة أو **ديناميّة (%SadDyn)** ⇒ تحويل آمن
    //      إلى double. الحالة الديناميّة تأتي من قيمةٍ نوعها يتقرّر زمنَ التشغيل (نتيجة
    //      `/`: صحيح/صحيح قد يكون صحيحًا أو عشريًّا)، وتُفَكّ بـunpackDouble (Float⇒bitcast،
    //      غيره⇒sitofp) نظير coerceFloatOperandToDouble في مسار الحساب. قبل هذا الحرس كان
    //      CreateFPCast يُطبَّق على هيكل %SadDyn فيسقط المصرّف بـ«Invalid cast».
    // (EN) The value may be i64, double, or a dynamic %SadDyn (a runtime-typed result such
    //      as `/`). Decode the dynamic case via unpackDouble, mirroring the arithmetic path;
    //      previously CreateFPCast on the struct crashed the compiler with "Invalid cast".
    llvm::Value* value = Sad::LLVM::isSadDyn(raw)
        ? Sad::LLVM::unpackDouble(cg_, raw)
        : (raw->getType()->isIntegerTy()
               ? cg_.builder_->CreateSIToFP(raw, f64Ty)
               : cg_.builder_->CreateFPCast(raw, f64Ty));
    return emitUIRuntimeCall(cg_, "sad_set_prop_num", voidTy,
        {ptrTy, ptrTy, f64Ty}, {widget, name, value});
}

// ════════════════════════════════════════════════════════════════════════════
// (AR) قيمةٌ لم يُحسَمْ نوعُها وقتَ الترجمة ⇒ نمرّرُ **الوسمَ والحمولةَ** ويحسمُ
//   وقتُ التشغيل، نظيرَ setIRPropertyFromValue في المفسّر حرفًا بحرف. كلُّ حسمٍ
//   ساكنٍ يُخطئ هنا: بالمفتاحِ وحدَه تُخفِقُ المفاتيحُ متعدّدةُ الأنواعِ في مصدرِ
//   الحقيقة («قيمة» = `عدد أو منطقيّ`)، والسقوطُ إلى STR يمرّرُ مؤشّرًا عدميًّا
//   فتُخزَّنُ الخاصّيّةُ نصًّا فارغًا: `ترقيم_صفحات(6 / 2)` كان يطبع «قيمة: ""»
//   بينما المفسّرُ يطبع «قيمة: 3».
//   الوسمُ والحمولةُ يُستخرَجانِ من %SadDyn مباشرةً (بلا فروعٍ ولا تخصيص)؛ وإن
//   وصلت قيمةٌ محدَّدةُ النوعِ عبرَ مسارٍ غيرِ معتادٍ نُغلّفُها بـtoDyn أوّلًا فلا
//   يسقطُ مدقِّقُ LLVM.
// (EN) Compile-time-undecided value ⇒ pass the %SadDyn kind+payload and let the
//   runtime decide, mirroring the interpreter's setIRPropertyFromValue exactly.
//   Every static guess is wrong here: key-based dispatch fails for polymorphic
//   keys, and the STR fallback substitutes a null pointer.
// ════════════════════════════════════════════════════════════════════════════
llvm::Value* UICodeGen::emitUiSetPropDyn(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* name = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* raw = cg_.resolveOperand(inst->operands[2]);
    // ════════════════════════════════════════════════════════════════════════
    // (AR) مقبضٌ مُبهَمٌ (عنصرُ واجهةٍ/كائنٌ/خريطة) **لا يُوسَمُ نصًّا**. الوسمُ العامُّ
    //   `toDyn` يُسقِطُ `Pointer` على `Str` — وهو صحيحٌ في مسارِ الحساب حيثُ المؤشّرُ
    //   نصٌّ فعلًا، وخاطئٌ هنا: `صندوق().لون(زر("س"))` كانت تطبعُ بايتاتِ كومةٍ خامّة
    //   لأنّ وقتَ التشغيلِ يفكُّ الحمولةَ `const char*`. الوسمُ الكائنيُّ يجعلُ الحقلَ
    //   يُترَكُ بلا كتابةٍ (دَينٌ مُعلَنٌ: المفسّرُ يُصيِّرُ الكائنَ نصًّا وليس لدى وقتِ
    //   التشغيلِ هنا مُصيِّرُ كائنات) بدل قراءةٍ خارجَ الحدّ.
    // (EN) An opaque handle must never be tagged Str: the generic toDyn maps Pointer
    //   to Str (right for arithmetic, wrong here), so the runtime decoded a widget
    //   handle as `const char*` and printed raw heap bytes. Tagging it as an object
    //   leaves the field unwritten (declared debt) instead of reading out of bounds.
    // ════════════════════════════════════════════════════════════════════════
    // ════════════════════════════════════════════════════════════════════════
    // (AR) الخريطةُ **تُصيَّرُ نصًّا** بدل أن تُترَكَ بلا كتابة: المفسّرُ يكتبُ
    //   «{أ: 1}» (صيغةُ `toString` غيرُ المقتبسة، لا صيغةُ الطباعةِ المقتبسة)،
    //   والمساعِدُ المولَّدُ نفسُه يُنتِجُها الآن. هذا هو الدَّينُ الذي أُعلِنَ في
    //   الدفعةِ السابقةِ مسدودًا لا مُبرَّرًا.
    // (EN) A map is now stringified rather than left unwritten: the interpreter
    //   writes the UNQUOTED toString spelling, and the generated helper produces it.
    // ════════════════════════════════════════════════════════════════════════
    if (!Sad::LLVM::isSadDyn(raw) && inst->operands[2].dataType == SadTypeKind::Map)
    {
        llvm::Value* mapPtr = raw;
        if (mapPtr->getType()->isIntegerTy())
            mapPtr = cg_.builder_->CreateIntToPtr(mapPtr, ptrTy, "prop.map.i2p");
        if (mapPtr->getType()->isPointerTy())
        {
            cg_.ensureMapToStringHelper(/*quoteKeys=*/false);
            llvm::FunctionCallee mapHelper = cg_.module_->getOrInsertFunction(
                ::Sad::Compiler::kMapToStringPlainFn, llvm::FunctionType::get(ptrTy, {ptrTy}, false));
            llvm::Value* mapText = cg_.builder_->CreateCall(mapHelper, {mapPtr}, "prop.map.str");
            llvm::Value* result = emitUIRuntimeCall(cg_, "sad_set_prop_str", voidTy,
                {ptrTy, ptrTy, ptrTy}, {widget, name, mapText});
            // (AR) المساعِدُ يُخصّص و`sad_set_prop_str` ينسخ ⇒ التحريرُ فورًا.
            cg_.emitFreeCall(mapText);
            return result;
        }
    }
    const bool opaqueHandle =
        !Sad::LLVM::isSadDyn(raw) &&
        Sad::Compiler::Frontend::UIProps::isOpaqueHandleKind(inst->operands[2].dataType);
    if (opaqueHandle)
    {
        llvm::Value* handleKind =
            llvm::ConstantInt::get(i8Ty, Sad::LLVM::DynKind::Obj);
        llvm::Value* handlePayload = raw->getType()->isPointerTy()
            ? cg_.builder_->CreatePtrToInt(raw, i64Ty, "prop.handle.i64")
            : cg_.builder_->CreateIntCast(raw, i64Ty, false);
        return emitUIRuntimeCall(cg_, "sad_set_prop_dyn", voidTy,
            {ptrTy, ptrTy, i8Ty, i64Ty}, {widget, name, handleKind, handlePayload});
    }
    llvm::Value* dyn = Sad::LLVM::isSadDyn(raw)
        ? raw
        : Sad::LLVM::toDyn(cg_, raw, inst->operands[2].dataType);
    llvm::Value* kind = Sad::LLVM::dynKindByte(cg_, dyn);
    llvm::Value* payload = Sad::LLVM::dynPayloadI64(cg_, dyn);
    return emitUIRuntimeCall(cg_, "sad_set_prop_dyn", voidTy,
        {ptrTy, ptrTy, i8Ty, i64Ty}, {widget, name, kind, payload});
}

// (AR) L2: ربط حدث عند_* — sad_add_event(widget, name, cb, data).
//      ردّ النداء وبياناته اختياريّان (يُستبدلان بمؤشّر فارغ عند الغياب)،
//      مثل onTap في مصنع الزرّ.
llvm::Value* UICodeGen::emitUiAddEvent(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* name = cg_.resolveOperand(inst->operands[1]);
    // (② rfcs#46) ردّ النداء = إغلاق (operands[2]). data = مؤشّر الإغلاق كما كان، لكنّ cb
    //   الآن جسرُ حدثٍ يطابق SadEventCallback = void(*)(void* data, const void* pod):
    //   بحسب أريّة المعالِج (من comment=«lambda:الاسم») نختار ثانك البنية (معالِج
    //   `دالة(حدث ح)` يتلقّى بنية «حدث» مبنيّة من POD) أو الثانك الصفريّ (يتجاهل POD).
    //   operands[3] القديم (userData) غير مستعمَل (data من الإغلاق).
    llvm::Value* closureVal = inst->operands.size() > 2 ? cg_.resolveOperand(inst->operands[2]) : nullptr;
    auto* nullp = llvm::ConstantPointerNull::get(ptrTy);
    llvm::Value* dataPtr = nullp;
    llvm::Value* cbFn = nullp;
    if (closureVal) {
        if (closureVal->getType()->isPointerTy())
            dataPtr = closureVal;
        else if (closureVal->getType()->isIntegerTy())
            dataPtr = cg_.builder_->CreateIntToPtr(closureVal, ptrTy, "cb.closure");
        if (dataPtr != nullp)
            cbFn = selectUiEventThunk(cg_, inst->comment);
    }
    return emitUIRuntimeCall(cg_, "sad_add_event", voidTy,
        {ptrTy, ptrTy, ptrTy, ptrTy}, {widget, name, cbFn, dataPtr});
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
llvm::Value* UICodeGen::emitUiSetEventPhase(std::shared_ptr<SIRInstruction> inst) {
    // (AR) .تفرع("فقاعة") ⇒ sad_set_event_phase(w, "فقاعة") — يعيّن طور **آخر**
    //      معالِجٍ سُجِّل على العقدة (نظير setLastEventPropagation في المفسّر).
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* phase = cg_.resolveOperand(inst->operands[1]);
    if (!phase->getType()->isPointerTy()) phase = llvm::ConstantPointerNull::get(ptrTy);
    return emitUIRuntimeCall(cg_, "sad_set_event_phase", voidTy,
        {ptrTy, ptrTy}, {widget, phase});
}
llvm::Value* UICodeGen::emitUiSetEventData(std::shared_ptr<SIRInstruction> inst) {
    // (AR) وسيطٌ ثالث لـ.عند_* ⇒ «بيانات» آخر معالِج (نظير setLastEventUserData).
    auto* ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
    auto* voidTy = llvm::Type::getVoidTy(*cg_.context_);
    llvm::Value* widget = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* text = cg_.resolveOperand(inst->operands[1]);
    if (!text->getType()->isPointerTy()) text = llvm::ConstantPointerNull::get(ptrTy);
    return emitUIRuntimeCall(cg_, "sad_set_event_data", voidTy,
        {ptrTy, ptrTy}, {widget, text});
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
        // ════════════════════════════════════════════════════════════════════
        // (AR) «حارسُ المؤشّرِ» هنا كان يحرسُ **النوعَ الخاطئ**: يسألُ «أهو مؤشّرٌ؟»
        //   ويُمرّرُ كلَّ مؤشّرٍ إلى `const char*`. والمقبضُ مؤشّرٌ أيضًا — فكان
        //   `صندوق().حدود(زر("س")، 3)` يقرأُ الكومةَ حتّى أوّلِ صفرٍ عارضٍ فيطبعُ
        //   بايتاتٍ **تختلفُ في كلِّ تشغيل** (وقد يعبرُ الصفحةَ فيُسقِطَ البرنامج).
        //   السؤالُ الصحيحُ عن **نوعِ SIR** لا عن تمثيلِ LLVM: النصُّ وحدَه يُمرَّرُ،
        //   وما عداه يُمرَّرُ عدمًا فيُدمَجُ فارغًا — غيابٌ مُعلَنٌ كنظيرِه في
        //   `sad_set_prop_dyn`، لا قراءةٌ خارجَ الحدّ.
        // (EN) The old "pointer guard" guarded the wrong thing — it asked whether the
        //   VALUE is a pointer and passed every pointer as `const char*`; a widget
        //   handle is a pointer too, so it read the heap until a stray NUL and printed
        //   per-run-varying bytes. Ask about the SIR TYPE instead: only String passes.
        // ════════════════════════════════════════════════════════════════════
        // (AR) والنوعُ غيرُ المحسومِ وقتَ الترجمةِ يُستثنى عمدًا فيبقى على سلوكِه:
        //   معامِلٌ بلا تصريحٍ يحملُ نصًّا حالةٌ واقعيّةٌ شائعة، وحملُه مقبضًا في
        //   خاصّيّةٍ مدموجةٍ بفواصلَ حالةٌ لا معنى لها. نُضيّقُ على **المقبضِ المحسومِ**
        //   ولا نكسرُ العاملَ — والتضييقُ الكاملُ يحتاجُ مُجمِّعًا يقبلُ الوسمَ والحمولةَ
        //   كـ`sad_set_prop_dyn`، وهو أوسعُ من هذه الدفعةِ فيُعلَن.
        // (EN) Undecided types keep their behaviour on purpose (an untyped parameter
        //   holding text is common; holding a handle in a comma-joined property is not).
        //   Full tightening needs a tag+payload accumulator — declared, not silently done.
        // (AR) والخريطةُ تُصيَّرُ نصًّا هنا أيضًا — نفسُ العلاجِ في نفسِ الطبقةِ لكِلا
        //   فرعَي المعدِّل (وسيطٌ واحد · وسائطُ متعدّدة)، فلا يعودُ أحدُهما يتباعدُ.
        if (valOp.dataType == SadTypeKind::Map)
        {
            llvm::Value* mapPtr = raw;
            if (mapPtr->getType()->isIntegerTy())
                mapPtr = cg_.builder_->CreateIntToPtr(mapPtr, ptrTy, "join.map.i2p");
            if (mapPtr->getType()->isPointerTy())
            {
                cg_.ensureMapToStringHelper(/*quoteKeys=*/false);
                llvm::FunctionCallee mapHelper = cg_.module_->getOrInsertFunction(
                    ::Sad::Compiler::kMapToStringPlainFn, llvm::FunctionType::get(ptrTy, {ptrTy}, false));
                llvm::Value* mapText = cg_.builder_->CreateCall(mapHelper, {mapPtr}, "join.map.str");
                llvm::Value* joined = emitUIRuntimeCall(cg_, "sad_prop_join_add_str", voidTy,
                    {ptrTy, ptrTy}, {widget, mapText});
                cg_.emitFreeCall(mapText); // المُجمِّعُ ينسخُ عندَ الإضافة
                return joined;
            }
        }
        // (AR) بلا حارسِ حجمٍ هنا: `operands[1]` مفكوكٌ أصلًا في رأسِ الدالّة، فحارسٌ
        //   بعدَه بستّةٍ وعشرينَ سطرًا يوحي بأمانٍ لا يوفّرُه.
        const bool textSafe =
            (Sad::Compiler::Frontend::UIProps::isTextSafeKind(valOp.dataType) ||
             Sad::Compiler::Frontend::UIProps::isUndecidedAtCompileTime(valOp.dataType));
        llvm::Value* s = (textSafe && raw->getType()->isPointerTy())
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
