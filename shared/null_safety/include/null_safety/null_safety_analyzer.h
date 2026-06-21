// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4819)
#endif

/**
 * @file null_safety_analyzer.h
 * @brief محلّل أمان null المشترك / Shared Null-Safety Analyzer
 * @author Sad Language Team
 * @phase NS-03 — تحليل التدفّق (smart narrowing) فوق NS-04 (D1-D5)
 *
 * @details
 * (AR) المكوّن المشترك الموحَّد لأمان null. يُستدعى من **كلا المحرّكين**
 *      (المفسّر sad-run + المترجم sadc) ليكون التحليل **مصدر حقيقة واحدًا**،
 *      تمامًا كنمط فاحص الاستعارة (shared/ownership/borrow_checker).
 *
 *      هذه القصّة (NS-01) تُنشئ **الهيكل والاستدعاء فقط** — لا منطق تضييق ولا
 *      رصد بعد. تأتي الدلالة لاحقًا:
 *        - NS-02: تكامل dispatch() بمستوى صرامة مجرّد + حذف فرض P9 القديم.
 *        - NS-04: رصد الوصول غير الآمن على T؟ غير المُضيَّق.
 *        - NS-03: تحليل التدفّق (smart narrowing) فوق NS-04.
 *        - NS-05: عامل التأكيد !!.
 *
 * (EN) Unified shared null-safety component, invoked from BOTH engines
 *      (interpreter + sadc) so the analysis is a single source of truth —
 *      mirroring the borrow checker pattern. NS-01 sets up the structure and
 *      call sites only; no narrowing/detection logic yet.
 *
 * @see shared/ownership/include/ownership/borrow_checker.h — النمط المرجعيّ
 * @see _bmad-output/systems/null-safety/decisions/ADR-NS-001-...md — القرارات D1-D9
 * @see _bmad-output/systems/null-safety/stories/README.md — ترتيب القصص
 */

#ifndef SAD_NULL_SAFETY_ANALYZER_H
#define SAD_NULL_SAFETY_ANALYZER_H

// (AR) يكفي ast_node.h: الواجهة تستعمل AST::Statement (معرّف كامل هنا) فقط.
//      العقد المشتقة (statements.h) ستلزم زائرَ الـ.cpp في NS-04/NS-03 لا الرأس.
// (EN) ast_node.h suffices: the interface only needs AST::Statement (fully defined
//      here). Derived nodes (statements.h) belong to the .cpp visitor in NS-04/NS-03.
#include "ast_node.h"
#include <set>
// (AR) NS-02: يلزم OwnershipMode لاشتقاق الصرامة من سياسة الذاكرة (مابِع منفصل
//      عن الصنف حفاظًا على فصل المحورين — D6: الصنف نفسه لا يعرف الأعلام).
// (EN) NS-02: needs OwnershipMode to map memory policy → strictness via a free
//      mapper (kept outside the class to preserve D6 axis separation).
#include "../../../memory_policy/include/memory/policy/gc_mode.h"
#include <memory>
#include <string>
#include <vector>

// (AR) تصريح أماميّ: التوقيع يشير إلى VarDeclStmt؛ التعريف الكامل في الـ.cpp فقط.
// (EN) Forward declaration: signature references VarDeclStmt; full def in the .cpp.
namespace Sad
{
    namespace AST
    {
        class VarDeclStmt;
    }
}

namespace Sad
{
    namespace NullSafety
    {
        // ====================================================================
        // (AR) مستوى الصرامة المجرّد (ADR-NS-001 D6)
        // (EN) Abstract strictness level (ADR-NS-001 D6)
        // ====================================================================
        /**
         * (AR) المحلّل يأخذ **مستوى صرامة مجرّدًا** — لا يعرف أعلام الذاكرة.
         *      `dispatch()` (يُربط في NS-02) يترجم سياسة الذاكرة
         *      (--gc/--learn/--prod) إلى هذا المستوى. المحوران منفصلان داخليًّا.
         * (EN) The analyzer consumes an abstract strictness level and is unaware
         *      of memory flags. dispatch() (wired in NS-02) translates the memory
         *      policy into this level. The two axes are internally decoupled.
         */
        enum class Strictness
        {
            Ignore, ///< (AR) تجاهل التشخيص (نظير --gc) / (EN) ignore (≈ --gc)
            Warn,   ///< (AR) تحذير تعليميّ (نظير --learn) / (EN) warn  (≈ --learn)
            Fatal   ///< (AR) خطأ قاتل (نظير --prod) / (EN) fatal (≈ --prod)
        };

