/**
 * @file stdlib_manager.cpp
 * @brief (AR) تطبيق مدير المكتبات القياسية / (EN) Implementation of Standard Library Manager
 *
 * Phase 1-8 Implementation of Standard Library:
 * - Phase 1: I/O Functions (5 functions) - IMPLEMENTED
 * - Phase 2-5: Future phases (TBD)
 * - Phase 6-8: Advanced features (TBD)
 *
 * @author S Language Development Team
 * @date November 22, 2025
 * @version 1.0
 */

#include "stdlib_manager.h"
#include "builtin_registry.h"
#include "io/io_functions.h"
#include "string/string_functions.h"
#include "array_functions.h"
#include "math/math_functions.h"
#include "type_functions.h"
#include "builtins/builtin_context.h"
#include "builtins/math_min_max_sign_aware.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <cstdint>
#include <memory>
#include <vector>

namespace Sad
{
    namespace StdLib
    {

        using namespace Data;

        // (AR) اختصارات لأسماء الدوال المركزية
        namespace Bs = Builtins::Names::Strings;
        namespace Ba = Builtins::Names::Arrays;
        namespace Bm = Builtins::Names::Math;

        // (AR) أصغر/أكبر بوعي الإشارة: وُحِّد في رأسٍ مشترك (مصدرٌ واحد؛ كان مكرَّرًا في
        //      builtin_module_strings مسارِ REPL). يُستدعى أدناه في التسجيل الواعي بالسياق.
        // (EN) Sign-aware min/max: unified into a shared header (single source; was duplicated
        //      in builtin_module_strings, the REPL path). Used in the context-aware registration below.
        using Sad::Interpreter::mathMinMaxSignAware;

        // ====================================================================
        // Constructor and Destructor
        // ====================================================================

        StandardLibraryManager::StandardLibraryManager(Data::FunctionManager &funcMgr)
            : functionManager_(funcMgr)
        {
            // Initialize tracking
            phase1_registered_ = false;
            phase2_registered_ = false;
            phase3_registered_ = false;
            phase4_registered_ = false;
            phase5_registered_ = false;
        }

        StandardLibraryManager::~StandardLibraryManager()
        {
            // Cleanup if needed
        }

        // ====================================================================
        // Phase Registration Methods
        // ====================================================================

        bool StandardLibraryManager::registerPhase1_IOFunctions()
        {
            /**
             * (AR) تسجيل دوال المرحلة 1: الإدخال والإخراج
             * (EN) Register Phase 1: Input/Output Functions
             *
             * (AR) ملاحظة: دوال الطباعة الأساسية (اطبع، اطبع_سطر، اقرأ) مُسجلة
             *      بالفعل بواسطة InterpreterCore كدوال نواة أساسية.
             *      هنا نسجل فقط الدوال التي لا يسجلها النواة.
             * (EN) Note: Core I/O functions (اطبع, اطبع_سطر, اقرأ) are already
             *      registered by InterpreterCore as core builtins.
             *      Here we only register functions not registered by the core.
             */

            if (phase1_registered_)
            {
                return true;
            }

            try
            {
                // (AR) قراءة_سطر — غير مسجلة في النواة
                registerBuiltin(
                    "قراءة_سطر",
                    "readLine",
                    [](const std::vector<Value> &args) -> Value
                    {
                        return IO::IOFunctions::readLine(args);
                    });

                // (AR) مسح_الشاشة — غير مسجلة في النواة
                registerBuiltin(
                    "مسح_الشاشة",
                    "clear",
                    [](const std::vector<Value> &args) -> Value
                    {
                        return IO::IOFunctions::clear(args);
                    });

                phase1_registered_ = true;
                return true;
            }
            catch (const std::exception &e)
            {
                std::cerr << "(AR) خطأ في تسجيل دوال المرحلة 1: "
                          << "(EN) Error registering Phase 1 functions: "
                          << e.what() << std::endl;
                return false;
            }
        }

