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
                           std::string_view name,
                           const std::vector<Types::SadTypeKind> *argTypes = nullptr) noexcept
                : args_(args), pos_(pos), name_(name), argTypes_(argTypes) {}

            // ─── الوسائط / Arguments ───
            const std::vector<ValuePtr> &args() const noexcept { return args_; }
            std::size_t argCount() const noexcept { return args_.size(); }
            /// (AR) وصول محمي بالحدود (CW-17) / (EN) bounds-checked access.
            const ValuePtr &arg(std::size_t i) const { return args_.at(i); }

            // ─── النوع السطحيّ الساكن للوسيط / Static surface type of an argument ───
            /**
             * @brief (AR) [طبقة طبيعي — الخطوة ٤] النوع الساكن للوسيط i (من resolveStaticType
             *        عند موقع النداء) — تستهلكه مدمجات الطباعة/التحويل لانتقاء تنسيق لا-موقَّع.
             *        الافتراض Integer إن لم يُمرَّر (يُبقي التنسيق الموقَّع القائم).
             * @brief (EN) [طبيعي layer — Step 4] Static type of argument i (from resolveStaticType
             *        at the call site) — consumed by print/convert built-ins to pick unsigned
             *        formatting. Defaults to Integer if not provided (keeps existing signed formatting).
             */
            Types::SadTypeKind argType(std::size_t i) const noexcept
            {
                return (argTypes_ && i < argTypes_->size()) ? (*argTypes_)[i]
                                                            : Types::SadTypeKind::Integer;
            }

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
            // (AR) أنواع الوسائط الساكنة (اختياريّ؛ null ⇒ افتراض موقَّع). يُملأ عند موقع
            //      النداء ويعيش حتّى نهاية callNative المتزامنة (مؤشّر لمتّجه محلّيّ هناك).
            // (EN) Optional static arg types (null ⇒ signed default). Filled at the call site,
            //      outlives the synchronous callNative (points to a local vector there).
            const std::vector<Types::SadTypeKind> *argTypes_ = nullptr;
        };

    } // namespace Interpreter
} // namespace Sad

#endif // SAD_INTERPRETER_BUILTIN_CONTEXT_H
