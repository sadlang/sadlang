#pragma once
#include <cstdint>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"
#include "sad_type_system.h" // (AR) لـSad::Types::repr (حارسُ انجرافِ DynKind من مصدرِ الحقيقة الموحَّد)

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
            // (AR) [م-٠٠١] فراغ — «لا قيمةَ هنا» تمييزًا عن «عدم» التي قيمةٌ صريحة.
            //      الفرقُ مقيسٌ لا نظريّ: قراءةُ مفتاحٍ غائبٍ من خريطةٍ تُرجع في المفسّرِ
            //      قيمةَ Void فـ`نوع()` تقولُ «فراغ»، بينما `نوع(لاشيء)` تقولُ «عدم».
            //      وكلتاهما تُطبَعان «لاشيء». وسمٌ خاصٌّ بـLLVM كـMap/Obj/Adt.
            // (EN) [card م-٠٠١] Void — "no value here", as distinct from Null which is an
            //      explicit value. The difference is measured, not theoretical: reading an
            //      absent map key yields a Void value in the interpreter, so نوع() says
            //      «فراغ», whereas نوع(لاشيء) says «عدم». Both print «لاشيء».
            //      An LLVM-only tag, like Map/Obj/Adt.
            inline constexpr uint8_t Void = 9;
        } // namespace DynKind

        // (AR) حارسُ انجرافٍ زمنَ الترجمة: الوسومُ المشترَكةُ (٠–٥) يجب أن تطابق مصدرَ الحقيقة
        //      الموحَّد (Sad::Types::repr، مُولَّد من value_repr.yaml) الذي تشترك فيه الخلفيّةُ الأصليّة
        //      والمفسّر — أيُّ انجرافٍ يصير خطأَ ترجمة. (Map/Obj/Adt خاصّةٌ بـLLVM فلا تُوحَّد بعد.)
        static_assert(DynKind::Null == ::Sad::Types::repr::kDynKindNull, "DynKind drift: Null");
        static_assert(DynKind::Int == ::Sad::Types::repr::kDynKindInt, "DynKind drift: Int");
        static_assert(DynKind::Float == ::Sad::Types::repr::kDynKindFloat, "DynKind drift: Float");
        static_assert(DynKind::Str == ::Sad::Types::repr::kDynKindStr, "DynKind drift: Str");
        static_assert(DynKind::Bool == ::Sad::Types::repr::kDynKindBool, "DynKind drift: Bool");
        static_assert(DynKind::Array == ::Sad::Types::repr::kDynKindArray, "DynKind drift: Array");

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
        /// (AR) ISSUE-063: استخرِج i64 بدقّة: عشريّ⇒fptosi(bitcast(الحمولة))؛ غيره⇒الحمولة
        ///      كما هي (دقّة الصحيح 64-بت محفوظة — لا مرور عبر double). بلا فروع (select).
        /// (EN) ISSUE-063: extract an i64 precisely: Float⇒fptosi(bitcast(payload)); else⇒
        ///      the raw payload (64-bit int precision preserved — no double round-trip).
        ///      Branchless (select).
        llvm::Value *unpackI64(LLVMCodeGen &cg, llvm::Value *dyn);

        /// (AR) يوائم وسيطًا مع نوعِ معاملٍ عند حدِّ نداء: معاملٌ موسوم ⇒ تعليب،
        ///      وسيطٌ موسومٌ لمعاملٍ محسوس ⇒ فكّ، وما عداهما تحويلاتُ مؤشّر/صحيح/عشريّ/منطقيّ.
        ///      استُخرِج لأنّ جدولَ التحويلِ كان منسوخًا في موضعَين كائنيَّين وينقصهما
        ///      الموسومُ في كليهما — والنسخُ هو ما جعل الثغرةَ ثغرتَين.
        ///      وصار **الجدولَ الوحيد**: نسخةُ النداءِ العامّة (`cf_branch_call.cpp`) كانت
        ///      تفكّ إلى i64 بالحمولةِ الخامّ (`dynPayloadI64`) بينما هذه تحترم الوسم
        ///      (`unpackI64`) — أي قيمةٌ وسمُها عشريٌّ تعبر ذاك الحدَّ بنمطِ بتّاتِها عددًا
        ///      صحيحًا: **جوابٌ خاطئٌ صامتٌ** يقرؤه المستخدمُ رقمًا فلكيًّا. فوُحِّدا هنا.
        ///      و`sirType` نوعُ SIR للوسيطِ يُستعمَل في اتّجاهِ **التعليب** وحدَه ليحمل الوسمَ
        ///      الصحيح؛ وتركُه Unknown يستنتجه من نوعِ LLVM (كافٍ في المواضعِ الكائنيّة).
        /// (EN) Reconcile an argument with a parameter type at a call boundary. This is now
        ///      the single cast table: the general call site unpacked to i64 with the RAW
        ///      payload while this one honours the tag, so a Float-tagged value crossed that
        ///      boundary as its bit pattern — a silent wrong answer. `sirType` is used only
        ///      in the packing direction to carry the correct tag.
        llvm::Value *coerceToParamType(LLVMCodeGen &cg, llvm::Value *v, llvm::Type *want,
                                       Compiler::SIR::SadTypeKind sirType =
                                           Compiler::SIR::SadTypeKind::Unknown);

        // ====================================================================
        // (AR) التعليب/فكّ التعليب على الكومة — «option A» لعناصر المصفوفات مختلطة
        //      الأنواع (ISSUE-052/070/080/082). خانةُ المصفوفة 8 بايت لا تسع الوسمَ،
        //      فنُعلّب العنصرَ في %SadDyn بالكومة ونخزّن مؤشّرَه، ونفكّه عند القراءة.
        // (EN) Heap box/unbox — "option A" for heterogeneous-array elements. An 8-byte
        //      slot can't hold the tag, so box the element into a heap %SadDyn, store its
        //      pointer, and unbox on read.
        // ====================================================================
        /// (AR) علّب قيمةً محدَّدة في %SadDyn بالكومة (malloc)، وأرجِع مؤشّرًا (ptr) إليه.
        ///      القيمةُ الديناميّة (%SadDyn أصلًا) تُمرَّر عبر toDyn كما هي.
        /// (EN) Box a concrete value into a heap %SadDyn (malloc); return a pointer to it.
        llvm::Value *boxDynToHeap(LLVMCodeGen &cg, llvm::Value *v, Compiler::SIR::SadTypeKind sirType);
        /// (AR) اقرأ %SadDyn من مؤشّر كومةٍ سبق تعليبه (عكس boxDynToHeap).
        /// (EN) Load a %SadDyn from a heap pointer previously boxed (inverse of boxDynToHeap).
        llvm::Value *unboxDynFromHeap(LLVMCodeGen &cg, llvm::Value *boxPtr);

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
        /// (AR) نوعُ بنيةِ SadArray — التعريفُ **الوحيد**: {i64 طول، i64 سعة، ptr بيانات،
        ///      ptr وسوم (أو null)، i8 نوعٌ متجانس}. كان مكرَّرًا في خمسةِ ملفّاتٍ بصيغتين
        ///      (StructType::create مسمّاةً وStructType::get بنيويّةً) — وهو نفسُ داءِ التكرار
        ///      الذي وحّده value_repr.yaml للثوابت. مكانُه هنا لأنّه تمثيلُ قيمةٍ زمنَ التشغيل.
        /// (EN) The SadArray struct type — the SINGLE definition. It used to be duplicated in
        ///      five files in two spellings; it lives here because it is a runtime value
        ///      representation, exactly like the DynKind tags above.
        llvm::StructType *sadArrayStructType(llvm::LLVMContext &ctx);

        /// (AR) نص(ديناميّ): يوزّع على الوسم (صحيح/عشريّ/منطقيّ/نصّ/مصفوفة/عدم) ويطابق المفسّر.
        ///      مؤشّر char*. يُبعَث **نداءً** لدالّةٍ مولَّدةٍ واحدةٍ لا توسيعًا سطريًّا (انظر
        ///      ensureDynToStringFn) — بلا ذلك يستحيل ذراعُ المصفوفة (عَوْدٌ لا نهائيٌّ زمنَ الترجمة).
        /// (EN) toString(dynamic): dispatches on the kind (int/float/bool/str/array/null),
        ///      matching the interpreter. Emitted as a CALL to one generated function rather
        ///      than expanded inline (see ensureDynToStringFn) — without that the array arm is
        ///      impossible (infinite expansion at compile time).
        llvm::Value *dynToString(LLVMCodeGen &cg, llvm::Value *dyn);

        /// (AR) يبعث (مرّةً واحدة) `i8* __sad_dyn_to_string(i8 وسم، i64 حمولة)` ويعيدها.
        ///      ذاتيّةُ العَوْدِ عبر __sad_array_to_string_dyn لمصفوفةٍ متداخلة. تُنشئ كتلةَ
        ///      «entry» **قبل** أيِّ نداءٍ للمساعِدِ المصفوفيّ، وبذلك يوقف حارسُ `!empty()`
        ///      العَوْدَ المتبادلَ زمنَ التوليد.
        /// (EN) Emits (once) `i8* __sad_dyn_to_string(i8 kind, i64 payload)` and returns it.
        ///      Mutually recursive with __sad_array_to_string_dyn for nested arrays. It creates
        ///      its «entry» block BEFORE calling the array helper, so the `!empty()` guard stops
        ///      the mutual recursion at generation time.
        llvm::Function *ensureDynToStringFn(LLVMCodeGen &cg);
        /// (AR) نوع(ديناميّ): يوزّع على الوسم ويعيد اسم النوع من نفس مصدر sadTypeKindArabicName
        ///      (رقم/عشري/منطقي/نص/…) ⇒ يطابق نوع() المفسّر بايتيًّا. مؤشّر char*.
        /// (EN) typeof(dynamic): dispatches on the kind and returns the type name from the same
        ///      source as sadTypeKindArabicName (رقم/عشري/منطقي/نص/…) ⇒ matches نوع(). char*.
        llvm::Value *dynTypeName(LLVMCodeGen &cg, llvm::Value *dyn);

        // ====================================================================
        // (AR) الحاجز ٧: التقاط هلع زمن التشغيل الجوهريّ (قسمة على صفر…) بـحاول/امسك
        // (EN) Barrier 7: catch an intrinsic runtime panic (division by zero…) via try/catch
        // ====================================================================
        /// (AR) إن وُجد معالِج «حاول» نشط (handlerCount > 0) يرفع استثناءً قابلًا للالتقاط
        ///      عبر آليّة setjmp/longjmp نفسها (يخزّن النوع «خطأ» + الرسالة msgPtr، ثمّ يقفز
        ///      لأعلى معالِج فلا يعود). وإلّا لا يفعل شيئًا ويترك موضعَ الإدراج في كتلة «لا
        ///      معالِج» ليُكمل النادي مساره القديم (طباعة التشخيص + exit(1)). مشترك بين
        ///      المسارين الساكن (arith_main) والديناميّ (dynBinOp).
        /// (EN) If an active «try» handler exists (handlerCount > 0), raise a catchable
        ///      exception via the same setjmp/longjmp mechanism (stores the «خطأ» type +
        ///      msgPtr message, then longjmps to the top handler — never returns). Otherwise
        ///      it is a no-op leaving the insert point at the «no handler» block so the caller
        ///      keeps its old path (print diagnostic + exit(1)). Shared by the static
        ///      (arith_main) and dynamic (dynBinOp) paths.
        void emitRecoverablePanicToHandler(LLVMCodeGen &cg, llvm::Value *msgPtr);

    } // namespace LLVM
} // namespace Sad
