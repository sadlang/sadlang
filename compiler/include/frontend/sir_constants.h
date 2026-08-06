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
    // (AR) رموز سبب الهلع (ABI نداء __sad_panic في الوضع الحرّ) — الوسيط
    //      الوحيد الذي يمرّره المولِّد إلى __sad_panic ليصنّف البرنامجُ الحرّ
    //      (نواة) لافتته. كانت كلّها الثابت السحريّ 1 ⇒ يُصنَّف الكلّ خطأً
    //      واحدًا؛ فُصِّلت هنا كعقد مستقرّ يُخرِّطه المستهلك لرموز كتالوجه.
    //      عقد مستقرّ: لا تُعِد ترقيم القيم القائمة (المستهلكون يعتمدونها).
    // (EN) __sad_panic reason codes (freestanding ABI) — the sole argument
    //      the backend passes to __sad_panic so a freestanding program (a
    //      kernel) can classify its banner. Previously all sites passed the
    //      magic 1; split here into a stable contract mapped by the consumer.
    //      STABLE: do not renumber existing values (consumers depend on them).
    // ──────────────────────────────────────────────────────────────────
    //   1 = فشلُ فحصٍ/تأكيدٍ بنيويّ (سواء زرعه المولِّد: حدّ مصفوفة، وسم تعداد
    //       خاطئ؛ أو استدعاه المستخدم: إجهاض، مدمج أمنيّ) — التصنيف التاريخيّ،
    //       تبقى قيمته 1 للتوافق.
    inline constexpr int64_t kSadPanicCheckViolation = 1;
    //   2 = فشلُ عامل التأكيد اللاحق «مؤكَّد» على قيمة عدم (مُناظِر RUN056) —
    //       مميَّز كي لا يُخلَط بانتهاك فحص بنيويّ (ليس تجاوز حدّ). حصريّ لعامل
    //       «مؤكَّد» اللاحق؛ مدمجات «تأكد»/«ذعر» تُصنَّف 1.
    inline constexpr int64_t kSadPanicNullAssert = 2;
    //   3 = طريقةٌ أُرسِلت على قيمةٍ موسومةٍ زمنَ التشغيل (نوعُها الساكنُ «أي»)
    //       فخالف وسمُها النوعَ الذي تتطلّبه الطريقة — مثالُه: طريقةُ خريطةٍ على
    //       قيمةٍ وسمُها نصّ. مميَّزٌ عن انتهاك الفحصِ البنيويّ لأنّه خطأُ نوعٍ
    //       في برنامج المستخدم لا تجاوزُ حدٍّ، وعن تأكيد العدم لأنّ القيمة حاضرة.
    // (EN) 3 = a method dispatched on a runtime-tagged value (static type «أي»)
    //       whose tag contradicts the type the method requires — e.g. a map method
    //       on a string-tagged value. Distinct from a structural check violation
    //       (a user type error, not an out-of-bounds) and from the null assert
    //       (the value is present).
    inline constexpr int64_t kSadPanicDynTypeMismatch = 3;
    //   4 = استثناءٌ رماه المستخدمُ بـ«ارمي» ولم يلتقطه «حاول/امسك» — مُناظِر
    //       RUN052 في المفسّر. مميَّزٌ عن انتهاك الفحص لأنّه مسارُ تحكّمٍ قصدَه
    //       المستخدم لا خللٌ بنيويّ، وعن تعارض الوسم لأنّ لا نوعَ خُولف.
    // (EN) 4 = an exception the user raised with «ارمي» that no «حاول/امسك» caught —
    //       the counterpart of the interpreter's RUN052. Distinct from a check
    //       violation (this is deliberate control flow, not a structural fault)
    //       and from a tag mismatch (no type was contradicted).
    inline constexpr int64_t kSadPanicUncaughtThrow = 4;
    // (AR) حارس زمن-ترجمة: القيم يجب أن تبقى متمايزة (يحمي العقد من
    //      تصادم قيم سهوًا عند إضافة رموز مستقبلًا). (EN) contract guard.
    static_assert(kSadPanicCheckViolation != kSadPanicNullAssert &&
                      kSadPanicNullAssert != kSadPanicDynTypeMismatch &&
                      kSadPanicCheckViolation != kSadPanicDynTypeMismatch &&
                      kSadPanicUncaughtThrow != kSadPanicCheckViolation &&
                      kSadPanicUncaughtThrow != kSadPanicNullAssert &&
                      kSadPanicUncaughtThrow != kSadPanicDynTypeMismatch,
                  "panic reason codes must stay distinct");

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

    // (AR) حمولة الاستثناء الحالي كبتّات i64 (رقم/منطقيّ/عشريّ مُعاد التفسير) —
    //      يفصل القيمة غير النصّيّة عن مؤشّر الرسالة النصّيّ فلا تُخزَّن i64 في مؤشّر
    //      (كان يُحطّم المترجَم). للكائنات/النصوص يُترك المؤشّر في kRuntimeExceptionMsg.
    // (EN) Current exception payload as i64 bits (number/bool/float reinterpreted) —
    //      keeps a non-string thrown value separate from the string message pointer,
    //      so an i64 is never stored into a ptr global (which crashed the compiler).
    //      For objects/strings the pointer stays in kRuntimeExceptionMsg.
    inline constexpr const char *kRuntimeExceptionValue = "__sad_exception_value";

    // (AR) عدّاد «حاول» النشطة (ذاتيّ ودقيق) — يميّز معالِج try/catch الحقيقيّ عن
    //      معالِج تنظيف الدالّة (defer/finally) المدفوع لكلّ دالّة على المكدّس نفسه.
    //      يستعمله حاجزُ الهلع الجوهريّ (قسمة على صفر) ليقرّر: يرفع استثناءً قابلًا
    //      للالتقاط فقط عند وجود «حاول» فعليّة (> 0)، وإلّا يطبع التشخيص ويخرج.
    //      لا يمسّ آليّةَ رمي/التقاط القيم إطلاقًا (سلامة النقطتين ١+٢).
    // (EN) Active-«try» counter (self-contained, precise) — distinguishes a real
    //      try/catch handler from the per-function cleanup (defer/finally) handler
    //      pushed onto the same stack. The intrinsic-panic guard (division by zero)
    //      uses it to decide: raise a catchable exception only when a real «try» is
    //      active (> 0), else print the diagnostic and exit. It never touches the
    //      value throw/catch mechanism (keeps points 1+2 intact).
    inline constexpr const char *kRuntimeTryActive = "__sad_try_active";

    // (AR) دالّةُ زمنِ التشغيل التي تُبلِّغ عن استثناءٍ لم يلتقطه أحدٌ ثمّ تخرج
    //      برمز 1 (مُناظِر RUN052 في المفسّر). توقيعُها
    //      (const char *type, const char *msg, long long value):
    //      توقيعُها (const char *prefix, const char *suffix, const char *msg,
    //      long long value): الشطران من صيغةِ RUN052 المولَّدة، والحمولةُ العدديّةُ
    //      تُطبَع حين تكون الرسالةُ null (‹ارمي 404›).
    // (EN) The runtime function that reports an uncaught exception then exits with 1
    //      (the counterpart of the interpreter's RUN052). Signature is
    //      (const char *prefix, const char *suffix, const char *msg, long long value): the
    //      numeric payload is printed when the message pointer is null («ارمي 404»).
    inline constexpr const char *kRuntimeReportUnhandledException =
        "sad_report_unhandled_exception";

    // (AR) النائبُ الذي تُشطَر عنده صيغةُ RUN052 المولَّدة إلى شطرَين يُمرَّران إلى زمن
    //      التشغيل، فتُحشى القيمةُ المرميّةُ بينهما. اسمُه معرَّفٌ في مصدر الحقيقة
    //      (language-truth/errors/runtime.yaml ⇒ placeholders: [message]).
    // (EN) The placeholder at which the generated RUN052 format is split into the two
    //      halves handed to the runtime, with the thrown value interpolated between them.
    //      Its name is defined by the source of truth (placeholders: [message]).
    inline constexpr const char *kUserThrownMessagePlaceholder = "{message}";

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
    // (AR) بقيّةُ أسماءِ زمنِ التشغيلِ للخريطة — العقدُ نفسُه مع map_ops.cpp.
    //      كانت مكرَّرةً سلاسلَ خامًّا في خمسةِ ملفّاتٍ أماميّة (الفهرسة،
    //      الطرائق، الاستيعابات، حلقةُ لكل، الثنائيّات)؛ وُحّدت هنا كي لا
    //      يتباعدَ طرفا العقدِ صامتَين.
    // (EN) The remaining map runtime symbol names — the same contract with
    //      map_ops.cpp. They were duplicated as raw literals across five
    //      frontend files (indexing, methods, comprehensions, for-loop,
    //      binary ops); unified here so the two ends cannot drift silently.
    // ──────────────────────────────────────────────────────────────────
    inline constexpr const char *kRuntimeMapCreate = "__sad_map_create";
    inline constexpr const char *kRuntimeMapGet = "__sad_map_get";
    inline constexpr const char *kRuntimeMapGetI64 = "__sad_map_get_i64";
    inline constexpr const char *kRuntimeMapSetTyped = "__sad_map_set_typed";
    inline constexpr const char *kRuntimeMapSize = "__sad_map_size";
    inline constexpr const char *kRuntimeMapHas = "__sad_map_has";
    inline constexpr const char *kRuntimeMapDelete = "__sad_map_delete";

    // (AR) نسخٌ سطحيٌّ للخريطة. تحتاجُه الواجهةُ المسمّاةُ وحدَها: `خريطة_عين`
    //      و`خريطة_احذف` **نقيّتان** في المفسّرِ — تُرجعان خريطةً جديدةً ولا تمسّان
    //      الأصل، بخلافِ الصيغةِ بالأقواسِ `م[ك] = ق` التي تُعدّلُ في مكانِها.
    //      فالفرقُ في العقدِ لا في التنفيذ، والمفسّرُ هو الحَكَم.
    // (EN) Shallow map copy. Needed only by the named interface: `خريطة_عين` and
    //      `خريطة_احذف` are **pure** in the interpreter — they return a new map and
    //      leave the original untouched, unlike the bracket form `م[ك] = ق` which
    //      mutates in place. The difference is in the contract, not the
    //      implementation, and the interpreter is the judge.
    inline constexpr const char *kRuntimeMapCopy = "__sad_map_copy";

    // (AR) قراءةٌ موسومةٌ زمنَ التشغيل: تُرجع %SadDyn بوسمِ القيمةِ الحقيقيِّ
    //      المخزَّنِ في مصفوفةِ types بدل تسويةِ كلِّ شيءٍ نصًّا. عقدٌ جديدٌ مع
    //      map_ops.cpp يغلقُ ق٣ من البطاقة م-٠٠١.
    // (EN) Runtime-tagged read: returns %SadDyn carrying the value's real tag
    //      from the types array instead of flattening everything to a string.
    //      A new contract with map_ops.cpp closing card م-٠٠١ criterion ق٣.
    inline constexpr const char *kRuntimeMapGetDyn = "__sad_map_get_dyn";

    // (AR) إزالةُ التشكيلِ العربيِّ من نصّ — نظيرُ `ازل_تشكيل` في المفسّر.
    // (EN) Strip Arabic diacritics from a string — the compiler counterpart of
    //      the interpreter's `ازل_تشكيل`.
    inline constexpr const char *kRuntimeStripDiacritics = "__sad_strip_diacritics";

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

    // (AR) تشخيص المطوّر (مستضاف فقط) لفشل عامل التأكيد اللاحق «مؤكَّد» على قيمة
    //      عدم (RUN056). في الوضع الحرّ يُستبدل بنداء __sad_panic(kSadPanicNullAssert)
    //      واللافتة السياديّة هي التشخيص الوحيد.
    // (EN) Hosted-only developer diagnostic for a failed «مؤكَّد» null-assert
    //      (RUN056); freestanding replaces it with __sad_panic(kSadPanicNullAssert).
    inline constexpr const char *kNullAssertRun056Msg =
        "خطأ [RUN056]: عامل التأكيد (مؤكَّد) طُبِّق على قيمة عدم\n";

    // ──────────────────────────────────────────────────────────────────
    // (AR) أوسامُ قيمةِ الخريطة — عقدٌ بين الأمامِ (يزرع الوسمَ في تعليمة
    //      __sad_map_set_typed) والخلفِ (يقرؤه ويكتبه في مصفوفة types).
    //      كانت القيمُ مكرّرةً حرفيًّا في الطرفَين: الأمامُ في
    //      expression_collections.cpp والخلفُ في map_ops.cpp — عقدٌ مشترَكٌ
    //      مكتوبٌ مرّتين، فبذرةُ انحرافٍ صامتٍ إن غُيّر أحدُهما وحدَه.
    // (EN) Map value tags — a contract between the frontend (which plants the tag
    //      in the __sad_map_set_typed instruction) and the backend (which reads it
    //      and writes the types array). The values used to be duplicated verbatim
    //      on both sides — a shared contract written twice, hence a silent-drift
    //      hazard if either side changed alone.
    // ──────────────────────────────────────────────────────────────────
    // ──────────────────────────────────────────────────────────────────
    // (AR) اسما مساعِدَي تصييرِ الخريطةِ نصًّا. للمفسّرِ **صيغتان** مقيستان:
    //      الطباعةُ تقتبسُ المفاتيح «{"أ": 1}» و`toString`/`نص()` لا تقتبسُها
    //      «{أ: 1}». الاسمانِ ثابتانِ مسمّيانِ لا حرفيّاتٍ مبعثرة: كانا مكرَّرَين
    //      في تسعةِ مواضع، وخطأٌ مطبعيٌّ واحدٌ في `getOrInsertFunction` يُنتِجُ
    //      تصريحًا بلا تعريفٍ لا يظهرُ إلّا خطأَ ربطٍ غامضًا عندَ التوصيل.
    // (EN) The two map-stringifier helper names. The interpreter has TWO measured
    //      spellings (quoted for print, unquoted for toString/نص()). Named constants,
    //      not literals scattered across nine call sites where one typo yields an
    //      undefined symbol surfacing only as an obscure link error.
    // ──────────────────────────────────────────────────────────────────
    inline constexpr const char *kMapToStringQuotedFn = "__sad_map_to_string";
    inline constexpr const char *kMapToStringPlainFn = "__sad_map_to_string_plain";

    inline constexpr int64_t kMapValueTagString = 0;
    inline constexpr int64_t kMapValueTagInteger = 1;
    inline constexpr int64_t kMapValueTagFloat = 2;
    inline constexpr int64_t kMapValueTagBoolean = 3;
    // (AR) [م-٠٠١] عدم — قيمةُ `لاشيء` الصريحة. كانت تسقطُ في وسمِ النصِّ صامتةً
    //      فتقولُ `نوع(خ["ك"])` «نصّ» والمفسّرُ «عدم»، بينما يفشلُ مسارُ القيمةِ
    //      الموسومةِ زمنَ التشغيل **صاخبًا** للوسمِ نفسِه — حارسٌ مزدوجُ المعيار.
    //      والقيمةُ يقابلُها `SVAL_NULL = 4` في زمنِ التشغيلِ المُضمَّن.
    // (EN) [card م-٠٠١] Null — the explicit `لاشيء` value. It used to fall silently into the
    //      string tag, so `نوع(خ["k"])` said «نصّ» where the interpreter said «عدم», while the
    //      runtime-tagged path failed **loudly** on the very same tag — a double-standard
    //      guard. The value matches `SVAL_NULL = 4` in the embedded runtime.
    inline constexpr int64_t kMapValueTagNull = 4;
    // (AR) [م-٠٠١] فراغ — «لا قيمةَ هنا»، تمييزًا عن «عدم» الصريحة. تنشأُ حين تُخزَّنُ
    //      نتيجةُ قراءةِ مفتاحٍ غائبٍ في خريطةٍ أخرى: `ج["س"] = م["غائب"]`؛ فالمفسّرُ
    //      يقولُ `نوع(ج["س"])` «فراغ» لا «عدم». تقابلُ `DynKind::Void`.
    // (EN) [card م-٠٠١] Void — "no value here", as distinct from an explicit Null. It arises
    //      when the result of reading an absent key is stored into another map:
    //      `ج["س"] = م["غائب"]`; the interpreter then says `نوع(ج["س"])` is «فراغ», not «عدم».
    //      Mirrors `DynKind::Void`.
    inline constexpr int64_t kMapValueTagVoid = 5;
    // (AR) [م-٠٠١] الحاويات. كان فضاءُ الأوسامِ يخلو منهما، فقيمةٌ خريطةٌ أو مصفوفةٌ
    //      تُخزَّنُ بوسمِ النصّ: ترويسةُ الخريطةِ تُقرأُ `char*` عندَ الطباعة، وفهرسةُ
    //      `م["أ"]["ب"]` بعدَ توسيمِ القراءةِ كانت تُجهِضُ المصرّفَ بتوكيدِ LLVM.
    //      يقابلانِ `DynKind::Map` و`DynKind::Array`.
    // (EN) [card م-٠٠١] Containers. The tag space had neither, so a map or array value was
    //      stored under the string tag: the map header was read as a `char*` when printed, and
    //      indexing `م["أ"]["ب"]` after the read became tagged aborted the compiler on an LLVM
    //      assertion. These mirror `DynKind::Map` and `DynKind::Array`.
    inline constexpr int64_t kMapValueTagMap = 6;
    inline constexpr int64_t kMapValueTagArray = 7;

    // ──────────────────────────────────────────────────────────────────
    // (AR) [م-٠٠١] الاشتقاقُ الوحيدُ لوسمِ قيمةِ الخريطةِ من نوعِها الساكن.
    //
    //      كان لهذا العقدِ **أربعةُ كتّابٍ** يكتبون السُّلَّمَ نفسَه يدويًّا: الفهرسةُ،
    //      والخريطةُ الحرفيّةُ، و`خريطة_عين`، و`.عين()`. فلمّا أُضيف وسمُ العدمِ زُرِعَ
    //      في اثنَين وأُغفِلَ في اثنَين، فسقطَ العدمُ في وسمِ النصِّ ⇒ مؤشّرُ عدمٍ
    //      يُقرأُ `char*` ⇒ SIGSEGV. الدواءُ اشتقاقٌ واحدٌ يستدعيه الجميع — لا رقعةٌ
    //      في موضعِ ظهورِ العَرَض.
    // (EN) [card م-٠٠١] The single derivation of a map value's tag from its static type.
    //
    //      This contract had **four writers** open-coding the same ladder: indexing, the map
    //      literal, `خريطة_عين`, and `.عين()`. When the null tag was added it was planted in
    //      two and missed in two, so null fell into the string tag ⇒ a null pointer read as
    //      `char*` ⇒ SIGSEGV. The cure is one derivation every writer calls — not a patch
    //      where the symptom surfaced.
    // ──────────────────────────────────────────────────────────────────
    template <typename TypeKind>
    inline int64_t mapValueTagFor(TypeKind valueType)
    {
        switch (valueType)
        {
        case TypeKind::Integer:
        case TypeKind::Byte:
        case TypeKind::UInt64:
            return kMapValueTagInteger;
        case TypeKind::Float:
            return kMapValueTagFloat;
        case TypeKind::Boolean:
            return kMapValueTagBoolean;
        case TypeKind::Null:
            return kMapValueTagNull;
        case TypeKind::Void:
            return kMapValueTagVoid;
        case TypeKind::Map:
        case TypeKind::Struct:
            return kMapValueTagMap;
        case TypeKind::Array:
            return kMapValueTagArray;
        default:
            // (AR) نصٌّ وكلُّ ما يُمرَّرُ مؤشّرًا.
            // (EN) String and anything else passed as a pointer.
            return kMapValueTagString;
        }
    }

    // (AR) تشخيص المطوّر (مستضاف فقط) لإرسال طريقةٍ على قيمةٍ موسومةٍ زمنَ التشغيل
    //      خالف وسمُها النوعَ المطلوب — مثالُه طريقةُ خريطةٍ على قيمةٍ وسمُها نصّ.
    //      يقابله في الوضع الحرّ __sad_panic(kSadPanicDynTypeMismatch).
    // (EN) Hosted-only developer diagnostic for a method dispatched on a
    //      runtime-tagged value whose tag contradicts the required type — e.g. a
    //      map method on a string-tagged value. Freestanding counterpart:
    //      __sad_panic(kSadPanicDynTypeMismatch).
    inline constexpr const char *kDynTypeMismatchMapMsg =
        "خطأ: طريقةُ خريطةٍ طُبِّقت على قيمةٍ ليست خريطة (النوع الساكن: أي)\n";

    // (AR) [م-٠٠١] نظيرُها للمصفوفات: فهرسةٌ بعددٍ على قيمةٍ موسومةٍ زمنَ التشغيلِ
    //      ليس وسمُها مصفوفةً. رسالةٌ مفصولةٌ لا مشترَكةٌ مع رسالةِ الخريطة: تشخيصٌ
    //      يصفُ ما لم يحدثْ أسوأُ من لا تشخيص.
    // (EN) [card م-٠٠١] The array counterpart: an integer index on a runtime-tagged value whose
    //      tag is not Array. A separate message rather than sharing the map one — a diagnostic
    //      that describes what did not happen is worse than none.
    inline constexpr const char *kDynTypeMismatchArrayMsg =
        "خطأ: فهرسةٌ بعددٍ طُبِّقت على قيمةٍ ليست مصفوفة (النوع الساكن: أي)\n";

    // (AR) تشخيص المطوّر (مستضاف فقط) للقسمة العشريّة على صفر (RUN001) — الحارس
    //      الزمنيّ المزروع قبل fdiv (نمط emitBoundsCheck/emitNullAssert): كان
    //      LLVM يطوي fdiv إلى nan/inf بصمت فيتباعد عن رفض المفسّر الصريح، وكانت
    //      هذه بوّابة NaN الوحيدة في ص (لا حرفيّة NaN في اللغة). صيغة printf
    //      بموضع %g للمقسوم — مرآةُ العنصر النائب {a} في رسالة الكتالوج RUN001.
    //      في الوضع الحرّ يُستبدل بنداء __sad_panic(kSadPanicCheckViolation)
    //      واللافتة السياديّة هي التشخيص الوحيد.
    // (EN) Hosted-only developer diagnostic for float division by zero (RUN001) —
    //      the runtime guard planted before fdiv (emitBoundsCheck/emitNullAssert
    //      pattern): LLVM silently folded fdiv to nan/inf, diverging from the
    //      interpreter's explicit rejection, and this was the language's only NaN
    //      gate (no NaN literal exists). printf format with %g for the dividend —
    //      mirrors the {a} placeholder of the RUN001 catalog message. Freestanding
    //      replaces it with __sad_panic(kSadPanicCheckViolation).
    inline constexpr const char *kDivZeroRun001Msg =
        "خطأ [RUN001]: محاولة قسمة %g على صفر\n";

    // (AR) تشخيصا القسمة الأرضيّة العشريّة والباقي العشريّ على صفر (RUN009/RUN010)
    //      — إغلاق بوّابتَي NaN المتبقّيتين بعد سدّ fdiv (نفس نمط الحارس؛ %g مرآة
    //      {a} في رسالتَي الكتالوج). الوضع الحرّ يستبدلهما بـ__sad_panic كذلك.
    // (EN) Hosted diagnostics for float floor-division and float modulo by zero
    //      (RUN009/RUN010) — closing the two NaN gates remaining after the fdiv
    //      guard (same guard pattern; %g mirrors the catalog {a} placeholder).
    //      Freestanding likewise replaces them with __sad_panic.
    inline constexpr const char *kFloorDivZeroRun009Msg =
        "خطأ [RUN009]: محاولة قسمة صحيحة لـ %g على صفر\n";
    inline constexpr const char *kModZeroRun010Msg =
        "خطأ [RUN010]: محاولة حساب باقي %g %% 0\n";

    // (AR) النظائر الصحيحة (%lld للمقسوم i64) — سدّ تباعد قسمة/أرضيّة/باقي
    //      الصحيحين على صفر: المفسّر يرمي RUN001/RUN009/RUN010 بينما كان المترجَم
    //      يتباين (المسار العامّ 0/لاشيء صامتة؛ الساكن المستنتَج ينهار 0xC0000094
    //      بقسمة عتاديّة على صفر). نفس نمط الحارس؛ حرًّا __sad_panic.
    // (EN) Integer counterparts (%lld for the i64 dividend) — closing the
    //      int div/floor-div/modulo-by-zero divergence: the interpreter throws
    //      RUN001/RUN009/RUN010 while the compiled side diverged (generic path:
    //      silent 0/null; inferred static path: 0xC0000094 hardware divide
    //      crash). Same guard pattern; freestanding uses __sad_panic.
    inline constexpr const char *kDivZeroRun001IntMsg =
        "خطأ [RUN001]: محاولة قسمة %lld على صفر\n";
    inline constexpr const char *kFloorDivZeroRun009IntMsg =
        "خطأ [RUN009]: محاولة قسمة صحيحة لـ %lld على صفر\n";
    inline constexpr const char *kModZeroRun010IntMsg =
        "خطأ [RUN010]: محاولة حساب باقي %lld %% 0\n";

} // namespace Sad::Compiler
