/**
 * @file dispatch.cpp
 * @brief (AR) تنفيذ دالة dispatch() — نقطة القرار الموحَّدة لأخطاء الملكية
 *        (EN) Implementation of dispatch() — unified decision point for ownership errors
 *
 * (AR) القاعدة الأساسية:
 *      - --gc   (OwnershipMode::Disabled)     → Ignore كل أخطاء الملكية
 *      - --learn (OwnershipMode::Warnings)    → Warn  مع رسالة تعليمية
 *      - --prod  (OwnershipMode::UltraStrict) → Fatal يوقف البناء
 *
 * (EN) Core rule:
 *      - --gc   (OwnershipMode::Disabled)     → Ignore all ownership errors
 *      - --learn (OwnershipMode::Warnings)    → Warn with educational message
 *      - --prod  (OwnershipMode::UltraStrict) → Fatal stops the build
 *
 * @see shared/errors/include/builders/dispatch.h
 */

#include "../include/builders/dispatch.h"
#include <sstream>

namespace Sad
{
    namespace Errors
    {

        // =============================================================================
        //              الدالة الرئيسية: dispatch()
        // =============================================================================

        DispatchResult dispatch(
            OwnershipErrorKind kind,
            const MemoryModeSettings &settings,
            const SourceLocation &location,
            const std::string &symbolName,
            const std::string &extraInfo)
        {
            DispatchResult result;

            // (AR) القاعدة: سلوك DispatchAction يُحدَّد من OwnershipMode
            // (EN) Rule: DispatchAction is determined from OwnershipMode
            switch (settings.ownershipMode)
            {

            case OwnershipMode::Disabled:
                // (AR) --gc: الملكية معطَّلة تماماً — تجاهل الخطأ بدون أي رسالة
                // (EN) --gc: ownership disabled entirely — ignore error without any message
                result.action = DispatchAction::Ignore;
                return result;

            case OwnershipMode::Warnings:
                // (AR) --learn: تحذير + رسالة تعليمية — لا يوقف التنفيذ
                // (EN) --learn: warning + educational message — does not stop execution
                result.action = DispatchAction::Warn;
                result.messageAr = buildArabicMessage(kind, symbolName, extraInfo);
                result.messageEn = buildEnglishMessage(kind, symbolName, extraInfo);
                if (settings.teacherMode)
                {
                    result.teachingNote = buildTeachingNote(kind);
                }
                return result;

            case OwnershipMode::Strict:
                // (AR) صارم: خطأ يوقف البناء (بدون teachingMode)
                // (EN) Strict: fatal error stops build (no teaching mode)
                result.action = DispatchAction::Fatal;
                result.messageAr = buildArabicMessage(kind, symbolName, extraInfo);
                result.messageEn = buildEnglishMessage(kind, symbolName, extraInfo);
                return result;

            case OwnershipMode::UltraStrict:
                // (AR) --prod: خطأ قاتل يوقف البناء — أقصى صرامة (كـ Rust)
                // (EN) --prod: fatal error stops build — maximum strictness (like Rust)
                result.action = DispatchAction::Fatal;
                result.messageAr = buildArabicMessage(kind, symbolName, extraInfo);
                result.messageEn = buildEnglishMessage(kind, symbolName, extraInfo);
                return result;
            }

            // (AR) احتياطي — لا يُفترض الوصول هنا
            // (EN) Fallback — should never reach here
            result.action = DispatchAction::Ignore;
            return result;
        }

        // =============================================================================
        //              الرسائل العربية / Arabic Messages
        // =============================================================================

