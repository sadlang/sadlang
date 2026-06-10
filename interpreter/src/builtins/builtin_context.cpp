/**
 * @file builtin_context.cpp
 * @brief (AR) تنفيذ BuiltinContext::error — يطلق خطأً من الكتالوج بموقع الاستدعاء.
 * @brief (EN) BuiltinContext::error — raises a catalog error at the call site.
 * @see ../../include/builtins/builtin_context.h
 */
#include "builtins/builtin_context.h"
#include "runtime_throw.h" // Sad::Errors::throwRuntime

namespace Sad
{
    namespace Interpreter
    {

        void BuiltinContext::error(Sad::Errors::ErrorCode code,
                                   std::map<std::string, std::string> placeholders) const
        {
            // (AR) حقن اسم الدالة افتراضياً (placeholder "func" شائع في رسائل الأخطاء).
            // (EN) Inject the function name by default ("func" is a common placeholder).
            if (placeholders.find("func") == placeholders.end())
            {
                placeholders.emplace("func", std::string(name_));
            }
            // (AR) المسار الموحَّد: يبلّغ الكتالوج بالموقع ثم يرمي RuntimeAbort.
            // (EN) Unified path: report from catalog at position, then abort.
            Sad::Errors::throwRuntime(code, pos_, std::move(placeholders));
        }

    } // namespace Interpreter
} // namespace Sad
