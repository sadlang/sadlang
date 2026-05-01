/**
 * @file dispatch.h
 * @brief (AR) نقطة القرار الموحَّدة لسلوك الأعلام الثلاثة تجاه أخطاء الملكية
 *        (EN) Unified decision point: how the three memory flags respond to ownership errors
 *
 * (AR) هذا الملف يوفر دالة dispatch() التي تحدد كيفية تصرُّف اللغة
 *      عند مواجهة خطأ ملكية، بناءً على علم الذاكرة النشط:
 *
 *      --gc   → تجاهل خطأ الملكية (GC يدير الذاكرة، الملكية معطَّلة)
 *      --learn → تحذير + رسالة تعليمية (لا يُوقف التنفيذ)
 *      --prod  → خطأ مترجم قاتل (يوقف البناء)
 *
 * (EN) This file provides dispatch() — the single decision point that determines
 *      how the language reacts to ownership errors based on the active memory flag:
 *
 *      --gc   → ignore ownership error (GC manages memory, ownership is disabled)
 *      --learn → warning + educational message (does not stop execution)
 *      --prod  → fatal compiler error (stops build)
 *
 * (AR) لماذا ملف منفصل؟
 *      - كلا المسارين (المُفسِّر وsadc) يستدعيان نفس dispatch() بنفس المعاملات
 *      - أي تعديل في السلوك يحدث في مكان واحد فقط
 *      - snapshot tests تُفشل البناء عند أي انحراف بين المسارين
 *
 * (EN) Why a separate file?
 *      - Both paths (interpreter and sadc) call the same dispatch() with same parameters
 *      - Any behavior change happens in exactly one place
 *      - Snapshot tests fail the build on any divergence between paths
 *
 * @see shared/memory_policy/include/memory/policy/gc_mode.h — MemoryModeSettings
 * @see shared/errors/include/error_manager.h — ErrorManager
 * @see docs/خطة_توحيد_التنفيذ_والأعلام_الثلاثة.md — Section 7 (نظام الأخطاء المشترك)
 *
 * @version 1.0 — 2026-05-04
 */

#pragma once

#include "../diagnostic.h"
#include "../error_codes.h"
#include "../source_location.h"
// (AR) مسار نسبي صحيح:
//     dispatch.h يقع في: shared/errors/include/builders/
//     ../  → shared/errors/include/
//     ../../ → shared/errors/
//     ../../../ → shared/
//     ../../../memory_policy/include/memory/policy/gc_mode.h ✓
// (EN) Correct relative path from shared/errors/include/builders/
#include "../../../memory_policy/include/memory/policy/gc_mode.h"
// (AR) ضم header نظام الملكية لتعريف Sad::Semantic::OwnershipErrorKind
//      يُضم على المستوى العام (قبل فتح namespace) لتجنّب تلوّث Sad::Errors::std::*
//      عند تضمين <variant> ترانزتيًا داخل ownership_tracker.h.
// (EN) Include ownership tracker header for Sad::Semantic::OwnershipErrorKind
//      at file scope (before opening namespace) to avoid Sad::Errors::std::*
//      pollution when <variant> is transitively included by ownership_tracker.h.
#include "../../../ownership/include/ownership/ownership_tracker.h"
#include <string>
#include <optional>

namespace Sad
{
    namespace Errors
    {

        // (AR) استيراد الأنواع المستخدمة من namespaces أخرى
        // (EN) Import types used from other namespaces
        using Sad::Memory::MemoryMode;
        using Sad::Memory::MemoryModeSettings;
        using Sad::Memory::OwnershipMode;

        // =============================================================================
        //              أنواع أخطاء الملكية الثمانية / Eight Ownership Error Kinds
        // =============================================================================