        std::string buildArabicMessage(
            OwnershipErrorKind kind,
            const std::string &symbolName,
            const std::string &extraInfo)
        {
            std::ostringstream msg;

            switch (kind)
            {
            case OwnershipErrorKind::UseAfterMove:
                msg << "استخدام `" << symbolName << "` بعد نقل ملكيته";
                if (!extraInfo.empty())
                    msg << " — " << extraInfo;
                return msg.str();

            case OwnershipErrorKind::BorrowAfterLifetimeEnd:
                msg << "استعارة `" << symbolName << "` بعد انتهاء حياته";
                if (!extraInfo.empty())
                    msg << " — " << extraInfo;
                return msg.str();

            case OwnershipErrorKind::MutableBorrowConflict:
                msg << "تعارض في استعارة `" << symbolName << "`: استعارة قابلة للتعديل مع وجود استعارة أخرى نشطة";
                if (!extraInfo.empty())
                    msg << " — " << extraInfo;
                return msg.str();

            case OwnershipErrorKind::ImmutableReassignment:
                msg << "إعادة إسناد `" << symbolName << "` وهو ثابت (const) — استخدم `متغير` بدل `ثابت`";
                if (!extraInfo.empty())
                    msg << " — " << extraInfo;
                return msg.str();

            case OwnershipErrorKind::ReferenceCycle:
                msg << "دورة مرجعية تضم `" << symbolName << "` — قد تمنع تحرير الذاكرة";
                if (!extraInfo.empty())
                    msg << " — " << extraInfo;
                return msg.str();

            case OwnershipErrorKind::ResourceLeak:
                msg << "تسرُّب مورد: `" << symbolName << "` لم يُحرَّر عند الخروج من النطاق";
                if (!extraInfo.empty())
                    msg << " — " << extraInfo;
                return msg.str();

            case OwnershipErrorKind::DanglingPointer:
                msg << "استخدام مؤشر معلَّق: `" << symbolName << "` يشير إلى ذاكرة محرَّرة";
                if (!extraInfo.empty())
                    msg << " — " << extraInfo;
                return msg.str();

            case OwnershipErrorKind::MoveFromBorrowed:
                msg << "نقل ملكية من حقل مُستعار: `" << symbolName << "` — لا يمكن نقل قيمة من حقل مستعار";
                if (!extraInfo.empty())
                    msg << " — " << extraInfo;
                return msg.str();
            }

            return "خطأ ملكية في `" + symbolName + "`";
        }

        // =============================================================================
        //              الرسائل الإنجليزية / English Messages
        // =============================================================================

        std::string buildEnglishMessage(
            OwnershipErrorKind kind,
            const std::string &symbolName,
            const std::string &extraInfo)
        {
            std::ostringstream msg;

            switch (kind)
            {
            case OwnershipErrorKind::UseAfterMove:
                msg << "use of `" << symbolName << "` after move";
                if (!extraInfo.empty())
                    msg << " — " << extraInfo;
                return msg.str();

            case OwnershipErrorKind::BorrowAfterLifetimeEnd:
                msg << "borrow of `" << symbolName << "` does not live long enough";
                if (!extraInfo.empty())
                    msg << " — " << extraInfo;
                return msg.str();

            case OwnershipErrorKind::MutableBorrowConflict:
                msg << "cannot borrow `" << symbolName << "` as mutable because it is also borrowed as immutable";
                if (!extraInfo.empty())
                    msg << " — " << extraInfo;
                return msg.str();

            case OwnershipErrorKind::ImmutableReassignment:
                msg << "cannot assign twice to immutable variable `" << symbolName << "`";
                if (!extraInfo.empty())
                    msg << " — " << extraInfo;
                return msg.str();

            case OwnershipErrorKind::ReferenceCycle:
                msg << "reference cycle detected involving `" << symbolName << "` — may prevent deallocation";
                if (!extraInfo.empty())
                    msg << " — " << extraInfo;
                return msg.str();

            case OwnershipErrorKind::ResourceLeak:
                msg << "resource leak: `" << symbolName << "` is not freed on scope exit";
                if (!extraInfo.empty())
                    msg << " — " << extraInfo;
                return msg.str();

            case OwnershipErrorKind::DanglingPointer:
                msg << "use of dangling pointer: `" << symbolName << "` points to freed memory";
                if (!extraInfo.empty())
                    msg << " — " << extraInfo;
                return msg.str();

            case OwnershipErrorKind::MoveFromBorrowed:
                msg << "cannot move out of borrowed field `" << symbolName << "`";
                if (!extraInfo.empty())
                    msg << " — " << extraInfo;
                return msg.str();
            }

            return "ownership error in `" + symbolName + "`";
        }

