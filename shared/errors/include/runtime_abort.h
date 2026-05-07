/**
 * @file runtime_abort.h
 * @brief (AR) إشارة لف المكدس عند خطأ وقت تشغيل في المفسر — لا تحمل
 *             أي بيانات خطأ. كل بيانات الخطأ تُسجَّل مباشرة في
 *             ErrorManager قبل الرمي. هذا الاستثناء وظيفته الوحيدة:
 *             إنهاء التنفيذ والوصول لمعالج الالتقاط النهائي حيث
 *             يُستدعى ErrorManager.flush().
 *
 * @brief (EN) Stack-unwind signal raised on a runtime error in the
 *             interpreter — carries NO error data. All error data is
 *             reported directly to ErrorManager BEFORE throwing. The sole
 *             purpose of this exception is to abort execution and reach
 *             the top-level catch site where ErrorManager.flush() runs.
 *
 * (AR) قاعدة معمارية صارمة (Phase 4):
 *      ❌ لا تضع رسائل خطأ هنا
 *      ❌ لا تضع ErrorCode هنا
 *      ❌ لا تضع موقع هنا
 *      ✅ سجّل الخطأ مباشرة في ErrorManager قبل رمي RuntimeAbort
 *
 * (EN) Strict architectural rule (Phase 4):
 *      ❌ Do NOT put error messages here
 *      ❌ Do NOT put ErrorCode here
 *      ❌ Do NOT put source location here
 *      ✅ Report the error directly to ErrorManager BEFORE throwing RuntimeAbort
 *
 * Usage / الاستخدام:
 * @code
 * auto& EM = Sad::Errors::ErrorManager::getInstance();
 * Sad::Errors::SourceLocation loc(file, pos.line, pos.column);
 * EM.reportFromCatalog(Sad::Errors::ErrorCode::RUN_DIVISION_BY_ZERO, loc);
 * throw Sad::Errors::RuntimeAbort{};
 * @endcode
 */

#pragma once

#include <exception>

namespace Sad
{
    namespace Errors
    {
        /**
         * @class RuntimeAbort
         * @brief (AR) إشارة فارغة لإنهاء تنفيذ المفسر بعد تسجيل الخطأ
         * @brief (EN) Empty signal to abort interpreter execution after error reported
         */
        class RuntimeAbort : public std::exception
        {
        public:
            const char *what() const noexcept override
            {
                return "Sad runtime aborted (see ErrorManager for details)";
            }
        };

    } // namespace Errors
} // namespace Sad
