#pragma once
#include <cstdint>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

// ============================================================================
// (AR) التمثيل الديناميّ ذو النوع المميّز `%SadDyn` — حلّ ISSUE-076 الجذريّ.
//      بدل وسم البتّات على i64 (الذي يُلبِس الديناميّ بالمحدَّد)، تحمل القيمةُ
//      الديناميّة نوعَ LLVM مميّزًا واصفًا لذاته:
//        %SadDyn = type { i8 /*kind*/, i64 /*payload*/ }   (16 بايت)
//      يعكس std::variant في المفسّر (value.h). لأنّ نوعه ≠ i64، فإنّ كلّ تحويلٍ
//      من/إلى المحدَّد يُفرَض عبر packDyn/unpack* (يحرسه مدقِّق LLVM)، والكود
//      المحدَّد يبقى نوعه الأصيل. تُمرَّر %SadDyn بالقيمة (ABI مُثبَت: سجلّان).
// (EN) The distinct-typed dynamic representation `%SadDyn` — the ISSUE-076 root
//      fix. Instead of bit-tagging on i64 (which conflates dynamic with typed),
//      a dynamic value carries a distinct self-describing LLVM type:
//        %SadDyn = type { i8 kind, i64 payload }   (16 bytes)
//      mirroring the interpreter's std::variant (value.h). Because its type is
//      not i64, every conversion to/from a concrete type is forced through
//      packDyn/unpack* (guarded by the LLVM verifier), and typed code keeps its
//      native type. %SadDyn is passed by value (ABI proven: two registers).
// ============================================================================

namespace Sad
{
    namespace LLVM
    {
        class LLVMCodeGen;

        // (AR) أسماء حقول البنية / (EN) struct field indices (named — no literals)
        inline constexpr unsigned kDynKindFieldIdx = 0;    // i8  kind
        inline constexpr unsigned kDynPayloadFieldIdx = 1; // i64 payload

        // ====================================================================
        // (AR) وسوم النوع زمنَ التشغيل — تعكس فهرس variant في المفسّر (value.h)،
        //      وتطابق مخرَج نوع()/طباعة بايتيًّا. ثوابت مسمّاة (لا سحرَ عدديّ).
        // (EN) Runtime kind tags — reflect the interpreter variant index (value.h)
        //      and match نوع()/print byte-for-byte. Named constants (no magic).
        // ====================================================================
        namespace DynKind
        {
            inline constexpr uint8_t Null = 0;  // (AR) عدم / (EN) null
            inline constexpr uint8_t Int = 1;   // (AR) صحيح i64 مباشرة / (EN) i64 directly
            inline constexpr uint8_t Float = 2; // (AR) عشريّ: bitcast(double) / (EN) bitcast(double)
            inline constexpr uint8_t Str = 3;   // (AR) نصّ: ptrtoint / (EN) ptrtoint
            inline constexpr uint8_t Bool = 4;  // (AR) منطقيّ 0/1 / (EN) 0/1
            inline constexpr uint8_t Array = 5; // (AR) مصفوفة: مؤشّر مُدار / (EN) managed ptr
            inline constexpr uint8_t Map = 6;   // (AR) خريطة / (EN) map
            inline constexpr uint8_t Obj = 7;   // (AR) كائن / (EN) object
            inline constexpr uint8_t Adt = 8;   // (AR) تعداد جبريّ / (EN) ADT
        } // namespace DynKind

        /// (AR) نوع المقارنة لموزِّع dynCompare / (EN) comparison kind for dynCompare
        enum class DynCmp
        {
            EQ,
            NE,
            LT,
            LE,
            GT,
            GE
        };

        // ====================================================================
        // (AR) الوصول للنوع + الفحص / (EN) type accessor + predicate
        // ====================================================================
        /// (AR) نوع %SadDyn المُخزَّن مرّة (يُعاد بالاسم) / (EN) the cached %SadDyn struct type
        llvm::StructType *getSadDynType(llvm::LLVMContext &ctx);
        /// (AR) هل القيمة من نوع %SadDyn؟ / (EN) is the value a %SadDyn?
        bool isSadDyn(const llvm::Value *v);