        // =============================================================================
        //              الرسائل التعليمية / Teaching Notes (--learn mode)
        // =============================================================================

        std::string buildTeachingNote(OwnershipErrorKind kind)
        {
            switch (kind)
            {
            case OwnershipErrorKind::UseAfterMove:
                return "📖 نظام الملكية: عند نقل متغير، تنتقل الملكية ولا يمكن استخدامه مجدداً.\n"
                       "   الحل: استخدم استعارة (&) إذا كنت تريد مجرد قراءة القيمة.\n"
                       "   مثال: دالة(‌&متغيري) بدلاً من دالة(متغيري)";

            case OwnershipErrorKind::BorrowAfterLifetimeEnd:
                return "📖 حياة المتغيرات: الاستعارة يجب أن تعيش أقل من أو مساوياً للمتغير الأصلي.\n"
                       "   الحل: تأكد أن المتغير المُستعار موجود طوال فترة استخدام الاستعارة.";

            case OwnershipErrorKind::MutableBorrowConflict:
                return "📖 قاعدة الاستعارة: يمكن وجود عدة استعارات للقراءة، أو استعارة واحدة للكتابة — لا كليهما.\n"
                       "   الحل: أنهِ الاستعارات للقراءة قبل الاستعارة للكتابة.";

            case OwnershipErrorKind::ImmutableReassignment:
                return "📖 الثوابت: `ثابت` يعني القيمة لا تتغير. استخدم `متغير` إذا كنت تحتاج لتغيير القيمة.";

            case OwnershipErrorKind::ReferenceCycle:
                return "📖 الدورات المرجعية: عندما يشير A إلى B ويشير B إلى A، لا يمكن تحرير أيٍّ منهما تلقائياً.\n"
                       "   الحل: استخدم مؤشراً ضعيفاً (weak reference) لأحد الاتجاهين.";

            case OwnershipErrorKind::ResourceLeak:
                return "📖 إدارة الموارد: الموارد (ملفات، اتصالات) يجب تحريرها عند الخروج من نطاقها.\n"
                       "   الحل: استخدم `أجِّل` لضمان التنظيف التلقائي.";

            case OwnershipErrorKind::DanglingPointer:
                return "📖 المؤشرات المعلَّقة: المؤشر يشير إلى ذاكرة تم تحريرها. هذا سلوك غير معرَّف.\n"
                       "   الحل: تأكد أن المؤشر لا يعيش أطول من القيمة التي يشير إليها.";

            case OwnershipErrorKind::MoveFromBorrowed:
                return "📖 النقل من المُستعار: لا يمكن نقل قيمة من حقل مُستعار لأن ذلك يُبطل الاستعارة.\n"
                       "   الحل: انسخ القيمة أو أنهِ الاستعارة أولاً.";
            }

            return "📖 راجع توثيق نظام الملكية في لغة ص للمزيد من التفاصيل.";
        }

        // =============================================================================
        //          تنفيذ RuntimeErrorKind dispatch (Phase F - بداية)
        //          Implementation of RuntimeErrorKind dispatch (Phase F - start)
        // =============================================================================

