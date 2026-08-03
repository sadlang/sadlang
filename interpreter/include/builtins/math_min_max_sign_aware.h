#ifndef SAD_BUILTINS_MATH_MIN_MAX_SIGN_AWARE_H
#define SAD_BUILTINS_MATH_MIN_MAX_SIGN_AWARE_H

// ════════════════════════════════════════════════════════════════════════
// (AR) أصغر/أكبر بوعي الإشارة — مصدرٌ واحدٌ لدلالةِ الطبقتين (توحيدُ الازدواج)
// (EN) Sign-aware min/max — single source of truth (unifies the duplicate)
// ------------------------------------------------------------------------
//   كان هذا المنطقُ مكرَّرًا حرفيًّا في مسارَين: stdlib_manager (المسارُ الحيُّ
//   لـsad-run) وbuiltin_module_strings (مسارُ REPL). وُحِّدا هنا `inline` (بلا
//   خرقٍ سلوكيٍّ، مصدرٌ واحد). عندما تكون كلُّ الوسائطِ أعدادًا صحيحةً ونوعُها
//   السطحيُّ «طبيعي64» (UInt64، من ctx.argType = resolveStaticType عند النداء)
//   تُقارَنُ بلا إشارةٍ (uint64) مطابقةً للخلفيّةِ الأصليّة (cmovb/cmova في x86 ·
//   csel-hi/lo في ARM64). خلافَ ذلك — عشريّ/موقَّع/خليط — نُفوِّضُ إلى
//   MathFunctions (int64 موقَّعٌ دقيقٌ للصحيح، double للعشريّ) = الأصليُّ الموقَّع.
// (EN) This logic was duplicated verbatim in two paths: stdlib_manager (the LIVE
//   sad-run path) and builtin_module_strings (the REPL path). Unified here as an
//   `inline` single source (no behavioural change). When every argument is an
//   integer and its surface type is طبيعي64 (UInt64, from ctx.argType =
//   resolveStaticType at the call site) they are compared unsigned (uint64) to
//   match the native backend; otherwise (float/signed/mixed) we delegate to
//   MathFunctions (precise signed int64 for integers, double for floats).
// ════════════════════════════════════════════════════════════════════════

#include "builtins/builtin_context.h"
#include "math/math_functions.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace Sad
{
    namespace Interpreter
    {
        // (AR) isMax=true ⇒ أكبر، false ⇒ أصغر. يُرجع قيمةً عبر مؤشّرٍ مشترك.
        inline std::shared_ptr<Data::Value>
        mathMinMaxSignAware(Sad::Interpreter::BuiltinContext &ctx, bool isMax)
        {
            const auto &ptrArgs = ctx.args();

            // (AR) هل كلُّ الوسائطِ صحيحةٌ ونوعُها السطحيُّ طبيعي64 صريحٌ؟ ⇒ مقارنةٌ لا-موقَّعة.
            bool allUnsigned = !ptrArgs.empty();
            for (std::size_t i = 0; i < ptrArgs.size(); ++i)
            {
                const auto &p = ptrArgs[i];
                if (!p || !p->isInteger() ||
                    ctx.argType(i) != Sad::Types::SadTypeKind::UInt64)
                {
                    allUnsigned = false;
                    break;
                }
            }

            if (allUnsigned)
            {
                uint64_t best = static_cast<uint64_t>(ptrArgs[0]->toInt64());
                for (std::size_t i = 1; i < ptrArgs.size(); ++i)
                {
                    const uint64_t v = static_cast<uint64_t>(ptrArgs[i]->toInt64());
                    if (isMax ? (v > best) : (v < best))
                        best = v;
                }
                return std::make_shared<Data::Value>(static_cast<int64_t>(best));
            }

            // (AR) المسار العامّ: عشريّ/موقَّع/خليط ⇒ فوِّض إلى MathFunctions (تتخطّى الوسائطَ الفارغة).
            std::vector<Data::Value> plain;
            plain.reserve(ptrArgs.size());
            for (const auto &p : ptrArgs)
                if (p)
                    plain.push_back(*p);
            Data::Value result = isMax ? StdLib::Math::MathFunctions::max(plain)
                                       : StdLib::Math::MathFunctions::min(plain);
            return std::make_shared<Data::Value>(result);
        }
    } // namespace Interpreter
} // namespace Sad

#endif // SAD_BUILTINS_MATH_MIN_MAX_SIGN_AWARE_H