        // ====================================================================
        // (AR) البناء (بلا malloc — insertvalue) / (EN) construction (no malloc)
        // ====================================================================
        /// (AR) غلّف قيمةً محدَّدة بوسمٍ ساكن. Float⇒bitcast · Int⇒i64 · Bool⇒zext ·
        ///      نصّ/مؤشّر⇒ptrtoint · عدم⇒0. لا تخصيص كومة. / (EN) pack a concrete value
        ///      with a static kind (Float⇒bitcast, Int⇒i64, Bool⇒zext, ptr⇒ptrtoint,
        ///      Null⇒0). No heap allocation.
        llvm::Value *packDyn(LLVMCodeGen &cg, llvm::Value *concrete, uint8_t kind);
        /// (AR) اجمع من وسمٍ زمنَ التشغيل (i8) وحمولة (i64) / (EN) assemble from a runtime i8 kind + i64 payload
        llvm::Value *makeDyn(LLVMCodeGen &cg, llvm::Value *kindI8, llvm::Value *payloadI64);
        /// (AR) غلّف قيمةً حسب نوع SIR (يشتقّ الوسم) / (EN) pack a value per its SIR type (derives the kind).
        ///      إن كانت القيمة أصلًا %SadDyn تُعاد كما هي / already-%SadDyn is returned as-is.
        llvm::Value *toDyn(LLVMCodeGen &cg, llvm::Value *v, Compiler::SIR::SadTypeKind sirType);

        // ====================================================================
        // (AR) الاستخراج / (EN) extraction
        // ====================================================================
        llvm::Value *dynKindByte(LLVMCodeGen &cg, llvm::Value *dyn);   // (AR) extractvalue 0 (i8)
        llvm::Value *dynPayloadI64(LLVMCodeGen &cg, llvm::Value *dyn); // (AR) extractvalue 1 (i64)
        /// (AR) استخرِج double: Float⇒bitcast(الحمولة)؛ غيره⇒sitofp(الحمولة). ترقيةٌ عدديّة
        ///      آمنة (بلا فروع، عبر select). / (EN) extract a double: Float⇒bitcast(payload);
        ///      else⇒sitofp(payload). Safe numeric promotion (branchless via select).
        llvm::Value *unpackDouble(LLVMCodeGen &cg, llvm::Value *dyn);
        /// (AR) استخرِج مؤشّرًا: inttoptr(الحمولة) / (EN) inttoptr(payload)
        llvm::Value *unpackPtr(LLVMCodeGen &cg, llvm::Value *dyn);

        // ====================================================================
        // (AR) الموزِّعات (تُبنى مرّة، لكلّ المستهلكين) / (EN) dispatchers (built once, all consumers)
        // ====================================================================
        /// (AR) عمليّة ثنائيّة ديناميّة (+ - * / % //): إن كان أيّ طرفٍ عشريًّا ⇒ عمليّة عشريّة
        ///      والنتيجة Float؛ وإلّا صحيحة. حتّى % و// تتبعان الوسم (عشريّ⇒frem/floor(fdiv)،
        ///      صحيح⇒srem/sdiv) مطابقةً للمفسّر (7.5%2=1.5). النتيجة %SadDyn. بلا malloc.
        /// (EN) dynamic binary op (+ - * / % //): if either side is Float ⇒ a float op with a
        ///      Float result; else integer. Even % and // follow the tag (float⇒frem/floor(fdiv),
        ///      int⇒srem/sdiv), matching the interpreter (7.5%2=1.5). Result is %SadDyn. No malloc.
        llvm::Value *dynBinOp(LLVMCodeGen &cg, Compiler::SIR::SIROpcode op, llvm::Value *l, llvm::Value *r);
        /// (AR) مقارنة ديناميّة (== != < <= > >=): إن كان أيّ طرفٍ عشريًّا ⇒ fcmp على double؛
        ///      وإلّا icmp موقَّع. يُغلق «< > بين صندوقين» بنيويًّا. النتيجة i1.
        /// (EN) dynamic compare (== != < <= > >=): if either side is Float ⇒ fcmp on double; else
        ///      signed icmp. Structurally closes "< > between two boxes". Returns i1.
        llvm::Value *dynCompare(LLVMCodeGen &cg, DynCmp cmp, llvm::Value *l, llvm::Value *r);
        /// (AR) نص(ديناميّ): يوزّع على الوسم (صحيح/عشريّ/منطقيّ/نصّ/عدم) ويطابق المفسّر. مؤشّر char*.
        /// (EN) toString(dynamic): dispatches on the kind (int/float/bool/str/null), matches the
        ///      interpreter. Returns a char*.
        llvm::Value *dynToString(LLVMCodeGen &cg, llvm::Value *dyn);
        /// (AR) نوع(ديناميّ): يوزّع على الوسم ويعيد اسم النوع من نفس مصدر sadTypeKindArabicName
        ///      (رقم/عشري/منطقي/نص/…) ⇒ يطابق نوع() المفسّر بايتيًّا. مؤشّر char*.
        /// (EN) typeof(dynamic): dispatches on the kind and returns the type name from the same
        ///      source as sadTypeKindArabicName (رقم/عشري/منطقي/نص/…) ⇒ matches نوع(). char*.
        llvm::Value *dynTypeName(LLVMCodeGen &cg, llvm::Value *dyn);

    } // namespace LLVM
} // namespace Sad
