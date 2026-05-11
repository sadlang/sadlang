// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: tools/sadinfo/include/builtin_data.h
// (AR) أنواع بيانات الدوال المدمجة (~21 دالة auto-loaded) والطرق المضمنة على
//      الأنواع (مصفوفات، نصوص، خرائط، قنوات). مصدر حقيقة ثابت داخل sadinfo
//      مستقل عن المفسر — مأخوذ من السجل الموحَّد shared/builtins ومن
//      .github/copilot-instructions.md (قسم "الدوال المضمنة تلقائياً").
//
// (EN) Data types for built-in functions (~21 auto-loaded) and built-in methods
//      on types (arrays, strings, maps, channels). A stable source of truth
//      inside sadinfo, independent from the interpreter — sourced from
//      shared/builtins registry and the project copilot-instructions.
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include <string_view>
#include <vector>

namespace Sad
{
    namespace Info
    {

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) فئة الدالة المدمجة — للعرض والتصفية
        // (EN) Built-in function category — for display and filtering
        // ───────────────────────────────────────────────────────────────────────────────
        enum class BuiltinCategory : int
        {
            Printing = 0,      // (AR) إخراج: اطبع، اطبع_سطر
            Input = 1,         // (AR) إدخال: اقرأ
            Reflection = 2,    // (AR) طول، نوع
            TypeCtor = 3,      // (AR) رقم، عشري، نص، منطقي
            Concurrency = 4,   // (AR) قناة، انتظر_الكل، عدد_المهام، مجموعة_انتظار، قفل، مستقبل
        };

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) معامل دالة — اسم + نوع + اختياري؟
        // (EN) Function parameter — name + type + optional flag
        // ───────────────────────────────────────────────────────────────────────────────
        struct BuiltinParam
        {
            std::string_view nameAr;   ///< (AR) اسم المعامل بالعربية
            std::string_view nameEn;   ///< (EN) اسم المعامل بالإنجليزية
            std::string_view typeAr;   ///< (AR) النوع: "نص"، "رقم"، "أي"، ...
            bool optional = false;     ///< (AR) هل المعامل اختياري؟
            bool variadic = false;     ///< (AR) ...رسائل
        };

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) إدخال دالة مدمجة (auto-loaded — بدون استيراد)
        // (EN) Built-in function entry (auto-loaded — no import needed)
        // ───────────────────────────────────────────────────────────────────────────────
        struct BuiltinFunction
        {
            std::string_view id;        ///< (AR) معرّف ASCII مستقر (e.g. "print", "to_int")
            std::string_view nameAr;    ///< (AR) الاسم العربي القانوني
            std::string_view nameEn;    ///< (EN) المقابل الإنجليزي
            BuiltinCategory category;   ///< (AR) الفئة
            std::string_view returnType;///< (AR) نوع القيمة المُرجَعة
            std::string_view summaryAr; ///< (AR) شرح موجز بالعربية
            std::string_view summaryEn; ///< (EN) Short English summary
            std::vector<BuiltinParam> params; ///< (AR) المعاملات بالترتيب
            std::string_view example;   ///< (AR) مثال موجز بالعربية (سطر واحد)
        };

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) النوع المُلحَق بطريقة مضمنة
        // (EN) Type that owns a built-in method
        // ───────────────────────────────────────────────────────────────────────────────
        enum class BuiltinHostType : int
        {
            Array = 0,    // (AR) مصفوفة
            String = 1,   // (AR) نص
            Map = 2,      // (AR) خريطة
            Channel = 3,  // (AR) قناة
        };

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) إدخال طريقة مضمنة على نوع
        // (EN) Built-in method entry on a host type
        // ───────────────────────────────────────────────────────────────────────────────
        struct BuiltinMethod
        {
            std::string_view id;        ///< (AR) معرّف مستقر يجمع النوع+الاسم
            BuiltinHostType host;       ///< (AR) النوع المُضيف
            std::string_view nameAr;    ///< (AR) اسم الطريقة بالعربية (e.g. "اضف")
            std::string_view nameEn;    ///< (EN) المقابل الإنجليزي (e.g. "push")
            std::string_view returnType;///< (AR) نوع القيمة المُرجَعة
            std::string_view summaryAr; ///< (AR) شرح موجز
            std::string_view summaryEn; ///< (EN) Summary
            std::vector<BuiltinParam> params;
        };

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) واجهات الوصول — مُعرَّفة في builtin_data.cpp
        // (EN) Accessor APIs — defined in builtin_data.cpp
        // ───────────────────────────────────────────────────────────────────────────────

        /**
         * @brief (AR) ترجع جميع الدوال المدمجة auto-loaded (~21)
         * @brief (EN) Returns all auto-loaded built-in functions (~21)
         */
        const std::vector<BuiltinFunction>& allBuiltinFunctions();

        /**
         * @brief (AR) ترجع جميع الطرق المضمنة على الأنواع
         * @brief (EN) Returns all built-in methods on host types
         */
        const std::vector<BuiltinMethod>& allBuiltinMethods();

        /**
         * @brief (AR) اسم الفئة كنص ASCII مستقر للـ JSON
         * @brief (EN) Stable ASCII category name for JSON
         */
        std::string_view categoryName(BuiltinCategory c);

        /**
         * @brief (AR) اسم النوع المُضيف كنص ASCII مستقر
         * @brief (EN) Stable ASCII host-type name
         */
        std::string_view hostTypeName(BuiltinHostType h);

    } // namespace Info
} // namespace Sad
