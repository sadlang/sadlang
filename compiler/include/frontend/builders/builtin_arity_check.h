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

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
