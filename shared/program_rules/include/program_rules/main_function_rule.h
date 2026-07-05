#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4819)
#endif

/**
 * @file main_function_rule.h
 * @brief قاعدة الدالة الرئيسية المشتركة / Shared Main-Function Rule (SEM018)
 * @author Sad Language Team
 *
 * @details
 * (AR) المكوّن المشترك الموحَّد لقاعدة الدالة الرئيسية (SEM018). يُستدعى من
 *      **كلا المحرّكين** (المفسّر sad-run + المترجم sad-build) ليكون الفحص
 *      **مصدر حقيقة واحدًا** — تمامًا كنمط أمان null المشترك
 *      (shared/null_safety) وفاحص الاستعارة (shared/ownership).
 *
 *      القاعدة (SoT: language-truth/errors/semantic.yaml — SEM018): إن عرّف
 *      البرنامج دالةً **رئيسية** فهي نقطة الدخول الوحيدة؛ لا يُسمح بأيّ جملة
 *      تنفيذية في المستوى الأعلى معها (السماح فقط بالتصريحات والمتغيّرات العامّة
 *      وجُمل الوحدات). خلاف ذلك يلتبس ترتيب الدخول.
 *
 *      كان هذا الفحص محصورًا في المفسّر (interpreter_core) فيقبله المترجم بصمت
 *      (تباعد محرّكات)؛ هذا المكوّن يوحّده. الفحص **قراءة فقط بلا إبلاغ**: كلّ
 *      محرّك يُبلّغ عبر مساره (ErrorManager/diagnostics) من رمز الكتالوج نفسه.
 *
 * (EN) Unified shared component for the main-function rule (SEM018), invoked
 *      from BOTH engines so the check is a single source of truth — mirroring
 *      the shared null-safety and ownership patterns. Pure read-only structural
 *      check; each engine reports through its own path from the same catalog code.
 *
 * @see shared/null_safety/include/null_safety/null_safety_analyzer.h — النمط المرجعيّ
 * @see language-truth/errors/semantic.yaml — SEM018 (مصدر الحقيقة)
 */

#ifndef SAD_MAIN_FUNCTION_RULE_H
#define SAD_MAIN_FUNCTION_RULE_H

// (AR) يكفي ast_node.h: الواجهة تستعمل AST::Statement فقط (معرّف كامل هنا).
//      عقد الاشتقاق (declarations.h/statements.h) تلزم زائرَ الـ.cpp لا الرأس.
// (EN) ast_node.h suffices: the interface only needs AST::Statement.
#include "ast_node.h"
#include <cstddef> // (AR) std::size_t صراحةً (لا اعتماد جرٍّ عابر) / (EN) explicit std::size_t
#include <memory>
#include <vector>

namespace Sad
{
    namespace Semantic
    {
        // ====================================================================
        // (AR) نتيجة فحص قاعدة الدالة الرئيسية / Main-Function Rule Result
        // ====================================================================
        /**
         * @struct MainRuleResult
         * @brief (AR) نتيجة بنيويّة موجزة يقرؤها المحرّكان للإبلاغ الموحَّد.
         *        (EN) Compact structural result consumed by both engines.
         */
        struct MainRuleResult
        {
            /// (AR) هل عرّف البرنامج دالة رئيسية؟ / (EN) Does the program define main?
            bool hasMain = false;
            /// (AR) هل القاعدة سليمة (لا انتهاك)؟ / (EN) Rule satisfied (no violation)?
            bool ok = true;
            /// (AR) سطر أوّل جملة تنفيذية علويّة مخالِفة (1-أساس) / (EN) offending line
            std::size_t line = 1;
            /// (AR) عمود المخالفة (1-أساس) / (EN) offending column
            std::size_t column = 1;
        };

        // ====================================================================
        // (AR) فحص قاعدة الدالة الرئيسية (SEM018) — نقطة موحَّدة عبر المحرّكين
        // (EN) Main-function rule check (SEM018) — unified across both engines
        // ====================================================================
        /**
         * @brief (AR) يفحص قاعدة SEM018 على برنامج كامل (قراءة فقط، بلا إبلاغ).
         * @brief (EN) Checks the SEM018 rule over a full program (read-only).
         *
         * @param program (AR) قائمة جُمل البرنامج (غير مملوكة، تُقرأ فقط)
         *                (EN) program statement list (non-owning, read-only)
         * @return MainRuleResult — (AR) hasMain وok وموقع أوّل مخالفة عند وجودها.
         *
         * @note (AR) عند غياب الدالة الرئيسية: ok=true دائمًا (الكود العلويّ برنامجٌ
         *       يُنفَّذ تلقائيًّا). عند وجودها: ok=false إن وُجدت جملة تنفيذية علويّة.
         *       جُمل التصريح والمتغيّرات العامّة والاستيراد/التصدير مسموحة دائمًا.
         */
        MainRuleResult checkMainFunctionRule(
            const std::vector<std::unique_ptr<AST::Statement>> &program);

    } // namespace Semantic
} // namespace Sad

#endif // SAD_MAIN_FUNCTION_RULE_H

#ifdef _MSC_VER
#pragma warning(pop)
#endif
