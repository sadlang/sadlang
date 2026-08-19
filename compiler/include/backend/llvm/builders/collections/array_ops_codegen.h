/*
 * ============================================================================
 * ArrayOpsCodeGen — توليد LLVM IR لعمليات المصفوفات والنصوص
 * Phase 7 Step 5
 * - normalizeArrayPtr / emitBoundsCheck (helpers داخلية)
 * - emitArrayNew / Get / Set / Len / Concat
 * - emitStringNew
 * ============================================================================
 */
#ifndef SAD_LLVM_ARRAY_OPS_CODEGEN_H
#define SAD_LLVM_ARRAY_OPS_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include <llvm/IR/BasicBlock.h>
#include "sir_instruction.h"

namespace Sad
{
    namespace LLVM
    {

        // (AR) حجم خانة عنصر المصفوفة/الصفّ بالبايت — ثابت ABI موحَّد عبر كلّ
        //      الأهداف (i64 دائمًا). التمثيل الداخليّ SadArray هو
        //      {i64 length, i64 capacity, ptr data} حيث data يشير إلى مصفوفة
        //      خانات 8 بايت (i64 أو مؤشّر مصندَق في الأدنى). توحيد الخانة على 8
        //      يزيل تناقض i686: كان التخصيص يستعمل getSizeOf(ptr) (=4 على i686)
        //      بينما الوصول للأرقام يخطو i64 (=8) ⇒ فيضان خانات. على x86_64
        //      القيمة 8 = getSizeOf(ptr) فالتوحيد مطابق للعنوان (صفر انحدار).
        // (EN) Array/tuple element slot size in bytes — a unified ABI constant
        //      across all targets (always i64=8). Unifying slots to 8 removes the
        //      i686 divergence where allocation used getSizeOf(ptr)=4 but numeric
        //      access strided by i64=8. On x86_64 (ptr=8) this is address-identical.
        constexpr unsigned long long SAD_ARRAY_SLOT_BYTES = 8;

        class LLVMCodeGen;
        using SIRInstruction = Compiler::SIR::SIRInstruction;

        class ArrayOpsCodeGen
        {
            LLVMCodeGen &cg_;

        public:
            explicit ArrayOpsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
            ArrayOpsCodeGen(const ArrayOpsCodeGen &) = delete;
            ArrayOpsCodeGen &operator=(const ArrayOpsCodeGen &) = delete;

            // (AR) دوال مساعدة داخلية
            // ============================================================
            // (AR) `assertDynTag`: هل يُزرَعُ تأكيدُ الوسمِ (`kind == Array`) بفرعِ
            //      إخفاقٍ صاخبٍ حين تصلُ قيمةٌ `%SadDyn`؟
            //
            //      🔑 يُمرَّرُ `false` **للفحصِ وحدَه** — أي حين لا يُراد من التطبيع
            //      إلّا قيمةٌ تُقارَنُ بالعدم. فالتأكيدُ يُنهي البرنامجَ قبلَ أن يرى
            //      المُوزِّعُ (`beginDynMapDispatch`) وسمَ الخريطة، وهو مبنيٌّ لها:
            //      `خ["س"]=1` يجعلُ `خ["أ"]` نوعُه «أي»، ثمّ `خ["أ"][2] = 5` كان
            //      يُهلِعُ «فهرسةٌ بعددٍ … ليست مصفوفة» بينما المفسّرُ يكتبُ مفتاحًا
            //      عدديًّا. وقُيس أنّ القراءةَ سليمةٌ والكتابةَ وحدَها تُخفِق، لأنّ
            //      `emitArrayGet` لا حارسَ عدمٍ قبلَ إرسالِها.
            //
            //      ⚠️ ولا يُنقَلُ الحارسُ إلى ما بعدَ المُوزِّعِ بدلًا من ذلك: قِيس
            //      أنّ الوعاءَ العدميَّ (`خريطة س` ثمّ `س["ك"]=1`) يعودُ حينئذٍ إلى
            //      الانهيارِ `rc=139` — لأنّ فرعَ المصفوفةِ لا يُبلَغُ أصلًا. فالفحصُ
            //      يبقى **قبلَ** الإرسالِ ويُنزَعُ منه التأكيدُ وحدَه.
            // (EN) `assertDynTag`: plant the loud `kind == Array` assertion when a
            //      %SadDyn value arrives? Pass `false` for CHECK-ONLY normalization —
            //      the assertion kills a Map-tagged value before beginDynMapDispatch,
            //      built precisely for it, can route it. Measured: the read path is
            //      fine; only the write fails, since emitArrayGet has no null guard.
            //      ⚠️ Moving the guard after the dispatch instead was measured to
            //      restore the rc=139 crash on a null container — the array branch is
            //      never reached. So the check stays BEFORE and loses only its assert.
            // ============================================================
            llvm::Value *normalizeArrayPtr(llvm::Value *arrPtr, const char *label = "arr",
                                           bool assertDynTag = true);