        // ====================================================================
        // (AR) مابِع سياسة الذاكرة → الصرامة المجرّدة (ADR-NS-001 D6)
        // (EN) Memory policy → abstract strictness mapper (ADR-NS-001 D6)
        // ====================================================================
        /**
         * (AR) الترجمة الوحيدة بين المحورين المتعامدين. يُستدعى من **كلا المحرّكين**
         *      فتبقى القاعدة مصدرَ حقيقة واحدًا (لا تكرار switch في موضعين):
         *        --gc (Disabled)               → Ignore
         *        --learn (Warnings، الافتراضي) → Warn
         *        --prod (Strict/UltraStrict)   → Fatal
         * (EN) The single translation point between the two orthogonal axes; invoked
         *      from both engines so the mapping stays a single source of truth.
         */
        inline Strictness strictnessFromOwnershipMode(Sad::Memory::OwnershipMode mode)
        {
            switch (mode)
            {
            case Sad::Memory::OwnershipMode::Disabled:
                return Strictness::Ignore;
            case Sad::Memory::OwnershipMode::Warnings:
                return Strictness::Warn;
            case Sad::Memory::OwnershipMode::Strict:
            case Sad::Memory::OwnershipMode::UltraStrict:
                return Strictness::Fatal;
            }
            return Strictness::Warn; // (AR) احتياطيّ آمن / (EN) safe fallback
        }

        // ====================================================================
        // (AR) أنواع أخطاء أمان null (ADR-NS-001 D6 — يحلّ القرار المؤجَّل في NS-01)
        // (EN) Null-safety error kinds (resolves the NS-01 deferred decision)
        // ====================================================================
        /**
         * @enum NullSafetyErrorKind
         * @brief (AR) أصناف انتهاكات أمان null المرصودة في المحلّل المشترك.
         *        (EN) Kinds of null-safety violations detected by the shared analyzer.
         *
         * @note (AR) NS-02 يبدأ بصنف واحد (نقل P9). تُضاف أصناف NS-04/NS-05 لاحقًا
         *       (وصول خام غير مُضيَّق، تأكيد `!!` على عدم) دون كسر التوقيع.
         */
        enum class NullSafetyErrorKind
        {
            /// (AR) إسناد `لاشيء` (عدم) حرفيًّا لمتغير من نوع غير اختياري [نقل P9]
            /// (EN) Assigning literal `null` to a non-optional-typed variable [P9]
            NullAssignedToNonOptional,
            /// (AR) وصول خام (`.عضو`/`.طريقة()`) على متغير اختياريّ `T؟` غير مُضيَّق [NS-04]
            /// (EN) Raw access (`.member`/`.method()`) on a non-narrowed optional `T?` [NS-04]
            UnsafeAccessOnOptional
        };

        // ====================================================================
        // (AR) نتيجة تحليل أمان null / Null-Safety Analysis Result
        // ====================================================================
        /**
         * @note (AR) **قرار بنيويّ مؤجَّل إلى NS-02:** الأخطاء هنا سلاسل نصّية مبدئيًّا.
         *       لتوجيهها عبر `Sad::Errors::dispatch()` (الذي يأخذ `…ErrorKind` لا سلسلة،
         *       انظر D6) سيلزم NS-02 إمّا إضافة `NullSafetyErrorKind` (لا يوجد بعد —
         *       dispatch مبنيّ اليوم على OwnershipErrorKind فقط) أو حمل بنية خطأ غنيّة
         *       تحمل (kind, location, variable). هذا الهيكل النصّيّ كافٍ لـNS-01 (لا
         *       إبلاغ فعليّ) لكنه **يُعاد تصميمه في NS-02** عند ربط الصرامة.
         * @note (EN) **Structural decision deferred to NS-02:** errors are plain strings
         *       for now. Routing them through dispatch() (which takes an ErrorKind, not a
         *       string) will require NS-02 to add a NullSafetyErrorKind (none exists yet)
         *       or carry a rich error record. Strings suffice for NS-01 (no reporting),
         *       but this shape is redesigned in NS-02.
         */
        /**
         * @struct NullSafetyDiagnostic
         * @brief (AR) تشخيص غنيّ واحد (NS-02 — يحلّ «بنية خطأ غنيّة» من ملاحظة NS-01).
         *        (EN) One rich diagnostic record (NS-02).
         *
         * (AR) يحمل ما يكفي لكي يُبلِّغ كلا المحرّكين بنفس الرسالة من نقطة قرار واحدة:
         *      الصنف + الموقع + الرمز + رسالتان جاهزتان + هل هو قاتل (حسب الصرامة).
         */
        struct NullSafetyDiagnostic
        {
            NullSafetyErrorKind kind = NullSafetyErrorKind::NullAssignedToNonOptional;
            int line = 0;             ///< (AR) السطر / (EN) source line
            int column = 0;           ///< (AR) العمود / (EN) source column
            std::string symbol;       ///< (AR) اسم المتغير / (EN) variable name
            std::string messageAr;    ///< (AR) رسالة عربية جاهزة / (EN) ready Arabic message
            std::string messageEn;    ///< (AR) رسالة إنجليزية جاهزة / (EN) ready English message
            bool fatal = false;       ///< (AR) قاتل (Fatal) أم تحذير (Warn)؟ / (EN) fatal vs warn
        };

