/**
 * @file sir_constants.h
 * @brief (AR) ثوابت مشتركة لنظام SIR — تُستخدم في frontend و backend
 * @brief (EN) Shared SIR constants — used by both frontend and backend layers
 *
 * (AR) يحتوي على القيم الثابتة المستخدمة عبر طبقات المترجم المختلفة.
 *      يُستورد من ملفات SIR Builder و LLVM Codegen على حد سواء.
 * (EN) Contains constant values shared across different compiler layers.
 *      Imported by both SIR Builder and LLVM Codegen files.
 */

#pragma once

#include <cstdint>
#include <string>

namespace Sad::Compiler
{

    // ──────────────────────────────────────────────────────────────────
    // (AR) قيمة الحارس لتمثيل لاشيء (null) في وقت التشغيل
    //      تُستخدم في:
    //        - عامل الاندماج الفارغ (??) للمقارنة مع لاشيء
    //        - طباعة القيم (اطبع) للتحقق من null قبل العرض
    //        - عمليات القنوات (حاول_استقبل) كقيمة افتراضية عند الفشل
    //      القيمة: INT64_MIN + 1 لتجنب التعارض مع أي رقم صحيح
    // (EN) Sentinel value representing لاشيء (null) at runtime
    //      Used in:
    //        - Null coalesce operator (??) for null comparison
    //        - Print builtin (اطبع) to detect null before display
    //        - Channel operations (حاول_استقبل) as default on failure
    //      Value: INT64_MIN + 1 to avoid collision with any valid integer
    // ──────────────────────────────────────────────────────────────────
    inline constexpr int64_t kSadNullSentinel = -9223372036854775807LL;

    // (AR) نسخة نصية للاستخدام في SIR operands الثابتة
    // (EN) String version for use in constant SIR operands
    inline const std::string kSadNullSentinelStr = "-9223372036854775807";

    // ──────────────────────────────────────────────────────────────────
    // (AR) أسماء الكتل الأساسية والمعاملات المتكررة في بناء SIR
    //      تُوحَّد هنا لتجنب تكرار النصوص الحرفية (CW-10)
    // (EN) Common block/parameter names used throughout SIR building
    //      Centralized here to avoid magic string literals (CW-10)
    // ──────────────────────────────────────────────────────────────────

    // (AR) اسم كتلة الدخول الافتراضي لأي دالة SIR
    // (EN) Default entry block name for any SIR function
    inline constexpr const char *kEntryBlockName = "entry";

    // (AR) اسم معامل self — المعامل الضمني الأول في دوال الصنف
    // (EN) Self parameter name — implicit first parameter in class methods
    inline constexpr const char *kSelfParamName = "self";

    // (AR) اسم سجل self — النسخة المسبوقة بـ % للاستخدام في SIR registers
    // (EN) Self register name — %-prefixed version for SIR register usage
    inline constexpr const char *kSelfRegisterName = "%self";

    // (AR) اسم المرادف العربي لـ this — "هذا" — يُسجَّل كمرادف لـ self
    // (EN) Arabic alias for "this" — "هذا" — registered as alias for self
    inline constexpr const char *kThisAliasName = "\xD9\x87\xD8\xB0\xD8\xA7";

    // ──────────────────────────────────────────────────────────────────
    // (AR) أسماء وقت التشغيل (runtime) للاستثناءات والمعالجات
    //      تُستخدم في LLVM Codegen لإنشاء/الوصول إلى المتغيرات العمومية
    // (EN) Runtime symbol names for exception handling infrastructure
    //      Used in LLVM Codegen to create/access global variables
    // ──────────────────────────────────────────────────────────────────

    // (AR) مكدس معالجات الاستثناءات (مصفوفة jmp_buf)
    // (EN) Exception handler stack (array of jmp_buf)
    inline constexpr const char *kRuntimeHandlerStack = "__sad_handler_stack";

    // (AR) عداد عدد المعالجات النشطة
    // (EN) Active handler count
    inline constexpr const char *kRuntimeHandlerCount = "__sad_handler_count";

    // (AR) نوع الاستثناء الحالي (مؤشر نص)
    // (EN) Current exception type (string pointer)
    inline constexpr const char *kRuntimeExceptionType = "__sad_exception_type";

    // (AR) رسالة الاستثناء الحالي (مؤشر نص)
    // (EN) Current exception message (string pointer)
    inline constexpr const char *kRuntimeExceptionMsg = "__sad_exception_msg";

    // ──────────────────────────────────────────────────────────────────
    // (AR) أسماء وقت التشغيل لجمع مفاتيح/قيم الخريطة عند تكرارها (حلقة «لكل»
    //      والاستيعابات). عقد ABI مع الخلفيّة في map_ops.cpp — تُعيد كلٌّ منهما
    //      SadArray {len,cap,data}. ثوابت مسمّاة تفاديًا للسلاسل الخام.
    // (EN) Runtime symbol names for collecting a map's keys/values during iteration
    //      («for» loop and comprehensions). ABI contract with the backend in
    //      map_ops.cpp — each returns a SadArray {len,cap,data}. Named constants to
    //      avoid raw string literals.
    // ──────────────────────────────────────────────────────────────────
    inline constexpr const char *kRuntimeMapKeys = "__sad_map_keys";
    inline constexpr const char *kRuntimeMapValues = "__sad_map_values";

