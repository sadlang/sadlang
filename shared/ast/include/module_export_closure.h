/**
 * @file module_export_closure.h
 * @brief (AR) إغلاقُ صادراتِ الوحدة: أيُّ تصريحٍ خاصٍّ يبلغه المُصدَّر؟
 * @brief (EN) Module export closure: which private declarations do exports reach?
 *
 * (AR) دلالةُ «صدّر» (gr.decl.export): ما لم يُصدَّر لا يُتاح للاستيراد. لكنّ
 *      الحجبَ الأعمى يقطع أحشاءَ الوحدة: دالّةٌ مُصدَّرةٌ تنادي مساعدًا خاصًّا،
 *      فحذفُ المساعِدِ يُعطِب المُصدَّر. فالمحجوبُ هو ما **لا يبلغه** المُصدَّر.
 *
 *      وهذا الحسابُ كان مكرَّرًا: نسخةٌ في المصرِّف وأخرى تلزم المفسّر — وهو
 *      نفسُ داءِ خريطتَي البوّابة اليدويّتين. فرُفع هنا نسخةً واحدةً
 *      يستهلكها المحرّكان.
 *
 * (EN) «صدّر» semantics: what is not exported is not importable. But blind
 *      hiding severs the module's internals — an exported function calling a
 *      private helper breaks when the helper is dropped. What must be hidden
 *      is exactly what the exports do NOT reach.
 *
 *      This computation used to be duplicated (one copy in the compiler, and
 *      the interpreter needed its own) — the same disease as the two
 *      hand-written import-gate maps. It lives here once for both engines.
 */

#pragma once

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "ast_node.h"

namespace Sad
{
    namespace AST
    {
        /**
         * (AR) يجمع كلَّ معرّفٍ حرٍّ في شجرةِ جملة — لا المُستدعَى وحدَه: فالمساعِدُ
         *      قد يُمرَّر مرجعًا بلا نداء، والثابتُ يُشار إليه اسمًا.
         * (EN) Collects every free identifier in a statement subtree — not just
         *      callees: a helper may be passed by reference, a constant named.
         */
        void collectReferencedNames(const Statement *stmt, std::set<std::string> &out);

        /** (AR) نظيرُه للتعابير — يلزم لمُهيّئِ الثابتِ المُصدَّر.
         *  (EN) Its expression counterpart — needed for an exported constant's initializer. */
        void collectReferencedNamesFromExpr(const Expression *expr, std::set<std::string> &out);

        /**
         * (AR) التصريحاتُ الخاصّةُ التي يبلغها المُصدَّرُ تعدّيًا، فتبقى منظورةً.
         *
         * @param moduleAst   شجرةُ الوحدة العلويّة.
         * @param requestedSymbols  رموزُ الاستيرادِ الانتقائيّ (تُتجاهَل مع الشامل).
         * @param isWildcard  استيرادٌ شامل ⇒ البذرةُ كلُّ المُصدَّر.
         *
         * (EN) The private declarations transitively reached from the exports,
         *      which therefore must stay visible.
         */
        std::set<std::string> computeExportedPrivateClosure(
            const std::vector<std::unique_ptr<Statement>> &moduleAst,
            const std::set<std::string> &requestedSymbols,
            bool isWildcard);
    } // namespace AST
} // namespace Sad