        struct NullSafetyResult
        {
            bool success = true;                ///< (AR) هل نجح التحليل؟ / (EN) passed?
            std::vector<std::string> errors;    ///< (AR) أخطاء (نصّ) / (EN) errors (text)
            std::vector<std::string> warnings;  ///< (AR) تحذيرات (نصّ) / (EN) warnings (text)
            /// (AR) التشخيصات الغنيّة (NS-02) — يقرؤها المحرّكان للإبلاغ الموحَّد.
            /// (EN) Rich diagnostics (NS-02) — consumed by both engines for unified reporting.
            std::vector<NullSafetyDiagnostic> diagnostics;

            void addError(const std::string &msg)
            {
                errors.push_back(msg);
                success = false;
            }
            void addWarning(const std::string &msg) { warnings.push_back(msg); }

            /// (AR) يسجّل تشخيصًا غنيًّا ويملأ القوائم النصّية المرافِقة تلقائيًّا.
            /// (EN) Records a rich diagnostic and mirrors it into the text lists.
            void addDiagnostic(const NullSafetyDiagnostic &d)
            {
                diagnostics.push_back(d);
                if (d.fatal)
                    addError(d.messageAr);
                else
                    addWarning(d.messageAr);
            }
        };

        // ====================================================================
        // (AR) محلّل أمان null / Null-Safety Analyzer
        // ====================================================================
        /**
         * @class NullSafetyAnalyzer
         * @brief (AR) نقطة التحليل الموحَّدة لأمان null عبر المحرّكين.
         * @brief (EN) The unified null-safety analysis point across both engines.
         *
         * @note NS-01: `analyze()` لا يفحص شيئًا بعد — يعيد نتيجة ناجحة فارغة.
         *       الغرض إرساء الهيكل والاستدعاء من المحرّكين قبل بناء الدلالة.
         */
        class NullSafetyAnalyzer
        {
        public:
            NullSafetyAnalyzer() = default;
            ~NullSafetyAnalyzer() = default;

            /// (AR) تعيين مستوى الصرامة المجرّد (NS-02 سيشتقّه من السياسة).
            /// (EN) Set the abstract strictness level (NS-02 derives it from policy).
            void setStrictness(Strictness level) { strictness_ = level; }
            Strictness strictness() const { return strictness_; }

            /// (AR) رسائل عربية / (EN) Arabic messages
            void setArabicMessages(bool arabic) { useArabicMessages_ = arabic; }

            /**
             * @brief (AR) تحليل برنامج كامل لأمان null.
             * @brief (EN) Analyze a full program for null safety.
             *
             * @param program — (AR) قائمة جُمل البرنامج (غير مملوكة، تُقرأ فقط)
             *                   (EN) program statement list (non-owning, read-only)
             * @return (NullSafetyResult)
             *
             * @note (AR) واجهة غير مالكة قصدًا: المفسّر يمرّر `program` الثابتة والمترجم
             *       يمرّر `current_ast_` — كلاهما دون نقل ملكية. زائر AST الفعليّ
             *       (NS-04/NS-03) يُبنى فوق هذه الواجهة نفسها.
             * @note NS-01: لا منطق بعد — يعيد نتيجة ناجحة فارغة.
             */
            NullSafetyResult analyze(
                const std::vector<std::unique_ptr<AST::Statement>> &program);

