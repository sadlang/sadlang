/*
 * ============================================================================
 * StringOpsCodeGen — توليد LLVM IR لعمليات النصوص
 * Phase 7 Step 6 — 17 methods استُخرِجت من 3 ملفات نصوص
 * ============================================================================
 */
#include "builders/collections/string_ops_codegen.h"
#include "llvm_codegen.h"

#include <llvm/IR/Function.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DerivedTypes.h>

#include "sir_constants.h"
#include "adt_payload_tags.h"
#include "builtin_registry.h" // (AR) أسماءُ طرائقِ النصِّ من السجلِّ المولَّد
#include "sad_dyn_repr.h" // (AR) ISSUE-063: dynToString لمعامل %SadDyn في السَلسلة / (EN) dynToString for a %SadDyn concat operand
#include "builders/collections/array_ops_codegen.h" // SAD_ARRAY_SLOT_BYTES

#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>

using namespace Sad::Compiler::SIR;

namespace Sad {
namespace LLVM {

// (AR) مخزنُ `حرف_من_رمز`: أربعةُ بايتاتٍ هي أقصى ترميزِ UTF-8 لنقطةِ ترميزٍ واحدة،
//      وخامسٌ للصفرِ الخاتم. ثابتٌ مسمًّى لأنّ الأربعةَ يعتمد عليها حسابُ الطولِ أدناه.
// (EN) The `حرف_من_رمز` buffer: four bytes is the maximum UTF-8 encoding of one code
//      point, plus one NUL terminator. Named because the length computation depends on it.
static constexpr unsigned kCharFromCodeMaxUtf8Bytes = 4;
static constexpr unsigned kCharFromCodeBufferBytes = kCharFromCodeMaxUtf8Bytes + 1;

static llvm::StructType *getArrayStructType(llvm::LLVMContext &ctx)
{
    llvm::StructType *st = llvm::StructType::getTypeByName(ctx, "SadArray");
    if (!st) {
        st = llvm::StructType::create(ctx, "SadArray");
        // (AR) التخطيط القانونيّ {طول، سعة، بيانات، وسوم} (كان مخالفَ الترتيب — مُيّت لأنّ
        //      getTypeByName يجد القانونيّ أوّلًا؛ نوحّده احتياطًا). / (EN) Canonical layout
        //      {len,cap,data,tags} (was divergent order — dead since getTypeByName finds the
        //      canonical first; unified defensively).
        st->setBody({llvm::Type::getInt64Ty(ctx), llvm::Type::getInt64Ty(ctx),
                     llvm::PointerType::getUnqual(ctx), llvm::PointerType::getUnqual(ctx),
                     llvm::Type::getInt8Ty(ctx)}); // homogKind (option A2): DynKind of a homogeneous array; read only when tags==null
    }
    return st;
}

// ============================================================================
// (AR) مُنشئ دالّة مساعِدة ذاتيّة الاحتواء لمدمج «تقسيم» — نمط الوضع الحرّ نفسه
//      (WeakODR + no-builtins + NoInline + OptimizeNone): تُصدَر مرّة واحدة
//      وتعمل مستضافةً (تُدمَج مع libc) وحرّةً (تُدمَج مع بدائيّات __sad) دون
//      تضارب. يعيد nullptr إن كانت الدالّة معرَّفة أصلًا (منع إعادة الإصدار).
// (EN) Self-contained helper builder for the split builtin, same shape as the
//      freestanding primitives (WeakODR/no-builtins/noinline/optnone): emitted
//      once, safe hosted and freestanding. Returns nullptr if already defined.
// ============================================================================
static llvm::Function *getOrCreateSplitHelper(
    llvm::Module *mod, llvm::LLVMContext &ctx,
    const std::string &name, llvm::FunctionType *ft)
{
    if (llvm::Function *existing = mod->getFunction(name))
        if (!existing->isDeclaration())
            return nullptr; // معرَّفة أصلًا / already has a body
    llvm::Function *fn = mod->getFunction(name);
    if (!fn)
        fn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, name, mod);
    fn->setLinkage(llvm::Function::WeakODRLinkage);
    fn->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
    fn->addFnAttr("no-builtins");
    fn->addFnAttr(llvm::Attribute::NoInline);
    fn->addFnAttr(llvm::Attribute::OptimizeNone);
    return fn;
}