        /**
         * @enum OwnershipErrorKind
         * @brief (AR) أنواع أخطاء نظام الملكية الثمانية
         *        (EN) The eight ownership error kinds
         *
         * (AR) هذه الأنواع الثمانية تُغطّي جميع انتهاكات نظام الملكية في لغة ص.
         *      كل نوع له سلوك مختلف بناءً على علم الذاكرة النشط (--gc / --learn / --prod).
         *
         * (EN) These eight kinds cover all ownership violations in Sad language.
         *      Each kind has different behavior depending on the active memory flag.
         */
        enum class OwnershipErrorKind
        {
            /// (AR) استخدام متغير بعد نقل ملكيته / (EN) Use of a moved variable
            UseAfterMove,

            /// (AR) استعارة متغير انتهت حياته / (EN) Borrow of variable past its lifetime
            BorrowAfterLifetimeEnd,

            /// (AR) استعارة قابلة للتعديل بينما توجد استعارة أخرى نشطة
            /// (EN) Mutable borrow while another borrow is active
            MutableBorrowConflict,

            /// (AR) إعادة إسناد متغير ثابت (const)
            /// (EN) Reassignment of const/immutable binding
            ImmutableReassignment,

            /// (AR) حلقة مرجعية (cycle) تمنع التحرير التلقائي
            /// (EN) Reference cycle preventing automatic deallocation
            ReferenceCycle,

            /// (AR) تسرُّب مورد لم يُحرَّر عند خروج النطاق
            /// (EN) Resource leak — not freed on scope exit
            ResourceLeak,

            /// (AR) استخدام مؤشر معلَّق (dangling pointer)
            /// (EN) Use of dangling pointer
            DanglingPointer,

            /// (AR) نقل قيمة من داخل حقل مُستعار
            /// (EN) Move out of borrowed field
            MoveFromBorrowed
        };

        // =============================================================================
        //              نتيجة dispatch / Dispatch Result
        // =============================================================================

        /**
         * @enum DispatchAction
         * @brief (AR) الإجراء الذي يجب اتخاذه بعد dispatch()
         *        (EN) The action to take after dispatch()
         */
        enum class DispatchAction
        {
            /// (AR) تجاهل الخطأ تماماً (--gc: الملكية معطَّلة)
            /// (EN) Ignore the error entirely (--gc: ownership disabled)
            Ignore,

            /// (AR) إصدار تحذير وإكمال التنفيذ (--learn)
            /// (EN) Emit warning and continue execution (--learn)
            Warn,

            /// (AR) إصدار خطأ قاتل وإيقاف البناء (--prod)
            /// (EN) Emit fatal error and stop build (--prod)
            Fatal
        };

        /**
         * @struct DispatchResult
         * @brief (AR) نتيجة dispatch() مع الرسالة المناسبة
         *        (EN) Result of dispatch() with appropriate message
         */
        struct DispatchResult
        {
            /// (AR) الإجراء المطلوب / (EN) Required action
            DispatchAction action;

            /// (AR) رسالة عربية للمستخدم (فارغة عند Ignore)
            /// (EN) Arabic message for user (empty on Ignore)
            std::string messageAr;

            /// (AR) رسالة إنجليزية للمستخدم (فارغة عند Ignore)
            /// (EN) English message for user (empty on Ignore)
            std::string messageEn;

            /// (AR) رسالة تعليمية إضافية (في --learn فقط)
            /// (EN) Additional educational message (--learn only)
            std::optional<std::string> teachingNote;

            /// (AR) هل يجب إيقاف التنفيذ؟ / (EN) Should execution stop?
            bool shouldStop() const
            {
                return action == DispatchAction::Fatal;
            }

            /// (AR) هل يجب إصدار تشخيص؟ / (EN) Should a diagnostic be emitted?
            bool shouldEmit() const
            {
                return action != DispatchAction::Ignore;
            }
        };

        // =============================================================================
        //              الدالة الرئيسية: dispatch()
        // =============================================================================

