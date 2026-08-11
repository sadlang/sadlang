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
            llvm::Value *normalizeArrayPtr(llvm::Value *arrPtr, const char *label = "arr");

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