            // ════════════════════════════════════════════════════════════════
            // (AR) 🔑 حارسُ الإسنادِ بالفهرسِ لقيمةٍ **موسومة**: يحكمُ بالوسمِ لا
            //      بالحمولة، ويرفعُ RUN018 باسمِ نوعِ الوسمِ نفسِه.
            //
            //      وسببُ وجودِه أنّ البابَ العامَّ (`emitRaiseIfNull`) يشتقُّ **شكلَ**
            //      العدمِ من الحمولةِ وحدَها: الحارسُ ⇒ `'NULL'` والصفرُ ⇒ `'VOID'`.
            //      وذلك صحيحٌ لقيمةٍ محسوسة، وكاذبٌ لقيمةٍ موسومة: `أي ك = 0`
            //      وسمُها صحيحٌ وحمولتُها صفر، و`أي ك = خطأ` وسمُها منطقيٌّ
            //      وحمولتُها صفر — فكلتاهما كانت تُشخَّصُ `'VOID'` والمفسّرُ يقول
            //      `'INTEGER'` و`'BOOLEAN'`. وتعليقُ البابِ نفسِه يُدين هذا الصنف:
            //      «الرفعُ يصير صحيحًا في نصفِ الحالاتِ ويكذبُ في نصفِها الآخرِ
            //      نصًّا يُصدَّق — وهو أخفى من عدمِ الرفعِ أصلًا».
            //
            //      ⚠️ والوعاءانِ (مصفوفةٌ · خريطة) يمرّان بلا رفعٍ إلى مُوزِّعِهما،
            //      فلا يُقتَلُ وسمُ الخريطةِ قبلَه (وهو عطبُ ISSUE-172 بعينِه).
            //      وما عداهما — كائنٌ أو تعدادٌ جبريّ — يُترَك لتأكيدِ فرعِ
            //      المصفوفةِ بعدَ الإرسال، فلا يُدَّعى له نصٌّ لم يُقَسْ مقابلُه.
            // (EN) Index-assign guard for a TAGGED value: it judges by the tag, not the
            //      payload, and raises RUN018 with the tag's own type name. The general
            //      door derives the null SHAPE from the payload alone (sentinel ⇒ NULL,
            //      zero ⇒ VOID), which is right for a concrete value and a lie for a
            //      tagged one: an Int 0 and a Bool false both carry payload zero and were
            //      reported as 'VOID' while the interpreter says 'INTEGER'/'BOOLEAN'.
            //      Containers (array, map) pass through untouched so a Map tag is not
            //      killed before its dispatch (that was ISSUE-172). Object/ADT tags are
            //      left to the post-dispatch array assertion rather than asserting a text
            //      whose counterpart was never measured.
            // ════════════════════════════════════════════════════════════════
            void emitDynIndexAssignGuard(llvm::Value *dynValue, const char *label);

            // (AR) [م-٠٠١] كتلةُ فشلٍ لقيمةٍ موسومةٍ زمنَ التشغيلِ ليس وسمُها
            //      مصفوفةً، تنتهي بـunreachable: مستضافٌ ⇒ تشخيصٌ عربيٌّ + exit(1)؛
            //      حرٌّ ⇒ __sad_panic برمزٍ مميَّز. نظيرُ emitDynTypeMismatchFailure للخرائط.
            // (EN) [card م-٠٠١] The failure block for a runtime-tagged value whose tag is not
            //      Array, terminated by unreachable. Mirrors the maps' emitDynTypeMismatchFailure.
            void emitDynNotArrayFailure(const char *label);
            llvm::Value *normalizeArrayIndex(llvm::Value *index, llvm::Value *arrPtr, const char *label = "idx");
            void emitBoundsCheck(llvm::Value *index, llvm::Value *arrPtr, const char *label = "bc");

            // (AR) عمليات المصفوفات
            llvm::Value *emitArrayNew(std::shared_ptr<SIRInstruction>);
            /// (AR) يرفض فهرسًا نصّيًّا على عمليّةِ مصفوفة (يمنع تأكيدَ LLVM الحاجب)
            /// (EN) rejects a string index on an array op (prevents the blocking LLVM assert)
            bool rejectStringIndex(const std::shared_ptr<SIRInstruction> &inst);

            // ================================================================
            // (AR) **إرسالُ الفهرسةِ بوسمِ الكائنِ زمنَ التشغيل.**
            //      حين يكون نوعُ الكائنِ الساكنُ «أي» وفهرسُه عددًا، لا يستطيع أيُّ
            //      تحليلٍ ساكنٍ أن يقرّر: `م["ج"][1]` مصفوفةٌ و`خ["أ"][2]` خريطةٌ
            //      بمفتاحٍ عدديّ، وكلتاهما ARRAY_GET/ARRAY_SET بالنوعِ نفسِه. فكان
            //      المسارُ يمضي إلى المصفوفةِ دائمًا فيُهلِعُ حارسُ الوسمِ على الخريطة.
            //      نتفرّعُ هنا على **وسمِ الكائنِ زمنَ التشغيل** بدل النوعِ الساكن:
            //      وسمُ خريطةٍ ⇒ ننصِّصُ المفتاحَ ونستدعي مساعِدَ الخريطة (تناظرًا مع
            //      تنصيصِ المفتاحِ في الأمام)، وإلّا مسارُ المصفوفةِ كما هو.
            // (EN) Runtime-tag dispatch for indexing. When the object's static type is
            //      «أي» and the index is numeric, no static analysis can decide between
            //      an array and a map with a numeric key — both are ARRAY_GET/ARRAY_SET
            //      with the same static type. Branch on the runtime tag instead.
            // ================================================================
            struct DynIndexDispatch
            {
                bool active = false;
                llvm::BasicBlock *contBB = nullptr;
                llvm::Value *slot = nullptr; ///< (AR) خانةُ الدمجِ (%SadDyn) للقراءة
            };

            DynIndexDispatch beginDynMapDispatch(const std::shared_ptr<SIRInstruction> &inst,
                                                 llvm::Value *objValue,
                                                 llvm::Value *index,
                                                 llvm::Value *value,
                                                 bool isSet);
            llvm::Value *endDynMapDispatch(DynIndexDispatch &dispatch, llvm::Value *arrayResult);

            llvm::Value *emitArrayGet(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitArraySet(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitArrayLen(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitArrayConcat(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitArrayZip(std::shared_ptr<SIRInstruction>);

            // (AR) إنشاء النصوص
            llvm::Value *emitStringNew(std::shared_ptr<SIRInstruction>);
        };

    }
} // namespace Sad::LLVM
#endif