        /**
         * @brief (AR) نقطة القرار الموحَّدة — تحدد سلوك اللغة تجاه خطأ ملكية
         *        (EN) Unified decision point — determines language behavior for an ownership error
         *
         * (AR) تُستدعى من المُفسِّر وsadc بنفس المعاملات.
         *      لا تعتمد على حالة عامة — كل المعلومات تُمرَّر صراحةً.
         *
         * (EN) Called from both interpreter and sadc with the same parameters.
         *      Stateless — all information is passed explicitly.
         *
         * @param kind       (AR) نوع خطأ الملكية / (EN) Kind of ownership error
         * @param settings   (AR) إعدادات الذاكرة المُحلَّلة من CLI / (EN) Memory settings parsed from CLI
         * @param location   (AR) موقع الخطأ في الكود المصدري / (EN) Error location in source
         * @param symbolName (AR) اسم المتغير/الرمز المتأثر / (EN) Name of affected variable/symbol
         * @param extraInfo  (AR) معلومات إضافية اختيارية / (EN) Optional extra context
         *
         * @return (AR) DispatchResult يحمل الإجراء والرسائل المناسبة
         *         (EN) DispatchResult carrying the appropriate action and messages
         *
         * @example
         * @code
         * // (AR) استخدام من المُفسِّر / (EN) Usage from interpreter
         * auto result = Sad::Errors::dispatch(
         *     OwnershipErrorKind::UseAfterMove,
         *     currentSettings,
         *     location,
         *     "متغير_س"
         * );
         * if (result.shouldStop()) {
         *     throw SadRuntimeError(result.messageAr);
         * } else if (result.shouldEmit()) {
         *     ErrorManager::getInstance().reportWarning(..., result.messageAr, result.messageEn);
         * }
         * @endcode
         */
        DispatchResult dispatch(
            OwnershipErrorKind kind,
            const MemoryModeSettings &settings,
            const SourceLocation &location,
            const std::string &symbolName,
            const std::string &extraInfo = "");

        // =============================================================================
        //              دوال مساعدة / Helper Functions
        // =============================================================================

        /**
         * @brief (AR) يحوِّل نوع خطأ الملكية إلى رسالة عربية
         *        (EN) Converts ownership error kind to Arabic message template
         *
         * @param kind       (AR) نوع الخطأ / (EN) Error kind
         * @param symbolName (AR) اسم الرمز / (EN) Symbol name
         * @param extraInfo  (AR) معلومات إضافية / (EN) Extra info
         * @return (AR) رسالة الخطأ بالعربية / (EN) Error message in Arabic
         */
        std::string buildArabicMessage(
            OwnershipErrorKind kind,
            const std::string &symbolName,
            const std::string &extraInfo);

        /**
         * @brief (AR) يحوِّل نوع خطأ الملكية إلى رسالة إنجليزية
         *        (EN) Converts ownership error kind to English message template
         *
         * @param kind       (AR) نوع الخطأ / (EN) Error kind
         * @param symbolName (AR) اسم الرمز / (EN) Symbol name
         * @param extraInfo  (AR) معلومات إضافية / (EN) Extra info
         * @return (AR) رسالة الخطأ بالإنجليزية / (EN) Error message in English
         */
        std::string buildEnglishMessage(
            OwnershipErrorKind kind,
            const std::string &symbolName,
            const std::string &extraInfo);

        /**
         * @brief (AR) يُنشئ رسالة تعليمية لوضع --learn
         *        (EN) Builds an educational message for --learn mode
         *
         * @param kind (AR) نوع الخطأ / (EN) Error kind
         * @return (AR) الرسالة التعليمية / (EN) Educational message
         */
        std::string buildTeachingNote(OwnershipErrorKind kind);

        // =============================================================================
        //      تحويل Sad::Semantic::OwnershipErrorKind ← تسهيل ربط المفسر وsadc
        //      Mapper: Sad::Semantic::OwnershipErrorKind → Sad::Errors::OwnershipErrorKind
        // =============================================================================

        // (AR) ownership_tracker.h مُضمَّن في الأعلى (قبل فتح namespace) لتجنّب
        //      تلوّث Sad::Errors::std بـ <variant> الذي يضمّه ownership_tracker.h.
        // (EN) ownership_tracker.h is included at the top (before namespace) to avoid
        //      Sad::Errors::std pollution from <variant> brought in transitively.