        DispatchResult dispatch(
            RuntimeErrorKind kind,
            const MemoryModeSettings &settings,
            const SourceLocation &location,
            const std::string &contextInfo)
        {
            (void)location; // (AR) متاح للاستخدام المستقبلي في رسائل مع snippet
            DispatchResult result;

            switch (settings.ownershipMode)
            {
            case OwnershipMode::Disabled:
                // (AR) --gc: تجاهل (السلوك المتوقع للغات GC)
                // (EN) --gc: ignore (expected behavior for GC languages)
                result.action = DispatchAction::Ignore;
                return result;

            case OwnershipMode::Warnings:
                // (AR) --learn: تحذير + ملاحظة تعليمية
                // (EN) --learn: warning + educational note
                result.action = DispatchAction::Warn;
                result.messageAr = buildRuntimeArabicMessage(kind, contextInfo);
                result.messageEn = buildRuntimeEnglishMessage(kind, contextInfo);
                if (settings.teacherMode)
                {
                    result.teachingNote = buildRuntimeTeachingNote(kind);
                }
                return result;

            case OwnershipMode::Strict:
            case OwnershipMode::UltraStrict:
                // (AR) --prod: خطأ قاتل
                // (EN) --prod: fatal error
                result.action = DispatchAction::Fatal;
                result.messageAr = buildRuntimeArabicMessage(kind, contextInfo);
                result.messageEn = buildRuntimeEnglishMessage(kind, contextInfo);
                return result;
            }

            result.action = DispatchAction::Ignore;
            return result;
        }

        std::string buildRuntimeArabicMessage(RuntimeErrorKind kind, const std::string &context)
        {
            std::ostringstream msg;
            switch (kind)
            {
            case RuntimeErrorKind::DivisionByZero:
                msg << "قسمة على صفر";
                if (!context.empty())
                    msg << " في `" << context << "`";
                return msg.str();
            case RuntimeErrorKind::ModuloByZero:
                msg << "باقي قسمة على صفر";
                if (!context.empty())
                    msg << " في `" << context << "`";
                return msg.str();
            case RuntimeErrorKind::IndexOutOfBounds:
                msg << "فهرس خارج حدود المصفوفة";
                if (!context.empty())
                    msg << ": " << context;
                return msg.str();
            case RuntimeErrorKind::NullMemberAccess:
                msg << "محاولة الوصول لخاصية على قيمة لاشيء";
                if (!context.empty())
                    msg << ": " << context;
                return msg.str();
            case RuntimeErrorKind::TypeMismatch:
                msg << "عدم تطابق نوع في وقت التشغيل";
                if (!context.empty())
                    msg << ": " << context;
                return msg.str();
            case RuntimeErrorKind::UndefinedFunction:
                msg << "استدعاء دالة غير معرَّفة";
                if (!context.empty())
                    msg << ": `" << context << "`";
                return msg.str();
            case RuntimeErrorKind::UndefinedVariable:
                msg << "استخدام متغير غير معرَّف";
                if (!context.empty())
                    msg << ": `" << context << "`";
                return msg.str();
            case RuntimeErrorKind::ArithmeticOverflow:
                msg << "فيض رقمي في عملية حسابية";
                if (!context.empty())
                    msg << ": " << context;
                return msg.str();
            case RuntimeErrorKind::EmptyStringRequired:
                msg << "نص فارغ في موقع يتطلب نصاً غير فارغ";
                if (!context.empty())
                    msg << ": " << context;
                return msg.str();
            case RuntimeErrorKind::KeyNotFound:
                msg << "مفتاح غير موجود في الخريطة";
                if (!context.empty())
                    msg << ": `" << context << "`";
                return msg.str();
            }
            return "خطأ في وقت التشغيل";
        }

