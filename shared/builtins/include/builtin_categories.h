/**
 * @file builtin_categories.h
 * @brief (AR) تصنيفات الدوال المدمجة — يحدد كيف تُعامل كل دالة في المفسر والمترجم
 * @brief (EN) Builtin function categories — determines how each function is treated
 *
 * @details
 * (AR) هذا الملف يعرّف التصنيفات الأربعة للدوال المدمجة:
 *      1. CORE_IO: دوال الإدخال/الإخراج الأساسية — لا تحتاج استيراد
 *      2. TYPE_CONSTRUCTOR: بانيات الأنواع — جزء من نظام الأنواع، لا تحتاج استيراد
 *      3. MODULE_FUNCTION: دوال تحتاج استيراد وحدة محددة
 *      4. TYPE_METHOD: طرق مدمجة على الأنواع (مصفوفة.رتب، نص.طول) — لا تحتاج استيراد
 *
 * (EN) This file defines the four categories for builtin functions:
 *      1. CORE_IO: Basic I/O functions — no import needed
 *      2. TYPE_CONSTRUCTOR: Type constructors — part of type system, no import needed
 *      3. MODULE_FUNCTION: Functions that require importing a specific module
 *      4. TYPE_METHOD: Built-in methods on types — no import needed
 *
 * القرار المعماري: ADR-003 — توحيد المكتبات القياسية
 * Architecture Decision: ADR-003 — Stdlib Unification
 *
 * @author فريق تطوير لغة ص / Sad Language Development Team
 * @date 2025-07-19
 */

#pragma once

#include <cstdint>

namespace Sad
{
    namespace Builtins
    {

        // ════════════════════════════════════════════════════════════════════
        // (AR) تصنيف الدالة المدمجة — يحدد سلوك المفسر والمترجم معاً
        // (EN) Builtin function category — determines both interpreter and compiler behavior
        // ════════════════════════════════════════════════════════════════════

        enum class BuiltinCategory : uint8_t
        {
            /// (AR) دوال الإدخال/الإخراج الأساسية — متاحة دائماً بدون استيراد
            /// (EN) Core I/O functions — always available without import
            /// مثال: اطبع(), اطبع_سطر(), اقرأ()
            CORE_IO = 0,

            /// (AR) بانيات الأنواع — جزء من نظام الأنواع، متاحة دائماً
            /// (EN) Type constructors — part of the type system, always available
            /// مثال: رقم(), عشري(), نص(), منطقي()
            TYPE_CONSTRUCTOR = 1,

            /// (AR) دوال تحتاج استيراد وحدة محددة قبل الاستخدام
            /// (EN) Functions that require importing a specific module
            /// مثال: جذر() ← استورد رياضيات
            MODULE_FUNCTION = 2,

            /// (AR) طرق مدمجة على الأنواع — تُستدعى عبر . (dot notation)
            /// (EN) Built-in methods on types — called via . (dot notation)
            /// مثال: مصفوفة.رتب(), نص.طول(), خريطة.أضف()
            TYPE_METHOD = 3
        };

        // ════════════════════════════════════════════════════════════════════
        // (AR) استراتيجية المترجم — كيف يولّد المترجم كود LLVM لهذه الدالة
        // (EN) Compiler strategy — how the compiler generates LLVM code for this function
        // ════════════════════════════════════════════════════════════════════

        enum class CompilerStrategy : uint8_t
        {
            /// (AR) تُوسّع مباشرة إلى تعليمات SIR/LLVM (بدون استدعاء دالة)
            /// (EN) Expanded inline to SIR/LLVM instructions (no function call)
            /// مثال: اطبع(), رقم(), طول()
            INLINE = 0,

            /// (AR) تُولّد كاستدعاء لدالة وقت التشغيل في runtime
            /// (EN) Generated as a call to a runtime function
            /// مثال: عشوائي(), مدى()
            RUNTIME_CALL = 1,

            /// (AR) تُترجم إلى LLVM intrinsic مباشرة
            /// (EN) Translated to LLVM intrinsic directly
            /// مثال: جذر() → llvm.sqrt, جيب() → llvm.sin
            LLVM_INTRINSIC = 2
        };

        // ════════════════════════════════════════════════════════════════════
        // (AR) أنواع البيانات المدعومة لطرق الأنواع
        // (EN) Supported data types for type methods
        // ════════════════════════════════════════════════════════════════════

        enum class TargetType : uint8_t
        {
            NONE = 0,    /// (AR) لا يوجد نوع محدد / (EN) No specific type
            STRING = 1,  /// (AR) نص / (EN) String
            ARRAY = 2,   /// (AR) مصفوفة / (EN) Array
            MAP = 3,     /// (AR) خريطة / (EN) Map
            CHANNEL = 4, /// (AR) قناة / (EN) Channel
            ANY = 5      /// (AR) أي نوع / (EN) Any type
        };

        // ════════════════════════════════════════════════════════════════════
        // (AR) دلالة المدمجات البتّيّة الموحَّدة بين المحرّكين (rfcs#43):
        //      قناع عدّاد الإزاحة — 6 بتّات (0–63) لعرض i64 (دلالة عتاد x86-64).
        //      العدّاد السالب/الكبير يُقنَّع حتميًّا بلا UB في المفسّر والمترجم معًا.
        //      ثابت واحد مشترك كي لا تتباعد نسختا المحرّكين (مراجعة أميليا ت-1).
        // (EN) Cross-engine bitwise-builtin semantics (rfcs#43): shift-count mask —
        //      6 bits (0–63) for the i64 width (x86-64 hardware semantics). Negative
        //      or oversized counts are masked deterministically, UB-free, in both
        //      the interpreter and the compiler. Single shared constant so the two
        //      engines cannot drift (Amelia review T-1).
        // ════════════════════════════════════════════════════════════════════
        inline constexpr int64_t kBitwiseShiftCountMask = 63;

    } // namespace Builtins
} // namespace Sad
