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

} // namespace Sad::Compiler