        // ============================================================================
        // (AR) عمليات النصوص: emitStringConcat, emitStringCharAt, emitStringCmp,
        //      cg_.emitInlineAsm, cg_.ensureArrayToStringHelper
        // (EN) String operations: concat, charAt, compare, inline asm, array-to-string
        // (AR) تم فصله من llvm_codegen_output.cpp وفق CW-05
        // ============================================================================
        llvm::Value *StringOpsCodeGen::emitStringConcat(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "STRING_CONCAT"}});
                return nullptr;
            }

            llvm::Value *left = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *right = cg_.resolveOperand(inst->operands[1]);

            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_SIR_OPERAND_RESOLVE, {{"detail", "STRING_CONCAT"}});
                return nullptr;
            }

            // Convert integer/float to string if needed
            llvm::Type *leftTy = left->getType();
            llvm::Type *rightTy = right->getType();

            // (AR) توليد دالة __sad_array_to_string إذا كان أحد المعاملين مصفوفة
            // (EN) Generate __sad_array_to_string helper if one operand is an array
            if (inst->operands[0].dataType == SadTypeKind::Array ||
                inst->operands[1].dataType == SadTypeKind::Array)
            {
                cg_.ensureArrayToStringHelper();
            }

            // Ensure both operands are string pointers
            // If one is an integer, convert it to string using sprintf (or __sad_itoa in freestanding)
            auto ensureString = [&](llvm::Value *val, llvm::Type *ty, const SIROperand &op) -> llvm::Value *
            {
                // (AR) ISSUE-063: قيمةٌ ديناميّة %SadDyn ⇒ الموزِّع dynToString (يفحص وسم
                //      النوع ويطابق المفسّر لكلّ نوع). كانت تسقط من كلّ الفروع أدناه (ليست
                //      مؤشّرًا ولا i64 ولا double) إلى مخزنٍ غير مهيَّأ ⇒ «نص + ديناميّ»
                //      يُلحق فراغًا/قمامة.
                // (EN) ISSUE-063: a dynamic %SadDyn value ⇒ the dynToString dispatcher
                //      (inspects the kind tag, matching the interpreter per type). It used
                //      to fall through every branch below (neither pointer, i64 nor double)
                //      into an uninitialized buffer ⇒ "string + dynamic" appended
                //      emptiness/garbage.
                if (isSadDyn(val))
                    return dynToString(cg_, val);

                // ════════════════════════════════════════════════════════════
                // (AR) الخريطةُ في وصلٍ نصّيّ: كانت تسقطُ إلى المسارِ العدديِّ فيُلحَقُ
                //   **عنوانُ المؤشّرِ رقمًا** — `"س" + {"أ": 1}` تُعطي «س1825978152560»
                //   بينما المفسّرُ يعطي «س{أ: 1}». نظيرُ ذراعِ المصفوفةِ أدناه تمامًا،
                //   وبالصيغةِ غيرِ المقتبسةِ (`toString`) لا صيغةِ الطباعةِ المقتبسة.
                // (EN) A map in a string concat used to fall to the integer path and append
                //   the pointer as a number. Mirrors the array arm below, unquoted spelling.
                // ════════════════════════════════════════════════════════════
                if (op.dataType == SadTypeKind::Map)
                {
                    auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
                    llvm::Value *mapPtr = val;
                    if (ty->isIntegerTy(64))
                        mapPtr = cg_.builder_->CreateIntToPtr(mapPtr, ptrTy, "map.i2p");
                    if (mapPtr->getType()->isPointerTy())
                    {
                        cg_.ensureMapToStringHelper(/*quoteKeys=*/false);
                        llvm::FunctionCallee mapHelper = cg_.module_->getOrInsertFunction(
                            ::Sad::Compiler::kMapToStringPlainFn,
                            llvm::FunctionType::get(ptrTy, {ptrTy}, false));
                        return cg_.builder_->CreateCall(mapHelper, {mapPtr}, "cat.map.str");
                    }
                }

                // (AR) تحويل المصفوفة إلى نص: "[عنصر1، عنصر2، ...]"
                // (EN) Convert array to string representation
                if (op.dataType == SadTypeKind::Array)
                {
                    // (AR) إذا كانت القيمة i64 (مؤشر مُخزّن كعدد صحيح)، نحوّلها إلى مؤشر أولاً
                    // (EN) If value is i64 (pointer stored as integer), convert to pointer first
                    if (ty->isIntegerTy(64))
                    {
                        val = cg_.builder_->CreateIntToPtr(val,
                                                       llvm::PointerType::getUnqual(*cg_.context_), "arr.i2p");
                        ty = val->getType();
                    }
                    if (ty->isPointerTy())
                    {
                        // (AR) قراءة طول المصفوفة من هيكل SadArray {i64 length, i64 capacity, ptr data}
                        // (EN) Read array length from SadArray struct {i64 length, i64 capacity, ptr data}
                        llvm::StructType *arrTy = llvm::StructType::getTypeByName(*cg_.context_, "SadArray");
                        if (!arrTy)
                        {
                            arrTy = llvm::StructType::create(*cg_.context_, {
                                                                            llvm::Type::getInt64Ty(*cg_.context_),       // length
                                                                            llvm::Type::getInt64Ty(*cg_.context_),       // capacity
                                                                            llvm::PointerType::getUnqual(*cg_.context_),  // data
                                                                            llvm::PointerType::getUnqual(*cg_.context_),  // tags (option A)
                                                                            llvm::Type::getInt8Ty(*cg_.context_)          // homogKind (option A2): DynKind of a homogeneous array; read only when tags==null
                                                                        },
                                                             "SadArray");
                        }

                        auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
                        auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

                        // ════════════════════════════════════════════════════════
                        // (AR) 🔑 حارسُ المصفوفةِ العدمِ — **عند المُنادي لا في المُساعِد**
                        //
                        //      وُضِع الحارسُ أوّلًا داخلَ مساعِداتِ التحويلِ الأربعةِ
                        //      (نظيرَ حارسِ الخريطةِ) وبُنِي، فبقي الانهيارُ كما هو:
                        //      لأنّ العطبَ **هنا** — `CreateStructGEP` + `CreateLoad`
                        //      على مؤشّرِ المصفوفةِ يقعان **قبل** بلوغِ المُساعِد.
                        //      فالحارسُ كان صحيحًا في ذاتِه وفي الطبقةِ الخطأ.
                        //
                        //      🔑 ودرسُه هو درسُ المقارنةِ نفسُه في الجلسةِ نفسِها:
                        //      **رقعةٌ في طبقةٍ لا يمرُّ بها العطبُ تُبنى وتُصرَّف ولا
                        //      تفعل شيئًا** — وتبدو صحيحةً في المراجعة. والقياسُ وحدَه
                        //      يفرّق بين «سُدَّ» و«كُتِبت رقعةٌ».
                        //
                        //      والصيغةُ: يُبدَّل المؤشّرُ العدمُ ببنيةٍ عامّةٍ مصفَّرةٍ
                        //      فتَسلم القراءات (طول=٠ · بيانات=عدم)، ثمّ يُنتقى المخرَجُ
                        //      من الشرطِ الأصليّ. والقرارُ على **مؤشّرِ المصفوفةِ** لا
                        //      على بياناتِها: مصفوفةٌ فارغةٌ مُهيّأةٌ (`مصفوفة س = []`)
                        //      حيّةٌ ويجب أن تبقى «[]» لا «لاشيء».
                        // (EN) Null-array guard AT THE CALLER, not in the helper. The guard
                        //      was first added inside the four to-string helpers and the crash
                        //      persisted: the fault is here — the StructGEP+Load on the array
                        //      pointer happen BEFORE the helper is reached. A patch in a layer
                        //      the defect never passes through compiles and does nothing.
                        //      The decision is on the ARRAY pointer, not its data, so an
                        //      initialised empty array still prints «[]».
                        // ════════════════════════════════════════════════════════
                        // ════════════════════════════════════════════════════════
                        // (AR) 🔑 **وهذه النسخةُ كانت تفحص وجهًا واحدًا**: المؤشّرَ
                        //      الصفريَّ وحدَه، بلا الحارسِ في خانةِ المؤشّر. فقِيس أنّ
                        //      `"ق: " + مصفوفة_عارية` يطبع «لاشيء» بينما
                        //      `مصفوفة عدمية س = لاشيء` ثمّ ضمُّها **ينهار** —
                        //      نسخةٌ رابعةٌ أُسقِط نصفُ فحصِها، والنصفُ الباقي يجعلها
                        //      تبدو محروسةً في المراجعةِ وفي نصفِ القياس.
                        //      فيُنادى البابُ الواحدُ بدلَها.
                        // (EN) This copy tested only ONE shape (the zero pointer), so a
                        //      concat of `مصفوفة عدمية س = لاشيء` crashed while the bare
                        //      declaration printed «لاشيء» — half a test looks guarded in
                        //      review and in half the measurements. Call the single door.
                        // ════════════════════════════════════════════════════════
                        auto arrGuard = cg_.emitContainerNullGuard(
                            val, arrTy, "__sad_null_array_placeholder", "arr.str");
                        llvm::Value *arrIsNull = arrGuard.isNull;
                        val = arrGuard.safePtr;

                        // Load length
                        llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, val, 0, "arr.str.len.gep");
                        llvm::Value *arrLen = cg_.builder_->CreateLoad(i64Ty, lenGep, "arr.str.len");

                        // Load data pointer
                        llvm::Value *dataGep = cg_.builder_->CreateStructGEP(arrTy, val, 2, "arr.str.data.gep");
                        llvm::Value *dataPtr = cg_.builder_->CreateLoad(ptrTy, dataGep, "arr.str.data");

                        // (AR) [عناصر موسومة — option A] مصفوفةٌ مختلطةٌ قياسيّة في سياق دمجٍ
                        //      نصّيّ (نصّ + مصفوفة): الخانات مؤشّرات صناديق %SadDyn ⇒ نستعمل
                        //      المساعِد الموسوم (dynToString لكلّ عنصر) بدل المسار العدديّ.
                        // (EN) [boxed elements] a scalar-heterogeneous array in a string-concat
                        //      context: slots are %SadDyn box pointers ⇒ use the boxed helper
                        //      (per-element dynToString) instead of the integer path.
                        if (op.elementType == SadTypeKind::Any)
                        {
                            // (AR) [وسم زمن-تشغيل] مرّر مخزنَ الوسوم (الحقل ٣) مع البيانات.
                            // (EN) [runtime tag] pass the tags buffer (field 3) with the data.
                            llvm::Value *tagsGep = cg_.builder_->CreateStructGEP(arrTy, val, 3, "arr.str.tags.gep");
                            llvm::Value *tagsPtr = cg_.builder_->CreateLoad(ptrTy, tagsGep, "arr.str.tags");
                            cg_.ensureArrayToStringDynHelper();
                            llvm::FunctionType *dTy = llvm::FunctionType::get(ptrTy, {i64Ty, ptrTy, ptrTy}, false);
                            llvm::FunctionCallee dFn = cg_.module_->getOrInsertFunction("__sad_array_to_string_dyn", dTy);
                            llvm::Value *dynText =
                                cg_.builder_->CreateCall(dFn, {arrLen, dataPtr, tagsPtr}, "arr.str.dyn");
                            // (AR) القرارُ من مؤشّرِ المصفوفةِ لا من بياناتِها (انظر الحارسَ أعلاه).
                            return cg_.builder_->CreateSelect(
                                arrIsNull, cg_.emitSafeStringPtr(nullptr, "arr.str.dyn.null"), dynText,
                                "arr.str.dyn.result");
                        }

                        // (AR) تخصيص مخزن كبير كافٍ: "[" + (كل عنصر حتى 32 حرف + ", ") * الطول + "]" + '\0'
                        // (EN) Allocate sufficient buffer: "[" + (each elem up to 32 chars + ", ") * len + "]" + '\0'
                        llvm::Value *bufLen = cg_.builder_->CreateAdd(
                            cg_.builder_->CreateMul(arrLen, llvm::ConstantInt::get(i64Ty, 34), "arr.str.elmsz"),
                            llvm::ConstantInt::get(i64Ty, 4), "arr.str.bufsz");

                        llvm::Value *buf = cg_.emitMalloc(bufLen, "arr.str.buf");

                        // (AR) استدعاء __sad_array_to_string_impl(buf, arrLen, dataPtr) -> i32 (عدد الأحرف المكتوبة)
                        // (EN) Call __sad_array_to_string_impl(buf, arrLen, dataPtr) -> i32 (chars written)
                        // We generate this inline: write "[", then loop over elements, write "]"

                        // For simplicity, use sprintf to format: "[%lld, %lld, ...]"
                        // Start with "["
                        llvm::FunctionType *sprintfType = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*cg_.context_), {ptrTy, ptrTy}, true);
                        llvm::FunctionCallee sprintfFn = cg_.module_->getOrInsertFunction("sprintf", sprintfType);

                        // (AR) بناء النص التمثيلي عبر دالة مساعدة مُولَّدة في LLVM IR
                        // (EN) Build string representation via helper function generated in LLVM IR
                        llvm::FunctionType *helperType = llvm::FunctionType::get(ptrTy, {ptrTy, i64Ty, ptrTy}, false);
                        llvm::FunctionCallee helperFn = cg_.module_->getOrInsertFunction("__sad_array_to_string", helperType);
                        llvm::Value *result = cg_.builder_->CreateCall(helperFn, {buf, arrLen, dataPtr}, "arr.str.result");
                        // (AR) القرارُ من مؤشّرِ المصفوفةِ لا من بياناتِها — فمصفوفةٌ فارغةٌ
                        //      مُهيّأةٌ تبقى «[]» ولا تنقلب «لاشيء».
                        return cg_.builder_->CreateSelect(
                            arrIsNull, cg_.emitSafeStringPtr(nullptr, "arr.str.null"), result,
                            "arr.str.result.sel");
                    } // end if (ty->isPointerTy())
                } // end if (op.dataType == SadTypeKind::Array)

                // (AR) معالجة لاشيء (null) — إذا كان النوع Pointer والقيمة null pointer
                //      نُرجع نص "void" كما يفعل المفسر
                //      بدون هذا الفحص، تمرير null pointer إلى strlen يسبب crash
                // (EN) Handle null (لاشيء) — if type is Pointer and value is null pointer
                //      return "void" string as the interpreter does
                //      Without this check, passing null pointer to strlen causes crash
                if (op.dataType == SadTypeKind::Pointer)
                {
                    // (AR) 🔑 كان يُنتِج «void» حرفيًّا بدعوى «كما يفعل المفسّر» — والمفسّرُ
                    //      يطبع «لاشيء» (مقيس). لفظٌ إنﭽليزيٌّ مكتوبٌ في الشيفرةِ يزعم
                    //      مطابقةَ محرّكٍ لا يطابقه. اللفظُ الآن من مصدرِ الحقيقةِ لا من هنا.
                    // (EN) This emitted a hardcoded "void" claiming interpreter parity; the
                    //      interpreter prints «لاشيء» (measured). Now read from SoT.
                    return cg_.emitSafeStringPtr(nullptr, "cat.null");
                }

                if (ty->isPointerTy())
                {
                    // (AR) 🔑 كان هنا فحصُ `nullptr` **وحدَه** — وحارسُ العدمِ ليس صفرًا،
                    //      فيمرُّ خامًّا إلى `strlen` ⇒ انهيار. مقيس: «اطبع_سطر("قيمة: " + س)»
                    //      حيث «س» خانةٌ نصّيّةٌ تحمل العدم: انهيارُ وصولٍ (0xC0000005)
                    //      بينما المفسّرُ يطبع «قيمة: لاشيء». والخانةُ تصلُ **مؤشّرًا**
                    //      لا i64، فحراسةُ الفرعِ العدديِّ وحدَها لم تكن تبلغها.
                    // (EN) This checked ONLY nullptr; the null sentinel is not zero, so it
                    //      reached strlen raw ⇒ crash (measured). The slot arrives as a
                    //      pointer, not a boxed i64, so guarding the integer branch missed it.
                    return cg_.emitSafeStringPtr(val, "cat.str");
                }
                // (AR) إذا كان نوع المعامل نصاً لكن القيمة i64 (مؤشر مُخزّن كعدد صحيح)
                //      نحوّل من i64 إلى مؤشر — يحدث مع النصوص المنسقة (f-strings) والمتغيرات النصية
                // (EN) If operand type is String but LLVM value is i64 (boxed string pointer),
                //      convert from i64 to pointer — happens with f-strings and string variables
                if (op.dataType == SadTypeKind::String && ty->isIntegerTy(64))
                {
                    // (AR) 🔑 عبر البابِ الواحد: الخانةُ نوعُها «نص» لكنّها قد تحمل حارسَ
                    //      العدم، و`inttoptr` الخامُّ كان يُسلّمه إلى strlen ⇒ انهيار
                    //      (مقيس: «اطبع("قيمة: " + س)» والمفسّرُ يطبع «قيمة: لاشيء»).
                    // (EN) Through the single door: this String slot may carry the null
                    //      sentinel; the raw inttoptr used to hand it to strlen ⇒ crash.
                    return cg_.emitSafeStringPtr(val, "str.unbox");
                }
                // (AR) القيم المنطقية: ارجع "صحيح"/"خطأ" مباشرة بدلاً من تحويل الرقم
                // (EN) Boolean values: return "صحيح"/"خطأ" directly instead of converting the number
                if (op.dataType == SadTypeKind::Boolean)
                {
                    llvm::Value *trueStr = cg_.builder_->CreateGlobalStringPtr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "bool.true");
                    llvm::Value *falseStr = cg_.builder_->CreateGlobalStringPtr("\xd8\xae\xd8\xb7\xd8\xa3", "bool.false");
                    llvm::Value *cond;
                    if (val->getType()->isIntegerTy(1))
                    {
                        cond = val;
                    }
                    else if (val->getType()->isPointerTy())
                    {
                        // (AR) [Fix BF-04] المؤشرات تُقارن بـ null وليس بـ 0
                        // (EN) [Fix BF-04] Pointers must be compared to null, not integer 0
                        cond = cg_.builder_->CreateICmpNE(
                            val,
                            llvm::ConstantPointerNull::get(
                                llvm::cast<llvm::PointerType>(val->getType())),
                            "bool.cmp.ptr");
                    }
                    else
                    {
                        cond = cg_.builder_->CreateICmpNE(val, llvm::ConstantInt::get(val->getType(), 0), "bool.cmp");
                    }
                    return cg_.builder_->CreateSelect(cond, trueStr, falseStr, "bool.str");
                }
                // Need to convert integer/float to string
                // (AR) === فحص ADT pointer tagging ===
                //      إذا كانت القيمة i64 من ENUM_GET_PAYLOAD وتحمل flag __is_ptr
                //      فنفحص وقت التشغيل: إذا كانت مؤشر (نص) → inttoptr → strlen
                //      وإلا → sprintf كرقم عادي
                // (EN) === ADT pointer tagging check ===
                //      If value is i64 from ENUM_GET_PAYLOAD with __is_ptr flag
                //      runtime check: if pointer (string) → inttoptr → strlen
                //      else → sprintf as regular number
                if (ty->isIntegerTy(64) && !op.name.empty())
                {
                    // ================================================================
                    // (AR) [إصلاح قنوات] فك وسم MSB 2-bit للقيم المستقبَلة من القنوات
                    //      نوع Any يعني أن القيمة موسومة:
                    //      bit63=0 → مؤشر (نص)
                    //      bit63=1, bit62=0 → رقم — مسح bit63 ثم sprintf
                    //      bit63=1, bit62=1 → منطقي — مسح bit63+62 ثم صحيح/خطأ
                    //      kSadNullSentinel → "لاشيء"
                    // (EN) [Channel fix] Decode MSB 2-bit for channel-received values in concat
                    // ================================================================
                    if (op.dataType == SadTypeKind::Any)
                    {
                        auto *i64Ty_l = llvm::Type::getInt64Ty(*cg_.context_);
                        auto *ptrTy_l = llvm::PointerType::getUnqual(*cg_.context_);

                        // (AR) فحص sentinel (لاشيء) أولاً
                        llvm::Value *isNullSentinel = cg_.builder_->CreateICmpEQ(
                            val, llvm::ConstantInt::get(i64Ty_l, Sad::Compiler::kSadNullSentinel),
                            "any.concat.null");

                        auto *parentFunc = cg_.builder_->GetInsertBlock()->getParent();
                        auto *nullBB = llvm::BasicBlock::Create(*cg_.context_, "any.c.null", parentFunc);
                        auto *checkBB = llvm::BasicBlock::Create(*cg_.context_, "any.c.check", parentFunc);
                        auto *ptrOrFloatBB = llvm::BasicBlock::Create(*cg_.context_, "any.c.pof", parentFunc);
                        auto *ptrBB_l = llvm::BasicBlock::Create(*cg_.context_, "any.c.ptr", parentFunc);
                        auto *floatBB_l = llvm::BasicBlock::Create(*cg_.context_, "any.c.float", parentFunc);
                        auto *intOrBoolBB = llvm::BasicBlock::Create(*cg_.context_, "any.c.iob", parentFunc);
                        auto *boolBB_l = llvm::BasicBlock::Create(*cg_.context_, "any.c.bool", parentFunc);
                        auto *intBB_l = llvm::BasicBlock::Create(*cg_.context_, "any.c.int", parentFunc);
                        auto *mergeBB = llvm::BasicBlock::Create(*cg_.context_, "any.c.merge", parentFunc);

                        cg_.builder_->CreateCondBr(isNullSentinel, nullBB, checkBB);

                        // (AR) لاشيء
                        cg_.builder_->SetInsertPoint(nullBB);
                        llvm::Value *nullStr = cg_.builder_->CreateGlobalStringPtr(
                            "\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1", "any.c.nullstr"); // لاشيء
                        cg_.builder_->CreateBr(mergeBB);

                        // (AR) فحص bit63: مصفّر ⇒ مؤشّر/عشريّ (00/01)؛ مضبوط ⇒ صحيح/منطقيّ (10/11)
                        // (EN) Check bit63: clear ⇒ pointer/float (00/01); set ⇒ int/bool (10/11)
                        cg_.builder_->SetInsertPoint(checkBB);
                        llvm::Value *bit63Mask = llvm::ConstantInt::get(i64Ty_l, kAdtPayloadBit63);
                        llvm::Value *bit63 = cg_.builder_->CreateAnd(val, bit63Mask, "any.c.bit63");
                        llvm::Value *hiClear = cg_.builder_->CreateICmpEQ(
                            bit63, llvm::ConstantInt::get(i64Ty_l, 0), "any.c.hiclear");
                        cg_.builder_->CreateCondBr(hiClear, ptrOrFloatBB, intOrBoolBB);

                        // (AR) ISSUE-076/084: تمييز النصّ (00) عن الصندوق العشريّ (01) عبر bit62
                        // (EN) ISSUE-076/084: distinguish string (00) from boxed float (01) via bit62
                        cg_.builder_->SetInsertPoint(ptrOrFloatBB);
                        llvm::Value *bit62pf = cg_.builder_->CreateAnd(
                            val, llvm::ConstantInt::get(i64Ty_l, kAdtPayloadBit62), "any.c.bit62pf");
                        llvm::Value *isFloatPf = cg_.builder_->CreateICmpNE(
                            bit62pf, llvm::ConstantInt::get(i64Ty_l, 0), "any.c.isfloat");
                        cg_.builder_->CreateCondBr(isFloatPf, floatBB_l, ptrBB_l);

                        // (AR) مؤشر (نص)
                        cg_.builder_->SetInsertPoint(ptrBB_l);
                        // (AR) لفظُ «void» الإنﭽليزيُّ كان مكتوبًا هنا يدًا ويفحص `nullptr` وحدَه، فلا
                        //      يرى حارسَ العدم (‎0x8000000000000001‎) ويزعم مطابقةَ محرّكٍ لا
                        //      يطابقه: المفسّرُ يطبع «لاشيء». عبرَ بابِ العرضِ الآن — من SoT.
                        // (EN) The hand-written English "void" checked nullptr only, missing the
                        //      sentinel, and claimed a parity the interpreter never had.
                        llvm::Value *safeStr = cg_.emitSafeStringPtr(val, "any.c");
                        cg_.builder_->CreateBr(mergeBB);

                        // (AR) صندوق عشريّ (01): امسح bit62 ⇒ مؤشّر ⇒ حمّل double ⇒ نُنسّقه
                        //      بـ__sad_format_double (نفس دقّة المفسّر) في مخزن مكدّس. ISSUE-076/084.
                        // (EN) Boxed float (01): clear bit62 ⇒ pointer ⇒ load double ⇒ format it via
                        //      __sad_format_double (interpreter precision) into a stack buffer.
                        cg_.builder_->SetInsertPoint(floatBB_l);
                        llvm::Value *fboxI64 = cg_.builder_->CreateAnd(
                            val, llvm::ConstantInt::get(i64Ty_l, ~kAdtPayloadBit62), "any.c.fclear");
                        llvm::Value *fboxPtr = cg_.builder_->CreateIntToPtr(fboxI64, ptrTy_l, "any.c.fptr");
                        llvm::Value *fdbl = cg_.builder_->CreateLoad(
                            llvm::Type::getDoubleTy(*cg_.context_), fboxPtr, "any.c.fload");
                        // (AR) ISSUE-076 (Amelia #8): 512 لا 32 — %.6f لـDBL_MAX ~316 حرفًا يفيض.
                        // (EN) ISSUE-076 (Amelia #8): 512 not 32 — %.6f for DBL_MAX ~316 chars overflows.
                        llvm::Value *fbuf = cg_.builder_->CreateAlloca(
                            llvm::Type::getInt8Ty(*cg_.context_),
                            llvm::ConstantInt::get(i64Ty_l, 512), "any.c.fbuf");
                        auto *fmtDblType = llvm::FunctionType::get(
                            llvm::Type::getVoidTy(*cg_.context_),
                            {ptrTy_l, llvm::Type::getDoubleTy(*cg_.context_)}, false);
                        auto fmtDblFn = cg_.module_->getOrInsertFunction("__sad_format_double", fmtDblType);
                        cg_.builder_->CreateCall(fmtDblFn, {fbuf, fdbl});
                        cg_.builder_->CreateBr(mergeBB);

                        // (AR) فحص bit62 — منطقي أم رقم
                        cg_.builder_->SetInsertPoint(intOrBoolBB);
                        llvm::Value *bit62Mask = llvm::ConstantInt::get(i64Ty_l, 1ULL << 62);
                        llvm::Value *bit62 = cg_.builder_->CreateAnd(val, bit62Mask, "any.c.bit62");
                        llvm::Value *isBool = cg_.builder_->CreateICmpNE(
                            bit62, llvm::ConstantInt::get(i64Ty_l, 0), "any.c.isbool");
                        cg_.builder_->CreateCondBr(isBool, boolBB_l, intBB_l);

                        // (AR) منطقي
                        cg_.builder_->SetInsertPoint(boolBB_l);
                        llvm::Value *clearMaskB = llvm::ConstantInt::get(i64Ty_l, ~(3ULL << 62));
                        llvm::Value *cleanBool = cg_.builder_->CreateAnd(val, clearMaskB, "any.c.bclean");
                        llvm::Value *trueStr = cg_.builder_->CreateGlobalStringPtr(
                            "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "any.c.true");
                        llvm::Value *falseStr = cg_.builder_->CreateGlobalStringPtr(
                            "\xd8\xae\xd8\xb7\xd8\xa3", "any.c.false");
                        llvm::Value *boolCond = cg_.builder_->CreateICmpNE(
                            cleanBool, llvm::ConstantInt::get(i64Ty_l, 0), "any.c.bcond");
                        llvm::Value *boolStr = cg_.builder_->CreateSelect(boolCond, trueStr, falseStr, "any.c.bstr");
                        cg_.builder_->CreateBr(mergeBB);

                        // (AR) رقم — مسح bit63 ثم sprintf
                        cg_.builder_->SetInsertPoint(intBB_l);
                        llvm::Value *clearBit63 = llvm::ConstantInt::get(i64Ty_l, ~(1ULL << 63));
                        llvm::Value *cleanInt = cg_.builder_->CreateAnd(val, clearBit63, "any.c.iclean");
                        llvm::Value *numBuf = cg_.builder_->CreateAlloca(
                            llvm::Type::getInt8Ty(*cg_.context_),
                            llvm::ConstantInt::get(i64Ty_l, 32), "any.c.nbuf");
                        llvm::FunctionType *sprintfTy = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*cg_.context_),
                            {ptrTy_l, ptrTy_l}, true);
                        llvm::FunctionCallee sprintfFunc = cg_.module_->getOrInsertFunction("sprintf", sprintfTy);
                        llvm::Value *intFmt = cg_.builder_->CreateGlobalStringPtr("%lld", "any.c.ifmt");
                        cg_.builder_->CreateCall(sprintfFunc, {numBuf, intFmt, cleanInt});
                        cg_.builder_->CreateBr(mergeBB);

                        // (AR) دمج النتائج بـ PHI (5 مصادر: لاشيء/نصّ/عشريّ/منطقيّ/صحيح)
                        // (EN) Merge results via PHI (5 sources: null/string/float/bool/int)
                        cg_.builder_->SetInsertPoint(mergeBB);
                        auto *phi = cg_.builder_->CreatePHI(ptrTy_l, 5, "any.c.result");
                        phi->addIncoming(nullStr, nullBB);
                        phi->addIncoming(safeStr, ptrBB_l);
                        phi->addIncoming(fbuf, floatBB_l);
                        phi->addIncoming(boolStr, boolBB_l);
                        phi->addIncoming(numBuf, intBB_l);
                        return phi;
                    }

                    auto isPtrIt = cg_.context_info_.namedValues.find(op.name + ".__is_ptr");
                    if (isPtrIt != cg_.context_info_.namedValues.end())
                    {
                        llvm::Value *isPtr = isPtrIt->second;

                        // (AR) فك وسم 2-bit: bit63 + bit62
                        //      bit63=0 → مؤشر (نص)
                        //      bit63=1, bit62=0 → رقم
                        //      bit63=1, bit62=1 → منطقي (boolean)
                        // (EN) 2-bit untagging: bit63 + bit62
                        llvm::Value *clearMask = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), ~(3ULL << 62));
                        llvm::Value *cleanVal = cg_.builder_->CreateAnd(val, clearMask, "adt.clean");

                        // (AR) فحص bit62 للتمييز بين رقم ومنطقي
                        llvm::Value *bit62Mask = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 1ULL << 62);
                        llvm::Value *bit62 = cg_.builder_->CreateAnd(val, bit62Mask, "adt.bit62");
                        llvm::Value *isBool = cg_.builder_->CreateICmpNE(bit62, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0), "adt.isbool");

                        // (AR) تحويل i64 إلى ptr لحالة النص
                        llvm::Value *asPtr = cg_.builder_->CreateIntToPtr(
                            cleanVal, llvm::PointerType::getUnqual(*cg_.context_), "adt.str.ptr");
                        llvm::Value *ptrIsNull = cg_.builder_->CreateICmpEQ(
                            asPtr,
                            llvm::ConstantPointerNull::get(
                                llvm::cast<llvm::PointerType>(asPtr->getType())),
                            "adt.ptr.isnull");
                        llvm::Value *safePtr = cg_.emitSafeStringPtr(asPtr, "adt.null");

                        // (AR) تحويل الرقم إلى نص عبر sprintf
                        llvm::Value *numBuf = cg_.builder_->CreateAlloca(
                            llvm::Type::getInt8Ty(*cg_.context_),
                            llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 32),
                            "adt.num.buf");
                        llvm::FunctionType *sprintfTy = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*cg_.context_),
                            {llvm::PointerType::getUnqual(*cg_.context_),
                             llvm::PointerType::getUnqual(*cg_.context_)},
                            true);
                        llvm::FunctionCallee sprintfFunc = cg_.module_->getOrInsertFunction("sprintf", sprintfTy);
                        llvm::Value *intFmt = cg_.builder_->CreateGlobalStringPtr("%lld", "adt.int.fmt");
                        cg_.builder_->CreateCall(sprintfFunc, {numBuf, intFmt, cleanVal});

                        // (AR) نصوص المنطقي
                        llvm::Value *trueStr = cg_.builder_->CreateGlobalStringPtr("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "adt.bool.true");
                        llvm::Value *falseStr = cg_.builder_->CreateGlobalStringPtr("\xd8\xae\xd8\xb7\xd8\xa3", "adt.bool.false");
                        llvm::Value *boolCond = cg_.builder_->CreateICmpNE(cleanVal, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 0), "adt.bool.cond");
                        llvm::Value *boolStr = cg_.builder_->CreateSelect(boolCond, trueStr, falseStr, "adt.bool.str");

                        // (AR) اختيار النتيجة: مؤشر / منطقي / رقم
                        //      isPtr → safePtr
                        //      !isPtr && isBool → boolStr
                        //      !isPtr && !isBool → numBuf
                        llvm::Value *nonPtrResult = cg_.builder_->CreateSelect(isBool, boolStr, numBuf, "adt.nonptr");
                        return cg_.builder_->CreateSelect(isPtr, safePtr, nonPtrResult, "adt.str.result");
                    }
                }

                // ================================================================
                // (AR) القيم المنطقية بنوع LLVM i1 — تحويل مباشر إلى "صحيح"/"خطأ"
                //      يحدث عند تحميل حقل bool من كائن وربطه بنص عبر string.concat
                // (EN) Boolean values with LLVM type i1 — direct conversion to "صحيح"/"خطأ"
                //      Occurs when loading a bool field from object and concatenating with string
                // ================================================================
                if (ty->isIntegerTy(1))
                {
                    llvm::Value *trueStr = cg_.builder_->CreateGlobalStringPtr(
                        "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "bool.true"); // صحيح
                    llvm::Value *falseStr = cg_.builder_->CreateGlobalStringPtr(
                        "\xd8\xae\xd8\xb7\xd8\xa3", "bool.false"); // خطأ
                    return cg_.builder_->CreateSelect(val, trueStr, falseStr, "bool.str");
                }

                // ================================================================
                // (AR) فحص kSadNullSentinel — القيمة الخاصة بـ لاشيء
                //      إذا كان الرقم يساوي kSadNullSentinel نُرجع "لاشيء"
                //      يحدث مع: حاول_استقبل من قناة فارغة، استقبل_بمهلة بعد انتهاء المهلة
                // (EN) Check kSadNullSentinel — special null value
                //      If integer equals kSadNullSentinel, return "لاشيء"
                //      Occurs with: try_recv from empty channel, recv_timeout after timeout
                // ================================================================
                if (ty->isIntegerTy(64))
                {
                    auto *i64Ty_s = llvm::Type::getInt64Ty(*cg_.context_);
                    auto *sentinelVal = llvm::ConstantInt::get(i64Ty_s, Sad::Compiler::kSadNullSentinel);
                    // (AR) [إصلاح تصادم kSadNullSentinel] طبيعي64/بايت لا يكونان نوعَ العدم
                    //      (العدمُ يُخزَّن ثابتَ i64 نوعُه Integer)؛ فالحارس (2^63+1 لا-موقَّعًا) لا
                    //      يُطبَّق عليهما — قيمةٌ شرعيّة قد تساويه فتُحوَّل «لاشيء» خطأً. نُثبّت
                    //      isSentinel=false فيؤخذ المسار العاديّ (تحويل الرقم) دائمًا. Integer
                    //      **مُستثنى** (يتصادم جوهريًّا مع العدم: `نص(لاشيء)` عدمُه Integer)،
                    //      فيبقى له الفحص كالأنواع النِّلابليّة.
                    // (EN) [kSadNullSentinel collision fix] طبيعي64/Byte are never the null type
                    //      (null is stored as an Integer-typed i64 constant); the sentinel (2^63+1
                    //      unsigned) is not applied to them — a legitimate equal value was wrongly
                    //      rendered «لاشيء». Force isSentinel=false so the normal number-conversion
                    //      path is always taken. Integer is EXCLUDED (it collides intrinsically:
                    //      `نص(لاشيء)`'s null is Integer-typed), so it keeps the check like nullable
                    //      types.
                    const bool isNonNullableNum =
                        op.dataType == SadTypeKind::UInt64 ||
                        op.dataType == SadTypeKind::Byte;
                    llvm::Value *isSentinel = isNonNullableNum
                                                  ? llvm::ConstantInt::getFalse(*cg_.context_)
                                                  : cg_.builder_->CreateICmpEQ(val, sentinelVal, "int.is.null");
                    auto *nullStr = cg_.builder_->CreateGlobalStringPtr(
                        "\xd9\x84\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xa1", "int.null.str"); // لاشيء

                    auto *parentFunc3 = cg_.builder_->GetInsertBlock()->getParent();
                    auto *sentinelBB = llvm::BasicBlock::Create(*cg_.context_, "int.sentinel", parentFunc3);
                    auto *normalBB = llvm::BasicBlock::Create(*cg_.context_, "int.normal", parentFunc3);
                    auto *intMergeBB = llvm::BasicBlock::Create(*cg_.context_, "int.merge", parentFunc3);

                    cg_.builder_->CreateCondBr(isSentinel, sentinelBB, normalBB);

                    // (AR) كتلة sentinel: إرجاع "لاشيء"
                    cg_.builder_->SetInsertPoint(sentinelBB);
                    cg_.builder_->CreateBr(intMergeBB);

                    // (AR) كتلة عادية: تحويل الرقم إلى نص
                    cg_.builder_->SetInsertPoint(normalBB);

                    // (AR) ISSUE-076 (Amelia #8): 512 — يخدم النصّ العدديّ والعشريّ (%.6f قد
                    //      يبلغ ~316 حرفًا لـDBL_MAX عبر __sad_format_double فيفيض 32).
                    // (EN) ISSUE-076 (Amelia #8): 512 — serves int + double text (%.6f can reach
                    //      ~316 chars for DBL_MAX via __sad_format_double, overflowing 32).
                    llvm::Value *buf = cg_.builder_->CreateAlloca(
                        llvm::Type::getInt8Ty(*cg_.context_),
                        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 512),
                        "strbuf");

                    if (cg_.freestanding_)
                    {
                        llvm::FunctionType *itoaType = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*cg_.context_),
                            {llvm::PointerType::getUnqual(*cg_.context_),
                             llvm::Type::getInt64Ty(*cg_.context_)},
                            false);
                        llvm::FunctionCallee itoaFn = cg_.module_->getOrInsertFunction("__sad_itoa", itoaType);

                        if (ty->isIntegerTy())
                        {
                            llvm::Value *val64 = cg_.builder_->CreateIntCast(val, llvm::Type::getInt64Ty(*cg_.context_), true);
                            cg_.builder_->CreateCall(itoaFn, {buf, val64});
                        }
                        else if (ty->isDoubleTy())
                        {
                            llvm::FunctionType *ftoaType = llvm::FunctionType::get(
                                llvm::Type::getInt32Ty(*cg_.context_),
                                {llvm::PointerType::getUnqual(*cg_.context_),
                                 llvm::Type::getDoubleTy(*cg_.context_)},
                                false);
                            llvm::FunctionCallee ftoaFn = cg_.module_->getOrInsertFunction("__sad_ftoa", ftoaType);
                            cg_.builder_->CreateCall(ftoaFn, {buf, val});
                        }
                    }
                    else
                    {
                        llvm::FunctionType *sprintfType = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*cg_.context_),
                            {llvm::PointerType::getUnqual(*cg_.context_),
                             llvm::PointerType::getUnqual(*cg_.context_)},
                            true);
                        llvm::FunctionCallee sprintfFn = cg_.module_->getOrInsertFunction("sprintf", sprintfType);

                        if (ty->isIntegerTy())
                        {
                            llvm::Value *fmt = cg_.builder_->CreateGlobalStringPtr("%lld", "int.fmt");
                            llvm::Value *val64 = cg_.builder_->CreateIntCast(val, llvm::Type::getInt64Ty(*cg_.context_), true);
                            cg_.builder_->CreateCall(sprintfFn, {buf, fmt, val64});
                        }
                        else if (ty->isDoubleTy())
                        {
                            // (AR) إصلاح: استخدام __sad_format_double لمطابقة دقة المفسر
                            // (EN) Fix: use __sad_format_double to match interpreter precision
                            auto *fmtDblType = llvm::FunctionType::get(
                                llvm::Type::getVoidTy(*cg_.context_),
                                {llvm::PointerType::getUnqual(*cg_.context_),
                                 llvm::Type::getDoubleTy(*cg_.context_)},
                                false);
                            auto fmtDblFn = cg_.module_->getOrInsertFunction("__sad_format_double", fmtDblType);
                            cg_.builder_->CreateCall(fmtDblFn, {buf, val});
                        }
                    }

                    cg_.builder_->CreateBr(intMergeBB);

                    // (AR) دمج النتائج
                    cg_.builder_->SetInsertPoint(intMergeBB);
                    auto *ptrTy3 = llvm::PointerType::getUnqual(*cg_.context_);
                    auto *intPhi = cg_.builder_->CreatePHI(ptrTy3, 2, "int.str.result");
                    intPhi->addIncoming(nullStr, sentinelBB);
                    intPhi->addIncoming(buf, normalBB);
                    return intPhi;
                }

                // (AR) للأنواع الأخرى (float/double): المسار العادي بدون فحص sentinel
                // (AR) ISSUE-076 (Amelia #8): 512 لا 32 — %.6f لـDBL_MAX ~316 حرفًا يفيض.
                // (EN) ISSUE-076 (Amelia #8): 512 not 32 — %.6f for DBL_MAX ~316 chars overflows.
                llvm::Value *buf = cg_.builder_->CreateAlloca(
                    llvm::Type::getInt8Ty(*cg_.context_),
                    llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 512),
                    "strbuf");
                if (ty->isDoubleTy())
                {
                    if (cg_.freestanding_)
                    {
                        llvm::FunctionType *ftoaType = llvm::FunctionType::get(
                            llvm::Type::getInt32Ty(*cg_.context_),
                            {llvm::PointerType::getUnqual(*cg_.context_),
                             llvm::Type::getDoubleTy(*cg_.context_)},
                            false);
                        llvm::FunctionCallee ftoaFn = cg_.module_->getOrInsertFunction("__sad_ftoa", ftoaType);
                        cg_.builder_->CreateCall(ftoaFn, {buf, val});
                    }
                    else
                    {
                        // (AR) إصلاح: استخدام __sad_format_double لمطابقة دقة المفسر
                        // (EN) Fix: use __sad_format_double to match interpreter precision
                        auto *fmtDblType = llvm::FunctionType::get(
                            llvm::Type::getVoidTy(*cg_.context_),
                            {llvm::PointerType::getUnqual(*cg_.context_),
                             llvm::Type::getDoubleTy(*cg_.context_)},
                            false);
                        auto fmtDblFn = cg_.module_->getOrInsertFunction("__sad_format_double", fmtDblType);
                        cg_.builder_->CreateCall(fmtDblFn, {buf, val});
                    }
                }
                return buf;
            };

            // (AR) الخريطةُ وحدَها تُخصِّصُ مخزنًا **مؤقّتًا** في `ensureString` (المساعِدُ
            //   كبيرٌ فلا يُدمَجُ والمؤشّرُ يهربُ بالإرجاع ⇒ لا يُرقّى إلى مكدّس كما يحدثُ
            //   لمساعِدِ المصفوفة). والنتيجةُ أدناه **تنسخُ** الطرفَين بـmemcpy، فالمؤقّتُ
            //   لقًى بعدَها ⇒ يُحرَّر. بلا هذا يتسرّبُ عند كلِّ وصلٍ نصّيٍّ بخريطة —
            //   مقيسٌ: ٥٠٠ ألف دورةٍ ⇒ ٢٨٣ م.ب مقابل ٦٫٦ م.ب لخطِّ الأساس.
            // (EN) Only the map allocates a TEMPORARY in ensureString (its helper is too large
            //   to inline and the pointer escapes, so unlike the array helper it is never
            //   promoted to the stack). The result below memcpy's both sides, so the temporary
            //   is dead afterwards and is freed — otherwise every map concat leaks (measured).
            const bool leftIsMapTemporary = inst->operands[0].dataType == SadTypeKind::Map;
            const bool rightIsMapTemporary = inst->operands[1].dataType == SadTypeKind::Map;
            left = ensureString(left, leftTy, inst->operands[0]);
            right = ensureString(right, rightTy, inst->operands[1]);

            // Get lengths using strlen

            llvm::Value *len1 = cg_.emitStrlen(left, "len1");
            llvm::Value *len2 = cg_.emitStrlen(right, "len2");
            llvm::Value *totalLen = cg_.builder_->CreateAdd(len1, len2, "totallen");
            llvm::Value *bufSize = cg_.builder_->CreateAdd(totalLen,
                                                       llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 1), "bufsize");

            // Allocate result buffer on HEAP (using malloc) so it's safe to return
            // (AR) تخصيص على الـ Heap بدلاً من Stack لتجنب مشاكل الرجوع من الدوال
            // (EN) Allocate on heap instead of stack to avoid returning dangling pointers
            llvm::Value *result = cg_.emitMalloc(bufSize, "concat.buf");

            // Copy first string using memcpy
            // (AR) طول ‎memcpy‎ بنوع ‎size_t‎ الهدف (i32 على 32-بت) — يطابق
            //      النداء المكتبيّ الذي تولّده الخلفيّة وتعريفَ وقت التشغيل الحرّ.
            llvm::Type *szTy = cg_.getSizeType();
            llvm::FunctionType *memcpyType = llvm::FunctionType::get(
                llvm::PointerType::getUnqual(*cg_.context_),
                {llvm::PointerType::getUnqual(*cg_.context_),
                 llvm::PointerType::getUnqual(*cg_.context_),
                 szTy},
                false);
            llvm::FunctionCallee memcpyFn = cg_.module_->getOrInsertFunction("memcpy", memcpyType);

            cg_.builder_->CreateCall(memcpyFn, {result, left,
                cg_.builder_->CreateZExtOrTrunc(len1, szTy, "concat.len1.sz")});

            // Copy second string after first
            llvm::Value *dest2 = cg_.builder_->CreateGEP(
                llvm::Type::getInt8Ty(*cg_.context_), result, len1, "dest2");
            llvm::Value *copyLen2 = cg_.builder_->CreateAdd(len2,
                                                        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*cg_.context_), 1), "copylen2");
            cg_.builder_->CreateCall(memcpyFn, {dest2, right,
                cg_.builder_->CreateZExtOrTrunc(copyLen2, szTy, "copylen2.sz")});

            // (AR) التحريرُ **بعدَ** النسختَين: قبلَهما استعمالٌ بعدَ التحرير.
            if (leftIsMapTemporary && left->getType()->isPointerTy())
                cg_.emitFreeCall(left);
            if (rightIsMapTemporary && right->getType()->isPointerTy())
                cg_.emitFreeCall(right);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }



        // (AR) 🔑 هذه الدالّةُ كانت تُحوِّل i64 إلى مؤشّرٍ **بلا فحصٍ**، فكلُّ مستدعٍ لها
        //      كان منفذَ انهيارٍ حين تحمل الخانةُ حارسَ العدم.
        //
        // (AR) ⚠️ وأوّلُ إصلاحٍ لها كان **خطأً أشدَّ من العطب**: جُعلت غلافًا على بابِ
        //      **العرض**، فصار «س.يحتوي("لا")» على خانةٍ عدميّةٍ يطبع «صحيح» — لأنّ
        //      الحارسَ أُبدل بلفظِ «لاشيء» ثمّ بُحث فيه عن «لا» فوُجد. مقيسٌ حيًّا:
        //      المفسّرُ يرفع RUN033 والمترجّمُ يجيب «صحيح». أي أنّ الإصلاحَ حوّل صنفًا
        //      كاملًا من الانهياراتِ **المرئيّة** إلى أجوبةٍ كاذبةٍ لا يراها أحد،
        //      وهو عينُ ما يحذّر منه تعليقُ البابِ نفسِه. كشفته مراجعةٌ خصميّة.
        //
        // (AR) 🔑 ولذلك صار اسمُ الطريقةِ **معامِلًا إلزاميًّا**: منفذٌ جديدٌ ينسى الاسمَ
        //      لا يُصرَّف أصلًا. الحارسُ في **التوقيع** لا في انضباطِ من يأتي بعدُ.
        // (EN) The first fix made this a wrapper over the DISPLAY door, turning a whole
        //      class of visible crashes into invisible wrong answers («س.يحتوي("لا")»
        //      answered true by searching the null word). It now goes through the
        //      OPERATION door, and the method name is a REQUIRED parameter so that a
        //      future port cannot silently forget it — the guard lives in the signature.
        llvm::Value *StringOpsCodeGen::normalizeStringPtr(llvm::Value *str,
                                                          const char *label,
                                                          std::string_view methodName)
        {
            if (!str)
                return nullptr;
            return cg_.emitStringPtrOrRaise(
                str, LLVMCodeGen::stringMethodOperationLabel(methodName), label);
        }

        // (AR) بابُ **الوسيط**: المفسّرُ يُبدِل الوسيطَ العدميَّ بلفظِ «لاشيء» ولا يرفع
        //      (مقيس: «"أهلا".استبدل("أ"، ب)» ⇒ «لاشيءهلا»، و«"أهلا".بحث(ب)» ⇒ ‎-1‎).
        //      فالمستقبِلُ يرفعُ والوسيطُ يُعرَض — بابان لأنّ الدلالتَين اثنتان.
        // (EN) Argument door: the interpreter substitutes the null word for a null
        //      argument and proceeds; only the receiver raises.
        llvm::Value *StringOpsCodeGen::normalizeStringArgPtr(llvm::Value *str, const char *label)
        {
            if (!str)
                return nullptr;
            return cg_.emitSafeStringPtr(str, label);
        }

        llvm::Value *StringOpsCodeGen::emitStringCharAt(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "BUILTIN_STRING_CHAR_AT"}});
                return nullptr;
            }

            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *index = cg_.resolveOperand(inst->operands[1]);
            if (!str || !index)
                return nullptr;

            // (AR) عبر البابِ الواحد لا بـinttoptr خامّ (كان يتجاوز normalizeStringPtr
            //      وهي على بُعدِ أسطر).
            // (EN) Through the single door, not a raw inttoptr.
            // (AR) بابُ العمليّة: المفسّرُ يرفعُ RUN033 على «.حرف_عند» لقيمةٍ عدميّة.
            // (EN) Operation door: the interpreter raises RUN033 here.
            str = cg_.emitStringPtrOrRaise(
                str,
                LLVMCodeGen::stringMethodOperationLabel(
                    Sad::Builtins::Names::TypeMethods::String::CHAR_AT),
                "char_at.str");

            // Ensure index is i64
            if (index->getType() != llvm::Type::getInt64Ty(*cg_.context_))
            {
                index = cg_.builder_->CreateIntCast(index, llvm::Type::getInt64Ty(*cg_.context_), true, "idx.i64");
            }

            // GEP to get pointer to character at index
            llvm::Value *charPtr = cg_.builder_->CreateGEP(
                llvm::Type::getInt8Ty(*cg_.context_), str, index, "char.ptr");

            // Load the character (i8)
            llvm::Value *charVal = cg_.builder_->CreateLoad(
                llvm::Type::getInt8Ty(*cg_.context_), charPtr, "char.val");

            // Zero-extend to i64
            llvm::Value *result = cg_.builder_->CreateZExt(charVal, llvm::Type::getInt64Ty(*cg_.context_), "char.i64");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }

            return result;
        }



        // ====================================================================
        // (AR) حرف_من_رمز — نقطةُ ترميزٍ (i64) ⇒ نصُّ UTF-8 مُنهًى بصفر.
        //      يُولَّد بلا فروعٍ: طولُ الترميزِ يُحسَب بمقارناتٍ تُجمَع، والبايتاتُ
        //      الأربعةُ تُبنى كلُّها ثمّ يُختار كلُّ بايتٍ بـ`select` حسب الطول.
        //      اخترتُ اللافرعيَّ لأنّ الفروعَ هنا كانت تعني أربعَ كتلٍ وPHI في
        //      دالّةٍ تُستدعى داخلَ حلقةِ تحليلٍ حرفًا حرفًا.
        //      والنقطةُ خارجَ المدى أو في نطاقِ البدائلِ **تَرمي** كما يرمي المفسّر.
        //      كانت تُعطي نصًّا فارغًا برمزِ خروجٍ ٠ بتسويغِ «لا مسارَ رميٍ هنا»،
        //      وذاك التسويغُ سقط: `emitCallException(kRuntimeRaise, …)` صار حيًّا في
        //      هذا الملفِّ عينِه لحارسَي `رقم`/`عشري`. والفرعُ الوحيدُ في دالّةٍ اخترتُها
        //      لافرعيّةً لا يُخلُّ بذلك: هو على مسارِ الخطأِ وحدَه، ويُتوقَّعُ عدمُ أخذِه.
        // (EN) حرف_من_رمز — a code point (i64) ⇒ a NUL-terminated UTF-8 string.
        //      Branch-free: the encoded length is a sum of comparisons and all four
        //      bytes are computed then selected per length. Chosen over four blocks
        //      and a PHI because this runs inside a per-character parse loop.
        //      Out-of-range or surrogate code points now RAISE, as the interpreter does.
        //      They used to yield an empty string with exit code 0, justified by "no throw
        //      path here" — an excuse this batch invalidated by making
        //      emitCallException(kRuntimeRaise, …) live in this same file. The single branch
        //      does not spoil the branch-free choice: it is on the error path only.
        // (AR) وتباعدٌ ثانٍ **مُعلَنٌ وبنيويّ**: `حرف_من_رمز(0)` طولُه ١ مفسَّرًا و٠
        //      مصرَّفًا. والسببُ أنّ النصَّ هنا منتهٍ بصفرٍ، فمحرفُ U+0000 نفسُه هو
        //      الخاتم. سدُّه يقتضي نصًّا محمولَ الطولِ لا منتهيًا بصفر — تغييرُ
        //      تمثيلٍ لا رقعةَ موضعٍ، فيُعلَن ولا يُدَّعى سدُّه.
        // (EN) A second DECLARED, structural divergence: حرف_من_رمز(0) has length 1
        //      interpreted and 0 compiled, because strings are NUL-terminated and U+0000
        //      *is* the terminator. Closing it needs a length-carrying string
        //      representation, not a local patch — declared, not claimed fixed.
        // ====================================================================
        llvm::Value *StringOpsCodeGen::emitStringCharFromCode(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS,
                                {{"detail", "BUILTIN_STRING_CHAR_FROM_CODE"}});
                return nullptr;
            }

            llvm::Value *code = cg_.resolveOperand(inst->operands[0]);
            if (!code)
                return nullptr;

            auto &b = *cg_.builder_;
            auto *i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            auto *i64Ty = llvm::Type::getInt64Ty(*cg_.context_);

            // (AR) العشريُّ يُحوَّل لا يُمدَّد: `CreateIntCast` على `double` تُصدِر
            //      `sext double … to i64` فيرفضه `verifyModule` («SExt only operates on
            //      integer») فتظهر «علّةُ مترجمٍ داخليّة» — قِيس على `حرف_من_رمز(65.9)`
            //      التي يقبلها المفسّرُ ويعطي «A». والاقتطاعُ نحوَ الصفرِ هو دلالةُ
            //      `toInt()` في المفسّر، فالطرفان يتّفقان.
            // (EN) A float must be converted, not extended: CreateIntCast on a double emits
            //      `sext double … to i64`, which verifyModule rejects, surfacing as an
            //      internal compiler error — measured on حرف_من_رمز(65.9), which the
            //      interpreter accepts and answers "A". Truncation toward zero matches toInt().
            if (code->getType()->isFloatingPointTy())
            {
                // (AR) الاقتطاعُ **مُشبِعٌ** لا عارٍ: `fptosi` على قيمةٍ خارجَ مدى i64
                //      (أو NaN) نتيجتُها poison في LLVM، فالمقارناتُ الآتيةُ وتفرّعُ
                //      البطلانِ عليها سلوكٌ غيرُ معرَّف — مقيسًا: `حرف_من_رمز(1e300)`
                //      يرمي مفسَّرًا ولا يرمي مصرَّفًا فيعودُ نصًّا فارغًا بخروجٍ ٠،
                //      أي العطبُ نفسُه الذي أضيفَ هذا الرميُ لسدِّه. والإشباعُ يجعل
                //      1e300 ⇒ INT64_MAX فيقعُ في `tooBig` ويرمي. ونظيرُه المعتمَدُ
                //      في `unpackI64` بالملفِّ sad_dyn_repr، فالموضعان متّسقان.
                // (EN) Saturating, not raw: `fptosi` on an out-of-i64-range value (or NaN)
                //      yields poison, making the range comparisons and the invalid-branch
                //      below undefined — measured: حرف_من_رمز(1e300) raises in the
                //      interpreter and silently yields an empty string with exit 0 when
                //      compiled. Saturation maps 1e300 to INT64_MAX ⇒ tooBig ⇒ raise.
                //      Mirrors unpackI64 in sad_dyn_repr.
                llvm::Function *saturatingConvert = llvm::Intrinsic::getDeclaration(
                    cg_.module_.get(), llvm::Intrinsic::fptosi_sat,
                    {i64Ty, code->getType()});
                code = b.CreateCall(saturatingConvert, {code}, "cfc.code.fp.sat");
            }
            else if (code->getType() != i64Ty)
                code = b.CreateIntCast(code, i64Ty, true, "cfc.code");

            auto konst = [&](long long v) { return llvm::ConstantInt::get(i64Ty, v); };

            // (AR) النطاقُ الصالح: [0, 0x10FFFF] خلا نطاقِ البدائل [0xD800, 0xDFFF].
            llvm::Value *tooSmall = b.CreateICmpSLT(code, konst(0), "cfc.neg");
            llvm::Value *tooBig = b.CreateICmpSGT(code, konst(0x10FFFF), "cfc.big");
            llvm::Value *surrogate = b.CreateAnd(
                b.CreateICmpSGE(code, konst(0xD800)),
                b.CreateICmpSLE(code, konst(0xDFFF)), "cfc.surrogate");
            llvm::Value *invalid = b.CreateOr(b.CreateOr(tooSmall, tooBig), surrogate, "cfc.invalid");

            // (AR) الرميُ في الوضعِ المستضافِ وحدَه: الوضعُ الحرُّ بلا استثناءاتٍ أصلًا،
            //      فيبقى فيه التصفيرُ أدناه كما كان — تباعدٌ في وضعٍ لا يملك المسار،
            //      لا في وضعٍ يملكه ويهمله.
            // (EN) Hosted only: freestanding has no exceptions at all, so the zeroing below
            //      stays there — a divergence in a mode that lacks the path, not in one that
            //      has it and skips it.
            if (!cg_.freestanding_)
            {
                llvm::Function *curFunc = b.GetInsertBlock()->getParent();
                llvm::BasicBlock *raiseBB =
                    llvm::BasicBlock::Create(*cg_.context_, "cfc.invalid.raise", curFunc);
                llvm::BasicBlock *okBB =
                    llvm::BasicBlock::Create(*cg_.context_, "cfc.invalid.ok", curFunc);
                b.CreateCondBr(invalid, raiseBB, okBB);

                b.SetInsertPoint(raiseBB);
                std::vector<llvm::Value *> raiseArgs;
                raiseArgs.push_back(cg_.getConstantString(::Sad::Compiler::kCharCodeOutOfRangeMsg));
                cg_.emitCallException(::Sad::Compiler::kRuntimeRaise, raiseArgs, nullptr);

                b.SetInsertPoint(okBB);
            }

            // (AR) الطول = 1 + (>=0x80) + (>=0x800) + (>=0x10000)، ويُصفَّر عند البطلان.
            llvm::Value *length = konst(1);
            length = b.CreateAdd(length, b.CreateZExt(b.CreateICmpSGE(code, konst(0x80)), i64Ty));
            length = b.CreateAdd(length, b.CreateZExt(b.CreateICmpSGE(code, konst(0x800)), i64Ty));
            length = b.CreateAdd(length, b.CreateZExt(b.CreateICmpSGE(code, konst(0x10000)), i64Ty),
                                 "cfc.len");
            length = b.CreateSelect(invalid, konst(0), length, "cfc.len.guarded");

            // (AR) خمسةُ بايتاتٍ دائمًا: أربعةٌ للترميزِ وواحدٌ للصفرِ الخاتم.
            //      عبرَ `cg_.emitMalloc` لا بتصريحٍ موضعيّ: هو **المسارُ الوحيدُ المسموح**
            //      (انظر llvm_codegen.h:490)، وتخطّيه يفوّت عقدَ `size_t` المركزيَّ
            //      ويتفلّت من بوّابةِ `call @malloc` في الوضعِ الحرّ (رصدُ أميليا م‑٤).
            // (EN) Via `cg_.emitMalloc`, not a local declaration: that is the SOLE sanctioned
            //      path (llvm_codegen.h:490); bypassing it misses the central `size_t`
            //      contract and evades the freestanding `call @malloc` gate (Amelia م‑٤).
            llvm::Value *buf = cg_.emitMalloc(
                llvm::ConstantInt::get(i64Ty, kCharFromCodeBufferBytes), "cfc.buf");

            auto lowSix = [&](llvm::Value *shifted) {
                return b.CreateOr(b.CreateAnd(shifted, konst(0x3F)), konst(0x80));
            };
            llvm::Value *shift6 = b.CreateLShr(code, konst(6));
            llvm::Value *shift12 = b.CreateLShr(code, konst(12));
            llvm::Value *shift18 = b.CreateLShr(code, konst(18));

            llvm::Value *isOne = b.CreateICmpEQ(length, konst(1), "cfc.is1");
            llvm::Value *isTwo = b.CreateICmpEQ(length, konst(2), "cfc.is2");
            llvm::Value *isThree = b.CreateICmpEQ(length, konst(3), "cfc.is3");

            // (AR) البايتُ الأوّل: بادئةٌ مختلفةٌ لكلِّ طول.
            llvm::Value *first = b.CreateOr(shift18, konst(0xF0));
            first = b.CreateSelect(isThree, b.CreateOr(b.CreateAnd(shift12, konst(0x0F)), konst(0xE0)), first);
            first = b.CreateSelect(isTwo, b.CreateOr(b.CreateAnd(shift6, konst(0x1F)), konst(0xC0)), first);
            first = b.CreateSelect(isOne, b.CreateAnd(code, konst(0x7F)), first, "cfc.b0");

            llvm::Value *second = lowSix(shift12);
            second = b.CreateSelect(isThree, lowSix(shift6), second);
            second = b.CreateSelect(isTwo, lowSix(code), second, "cfc.b1");

            llvm::Value *third = lowSix(shift6);
            third = b.CreateSelect(isThree, lowSix(code), third, "cfc.b2");

            llvm::Value *fourth = lowSix(code);

            // (AR) تُكتَب البايتاتُ الأربعةُ كلُّها دائمًا داخلَ المخزنِ (٥ بايتات)،
            //      ثمّ يُوضَع الصفرُ الخاتمُ عند `length` فيقتطع ما زاد. لا كتابةَ
            //      خارجَ المخزنِ في أيِّ حال.
            llvm::Value *bytes[kCharFromCodeMaxUtf8Bytes] = {first, second, third, fourth};
            for (unsigned i = 0; i < kCharFromCodeMaxUtf8Bytes; ++i)
            {
                llvm::Value *slot = b.CreateGEP(i8Ty, buf, {konst(i)});
                b.CreateStore(b.CreateTrunc(bytes[i], i8Ty), slot);
            }
            llvm::Value *terminator = b.CreateGEP(i8Ty, buf, {length}, "cfc.nul");
            b.CreateStore(llvm::ConstantInt::get(i8Ty, 0), terminator);

            if (inst->result.has_value())
                cg_.context_info_.namedValues[inst->result->name] = buf;
            return buf;
        }

        llvm::Value *StringOpsCodeGen::emitStringCmp(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "STRING_CMP"}});
                return nullptr;
            }

            llvm::Value *left = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *right = cg_.resolveOperand(inst->operands[1]);
            if (!left || !right)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "STRING_CMP"}});
                return nullptr;
            }

            // (AR) م1ب ISSUE-076: معامل %SadDyn (نتيجة «؟.» نصّيّة مثلًا) ⇒ فوّض للموزِّع
            //      dynCompare (وسم Str ⇒ strcmp بالمحتوى؛ غيره ⇒ عدديّ). STRING_CMP دلالتُه
            //      «متساويان؟» فيطابق DynCmp::EQ. بلا هذا كان الوسمُ البتّيّ يعمل «and %SadDyn, i64»
            //      ⇒ فشل verifyModule (نوعان مختلفان).
            // (EN) م1ب ISSUE-076: a %SadDyn operand (e.g. a «؟.» string result) ⇒ delegate to the
            //      dynCompare dispatcher (Str tag ⇒ strcmp by content; else numeric). STRING_CMP means
            //      «are they equal?», matching DynCmp::EQ. Without this the bit-tag did «and %SadDyn, i64»
            //      ⇒ verifyModule failure (mismatched types).
            if (isSadDyn(left) || isSadDyn(right))
            {
                llvm::Value *dl = toDyn(cg_, left, inst->operands[0].dataType);
                llvm::Value *dr = toDyn(cg_, right, inst->operands[1].dataType);
                llvm::Value *dres = dynCompare(cg_, DynCmp::EQ, dl, dr);
                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = dres;
                return dres;
            }

            // (AR) حماية ضد null sentinel وقيم tagged:
            //      القيم المستقبلة من القنوات قد تحمل null sentinel أو أرقام مُعلّمة
            //      (bit63=1). استدعاء strcmp على هذه القيم يسبب ACCESS_VIOLATION.
            //      الحل: فحص bit63 لكلا المعاملين قبل strcmp.
            //      إذا كان أي منهما يحمل bit63=1 أو يساوي null → النتيجة = خطأ (غير متساويين).
            // (EN) Guard against null sentinel and tagged values from channel recv:
            //      If bit63=1, value is a tagged int/bool, not a string pointer.
            //      Calling strcmp on it would crash. Check both operands before strcmp.
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8p = llvm::PointerType::getUnqual(*cg_.context_);
            auto zero64 = llvm::ConstantInt::get(i64Ty, 0);
            auto bit63Mask = llvm::ConstantInt::get(i64Ty, 1ULL << 63);

            // (AR) الحصول على تمثيل i64 لفحص العلامات
            llvm::Value *leftI64 = left->getType()->isPointerTy()
                                       ? cg_.builder_->CreatePtrToInt(left, i64Ty, "str.cmp.l.i64")
                                       : left;
            llvm::Value *rightI64 = right->getType()->isPointerTy()
                                        ? cg_.builder_->CreatePtrToInt(right, i64Ty, "str.cmp.r.i64")
                                        : right;

            // (AR) فحص: bit63=0 (مؤشر صالح) و ≠ null
            auto leftTag = cg_.builder_->CreateAnd(leftI64, bit63Mask, "str.cmp.l.tag");
            auto rightTag = cg_.builder_->CreateAnd(rightI64, bit63Mask, "str.cmp.r.tag");
            auto leftIsStr = cg_.builder_->CreateICmpEQ(leftTag, zero64, "str.cmp.l.isstr");
            auto rightIsStr = cg_.builder_->CreateICmpEQ(rightTag, zero64, "str.cmp.r.isstr");
            auto leftNotNull = cg_.builder_->CreateICmpNE(leftI64, zero64, "str.cmp.l.nn");
            auto rightNotNull = cg_.builder_->CreateICmpNE(rightI64, zero64, "str.cmp.r.nn");

            auto leftOk = cg_.builder_->CreateAnd(leftIsStr, leftNotNull, "str.cmp.l.ok");
            auto rightOk = cg_.builder_->CreateAnd(rightIsStr, rightNotNull, "str.cmp.r.ok");
            auto canStrcmp = cg_.builder_->CreateAnd(leftOk, rightOk, "str.cmp.can");

            // (AR) 🔑 «عدمٌ يساوي عدمًا» — تُحسَب **هنا** في كتلةِ الدخولِ لا في كتلةِ
            //      الدمج: قيمةُ PHI الواردةُ من ذراعٍ يجب أن تسيطرَ على نهايةِ تلك الذراع،
            //      وقيمةٌ مولَّدةٌ في كتلةِ الدمجِ لا تسيطر على سابقتها ⇒ IR فاسد.
            // (EN) Computed in the entry block: a PHI incoming value must dominate the end
            //      of its predecessor; building it in the merge block yields invalid IR.
            auto sentinel64 = llvm::ConstantInt::get(
                i64Ty, static_cast<uint64_t>(Sad::Compiler::kSadNullSentinel));
            auto leftIsNull = cg_.builder_->CreateOr(
                cg_.builder_->CreateICmpEQ(leftI64, sentinel64, "str.cmp.l.sent"),
                cg_.builder_->CreateICmpEQ(leftI64, zero64, "str.cmp.l.zero"), "str.cmp.l.null");
            auto rightIsNull = cg_.builder_->CreateOr(
                cg_.builder_->CreateICmpEQ(rightI64, sentinel64, "str.cmp.r.sent"),
                cg_.builder_->CreateICmpEQ(rightI64, zero64, "str.cmp.r.zero"), "str.cmp.r.null");
            auto bothNull = cg_.builder_->CreateAnd(leftIsNull, rightIsNull, "str.cmp.both_null");

            // (AR) تفريع: إذا كلاهما نص صالح → strcmp، وإلا → خطأ
            auto *entryBB = cg_.builder_->GetInsertBlock();
            auto *parentFunc = entryBB->getParent();
            auto *strcmpBB = llvm::BasicBlock::Create(*cg_.context_, "str.cmp.do", parentFunc);
            auto *doneBB = llvm::BasicBlock::Create(*cg_.context_, "str.cmp.done", parentFunc);

            cg_.builder_->CreateCondBr(canStrcmp, strcmpBB, doneBB);

            // ─── كتلة strcmp: مقارنة فعلية ───
            cg_.builder_->SetInsertPoint(strcmpBB);
            llvm::Value *leftPtr = left->getType()->isPointerTy()
                                       ? left
                                       : cg_.builder_->CreateIntToPtr(left, i8p, "str.l");
            llvm::Value *rightPtr = right->getType()->isPointerTy()
                                        ? right
                                        : cg_.builder_->CreateIntToPtr(right, i8p, "str.r");

            llvm::FunctionType *strcmpType = llvm::FunctionType::get(
                llvm::Type::getInt32Ty(*cg_.context_), {i8p, i8p}, false);
            llvm::FunctionCallee strcmpFn = cg_.module_->getOrInsertFunction("strcmp", strcmpType);
            llvm::Value *cmpResult = cg_.builder_->CreateCall(strcmpFn, {leftPtr, rightPtr}, "strcmp.ret");

            // strcmp == 0 → متساويان → النتيجة 1 (صحيحة)
            llvm::Value *streq = cg_.builder_->CreateICmpEQ(cmpResult,
                                                        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*cg_.context_), 0), "streq");
            cg_.builder_->CreateBr(doneBB);

            // ─── كتلة النتيجة: دمج ───
            cg_.builder_->SetInsertPoint(doneBB);
            auto *result = cg_.builder_->CreatePHI(llvm::Type::getInt1Ty(*cg_.context_), 2, "streq.phi");
            result->addIncoming(streq, strcmpBB);
            // (AR) 🔑 الذراعُ غيرُ القابلةُ لـstrcmp كانت تُرجِع **كذبًا مطلقًا**، فـ
            //      «س == لاشيء» على خانةٍ نصّيّةٍ عدميّةٍ يُعطي كذبًا في المترجّمِ وصدقًا
            //      في المفسّر (مقيس: الاختبار 075). وهذا عيبُ **دلالةٍ** لا انهيار — أخفى
            //      نفسَه لأنّه لا يُسقِط البرنامج. والصوابُ: عدمٌ يساوي عدمًا، ولا يساوي
            //      نصًّا حاضرًا (الحاضرُ يذهب إلى ذراعِ strcmp أصلًا).
            // (EN) This arm returned an unconditional false, so «س == لاشيء» was false in
            //      the compiler and true in the interpreter — a semantic divergence that
            //      hid because it never crashed. null == null is true.
            result->addIncoming(bothNull, entryBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }



        // (AR) انظر التوثيق في string_ops_codegen.h — الترتيبُ المعجميُّ لنصّين.
        // (EN) See string_ops_codegen.h — lexicographic ordering of two strings.
        llvm::Value *StringOpsCodeGen::emitStringOrdCmp(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                // (AR) اسمُ الأوپكودِ مشتقٌّ من جدولِ الأسماءِ الواحد، لا سلسلةً حرفيّةً
                //      تنحرف عنه عند إعادةِ التسمية — نظيرُ detailOpcode في الخلفيّةِ السياديّة.
                // (EN) The opcode name is derived from the single name table rather than a
                //      literal that would drift on a rename — mirrors detailOpcode in the
                //      sovereign backend.
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS,
                                {{"detail", sirOpcodeToString(SIROpcode::STRING_ORD_CMP)}});
                return nullptr;
            }

            llvm::Value *left = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *right = cg_.resolveOperand(inst->operands[1]);
            if (!left || !right)
            {
                // (AR) اسمُ الأوپكودِ مشتقٌّ من جدولِ الأسماءِ الواحد، لا سلسلةً حرفيّةً
                //      تنحرف عنه عند إعادةِ التسمية — نظيرُ detailOpcode في الخلفيّةِ السياديّة.
                // (EN) The opcode name is derived from the single name table rather than a
                //      literal that would drift on a rename — mirrors detailOpcode in the
                //      sovereign backend.
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS,
                                {{"detail", sirOpcodeToString(SIROpcode::STRING_ORD_CMP)}});
                return nullptr;
            }

            auto *i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto *i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto *i8p = llvm::PointerType::getUnqual(*cg_.context_);
            auto *zero64 = llvm::ConstantInt::get(i64Ty, 0);
            auto *zero32 = llvm::ConstantInt::get(i32Ty, 0);
            auto *bit63Mask = llvm::ConstantInt::get(i64Ty, 1ULL << 63);

            // (AR) قيمةٌ موسومةٌ زمنَ التشغيل (%SadDyn): وسمُ Str في dynCompare يقارن المحتوى
            //      بـstrcmp — عينُ الدلالةِ هنا — فنفوّض إليه بدل تكرارِ الموزِّع. نستخرج
            //      الإشارةَ من نتيجتِه المنطقيّة: LT ⇒ ‎-1‎، وإلّا GT ⇒ ‎+1‎، وإلّا ٠.
            // (EN) A runtime-tagged value (%SadDyn): dynCompare's Str tag compares content with
            //      strcmp — the very semantics used here — so delegate instead of duplicating the
            //      dispatcher. Derive the sign from its boolean results: LT ⇒ -1, else GT ⇒ +1,
            //      else 0.
            if (isSadDyn(left) || isSadDyn(right))
            {
                llvm::Value *dl = toDyn(cg_, left, inst->operands[0].dataType);
                llvm::Value *dr = toDyn(cg_, right, inst->operands[1].dataType);
                llvm::Value *isLt = dynCompare(cg_, DynCmp::LT, dl, dr);
                llvm::Value *isGt = dynCompare(cg_, DynCmp::GT, dl, dr);
                llvm::Value *dres = cg_.builder_->CreateSelect(
                    isLt, llvm::ConstantInt::get(i64Ty, -1),
                    cg_.builder_->CreateSelect(isGt, llvm::ConstantInt::get(i64Ty, 1), zero64,
                                               "str.ord.dyn.gt"),
                    "str.ord.dyn");
                if (inst->result.has_value())
                    cg_.context_info_.namedValues[inst->result->name] = dres;
                return dres;
            }

            // (AR) الحارسُ نفسُه المستعمَل في STRING_CMP: قيمةٌ واصلةٌ من قناةٍ قد تحمل حارسَ
            //      العدمِ أو عددًا موسومًا (bit63=1) لا مؤشّرَ نصّ، وstrcmp عليها ⇒ انهيار.
            //      في هذه الحالةِ المنحطّة نُرجِع ٠ (لا تمييزَ بين الطرفين) بدل مقارنةِ
            //      عنوانَين لا معنى لها — والترتيبُ بين نصٍّ وغيرِ نصٍّ خطأٌ دلاليٌّ أصلًا
            //      مكانُه الواجهةُ الأماميّة، لا اختراعُ جوابٍ هنا.
            // (EN) The same guard STRING_CMP uses: a value arriving from a channel may carry the
            //      null sentinel or a tagged integer (bit63=1) rather than a string pointer, and
            //      strcmp on it crashes. In that degenerate case we return 0 (the two are
            //      indistinguishable) rather than comparing two meaningless addresses — ordering a
            //      string against a non-string is a semantic error that belongs in the frontend,
            //      not an answer invented here.
            llvm::Value *leftI64 = left->getType()->isPointerTy()
                                       ? cg_.builder_->CreatePtrToInt(left, i64Ty, "str.ord.l.i64")
                                       : left;
            llvm::Value *rightI64 = right->getType()->isPointerTy()
                                        ? cg_.builder_->CreatePtrToInt(right, i64Ty, "str.ord.r.i64")
                                        : right;

            llvm::Value *leftOk = cg_.builder_->CreateAnd(
                cg_.builder_->CreateICmpEQ(
                    cg_.builder_->CreateAnd(leftI64, bit63Mask, "str.ord.l.tag"), zero64,
                    "str.ord.l.isstr"),
                cg_.builder_->CreateICmpNE(leftI64, zero64, "str.ord.l.nn"), "str.ord.l.ok");
            llvm::Value *rightOk = cg_.builder_->CreateAnd(
                cg_.builder_->CreateICmpEQ(
                    cg_.builder_->CreateAnd(rightI64, bit63Mask, "str.ord.r.tag"), zero64,
                    "str.ord.r.isstr"),
                cg_.builder_->CreateICmpNE(rightI64, zero64, "str.ord.r.nn"), "str.ord.r.ok");
            llvm::Value *canStrcmp =
                cg_.builder_->CreateAnd(leftOk, rightOk, "str.ord.can");

            auto *entryBB = cg_.builder_->GetInsertBlock();
            auto *parentFunc = entryBB->getParent();
            auto *strcmpBB = llvm::BasicBlock::Create(*cg_.context_, "str.ord.do", parentFunc);
            auto *doneBB = llvm::BasicBlock::Create(*cg_.context_, "str.ord.done", parentFunc);
            cg_.builder_->CreateCondBr(canStrcmp, strcmpBB, doneBB);

            cg_.builder_->SetInsertPoint(strcmpBB);
            llvm::Value *leftPtr = left->getType()->isPointerTy()
                                       ? left
                                       : cg_.builder_->CreateIntToPtr(left, i8p, "str.ord.l");
            llvm::Value *rightPtr = right->getType()->isPointerTy()
                                        ? right
                                        : cg_.builder_->CreateIntToPtr(right, i8p, "str.ord.r");

            llvm::FunctionType *strcmpType = llvm::FunctionType::get(i32Ty, {i8p, i8p}, false);
            llvm::FunctionCallee strcmpFn = cg_.module_->getOrInsertFunction("strcmp", strcmpType);
            llvm::Value *raw =
                cg_.builder_->CreateCall(strcmpFn, {leftPtr, rightPtr}, "str.ord.raw");

            // (AR) تطبيعُ الإشارةِ إلى ‎-1/0/+1‎ بالضبط: المعيارُ C يضمن الإشارةَ لا المقدار،
            //      وglibc تُرجع فرقَ البايتين بينما نسختُنا الحرّةُ تُرجعه أيضًا — فلولا
            //      التطبيعُ لاختلفت القيمةُ بين المسارات، والحتميّةُ عبرَ المحرّكاتِ عقدٌ لغويّ.
            // (EN) Normalize the sign to exactly -1/0/+1: the C standard guarantees the sign, not
            //      the magnitude, and glibc returns the byte difference (as does our freestanding
            //      copy) — without normalization the value would differ between tracks, and
            //      determinism across engines is a language contract.
            llvm::Value *sign = cg_.builder_->CreateSub(
                cg_.builder_->CreateZExt(
                    cg_.builder_->CreateICmpSGT(raw, zero32, "str.ord.pos"), i64Ty, "str.ord.pos64"),
                cg_.builder_->CreateZExt(
                    cg_.builder_->CreateICmpSLT(raw, zero32, "str.ord.neg"), i64Ty, "str.ord.neg64"),
                "str.ord.sign");
            // (AR) dynCompare/strcmp قد يتفرّعان ⇒ أعِد التقاط كتلةِ الخروجِ الفعليّة قبل PHI.
            // (EN) The call path may branch ⇒ re-capture the real exit block before the PHI.
            auto *strcmpExitBB = cg_.builder_->GetInsertBlock();
            cg_.builder_->CreateBr(doneBB);

            cg_.builder_->SetInsertPoint(doneBB);
            auto *result = cg_.builder_->CreatePHI(i64Ty, 2, "str.ord.phi");
            result->addIncoming(sign, strcmpExitBB);
            result->addIncoming(zero64, entryBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================
        // Phase N: Builtin String Functions / دوال النصوص المضمنة
        // ============================================================================

        // ════════════════════════════════════════════════════════════════════
        // (AR) حارسُ مدى التحويلِ النصّيِّ إلى عدد — مُشترَكٌ بين الصحيحِ والعشريّ.
        //
        //      يصفّرُ `errno`، ثمّ يُنادى المحوِّلُ، ثمّ يُقرأُ `errno`: إن كان
        //      `ERANGE` رُمِيَ استثناءٌ يلتقطه `امسك`، وإلّا مضى التنفيذُ بالقيمة.
        //      والرميُ لا يُتبَعُ بقفزةٍ إلى كتلةِ النجاح: باعثُ `__sad_raise` ينتهي
        //      بـ`unreachable` (إمّا longjmp وإمّا تقريرُ «لم يلتقطه أحد» ثمّ خروج)،
        //      فأيُّ فرعٍ نضيفه بعده يجعلُ للكتلةِ خاتمتَين ويردُّه المُدقِّق.
        //
        //      والقياسُ لا الادّعاء: `errno` يُقرأُ بعد النداءِ مباشرةً في الكتلةِ
        //      نفسِها، فلا شيءَ بينهما يمكن أن يكتبَه.
        // (EN) The string-to-number range guard — shared by the integer and float paths.
        //
        //      It zeroes `errno`, calls the converter, then reads `errno`: `ERANGE` raises
        //      an exception that `catch` can see, otherwise execution continues with the
        //      value. The raise is not followed by a branch into the success block: the
        //      `__sad_raise` emitter ends in `unreachable` (either a longjmp or the
        //      "nobody caught this" report and exit), so any branch added after it would
        //      give the block two terminators and the verifier would reject it.
        //
        //      Measured, not assumed: `errno` is read immediately after the call in the
        //      same block, so nothing in between can write it.
        // ════════════════════════════════════════════════════════════════════
        namespace
        {
            const char *errnoLocationSymbol(const llvm::Module &module)
            {
                const llvm::Triple triple(llvm::Triple::normalize(module.getTargetTriple()));
                if (triple.isOSWindows())
                    return ::Sad::Compiler::kErrnoLocationWindows;
                if (triple.isOSDarwin())
                    return ::Sad::Compiler::kErrnoLocationDarwin;
                return ::Sad::Compiler::kErrnoLocationPosix;
            }
        } // namespace

        llvm::Value *StringOpsCodeGen::emitNumericRangeErrnoSlot()
        {
            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto *i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto *errnoTy = llvm::FunctionType::get(ptrTy, {}, false);
            auto errnoFn =
                cg_.module_->getOrInsertFunction(errnoLocationSymbol(*cg_.module_), errnoTy);
            llvm::Value *slot = cg_.builder_->CreateCall(errnoFn, {}, "num.errno.slot");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i32Ty, 0), slot);
            return slot;
        }

        void StringOpsCodeGen::emitNumericRangeCheck(llvm::Value *errnoSlot, const char *label)
        {
            auto *i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            llvm::Value *code = cg_.builder_->CreateLoad(i32Ty, errnoSlot,
                                                         std::string(label) + ".errno");
            llvm::Value *outOfRange = cg_.builder_->CreateICmpEQ(
                code, llvm::ConstantInt::get(i32Ty, ::Sad::Compiler::kErrnoRangeError),
                std::string(label) + ".range");

            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *raiseBB =
                llvm::BasicBlock::Create(*cg_.context_, std::string(label) + ".range.raise", curFunc);
            llvm::BasicBlock *okBB =
                llvm::BasicBlock::Create(*cg_.context_, std::string(label) + ".range.ok", curFunc);
            cg_.builder_->CreateCondBr(outOfRange, raiseBB, okBB);

            cg_.builder_->SetInsertPoint(raiseBB);
            std::vector<llvm::Value *> raiseArgs;
            raiseArgs.push_back(cg_.getConstantString(::Sad::Compiler::kNumberOutOfRangeMsg));
            cg_.emitCallException(::Sad::Compiler::kRuntimeRaise, raiseArgs, nullptr);

            cg_.builder_->SetInsertPoint(okBB);
        }

        llvm::Value *StringOpsCodeGen::emitNumericParseEndSlot(const char *label)
        {
            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock &entry = curFunc->getEntryBlock();
            llvm::IRBuilder<> entryBuilder(&entry, entry.getFirstInsertionPt());
            return entryBuilder.CreateAlloca(ptrTy, nullptr, std::string(label) + ".end");
        }

        void StringOpsCodeGen::emitNumericParseCheck(llvm::Value *endSlot, llvm::Value *sourcePtr,
                                                     const char *label)
        {
            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            llvm::Value *end = cg_.builder_->CreateLoad(ptrTy, endSlot,
                                                        std::string(label) + ".endptr");
            // (AR) لا محرفَ استُهلِك ⇒ النصُّ ليس عددًا. المقارنةُ بالمؤشّرِ نفسِه الذي
            //      مُرّر للمحوِّل — لا بنسخةٍ منه — وإلّا قارنّا عنوانَين مختلفَين لنصٍّ واحد.
            // (EN) Nothing consumed ⇒ the string is not a number. Compared against the very
            //      pointer handed to the converter, not a copy, or we would be comparing two
            //      different addresses of one string.
            llvm::Value *notNumeric =
                cg_.builder_->CreateICmpEQ(end, sourcePtr, std::string(label) + ".notnum");

            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *raiseBB = llvm::BasicBlock::Create(
                *cg_.context_, std::string(label) + ".notnum.raise", curFunc);
            llvm::BasicBlock *okBB = llvm::BasicBlock::Create(
                *cg_.context_, std::string(label) + ".notnum.ok", curFunc);
            cg_.builder_->CreateCondBr(notNumeric, raiseBB, okBB);

            cg_.builder_->SetInsertPoint(raiseBB);
            std::vector<llvm::Value *> raiseArgs;
            raiseArgs.push_back(cg_.getConstantString(::Sad::Compiler::kNumberNotNumericMsg));
            cg_.emitCallException(::Sad::Compiler::kRuntimeRaise, raiseArgs, nullptr);

            cg_.builder_->SetInsertPoint(okBB);
        }

        llvm::Value *StringOpsCodeGen::emitStringToI64(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "STRING_TO_I64"}});
                return nullptr;
            }
            llvm::Value *str = cg_.resolveOperand(inst->operands[0]);
            if (!str)
                return nullptr;

            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            llvm::Value *result = nullptr;

            if (cg_.freestanding_)
            {
                // (AR) الوضعُ الحرُّ: لا `errno` ولا استثناءات — المسارُ القديمُ بلا مساس.
                // (EN) Freestanding: no `errno`, no exceptions — the old path, untouched.
                auto *atollType = llvm::FunctionType::get(cg_.getInt64Type(), {ptrTy}, false);
                auto atollFunc = cg_.module_->getOrInsertFunction("atoll", atollType);
                result = cg_.builder_->CreateCall(atollFunc, {str}, "str2i64");
            }
            else
            {
                // (AR) `strtoll(str, NULL, 10)` مع تصفيرِ `errno` قبلَه وفحصِه بعدَه.
                // (EN) `strtoll(str, NULL, 10)` with `errno` zeroed before and checked after.
                llvm::Value *errnoSlot = emitNumericRangeErrnoSlot();
                llvm::Value *endSlot = emitNumericParseEndSlot("str2i64");
                auto *strtollType = llvm::FunctionType::get(
                    cg_.getInt64Type(),
                    {ptrTy, ptrTy, llvm::Type::getInt32Ty(*cg_.context_)}, false);
                auto strtollFunc = cg_.module_->getOrInsertFunction(
                    ::Sad::Compiler::kLibcStringToLongLong, strtollType);
                result = cg_.builder_->CreateCall(
                    strtollFunc,
                    {str, endSlot,
                     llvm::ConstantInt::get(llvm::Type::getInt32Ty(*cg_.context_),
                                            ::Sad::Compiler::kDecimalBase)},
                    "str2i64");
                emitNumericRangeCheck(errnoSlot, "str2i64");
                emitNumericParseCheck(endSlot, str, "str2i64");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }



        // ============================================================================
        // (AR) تحويل نص إلى عدد عشري — STRING_TO_F64
        //      نستدعي دالة atof() من مكتبة C القياسية التي تحوّل نصاً مثل "3.14"
        //      إلى قيمة عشرية مزدوجة الدقة (double/f64)
        //      هذا ضروري لدالة لعشري() المضمنة في لغة ص
        //
        // (EN) Convert string to float — STRING_TO_F64
        //      Calls C standard library atof() which converts a string like "3.14"
        //      to a double-precision floating point value (f64)
        //      Required for the built-in لعشري() function in Sad language
        // ============================================================================
        llvm::Value *StringOpsCodeGen::emitStringToF64(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "STRING_TO_F64"}});
                return nullptr;
            }
            // (AR) هذا منفذُ **تحويلٍ** لا طريقةً، والمفسّرُ يفرّق بينهما مقيسًا:
            //      «عشري(س)» على عدمٍ ⇒ RUN037 باسمِ الدالّة، لا RUN033 باسمِ العمليّة.
            //      فلا يمرُّ بـ`normalizeStringPtr` (وهي لمنافذِ الطرائق) بل يُنادي البابَ
            //      برمزِه. وقبلَ هذا كان يُبدِل العدمَ باللفظِ ثمّ يمرّره إلى `atof`
            //      فيطبع «0.0» صامتًا — جوابٌ كاذبٌ حيث يرفعُ المفسّر (مقيس).
            // (EN) A conversion port, not a method: the interpreter raises RUN037 here
            //      naming the builtin. Previously it silently answered 0.0 via atof.
            llvm::Value *str = cg_.emitStringPtrOrRaise(
                cg_.resolveOperand(inst->operands[0]),
                std::string(Sad::Builtins::Names::TypeCtor::TO_FLOAT),
                "to_f64.src",
                ::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            if (!str)
                return nullptr;

            auto *ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto *doubleTy = llvm::Type::getDoubleTy(*cg_.context_);
            llvm::Value *result = nullptr;

            if (cg_.freestanding_)
            {
                // (AR) الوضعُ الحرُّ: المسارُ القديمُ كما هو — انظر نظيرَه في التحويلِ الصحيح.
                // (EN) Freestanding: the old path unchanged — see its twin in the integer path.
                auto *atofType = llvm::FunctionType::get(doubleTy, {ptrTy}, false);
                auto atofFunc = cg_.module_->getOrInsertFunction("atof", atofType);
                result = cg_.builder_->CreateCall(atofFunc, {str}, "str2f64");
            }
            else
            {
                // (AR) و`ERANGE` هنا يشملُ الطرفَين: «1e309» طفحًا و«1e-400» هبوطًا —
                //      وكلاهما جيسونٌ صالحٌ خارجَ مدى `عشري`، وكلاهما يرمي في المفسّر.
                // (EN) `ERANGE` here covers both ends: "1e309" overflow and "1e-400"
                //      underflow — both valid JSON outside the float range, and both raise
                //      in the interpreter.
                llvm::Value *errnoSlot = emitNumericRangeErrnoSlot();
                llvm::Value *endSlot = emitNumericParseEndSlot("str2f64");
                auto *strtodType = llvm::FunctionType::get(doubleTy, {ptrTy, ptrTy}, false);
                auto strtodFunc = cg_.module_->getOrInsertFunction(
                    ::Sad::Compiler::kLibcStringToDouble, strtodType);
                result = cg_.builder_->CreateCall(strtodFunc, {str, endSlot}, "str2f64");
                emitNumericRangeCheck(errnoSlot, "str2f64");
                emitNumericParseCheck(endSlot, str, "str2f64");
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringLength(std::shared_ptr<SIRInstruction> inst)
        {
            return cg_.emitFFIStrlen(inst);
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringToUpper(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *str = normalizeStringPtr(cg_.resolveOperand(inst->operands[0]), "upper.src",
                                                         Sad::Builtins::Names::TypeMethods::String::TO_UPPER);
            if (!str)
                return nullptr;

            // strlen + malloc + loop calling toupper
            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);

            llvm::Value *len = cg_.emitStrlen(str, "len");

            llvm::Value *newLen = cg_.builder_->CreateAdd(len, llvm::ConstantInt::get(i64Ty, 1));
            llvm::Value *buf = cg_.emitMalloc(newLen, "upper_buf");

            // strcpy then loop toupper
            auto *strcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto strcpyFunc = cg_.module_->getOrInsertFunction("strcpy", strcpyType);
            cg_.builder_->CreateCall(strcpyFunc, {buf, str});

            // (AR) تحويل الحالة عبر دالّة زمن التشغيل المحمولة sad_llvm_str_upper
            //      (ASCII بايتيّ، تطابق مسار المفسّر الاحتياطيّ). كان _strupr رمز
            //      MSVC غير قياسيّ يكسر الربط على Linux/macOS.
            // (EN) Case-convert via the portable runtime sad_llvm_str_upper
            //      (byte-wise ASCII, matching the interpreter fallback). _strupr
            //      was a non-standard MSVC symbol that broke linking on Linux/macOS.
            auto *struprType = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
            auto struprFunc = cg_.module_->getOrInsertFunction("sad_llvm_str_upper", struprType);
            cg_.builder_->CreateCall(struprFunc, {buf});

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringToLower(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *str = normalizeStringPtr(cg_.resolveOperand(inst->operands[0]), "lower.src",
                                                         Sad::Builtins::Names::TypeMethods::String::TO_LOWER);
            if (!str)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();

            llvm::Value *len = cg_.emitStrlen(str, "len");

            llvm::Value *newLen = cg_.builder_->CreateAdd(len, llvm::ConstantInt::get(i64Ty, 1));
            llvm::Value *buf = cg_.emitMalloc(newLen, "lower_buf");

            auto *strcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto strcpyFunc = cg_.module_->getOrInsertFunction("strcpy", strcpyType);
            cg_.builder_->CreateCall(strcpyFunc, {buf, str});

            // (AR) نظيرة الحالة الصغيرة عبر sad_llvm_str_lower المحمولة (بديل _strlwr).
            // (EN) Lowercase counterpart via portable sad_llvm_str_lower (replaces _strlwr).
            auto *strlwrType = llvm::FunctionType::get(ptrTy, {ptrTy}, false);
            auto strlwrFunc = cg_.module_->getOrInsertFunction("sad_llvm_str_lower", strlwrType);
            cg_.builder_->CreateCall(strlwrFunc, {buf});

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringFind(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "STRING_FIND"}});
                return nullptr;
            }
            llvm::Value *haystack = normalizeStringPtr(cg_.resolveOperand(inst->operands[0]), "find.hay",
                                                       Sad::Builtins::Names::TypeMethods::String::FIND);
            llvm::Value *needle = normalizeStringArgPtr(cg_.resolveOperand(inst->operands[1]), "find.needle");
            if (!haystack || !needle)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();

            // strstr(haystack, needle) → ptr or null
            auto *strstrType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto strstrFunc = cg_.module_->getOrInsertFunction("strstr", strstrType);
            llvm::Value *found = cg_.builder_->CreateCall(strstrFunc, {haystack, needle}, "found");

            // (AR) حساب الإزاحة بالبايت ثم تحويلها إلى فهرس حرف UTF-8
            // (EN) Calculate byte offset then convert to UTF-8 character index
            llvm::Value *isNull = cg_.builder_->CreateICmpEQ(found,
                                                         llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(*cg_.context_)), "isnull");
            llvm::Value *foundInt = cg_.builder_->CreatePtrToInt(found, i64Ty, "found.int");
            llvm::Value *hstackInt = cg_.builder_->CreatePtrToInt(haystack, i64Ty, "hstack.int");
            llvm::Value *byteOffset = cg_.builder_->CreateSub(foundInt, hstackInt, "byte.offset");

            // (AR) تحويل موقع البايت إلى فهرس الحرف UTF-8
            // (EN) Convert byte offset to UTF-8 character index
            llvm::Function *byteToCharFn = cg_.getOrCreateUtf8ByteToChar();
            llvm::Value *charIndex = cg_.builder_->CreateCall(byteToCharFn, {haystack, byteOffset}, "char.index");

            // (AR) إذا لم يُعثر → -1، وإلا → فهرس الحرف
            llvm::Value *result = cg_.builder_->CreateSelect(isNull,
                                                         llvm::ConstantInt::get(i64Ty, -1), charIndex, "find_result");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringReplace(std::shared_ptr<SIRInstruction> inst)
        {
            // Full string replace: call runtime helper sad_string_replace(str, old, new) -> char*
            if (!inst || inst->operands.size() < 3)
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "STRING_REPLACE"}});
                return nullptr;
            }
            llvm::Value *str = normalizeStringPtr(cg_.resolveOperand(inst->operands[0]), "replace.src",
                                                  Sad::Builtins::Names::TypeMethods::String::REPLACE);
            llvm::Value *oldStr = normalizeStringArgPtr(cg_.resolveOperand(inst->operands[1]), "replace.old");
            // (AR) المعامِلُ الثالثُ كان **يتجاوز البابَ** فيبلغ `strlen` خامًّا (السطرُ أدناه):
            //      «س.استبدل("أ"، ب)» وب عدميّةٌ ⇒ انهيارُ تجزئة. «بابٌ واحدٌ» يعني كلَّ
            //      المعاملات لا أوّلَها. كشفته مراجعةٌ خصميّة.
            // (EN) The third operand bypassed the door and reached strlen raw.
            llvm::Value *newStr = normalizeStringArgPtr(cg_.resolveOperand(inst->operands[2]), "replace.new");
            if (!str || !oldStr || !newStr)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);

            // Build inline: find first occurrence with strstr, copy before + new + after
            // Allocate generous buffer: strlen(str) * 2 + strlen(newStr) + 1
            llvm::Value *srcLen = cg_.emitStrlen(str, "src.len");
            llvm::Value *oldLen = cg_.emitStrlen(oldStr, "old.len");
            llvm::Value *newLen = cg_.emitStrlen(newStr, "new.len");

            // bufSize = srcLen * 2 + newLen + 1 (generous)
            llvm::Value *bufSize = cg_.builder_->CreateMul(srcLen, llvm::ConstantInt::get(i64Ty, 2));
            bufSize = cg_.builder_->CreateAdd(bufSize, newLen);
            bufSize = cg_.builder_->CreateAdd(bufSize, llvm::ConstantInt::get(i64Ty, 1));

            llvm::Value *buf = cg_.emitMalloc(bufSize, "replace_buf");

            // Use strstr to find oldStr in str
            auto *strstrType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto strstrFunc = cg_.module_->getOrInsertFunction("strstr", strstrType);
            llvm::Value *found = cg_.builder_->CreateCall(strstrFunc, {str, oldStr}, "found");

            // If not found, just copy original
            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *foundBB = llvm::BasicBlock::Create(*cg_.context_, "replace.found", curFunc);
            llvm::BasicBlock *notFoundBB = llvm::BasicBlock::Create(*cg_.context_, "replace.notfound", curFunc);
            llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(*cg_.context_, "replace.merge", curFunc);

            llvm::Value *isNull = cg_.builder_->CreateICmpEQ(found,
                                                         llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)), "isnull");
            cg_.builder_->CreateCondBr(isNull, notFoundBB, foundBB);

            // Not found: strcpy original
            cg_.builder_->SetInsertPoint(notFoundBB);
            auto *strcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto strcpyFunc = cg_.module_->getOrInsertFunction("strcpy", strcpyType);
            cg_.builder_->CreateCall(strcpyFunc, {buf, str});
            cg_.builder_->CreateBr(mergeBB);

            // Found: copy prefix + newStr + suffix
            cg_.builder_->SetInsertPoint(foundBB);
            llvm::Value *prefixLen = cg_.builder_->CreatePtrDiff(i8Ty, found, str, "prefix.len");
            // (AR) طول ‎memcpy‎ بنوع ‎size_t‎ الهدف (i32 على 32-بت) — يطابق
            //      النداء المكتبيّ الذي تولّده الخلفيّة وتعريفَ وقت التشغيل الحرّ.
            llvm::Type *szTy = cg_.getSizeType();
            auto *memcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, szTy}, false);
            auto memcpyFunc = cg_.module_->getOrInsertFunction("memcpy", memcpyType);
            cg_.builder_->CreateCall(memcpyFunc, {buf, str,
                cg_.builder_->CreateZExtOrTrunc(prefixLen, szTy, "prefix.len.sz")}); // copy prefix
            llvm::Value *dst1 = cg_.builder_->CreateGEP(i8Ty, buf, {prefixLen}, "dst1");
            cg_.builder_->CreateCall(memcpyFunc, {dst1, newStr,
                cg_.builder_->CreateZExtOrTrunc(newLen, szTy, "new.len.sz")}); // copy newStr
            llvm::Value *dst2 = cg_.builder_->CreateGEP(i8Ty, dst1, {newLen}, "dst2");
            llvm::Value *suffixStart = cg_.builder_->CreateGEP(i8Ty, found, {oldLen}, "suffix.start");
            llvm::Value *suffixLen = cg_.builder_->CreateSub(srcLen, cg_.builder_->CreateAdd(prefixLen, oldLen));
            llvm::Value *suffixCopyLen = cg_.builder_->CreateAdd(suffixLen, llvm::ConstantInt::get(i64Ty, 1)); // include null
            cg_.builder_->CreateCall(memcpyFunc, {dst2, suffixStart,
                cg_.builder_->CreateZExtOrTrunc(suffixCopyLen, szTy, "suffix.len.sz")});
            cg_.builder_->CreateBr(mergeBB);

            cg_.builder_->SetInsertPoint(mergeBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringSubstring(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) 🔑 الوسائطُ **اختياريّةٌ في اللغة**، وكان اشتراطُ ثلاثةٍ هنا يجعل
            //      «"أهلا".جزء()» علّةَ مترجّمٍ داخليّةً لا تُفشِل البناء، فيطبعُ
            //      البرنامجُ «لاشيء» بينما يطبعُ المفسّرُ «أهلا» — كذبٌ صامتٌ لا انهيار.
            //
            // (AR) ⚠️ ودرسٌ ثانٍ من مراجعةٍ خصميّة: أوّلُ رقعةٍ نقلت **الافتراضَ** وحدَه
            //      وتركت **القصَّ**، فصارت «"أهلا".جزء(-2)» تطبع «أهلا» بينما يطبعُ
            //      المفسّرُ «لا» — أي أنّ الرقعةَ حوّلت خطأً ظاهرًا («لاشيء») إلى جوابٍ
            //      يبدو صحيحًا وليس هو. فنُقل الحسابُ كلُّه: الافتراضاتُ **والقصُّ**.
            // (EN) 🔑 The arguments are OPTIONAL in the language; requiring three made
            //      «"أهلا".جزء()» an internal compiler error that did NOT fail the build.
            // (EN) ⚠️ The first patch ported the DEFAULTS and not the CLAMPING, so
            //      «"أهلا".جزء(-2)» answered «أهلا» where the interpreter answers «لا» —
            //      trading a visible error for a plausible-looking lie. Both are ported now.
            if (!inst || inst->operands.empty())
            {
                cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "STRING_SUBSTRING"}});
                return nullptr;
            }
            const bool hasStart = inst->operands.size() > 1;
            const bool hasLength = inst->operands.size() > 2;
            // (AR) بابُ العمليّة لا العرض: قِيس أنّ «س.جزء(0، 2)» على خانةٍ عدميّةٍ كان
            //      يُعطي «لا» — أوّلَ حرفَين من لفظِ «لاشيء» — بينما المفسّرُ يرفعُ RUN033.
            // (EN) Operation door: «س.جزء(0،2)» on a null slot fabricated «لا» (measured).
            llvm::Value *str = cg_.emitStringPtrOrRaise(
                cg_.resolveOperand(inst->operands[0]),
                LLVMCodeGen::stringMethodOperationLabel(
                    Sad::Builtins::Names::TypeMethods::String::SUBSTRING),
                "substr.src");
            if (!str)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();

            llvm::Value *rawStart = hasStart ? cg_.resolveOperand(inst->operands[1])
                                             : llvm::ConstantInt::get(i64Ty, 0);
            llvm::Value *rawLength = hasLength ? cg_.resolveOperand(inst->operands[2]) : nullptr;
            if (!rawStart || (hasLength && !rawLength))
                return nullptr;

            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::IRBuilder<> &b = *cg_.builder_;
            auto constant = [&](int64_t value) { return llvm::ConstantInt::get(i64Ty, value); };
            auto signedMax = [&](llvm::Value *a, llvm::Value *c, const char *tag) {
                return b.CreateSelect(b.CreateICmpSGT(a, c), a, c, tag);
            };
            auto signedMin = [&](llvm::Value *a, llvm::Value *c, const char *tag) {
                return b.CreateSelect(b.CreateICmpSLT(a, c), a, c, tag);
            };

            // ────────────────────────────────────────────────────────────────
            // (AR) قصُّ المدى كما يقصُّه المفسّرُ بالترتيبِ نفسِه — والترتيبُ ليس
            //      تفصيلًا: الطولُ الافتراضيُّ يُحسَبُ من البدايةِ **قبل** قصِّها،
            //      فـ«"أهلا".جزء(-2)» طولُها ٤-(-٢)=٦ ثمّ بدايتُها ٢ ثمّ يُقصُّ
            //      الطولُ إلى ٢ ⇒ «لا». ولو قُصَّت البدايةُ أوّلًا لخرج «لا» خطأً
            //      من طريقٍ آخرَ ثمّ افترقَ في مدخلٍ ثالث.
            //      المرجع: expression_evaluator_oop_string_map_methods.cpp — «جزء».
            // (EN) Clamp exactly as the interpreter does, in the same ORDER: the default
            //      length is computed from the UNCLAMPED start, so «"أهلا".جزء(-2)» is
            //      length 4-(-2)=6, then start 2, then length clamped to 2 ⇒ «لا».
            // ────────────────────────────────────────────────────────────────
            llvm::Value *charCount =
                b.CreateCall(cg_.getOrCreateUtf8Strlen(), {str}, "substr.chars");

            llvm::Value *length =
                hasLength ? rawLength : b.CreateSub(charCount, rawStart, "substr.len.rest");

            // (AR) بدايةٌ سالبةٌ تُعَدُّ من الآخر، ولا تنزلُ تحتَ الصفر ولا تتجاوزُ الطول.
            // (EN) A negative start counts from the end; clamped into [0, charCount].
            llvm::Value *wrapped = b.CreateAdd(charCount, rawStart, "substr.start.wrap");
            llvm::Value *fromEnd = signedMax(wrapped, constant(0), "substr.start.wrap.nn");
            llvm::Value *start = b.CreateSelect(
                b.CreateICmpSLT(rawStart, constant(0)), fromEnd, rawStart, "substr.start.pick");
            start = signedMin(start, charCount, "substr.start");

            // (AR) طولٌ سالبٌ صفرٌ، ولا يتجاوزُ ما بقي من النصّ.
            // (EN) A negative length is zero, and never exceeds what remains.
            length = signedMax(length, constant(0), "substr.len.nn");
            length = signedMin(length, b.CreateSub(charCount, start, "substr.len.max"),
                               "substr.len");

            // (AR) تحويل فهرس الحرف والطول إلى إزاحات بايت باستخدام UTF-8
            // (EN) Convert character start index and length to byte offsets using UTF-8
            llvm::Function *charToByteFn = cg_.getOrCreateUtf8CharToByte();

            // (AR) موقع البايت لبداية الاستخراج
            // (EN) Byte offset of start character
            llvm::Value *byteStart = b.CreateCall(charToByteFn, {str, start}, "byte.start");

            // (AR) موقع البايت لنهاية الاستخراج — والمدى مقصوصٌ سلفًا، فـ`start+length`
            //      لا يتجاوزُ عددَ المحارفِ بحال، ولا يُسأل التحويلُ عمّا لا عقدَ له فيه.
            // (EN) End byte; the range is already clamped, so start+length never exceeds the
            //      character count and the char→byte conversion is never asked out of range.
            llvm::Value *charEnd = b.CreateAdd(start, length, "char.end");
            llvm::Value *byteEnd = b.CreateCall(charToByteFn, {str, charEnd}, "byte.end");

            // (AR) طول البايتات = byteEnd - byteStart
            // (EN) Byte length = byteEnd - byteStart
            llvm::Value *byteLen = cg_.builder_->CreateSub(byteEnd, byteStart, "byte.len");

            // (AR) حجز ذاكرة: byteLen + 1
            llvm::Value *bufSize = cg_.builder_->CreateAdd(byteLen, llvm::ConstantInt::get(i64Ty, 1));
            llvm::Value *buf = cg_.emitMalloc(bufSize, "substr_buf");

            // (AR) مؤشر المصدر: str + byteStart
            llvm::Value *srcPtr = cg_.builder_->CreateGEP(i8Ty, str, {byteStart}, "substr.src");

            // memcpy(buf, srcPtr, byteLen)
            // (AR) طول ‎memcpy‎ بنوع ‎size_t‎ الهدف (i32 على 32-بت) — يطابق
            //      النداء المكتبيّ الذي تولّده الخلفيّة وتعريفَ وقت التشغيل الحرّ.
            llvm::Type *szTy = cg_.getSizeType();
            auto *memcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, szTy}, false);
            auto memcpyFunc = cg_.module_->getOrInsertFunction("memcpy", memcpyType);
            cg_.builder_->CreateCall(memcpyFunc, {buf, srcPtr,
                cg_.builder_->CreateZExtOrTrunc(byteLen, szTy, "substr.len.sz")});

            // (AR) إنهاء النص بـ null
            llvm::Value *endPtr = cg_.builder_->CreateGEP(i8Ty, buf, {byteLen}, "substr.end");
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), endPtr);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringTrim(std::shared_ptr<SIRInstruction> inst)
        {
            // Call C runtime: skip leading whitespace, then copy until trailing whitespace
            if (!inst || inst->operands.empty())
                return nullptr;
            llvm::Value *str = normalizeStringPtr(cg_.resolveOperand(inst->operands[0]), "trim.src",
                                                  Sad::Builtins::Names::TypeMethods::String::TRIM);
            if (!str)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

            // Use strspn to find leading whitespace count, then strlen-based trim end
            // strspn(str, " \t\n\r") returns number of leading whitespace chars
            // (AR) ‎strspn‎ يعيد ‎size_t‎؛ إعلانه i64 على 32-بت يقرأ ‎edx‎ قمامةً
            //      كعدد المسافات البادئة ⇒ ‎GEP‎ خارج النصّ. نمدّده بعد النداء.
            auto *strspnType = llvm::FunctionType::get(cg_.getSizeType(), {ptrTy, ptrTy}, false);
            auto strspnFunc = cg_.module_->getOrInsertFunction("strspn", strspnType);
            llvm::Value *ws = cg_.builder_->CreateGlobalStringPtr(" \t\n\r", "ws_chars");
            llvm::Value *leadingWS = cg_.builder_->CreateZExtOrTrunc(
                cg_.builder_->CreateCall(strspnFunc, {str, ws}, "leading.ws.sz"),
                i64Ty, "leading.ws");

            // start = str + leadingWS
            llvm::Value *start = cg_.builder_->CreateGEP(i8Ty, str, {leadingWS}, "trim.start");

            // Get length of remaining string
            llvm::Value *remLen = cg_.emitStrlen(start, "rem.len");

            // Allocate buffer: remLen + 1
            llvm::Value *bufSize = cg_.builder_->CreateAdd(remLen, llvm::ConstantInt::get(i64Ty, 1));
            llvm::Value *buf = cg_.emitMalloc(bufSize, "trim_buf");

            // memcpy start content
            // (AR) طول ‎memcpy‎ بنوع ‎size_t‎ الهدف (i32 على 32-بت) — يطابق
            //      النداء المكتبيّ الذي تولّده الخلفيّة وتعريفَ وقت التشغيل الحرّ.
            llvm::Type *szTy = cg_.getSizeType();
            auto *memcpyType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, szTy}, false);
            auto memcpyFunc = cg_.module_->getOrInsertFunction("memcpy", memcpyType);
            cg_.builder_->CreateCall(memcpyFunc, {buf, start,
                cg_.builder_->CreateZExtOrTrunc(bufSize, szTy, "trim.size.sz")});

            // Trim trailing whitespace: walk back from end while isspace
            // Simple approach: create loop to null-terminate at first non-whitespace from end
            // For simplicity, use a runtime helper pattern: buf[len] scanning back
            // We'll call isspace on each char from the end
            auto *isSpaceType = llvm::FunctionType::get(i32Ty, {i32Ty}, false);
            auto isSpaceFunc = cg_.module_->getOrInsertFunction("isspace", isSpaceType);

            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "trim.loop", curFunc);
            llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*cg_.context_, "trim.body", curFunc);
            llvm::BasicBlock *doneBB = llvm::BasicBlock::Create(*cg_.context_, "trim.done", curFunc);

            // idx = remLen - 1
            llvm::Value *startIdx = cg_.builder_->CreateSub(remLen, llvm::ConstantInt::get(i64Ty, 1));
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(loopBB);
            llvm::PHINode *idx = cg_.builder_->CreatePHI(i64Ty, 2, "trim.idx");
            idx->addIncoming(startIdx, loopBB->getSinglePredecessor());

            // if idx < 0, done
            llvm::Value *isNeg = cg_.builder_->CreateICmpSLT(idx, llvm::ConstantInt::get(i64Ty, 0), "is.neg");
            cg_.builder_->CreateCondBr(isNeg, doneBB, bodyBB);

            cg_.builder_->SetInsertPoint(bodyBB);
            llvm::Value *charPtr = cg_.builder_->CreateGEP(i8Ty, buf, {idx}, "char.ptr");
            llvm::Value *ch = cg_.builder_->CreateLoad(i8Ty, charPtr, "ch");
            llvm::Value *chInt = cg_.builder_->CreateZExt(ch, i32Ty, "ch.int");
            llvm::Value *isSp = cg_.builder_->CreateCall(isSpaceFunc, {chInt}, "is.sp");
            llvm::Value *isSpBool = cg_.builder_->CreateICmpNE(isSp, llvm::ConstantInt::get(i32Ty, 0), "is.sp.bool");

            // If space, null-terminate and continue
            llvm::BasicBlock *trimBB = llvm::BasicBlock::Create(*cg_.context_, "trim.set", curFunc);
            cg_.builder_->CreateCondBr(isSpBool, trimBB, doneBB);

            cg_.builder_->SetInsertPoint(trimBB);
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), charPtr);
            llvm::Value *nextIdx = cg_.builder_->CreateSub(idx, llvm::ConstantInt::get(i64Ty, 1));
            idx->addIncoming(nextIdx, trimBB);
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(doneBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }


        llvm::Value *StringOpsCodeGen::emitBuiltinStringSplit(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) «تقسيم»/split: يفصل النصّ على الفاصل كسلسلة فرعيّة كاملة (مطابقًا
            //      للمفسّر StringFunctions::split عبر str.find/substr) — يُبقي
            //      الأجزاء الفارغة، يدعم الوسيط الثالث maxSplits، والفاصل الفارغ
            //      يقسم بأحرف UTF-8. المنطق كلّه في @__sad_string_split الموحَّد
            //      (يزيل تبعيّة strtok البايتيّة التي كانت تُهشّم الفواصل العربيّة).
            // (EN) split: substring-delimiter semantics matching the interpreter
            //      (keeps empties, honours maxSplits, empty delim → UTF-8 chars).
            //      All logic in the unified @__sad_string_split (drops the
            //      byte-wise strtok that shattered multibyte delimiters).
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *str = normalizeStringPtr(cg_.resolveOperand(inst->operands[0]), "split.src",
                                                  Sad::Builtins::Names::TypeMethods::String::SPLIT);
            llvm::Value *delim = normalizeStringArgPtr(cg_.resolveOperand(inst->operands[1]), "split.delim");
            if (!str || !delim)
                return nullptr;

            auto i64Ty = cg_.getInt64Type();

            // (AR) الوسيط الثالث الاختياريّ maxSplits (‎-1 = بلا حدّ، الافتراض).
            // (EN) Optional 3rd arg maxSplits (-1 = unlimited default).
            llvm::Value *maxSplits = llvm::ConstantInt::get(i64Ty, -1);
            if (inst->operands.size() >= 3)
            {
                if (llvm::Value *ms = cg_.resolveOperand(inst->operands[2]))
                {
                    if (ms->getType()->isIntegerTy())
                    {
                        maxSplits = cg_.builder_->CreateSExtOrTrunc(ms, i64Ty, "split.max");
                    }
                }
            }

            llvm::Function *splitFn = ensureStringSplitHelper();
            if (!splitFn)
                return nullptr;

            llvm::Value *arrPtr =
                cg_.builder_->CreateCall(splitFn, {str, delim, maxSplits}, "split.arr");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = arrPtr;
            }
            return arrPtr;
        }

        // ============================================================================
        // (AR) ensureStringSplitHelper — يُصدر (مرّة) عائلة دوال «تقسيم» ذاتيّة
        //      الاحتواء ثمّ يعيد @__sad_string_split. تعتمد فقط على
        //      malloc/memcpy/strlen/realloc (متوفّرة مستضافةً عبر libc، وحرّةً عبر
        //      بدائيّات __sad المُصدَرة) فتتطابق دلالتها في الوضعين مع المفسّر.
        //      العائلة: __sad_utf8_clen، __sad_strstr، __sad_substr_dup،
        //      __sad_split_append، __sad_string_split.
        // (EN) Emits (once) the self-contained split helper family and returns
        //      @__sad_string_split. Depends only on malloc/memcpy/strlen/realloc.
        // ============================================================================
        llvm::Function *StringOpsCodeGen::ensureStringSplitHelper()
        {
            llvm::LLVMContext &ctx = *cg_.context_;
            llvm::Module *mod = cg_.module_.get();
            auto *ptrTy = llvm::PointerType::getUnqual(ctx);
            auto *i64Ty = llvm::Type::getInt64Ty(ctx);
            auto *i8Ty = llvm::Type::getInt8Ty(ctx);
            auto *voidTy = llvm::Type::getVoidTy(ctx);
            llvm::Value *nullP =
                llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy));
            auto ci64 = [&](long long v) { return llvm::ConstantInt::get(i64Ty, v); };
            auto ci8 = [&](int v) { return llvm::ConstantInt::get(i8Ty, v); };

            // (AR) البدائيّات الخارجيّة (libc مستضافةً / __sad حرًّا).
            // (AR) طول ‎memcpy‎ بنوع ‎size_t‎ الهدف — يطابق تعريف وقت التشغيل الحرّ.
            llvm::Type *szTy = cg_.getSizeType();
            auto memcpyF = mod->getOrInsertFunction(
                "memcpy", llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, szTy}, false));
            auto savedIP = cg_.builder_->saveIP();
            auto &B = *cg_.builder_;
            auto mkBB = [&](const char *n, llvm::Function *f) {
                return llvm::BasicBlock::Create(ctx, n, f);
            };

            // (AR) بنية SadArray القانونيّة نفسها (getArrayStructType) كي تتطابق
            //      إزاحات StructGEP وحجم التخصيص تمامًا مع مُنشئ المصفوفات وقارئها
            //      (emitArrayNew/emitArrayGet) على كلّ هدف. بنية محلّيّة {i64,i64,ptr}
            //      توافق x86_64 صدفةً (0/8/16) لكنّها تنحرف على i686 (سعة@4/بيانات@12
            //      قانونيًّا مقابل 8/16 محلّيًّا) ⇒ تلف كلّ وصول لعنصر — يناقض توحيد
            //      الخانة على i686 نفسه.
            // (EN) Use the canonical SadArray type so StructGEP offsets AND alloc
            //      size match emitArrayNew/emitArrayGet on every target. A local
            //      {i64,i64,ptr} matches x86_64 by luck but diverges on i686.
            llvm::StructType *arrTy = getArrayStructType(ctx);
            llvm::Value *arrAllocSz =
                ci64((long long)mod->getDataLayout().getTypeAllocSize(arrTy).getFixedValue());
            const unsigned SLOT = (unsigned)Sad::LLVM::SAD_ARRAY_SLOT_BYTES;

            // ================================================================
            // (1) __sad_utf8_clen(i8 lead) -> i64 — طول الحرف بالبايتات (1..4).
            // ================================================================
            if (llvm::Function *f = getOrCreateSplitHelper(
                    mod, ctx, "__sad_utf8_clen",
                    llvm::FunctionType::get(i64Ty, {i8Ty}, false)))
            {
                llvm::BasicBlock *e = mkBB("entry", f), *c2 = mkBB("c2", f),
                                 *c3 = mkBB("c3", f), *c4 = mkBB("c4", f),
                                 *r1 = mkBB("r1", f), *r2 = mkBB("r2", f),
                                 *r3 = mkBB("r3", f), *r4 = mkBB("r4", f);
                llvm::Value *lead = f->getArg(0);
                B.SetInsertPoint(e);
                llvm::Value *a1 = B.CreateAnd(lead, ci8(0x80));
                B.CreateCondBr(B.CreateICmpEQ(a1, ci8(0)), r1, c2);
                B.SetInsertPoint(c2);
                llvm::Value *a2 = B.CreateAnd(lead, ci8(0xE0));
                B.CreateCondBr(B.CreateICmpEQ(a2, ci8(0xC0)), r2, c3);
                B.SetInsertPoint(c3);
                llvm::Value *a3 = B.CreateAnd(lead, ci8(0xF0));
                B.CreateCondBr(B.CreateICmpEQ(a3, ci8(0xE0)), r3, c4);
                B.SetInsertPoint(c4);
                llvm::Value *a4 = B.CreateAnd(lead, ci8(0xF8));
                B.CreateCondBr(B.CreateICmpEQ(a4, ci8(0xF0)), r4, r1);
                B.SetInsertPoint(r1); B.CreateRet(ci64(1));
                B.SetInsertPoint(r2); B.CreateRet(ci64(2));
                B.SetInsertPoint(r3); B.CreateRet(ci64(3));
                B.SetInsertPoint(r4); B.CreateRet(ci64(4));
            }
            llvm::Function *utf8ClenF = mod->getFunction("__sad_utf8_clen");

            // ================================================================
            // (2) __sad_strstr(ptr h, ptr n) -> ptr — أوّل ورود لـ n في h أو null.
            //     مطابقة السلسلة الفرعيّة كاملةً (تحترم حدود بايتات الفاصل).
            // ================================================================
            if (llvm::Function *f = getOrCreateSplitHelper(
                    mod, ctx, "__sad_strstr",
                    llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false)))
            {
                llvm::Value *h = f->getArg(0), *n = f->getArg(1);
                llvm::BasicBlock *e = mkBB("entry", f), *cont = mkBB("cont", f),
                                 *oInit = mkBB("outer.init", f), *outer = mkBB("outer", f),
                                 *inner = mkBB("inner", f), *cmpc = mkBB("cmp", f),
                                 *jNext = mkBB("j.next", f), *adv = mkBB("adv", f),
                                 *found = mkBB("found", f), *retH = mkBB("ret.h", f),
                                 *retN = mkBB("ret.null", f);
                B.SetInsertPoint(e);
                llvm::Value *nlen = cg_.emitStrlen(n, "nlen");
                B.CreateCondBr(B.CreateICmpEQ(nlen, ci64(0)), retH, cont);
                B.SetInsertPoint(cont);
                llvm::Value *hlen = cg_.emitStrlen(h, "hlen");
                B.CreateCondBr(B.CreateICmpULT(hlen, nlen), retN, oInit);
                B.SetInsertPoint(oInit);
                llvm::Value *lastStart = B.CreateSub(hlen, nlen, "last.start");
                B.CreateBr(outer);
                // outer: i in [0, lastStart]
                B.SetInsertPoint(outer);
                llvm::PHINode *i = B.CreatePHI(i64Ty, 2, "i");
                i->addIncoming(ci64(0), oInit);
                B.CreateCondBr(B.CreateICmpUGT(i, lastStart), retN, inner);
                // inner: j in [0, nlen)
                B.SetInsertPoint(inner);
                llvm::PHINode *j = B.CreatePHI(i64Ty, 2, "j");
                j->addIncoming(ci64(0), outer);
                B.CreateCondBr(B.CreateICmpUGE(j, nlen), found, cmpc);
                B.SetInsertPoint(cmpc);
                llvm::Value *hIdx = B.CreateAdd(i, j, "h.idx");
                llvm::Value *hc = B.CreateLoad(i8Ty, B.CreateGEP(i8Ty, h, hIdx), "hc");
                llvm::Value *nc = B.CreateLoad(i8Ty, B.CreateGEP(i8Ty, n, j), "nc");
                B.CreateCondBr(B.CreateICmpNE(hc, nc), adv, jNext);
                B.SetInsertPoint(jNext);
                j->addIncoming(B.CreateAdd(j, ci64(1)), jNext);
                B.CreateBr(inner);
                B.SetInsertPoint(found);
                B.CreateRet(B.CreateGEP(i8Ty, h, i));
                B.SetInsertPoint(adv);
                i->addIncoming(B.CreateAdd(i, ci64(1)), adv);
                B.CreateBr(outer);
                B.SetInsertPoint(retH); B.CreateRet(h);
                B.SetInsertPoint(retN); B.CreateRet(nullP);
            }
            llvm::Function *strstrF = mod->getFunction("__sad_strstr");

            // ================================================================
            // (3) __sad_substr_dup(ptr src, i64 start, i64 len) -> ptr نسخة
            //     منتهية بصفر للمقطع src[start..start+len).
            // ================================================================
            if (llvm::Function *f = getOrCreateSplitHelper(
                    mod, ctx, "__sad_substr_dup",
                    llvm::FunctionType::get(ptrTy, {ptrTy, i64Ty, i64Ty}, false)))
            {
                llvm::Value *src = f->getArg(0), *start = f->getArg(1), *len = f->getArg(2);
                llvm::BasicBlock *e = mkBB("entry", f);
                B.SetInsertPoint(e);
                llvm::Value *buf = cg_.emitMalloc(B.CreateAdd(len, ci64(1)), "buf");
                llvm::Value *srcStart = B.CreateGEP(i8Ty, src, start, "src.start");
                B.CreateCall(memcpyF, {buf, srcStart,
                    B.CreateZExtOrTrunc(len, szTy, "substr.dup.len.sz")});
                B.CreateStore(ci8(0), B.CreateGEP(i8Ty, buf, len));
                B.CreateRet(buf);
            }
            llvm::Function *substrDupF = mod->getFunction("__sad_substr_dup");

            // ================================================================
            // (4) __sad_split_append(ptr arr, ptr s) -> void — يُلحق مؤشّر النصّ
            //     بمصفوفة SadArray، مُنمّيًا البيانات (realloc ×2) عند الامتلاء.
            //     خطوة الخانة SAD_ARRAY_SLOT_BYTES (موحَّدة عبر الأهداف، ث4).
            // ================================================================
            if (llvm::Function *f = getOrCreateSplitHelper(
                    mod, ctx, "__sad_split_append",
                    llvm::FunctionType::get(voidTy, {ptrTy, ptrTy}, false)))
            {
                llvm::Value *arr = f->getArg(0), *s = f->getArg(1);
                llvm::BasicBlock *e = mkBB("entry", f), *grow = mkBB("grow", f),
                                 *store = mkBB("store", f);
                B.SetInsertPoint(e);
                llvm::Value *lenP = B.CreateStructGEP(arrTy, arr, 0, "len.p");
                llvm::Value *capP = B.CreateStructGEP(arrTy, arr, 1, "cap.p");
                llvm::Value *datP = B.CreateStructGEP(arrTy, arr, 2, "dat.p");
                llvm::Value *len = B.CreateLoad(i64Ty, lenP, "len");
                llvm::Value *cap = B.CreateLoad(i64Ty, capP, "cap");
                B.CreateCondBr(B.CreateICmpEQ(len, cap), grow, store);
                B.SetInsertPoint(grow);
                llvm::Value *oldData = B.CreateLoad(ptrTy, datP, "old.data");
                llvm::Value *newCap = B.CreateMul(cap, ci64(2), "new.cap");
                llvm::Value *newBytes = B.CreateMul(newCap, ci64(SLOT), "new.bytes");
                llvm::Value *newData = cg_.emitRealloc(oldData, newBytes, "new.data");
                B.CreateStore(newData, datP);
                B.CreateStore(newCap, capP);
                B.CreateBr(store);
                B.SetInsertPoint(store);
                llvm::Value *data = B.CreateLoad(ptrTy, datP, "data");
                llvm::Value *len2 = B.CreateLoad(i64Ty, lenP, "len2");
                // (AR) خطوة i64 (SAD_ARRAY_SLOT_BYTES) لتخزين مؤشّر النصّ في الخانة.
                llvm::Value *slot = B.CreateGEP(i64Ty, data, len2, "slot");
                B.CreateStore(s, slot);
                B.CreateStore(B.CreateAdd(len2, ci64(1)), lenP);
                B.CreateRetVoid();
            }
            llvm::Function *appendF = mod->getFunction("__sad_split_append");

            // ================================================================
            // (5) __sad_string_split(ptr str, ptr delim, i64 maxSplits) -> SadArray*
            //     الدلالة الكاملة المطابِقة للمفسّر.
            // ================================================================
            llvm::Function *splitFn = getOrCreateSplitHelper(
                mod, ctx, "__sad_string_split",
                llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy, i64Ty}, false));
            if (splitFn)
            {
                llvm::Value *str = splitFn->getArg(0), *delim = splitFn->getArg(1),
                            *maxSplits = splitFn->getArg(2);
                llvm::BasicBlock *e = mkBB("entry", splitFn),
                                 *whole = mkBB("whole", splitFn),
                                 *notZero = mkBB("not.zero", splitFn),
                                 *charLoop = mkBB("char.loop", splitFn),
                                 *charBody = mkBB("char.body", splitFn),
                                 *subInit = mkBB("sub.init", splitFn),
                                 *subLoop = mkBB("sub.loop", splitFn),
                                 *haveMatch = mkBB("have.match", splitFn),
                                 *afterLoop = mkBB("after.loop", splitFn),
                                 *doneBB = mkBB("done", splitFn);
                B.SetInsertPoint(e);
                // arr = malloc(sizeof SadArray); len=0, cap=8, data=malloc(8*SLOT)
                llvm::Value *arr = cg_.emitMalloc(arrAllocSz, "arr");
                llvm::Value *initCap = ci64(8);
                llvm::Value *data0 =
                    cg_.emitMalloc(B.CreateMul(initCap, ci64(SLOT)), "data0");
                B.CreateStore(ci64(0), B.CreateStructGEP(arrTy, arr, 0));
                B.CreateStore(initCap, B.CreateStructGEP(arrTy, arr, 1));
                B.CreateStore(data0, B.CreateStructGEP(arrTy, arr, 2));
                // (AR) تقسيمُ النصّ يُنتج مصفوفةً متجانسةً من نصوص ⇒ الوسوم=null (الحقل ٣)
                //      وhomogKind=Str (الحقل ٤). كانا غيرَ مُهيّأين (malloc) ⇒ القارئُ الموسوم
                //      يقرأ الحقل ٤ حين tags==null، فوجب ضبطُهما صراحةً.
                // (EN) split yields a homogeneous string array ⇒ tags=null (field 3) and
                //      homogKind=Str (field 4). Both were uninitialized (malloc); the tagged
                //      reader reads field 4 when tags==null, so set them explicitly.
                B.CreateStore(llvm::ConstantPointerNull::get(ptrTy),
                              B.CreateStructGEP(arrTy, arr, 3));
                B.CreateStore(llvm::ConstantInt::get(i8Ty, Sad::LLVM::DynKind::Str),
                              B.CreateStructGEP(arrTy, arr, 4));
                llvm::Value *strLen = cg_.emitStrlen(str, "str.len");
                llvm::Value *delimLen = cg_.emitStrlen(delim, "delim.len");
                // maxSplits==0 ⇒ النصّ كاملًا في عنصر واحد
                B.CreateCondBr(B.CreateICmpEQ(maxSplits, ci64(0)), whole, notZero);
                B.SetInsertPoint(whole);
                B.CreateCall(appendF, {arr, B.CreateCall(substrDupF, {str, ci64(0), strLen})});
                B.CreateBr(doneBB);
                // فاصل فارغ ⇒ تقسيم بأحرف UTF-8
                B.SetInsertPoint(notZero);
                B.CreateCondBr(B.CreateICmpEQ(delimLen, ci64(0)), charLoop, subInit);
                // ---- char.loop: i in [0, strLen) بخطوة طول الحرف ----
                B.SetInsertPoint(charLoop);
                llvm::PHINode *ci = B.CreatePHI(i64Ty, 2, "ci");
                ci->addIncoming(ci64(0), notZero);
                B.CreateCondBr(B.CreateICmpULT(ci, strLen), charBody, doneBB);
                B.SetInsertPoint(charBody);
                llvm::Value *lead = B.CreateLoad(i8Ty, B.CreateGEP(i8Ty, str, ci), "lead");
                llvm::Value *clen = B.CreateCall(utf8ClenF, {lead}, "clen");
                // clamp: useLen = min(clen, strLen - i)
                llvm::Value *rem = B.CreateSub(strLen, ci, "rem");
                llvm::Value *useLen =
                    B.CreateSelect(B.CreateICmpULE(clen, rem), clen, rem, "use.len");
                B.CreateCall(appendF, {arr, B.CreateCall(substrDupF, {str, ci, useLen})});
                ci->addIncoming(B.CreateAdd(ci, useLen), charBody);
                B.CreateBr(charLoop);
                // ---- sub.init / sub.loop: تقسيم على سلسلة فرعيّة ----
                B.SetInsertPoint(subInit);
                // pos, splits عبر alloca (OptimizeNone ⇒ لا حاجة لـmem2reg)
                llvm::IRBuilder<> entryB(e, e->getFirstInsertionPt());
                llvm::Value *posA = entryB.CreateAlloca(i64Ty, nullptr, "pos.a");
                llvm::Value *splA = entryB.CreateAlloca(i64Ty, nullptr, "spl.a");
                B.CreateStore(ci64(0), posA);
                B.CreateStore(ci64(0), splA);
                B.CreateBr(subLoop);
                B.SetInsertPoint(subLoop);
                llvm::Value *pos = B.CreateLoad(i64Ty, posA, "pos");
                llvm::Value *posPtr = B.CreateGEP(i8Ty, str, pos, "pos.ptr");
                llvm::Value *found = B.CreateCall(strstrF, {posPtr, delim}, "found");
                B.CreateCondBr(B.CreateICmpEQ(found, nullP), afterLoop, haveMatch);
                B.SetInsertPoint(haveMatch);
                // partLen = found - posPtr (فرق مؤشّرَي i8 ⇒ عدد بايتات)
                llvm::Value *foundI = B.CreatePtrToInt(found, i64Ty, "found.i");
                llvm::Value *posPtrI = B.CreatePtrToInt(posPtr, i64Ty, "pos.ptr.i");
                llvm::Value *partLen = B.CreateSub(foundI, posPtrI, "part.len");
                B.CreateCall(appendF, {arr, B.CreateCall(substrDupF, {str, pos, partLen})});
                // pos = pos + partLen + delimLen ; splits++
                llvm::Value *newPos =
                    B.CreateAdd(B.CreateAdd(pos, partLen), delimLen, "new.pos");
                B.CreateStore(newPos, posA);
                llvm::Value *newSpl = B.CreateAdd(B.CreateLoad(i64Ty, splA), ci64(1), "new.spl");
                B.CreateStore(newSpl, splA);
                // break if maxSplits>0 && splits>=maxSplits
                llvm::Value *maxPos = B.CreateICmpSGT(maxSplits, ci64(0));
                llvm::Value *reached = B.CreateICmpSGE(newSpl, maxSplits);
                B.CreateCondBr(B.CreateAnd(maxPos, reached), afterLoop, subLoop);
                // ---- after.loop: الجزء الأخير str[pos..strLen) ----
                B.SetInsertPoint(afterLoop);
                llvm::Value *lastPos = B.CreateLoad(i64Ty, posA, "last.pos");
                llvm::Value *lastLen = B.CreateSub(strLen, lastPos, "last.len");
                B.CreateCall(appendF, {arr, B.CreateCall(substrDupF, {str, lastPos, lastLen})});
                B.CreateBr(doneBB);
                B.SetInsertPoint(doneBB);
                B.CreateRet(arr);
            }

            cg_.builder_->restoreIP(savedIP);
            return mod->getFunction("__sad_string_split");
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringJoin(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) دمج مصفوفة نصوص بفاصل — مثلاً: دمج(["أ","ب","ج"], "-") → "أ-ب-ج"
            // (EN) Join array of strings with separator
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *arrPtr = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *sep = normalizeStringArgPtr(cg_.resolveOperand(inst->operands[1]), "join.sep");
            if (!arrPtr || !sep)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();
            auto i8Ty = llvm::Type::getInt8Ty(*cg_.context_);
            llvm::StructType *arrTy = getArrayStructType(*cg_.context_);

            // ================================================================
            // (AR) إصلاح خطأ GEP: المتغير المحمّل من alloca(i64) يكون i64 وليس مؤشر.
            //      CreateStructGEP يتطلب مؤشراً — cg_.normalizeArrayPtr يحوّل i64→ptr.
            //      هذا يحدث عندما: تقسيم() ترجع ptr → STORE يحوّله لـ i64 (PtrToInt)
            //      → cg_.resolveOperand يحمّل i64 → GEP يفشل بدون هذا التحويل.
            // (EN) Fix GEP error: variable loaded from alloca(i64) is i64 not pointer.
            //      CreateStructGEP requires pointer — cg_.normalizeArrayPtr converts i64→ptr.
            // ================================================================
            arrPtr = cg_.normalizeArrayPtr(arrPtr, "join");

            // Load array length and data
            llvm::Value *lenGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 0, "join.len.gep");
            llvm::Value *arrLen = cg_.builder_->CreateLoad(i64Ty, lenGep, "join.len");
            llvm::Value *datGep = cg_.builder_->CreateStructGEP(arrTy, arrPtr, 2, "join.dat.gep");
            llvm::Value *dataPtr = cg_.builder_->CreateLoad(ptrTy, datGep, "join.data");

            // Get separator length
            llvm::Value *sepLen = cg_.emitStrlen(sep, "sep.len");

            // Allocate generous buffer: arrLen * 256 (rough estimate)
            llvm::Value *bufSize = cg_.builder_->CreateMul(arrLen, llvm::ConstantInt::get(i64Ty, 256));
            bufSize = cg_.builder_->CreateAdd(bufSize, llvm::ConstantInt::get(i64Ty, 1));
            llvm::Value *buf = cg_.emitMalloc(bufSize, "join.buf");

            // Start with empty string
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i8Ty, 0), buf);

            // Loop: strcat each element + separator
            auto *strcatType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto strcatFunc = cg_.module_->getOrInsertFunction("strcat", strcatType);

            llvm::Function *curFunc = cg_.builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *loopBB = llvm::BasicBlock::Create(*cg_.context_, "join.loop", curFunc);
            llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*cg_.context_, "join.body", curFunc);
            llvm::BasicBlock *doneBB = llvm::BasicBlock::Create(*cg_.context_, "join.done", curFunc);
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(loopBB);
            llvm::PHINode *idx = cg_.builder_->CreatePHI(i64Ty, 2, "join.idx");
            idx->addIncoming(llvm::ConstantInt::get(i64Ty, 0), loopBB->getSinglePredecessor());
            llvm::Value *isDone = cg_.builder_->CreateICmpUGE(idx, arrLen, "join.done.check");
            cg_.builder_->CreateCondBr(isDone, doneBB, bodyBB);

            cg_.builder_->SetInsertPoint(bodyBB);
            // If not first element, append separator
            llvm::Value *isFirst = cg_.builder_->CreateICmpEQ(idx, llvm::ConstantInt::get(i64Ty, 0));
            llvm::BasicBlock *sepBB = llvm::BasicBlock::Create(*cg_.context_, "join.sep", curFunc);
            llvm::BasicBlock *concatBB = llvm::BasicBlock::Create(*cg_.context_, "join.concat", curFunc);
            cg_.builder_->CreateCondBr(isFirst, concatBB, sepBB);

            cg_.builder_->SetInsertPoint(sepBB);
            cg_.builder_->CreateCall(strcatFunc, {buf, sep});
            cg_.builder_->CreateBr(concatBB);

            cg_.builder_->SetInsertPoint(concatBB);
            // (AR) خطوة العنصر i64 (8) لتوحيد حجم خانة SadArray — كانت ptrTy
            //      (=4 على i686) تقرأ مؤشّرًا فاسدًا من مصفوفة مخزَّنة بخطوة 8.
            //      التحميل يبقى بنوع مؤشّر.
            // (EN) i64-stride element (8) for unified SadArray slots; load as ptr.
            llvm::Value *elemGep = cg_.builder_->CreateGEP(cg_.getInt64Type(), dataPtr, {idx}, "join.elem.gep");
            llvm::Value *elem = cg_.builder_->CreateLoad(ptrTy, elemGep, "join.elem");
            cg_.builder_->CreateCall(strcatFunc, {buf, elem});

            llvm::Value *nextIdx = cg_.builder_->CreateAdd(idx, llvm::ConstantInt::get(i64Ty, 1));
            idx->addIncoming(nextIdx, concatBB);
            cg_.builder_->CreateBr(loopBB);

            cg_.builder_->SetInsertPoint(doneBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = buf;
            }
            return buf;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringStartsWith(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *str = normalizeStringPtr(cg_.resolveOperand(inst->operands[0]), "starts.src",
                                                  Sad::Builtins::Names::TypeMethods::String::STARTS_WITH);
            llvm::Value *prefix = normalizeStringArgPtr(cg_.resolveOperand(inst->operands[1]), "starts.prefix");
            if (!str || !prefix)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();

            // Get prefix length

            llvm::Value *prefLen = cg_.emitStrlen(prefix, "pref.len");

            // strncmp(str, prefix, prefLen)
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto *strncmpType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy, cg_.getSizeType()}, false);
            auto strncmpFunc = cg_.module_->getOrInsertFunction("strncmp", strncmpType);
            llvm::Value *cmp = cg_.builder_->CreateCall(strncmpFunc,
                {str, prefix, cg_.coerceToSize(prefLen, "pref.len.sz")}, "starts.cmp");
            llvm::Value *cmpBool = cg_.builder_->CreateICmpEQ(cmp,
                                                          llvm::ConstantInt::get(i32Ty, 0), "starts_with");
            llvm::Value *result = cg_.builder_->CreateZExt(cmpBool, i64Ty, "starts_with.i64");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringEndsWith(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *str = normalizeStringPtr(cg_.resolveOperand(inst->operands[0]), "ends.src",
                                                  Sad::Builtins::Names::TypeMethods::String::ENDS_WITH);
            llvm::Value *suffix = normalizeStringArgPtr(cg_.resolveOperand(inst->operands[1]), "ends.suffix");
            if (!str || !suffix)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);
            auto i64Ty = cg_.getInt64Type();
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);

            llvm::Value *strLen = cg_.emitStrlen(str, "str.len");
            llvm::Value *sufLen = cg_.emitStrlen(suffix, "suf.len");

            // Compare last sufLen chars: str + (strLen - sufLen)
            llvm::Value *offset = cg_.builder_->CreateSub(strLen, sufLen, "offset");
            llvm::Value *endPtr = cg_.builder_->CreateGEP(
                llvm::Type::getInt8Ty(*cg_.context_), str, {offset}, "end.ptr");

            auto *strcmpType = llvm::FunctionType::get(i32Ty, {ptrTy, ptrTy}, false);
            auto strcmpFunc = cg_.module_->getOrInsertFunction("strcmp", strcmpType);
            llvm::Value *cmp = cg_.builder_->CreateCall(strcmpFunc, {endPtr, suffix}, "ends.cmp");
            llvm::Value *cmpBool = cg_.builder_->CreateICmpEQ(cmp,
                                                          llvm::ConstantInt::get(i32Ty, 0), "ends_with");
            llvm::Value *result = cg_.builder_->CreateZExt(cmpBool, i64Ty, "ends_with.i64");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }



        llvm::Value *StringOpsCodeGen::emitBuiltinStringContains(std::shared_ptr<SIRInstruction> inst)
        {
            if (!inst || inst->operands.size() < 2)
                return nullptr;
            llvm::Value *str = normalizeStringPtr(cg_.resolveOperand(inst->operands[0]), "contains2.src",
                                                  Sad::Builtins::Names::TypeMethods::String::CONTAINS);
            llvm::Value *substr = normalizeStringArgPtr(cg_.resolveOperand(inst->operands[1]), "contains2.sub");
            if (!str || !substr)
                return nullptr;

            auto ptrTy = llvm::PointerType::getUnqual(*cg_.context_);

            // strstr(str, substr) != null
            auto *strstrType = llvm::FunctionType::get(ptrTy, {ptrTy, ptrTy}, false);
            auto strstrFunc = cg_.module_->getOrInsertFunction("strstr", strstrType);
            llvm::Value *found = cg_.builder_->CreateCall(strstrFunc, {str, substr}, "found");
            llvm::Value *cmpBool = cg_.builder_->CreateICmpNE(found,
                                                          llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(*cg_.context_)), "contains");
            llvm::Value *result = cg_.builder_->CreateZExt(cmpBool, cg_.getInt64Type(), "contains.i64");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }


} // namespace LLVM
} // namespace Sad
