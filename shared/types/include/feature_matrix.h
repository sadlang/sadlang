/**
 * @file feature_matrix.h
 * @brief (AR) مصفوفة الميزات — جدول ساكن يوضح حالة كل ميزة في المفسر والمترجم
 * @brief (EN) Feature Matrix — static table showing feature status in interpreter and compiler
 *
 * ═══════════════════════════════════════════════════════════════════════════════════
 *
 * (AR) هذا الملف هو المصدر الوحيد لمعرفة ما يدعمه المفسر والمترجم.
 *      يُستخدم من:
 *        • المترجم (sadc) — لرفض أو تحذير عند ميزات غير مكتملة
 *        • المفسر (sad) — للتحقق من قدراته
 *        • اختبارات dual_execution — لتخطي اختبارات الميزات غير المدعومة
 *        • أدوات CLI — لعرض جدول الميزات للمستخدم
 *
 *      المبدأ الأساسي (ADR-03):
 *        المترجم يجب أن يكون بديلاً كاملاً للمفسر.
 *        أي ميزة تعمل في المفسر يجب أن تعمل في المترجم بنفس السلوك.
 *        هدف: 100% توافقية.
 *
 *      كيفية التحديث:
 *        عند إضافة دعم ميزة في المترجم، عدّل حالتها من PARTIAL/UNSUPPORTED إلى FULL.
 *        عند إضافة ميزة جديدة للغة، أضف مُدخلاً جديداً في FEATURE_TABLE.
 *
 * (EN) Single source of truth for interpreter/compiler feature parity.
 *      Used by sadc (reject/warn), sad (capability check), dual_execution tests
 *      (skip unsupported), and CLI tools (display feature table).
 *
 * ═══════════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_FEATURE_MATRIX_H
#define SAD_FEATURE_MATRIX_H

#include <array>
#include <cstdint>
#include <string_view>

namespace Sad
{
    namespace Features
    {

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  الجزء ①: التعدادات — فئة الميزة وحالة الدعم
        //  Part ①: Enums — Feature category and support level
        //
        // █████████████████████████████████████████████████████████████████████████████████

        /**
         * @brief (AR) فئة الميزة — لتصنيف وتجميع الميزات
         * @brief (EN) Feature category — for classification and grouping
         */
        enum class FeatureCategory : uint8_t
        {
            Core,            // (AR) أساسيات: متغيرات، ثوابت، عوامل، تعبيرات
            ControlFlow,     // (AR) تحكم: إذا/وإلا، بينما، لكل، طابق
            Functions,       // (AR) دوال: تعريف، استدعاء، تكرار، إغلاق
            OOP,             // (AR) كائنية: أصناف، وراثة، سمات، تعداد
            DataTypes,       // (AR) أنواع بيانات: مصفوفات، خرائط، صفوف
            ErrorHandling,   // (AR) معالجة أخطاء: حاول/امسك/ارمي
            Modules,         // (AR) وحدات: استورد/صدّر
            Async,           // (AR) غير متزامن: غير_متزامن/انتظر، مولّدات
            Concurrency,     // (AR) تزامن: أطلق، قنوات، أقفال
            Advanced,        // (AR) متقدم: ماكروز، امتداد، عقود
            Memory,          // (AR) ذاكرة: ملكية، استعارة، أعمار
            Metaprogramming, // (AR) ميتا: توجيهات @، تجميع مضمّن
        };

        /**
         * @brief (AR) حالة دعم الميزة في مكون معين
         * @brief (EN) Feature support level in a specific component
         */
        enum class SupportLevel : uint8_t
        {
            Full,          // (AR) دعم كامل — يعمل بنفس سلوك المفسر
            Partial,       // (AR) دعم جزئي — بعض الحالات تعمل والبعض لا
            Unsupported,   // (AR) غير مدعوم — لا يعمل على الإطلاق
            NotApplicable, // (AR) لا ينطبق — الميزة لا تنطبق على هذا المكون
        };

        /**
         * @brief (AR) معرّف الميزة — لكل ميزة لغوية إسم فريد
         * @brief (EN) Feature identifier — unique name for each language feature
         *
         * (AR) الترتيب يتبع الفئات: Core → ControlFlow → ... → Metaprogramming
         *      عند إضافة ميزة جديدة: أضفها في نهاية فئتها + حدّث FEATURE_COUNT + FEATURE_TABLE
         */
        enum class Feature : uint16_t
        {
            // ─── Core (أساسيات) ───
            Variables = 0,       // متغير/ثابت
            Operators,           // عوامل حسابية ومنطقية ومقارنة
            TypeConversion,      // تحويل أنواع: رقم()، نص()، عشري()
            StringInterpolation, // نصوص التنسيق (f-strings)
            TernaryOperator,     // عامل ثلاثي ? :
            NullCoalescing,      // عامل ?? (الاندماج الفارغ)

            // ─── ControlFlow (تحكم) ───
            IfElse,          // إذا/وإلا
            WhileLoop,       // بينما
            ForEach,         // لكل ... في
            PatternMatching, // طابق/عندما
            RangePatterns,   // نطاق 1..10 في طابق
            BreakContinue,   // توقف/استمر

            // ─── Functions (دوال) ───
            FunctionDef,          // دالة
            Recursion,            // تكرار (استدعاء ذاتي)
            Closures,             // إغلاقات (lambdas)
            DefaultParams,        // معاملات افتراضية
            VariadicParams,       // معاملات متغيرة العدد (...)
            HigherOrderFunctions, // دوال عليا (map/filter/reduce)

            // ─── OOP (كائنية) ───
            Classes,             // صنف
            Inheritance,         // يرث
            Constructors,        // باني
            Destructors,         // هدم
            AccessModifiers,     // عام/خاص/محمي
            Traits,              // سمة/نفّذ
            OperatorOverloading, // عامل (تحميل عامل زائد)
            Properties,          // خاصية/احصل/عيّن
            AbstractClasses,     // مجرد

            // ─── DataTypes (أنواع بيانات) ───
            Arrays,        // مصفوفات
            Maps,          // خرائط
            Tuples,        // صفوف
            Structs,       // بنية
            Enums,         // تعداد
            SealedClasses, // أصناف مختومة

            // ─── ErrorHandling (معالجة أخطاء) ───
            TryCatch,        // حاول/امسك/أخيراً
            ThrowExceptions, // ارمي
            Contracts,       // يتطلب/يضمن (عقود برمجية)

            // ─── Modules (وحدات) ───
            Import,         // استورد/من
            Export,         // صدّر
            PackageManager, // مدير الحزم (أداة خارجية)

            // ─── Async (غير متزامن) ───
            AsyncAwait, // غير_متزامن/انتظر
            Generators, // مولّد/أنتج

            // ─── Concurrency (تزامن) ───
            Goroutines, // أطلق
            Channels,   // قناة/أرسل/استقبل
            Select,     // اختر
            WaitGroup,  // مجموعة_انتظار
            Mutex,      // قفل
            Future,     // مستقبل

            // ─── Advanced (متقدم) ───
            Macros,           // ماكرو
            ExtensionMethods, // امتداد
            SmartContracts,   // عقد (ذكي)
            PropertyTesting,  // اختبر + تكرارات + بذرة
            Defer,            // أجّل
            TypeAliases,      // مستعار نوع

            // ─── Memory (ذاكرة) ───
            Ownership,    // نظام الملكية (move/borrow)
            Lifetimes,    // أعمار <'أ>
            UnsafeBlocks, // @غير_آمن

            // ─── Metaprogramming (ميتا) ───
            CompileTimeExec, // @وقت_الترجمة
            InlineAssembly,  // @تجميع
            Directives,      // @حجم، @ذري، @متطاير

            // ─── الحارس — يجب أن يكون آخر عنصر ───
            _Count
        };

        /** @brief (AR) عدد الميزات الإجمالي */
        static constexpr size_t FEATURE_COUNT = static_cast<size_t>(Feature::_Count);

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  الجزء ②: FeatureInfo — معلومات ميزة واحدة
        //  Part ②: FeatureInfo — single feature info
        //
        // █████████████████████████████████████████████████████████████████████████████████

        /**
         * @brief (AR) معلومات ميزة واحدة: الاسم، الفئة، حالة الدعم في المفسر والمترجم
         * @brief (EN) Info for a single feature: name, category, support level in interpreter and compiler
         */
        struct FeatureInfo
        {
            Feature id;                   // (AR) معرّف الميزة
            std::string_view arabicName;  // (AR) الاسم بالعربية
            std::string_view englishName; // (EN) الاسم بالإنجليزية
            FeatureCategory category;     // (AR) الفئة
            SupportLevel interpreter;     // (AR) حالة الدعم في المفسر
            SupportLevel compiler;        // (AR) حالة الدعم في المترجم
            std::string_view notes;       // (AR) ملاحظات (سبب عدم الدعم مثلاً)
        };

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  الجزء ③: FEATURE_TABLE — الجدول الرئيسي (constexpr)
        //  Part ③: FEATURE_TABLE — main table (constexpr)
        //
        //  (AR) ⚠️ عند تحديث حالة ميزة: عدّل SupportLevel هنا فقط
        //       الكود يقرأ هذا الجدول في وقت الترجمة — لا تكلفة وقت تشغيل
        //
        // █████████████████████████████████████████████████████████████████████████████████

        // (AR) اختصارات للوضوح في الجدول
        using SL = SupportLevel;
        using FC = FeatureCategory;
        using F = Feature;

        // clang-format off
        static constexpr std::array<FeatureInfo, FEATURE_COUNT> FEATURE_TABLE = {{
            // ═══ Core (أساسيات) ═══
            {F::Variables,            "متغيرات وثوابت",         "Variables & Constants",     FC::Core,         SL::Full,        SL::Full,        ""},
            {F::Operators,            "عوامل",                  "Operators",                 FC::Core,         SL::Full,        SL::Full,        ""},
            {F::TypeConversion,       "تحويل أنواع",            "Type Conversion",           FC::Core,         SL::Full,        SL::Full,        ""},
            {F::StringInterpolation,  "نصوص التنسيق",           "String Interpolation",      FC::Core,         SL::Full,        SL::Full,        ""},
            {F::TernaryOperator,      "عامل ثلاثي",             "Ternary Operator",          FC::Core,         SL::Full,        SL::Full,        ""},
            {F::NullCoalescing,       "اندماج فارغ",            "Null Coalescing",           FC::Core,         SL::Full,        SL::Full,        ""},

            // ═══ ControlFlow (تحكم) ═══
            {F::IfElse,               "إذا/وإلا",               "If/Else",                   FC::ControlFlow,  SL::Full,        SL::Full,        ""},
            {F::WhileLoop,            "حلقة بينما",             "While Loop",                FC::ControlFlow,  SL::Full,        SL::Full,        ""},
            {F::ForEach,              "لكل",                    "For Each",                  FC::ControlFlow,  SL::Full,        SL::Full,        ""},
            {F::PatternMatching,      "مطابقة أنماط",           "Pattern Matching",          FC::ControlFlow,  SL::Full,        SL::Full,        ""},
            {F::RangePatterns,        "أنماط نطاق",             "Range Patterns",            FC::ControlFlow,  SL::Full,        SL::Full,        ""},
            {F::BreakContinue,        "توقف/استمر",             "Break/Continue",            FC::ControlFlow,  SL::Full,        SL::Full,        ""},

            // ═══ Functions (دوال) ═══
            {F::FunctionDef,          "تعريف دوال",             "Function Definitions",      FC::Functions,    SL::Full,        SL::Full,        ""},
            {F::Recursion,            "تكرار ذاتي",             "Recursion",                 FC::Functions,    SL::Full,        SL::Full,        ""},
            {F::Closures,             "إغلاقات",                "Closures/Lambdas",          FC::Functions,    SL::Full,        SL::Full,        ""},
            {F::DefaultParams,        "معاملات افتراضية",       "Default Parameters",        FC::Functions,    SL::Full,        SL::Full,        ""},
            {F::VariadicParams,       "معاملات متغيرة",         "Variadic Parameters",       FC::Functions,    SL::Full,        SL::Partial,     "تعمل في الماكروز فقط"},
            {F::HigherOrderFunctions, "دوال عليا",              "Higher-Order Functions",    FC::Functions,    SL::Full,        SL::Full,        ""},

            // ═══ OOP (كائنية) ═══
            {F::Classes,              "أصناف",                  "Classes",                   FC::OOP,          SL::Full,        SL::Full,        ""},
            {F::Inheritance,          "وراثة",                  "Inheritance",               FC::OOP,          SL::Full,        SL::Full,        ""},
            {F::Constructors,         "باني",                   "Constructors",              FC::OOP,          SL::Full,        SL::Full,        ""},
            {F::Destructors,          "هدم",                    "Destructors",               FC::OOP,          SL::Full,        SL::Partial,     "تحتاج ربط مع RAII"},
            {F::AccessModifiers,      "محددات الوصول",          "Access Modifiers",          FC::OOP,          SL::Full,        SL::Full,        ""},
            {F::Traits,               "سمات",                   "Traits",                    FC::OOP,          SL::Full,        SL::Full,        ""},
            {F::OperatorOverloading,   "تحميل عوامل",           "Operator Overloading",      FC::OOP,          SL::Full,        SL::Full,        ""},
            {F::Properties,           "خاصيات",                 "Properties",                FC::OOP,          SL::Full,        SL::Partial,     "getter يعمل، setter جزئي"},
            {F::AbstractClasses,      "أصناف مجردة",            "Abstract Classes",          FC::OOP,          SL::Full,        SL::Full,        ""},

            // ═══ DataTypes (أنواع بيانات) ═══
            {F::Arrays,               "مصفوفات",                "Arrays",                    FC::DataTypes,    SL::Full,        SL::Full,        ""},
            {F::Maps,                 "خرائط",                  "Maps",                      FC::DataTypes,    SL::Full,        SL::Full,        ""},
            {F::Tuples,               "صفوف",                   "Tuples",                    FC::DataTypes,    SL::Full,        SL::Full,        ""},
            {F::Structs,              "بنيات",                  "Structs",                   FC::DataTypes,    SL::Full,        SL::Full,        ""},
            {F::Enums,                "تعدادات",                "Enums",                     FC::DataTypes,    SL::Full,        SL::Full,        ""},
            {F::SealedClasses,        "أصناف مختومة",           "Sealed Classes",            FC::DataTypes,    SL::Full,        SL::Partial,     "يحتاج فحص شمولية"},

            // ═══ ErrorHandling (معالجة أخطاء) ═══
            {F::TryCatch,             "حاول/امسك",              "Try/Catch",                 FC::ErrorHandling, SL::Full,       SL::Full,        ""},
            {F::ThrowExceptions,      "ارمي",                   "Throw Exceptions",          FC::ErrorHandling, SL::Full,       SL::Full,        ""},
            {F::Contracts,            "عقود برمجية",            "Contracts (requires/ensures)", FC::ErrorHandling, SL::Full,    SL::Partial,     "يتطلب يعمل، يضمن جزئي"},

            // ═══ Modules (وحدات) ═══
            {F::Import,               "استورد",                 "Import",                    FC::Modules,      SL::Full,        SL::Full,        ""},
            {F::Export,               "صدّر",                   "Export",                    FC::Modules,      SL::Full,        SL::Full,        ""},
            {F::PackageManager,       "مدير حزم",               "Package Manager",           FC::Modules,      SL::Full,        SL::NotApplicable, "أداة خارجية (sad pkg)"},

            // ═══ Async (غير متزامن) ═══
            {F::AsyncAwait,           "غير متزامن/انتظر",       "Async/Await",               FC::Async,        SL::Full,        SL::Partial,     "البنية الأساسية موجودة، التنفيذ 50%"},
            {F::Generators,           "مولّدات",                "Generators",                FC::Async,        SL::Full,        SL::Partial,     "البنية الأساسية موجودة، التنفيذ 50%"},

            // ═══ Concurrency (تزامن) ═══
            {F::Goroutines,           "خيوط خفيفة",             "Goroutines",                FC::Concurrency,  SL::Full,        SL::Partial,     "SIR ASYNC_SPAWN موجود، codegen جزئي"},
            {F::Channels,             "قنوات",                  "Channels",                  FC::Concurrency,  SL::Full,        SL::Partial,     "بنية SIR موجودة، ربط runtime جزئي"},
            {F::Select,               "اختر",                   "Select",                    FC::Concurrency,  SL::Full,        SL::Unsupported, "يحتاج تنفيذ في المترجم"},
            {F::WaitGroup,            "مجموعة_انتظار",          "WaitGroup",                 FC::Concurrency,  SL::Full,        SL::Partial,     "builtin مربوط جزئياً"},
            {F::Mutex,                "قفل",                    "Mutex",                     FC::Concurrency,  SL::Full,        SL::Partial,     "builtin مربوط جزئياً"},
            {F::Future,               "مستقبل",                 "Future",                    FC::Concurrency,  SL::Full,        SL::Partial,     "SadFutureType موجود، codegen جزئي"},

            // ═══ Advanced (متقدم) ═══
            {F::Macros,               "ماكروز",                 "Macros",                    FC::Advanced,     SL::Full,        SL::Partial,     "التوسعة الأساسية تعمل، hygiene 50%"},
            {F::ExtensionMethods,     "طرق امتداد",             "Extension Methods",         FC::Advanced,     SL::Full,        SL::Unsupported, "يحتاج تنفيذ في SIR Builder"},
            {F::SmartContracts,       "عقود ذكية",              "Smart Contracts",           FC::Advanced,     SL::Full,        SL::Unsupported, "يحتاج تنفيذ في المترجم"},
            {F::PropertyTesting,      "اختبار خصائص",           "Property Testing",          FC::Advanced,     SL::Full,        SL::Unsupported, "يحتاج تنفيذ في المترجم"},
            {F::Defer,                "تأجيل",                  "Defer",                     FC::Advanced,     SL::Full,        SL::Full,        ""},
            {F::TypeAliases,          "مستعارات أنواع",         "Type Aliases",              FC::Advanced,     SL::Full,        SL::Partial,     "SadTypeAlias موجود، ربط جزئي"},

            // ═══ Memory (ذاكرة) ═══
            {F::Ownership,            "نظام ملكية",             "Ownership System",          FC::Memory,       SL::Partial,     SL::Full,        "12 عملية SIR مُنفّذة بالكامل"},
            {F::Lifetimes,            "أعمار المراجع",          "Lifetimes",                 FC::Memory,       SL::Partial,     SL::Partial,     "التحقق الأساسي موجود"},
            {F::UnsafeBlocks,         "كتل غير آمنة",           "Unsafe Blocks",             FC::Memory,       SL::Full,        SL::Full,        ""},

            // ═══ Metaprogramming (ميتا) ═══
            {F::CompileTimeExec,      "تنفيذ وقت الترجمة",      "Compile-Time Execution",    FC::Metaprogramming, SL::NotApplicable, SL::Partial, "بنية أساسية موجودة"},
            {F::InlineAssembly,       "تجميع مضمن",             "Inline Assembly",           FC::Metaprogramming, SL::NotApplicable, SL::Full,    "sadc فقط"},
            {F::Directives,           "توجيهات @",              "Directives (@)",            FC::Metaprogramming, SL::Partial,  SL::Full,        "@حجم، @ذري، @متطاير"},
        }};
        // clang-format on

        // █████████████████████████████████████████████████████████████████████████████████
        //
        //  الجزء ④: دوال الاستعلام — constexpr للاستخدام في وقت الترجمة ووقت التشغيل
        //  Part ④: Query functions — constexpr for compile-time and runtime use
        //
        // █████████████████████████████████████████████████████████████████████████████████

        /**
         * @brief (AR) الحصول على معلومات ميزة معينة
         * @brief (EN) Get info for a specific feature
         */
        inline constexpr const FeatureInfo &getFeatureInfo(Feature f)
        {
            return FEATURE_TABLE[static_cast<size_t>(f)];
        }

        /**
         * @brief (AR) هل المترجم يدعم ميزة معينة بالكامل؟
         * @brief (EN) Does the compiler fully support a given feature?
         */
        inline constexpr bool compilerSupports(Feature f)
        {
            return FEATURE_TABLE[static_cast<size_t>(f)].compiler == SL::Full;
        }

        /**
         * @brief (AR) هل المفسر يدعم ميزة معينة بالكامل؟
         * @brief (EN) Does the interpreter fully support a given feature?
         */
        inline constexpr bool interpreterSupports(Feature f)
        {
            return FEATURE_TABLE[static_cast<size_t>(f)].interpreter == SL::Full;
        }

        /**
         * @brief (AR) هل الميزة مدعومة بالكامل في كلا المكونين؟
         * @brief (EN) Is the feature fully supported in both components?
         */
        inline constexpr bool fullySupported(Feature f)
        {
            const auto &info = FEATURE_TABLE[static_cast<size_t>(f)];
            return (info.interpreter == SL::Full || info.interpreter == SL::NotApplicable) &&
                   (info.compiler == SL::Full || info.compiler == SL::NotApplicable);
        }

        /**
         * @brief (AR) حساب نسبة التكافؤ بين المفسر والمترجم (وقت تشغيل فقط)
         * @brief (EN) Calculate interpreter/compiler feature parity percentage (runtime only)
         *
         * @return نسبة مئوية 0-100
         */
        inline double calculateParity()
        {
            size_t applicable = 0;
            size_t matching = 0;
            for (const auto &f : FEATURE_TABLE)
            {
                // (AR) تخطي الميزات التي لا تنطبق على أحد المكونين
                if (f.interpreter == SL::NotApplicable || f.compiler == SL::NotApplicable)
                    continue;
                applicable++;
                if (f.interpreter == f.compiler)
                    matching++;
            }
            return applicable > 0 ? (static_cast<double>(matching) / applicable) * 100.0 : 0.0;
        }

        /**
         * @brief (AR) عدد الميزات الكاملة في المترجم
         * @brief (EN) Count of features fully supported in the compiler
         */
        inline constexpr size_t countCompilerFull()
        {
            size_t count = 0;
            for (size_t i = 0; i < FEATURE_COUNT; ++i)
            {
                if (FEATURE_TABLE[i].compiler == SL::Full)
                    ++count;
            }
            return count;
        }

        /**
         * @brief (AR) عدد الميزات الكاملة في المفسر
         * @brief (EN) Count of features fully supported in the interpreter
         */
        inline constexpr size_t countInterpreterFull()
        {
            size_t count = 0;
            for (size_t i = 0; i < FEATURE_COUNT; ++i)
            {
                if (FEATURE_TABLE[i].interpreter == SL::Full)
                    ++count;
            }
            return count;
        }

        /**
         * @brief (AR) الحصول على اسم الفئة بالعربية
         * @brief (EN) Get Arabic name for a category
         */
        inline constexpr std::string_view categoryToArabic(FeatureCategory cat)
        {
            switch (cat)
            {
            case FC::Core:
                return "أساسيات";
            case FC::ControlFlow:
                return "تحكم";
            case FC::Functions:
                return "دوال";
            case FC::OOP:
                return "كائنية";
            case FC::DataTypes:
                return "أنواع بيانات";
            case FC::ErrorHandling:
                return "معالجة أخطاء";
            case FC::Modules:
                return "وحدات";
            case FC::Async:
                return "غير متزامن";
            case FC::Concurrency:
                return "تزامن";
            case FC::Advanced:
                return "متقدم";
            case FC::Memory:
                return "ذاكرة";
            case FC::Metaprogramming:
                return "ميتا";
            }
            return "مجهول";
        }

        /**
         * @brief (AR) الحصول على اسم حالة الدعم بالعربية
         * @brief (EN) Get Arabic name for support level
         */
        inline constexpr std::string_view supportLevelToArabic(SupportLevel sl)
        {
            switch (sl)
            {
            case SL::Full:
                return "كامل";
            case SL::Partial:
                return "جزئي";
            case SL::Unsupported:
                return "غير مدعوم";
            case SL::NotApplicable:
                return "لا ينطبق";
            }
            return "مجهول";
        }

        /**
         * @brief (AR) الحصول على اسم حالة الدعم بالإنجليزية
         * @brief (EN) Get English name for support level
         */
        inline constexpr std::string_view supportLevelToEnglish(SupportLevel sl)
        {
            switch (sl)
            {
            case SL::Full:
                return "Full";
            case SL::Partial:
                return "Partial";
            case SL::Unsupported:
                return "Unsupported";
            case SL::NotApplicable:
                return "N/A";
            }
            return "Unknown";
        }

    } // namespace Features
} // namespace Sad

#endif // SAD_FEATURE_MATRIX_H