        /**
         * @brief (AR) تحويل نوع الخطأ من نظام الملكية إلى نوع dispatch
         *        (EN) Map ownership tracker error kind to dispatch error kind
         *
         * (AR) يُتيح للمفسر وsadc استخدام dispatch() مع الأخطاء التي تُعيدها
         *      OwnershipTracker / OwnershipManager مباشرةً بدون تحويل يدوي.
         *
         * (EN) Allows interpreter and sadc to call dispatch() with errors returned
         *      by OwnershipTracker / OwnershipManager without manual conversion.
         */
        inline OwnershipErrorKind toDispatchKind(Sad::Semantic::OwnershipErrorKind semanticKind)
        {
            switch (semanticKind)
            {
            case Sad::Semantic::OwnershipErrorKind::UseAfterMove:
            case Sad::Semantic::OwnershipErrorKind::DoubleMove:
            case Sad::Semantic::OwnershipErrorKind::BorrowOfMoved:
                return OwnershipErrorKind::UseAfterMove;

            case Sad::Semantic::OwnershipErrorKind::MutBorrowConflict:
            case Sad::Semantic::OwnershipErrorKind::BorrowWhileMutBorrow:
            case Sad::Semantic::OwnershipErrorKind::MutBorrowWhileBorrow:
            case Sad::Semantic::OwnershipErrorKind::MutateWhileBorrowed:
                return OwnershipErrorKind::MutableBorrowConflict;

            case Sad::Semantic::OwnershipErrorKind::UseOfUninitialized:
                return OwnershipErrorKind::UseAfterMove;

            case Sad::Semantic::OwnershipErrorKind::DropWhileBorrowed:
                return OwnershipErrorKind::ResourceLeak;

            case Sad::Semantic::OwnershipErrorKind::InvalidLifetime:
                return OwnershipErrorKind::BorrowAfterLifetimeEnd;

            default:
                return OwnershipErrorKind::UseAfterMove;
            }
        }

        /**
         * @brief (AR) overload يقبل أخطاء OwnershipTracker مباشرة
         *        (EN) Overload accepting OwnershipTracker errors directly
         *
         * (AR) مختصر للاستخدام المباشر من المفسر وsadc:
         *      بدلاً من `dispatch(toDispatchKind(err.kind), ...)` يمكن كتابة
         *      `dispatch(err.kind, ...)`
         *
         * (EN) Shorthand for direct use from interpreter and sadc:
         *      instead of `dispatch(toDispatchKind(err.kind), ...)` write
         *      `dispatch(err.kind, ...)`
         */
        inline DispatchResult dispatch(
            Sad::Semantic::OwnershipErrorKind kind,
            const MemoryModeSettings &settings,
            const SourceLocation &location,
            const std::string &symbolName,
            const std::string &extraInfo = "")
        {
            return dispatch(toDispatchKind(kind), settings, location, symbolName, extraInfo);
        }

        // =============================================================================
        //          أخطاء وقت التشغيل خارج الملكية / Non-Ownership Runtime Errors
        //          (Phase F - بداية)
        // =============================================================================

        /**
         * @enum RuntimeErrorKind
         * @brief (AR) أنواع أخطاء وقت التشغيل خارج نظام الملكية
         *        (EN) Non-ownership runtime error kinds
         *
         * (AR) أخطاء تحدث في وقت التنفيذ نتيجة عمليات غير صالحة على القيم
         *      (وليس انتهاكات ملكية). تتبع نفس نظام التحكم بالأعلام:
         *      --gc → Ignore (للتوافق مع لغات GC)، --learn → Warn، --prod → Fatal.
         *
         * (EN) Errors that happen during execution due to invalid operations on values
         *      (not ownership violations). Follow the same flag-driven dispatch policy:
         *      --gc → Ignore (for GC-language parity), --learn → Warn, --prod → Fatal.
         *
         * @note (AR) في الإصدار الحالي يقتصر التطبيق على DivisionByZero كنموذج أولي.
         *            الأنواع الأخرى مُعرَّفة وجاهزة للربط لاحقاً.
         *       (EN) Current implementation wires only DivisionByZero as a prototype.
         *            Other kinds are declared and ready for wiring later.
         */
        enum class RuntimeErrorKind
        {
            /// (AR) قسمة على صفر (صحيح/عشري) / (EN) Division by zero (int/float)
            DivisionByZero,

