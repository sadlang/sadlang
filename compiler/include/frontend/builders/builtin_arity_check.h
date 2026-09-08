// ============================================================================
// (AR) حارسُ رتبةِ المدمجِ — نسخةٌ واحدةٌ لكلِّ مخفّضاتِ المدمجات.
//
//      كانت الرتبةُ تُفحَص بأربعِ صيغٍ متباينةٍ في المصرّفِ الواحد:
//        ① أربعُ نسخٍ متطابقةٍ من الدالّةِ نفسِها (os_core، os_hardware،
//           os_system، uefi) — كلُّ إصلاحٍ يلزمه أربعُ رقعات؛
//        ② مئةٌ وتسعةُ فحوصٍ خامّةٍ تطبعُ سلسلةً نصّيّةً مكتوبةً باليدِ
//           **ويمضي البناءُ ناجحًا** فيتبخّرُ النداءُ صامتًا؛
//        ③ حرّاسُ الواجهةِ يبنون من الكتالوجِ ويطبعون بلا إفشال؛
//        ④ وأعدادٌ حرفيّةٌ في موضعِ الفحصِ لا يعرفُها مصدرُ الحقيقة.
//
// 🔑    والعلّةُ الجامعةُ واحدة: **حقيقةٌ واحدةٌ سكنت مواضعَ كثيرة**. فصار
//       العددُ يُعلَن مرّةً في `language-truth/builtins/*.yaml` (حقل `arity`)
//       ويُقرأ ثابتًا مولَّدًا، والنصُّ من كتالوجِ الأخطاءِ وحدَه، والتبعةُ
//       دفعٌ إلى `errors_` — أي إفشالُ البناء. رفضٌ يمضي بناؤه ليس رفضًا.
//
// (EN) One arity guard for every builtin lowering: the count comes from SoT,
//      the message from the error catalog, and the consequence is a build
//      failure — never a print that lets the call vanish.
// ============================================================================

