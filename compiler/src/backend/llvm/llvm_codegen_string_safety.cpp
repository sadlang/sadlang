/*
 * ============================================================================
 * (AR) البابُ الواحدُ لمؤشّراتِ النصّ — «لا يُقرأ نصٌّ إلّا من هنا»
 * (EN) The single door for string pointers — "no string is read except here"
 * ============================================================================
 *
 * (AR) السياقُ الذي وُلِد منه هذا الملفّ (مقيسٌ لا مُخمَّن):
 *
 *      «متغير نص س = لاشيء» ثمّ «اطبع_سطر(س)» كان **ينهار انهيارَ تجزئة** في
 *      المترجّم بينما يطبع المفسّرُ «لاشيء». وحين حُصِّن فرعُ الطباعةِ وحدَه، بقي
 *      الضمُّ «اطبع("قيمة: " + س)» ينهار (مقيس: المفسّرُ «قيمة: لاشيء» والمترجّمُ
 *      لا شيء). ثمّ تبيّن أنّ في الخلفيّةِ ~١١٠ موضعِ `inttoptr` في سياقٍ نصّيّ:
 *      كلُّ واحدٍ منها منفذُ انهيارٍ مستقلٌّ يلزمه حارسُه، ومنها ما لم يُحصَ.
 *
 * (AR) 🔑 والدرسُ أنّ رقعةَ المنفذِ تُغلق ما عُدَّ وتترك ما لم يُعَدّ. فالعلاجُ في
 *      الطبقة: دالّةٌ واحدةٌ يمرُّ بها كلُّ قارئِ مؤشّرِ نصٍّ، فتُعيد **دائمًا** مؤشّرًا
 *      يجوز إسنادُه. وأثرُها أنّ منفذًا منسيًّا يهبط إلى طباعةِ «لاشيء» لا إلى
 *      انهيارِ تجزئة — أي أنّ **صنفَ** العطبِ يُغلق لا حالاتُه المعدودة.
 *
 * (AR) ولماذا لا يكفي فحصُ `nullptr`؟ لأنّ حارسَ العدمِ ليس صفرًا بل
 *      `kSadNullSentinel`، فمقارنةُ المؤشّرِ الصفريِّ **تمرّه**. ولذلك يفحص البابُ
 *      الحالتَين معًا: الحارسَ في المجالِ العدديّ، والصفرَ في مجالِ المؤشّرات.
 *
 * (EN) One function every string-pointer reader goes through; it always returns a
 *      dereferenceable i8*. A null (integer sentinel or null pointer) becomes the
 *      SoT null word, so an unenumerated port degrades to «لاشيء», never a segfault.
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "sir_constants.h"
#include "value_repr_generated.h" // (AR) kNullDisplay — لفظُ العدمِ من مصدرِ الحقيقة
#include "error_manager.h"        // (AR) تصييرُ RUN033 من الكتالوجِ لا من سلسلةٍ توأم
#include "value.h"                // (AR) Value::makeNull().getTypeName() — اسمُ النوعِ نفسُه
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>

namespace Sad
{
    namespace LLVM
    {
        llvm::Value *LLVMCodeGen::emitSafeStringPtr(llvm::Value *value, const char *label)
        {
            const std::string tag = (label && *label) ? std::string(label) : std::string("str");

            // (AR) لفظُ العدمِ يُبنى مرّةً لكلِّ نداء؛ LLVM يوحّد الثوابتَ المتطابقةَ
            //      داخلَ الوحدة، فلا تضخُّمَ من التكرار.
            // (EN) The null word; LLVM merges identical constants within the module.
            auto nullWord = [&]() {
                return builder_->CreateGlobalStringPtr(Sad::Types::repr::kNullDisplay,
                                                       tag + ".null_word");
            };

            // (AR) لا قيمةَ أصلًا: لا مؤشّرَ يُقرأ، فاللفظُ هو الجوابُ الوحيدُ الآمن.
            // (EN) No value at all: the word is the only safe answer.
            if (!value)
                return nullWord();

            llvm::Type *ptrTy = llvm::PointerType::getUnqual(*context_);
            llvm::Type *i64Ty = llvm::Type::getInt64Ty(*context_);

            // ────────────────────────────────────────────────────────────────
            // (AR) المجالُ العدديّ: نصٌّ «مُعلَّب» في i64 (أو حارسُ عدم).
            // (EN) Integer domain: a string boxed in i64 (or the null sentinel).
            // ────────────────────────────────────────────────────────────────
            if (value->getType()->isIntegerTy())
            {
                llvm::Value *asI64 = value;
                if (!value->getType()->isIntegerTy(64))
                    asI64 = builder_->CreateIntCast(value, i64Ty, false, tag + ".i64");

                llvm::Value *isNull = builder_->CreateICmpEQ(
                    asI64,
                    llvm::ConstantInt::get(
                        i64Ty, static_cast<uint64_t>(Sad::Compiler::kSadNullSentinel)),
                    tag + ".is_null");
                // (AR) والصفرُ عدميٌّ كذلك: خانةٌ نصّيّةٌ غيرُ مُهيّأةٍ تحمل صفرًا،
                //      و`inttoptr 0` قراءةٌ من العنوانِ صفر.
                // (EN) Zero counts too: an uninitialized string slot holds 0.
                llvm::Value *isZero = builder_->CreateICmpEQ(
                    asI64, llvm::ConstantInt::get(i64Ty, 0), tag + ".is_zero");
                llvm::Value *isEmpty = builder_->CreateOr(isNull, isZero, tag + ".is_empty");

                llvm::Value *asPtr = builder_->CreateIntToPtr(asI64, ptrTy, tag + ".ptr");
                return builder_->CreateSelect(isEmpty, nullWord(), asPtr, tag + ".safe");
            }

            // ────────────────────────────────────────────────────────────────
            // (AR) مجالُ المؤشّرات: قد يكون صفريًّا، وقد يحمل الحارسَ مُحوَّلًا سلفًا.
            // (EN) Pointer domain: may be null, or may already carry the sentinel.
            // ────────────────────────────────────────────────────────────────
            if (value->getType()->isPointerTy())
            {
                llvm::Value *asInt = builder_->CreatePtrToInt(value, i64Ty, tag + ".as_int");
                llvm::Value *isNull = builder_->CreateICmpEQ(
                    asInt,
                    llvm::ConstantInt::get(
                        i64Ty, static_cast<uint64_t>(Sad::Compiler::kSadNullSentinel)),
                    tag + ".is_null");
                llvm::Value *isZero = builder_->CreateICmpEQ(
                    asInt, llvm::ConstantInt::get(i64Ty, 0), tag + ".is_zero");
                llvm::Value *isEmpty = builder_->CreateOr(isNull, isZero, tag + ".is_empty");
                return builder_->CreateSelect(isEmpty, nullWord(), value, tag + ".safe");
            }

            // (AR) نوعٌ لا يُقرأ نصًّا (عشريّ/بنية/…): لا يُحوَّل إلى عنوانٍ بحالٍ.
            //      الوصولُ إلى هنا خللٌ في المستدعي، والجوابُ الآمنُ اللفظُ لا الانهيار.
            // (EN) A type that is not readable as a string: caller bug; answer safely.
            return nullWord();
        }

        // ====================================================================
        // (AR) اسمُ العمليّةِ كما يعرضه المفسّر: «.طول()». الاسمُ من سجلِّ المدمجاتِ
        //      المولَّدِ لا مكتوبًا هنا، والزخرفةُ (نقطةٌ وقوسان) صيغةُ عرضٍ لا اسم.
        // (EN) Operation label as the interpreter shows it; the name comes from the
        //      generated builtin registry, only the decoration is formatting.
        // ====================================================================
        std::string LLVMCodeGen::stringMethodOperationLabel(std::string_view methodName)
        {
            return "." + std::string(methodName) + "()";
        }

        // ====================================================================
        // (AR) البابُ الثاني: العمليّةُ على عدمٍ ترفعُ RUN033 كما يرفعه المفسّر.
        // (EN) Second door: an operation on null raises RUN033, as the interpreter does.
        // ====================================================================
        // ====================================================================
        // (AR) 🔑 البابُ العامُّ للرفع: عدمٌ في **مسارِ عمليّة** ⇒ خطأُ تشغيلٍ يُطابق
        //      المفسّرَ رمزًا ونصًّا، ثمّ `exit(1)` — لا انهيارُ تجزئة.
        //
        //      وسببُ استخراجِه من `emitStringPtrOrRaise`: مسارُ النصِّ ليس المسارَ
        //      الوحيدَ الذي يُشتَقُّ منه العدم. قِيس (2026-08-16) أنّ `مصفوفة س`
        //      ثمّ `س.أضف(1)` و`خريطة س` ثمّ `س["ك"]=1` **يبنيان برمزِ خروجٍ صفر**
        //      ثمّ ينهاران `rc=139`، بينما المفسّرُ يرفع RUN033 وRUN018 على الترتيب.
        //      ولو نُسِخت كتلةُ الرفعِ لكلِّ عائلةٍ لانحرفت النسخُ عن بعضها عند أوّلِ
        //      تحريرٍ للكتالوج — وهو الانحرافُ عينُه الذي وُلِد منه هذا الملفّ.
        //
        //      ⚠️ ولا يُستعمل هذا البابُ في مسارِ **عرض**: هناك الجوابُ لفظُ العدمِ
        //      لا الرفع. والخلطُ بينهما يُبدِّل عطبًا مرئيًّا بجوابٍ كاذبٍ صامتٍ
        //      (أو بالعكس: يرفع حيث يطبع المفسّرُ «لاشيء»).
        // (EN) The general raise door: a null in an OPERATION path exits(1) with the
        //      interpreter's own code and text, instead of segfaulting. Extracted
        //      because the string path is not the only null-bearing family — measured:
        //      array append and map index-assign on a bare declaration built rc=0 and
        //      crashed rc=139 where the interpreter raises RUN033 / RUN018.
        // ====================================================================
        void LLVMCodeGen::emitRaiseIfNull(llvm::Value *value,
                                          Sad::Errors::ErrorCode raisedCode,
                                          const std::map<std::string, std::string> &placeholders,
                                          const char *label)
        {
            if (!value || !builder_->GetInsertBlock())
                return;

            const std::string tag = (label && *label) ? std::string(label) : std::string("op");
            llvm::Type *i64Ty = llvm::Type::getInt64Ty(*context_);

            llvm::Value *asI64 = nullptr;
            llvm::Type *valueType = value->getType();
            if (valueType->isPointerTy())
                asI64 = builder_->CreatePtrToInt(value, i64Ty, tag + ".p2i");
            else if (valueType->isIntegerTy())
                asI64 = (valueType == i64Ty)
                            ? value
                            : builder_->CreateIntCast(value, i64Ty, false, tag + ".i64");
            else
                return; // (AR) لا يمثّل عدمًا أصلًا / (EN) cannot be null

            llvm::Value *isSentinel = builder_->CreateICmpEQ(
                asI64,
                llvm::ConstantInt::get(i64Ty,
                                       static_cast<uint64_t>(Sad::Compiler::kSadNullSentinel)),
                tag + ".is_sentinel");
            llvm::Value *isZero =
                builder_->CreateICmpEQ(asI64, llvm::ConstantInt::get(i64Ty, 0), tag + ".is_zero");
            llvm::Value *isNull = builder_->CreateOr(isSentinel, isZero, tag + ".is_null");

            llvm::Function *curFunc = builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *sentinelBB =
                llvm::BasicBlock::Create(*context_, tag + ".raise_null", curFunc);
            llvm::BasicBlock *zeroBB =
                llvm::BasicBlock::Create(*context_, tag + ".raise_void", curFunc);
            llvm::BasicBlock *nullBB =
                llvm::BasicBlock::Create(*context_, tag + ".null_raise", curFunc);
            llvm::BasicBlock *contBB =
                llvm::BasicBlock::Create(*context_, tag + ".present", curFunc);
            builder_->CreateCondBr(isNull, nullBB, contBB);

            // ════════════════════════════════════════════════════════════════════
            // (AR) 🔑 والعدمُ **وجهان**، والمفسّرُ يفرّق بينهما في نصِّ الخطأِ نفسِه:
            //        • `مصفوفة س` (تصريحٌ مجرَّد) ⇒ خانةٌ صفريّة ⇒ المفسّر: `'VOID'`
            //        • `مصفوفة عدمية س = لاشيء` ⇒ حارسُ العدم ⇒ المفسّر: `'NULL'`
            //      وكلاهما مقيسٌ (2026-08-16) بتشغيلِ المفسّرِ على البرنامجَين.
            //
            //      فلا يصحّ ملءُ `{type}` ملءً واحدًا: الرفعُ يصير صحيحًا في نصفِ
            //      الحالاتِ **ويكذب في نصفِها الآخرِ نصًّا يُصدَّق** — وهو أخفى من
            //      عدمِ الرفعِ أصلًا. ولذلك يفرّع البابُ على الشكلِ لا على وجودِ العدم،
            //      ويملأ `{type}` بنفسِه من `Value` عينِها التي يملأ بها المفسّر.
            //
            //      ⚠️ ولذلك أيضًا لا يستقبل هذا البابُ `{type}` من المستدعي: لو تُرِك
            //      له لعاد الشكلان واحدًا عند أوّلِ مستدعٍ يغفل عنهما.
            // (EN) Null has TWO shapes and the interpreter's own message distinguishes
            //      them (measured): a bare declaration reads 'VOID', an explicit
            //      «= لاشيء» reads 'NULL'. Filling {type} once would be right half the
            //      time and print a believable lie the other half — worse than not
            //      raising at all. Hence the branch on SHAPE, and {type} is filled here
            //      (never by the caller) from the very Value the interpreter uses.
            // ════════════════════════════════════════════════════════════════════
            builder_->SetInsertPoint(nullBB);
            builder_->CreateCondBr(isSentinel, sentinelBB, zeroBB);

            auto raiseWithTypeName = [&](llvm::BasicBlock *bb, const std::string &typeName) {
                builder_->SetInsertPoint(bb);
                std::map<std::string, std::string> filled = placeholders;
                filled["type"] = typeName;
                emitNullRaiseBody(raisedCode, filled, tag);
                builder_->CreateUnreachable();
            };
            raiseWithTypeName(sentinelBB, Sad::Data::Value::makeNull().getTypeName());
            raiseWithTypeName(zeroBB, Sad::Data::Value().getTypeName());

            builder_->SetInsertPoint(contBB);
        }

        // ====================================================================
        // (AR) جسمُ الرفعِ وحدَه (بلا تفريع): يُطبَع النصُّ المُصيَّرُ من الكتالوجِ
        //      ثمّ `exit(1)`. مشتركٌ بين البابَين فلا يُنسَخ سلوكُ الوضعِ الحرِّ مرّتَين.
        // (EN) The raise body alone (no branching), shared by both doors.
        // ====================================================================
        void LLVMCodeGen::emitNullRaiseBody(Sad::Errors::ErrorCode raisedCode,
                                            const std::map<std::string, std::string> &placeholders,
                                            const std::string &tag)
        {
            llvm::Type *ptrTy = llvm::PointerType::getUnqual(*context_);
            if (freestanding_)
            {
                emitFreestandingPanicCall(Sad::Compiler::kSadPanicNullAssert);
                return;
            }
            Sad::Errors::RenderContext ctx;
            ctx.placeholders = placeholders;
            const std::string rendered =
                Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(raisedCode, ctx);
            auto *printfType =
                llvm::FunctionType::get(llvm::Type::getInt32Ty(*context_), {ptrTy}, true);
            auto printfFunc = module_->getOrInsertFunction("printf", printfType);
            llvm::Value *msg = builder_->CreateGlobalStringPtr(rendered + "\n", tag + ".raise");
            builder_->CreateCall(printfFunc, {msg});
            auto *exitType = llvm::FunctionType::get(
                llvm::Type::getVoidTy(*context_), {llvm::Type::getInt32Ty(*context_)}, false);
            auto exitFunc = module_->getOrInsertFunction("exit", exitType);
            builder_->CreateCall(exitFunc,
                                 {llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 1)});
        }

        llvm::Value *LLVMCodeGen::emitStringPtrOrRaise(llvm::Value *value,
                                                       const std::string &operationArabic,
                                                       const char *label,
                                                       Sad::Errors::ErrorCode raisedCode)
        {
            const std::string tag = (label && *label) ? std::string(label) : std::string("str");
            llvm::Type *ptrTy = llvm::PointerType::getUnqual(*context_);
            llvm::Type *i64Ty = llvm::Type::getInt64Ty(*context_);

            if (!value)
                return llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy));

            // (AR) وحّد التمثيلَ في i64 للفحص، كما يفعل emitNullAssert حرفًا بحرف.
            // (EN) Normalize to i64 for the check, mirroring emitNullAssert.
            llvm::Value *asI64 = nullptr;
            llvm::Type *valueType = value->getType();
            if (valueType->isPointerTy())
                asI64 = builder_->CreatePtrToInt(value, i64Ty, tag + ".p2i");
            else if (valueType->isIntegerTy())
                asI64 = (valueType == i64Ty)
                            ? value
                            : builder_->CreateIntCast(value, i64Ty, false, tag + ".i64");
            else
                return value; // (AR) لا يمثّل عدمًا أصلًا / (EN) cannot be null

            llvm::Value *isSentinel = builder_->CreateICmpEQ(
                asI64,
                llvm::ConstantInt::get(i64Ty,
                                       static_cast<uint64_t>(Sad::Compiler::kSadNullSentinel)),
                tag + ".is_sentinel");
            llvm::Value *isZero = builder_->CreateICmpEQ(
                asI64, llvm::ConstantInt::get(i64Ty, 0), tag + ".is_zero");
            llvm::Value *isNull = builder_->CreateOr(isSentinel, isZero, tag + ".is_null");

            llvm::Function *curFunc = builder_->GetInsertBlock()->getParent();
            llvm::BasicBlock *failBB =
                llvm::BasicBlock::Create(*context_, tag + ".null_fail", curFunc);
            llvm::BasicBlock *contBB =
                llvm::BasicBlock::Create(*context_, tag + ".present", curFunc);
            builder_->CreateCondBr(isNull, failBB, contBB);

            builder_->SetInsertPoint(failBB);
            {
                // (AR) 🔑 النصُّ من الكتالوج لا من هنا: العمليّةُ معروفةٌ زمنَ التوليد،
                //      فتُملأ الفراغاتُ ويُطبع النصُّ المُصيَّر. ولو كُتِب اللفظُ هنا
                //      لانحرف عن المفسّرِ عند أوّلِ تحريرٍ للكتالوج — وهو ما وقع فعلًا
                //      في `kNullAssertRun056Msg` المكتوبِ يدًا.
                // (EN) The text comes from the catalog, not from here: the operation is
                //      known at codegen time, so placeholders are filled and the rendered
                //      string is printed — no hand-written twin that can drift.
                // (AR) واسمُ النوعِ من `Value::makeNull().getTypeName()` — الدالّةُ عينُها
                //      التي يملأ بها المفسّرُ الفراغَ، فلا توأمَ نصّيًّا ينحرف.
                // (EN) The type name comes from the very function the interpreter uses.
                // (AR) ⚠️ الفراغاتُ تختلف باختلافِ الرمز، فلا يصحّ ملؤها ملءً واحدًا:
                //      RUN033 يسأل عن العمليّةِ والنوع، وRUN037 يسأل عن اسمِ الدالّة.
                //      وسببُ وجودِ الرمزَين أنّ المفسّرَ يفرّق بينهما مقيسًا: «س.يحتوي()»
                //      على عدمٍ ⇒ RUN033، و«عشري(س)» ⇒ RUN037. فلو وحّدناهما هنا
                //      لصار المترجّمُ يرفع — وهو الصواب — برمزٍ يخالف المفسّر.
                // (EN) Placeholders differ per code: RUN033 asks for operation+type,
                //      RUN037 for the builtin name. The interpreter distinguishes them
                //      (measured), so the compiler must not collapse them into one.
                std::map<std::string, std::string> placeholders;
                if (raisedCode == Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG)
                    placeholders = {{"func", operationArabic}};
                else
                    placeholders = {{"operation", operationArabic},
                                    {"type", Sad::Data::Value::makeNull().getTypeName()}};
                emitNullRaiseBody(raisedCode, placeholders, tag);
            }
            builder_->CreateUnreachable();

            builder_->SetInsertPoint(contBB);
            return valueType->isPointerTy() ? value
                                            : builder_->CreateIntToPtr(asI64, ptrTy, tag + ".ptr");
        }


        // ════════════════════════════════════════════════════════════════════
        // (AR) البابُ الثالث — حارسُ الحاويةِ العدميّة، موضعٌ واحدٌ يُنادى.
        //      انظر التعليلَ الكاملَ في `llvm_codegen.h` عند الإعلان.
        // (EN) Third door — the container null guard. Rationale at the decl.
        // ════════════════════════════════════════════════════════════════════
        LLVMCodeGen::ContainerNullGuard LLVMCodeGen::emitContainerNullGuard(
            llvm::Value *containerPtr,
            llvm::Type *containerStructTy,
            const char *placeholderName,
            const std::string &tag)
        {
            auto *i64Ty = llvm::Type::getInt64Ty(*context_);
            auto *ptrTy = llvm::PointerType::getUnqual(*context_);

            // (AR) الواردُ قد يكون i64 (مؤشّرٌ مخزَّنٌ في خانةٍ عدديّة) — يُحوَّل أوّلًا.
            // (EN) The incoming value may be an i64 slot holding a pointer.
            llvm::Value *asPtr = containerPtr;
            if (asPtr->getType()->isIntegerTy(64))
            {
                asPtr = builder_->CreateIntToPtr(asPtr, ptrTy, tag + ".i2p");
            }

            // (AR) 🔑 الوجهان معًا: المؤشّرُ الصفريُّ **و**الحارسُ في خانةِ المؤشّر.
            //      وفحصُ أحدِهما وحدَه يترك الآخَرَ ينهار — مقيسٌ في الوجهين.
            // (EN) BOTH shapes: the zero pointer AND the sentinel in the pointer slot.
            llvm::Value *asInt = builder_->CreatePtrToInt(asPtr, i64Ty, tag + ".p2i");
            llvm::Value *isNull = builder_->CreateOr(
                builder_->CreateICmpEQ(
                    asPtr,
                    llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)),
                    tag + ".isnullptr"),
                builder_->CreateICmpEQ(
                    asInt,
                    llvm::ConstantInt::get(i64Ty, Sad::Compiler::kSadNullSentinel),
                    tag + ".issentinel"),
                tag + ".isnull");

            // (AR) نائبٌ صفريٌّ **مشترَكٌ** بالاسم: بنيةٌ ثابتةٌ أصفارُها كلُّها، فيُفَكُّ
            //      المؤشّرُ بأمانٍ في المسارِ الميّتِ بدل أن يُقرأَ عنوانٌ شارد. والانتقاءُ
            //      بـ`select` لا بتفرّعٍ عمدًا: لا كتلَ جديدةً تُدخَل في مسارٍ قد يكون
            //      داخلَ حلقةٍ أو تعبيرٍ مركَّب.
            // (EN) A shared all-zero placeholder struct so the dead path dereferences
            //      something valid. Selected branchlessly to avoid injecting basic
            //      blocks into arbitrary expression contexts.
            ContainerNullGuard guard;
            guard.isNull = isNull;

            // (AR) مُستهلِكٌ يحرس الصفرَ سلفًا: تُسوَّى الحالتانِ إلى صفرٍ فيبلغُ حارسَه.
            // (EN) Consumer already guards zero: normalise both shapes to a null pointer.
            if (containerStructTy == nullptr)
            {
                guard.safePtr = builder_->CreateSelect(
                    isNull,
                    llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(ptrTy)),
                    asPtr, tag + ".norm");
                return guard;
            }

            // (AR) الغيابُ يُعبَّر عنه بـ`nullptr` لا بسلسلةٍ فارغة: السلسلةُ
            //      الفارغةُ اسمٌ صالحٌ في التوقيعِ فلا تقول «لا نائبَ هنا».
            // (EN) Absence is nullptr, not "": an empty name is still a valid name.
            llvm::GlobalVariable *placeholder = module_->getNamedGlobal(placeholderName);
            if (!placeholder)
            {
                placeholder = new llvm::GlobalVariable(
                    *module_, containerStructTy, /*isConstant=*/true,
                    llvm::GlobalValue::InternalLinkage,
                    llvm::ConstantAggregateZero::get(containerStructTy),
                    placeholderName);
            }

            guard.safePtr = builder_->CreateSelect(isNull, placeholder, asPtr, tag + ".safe");
            return guard;
        }

    } // namespace LLVM
} // namespace Sad