        private:
            // (AR) ماشٍ متدرّج مركّز (NS-02): يهبط في كلّ الجُمل الحاملة للكتل
            //      ويفحص تصريحات المتغيرات. سليم-متحفّظ (D5): الإغفال = رصد أقلّ لا خطأ.
            // (EN) Focused recursive walker (NS-02): descends into all block-bearing
            //      statements, inspects var declarations. Sound-conservative (D5).
            void analyzeStmt(AST::Statement *stmt, NullSafetyResult &result);
            // (AR) فحص تصريح متغير واحد لحالة P9 (حرفيّ لاشيء → نوع غير اختياري).
            // (EN) Check a single var declaration for the P9 case.
            void checkVarDecl(AST::VarDeclStmt &decl, NullSafetyResult &result);

            // ── NS-04: ماشي التعابير + تتبّع المتغيرات الاختيارية ──────────────
            // (AR) يهبط في كلّ تعبير بحثًا عن وصول خام (`.عضو`/`.طريقة()`) على
            //      متغيّر مُعلَن اختياريًّا `T؟`. الوصول الآمن `؟.` (OptionalChainExpr)
            //      مستثنًى. لا تضييق بعد (NS-03 لاحقًا): كلّ وصول خام يُرصَد.
            // (EN) NS-04: descends into every expression looking for raw access
            //      (`.member`/`.method()`) on a declared optional `T?`. Safe access
            //      `?.` (OptionalChainExpr) is excluded. No narrowing yet (NS-03).
            void analyzeExpr(AST::Expression *expr, NullSafetyResult &result);
            // (AR) يرصد وصولًا خامًّا على كائنٍ متغيّرٍ اختياريّ ويُبلّغ.
            // (EN) Flags a raw access whose object is an optional variable.
            void checkOptionalAccess(AST::Expression *object, const std::string &member,
                                     bool isMethodCall, const Lexer::Position &pos,
                                     NullSafetyResult &result);

            // ── إدارة نطاق المتغيرات الاختيارية (NS-04) ───────────────────────
            // (AR) مكدّس نطاقات: كلّ نطاق مجموعة أسماء متغيرات اختيارية. يمنع تسرّب
            //      تعريف من دالّة إلى أخرى (تفادي إيجابيّ كاذب). لا تضييق هنا (D5).
            // (EN) Scope stack of optional-variable names; prevents cross-scope leakage.
            void pushScope() { optionalScopes_.emplace_back(); }
            void popScope()
            {
                if (!optionalScopes_.empty())
                    optionalScopes_.pop_back();
            }
            void declareOptional(const std::string &name)
            {
                if (!optionalScopes_.empty())
                    optionalScopes_.back().insert(name);
            }
            bool isOptionalVar(const std::string &name) const
            {
                for (auto it = optionalScopes_.rbegin(); it != optionalScopes_.rend(); ++it)
                    if (it->count(name))
                        return true;
                return false;
            }

            std::vector<std::set<std::string>> optionalScopes_; ///< (AR) مكدّس النطاقات

            // ── NS-03: تحليل التدفّق (smart narrowing) ─────────────────────────
            // (AR) المتغيّرات الاختيارية المُثبَت أنها **غير عدم** عند نقطة التدفّق
            //      الحالية (تضييق `T؟→T`). يُملأ بفحوص `!= لاشيء`/`== لاشيء` ويُفرَّغ
            //      عند التحوّر (D2). checkOptionalAccess يتخطّى المُضيَّق (لا تحذير).
            //      سليم-متحفّظ (D5): عند الشكّ لا يُضيِّق (يُبقي التحذير) لا العكس.
            // (EN) NS-03: variables proven non-null at the current flow point (T?→T).
            //      Populated by `!= null`/`== null` guards, cleared on mutation (D2).
            //      Sound-conservative (D5): when unsure, do NOT narrow (keep warning).
            std::set<std::string> narrowed_;
            bool isNarrowed(const std::string &name) const { return narrowed_.count(name) != 0; }

            Strictness strictness_ = Strictness::Warn; ///< (AR) الافتراضي تحذير (نظير --learn)
            bool useArabicMessages_ = true;
        };

    } // namespace NullSafety
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // SAD_NULL_SAFETY_ANALYZER_H
