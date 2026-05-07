/**
 * @file runtime_throw.h
 * @brief (AR) واجهات مساعدة موحَّدة لرفع أخطاء وقت التشغيل من المفسر.
 *             كل throw سابق لفئات الاستثناء القديمة (RuntimeError،
 *             DivisionByZeroError، TypeError، ...) يستخدم بدلاً منها هذه
 *             الدوال — التي تستدعي ErrorManager مباشرة ثم تُطلق
 *             RuntimeAbort لإنهاء التنفيذ.
 *
 * @brief (EN) Unified helpers for raising runtime errors from the
 *             interpreter. Every legacy throw of RuntimeError /
 *             DivisionByZeroError / TypeError / ... is replaced by these
 *             helpers — which call ErrorManager directly and then raise
 *             RuntimeAbort to unwind execution.
 *
 * (AR) مبدأ صارم (Phase 4):
 *      • لا توجد فئة استثناء تحمل نص الخطأ.
 *      • نص الخطأ يُسجَّل مباشرة في ErrorManager.
 *      • RuntimeAbort فارغة تمامًا.
 *
 * (EN) Strict principle (Phase 4):
 *      • No exception class carries the error text.
 *      • The error text is reported directly into ErrorManager.
 *      • RuntimeAbort is completely empty.
 */

#pragma once

#include <string>
#include <map>

#include "error_codes.h"
#include "error_catalog.h" // RenderContext + std::map
#include "error_manager.h"
#include "runtime_abort.h"
#include "source_location.h"
#include "token.h" // Sad::Lexer::Position

namespace Sad
{
    namespace Errors
    {
        /**
         * @brief (AR) يحوّل Sad::Lexer::Position إلى SourceLocation للسجل
         * @brief (EN) Converts a Sad::Lexer::Position to a SourceLocation
         */
        inline SourceLocation toLocation(const Sad::Lexer::Position &pos)
        {
            // (AR) اسم الملف يأتي عادة من ErrorManager.sourceFile_
            // (EN) Filename is normally taken from ErrorManager.sourceFile_
            const std::string &file = ErrorManager::getInstance().getSourceFilename();
            return SourceLocation(file, pos.line, pos.column,
                                  pos.offset, pos.length);
        }

        /**
         * @brief (AR) يسجّل خطأ وقت التشغيل في ErrorManager ثم يُطلق RuntimeAbort
         * @brief (EN) Reports a runtime error to ErrorManager then raises RuntimeAbort
         *
         * @param code (AR) رمز الخطأ من ErrorCatalog / (EN) error code from ErrorCatalog
         * @param pos (AR) موقع الخطأ في الكود المصدري / (EN) source location
         * @param placeholders (AR) متغيرات القالب (msg, name, ...) / (EN) template placeholders
         */
        [[noreturn]] inline void throwRuntime(
            ErrorCode code,
            const Sad::Lexer::Position &pos,
            std::map<std::string, std::string> placeholders = {})
        {
            auto &EM = ErrorManager::getInstance();
            RenderContext ctx;
            ctx.placeholders = std::move(placeholders);
            EM.reportFromCatalog(code, toLocation(pos), ctx);
            throw RuntimeAbort{};
        }

        // (AR) ملاحظة صارمة: لا توجد نسخة تأخذ "رسالة نصية" كنص حر.
        //      المفسر يجب أن يُمرّر دائمًا (ErrorCode + placeholders) فقط.
        //      نص الرسالة الكامل يعيش في ErrorCatalog وحده.
        // (EN) Strict note: there is NO overload that accepts a raw message string.
        //      The interpreter must always pass (ErrorCode + placeholders) only.
        //      Full message text lives exclusively in ErrorCatalog.

    } // namespace Errors
} // namespace Sad
