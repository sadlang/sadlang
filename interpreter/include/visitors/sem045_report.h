/**
 * @file sem045_report.h
 * (AR) SEM045 (RFC عقد الغياب) — بابُ الإبلاغِ الواحد عن عبورِ «فراغ» إلى
 *      خانةٍ مصنَّفة. كانت الكتلةُ منسوخةً حرفيًّا في أربعةِ مواضعَ
 *      (التصريح، إعادة الإسناد، المعامل، حقل الصنف) — والنسخُ الأربعُ
 *      نسخٌ ثانيةٌ لحقيقةٍ واحدةٍ تنجرف صامتةً (درس «ثلاث نسخ: أصلحت
 *      اثنتين وتركت الثالثة»). المسنَدُ kindIsGuarded يوحّد طقمَ
 *      الاستثناءِ أيضًا: {Unknown، Any، Void، Null} خارجَ الحراسةِ عمدًا
 *      (المجرَّدُ خانةٌ ديناميّة، وAny قابلٌ للكلّ، وVoid يرفضه SEM040
 *      قبل الوصول، وNull علامةُ «عدمي» لا نوعُ خانة).
 * (EN) SEM045 (absence-contract RFC) — the single reporting door for a Void
 *      crossing into a typed slot. The block was copied verbatim at four
 *      sites (declaration, reassignment, parameter, class field) — four
 *      second copies of one fact drifting silently. kindIsGuarded also
 *      unifies the exemption set: {Unknown, Any, Void, Null} are unguarded
 *      on purpose (bare = dynamic slot, Any accepts all, Void is rejected
 *      by SEM040 upstream, Null is the «عدمي» marker, not a slot kind).
 */
#pragma once

#include <string>

#include "null_safety/null_safety_analyzer.h"
#include "sad_type_utils.h"
#include "token.h"

namespace Sad
{
    namespace Interpreter
    {
        namespace Sem045
        {
            /**
             * (AR) هل النوعُ المصرَّحُ مقصودٌ بحراسةِ SEM045؟
             * (EN) Is the declared kind guarded by SEM045?
             */
            bool kindIsGuarded(Sad::Types::SadTypeKind declaredKind);

            /**
             * (AR) الإبلاغُ الموحَّد على محورِ الصرامة D6: تجاهلٌ ⇒ لا شيء؛
             *      قاتلٌ ⇒ سطرُ خطأٍ على stderr ثم throwRuntime (يرمي
             *      RuntimeAbort فلا يعود)؛ وإلا ⇒ سطرُ تحذيرٍ على stderr
             *      وإبلاغُ الكتالوج تحذيرًا والتنفيذُ يستمرّ.
             * (EN) Unified D6-strictness reporting: Ignore ⇒ nothing;
             *      Fatal ⇒ stderr error line then throwRuntime (raises
             *      RuntimeAbort, does not return); otherwise ⇒ stderr
             *      warning line + catalog warning, execution continues.
             *
             * @param slotName       (AR) اسمُ الخانة (متغيّر/معامل/حقل) / (EN) slot name
             * @param typeArabicName (AR) اسمُ النوعِ المصرَّحِ بالعربية / (EN) declared type, Arabic
             * @param position       (AR) موضعُ الإسناد / (EN) assignment position
             * @param strictness     (AR) صرامةُ D6 المحسوبةُ عند المتصل / (EN) caller-resolved D6 strictness
             */
            void reportVoidCrossing(const std::string &slotName,
                                    const std::string &typeArabicName,
                                    const Sad::Lexer::Position &position,
                                    Sad::NullSafety::Strictness strictness);
        } // namespace Sem045
    } // namespace Interpreter
} // namespace Sad