            /// (AR) باقي قسمة على صفر / (EN) Modulo by zero
            ModuloByZero,

            /// (AR) فهرس مصفوفة خارج الحدود / (EN) Array index out of bounds
            IndexOutOfBounds,

            /// (AR) محاولة الوصول لخاصية على قيمة لاشيء / (EN) Member access on null
            NullMemberAccess,

            /// (AR) عدم تطابق نوع في وقت التشغيل / (EN) Runtime type mismatch
            TypeMismatch,

            /// (AR) استدعاء دالة غير موجودة / (EN) Call to undefined function
            UndefinedFunction,

            /// (AR) استخدام متغير غير معرَّف / (EN) Use of undefined variable
            UndefinedVariable,

            /// (AR) فيض رقمي (overflow) في عملية حسابية / (EN) Arithmetic overflow
            ArithmeticOverflow,

            /// (AR) نص فارغ في موقع يتطلب نص غير فارغ / (EN) Empty string where required non-empty
            EmptyStringRequired,

            /// (AR) مفتاح غير موجود في خريطة / (EN) Key not found in map
            KeyNotFound
        };

        /**
         * @brief (AR) overload لأخطاء وقت التشغيل خارج الملكية
         *        (EN) Overload for non-ownership runtime errors
         *
         * (AR) نفس آلية القرار كـ OwnershipErrorKind لكن مع رسائل مخصصة لكل نوع.
         *      السلوك:
         *        --gc       → Ignore  (السلوك الافتراضي للغات GC)
         *        --learn    → Warn + رسالة تعليمية
         *        --prod     → Fatal (يجب إيقاف التنفيذ/البناء)
         *
         * (EN) Same decision mechanism as OwnershipErrorKind but with messages
         *      tailored to each runtime error kind.
         *
         * @param kind       (AR) نوع خطأ وقت التشغيل / (EN) Runtime error kind
         * @param settings   (AR) إعدادات الذاكرة من CLI / (EN) Memory settings from CLI
         * @param location   (AR) موقع الخطأ / (EN) Error location
         * @param contextInfo (AR) سياق إضافي (مثل اسم المتغير أو نص التعبير)
         *                    (EN) Extra context (variable name or expression text)
         * @return (AR) DispatchResult يحمل الإجراء والرسائل
         *         (EN) DispatchResult with action and messages
         *
         * @example
         * @code
         * if (divisor == 0) {
         *     auto r = Sad::Errors::dispatch(
         *         RuntimeErrorKind::DivisionByZero,
         *         settings, loc, "س / ع");
         *     if (r.shouldStop()) throw SadRuntimeError(r.messageAr);
         *     if (r.shouldEmit()) emitWarning(r.messageAr);
         *     return Value::nothing(); // (AR) قيمة آمنة في وضع --gc/--learn
         * }
         * @endcode
         */
        DispatchResult dispatch(
            RuntimeErrorKind kind,
            const MemoryModeSettings &settings,
            const SourceLocation &location,
            const std::string &contextInfo = "");

        /**
         * @brief (AR) رسائل عربي/إنجليزي + ملاحظة تعليمية لأخطاء وقت التشغيل
         *        (EN) Arabic/English messages + teaching note for runtime errors
         */
        std::string buildRuntimeArabicMessage(RuntimeErrorKind kind, const std::string &context);
        std::string buildRuntimeEnglishMessage(RuntimeErrorKind kind, const std::string &context);
        std::string buildRuntimeTeachingNote(RuntimeErrorKind kind);

    } // namespace Errors
} // namespace Sad