        bool StandardLibraryManager::registerPhase2_StringFunctions()
        {
            /**
             * (AR) تسجيل دوال المرحلة 2: معالجة النصوص
             * (EN) Register Phase 2: String Functions
             */
            if (phase2_registered_)
                return true;

            try
            {
                using namespace StdLib::String;

                // (AR) أسماء موحدة مع وحدة نصوص (builtin_module_strings.cpp)
                registerBuiltin(std::string(Bs::FIND), "find", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return StringFunctions::find(args); });
                registerBuiltin(std::string(Bs::REPLACE), "replace", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return StringFunctions::replace(args); });
                registerBuiltin(std::string(Bs::SUBSTRING), "substring", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return StringFunctions::substring(args); });
                registerBuiltin(std::string(Bs::TRIM), "trim", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return StringFunctions::trim(args); });
                registerBuiltin(std::string(Bs::STARTS_WITH), "startsWith", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return StringFunctions::startsWith(args); });
                registerBuiltin(std::string(Bs::ENDS_WITH), "endsWith", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return StringFunctions::endsWith(args); });
                registerBuiltin(std::string(Bs::CONTAINS), "contains", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return StringFunctions::contains(args); });

                phase2_registered_ = true;
                return true;
            }
            catch (const std::exception &e)
            {
                std::cerr << "(AR) خطأ في تسجيل دوال النصوص: " << e.what() << std::endl;
                return false;
            }
        }

        bool StandardLibraryManager::registerPhase3_ArrayFunctions()
        {
            /**
             * (AR) تسجيل دوال المرحلة 3: معالجة المصفوفات
             * (EN) Register Phase 3: Array Functions
             */
            if (phase3_registered_)
                return true;

            try
            {
                using namespace StdLib::Core;

                // (AR) أسماء موحدة مع وحدة نصوص (builtin_module_strings.cpp)
                registerBuiltin(std::string(Ba::ADD), "array_append", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return ArrayFunctions::append(args); });
                registerBuiltin(std::string(Ba::REMOVE), "array_pop", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return ArrayFunctions::pop(args); });
                registerBuiltin(std::string(Ba::SIZE), "array_size", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return ArrayFunctions::size(args); });
                registerBuiltin(std::string(Ba::INDEX_OF), "indexOf", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return ArrayFunctions::indexOf(args); });
                registerBuiltin(std::string(Ba::REVERSE), "reverse", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return ArrayFunctions::reverse(args); });
                registerBuiltin(std::string(Ba::SORT), "sort", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return ArrayFunctions::sort(args); });
                registerBuiltin(std::string(Ba::SLICE), "slice", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return ArrayFunctions::slice(args); });

                phase3_registered_ = true;
                return true;
            }
            catch (const std::exception &e)
            {
                std::cerr << "(AR) خطأ في تسجيل دوال المصفوفات: " << e.what() << std::endl;
                return false;
            }
        }

        bool StandardLibraryManager::registerPhase4_MathFunctions()
        {
            /**
             * (AR) تسجيل دوال المرحلة 4: الرياضيات
             * (EN) Register Phase 4: Math Functions
             */
            if (phase4_registered_)
                return true;

            try
            {
                using namespace StdLib::Math;

                // (AR) أسماء موحدة مع وحدة نصوص (builtin_module_strings.cpp)
                registerBuiltin(std::string(Bm::SQRT), "sqrt", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::sqrt(args); });
                registerBuiltin(std::string(Bm::POWER), "power", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::power(args); });
                registerBuiltin(std::string(Bm::ABS), "abs", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::abs(args); });
                registerBuiltin(std::string(Bm::ROUND), "round", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::round(args); });
                registerBuiltin(std::string(Bm::FLOOR), "floor", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::floor(args); });
                registerBuiltin(std::string(Bm::CEIL), "ceil", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::ceil(args); });

                // (AR) دوال مثلثية / (EN) Trigonometric functions
                registerBuiltin(std::string(Bm::SIN), "sin", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::sin(args); });
                registerBuiltin(std::string(Bm::COS), "cos", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::cos(args); });
                registerBuiltin(std::string(Bm::TAN), "tan", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::tan(args); });
                registerBuiltin(std::string(Bm::ASIN), "asin", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::asin(args); });
                registerBuiltin(std::string(Bm::ACOS), "acos", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::acos(args); });
                registerBuiltin(std::string(Bm::ATAN), "atan", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::atan(args); });
                registerBuiltin(std::string(Bm::ATAN2), "atan2", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::atan2(args); });

                // (AR) أسماء موحدة مع وحدة رياضيات (builtin_module_math.cpp)
                registerBuiltin(std::string(Bm::LOG), "log", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::log(args); });
                registerBuiltin(std::string(Bm::LOG2), "log2", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::log2(args); });
                registerBuiltin(std::string(Bm::LOG10), "log10", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::log10(args); });
                registerBuiltin(std::string(Bm::EXP), "exp", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::exp(args); });

                // (AR) ثوابت رياضية / (EN) Math constants
                registerBuiltin(std::string(Bm::PI), "pi", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::pi(args); });
                registerBuiltin(std::string(Bm::E), "e", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::e(args); });

                // (AR) دوال عشوائية / (EN) Random functions
                registerBuiltin(std::string(Bm::RANDOM), "random", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::random(args); });
                registerBuiltin(std::string(Bm::RANDOM_INT), "randomInt", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::randomInt(args); });

                // (AR) دوال إضافية / (EN) Additional math functions
                registerBuiltin(std::string(Bm::SQUARE), "square", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return MathFunctions::square(args); });
                // (AR) أصغر/أكبر: تسجيلٌ واعٍ بالسياق (لا عبر registerBuiltin
                //   العديمِ السياق) كي نصلَ إلى ctx.argType ونقارنَ طبيعي
                //   بلا إشارةٍ مطابقةً للخلفيّةِ الأصليّة. [[التوحيد الكامل]]
                functionManager_.registerBuiltinFunction(
                    std::string(Bm::MAX),
                    [](Sad::Interpreter::BuiltinContext &ctx)
                    { return mathMinMaxSignAware(ctx, /*isMax=*/true); });
                functionManager_.registerBuiltinFunction(
                    std::string(Bm::MIN),
                    [](Sad::Interpreter::BuiltinContext &ctx)
                    { return mathMinMaxSignAware(ctx, /*isMax=*/false); });

                phase4_registered_ = true;
                return true;
            }
            catch (const std::exception &e)
            {
                std::cerr << "(AR) خطأ في تسجيل دوال الرياضيات: " << e.what() << std::endl;
                return false;
            }
        }

        bool StandardLibraryManager::registerPhase5_TypeFunctions()
        {
            /**
             * (AR) تسجيل دوال المرحلة 5: فحص وتحويل الأنواع
             * (EN) Register Phase 5: Type Functions
             */
            if (phase5_registered_)
                return true;

            try
            {
                using namespace StdLib::Core;

                // (AR) أسماء موحدة مع النواة (core builtins)
                registerBuiltin("هو_رقم_صحيح", "isInt", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return TypeFunctions::isInt(args); });
                registerBuiltin("هو_رقم_عشري", "isFloat", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return TypeFunctions::isFloat(args); });
                registerBuiltin("هو_نص", "isString", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return TypeFunctions::isString(args); });
                registerBuiltin("هو_مصفوفة", "isArray", [](const std::vector<Data::Value> &args) -> Data::Value
                                { return TypeFunctions::isArray(args); });

                phase5_registered_ = true;
                return true;
            }
            catch (const std::exception &e)
            {
                std::cerr << "(AR) خطأ في تسجيل دوال الأنواع: " << e.what() << std::endl;
                return false;
            }
        }

        // ====================================================================
        // Public Interface Methods
        // ====================================================================

        bool StandardLibraryManager::registerAllFunctions()
        {
            /**
             * (AR) تسجيل جميع دوال المكتبة المتاحة حالياً
             * (EN) Register all currently available library functions
             */

            bool allSuccess = true;

            // Register Phase 1 (I/O Functions)
            if (!registerPhase1_IOFunctions())
            {
                std::cerr << "(AR) فشل تسجيل دوال المرحلة 1 / "
                          << "(EN) Failed to register Phase 1 functions" << std::endl;
                allSuccess = false;
            }

            // Register Phase 2 (String Functions)
            if (!registerPhase2_StringFunctions())
            {
                std::cerr << "(AR) فشل تسجيل دوال المرحلة 2 / "
                          << "(EN) Failed to register Phase 2 functions" << std::endl;
                allSuccess = false;
            }

            // Register Phase 3 (Array Functions)
            if (!registerPhase3_ArrayFunctions())
            {
                std::cerr << "(AR) فشل تسجيل دوال المرحلة 3 / "
                          << "(EN) Failed to register Phase 3 functions" << std::endl;
                allSuccess = false;
            }

            // Register Phase 4 (Math Functions)
            if (!registerPhase4_MathFunctions())
            {
                std::cerr << "(AR) فشل تسجيل دوال المرحلة 4 / "
                          << "(EN) Failed to register Phase 4 functions" << std::endl;
                allSuccess = false;
            }

            // Register Phase 5 (Type Functions)
            if (!registerPhase5_TypeFunctions())
            {
                std::cerr << "(AR) فشل تسجيل دوال المرحلة 5 / "
                          << "(EN) Failed to register Phase 5 functions" << std::endl;
                allSuccess = false;
            }

            return allSuccess;
        }

        std::string StandardLibraryManager::getRegistrationStatus() const
        {
            /**
             * (AR) الحصول على حالة التسجيل الحالية
             * (EN) Get current registration status
             */

            std::ostringstream oss;

            oss << "========================================\n";
            oss << "(AR) حالة تسجيل المكتبة القياسية\n";
            oss << "(EN) Standard Library Registration Status\n";
            oss << "========================================\n\n";

            oss << "Phase 1 (I/O Functions):        " << (phase1_registered_ ? "✓ REGISTERED" : "✗ NOT REGISTERED") << "\n";
            oss << "Phase 2 (String Functions):    " << (phase2_registered_ ? "✓ REGISTERED" : "✗ NOT REGISTERED") << "\n";
            oss << "Phase 3 (Array Functions):     " << (phase3_registered_ ? "✓ REGISTERED" : "✗ NOT REGISTERED") << "\n";
            oss << "Phase 4 (Math Functions):      " << (phase4_registered_ ? "✓ REGISTERED" : "✗ NOT REGISTERED") << "\n";
            oss << "Phase 5 (Type Functions):      " << (phase5_registered_ ? "✓ REGISTERED" : "✗ NOT REGISTERED") << "\n";

            oss << "\n(AR) الملخص / (EN) Summary:\n";
            int registered = static_cast<int>(phase1_registered_) +
                             static_cast<int>(phase2_registered_) +
                             static_cast<int>(phase3_registered_) +
                             static_cast<int>(phase4_registered_) +
                             static_cast<int>(phase5_registered_);
            oss << "Phases Registered: " << registered << "/5\n";

            return oss.str();
        }

        // ====================================================================
        // Private Helper Methods
        // ====================================================================

        bool StandardLibraryManager::registerBuiltin(
            const std::string &arabicName,
            const std::string &englishName,
            std::function<Data::Value(const std::vector<Data::Value> &)> implementation)
        {

            /**
             * (AR) تسجيل دالة مضمنة جديدة باسمين (عربي وإنجليزي)
             * (EN) Register new built-in function with two names (Arabic and English)
             */

            try
            {
                // Create function that wraps the Value-based implementation
                // (AR) EM-CPP: التوقيع الموحَّد (BuiltinContext&).
                std::function<std::shared_ptr<Value>(Sad::Interpreter::BuiltinContext &)> wrapper =
                    [implementation](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Value>
                {
                    const auto &ptrArgs = ctx.args();
                    // Convert shared_ptr<Value> to Value for the implementation
                    std::vector<Value> args;
                    for (const auto &ptr : ptrArgs)
                    {
                        if (ptr)
                        {
                            args.push_back(*ptr);
                        }
                    }

                    // Call the actual implementation and return wrapped result
                    Value result = implementation(args);
                    return std::make_shared<Value>(result);
                };

                // (AR) تسجيل بالاسم العربي فقط — الأسماء الإنجليزية أُزيلت
                // (EN) Register with Arabic name only — English names removed
                functionManager_.registerBuiltinFunction(arabicName, wrapper);

                return true;
            }
            catch (const std::exception &e)
            {
                std::cerr << "(AR) خطأ في تسجيل الدالة / (EN) Error registering function: "
                          << e.what() << std::endl;
                return false;
            }
        }

    } // namespace StdLib
} // namespace Sad