#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "builtin_arity.h"
#include "error_manager.h"
#include "sir_builder_types.h" // (AR) BuildResult — نتيجةُ الرفضِ مملوءةُ الخانة

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            /// (AR) يُرجع true إن كان عددُ الوسائطِ داخلَ المدى المُعلَنِ في مصدرِ
            ///      الحقيقة؛ وإلّا دفع تشخيصَ SEM005 إلى `errors` وأرجع false.
            ///      المتوقَّعُ المعروضُ هو الطرفُ الذي خولِف: الأدنى عند النقصِ
            ///      والأقصى عند الزيادة — فالرسالةُ تدلُّ على الإصلاحِ لا على الخطأ.
            /// (EN) True when `found` is within the SoT-declared range; otherwise
            ///      pushes SEM005 into `errors` and returns false.
            [[nodiscard]] inline bool checkBuiltinArity(std::vector<std::string> &errors,
                                                        const std::string &name,
                                                        const Sad::Builtins::Arity::Range &range,
                                                        std::size_t found)
            {
                if (found >= range.min && found <= range.max)
                    return true;
                Sad::Errors::RenderContext ctx;
                ctx.placeholders = {
                    {"name", name},
                    {"expected", std::to_string(found < range.min ? range.min : range.max)},
                    {"found", std::to_string(found)}};
                errors.push_back(
                    Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                        Sad::Errors::ErrorCode::SEM_WRONG_ARG_COUNT, ctx));
                return false;
            }

            // ════════════════════════════════════════════════════════════════
            // (AR) 🔑 وحدةٌ في موضعِ وسيطِ تحويل: **رفضٌ مُعلَنٌ لا انهيار**.
            //
            //      كان `رقم(())` يُخفِقُ بـ`INT011` («خطأ مترجم داخلي … يُرجى
            //      الإبلاغ») — إذ تسقطُ الوحدةُ إلى ذراعِ النصِّ فيُمرَّرُ حاملُها
            //      `i8` إلى `strtoll(ptr, …)` فيرفضُه مدقّقُ LLVM. أي أنّ
            //      المترجّمَ كان **يطلبُ من كاتبِ البرنامجِ أن يُبلِّغَ عن علّةٍ
            //      لا وجودَ لها** — وهو الصنفُ الذي أُعلِنَ صفرًا في §١ من
            //      CLAUDE.md، وهذا بابٌ باقٍ منه.
            //
            //   🔑 والوحدةُ **لا حمولةَ لها بالتعريف**، فلا عددَ ولا نصَّ ولا
            //      عشريَّ «فيها» يُستخرَج. فالرفضُ ليس قيدَ تنفيذٍ مؤقّتًا
            //      يُرفَعُ يومًا، بل خاصّيّةُ النوعِ نفسِه — ولذلك يُشخَّصُ
            //      بالرمزِ ولا يُترَكُ للتوليد.
            //
            //   ⚠️ ولا رمزَ جديد: `RUN033` مُعلَنٌ بنائبَيه `{operation}` و
            //      `{type}` ونصُّه «نوع المعامل '{type}' غير مدعوم في العملية
            //      '{operation}'» — يصفُ الحالةَ حرفيًّا. وإعلانُ رمزٍ جديدٍ
            //      قرارُ مصدرِ حقيقةٍ لا ترقيعُ باني.
            // (EN) A unit in a conversion-argument position: an explicit rejection, not
            //      a crash. `رقم(())` failed with INT011 ("internal compiler error …
            //      please report") because the unit fell into the string arm and its i8
            //      carrier was passed to strtoll(ptr, …), which the LLVM verifier
            //      rejects — the compiler asking the author to report a bug that does not
            //      exist. The unit has NO payload by definition, so there is no number,
            //      text or float "inside" it to extract: the rejection is a property of
            //      the type, not a temporary implementation limit, so it is diagnosed by
            //      code rather than left to codegen. No new code is minted: RUN033 is
            //      declared with exactly the {operation} and {type} slots.
            // ════════════════════════════════════════════════════════════════
            [[nodiscard]] inline bool rejectUnitConversionArg(std::vector<std::string> &errors,
                                                             const std::string &name,
                                                             Sad::Types::SadTypeKind argKind)
            {
                if (argKind != Sad::Types::SadTypeKind::Unit)
                    return false;
                Sad::Errors::RenderContext ctx;
                ctx.placeholders = {
                    {"operation", name},
                    {"type", Sad::Types::sadTypeKindArabicName(Sad::Types::SadTypeKind::Unit)}};
                errors.push_back(
                    Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                        Sad::Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID, ctx));
                return true;
            }

            // ════════════════════════════════════════════════════════════════
            // (AR) 🔑 **نتيجةُ رفضٍ مملوءةُ الخانةِ — لا «تِلْو».**
            //      كان الرفضُ يُرجِعُ `BuildResult()` وهو `{خالي، سِجِلٌّ فارغ}`،
            //      فيبني المحيطُ (`اطبع(رقم(()))` مثلًا) `Register("")` ويمضي
            //      الخفضُ على معاملٍ مشوَّه ⇒ تشخيصٌ صحيحٌ يتلوه **اتّهامٌ كاذبٌ
            //      للمترجِم** «يُرجى الإبلاغ». وملءُ الخانةِ ليس تسترًا: البوّابةُ
            //      العامّةُ تُفشِلُ الترجمةَ حتمًا فلا ثنائيَّ يُسلَّم؛ غايتُه بقاءُ
            //      الخفضِ سليمَ البنيةِ حتّى تُجمَعَ بقيّةُ الأخطاءِ الحقيقيّة.
            // (EN) A rejection whose result slot is FILLED — not a التِّلْو. Returning
            //      the default BuildResult ({Unit, ""}) made the enclosing expression
            //      build Register("") and lower on a malformed operand: a correct
            //      diagnosis followed by a false accusation of the compiler. Filling
            //      the slot hides nothing — the gate fails the build regardless; it
            //      keeps lowering well-formed so the remaining real errors are collected.
            // ════════════════════════════════════════════════════════════════
            inline BuildResult rejectedConversionResult(Sad::Types::SadTypeKind kind)
            {
                switch (kind)
                {
                case Sad::Types::SadTypeKind::Float:
                    return BuildResult(std::string("0.0"), kind, true);
                case Sad::Types::SadTypeKind::String:
                    return BuildResult(std::string(""), kind, true);
                default:
                    return BuildResult(std::string("0"), kind, true);
                }
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