    // ──────────────────────────────────────────────────────────────────
    // (AR) بادئة أسماء دوال بانِي متغيّرات التعداد الجبريّ (ADT). البانِي
    //      المُولَّد `__adt_ctor_<تعداد>_<متغيّر>` (statement_types.cpp) يُنشئ
    //      بنية {tag، حقول...} عبر ENUM_CONSTRUCT؛ يُستدعى عند `شكل.دائرة(..)`
    //      ويُكشَف بهذه البادئة في التوزيع. ثابتٌ مسمّى تفاديًا للسلسلة الخام.
    // (EN) Name prefix for generated ADT variant constructor functions. The
    //      synthesized `__adt_ctor_<enum>_<variant>` (statement_types.cpp) builds a
    //      {tag, fields...} struct via ENUM_CONSTRUCT; invoked at `Shape.Circle(..)`
    //      and detected by this prefix during dispatch. Named to avoid a raw literal.
    // ──────────────────────────────────────────────────────────────────
    inline constexpr const char *kAdtCtorPrefix = "__adt_ctor_";

    // ──────────────────────────────────────────────────────────────────
    // (AR) سنتينلات حقل `MatchDeferredField::enumName` للتمييز بين نوع
    //      الاستخراج المؤجّل في المطابقة: عنصر قائمة يُربَط بمتغيّر
    //      (`__list_pattern`) أو يُقارَن حرفيًّا (`__list_pattern_literal`)؛
    //      أيّ قيمةٍ أخرى = اسم تعدادٍ حقيقيّ لاستخراج حمولة ADT. ثوابت
    //      مسمّاة تفاديًا للسلاسل الخام (تُقارَن في statement_match.cpp
    //      وsir_builder_match_patterns.cpp).
    // (EN) Sentinels for `MatchDeferredField::enumName` distinguishing the kind
    //      of deferred match extraction: a list element bound to a variable
    //      (`__list_pattern`) or compared as a literal (`__list_pattern_literal`);
    //      any other value = a real enum name for ADT payload extraction. Named
    //      constants to avoid raw literals (compared in statement_match.cpp and
    //      sir_builder_match_patterns.cpp).
    // ──────────────────────────────────────────────────────────────────
    inline constexpr const char *kListPatternSentinel = "__list_pattern";
    inline constexpr const char *kListPatternLiteralSentinel = "__list_pattern_literal";

    // ──────────────────────────────────────────────────────────────────
    // (AR) [ISSUE-080] وسمُ «وضع التوزيع» في المعامل [1] من ENUM_GET_PAYLOAD:
    //      الوصول النقطيّ المباشر `قيمة.حقل` على تعدادٍ جبريّ لا يعرف حالته
    //      سكونيًّا، فبدل فهرسٍ ثابتٍ (بحثٍ أحاديّ يخطئ عند تصادم الأسماء عبر
    //      الحالات) يمرّر هذا الحارس ثمّ لائحةَ أزواج (وسم الحالة، فهرس الحقل
    //      داخلها)؛ تبني الخلفيّةُ توزيعًا زمن-تشغيليّ يقرأ الوسم ويختار الخانة
    //      الصحيحة، وتُطلق trap عند حالةٍ لا تحوي الحقل. الفهارس الحقيقيّة ≥ 0
    //      فالسالب حارسٌ آمن. (enum_ops.cpp / expression_members|objects.cpp)
    // (EN) [ISSUE-080] «dispatch mode» marker in ENUM_GET_PAYLOAD operand[1]:
    //      direct `value.field` access on an ADT whose variant is unknown at
    //      compile time. Instead of a static index (single-scan lookup that is
    //      wrong on cross-variant name collisions), this sentinel is followed by
    //      a list of (variant tag, in-variant field index) pairs; the backend
    //      builds a runtime dispatch that reads the tag, selects the correct
    //      slot, and traps on a variant lacking the field. Real indices are ≥ 0
    //      so a negative is a safe sentinel.
    // ──────────────────────────────────────────────────────────────────
    inline constexpr int64_t kAdtFieldDispatchSentinel = -1;

    // (AR) رسالة الـ trap عند وصول حقلٍ لحالةٍ لا تحويه (ISSUE-080، العلّة ب).
    //      تطابقُ رسالة RUN005 البايتيّ للمفسّر غير عمليّ (ErrorManager كامل)؛
    //      البديل توقّفٌ حتميّ بخروج ≠0 يمنع القمامة الصامتة. %s = اسم التعداد.
    // (EN) Trap message when a field is accessed on a variant that lacks it
    //      (ISSUE-080, cause b). Byte-matching the interpreter's RUN005 report is
    //      impractical; the alternative is a deterministic exit≠0 halt that
    //      prevents silent garbage. %s = enum name.
    inline constexpr const char *kAdtWrongVariantFieldMsg =
        "Error [RUN005]: ADT field access on a variant that does not contain the field (enum '%s')\n";

    // ──────────────────────────────────────────────────────────────────
    // (AR) علامة SOH ‎(\x01)‎ في بداية قالب INLINE_ASM: نصّ بصيغة LLVM جاهزة
    //      (‎$$‎ للثابت، ‎$N‎ للمعامل، ‎%reg‎ للسجلّ) تُصدرها لهجة التجميع العربيّ (م١)
    //      في statement_asm.cpp، فتتخطّى الخلفيّة (strings_ops.cpp) تحويل صيغة
    //      GCC→LLVM وتحذف العلامة. عقد بين طبقتَي المترجم ⇒ ثابت واحد مشترك.
    // (EN) SOH ‎(\x01)‎ prefix on an INLINE_ASM template: the text is already
    //      LLVM-style (emitted by the Arabic assembly dialect, M1) — the backend
    //      skips its GCC→LLVM conversion and strips the marker. A contract
    //      between compiler layers ⇒ one shared named constant.
    // ──────────────────────────────────────────────────────────────────
    inline constexpr char kRawLlvmAsmMarker = '\x01';

} // namespace Sad::Compiler
