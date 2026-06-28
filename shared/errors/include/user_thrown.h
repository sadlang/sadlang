/**
 * @file user_thrown.h
 * @brief (AR) آلية رمي/امساك القيم اللغوية في "ص" (حاول/امسك/ارمي).
 *             لا علاقة لهذا بنظام الأخطاء — هذه دلالات لغوية تنقل
 *             قيم Sad::Data::Value التي يرميها المستخدم في كوده.
 *
 * @brief (EN) Language-level throw/catch mechanism for Sad
 *             (حاول/امسك/ارمي). This has nothing to do with the error
 *             system — it is purely language semantics carrying user
 *             Sad::Data::Value values thrown by user code.
 *
 * (AR) Phase 4 — التصميم الموحَّد:
 *      • أخطاء وقت التشغيل (قسمة على صفر، فهرس خارج النطاق...) →
 *        تُسجَّل في ErrorManager وتُرمى Sad::Errors::RuntimeAbort
 *      • قيم رماها المستخدم في كوده (ارمي س) →
 *        تُرمى UserThrownException وتحملها هذه الفئة فقط
 *
 * (EN) Phase 4 — unified design:
 *      • Runtime errors (div-by-zero, out-of-range, ...) →
 *        reported to ErrorManager + Sad::Errors::RuntimeAbort thrown
 *      • User-thrown values (ارمي s) →
 *        UserThrownException thrown carrying the value
 *
 * (AR) RFC sadlang-rfcs#10 (م3 خطوة 5): نُقل من interpreter/include/ إلى
 *      shared/errors/include/ — نوع استثناء مشترك يحتاجه sad_builtins، فموضعه الصحيح
 *      في الطبقة المشتركة لا داخل ترويسات المفسّر الخاصّة. (النطاق Sad::Interpreter يبقى.)
 * (EN) RFC #10 (phase-3 step-5): relocated from interpreter/include/ to
 *      shared/errors/include/ — a shared exception type needed by sad_builtins, so it
 *      belongs in the shared layer, not the interpreter-private headers. (Namespace kept.)
 */

#pragma once

#include <exception>
#include <optional>
#include <string>
#include <vector>

#include "token.h" // Sad::Lexer::Position
#include "value.h" // Sad::Data::Value

namespace Sad
{
    namespace Interpreter
    {
        /**
         * @class UserThrownException
         * @brief (AR) استثناء يحمل قيمة رماها المستخدم عبر "ارمي"
         * @brief (EN) Exception carrying a value thrown by user via "ارمي"
         */
        class UserThrownException : public std::exception
        {
        public:
            UserThrownException(const Sad::Data::Value &value,
                                std::string typeName,
                                std::string message,
                                Sad::Lexer::Position position = Sad::Lexer::Position())
                : value_(value),
                  typeName_(std::move(typeName)),
                  message_(std::move(message)),
                  position_(position)
            {
            }

            UserThrownException(std::string typeName,
                                std::string message,
                                Sad::Lexer::Position position = Sad::Lexer::Position())
                : typeName_(std::move(typeName)),
                  message_(std::move(message)),
                  position_(position)
            {
            }

            const char *what() const noexcept override { return message_.c_str(); }

            // (AR) واجهات للتوافق مع نمط حاول/امسك القديم
            // (EN) Accessors compatible with the legacy try/catch pattern
            const std::string &getMessage() const noexcept { return message_; }
            const std::string &getType() const noexcept { return typeName_; }
            const Sad::Lexer::Position &getPosition() const noexcept { return position_; }

            bool hasThrownValue() const noexcept { return value_.has_value(); }
            const Sad::Data::Value &getThrownValue() const { return value_.value(); }
            void setThrownValue(const Sad::Data::Value &v) { value_ = v; }

            void addStackFrame(const std::string &frame) { stackTrace_.push_back(frame); }
            const std::vector<std::string> &getStackTrace() const noexcept { return stackTrace_; }

        private:
            std::optional<Sad::Data::Value> value_;
            std::string typeName_;
            std::string message_;
            Sad::Lexer::Position position_;
            std::vector<std::string> stackTrace_;
        };

        /**
         * @class ExitException
         * @brief (AR) طلب خروج نظيف من البرنامج (لتنظيف RAII)
         * @brief (EN) Clean program-exit request (preserves RAII unwind)
         */
        class ExitException : public std::exception
        {
        public:
            explicit ExitException(int code = 0) : exitCode_(code) {}
            int getExitCode() const noexcept { return exitCode_; }
            const char *what() const noexcept override { return "Program exit requested"; }

        private:
            int exitCode_;
        };

    } // namespace Interpreter
} // namespace Sad
