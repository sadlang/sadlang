/**
 * @file builtin_context.h
 * @brief (AR) سياق استدعاء الدوال المضمنة — الوسائط + الموقع + أخطاء الكتالوج.
 * @brief (EN) Built-in invocation context — args + call-site position + catalog errors.
 *
 * @details
 * (AR) العقد الموحَّد للدوال المضمنة (ADR-EM-CPP-1). يحمل الوسائط وموقع الاستدعاء وواجهة
 *      `error()` التي تُطلق خطأً من الكتالوج المُولَّد بموقع صحيح — بدل `throw std::runtime_error`.
 *      قابل للتوسّع لاحقاً (اسم الدالة/المفسر/المكدس) بلا تغيير التواقيع.
 * (EN) Unified contract for built-ins (ADR-EM-CPP-1): args + call position + catalog `error()`.
 *
 * @see ../../../_bmad-output/systems/error-messages/docs/BUILTIN_CONTEXT_DESIGN.md
 */
#ifndef SAD_INTERPRETER_BUILTIN_CONTEXT_H
#define SAD_INTERPRETER_BUILTIN_CONTEXT_H

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "value.h"        // Sad::Data::Value
#include "token.h"        // Sad::Lexer::Position
#include "error_codes.h"  // Sad::Errors::ErrorCode

namespace Sad
{
    namespace Interpreter
    {

        /**
         * @class BuiltinContext
         * @brief (AR) سياق استدعاء دالة مضمنة واحدة (يُبنى في callNative).
         * @brief (EN) Per-invocation context for a built-in (built in callNative).
         */
        class BuiltinContext
        {
        public:
            using ValuePtr = std::shared_ptr<Data::Value>;

            BuiltinContext(const std::vector<ValuePtr> &args,
                           const Sad::Lexer::Position &pos,
                           std::string_view name) noexcept
                : args_(args), pos_(pos), name_(name) {}

            // ─── الوسائط / Arguments ───
            const std::vector<ValuePtr> &args() const noexcept { return args_; }
            std::size_t argCount() const noexcept { return args_.size(); }
            /// (AR) وصول محمي بالحدود (CW-17) / (EN) bounds-checked access.
            const ValuePtr &arg(std::size_t i) const { return args_.at(i); }

            // ─── الموقع / Position ───
            const Sad::Lexer::Position &position() const noexcept { return pos_; }
            std::string_view functionName() const noexcept { return name_; }

            // ─── أخطاء من الكتالوج المُولَّد / Catalog-based errors ───
            /**
             * @brief (AR) يبلّغ الكتالوج بموقع الاستدعاء ثم يرمي RuntimeAbort.
             * @brief (EN) Reports the error from the catalog at the call site, then aborts.
             * @note (AR) لا نص حر — الرسالة تعيش في language-truth/errors. يحقن "func" تلقائياً.
             */
            [[noreturn]] void error(Sad::Errors::ErrorCode code,
                                    std::map<std::string, std::string> placeholders = {}) const;

        private:
            const std::vector<ValuePtr> &args_;
            Sad::Lexer::Position         pos_;
            std::string_view             name_;
        };

    } // namespace Interpreter
} // namespace Sad

#endif // SAD_INTERPRETER_BUILTIN_CONTEXT_H