        std::string buildRuntimeEnglishMessage(RuntimeErrorKind kind, const std::string &context)
        {
            std::ostringstream msg;
            switch (kind)
            {
            case RuntimeErrorKind::DivisionByZero:
                msg << "division by zero";
                if (!context.empty())
                    msg << " in `" << context << "`";
                return msg.str();
            case RuntimeErrorKind::ModuloByZero:
                msg << "modulo by zero";
                if (!context.empty())
                    msg << " in `" << context << "`";
                return msg.str();
            case RuntimeErrorKind::IndexOutOfBounds:
                msg << "array index out of bounds";
                if (!context.empty())
                    msg << ": " << context;
                return msg.str();
            case RuntimeErrorKind::NullMemberAccess:
                msg << "member access on null value";
                if (!context.empty())
                    msg << ": " << context;
                return msg.str();
            case RuntimeErrorKind::TypeMismatch:
                msg << "runtime type mismatch";
                if (!context.empty())
                    msg << ": " << context;
                return msg.str();
            case RuntimeErrorKind::UndefinedFunction:
                msg << "call to undefined function";
                if (!context.empty())
                    msg << ": `" << context << "`";
                return msg.str();
            case RuntimeErrorKind::UndefinedVariable:
                msg << "use of undefined variable";
                if (!context.empty())
                    msg << ": `" << context << "`";
                return msg.str();
            case RuntimeErrorKind::ArithmeticOverflow:
                msg << "arithmetic overflow";
                if (!context.empty())
                    msg << ": " << context;
                return msg.str();
            case RuntimeErrorKind::EmptyStringRequired:
                msg << "empty string where non-empty required";
                if (!context.empty())
                    msg << ": " << context;
                return msg.str();
            case RuntimeErrorKind::KeyNotFound:
                msg << "key not found in map";
                if (!context.empty())
                    msg << ": `" << context << "`";
                return msg.str();
            }
            return "runtime error";
        }

        std::string buildRuntimeTeachingNote(RuntimeErrorKind kind)
        {
            switch (kind)
            {
            case RuntimeErrorKind::DivisionByZero:
            case RuntimeErrorKind::ModuloByZero:
                return "📖 القسمة على صفر غير معرَّفة رياضياً.\n"
                       "   الحل: تحقق أن المقسوم عليه ≠ 0 قبل العملية:\n"
                       "   إذا (ع != 0) { نتيجة = س / ع } وإلا { ... }";
            case RuntimeErrorKind::IndexOutOfBounds:
                return "📖 الفهارس في المصفوفات تبدأ من 0 وتنتهي عند الطول-1.\n"
                       "   الحل: تحقق من الفهرس قبل الوصول:\n"
                       "   إذا (ف >= 0 و ف < طول(م)) { ... }";
            case RuntimeErrorKind::NullMemberAccess:
                return "📖 لاشيء (null) لا يحوي أي خاصية أو دالة.\n"
                       "   الحل: تحقق من القيمة قبل الوصول:\n"
                       "   إذا (ك != لاشيء) { ك.خاصية }";
            case RuntimeErrorKind::TypeMismatch:
                return "📖 العمليات تتطلب أنواعاً متوافقة (مثل رقم+رقم، نص+نص).\n"
                       "   الحل: استخدم دوال التحويل: رقم(ن)، نص(ر)، عشري(ع).";
            case RuntimeErrorKind::UndefinedFunction:
            case RuntimeErrorKind::UndefinedVariable:
                return "📖 تأكد من تعريف الرمز قبل استخدامه، وتحقق من التهجئة والاستيرادات.";
            case RuntimeErrorKind::ArithmeticOverflow:
                return "📖 الأرقام الصحيحة لها حد أعلى. للأرقام الكبيرة، استخدم نوع `عشري`.";
            case RuntimeErrorKind::EmptyStringRequired:
                return "📖 تحقق من النص بـ `.طول() > 0` أو `.فارغ()` قبل الاستخدام.";
            case RuntimeErrorKind::KeyNotFound:
                return "📖 استخدم `.يحتوي(مفتاح)` للتحقق قبل القراءة، أو وفّر قيمة افتراضية.";
            }
            return "📖 راجع توثيق لغة ص للمزيد من التفاصيل.";
        }

    } // namespace Errors
} // namespace Sad
