// ============================================================================
// sir_builder_lambda_inference.cpp — استدلال أنواع معاملات لامبدا
// (AR) مسح مواقع الاستدعاء واستنتاج أنواع معاملات اللامبدا تلقائياً
// (EN) Scan call sites and infer lambda parameter types automatically
// تم استخراج هذا الملف من sir_builder_type_inference.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include <functional>
#include <string>
#include <map>
#include "sir_builder.h"
#include "builders/template_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include "sad_debug_log.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // (AR) جداول أسماء الطرق — لاستدلال أنواع معاملات اللامبدا
            // (EN) Method name tables — for lambda param type inference
            static const std::set<std::string> kStringMethodNames = {
                "\xD8\xB7\xD9\x88\xD9\x84",                                                  // طول
                "\xD9\x8A\xD8\xAD\xD8\xAA\xD9\x88\xD9\x8A",                                  // يحتوي
                "\xD9\x8A\xD8\xA8\xD8\xAF\xD8\xA3_\xD8\xA8\xD9\x80",                         // يبدأ_بـ
                "\xD9\x8A\xD9\x86\xD8\xAA\xD9\x87\xD9\x8A_\xD8\xA8\xD9\x80",                 // ينتهي_بـ
                "\xD8\xAA\xD9\x82\xD8\xB3\xD9\x8A\xD9\x85",                                  // تقسيم
                "\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xA8\xD8\xAF\xD9\x84",                          // استبدل
                "\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xB1\xD8\xA7\xD8\xAC",                  // استخراج
                "\xD9\x82\xD8\xB5_\xD8\xA3\xD8\xB7\xD8\xB1\xD8\xA7\xD9\x81",                 // قص_أطراف
                "\xD8\xAA\xD8\xAD\xD9\x88\xD9\x8A\xD9\x84_\xD9\x83\xD8\xA8\xD9\x8A\xD8\xB1", // تحويل_كبير
                "\xD8\xAA\xD8\xAD\xD9\x88\xD9\x8A\xD9\x84_\xD8\xB5\xD8\xBA\xD9\x8A\xD8\xB1", // تحويل_صغير
            };
            static const std::set<std::string> kArrayMethodNames = {
                "\xD8\xA7\xD8\xB6\xD9\x81",                                          // اضف
                "\xD8\xA7\xD8\xAD\xD8\xB0\xD9\x81_\xD8\xA7\xD8\xAE\xD9\x8A\xD8\xB1", // احذف_اخير
                "\xD8\xB1\xD8\xAA\xD8\xA8",                                          // رتب
                "\xD8\xB9\xD9\x83\xD8\xB3",                                          // عكس
                "\xD8\xAE\xD8\xB1\xD9\x8A\xD8\xB7\xD8\xA9",                          // خريطة
                "\xD8\xB1\xD8\xB4\xD8\xAD",                                          // رشح
                "\xD8\xA7\xD8\xAE\xD8\xAA\xD8\xB2\xD9\x84",                          // اختزل
                "\xD9\x84\xD9\x83\xD9\x84",                                          // لكل
                "\xD8\xA7\xD9\x88\xD9\x84",                                          // اول
                "\xD8\xA2\xD8\xAE\xD8\xB1",                                          // آخر
                "\xD9\x81\xD8\xA7\xD8\xB1\xD8\xBA\xD8\xA9",                          // فارغة
                "\xD8\xA7\xD9\x84\xD8\xB7\xD9\x88\xD9\x84",                          // الطول
                "\xD8\xB4\xD8\xB1\xD9\x8A\xD8\xAD\xD8\xA9",                          // شريحة
            };

            // ============================================================================
            // refineMemberParams / refineCalledMember — استنتاج أنواع معاملات الأعضاء
            // ============================================================================
            // (AR) تُرقَّى أنواعُ معاملاتِ العضوِ من مواقعِ النداءِ كما تُرقّى معاملاتُ
            //      الدالّةِ الحرّة. والترقيةُ تُكتَبُ في الشجرة، فبانِيا الصنفِ والطريقةِ
            //      يقرآنِ أنواعَهما منها. وهي أُحاديّةُ الاتّجاه: من الافتراضيِّ العامِّ
            //      إلى الأخصّ، ولا تنقضُ نوعًا صرّح به الكاتب.
            // (EN) A member's parameter types are promoted from call sites exactly as a free
            //      function's are. The promotion is written into the AST, which is where the
            //      class and method builders read their types from. It is one-way — from the
            //      general default toward the more specific — and never overrides a type the
            //      author declared.
            void TemplateBuilder::recordMemberParamArgs(
                std::vector<Sad::AST::Parameter> &params,
                const std::vector<std::unique_ptr<Sad::AST::Expr>> &arguments,
                bool paramsBelongToConstructor)
            {
                for (size_t index = 0; index < params.size(); ++index)
                {
                    auto &param = params[index];

                    // (AR) لا يُرقّى إلّا معامِلٌ **لم يُصرَّحْ** نوعُه؛ فالنوعُ المُصرَّحُ
                    //      عقدُ الكاتبِ لا يُخمَّنُ فوقه. والمعامِلُ غيرُ المُصرَّحِ يبلغُ
                    //      الشجرةَ بنوعِ «صنف» واسمِ نوعٍ فارغ — لا بـ«مجهول» كما قد
                    //      يُظَنّ؛ وهذا مقيسٌ لا مُستنتَج.
                    // (EN) Consider only a parameter whose type was **not declared**; a declared
                    //      type is the author's contract and is never guessed over. An
                    //      undeclared parameter reaches the AST as Class with an empty type
                    //      name — not as Unknown as one might assume; this is measured, not
                    //      supposed.
                    const bool isUndeclared =
                        param.type == Types::SadTypeKind::Unknown ||
                        (param.type == Types::SadTypeKind::Class && param.typeName.empty());
                    // (AR) استثناءٌ واحدٌ مقيس (نظيرُ دَين #258 في الدوالِّ الحرّة):
                    //      المصرَّحُ «رقم» يقبلُ العشريَّ في المفسّرِ (المرجعُ لا يقسر
                    //      معاملَ الطريقة — بذرة [٧])، فتُسجَّلُ وسائطُه ليُرقَّى عشريًّا
                    //      **بالإجماعِ حصرًا** في applyAgreedMemberParamTypes. وبقيّةُ
                    //      الأنواعِ المصرَّحةِ عقدُ الكاتبِ ولا تُخمَّنُ فوقه.
                    // (EN) One measured exception (mirror of free-function debt #258): a
                    //      declared «رقم» accepts floats in the interpreter (the reference
                    //      does not coerce method params — red seed [٧]), so its arguments
                    //      are recorded and may be widened to Float ONLY on unanimity in
                    //      applyAgreedMemberParamTypes. Every other declared type is the
                    //      author's contract and is never guessed over.
                    const bool isDeclaredInteger = param.type == Types::SadTypeKind::Integer;
                    // (AR) والمصرَّحُ «عشري» يُسجَّلُ كذلك — لا ليُرقَّى عدديًّا (عقدُه
                    //      قائمٌ) بل ليُكشَفَ موقعٌ **غيرُ عدديٍّ** (نصٌّ) يبلغُه: كان
                    //      المضمِّنُ يستبدلُه في جسمٍ منمَّطٍ double فينفجرُ LLVM
                    //      «Invalid cast» — شقيقُ t06 المقيسُ (درسُ «الرقعةُ تُسدُّ في
                    //      ملفٍّ وتتركُ أشقّاءَه»).
                    // (EN) A declared «عشري» (Float) is recorded too — not for numeric
                    //      promotion (its contract stands) but to detect a NON-numeric
                    //      site (a string) reaching it: the inliner substituted it into
                    //      a double-typed body and LLVM asserted «Invalid cast» — the
                    //      measured sibling of t06 (the "patch sealed in one file
                    //      leaves siblings" lesson).
                    const bool isDeclaredFloat = param.type == Types::SadTypeKind::Float;
                    if (!isUndeclared && !isDeclaredInteger && !isDeclaredFloat)
                        continue;

                    // (AR) خانةٌ لم يبلغْها وسيطٌ في هذا الموقع: قيمتُها **عدم** بنصِّ
                    //      المادّةِ (٣)، فهي مخالِفةٌ لا غائبة. وإغفالُها عن التسجيلِ
                    //      يجعلُ موقعًا واحدًا «إجماعًا» فتُرقّى الخانةُ إلى نصٍّ مثلًا،
                    //      ثمّ يُبطِّنُها الموقعُ المُغفِلُ بصفرِ النوعِ لا بعدم — فيتباعدُ
                    //      المحرّكانِ في `س() == لاشيء`. فالعدمُ يُسجَّلُ صراحةً ليمنع.
                    // (EN) A slot no argument reached at this site is **null** by the letter of
                    //      Article (3) — it disagrees, it is not absent. Leaving it unrecorded
                    //      makes a single site a "unanimity", the slot is promoted to e.g.
                    //      String, and the omitting site then pads it with the type's zero
                    //      rather than null — so the two engines diverge on `س() == لاشيء`.
                    //      Null is therefore recorded explicitly, so that it blocks.
                    if (index >= arguments.size())
                    {
                        // (AR) الإغفالُ مشروعٌ في البانِي وحدَه (المادّة ٣)، وهناكَ يُسجَّلُ
                        //      **عدمًا** مخالِفًا يمنعُ الإجماع. أمّا الطريقةُ فنداؤُها الناقصُ
                        //      مرفوضٌ أصلًا (RUN030)، فلا يجوزُ لموقعٍ لا يُنفَّذُ قانونًا أن
                        //      يمنعَ ترقيةً لموقعٍ يُنفَّذ.
                        // (EN) Omission is lawful in a constructor alone (Article 3), and there it
                        //      is recorded as a disagreeing **null** that blocks unanimity. A
                        //      method's short call is rejected outright (RUN030), so a site that
                        //      can never lawfully run must not block a promotion for one that can.
                        if (paramsBelongToConstructor)
                        {
                            scanMemberArgKinds_[&param].insert(static_cast<int>(SadTypeKind::Void));
                        }
                        continue;
                    }

                    scanMemberArgKinds_[&param].insert(
                        static_cast<int>(inferExprType(arguments[index].get())));
                }
            }

            void TemplateBuilder::applyAgreedMemberParamTypes()
            {
                for (auto &[param, kinds] : scanMemberArgKinds_)
                {
                    // (AR) اتّفاقٌ تامٌّ أو لا ترقية. واختلافُ نوعٍ واحدٍ — ولو كان عدمًا —
                    //      يكفي لإبقاءِ الخانةِ على عمومِها، وهو الجانبُ الآمن.
                    // (EN) Unanimity or no promotion. A single disagreeing kind — null included
                    //      — is enough to leave the slot general, which is the safe side.
                    if (kinds.size() != 1)
                    {
                        // (AR) استثناءُ الاختلافِ العدديِّ الخالص: خانةٌ مصرَّحةٌ «رقم»
                        //      بلغَها صحيحٌ **وعشريٌّ** معًا لا غير. تركُها «رقمًا» يجعل
                        //      الموقعَ العشريَّ يبتُر (بذرة [٧] المختلطة)، وتثبيتُها عشريًّا
                        //      يُحرّف الموقعَ الصحيحَ (3 ⇒ 3.0) — بل قِيسَ أسوأُ من ذلك:
                        //      خانةُ بانٍ تبِعت موقعَها الأوّلَ الصحيحَ فكتبَ الموقعُ العشريُّ
                        //      بتاتِ double في i64 وقُرِئت قمامةً صامتة. والمفسّرُ (المرجع)
                        //      يُمرّر كلَّ موقعٍ بوسمِه — فالخانةُ تعمُّ إلى Any لتُقرأ
                        //      موسومةً زمنَ التشغيل، نظيرَ نقضِ الدوالِّ الحرّةِ في
                        //      applyAgreedFreeParamTypes (ولذلك المسارُ الحرُّ أخضرُ أصلًا).
                        //      وأيُّ مخالفٍ غيرِ عدديٍّ — عدمُ الإغفالِ أو نصٌّ — يُبقي
                        //      البابَ مغلقًا كما كان: الجانبُ الآمن.
                        // (EN) The pure-numeric disagreement exception: a declared-«رقم» slot
                        //      whose sites carried Integer AND Float and nothing else. Leaving
                        //      it Integer truncates the float site (mixed seed [٧]); pinning it
                        //      Float distorts the integer site (3 ⇒ 3.0) — and worse was
                        //      measured: a constructor slot followed its first (int) site and
                        //      the float site wrote double BITS into an i64, read back as
                        //      silent garbage. The interpreter (the reference) passes each
                        //      site through with its own tag — so the slot generalizes to Any
                        //      and is read runtime-tagged, mirroring the free-function
                        //      revocation in applyAgreedFreeParamTypes (which is why the free
                        //      path is already green). Any non-numeric disagreeing kind —
                        //      an omission's null, a string — keeps the door shut as before:
                        //      the safe side.
                        const bool purelyNumericSplit =
                            param->type == Types::SadTypeKind::Integer &&
                            kinds.size() == 2 &&
                            kinds.count(static_cast<int>(SadTypeKind::Integer)) &&
                            kinds.count(static_cast<int>(SadTypeKind::Float));
                        // (AR) توسيعُ البابِ المقيس: خانةٌ مصرَّحةٌ «رقم» بلغَها خليطٌ
                        //      يشملُ غيرَ العدديِّ (نصٌّ مثلًا) — المفسّرُ (المرجعُ) يحذّرُ
                        //      ويُمرِّرُ كلَّ موقعٍ بوسمِه، بينما كان المضمِّنُ الأماميُّ
                        //      يستبدلُ الثابتَ النصّيَّ في جسمٍ منمَّطٍ i64 فينفجرُ LLVM
                        //      «Invalid cast» (ICE مقيس — t06). التعميمُ إلى Any يُطابقُ
                        //      المفسّرَ، والمضمِّنُ يرفضُ الحدودَ الموسومةَ أصلًا فلا ICE.
                        //      وعدمُ الإغفالِ (Void — بابُ الباني وحدَه) يبقى مانعًا كما
                        //      كان: دلالةُ تبطينِه بالعدمِ لا تُمَسّ.
                        // (EN) Widen the measured door: a declared-«رقم» slot whose sites
                        //      mix in a non-numeric kind (e.g. String) — the interpreter
                        //      (the reference) warns and passes each site through with its
                        //      own tag, while the frontend inliner substituted the string
                        //      constant into an i64-typed body and LLVM asserted
                        //      «Invalid cast» (measured ICE — t06). Generalizing to Any
                        //      matches the interpreter, and the inliner already refuses
                        //      tagged boundaries, so no ICE. An omission's Void (the
                        //      constructor-only door) keeps blocking: its null-padding
                        //      semantics are untouched.
                        const bool declaredNumberMixed =
                            param->type == Types::SadTypeKind::Integer &&
                            kinds.count(static_cast<int>(SadTypeKind::Void)) == 0;
                        // (AR) والمصرَّحُ «عشري» يعمُّ فقط إذا خالطَه **غيرُ عدديٍّ**
                        //      (نصٌّ مثلًا): الخليطُ العدديُّ الخالصُ {صحيح، عشريّ} يبقى
                        //      على عقدِه double (التكييفُ sitofp قائمٌ ومقيس) — التعميمُ
                        //      هنا يسدُّ ICE المضمِّنِ لا يغيّرُ الدلالةَ العدديّة.
                        // (EN) A declared «عشري» generalizes ONLY when a NON-numeric
                        //      kind mixes in (e.g. String): a purely numeric
                        //      {Int, Float} mix keeps its double contract (the sitofp
                        //      coercion stands, measured) — the generalization here
                        //      seals the inliner ICE without changing numeric
                        //      semantics.
                        bool declaredFloatNonNumericMixed = false;
                        if (param->type == Types::SadTypeKind::Float &&
                            kinds.count(static_cast<int>(SadTypeKind::Void)) == 0)
                        {
                            for (const int kd : kinds)
                            {
                                if (kd != static_cast<int>(SadTypeKind::Integer) &&
                                    kd != static_cast<int>(SadTypeKind::Float))
                                {
                                    declaredFloatNonNumericMixed = true;
                                    break;
                                }
                            }
                        }
                        if (!purelyNumericSplit && !declaredNumberMixed &&
                            !declaredFloatNonNumericMixed)
                            continue;
                        param->type = SadTypeKind::Any;
                        param->sadType = Types::SadType::fromValueType(SadTypeKind::Any);
                        continue;
                    }

                    const auto agreed = static_cast<SadTypeKind>(*kinds.begin());
                    // (AR) الخانةُ المصرَّحةُ «رقم» (سُجِّلت باستثناءِ بذرة [٧] أعلاه):
                    //      توسيعُها **عشريٌّ حصرًا** — التوسيعُ العدديُّ الذي يقبله
                    //      المفسّرُ — ولا يُكتَبُ فوقَ تصريحِ الكاتبِ نصٌّ أو منطقيّ.
                    // (EN) A declared-«رقم» slot (recorded via the seed-[٧] exception above)
                    //      widens to Float ONLY — the numeric widening the interpreter
                    //      accepts; String/Boolean never overwrite the author's declaration.
                    if (param->type == Types::SadTypeKind::Integer)
                    {
                        // (AR) إجماعٌ غيرُ عدديٍّ على خانةٍ مصرَّحةٍ «رقم» (نصٌّ وحيدُ
                        //      المواقعِ مثلًا): لا يُكتَبُ فوقَ التصريحِ نصًّا بل تعمُّ
                        //      الخانةُ إلى Any — نظيرُ ذراعِ الخليطِ أعلاه وبالقياسِ
                        //      نفسِه (ICE المضمِّن t06a)؛ والعدمُ يبقى مانعًا.
                        // (EN) A non-numeric unanimity on a declared-«رقم» slot (e.g. a
                        //      single all-String site): never overwrite the declaration
                        //      with String — generalize to Any, mirroring the mixed arm
                        //      above under the same measurement (inliner ICE t06a);
                        //      Void keeps blocking.
                        if (agreed != SadTypeKind::Float)
                        {
                            if (agreed != SadTypeKind::Integer &&
                                agreed != SadTypeKind::Void)
                            {
                                param->type = SadTypeKind::Any;
                                param->sadType =
                                    Types::SadType::fromValueType(SadTypeKind::Any);
                            }
                            continue;
                        }
                    }
                    else if (param->type == Types::SadTypeKind::Float)
                    {
                        // (AR) إجماعٌ غيرُ عدديٍّ على خانةٍ مصرَّحةٍ «عشري» (نصٌّ وحيدُ
                        //      المواقعِ): تعمُّ Any — لا يُكتَبُ نصٌّ فوقَ عقدِ الكاتبِ
                        //      ولا يُترَكُ ICE المضمِّنِ حيًّا. والعدديُّ الخالصُ
                        //      (صحيحٌ وحيدُ المواقعِ) يبقى على العقدِ double.
                        // (EN) A non-numeric unanimity on a declared-«عشري» slot (an
                        //      all-String site): generalize to Any — never overwrite
                        //      the author's contract with String, never leave the
                        //      inliner ICE alive. A purely numeric unanimity (an
                        //      all-Integer site) keeps the double contract.
                        if (agreed != SadTypeKind::Integer &&
                            agreed != SadTypeKind::Float &&
                            agreed != SadTypeKind::Void)
                        {
                            param->type = SadTypeKind::Any;
                            param->sadType =
                                Types::SadType::fromValueType(SadTypeKind::Any);
                        }
                        continue;
                    }
                    else if (agreed != SadTypeKind::String &&
                             agreed != SadTypeKind::Float &&
                             agreed != SadTypeKind::Boolean)
                        continue;

                    param->type = agreed;
                    param->sadType = Types::SadType::fromValueType(agreed);
                }
            }

            // ========================================================================
            // (AR) إجماعُ مواقعِ النداءِ للدوالِّ الحرّة — نظيرُ applyAgreedMemberParamTypes.
            //      لا نُعيد بناءَ الترقيةِ (فروعُها كثيرةٌ ومختبَرة)، بل نَنقُضها حيث بُنيت
            //      على موقعٍ واحدٍ خالفَه غيرُه: تعودُ الخانةُ عامّةً (Any) فتُقرأ موسومةً
            //      زمنَ التشغيل كما يفعل المفسّرُ (المرجع).
            //      ويقتصر النقضُ على الأنواعِ «الحادّة» — نصّ/عشريّ/منطقيّ/مصفوفة — التي
            //      يُفسِد تثبيتُها الخاطئُ القراءةَ. والصحيحُ هو الافتراضيُّ غيرُ المُرقَّى
            //      فلا يُنقَض هنا كي لا تُعلَّب خاناتٌ لا حاجةَ بها إلى التعليب.
            //      ولا يُمَسّ معامِلٌ صرّح الكاتبُ نوعَه: التصريحُ عقدٌ لا يُخمَّن فوقه.
            // (EN) Call-site unanimity for free functions — the counterpart of
            //      applyAgreedMemberParamTypes. Rather than rebuilding the promotion (whose
            //      branches are many and well-tested), we revoke it where it rested on a
            //      single site that other sites contradict: the slot returns to Any and is
            //      read runtime-tagged, as the interpreter (the reference) does.
            //      Revocation is limited to the "sharp" kinds — String/Float/Boolean/Array —
            //      whose wrong pinning corrupts the read. Integer is the unpromoted default
            //      and is left alone so slots that need no boxing are not boxed. A parameter
            //      whose type the author declared is never touched.
            // ========================================================================
            void TemplateBuilder::applyAgreedFreeParamTypes()
            {
                // (AR) نقضُ الربطِ بالصنفِ أوّلًا: خانةٌ اختلفت أصنافُ وسائطِها تفقده.
                // (EN) Revoke class bindings first: a slot whose sites disagree loses it.
                for (const auto &[funcName, slots] : scanFreeArgClasses_)
                {
                    auto fnIt = b_.functionTable_.find(funcName);
                    if (fnIt == b_.functionTable_.end())
                        continue;
                    auto pctIt = b_.paramClassTypes_.find(funcName);
                    if (pctIt == b_.paramClassTypes_.end())
                        continue;
                    for (const auto &[index, classes] : slots)
                    {
                        if (classes.size() < 2 || index >= fnIt->second.parameters.size())
                            continue;
                        pctIt->second.erase(fnIt->second.parameters[index].name);
                    }
                }

                for (const auto &[funcName, slots] : scanFreeArgKinds_)
                {
                    auto fnIt = b_.functionTable_.find(funcName);
                    if (fnIt == b_.functionTable_.end())
                        continue;
                    const Sad::AST::FunctionDecl *decl = fnIt->second.astDecl;

                    for (const auto &[index, kinds] : slots)
                    {
                        if (kinds.size() < 2)
                            continue;
                        if (index >= fnIt->second.parameters.size())
                            continue;

                        // (AR) المعامِلُ غيرُ المُصرَّحِ يبلغُ الشجرةَ بـ«مجهول» أو بـ«صنف»
                        //      واسمِ نوعٍ فارغ — كما هو مقيسٌ في recordMemberParamArgs.
                        // (EN) An undeclared parameter reaches the AST as Unknown, or as Class
                        //      with an empty type name — as measured in recordMemberParamArgs.
                        if (decl && index < decl->parameters.size())
                        {
                            const auto &p = decl->parameters[index];
                            const bool isUndeclared =
                                p.type == Types::SadTypeKind::Unknown ||
                                (p.type == Types::SadTypeKind::Class && p.typeName.empty());
                            if (!isUndeclared)
                                continue;
                        }

                        // (AR) و«صحيح» يُنقَض كذلك. كان مستثنًى لأنّه الافتراضيُّ غيرُ
                        //      المُرقَّى فبدا نقضُه تعليبًا بلا داعٍ — لكنّ القياسَ نقضَ
                        //      الظنَّ: خانةٌ بقيت «صحيحًا» ووصلَها كائنٌ تُقارَن **مؤشِّرَين
                        //      عدديًّا**، فتُجيب «مختلفان» حيث يقول المفسّرُ «متساويان»
                        //      بلا رمزِ خطأٍ ولا رسالة — جوابٌ خاطئٌ صامتٌ يمرّ أخضرَ،
                        //      وهو أخطرُ من الانهيارِ الذي سبقَ سدُّه.
                        //      والشرطُ نفسُه يحمي من التعليبِ العابث: لا يُنقَض إلّا ما
                        //      **اختلفت** مواقعُه فعلًا (kinds.size() >= 2).
                        // (EN) Integer is revoked too. It was excluded as the unpromoted
                        //      default — revoking it looked like needless boxing — but the
                        //      measurement refuted that: a slot left Integer that receives an
                        //      object compares two POINTERS numerically, answering "different"
                        //      where the interpreter says "equal", with no error code and no
                        //      message — a silent wrong answer that passes green, worse than
                        //      the crash already sealed. The disagreement gate still applies:
                        //      only slots whose sites actually differ are revoked.
                        // ════════════════════════════════════════════════════════
                        // (AR) 🔑 **نفيٌ لا إثبات**: يُعمَّمُ كلُّ وسمٍ إلّا ما لا
                        //      معنى لتعميمِه.
                        //
                        //      كان هنا سردٌ إيجابيٌّ يتّسعُ وسمًا بوسم، وكلُّ وسمٍ
                        //      غائبٍ عنه عطبٌ صامتٌ ينتظرُ مَن يقعُ فيه. وقد سقطَ
                        //      منه `Null` و`Class`، وأعراضُهما مقيسة:
                        //
                        //        دالة ض(أ، ب) ارجع أ + ب نهاية
                        //        ض("أ:"، لاشيء)  و  ض(1، 2)
                        //          ⇒ التوقيعُ المُخرَجُ `@ض(%SadDyn, i64)` — **ب لم
                        //            يُعمَّمْ قطُّ** — فيُعرَضُ العدمُ حارسَه عددًا:
                        //            `أ:-9223372036854775807` بدل `أ:لاشيء`.
                        //
                        //        اعرض(5)  و  اعرض(كلب())
                        //          ⇒ مؤشّرٌ يُطبَعُ عددًا، رمزُ خروجٍ صفر.
                        //
                        //      والبوّابةُ فوقَه (`kinds.size() >= 2`) تعني أنّ المواقعَ
                        //      **اختلفت فعلًا**، فلا وسمَ ساكنًا واحدًا يصدُقُ عليها.
                        //      وهذا نصُّ العقدِ في `types.yaml` عند `type.null`:
                        //      «الوسمُ الساكنُ لا يكفي وحدَه ولا يمكنُ أن يكفي… والسؤالُ
                        //      نفسُه سؤالُ زمنِ تشغيل».
                        //
                        //      فالسقوطُ يجبُ أن يكونَ إلى **الأسلمِ لا إلى الأسرع**:
                        //      وسمٌ منسيٌّ يصيرُ تعليبًا زائدًا (بطيءٌ وصحيح) لا خانةً
                        //      ساكنةً كاذبة (سريعٌ وخاطئ).
                        // (EN) 🔑 A negative list, not a positive one: widen every kind
                        //      except those there is no meaning in widening.
                        //
                        //      What stood here was a positive list that grew one kind at a
                        //      time, and every kind missing from it was a silent defect
                        //      waiting for someone to hit it. `Null` and `Class` had both
                        //      fallen out, and their symptoms are measured: the emitted
                        //      signature stayed `i64` for a slot receiving null at one site
                        //      and a number at another, printing the null sentinel as a
                        //      number; and a slot receiving a class at one site printed a
                        //      pointer as a number with exit code 0.
                        //      The gate above means the sites genuinely disagree, so no single
                        //      static tag is true of them — which is exactly what types.yaml
                        //      says about `type.null`. The fallback must therefore be to the
                        //      SAFER option, not the faster one: a forgotten kind becomes
                        //      needless boxing (slow and right) rather than a lying static
                        //      slot (fast and wrong).
                        // ════════════════════════════════════════════════════════
                        SadTypeKind &slot = fnIt->second.parameters[index].type;

                        // (AR) لا معنى للتعميم: `أي` مُعمَّمٌ سلفًا، و`مجهول` لا معلومةَ
                        //      فيه يُعمَّمُ عنها، و`فراغ` ليس نوعَ خانةٍ أصلًا (مرفوضٌ
                        //      بـSEM040 في المحلّلِ المشترك).
                        // (EN) No meaning in widening: Any is already dynamic, Unknown has no
                        //      information to widen from, and Void is not a slot type at all
                        //      (rejected by SEM040 in the shared parser).
                        const bool widenHasNoMeaning = (slot == SadTypeKind::Any ||
                                                        slot == SadTypeKind::Unknown ||
                                                        slot == SadTypeKind::Void);

                        // ⚠️ (AR) استثناءٌ ثانٍ **بشرطِ خروجٍ جديدٍ مقيس**. وشرطُه
                        //      الأوّلُ سُدَّ فعلًا: كان تعميمُ الصنفِ يبلغُ الإرسالَ
                        //      الديناميَّ فيُجهِضُ `verifyModule` برسالةِ مدقّقِ LLVM
                        //      إنجليزيّةً خامّة، وقد وُحِّدَ فَكُّ المُستقبِلِ في
                        //      `objectPointerOperand` فزالَ الإجهاض.
                        //
                        //      🔑 غير أنّ رفعَه اليومَ يُنتِجُ **رفضًا كاذبًا**، وذلك
                        //      قياسٌ لا ظنّ: وسمُ الكائنِ لا ينجو من التعليب — يُنزِلُ
                        //      `astTypeToSIRType` الصنفَ إلى مقبضِ i64 فيُعلَّبُ `Int`،
                        //      فخانةٌ عُمِّمَت إلى `أي` يصلُها كائنٌ يرفعُ حارسُ
                        //      المُستقبِلِ عليها RUN033 «نوع المعامل INTEGER» وهو
                        //      كائنٌ سليم. والرفضُ الكاذبُ أسوأُ ممّا يحلُّ محلَّه.
                        //
                        //      ⚠️ ولا يُسَدُّ ذلك برفعِ الوسمِ من `objectClassMap`:
                        //      جُرِّبَ فقِيسَ أنّ خريطةً مفتاحُها اسمٌ بلا عزلِ كتلٍ
                        //      تجعلُ العددَ ٥ يُوزَّعُ كائنَ صنفٍ (`مواء` · rc=0)
                        //      والكتابةَ عبرَه `store` على `inttoptr(7)` (rc=139) —
                        //      ISSUE-195، وحارسُه بذرتا `gr.oop.method/edge/058`
                        //      و`gr.oop.field/edge/054`.
                        //
                        //      🔑 وهذا الاستثناءُ **لا أثرَ له اليومَ**: بُنيَ المترجِمُ
                        //      بالحظرِ وبلا الحظرِ فتطابقَ المخرَجان، لأنّ
                        //      `inferExprType` تُصنِّفُ نداءَ البانِي والمتغيّرَ
                        //      المُصرَّحَ بصنفِه **صحيحًا** فلا يبلغُ `slot` قيمةَ
                        //      `Class` أبدًا (ISSUE-193). فهو **مِرصادٌ لا حارسٌ عامل**:
                        //      يُبقيه أنّ سدَّ ISSUE-193 وحدَه — بلا ISSUE-195 — يُحوِّلُ
                        //      شرطَه من ميّتٍ إلى حيّ. ⚠️ وأنّه يمنعُ ساعتَئذٍ رفضًا كاذبًا
                        //      **توقّعٌ لا قياس**، ولا سبيلَ إلى قياسِه قبلَ سدِّ ١٩٣؛
                        //      فليُقَسْ عندَها ولا يُؤخَذْ من هنا مأخذَ المقيس.
                        //      **شرطُ رفعِه**: سدُّ ISSUE-195 (برهانُ الكائنِ محمولٌ على
                        //      المُعامِلِ لا مبحوثٌ عنه باسمِه).
                        // ⚠️ (EN) A second exception with a NEW, measured exit condition. Its
                        //      first condition is genuinely sealed — the verifyModule abort is
                        //      gone now that the receiver is unwrapped through
                        //      objectPointerOperand. But lifting it today produces a FALSE
                        //      REJECTION: the object tag does not survive boxing (Class lowers
                        //      to an i64 handle and is packed as Int), so a slot widened to
                        //      «أي» that receives an object raises RUN033 «operand type
                        //      INTEGER» on a valid object — worse than what it replaces.
                        //      And that is NOT fixed by lifting the tag from objectClassMap:
                        //      measured, a name-keyed map with no block isolation dispatches
                        //      the integer 5 as a class object (rc=0) and stores through
                        //      inttoptr(7) (rc=139) — ISSUE-195, guarded by two seeds.
                        //      This exception has no effect today (the compiler was built with
                        //      and without it and both outputs matched, because inferExprType
                        //      never yields Class — ISSUE-193). It is kept as a TRIPWIRE, not a
                        //      working guard: sealing ISSUE-193 alone, without ISSUE-195, would
                        //      make its condition live and it would then prevent the false
                        //      rejection. Lift it only once ISSUE-195 is sealed.
                        const bool classWideningBlocked = (slot == SadTypeKind::Class ||
                                                           slot == SadTypeKind::Struct);

                        if (!widenHasNoMeaning && !classWideningBlocked)
                        {
                            slot = SadTypeKind::Any;
                        }
                    }
                }
            }

            // ========================================================================
            // (AR) [موجة الجسر الموسوم] الدالّةُ الهاربةُ مرجعًا إلى دالّةِ مستخدمٍ:
            //      مواقعُ ندائِها عبرَ المرجعِ غيرُ مرئيّةٍ للمسحِ، فإجماعُ المواقعِ
            //      المرئيّةِ ادّعاءٌ ناقصٌ. تُوسَّعُ خاناتُها **الرقميّةُ الصحيحةُ
            //      والعشريّةُ وغيرُ المصرَّحةُ** إلى Any فتُقرأُ موسومةً زمنَ التشغيل
            //      (جسرُ %SadDyn يفكُّها بوسمِها) — القياس: «نفذ(ثلاثي)» ثم
            //      «ارجع د(2.5)» بترَ العشريَّ إلى 2 داخلَ الجسمِ الموقَّعِ i64
            //      (t01: المفسّرُ 7.5). والمصرَّحُ الحادُّ (نصٌّ/منطقيٌّ/مصفوفة)
            //      عقدُ الكاتبِ لا يُمَسّ — يفكُّه الجسرُ إلى نوعِه مباشرةً.
            // (EN) [Tagged-bridge wave] A function escaping as a reference into a
            //      user function: its call sites through the reference are invisible
            //      to the scan, so visible-site unanimity is an incomplete claim.
            //      Its Integer/Float/undeclared slots widen to Any and read
            //      runtime-tagged (the %SadDyn bridge unboxes by tag) — measured:
            //      «نفذ(ثلاثي)» then «ارجع د(2.5)» truncated the float to 2 inside
            //      the i64-typed body (t01: interpreter says 7.5). Sharp declared
            //      kinds (String/Boolean/Array) are the author's contract and stay —
            //      the bridge unboxes straight to them.
            // ========================================================================
            void TemplateBuilder::applyEscapedFuncParamWidening()
            {
                for (const auto &escName : b_.scanEscapedFuncs_)
                {
                    auto fnIt = b_.functionTable_.find(escName);
                    if (fnIt == b_.functionTable_.end())
                        continue;
                    const Sad::AST::FunctionDecl *decl = fnIt->second.astDecl;
                    for (size_t index = 0; index < fnIt->second.parameters.size(); ++index)
                    {
                        bool widenable = true;
                        if (decl && index < decl->parameters.size())
                        {
                            const auto &p = decl->parameters[index];
                            const bool isUndeclared =
                                p.type == Types::SadTypeKind::Unknown ||
                                (p.type == Types::SadTypeKind::Class && p.typeName.empty());
                            const bool isDeclaredNumeric =
                                p.type == Types::SadTypeKind::Integer ||
                                p.type == Types::SadTypeKind::Float;
                            widenable = isUndeclared || isDeclaredNumeric;
                        }
                        if (!widenable)
                            continue;
                        SadTypeKind &slot = fnIt->second.parameters[index].type;
                        if (slot == SadTypeKind::Integer || slot == SadTypeKind::Float ||
                            slot == SadTypeKind::Unknown)
                        {
                            slot = SadTypeKind::Any;
                        }
                    }
                }
            }

            void TemplateBuilder::refineCalledMember(
                const Sad::AST::Expression *objectExpr,
                const std::string &methodName,
                const std::vector<std::unique_ptr<Sad::AST::Expr>> &arguments)
            {
                Sad::AST::ClassDecl *owner = nullptr;

                // (AR) الكائنُ إنشاءٌ مباشرٌ «صنف().طريقة(...)» ⇒ المالكُ معلومٌ يقينًا.
                // (EN) The object is a direct instantiation ⇒ the owner is known for certain.
                // (AR) والمُستقبِلُ «هذا» مالكُه الصنفُ المحيطُ الذي نمسحُ عضوَه الآن —
                //      يقينٌ لا تخمين. وتركُه بلا حسمٍ ليسَ «امتناعًا عن الترقية» بل خرجٌ
                //      فاسدٌ أو إجهاضُ المصرِّفِ برسالةِ LLVM بلا تشخيصِ لغة.
                // (EN) The receiver `this` is owned by the enclosing class whose member we are
                //      scanning — certainty, not a guess. Leaving it unresolved is not "no
                //      promotion" but corrupt output, or an LLVM-level compiler abort with no
                //      language diagnostic.
                if (dynamic_cast<const Sad::AST::ThisExpr *>(objectExpr))
                {
                    owner = scanEnclosingClass_;
                }
                else
                {
                    owner = resolveExprClass(objectExpr);
                }

                // (AR) ولا يُلجَأُ إلى اسمِ الطريقةِ وحدَه البتّة: مُستقبِلٌ مدمَجٌ يحملُ
                //      اسمَ الطريقةِ نفسَه (`مصفوفة.أضف`) يُنسَبُ حينَها إلى صنفٍ لا صلةَ
                //      له به، فيُسجَّلُ نوعُ وسيطٍ أجنبيٍّ في خانتِه فيكسِرُ إجماعًا صحيحًا
                //      — أو يُرقّيها خطأً. والمالكُ الذي لا يُحسَمُ يُترَكُ بلا ترقية.
                // (EN) Never by method name alone: a builtin receiver carrying the same method
                //      name (`array.add`) would be attributed to an unrelated class, so a
                //      foreign argument type is recorded in its slot — breaking a correct
                //      unanimity, or promoting it wrongly. An owner that cannot be resolved is
                //      left unpromoted.
                if (!owner)
                    return;

                // (AR) والطريقةُ تُطلَبُ في الصنفِ ثمّ في سلسلةِ أساسِه: الوراثةُ تجعلُ
                //      المالكَ المُعلَنَ غيرَ المُعرِّف. والوقوفُ عندَ المستوى الواحدِ ليسَ
                //      «ترقيةً فائتةً»: خانةُ المعامِلِ تبقى صحيحًا بينما موقعُ النداءِ
                //      يُمرِّرُ عشريًّا، فيسقطُ المصرِّفُ في LLVM ويعلَق بلا تشخيصِ لغة.
                // (EN) The method is looked up in the class then up its base chain: inheritance
                //      makes the declared owner not the definer. Stopping at one level is not a
                //      "missed promotion": the parameter slot stays integer while the call site
                //      passes a float, so the compiler asserts inside LLVM and hangs, with no
                //      language diagnostic.
                if (auto *method = findMethodInHierarchy(owner, methodName))
                    recordMemberParamArgs(method->parameters, arguments, false);
            }

            // (AR) فكُّ غلافِ التصدير: «صدّر صنف …» عقدةُ تصديرٍ تحوي الصنف، ومقارنةُ
            //      النوعِ عاريةً تتخطّاه بالكلّيّة — فلا يُفهرَسُ ولا تُمسَحُ أعضاؤه.
            // (EN) Unwrap the export shell: `export class ...` is an export node wrapping the
            //      class, and a bare type test skips it entirely — so it is neither indexed
            //      nor are its members scanned.
            Sad::AST::ClassDecl *TemplateBuilder::asClassDecl(Sad::AST::Statement *stmt)
            {
                if (!stmt)
                    return nullptr;
                if (auto *classDecl = dynamic_cast<Sad::AST::ClassDecl *>(stmt))
                    return classDecl;
                if (auto *exportDecl = dynamic_cast<Sad::AST::ExportDecl *>(stmt))
                    return dynamic_cast<Sad::AST::ClassDecl *>(exportDecl->declaration.get());
                if (auto *exportStmt = dynamic_cast<Sad::AST::ExportStmt *>(stmt))
                    return dynamic_cast<Sad::AST::ClassDecl *>(exportStmt->declaration.get());
                return nullptr;
            }

            // (AR) طلبُ الطريقةِ في الصنفِ ثمّ في أصنافِه الأساسيّةِ بالعرضِ أوّلًا —
            //      كترتيبِ التوزيعِ نفسِه. والمزوراتُ تمنعُ اللانهايةَ في وراثةٍ دائريّةٍ
            //      أو ماسٍ متعدّد: عطبٌ في المصدرِ لا يجوزُ أن يصيرَ تعليقًا في المصرِّف.
            // (EN) Look the method up in the class then in its base classes, breadth-first —
            //      the same order dispatch uses. The visited set prevents non-termination on
            //      cyclic or diamond inheritance: a defect in the source must never become a
            //      compiler hang.
            Sad::AST::MethodDecl *TemplateBuilder::findMethodInHierarchy(Sad::AST::ClassDecl *owner,
                                                                        const std::string &methodName)
            {
                std::set<const Sad::AST::ClassDecl *> visitedClasses;
                std::vector<Sad::AST::ClassDecl *> pendingClasses{owner};

                while (!pendingClasses.empty())
                {
                    Sad::AST::ClassDecl *currentClass = pendingClasses.front();
                    pendingClasses.erase(pendingClasses.begin());
                    if (!currentClass || !visitedClasses.insert(currentClass).second)
                        continue;

                    for (const auto &member : currentClass->members)
                    {
                        auto *method = dynamic_cast<Sad::AST::MethodDecl *>(member.get());
                        if (method && method->name == methodName)
                            return method;
                    }

                    for (const auto &baseName : currentClass->superclasses)
                    {
                        auto it = scanClassByName_.find(baseName);
                        if (it != scanClassByName_.end())
                            pendingClasses.push_back(it->second);
                    }
                }

                return nullptr;
            }

            // (AR) حسمُ صنفِ تعبيرٍ ساكنًا — يقينًا لا تخمينًا. تُجمَعُ هنا الصورُ الثلاثُ
            //      التي كانت متناثرةً: إنشاءٌ مباشر، ونداءُ صنفٍ باسمِه، ومتغيّرٌ مربوطٌ.
            // (EN) Resolve an expression's class statically — with certainty, not by guessing.
            //      The three forms that were scattered are gathered here: direct instantiation,
            //      a call naming a class, and a bound variable.
            Sad::AST::ClassDecl *TemplateBuilder::resolveExprClass(const Sad::AST::Expression *expr)
            {
                if (!expr)
                    return nullptr;

                if (auto *newExpr = dynamic_cast<const Sad::AST::NewExpr *>(expr))
                {
                    auto it = scanClassByName_.find(newExpr->className);
                    return it != scanClassByName_.end() ? it->second : nullptr;
                }
                if (auto *callExpr = dynamic_cast<const Sad::AST::CallExpr *>(expr))
                {
                    if (auto *calleeName =
                            dynamic_cast<const Sad::AST::VariableExpr *>(callExpr->callee.get()))
                    {
                        auto it = scanClassByName_.find(calleeName->name);
                        if (it != scanClassByName_.end())
                            return it->second;
                    }
                    return nullptr;
                }
                if (auto *varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(expr))
                {
                    // (AR) المعامِلُ المربوطُ في نطاقِ دالّتِه يسبقُ الخريطةَ المسطَّحة: هو
                    //      أدقُّ منها، وقيمتُه العدميّةُ تعارُضٌ محسومٌ يمنعُ الترقيةَ ولا
                    //      يسمحُ بالسقوطِ إلى اسمٍ عارٍ يخصُّ دالّةً أخرى.
                    // (EN) A parameter bound within its own function outranks the flat map: it is
                    //      more precise, and a null value there is a settled conflict that blocks
                    //      promotion — it must not fall through to a bare name owned by another
                    //      function.
                    if (!b_.currentScanFuncName_.empty())
                    {
                        auto scoped = b_.paramClassBindings_.find(
                            {b_.currentScanFuncName_, varExpr->name});
                        if (scoped != b_.paramClassBindings_.end())
                        {
                            if (scoped->second.empty())
                                return nullptr;
                            auto named = scanClassByName_.find(scoped->second);
                            return named != scanClassByName_.end() ? named->second : nullptr;
                        }
                    }

                    auto it = scanClassOfVariable_.find(varExpr->name);
                    return it != scanClassOfVariable_.end() ? it->second : nullptr;
                }
                return nullptr;
            }

            // (AR) رَبطُ معامِلاتِ الدالّةِ الحرّةِ بأصنافِ وسائطِها من مواقعِ النداء.
            //
            //      كائنٌ يُمرَّرُ إلى دالّةٍ حرّةٍ ثمّ يُنادى عليه: `دالة ع(م) … م.اعرض(٢٫٥)`.
            //      كانَ مالكُ `م.اعرض` لا يُحسَمُ ساكنًا، فلا تُرقّى خانةُ معامِلِ الطريقة.
            //      وعاقبتُها ليست «ترقيةً فائتة»: المعامِلُ غيرُ المُصرَّحِ يُخفَضُ إلى
            //      **صحيح**، فتُمرَّرُ بتّاتُ العشريِّ عددًا صحيحًا ⇒ خرجٌ فاسدٌ بلا تشخيص
            //      (`اعرض:4612811918334230528`) بينما المفسّرُ يُصيب.
            //
            //      والمعلومةُ كانت حاضرةً في موقعِ النداءِ ولم تُقرَأ. والمسحُ ثلاثُ تمريراتٍ
            //      والخرائطُ لا تُمسَحُ بينَها، فالرَّبطُ هنا يبلغُ جسمَ الدالّةِ في التمريرةِ
            //      التالية. ومالكٌ لا يُحسَمُ يبقى بلا ترقيةٍ كما كان — لا يُخمَّن.
            // (EN) Bind a free function's parameters to the classes of its arguments at call sites.
            //
            //      An object passed to a free function and then called on: `func f(o) … o.show(2.5)`.
            //      The owner of `o.show` was not statically resolved, so the method's parameter slot
            //      was never promoted. That is not a "missed promotion": an undeclared parameter is
            //      lowered to **integer**, so a float is passed as its bits reinterpreted as an
            //      integer ⇒ corrupt output with no diagnostic, while the interpreter is correct.
            //
            //      The information was present at the call site and simply was not read. The scan
            //      runs three passes and these maps are not cleared between them, so a binding made
            //      here reaches the function body on the next pass. An owner that cannot be resolved
            //      stays unpromoted exactly as before — never guessed.
            void TemplateBuilder::bindFunctionParamsToArgumentClasses(
                const std::string &functionName,
                const std::vector<std::unique_ptr<Sad::AST::Expr>> &arguments)
            {
                if (functionName.empty())
                    return;

                auto functionIt = b_.functionTable_.find(functionName);
                if (functionIt == b_.functionTable_.end())
                    return;

                const auto &parameters = functionIt->second.parameters;
                const size_t boundCount = std::min(parameters.size(), arguments.size());

                for (size_t index = 0; index < boundCount; ++index)
                {
                    Sad::AST::ClassDecl *argumentClass = resolveExprClass(arguments[index].get());
                    if (!argumentClass)
                        continue;

                    const std::pair<std::string, std::string> key{functionName,
                                                                  parameters[index].name};
                    auto existing = b_.paramClassBindings_.find(key);
                    if (existing == b_.paramClassBindings_.end())
                    {
                        b_.paramClassBindings_[key] = argumentClass->name;
                    }
                    else if (existing->second != argumentClass->name)
                    {
                        // (AR) موقعانِ يُمرّرانِ صنفَينِ مختلفَينِ: يُحسَمُ تعارُضًا فلا تُرقّى
                        //      الخانة. وترجيحُ آخِرِ كاتبٍ كانَ سيُصرِّفُ نداءً إلى صنفٍ خاطئٍ
                        //      صامتًا — وهو التخمينُ بالاسمِ نفسُه عائدًا من مفتاحٍ آخر.
                        // (EN) Two sites pass different classes: settle it as a conflict so the slot
                        //      is not promoted. Last-writer-wins would dispatch a call to the wrong
                        //      class silently — the same name-guessing returning under another key.
                        existing->second.clear();
                    }
                }
            }

            void TemplateBuilder::scanCallSitesInExpr(const Sad::AST::Expression *expr)
            {
                if (!expr)
                    return;

                // (AR) [موجة ABI المغاليق] هدفُ إسنادٍ ⇒ تسميمُ الاسمِ لنزعِ الوساطة.
                //      لا يُنزَل في القيمةِ عمدًا: النداءاتُ داخلَ قيمةِ الإسنادِ خارجُ
                //      المسحِ اليومَ أصلًا (دَينٌ قائمٌ)، وضمُّها هنا يُبدِّلُ ترقياتٍ
                //      مستقرّةً في غيرِ موضعِ هذه الموجةِ — فيُترَكُ لموجتِه بقياسِه.
                // (EN) [Closure-ABI wave] An assignment target ⇒ poison the name for
                //      devirtualization. Deliberately does NOT recurse into the value:
                //      calls inside assignment values are outside the scan today (an
                //      existing debt), and pulling them in here would shift settled
                //      promotions far beyond this wave — left to its own measured wave.
                if (auto *assign = dynamic_cast<const Sad::AST::AssignExpr *>(expr))
                {
                    b_.scanAssignedNames_.insert(assign->name);
                    return;
                }

                // (AR) استدعاء دالة — استنتاج أنواع الوسائط وتحديث b_.functionTable_
                // (EN) Function call — infer arg types and update b_.functionTable_
                if (auto *call = dynamic_cast<const Sad::AST::CallExpr *>(expr))
                {
                    // (AR) استخراج اسم الدالة
                    // (EN) Extract function name
                    std::string funcName;
                    if (auto *varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(call->callee.get()))
                    {
                        funcName = varExpr->name;
                    }

                    // (AR) [موجة ABI المغاليق] حلُّ النداءِ غيرِ المباشرِ إلى دالّتِه:
                    //      اسمٌ ليس دالّةً لكنّه مربوطٌ ببرهانِ أصلٍ نظيفٍ ⇒ تُسجَّلُ
                    //      وسائطُه تحتَ الدالّةِ الحقيقيّةِ فيبلغُها توحيدُ int⊔float
                    //      وترقياتُ المعاملات. من التمريرةِ الثانيةِ فصاعدًا حصرًا:
                    //      التسميمُ (إسنادٌ لاحقٌ نصًّا، تظليلٌ) يكتملُ في الأولى، فلا
                    //      يُسجَّلُ موقعٌ عبرَ ربطٍ سيسقط.
                    // (EN) [Closure-ABI wave] Resolve an indirect call to its function: a
                    //      name that is not a function but carries clean provenance ⇒ its
                    //      arguments are recorded under the real function, so int⊔float
                    //      unification and param promotions reach it. Second pass onward
                    //      ONLY: poisoning (a textually later assignment, shadowing)
                    //      completes in the first pass, so no site is recorded through a
                    //      binding that is about to fall.
                    if (!funcName.empty() && b_.scanPassIndex_ >= 1 &&
                        b_.functionTable_.find(funcName) == b_.functionTable_.end() &&
                        b_.scanAssignedNames_.count(funcName) == 0)
                    {
                        // (AR) المفتاحُ المُنطاقُ أوّلًا؛ فإن لم يكن للاسمِ **تصريحٌ**
                        //      محلّيٌّ أصلًا سقطنا إلى مفتاحِ المستوى الأعلى — يحاكي
                        //      lookupVariable في البناءِ الذي يرى العامَّ عبرَ النطاقات.
                        //      ووجودُ تصريحٍ محلّيٍّ (ولو غيرَ مرجعيٍّ) يمنعُ السقوطَ:
                        //      الظلُّ المحلّيُّ يملك الاسم.
                        // (EN) Scoped key first; if the name has no LOCAL declaration at
                        //      all, fall back to the top-level key — mirroring the build's
                        //      lookupVariable, which sees globals through scopes. Any local
                        //      declaration (even a non-ref one) blocks the fallback: the
                        //      local shadow owns the name.
                        std::string bindKey = b_.currentScanFuncName_ + "#" + funcName;
                        if (b_.scanFuncRefBindings_.find(bindKey) ==
                                b_.scanFuncRefBindings_.end() &&
                            b_.scanFuncRefDeclNode_.find(bindKey) ==
                                b_.scanFuncRefDeclNode_.end())
                        {
                            bindKey = "#" + funcName;
                        }
                        if (b_.scanFuncRefPoisoned_.count(bindKey) == 0)
                        {
                            auto bindIt = b_.scanFuncRefBindings_.find(bindKey);
                            if (bindIt != b_.scanFuncRefBindings_.end())
                                funcName = bindIt->second;
                        }
                    }

                    bindFunctionParamsToArgumentClasses(funcName, call->arguments);

                    // (AR) [GAP 4] نداءُ متغيّرٍ مربوطٍ بلامدا: إن كان الوسيطُ مصفوفةً
                    //      مختلطةً/متجانسةً غيرَ صحيحةٍ (حرفيّةً أو متغيّرًا مُسجَّلًا في
                    //      scanVarElementType_)، فسجّل فهرسَ ذلك المعامل في scanLambdaParamAny_
                    //      كي يُوسَّع إلى Any في buildExprLambda قبل بناء الجسم. لا نُغيّر
                    //      نوعًا هنا (جسمُ اللامدا يُبنى في مرحلةٍ لاحقة). نُسجّل Any حصرًا
                    //      (كـGAP 3b) فآمنٌ عبر homogKind ولو شارك المعاملَ مواقعُ مختلفةٌ.
                    // (EN) [GAP 4] Call of a lambda-bound variable: if an argument is a mixed /
                    //      homogeneous-non-int array (literal, or a variable recorded in
                    //      scanVarElementType_), record that parameter index in
                    //      scanLambdaParamAny_ so buildExprLambda widens it to Any before
                    //      building the body. We only record here (the lambda body is built
                    //      in a later phase). Only Any is ever recorded (like GAP 3b), so it is
                    //      safe via homogKind even if the param is shared across call sites.
                    if (!funcName.empty())
                    {
                        auto lamIt = b_.scanLambdaVar_.find(b_.currentScanFuncName_ + "#" + funcName);
                        if (lamIt != b_.scanLambdaVar_.end())
                        {
                            const Sad::AST::LambdaExpr *lam = lamIt->second;
                            auto isScalarKind = [](SadTypeKind t) {
                                return t == SadTypeKind::Integer || t == SadTypeKind::Float ||
                                       t == SadTypeKind::String || t == SadTypeKind::Boolean ||
                                       t == SadTypeKind::UInt8 || t == SadTypeKind::UInt64 ||
                                       t == SadTypeKind::Null || t == SadTypeKind::Any;
                            };
                            // (AR) بوّابةُ حرفيّة المصفوفة: مختلطٌ قياسيّ (أو فيه Any) أو
                            //      متجانسٌ غيرُ صحيحٍ ⇒ يستحقّ التوسيع إلى Any.
                            // (EN) Array-literal gate: scalar-mixed (or containing Any) or
                            //      homogeneous-non-int ⇒ warrants widening to Any.
                            auto arrWantsAny = [&](const Sad::AST::ArrayExpr *arr) -> bool {
                                if (arr->elements.empty())
                                    return false;
                                SadTypeKind first = inferExprType(arr->elements[0].get());
                                bool homogeneous = true, allScalar = true, hasAny = false;
                                for (const auto &el : arr->elements)
                                {
                                    SadTypeKind et = inferExprType(el.get());
                                    if (et != first)
                                        homogeneous = false;
                                    if (!isScalarKind(et))
                                        allScalar = false;
                                    if (et == SadTypeKind::Any)
                                        hasAny = true;
                                }
                                return allScalar &&
                                       (((!homogeneous && arr->elements.size() > 1) || hasAny) ||
                                        (homogeneous && first != SadTypeKind::Integer));
                            };
                            // (AR) هل يستدعي هذا الوسيطُ توسيعَ معامله إلى Any؟
                            //      (متغيّرٌ مُسجَّل Any، أو حرفيّةُ مصفوفةٍ مختلطةٍ/متجانسةٍ غيرِ صحيحة،
                            //      أو [GAP 2] نتيجةُ نداءٍ يُرجع حرفيّةَ مصفوفةٍ كذلك)
                            // (EN) Does this argument warrant widening its param to Any?
                            auto argWantsAny = [&](const Sad::AST::Expression *a) -> bool {
                                if (auto *v = dynamic_cast<const Sad::AST::VariableExpr *>(a))
                                    return b_.scanVarElementType_.count(b_.currentScanFuncName_ + "#" + v->name) > 0;
                                if (auto *arr = dynamic_cast<const Sad::AST::ArrayExpr *>(a))
                                    return arrWantsAny(arr);
                                // (AR) [GAP 2] وسيطٌ نتيجةُ نداءٍ يُرجع مصفوفةً حرفيّة (خذ(يصنع()))
                                //      حيث خذ لامدا: نحلّ المُستدعى ونمسح جملةَ إرجاعه بنفس البوّابة.
                                // (EN) [GAP 2] A call-result arg returning an array literal (خذ(يصنع()))
                                //      where خذ is a lambda: resolve the callee and scan its return
                                //      statement with the same gate.
                                if (auto *ca = dynamic_cast<const Sad::AST::CallExpr *>(a))
                                {
                                    if (auto *cv = dynamic_cast<const Sad::AST::VariableExpr *>(ca->callee.get()))
                                    {
                                        auto cit = b_.functionTable_.find(cv->name);
                                        if (cit != b_.functionTable_.end() && cit->second.astDecl &&
                                            cit->second.astDecl->body)
                                            if (auto *blk = dynamic_cast<const Sad::AST::BlockStmt *>(cit->second.astDecl->body.get()))
                                                for (const auto &st : blk->statements)
                                                    if (auto *ret = dynamic_cast<const Sad::AST::ReturnStmt *>(st.get()))
                                                        if (ret->value)
                                                            if (auto *ae = dynamic_cast<const Sad::AST::ArrayExpr *>(ret->value.get()))
                                                                if (arrWantsAny(ae))
                                                                    return true;
                                    }
                                }
                                return false;
                            };
                            for (size_t ai = 0; ai < call->arguments.size(); ++ai)
                            {
                                if (argWantsAny(call->arguments[ai].get()))
                                    b_.scanLambdaParamAny_[lam].insert(ai);
                                // (AR) [موجة الجسر الموسوم — t05] وسيطٌ عشريٌّ لموقعِ
                                //      نداءِ لامدا: معاملُ اللامدا يُبنى i64 افتراضًا
                                //      فتعبرُ 2.5 بتّاتِها قمامةً (والمفسّرُ يُجيب 5.0).
                                //      يُوسَّعُ المعاملُ إلى Any فيُغلَّفُ الوسيطُ بوسمِه
                                //      وموقعُ الصحيحِ يبقى صادقًا عبرَ الوسمِ نفسِه.
                                // (EN) [Tagged-bridge wave — t05] A float argument at a
                                //      lambda call site: the lambda param builds as
                                //      default i64, so 2.5 crosses as raw bits (the
                                //      interpreter answers 5.0). Widen the param to Any:
                                //      the argument is boxed with its tag, and an
                                //      integer site stays truthful through the same tag.
                                else if (inferExprType(call->arguments[ai].get()) ==
                                         SadTypeKind::Float)
                                    b_.scanLambdaParamDynAny_[lam].insert(ai);
                            }
                        }
                    }

                    // ════════════════════════════════════════════════════════════
                    // (AR) [موجة الجسر الموسوم] تسجيلُ الهروبِ: مرجعُ دالّةٍ يُمرَّرُ
                    //      وسيطًا إلى دالّةِ مستخدمٍ («نفذ(ثلاثي)»، أو «طبق(د)» حيث د
                    //      ببرهانِ أصلٍ نظيف) — مواقعُ نداءِ الهاربةِ داخلَ المستقبِلةِ
                    //      غيرُ مرئيّةٍ للمسحِ فتُوسَّعُ خاناتُها الرقميّةُ إلى Any في
                    //      applyEscapedFuncParamWidening. من التمريرةِ الثانيةِ (بعدَ
                    //      اكتمالِ الجدولِ والتسميم)، ودوالُّ المستخدمِ حصرًا: تمريرُ
                    //      مرجعٍ لطرائقِ المدمجاتِ يسلكُ مسارَه المعلومَ القائمَ.
                    // (EN) [Tagged-bridge wave] Escape recording: a func-ref passed as
                    //      an argument into a USER function («نفذ(ثلاثي)», or «طبق(د)»
                    //      with clean provenance) — the escapee's call sites inside the
                    //      receiver are invisible to the scan, so its numeric slots
                    //      widen to Any in applyEscapedFuncParamWidening. Second pass
                    //      onward (table and poisoning complete), USER functions only:
                    //      refs passed to builtin methods keep their known path.
                    // ════════════════════════════════════════════════════════════
                    if (!funcName.empty() && b_.scanPassIndex_ >= 1 &&
                        b_.functionTable_.find(funcName) != b_.functionTable_.end())
                    {
                        // (AR) معاملاتُ الدالّةِ الماسحةِ نفسِها تحجبُ أسماءَ الدوالِّ
                        //      العليا — بها يُحرَسُ الذراعُ المباشرُ أدناه.
                        // (EN) The scanning function's own parameters shadow top-level
                        //      function names — used to guard the direct arm below.
                        auto isScanFuncParam = [&](const std::string &argName) -> bool {
                            auto scanFnIt = b_.functionTable_.find(b_.currentScanFuncName_);
                            if (scanFnIt == b_.functionTable_.end() ||
                                !scanFnIt->second.astDecl)
                                return false;
                            for (const auto &p : scanFnIt->second.astDecl->parameters)
                                if (p.name == argName)
                                    return true;
                            return false;
                        };
                        for (const auto &escArg : call->arguments)
                        {
                            // (AR) لامدا حرفيّةً وسيطًا: هروبٌ يجعل هدفَها مجهولًا داخلَ
                            //      المستقبِلةِ (dynproto) — تُوسَّعُ معاملاتُها كلُّها
                            //      قيمًا Any كي يفكَّها الجسرُ بوسومِها (كان العشريُّ
                            //      يُبترُ على توقيعِ i64 — مقيس عدائيًّا).
                            // (EN) A lambda literal as an argument: an escape that makes
                            //      its target unknown inside the receiver (dynproto) —
                            //      all its params widen to value-Any so the bridge
                            //      unboxes by tag (the float used to truncate on an
                            //      i64 signature — adversarially measured).
                            if (const auto *argLam =
                                    dynamic_cast<const Sad::AST::LambdaExpr *>(escArg.get()))
                            {
                                for (size_t pi = 0; pi < argLam->parameters.size(); ++pi)
                                    b_.scanLambdaParamDynAny_[argLam].insert(pi);
                                continue;
                            }
                            const auto *argVar =
                                dynamic_cast<const Sad::AST::VariableExpr *>(escArg.get());
                            if (!argVar)
                                continue;
                            // (AR) حرّاسُ التظليلِ يسبقون الذراعَين معًا: اسمٌ وقعَ هدفَ
                            //      إسنادٍ، أو له تصريحٌ محلّيٌّ، أو هو معاملُ الماسحةِ —
                            //      لا يُسجَّلُ هروبُه باسمِ الدالّةِ العليا المظلَّلة.
                            // (EN) Shadow guards precede BOTH arms: an assigned name, a
                            //      local declaration, or the scanner's own parameter
                            //      never records an escape under the shadowed
                            //      top-level function's name.
                            if (b_.scanAssignedNames_.count(argVar->name) != 0 ||
                                isScanFuncParam(argVar->name))
                                continue;
                            const std::string scopedEscKey =
                                b_.currentScanFuncName_ + "#" + argVar->name;
                            const bool hasLocalDecl =
                                b_.scanFuncRefDeclNode_.find(scopedEscKey) !=
                                b_.scanFuncRefDeclNode_.end();
                            if (b_.functionTable_.find(argVar->name) !=
                                    b_.functionTable_.end() &&
                                !hasLocalDecl)
                            {
                                b_.scanEscapedFuncs_.insert(argVar->name);
                                continue;
                            }
                            // (AR) متغيّرٌ مربوطٌ بلامدا يهربُ وسيطًا — التوسيعُ نفسُه.
                            // (EN) A lambda-bound variable escaping as an argument —
                            //      the same widening.
                            {
                                std::string lamKey = scopedEscKey;
                                auto lamEscIt = b_.scanLambdaVar_.find(lamKey);
                                if (lamEscIt == b_.scanLambdaVar_.end())
                                    lamEscIt = b_.scanLambdaVar_.find("#" + argVar->name);
                                if (lamEscIt != b_.scanLambdaVar_.end())
                                {
                                    const Sad::AST::LambdaExpr *escLam = lamEscIt->second;
                                    for (size_t pi = 0; pi < escLam->parameters.size(); ++pi)
                                        b_.scanLambdaParamDynAny_[escLam].insert(pi);
                                    continue;
                                }
                            }
                            // (AR) عبرَ ربطِ برهانِ الأصلِ — بحرّاسِ المسحِ نفسِها.
                            // (EN) Through the provenance binding — same scan guards.
                            std::string escKey = scopedEscKey;
                            if (b_.scanFuncRefBindings_.find(escKey) ==
                                    b_.scanFuncRefBindings_.end() &&
                                !hasLocalDecl)
                            {
                                escKey = "#" + argVar->name;
                            }
                            if (b_.scanFuncRefPoisoned_.count(escKey) == 0)
                            {
                                auto escIt = b_.scanFuncRefBindings_.find(escKey);
                                if (escIt != b_.scanFuncRefBindings_.end())
                                    b_.scanEscapedFuncs_.insert(escIt->second);
                            }
                        }
                    }

                    if (!funcName.empty())
                    {
                        auto it = b_.functionTable_.find(funcName);

                        // ═══════════════════════════════════════════════════════════════
                        // (AR) إصلاح: إذا لم نجد الدالة، نتحقق إذا كان استدعاء باني صنف
                        //      بدون كلمة "جديد". في لغة ص، كائن_حي("حي") يُحلَّل كـ CallExpr
                        //      لكن الباني مسجّل باسمه المفكوك (constructorNameFor) في b_.functionTable_
                        //      بدون هذا: أنواع معاملات الباني لا تُحدَّث من call-site
                        //      مما يؤدي لبقاء المعاملات كـ Integer بدلاً من String
                        // (EN) Fix: If function not found, check if it's a class constructor
                        //      call without "new" keyword. In Sad, ClassName("arg") is parsed
                        //      as CallExpr but the ctor is registered under its mangled name (constructorNameFor)
                        //      Without this: constructor param types don't get updated from call-site
                        //      causing params to remain Integer instead of String
                        // ═══════════════════════════════════════════════════════════════
                        bool isImplicitCtorCall = false;
                        if (it == b_.functionTable_.end())
                        {
                            // (AR) إصلاح: بدلاً من b_.module_->getClass() (غير متاح في Phase 1.7)
                            //      نبحث مباشرة عن اسم الباني المفكوك (constructorNameFor) في
                            //      b_.functionTable_ — مسجّل في Phase 1.35 قبل Phase 1.7
                            // (EN) Fix: Instead of b_.module_->getClass() (unavailable in Phase 1.7)
                            //      look directly for the mangled ctor name (constructorNameFor) in
                            //      b_.functionTable_ — registered in Phase 1.35 before Phase 1.7
                            std::string ctorName = constructorNameFor(funcName);
                            it = b_.functionTable_.find(ctorName);
                            isImplicitCtorCall = (it != b_.functionTable_.end());
                        }

                        // (AR) صيغةُ الإنشاءِ بلا «جديد» — تُرقّى معاملاتُ البانِي في
                        //      الشجرةِ كصيغةِ NewExpr سواءً بسواء.
                        // (EN) The no-`new` instantiation form — the constructor's parameters
                        //      are promoted in the AST exactly as for the NewExpr form.
                        if (isImplicitCtorCall)
                        {
                            auto classIt = scanClassByName_.find(funcName);
                            if (classIt != scanClassByName_.end())
                                for (const auto &member : classIt->second->members)
                                    if (auto *ctor = dynamic_cast<Sad::AST::ConstructorDecl *>(member.get()))
                                    {
                                        recordMemberParamArgs(ctor->parameters, call->arguments, true);
                                        break;
                                    }
                        }

                        if (it != b_.functionTable_.end())
                        {
                            auto &funcInfo = it->second;
                            // (AR) عند استدعاء الباني ضمنياً، المعامل الأول هو self → نزيح بـ 1
                            // (EN) For implicit constructor calls, first param is self → offset by 1
                            size_t paramOffset = isImplicitCtorCall ? 1 : 0;
                            for (size_t i = 0; i < call->arguments.size() && (i + paramOffset) < funcInfo.parameters.size(); i++)
                            {
                                SadTypeKind argType = inferExprType(call->arguments[i].get());
                                SadTypeKind &paramType = funcInfo.parameters[i + paramOffset].type;

                                // (AR) تسجيلُ نوعِ الوسيطِ في هذا الموقع — لا يُغيّر شيئًا هنا؛
                                //      الحكمُ يقع في applyAgreedFreeParamTypes بعد مسحِ المواقع
                                //      كلِّها، فخانةٌ اختلفت مواقعُها تُترَك على عمومِها.
                                // (EN) Record this site's argument kind. Nothing changes here;
                                //      the ruling happens in applyAgreedFreeParamTypes once every
                                //      site has been scanned, so a slot whose sites disagree is
                                //      left general.
                                scanFreeArgKinds_[funcName][i + paramOffset]
                                    .insert(static_cast<int>(argType));

                                // (AR) [GAP 3b] وسيطٌ متغيّرٌ يحمل مصفوفةً سُجِّل نوعُ عنصرها
                                //      في المسح المُسبَق عند تصريحه (مختلطٌ قياسيّ ⇒ Any، أو
                                //      متجانسٌ ⇒ نوعُه). نوّع المعاملَ إلى مصفوفةٍ ونضبط نوعَ
                                //      عنصرها، فيُبنى جسمُ الدالّة عالمًا أنّ فهرسةَ المعامل
                                //      تُقرأ موسومةً زمنَ التشغيل لا عدديًّا. «الموقعُ الأوّل
                                //      يفوز»: لا نكتب فوق نوع عنصرٍ سبق ضبطُه (يطابق مسارَ
                                //      الحرفيّة المباشرة). آمنٌ بعد الخيار ٢.
                                // (EN) [GAP 3b] A variable arg holding an array whose element
                                //      type was recorded in the pre-pass at its declaration
                                //      (scalar-heterogeneous ⇒ Any, homogeneous ⇒ its type).
                                //      Widen the param to Array and set its element type so the
                                //      body is built knowing the param's index reads runtime-
                                //      tagged, not as int. "First call site wins": never
                                //      overwrite an element type already set (mirrors the
                                //      direct-literal path). Safe after option 2.
                                bool handledByVarWiden = false;
                                if (auto *argVar = dynamic_cast<const Sad::AST::VariableExpr *>(call->arguments[i].get()))
                                {
                                    auto vit = b_.scanVarElementType_.find(b_.currentScanFuncName_ + "#" + argVar->name);
                                    if (vit != b_.scanVarElementType_.end() &&
                                        (paramType == SadTypeKind::Integer || paramType == SadTypeKind::Array))
                                    {
                                        paramType = SadTypeKind::Array;
                                        SadTypeKind &pElem = funcInfo.parameters[i + paramOffset].elementType;
                                        if (pElem == SadTypeKind::Void)
                                            pElem = vit->second;
                                        handledByVarWiden = true;
                                    }
                                }

                                // (AR) إذا كان المعامل I64 (افتراضي من UNKNOWN) والوسيط ذو نوع أكثر تحديداً
                                // (EN) If param is I64 (default from UNKNOWN) and arg is a more specific type
                                if (handledByVarWiden)
                                {
                                    // (AR) عُولج أعلاه (توسيعُ وسيط المتغيّر) ⇒ لا نُطبّق قواعدَ
                                    //      النوع القياسيّ التالية على هذا الوسيط.
                                    // (EN) Handled above (variable-arg widening) ⇒ skip the
                                    //      scalar type rules below for this argument.
                                }
                                else if (paramType == SadTypeKind::Integer && argType == SadTypeKind::String)
                                {
                                    paramType = SadTypeKind::String;
#ifdef SIR_BUILDER_DEBUG
                                    std::cerr << "[TYPE-INFER] " << funcName << " param[" << i
                                              << "] '" << funcInfo.parameters[i + paramOffset].name
                                              << "': I64 -> STRING" << std::endl;
#endif
                                }
                                // (AR) ISSUE-076 (حلّ %SadDyn الجذريّ): توحيد int⊔float للمعامل.
                                //      المعامل الافتراضيّ Integer ملتبِسٌ مع الصحيح الحقيقيّ، فنتتبّع
                                //      الأنواع العدديّة المُشاهَدة عبر كلّ مواقع الاستدعاء (bit0=صحيح،
                                //      bit1=عشريّ): إن اجتمعا ⇒ Any (يُخفَض %SadDyn) فيبقى
                                //      `جمع(3،4)⇒7` و`جمع(2.5،1.5)⇒4.0`؛ عشريّ فقط ⇒ Float (سريعٌ
                                //      أصيل)؛ صحيح فقط ⇒ Integer. مستقلٌّ عن ترتيب المواقع.
                                // (EN) ISSUE-076 (%SadDyn root fix): int⊔float param unification. The
                                //      default Integer is ambiguous with a real int, so track the
                                //      numeric arg types seen across ALL call sites (bit0=int,
                                //      bit1=float): both ⇒ Any (lowered to %SadDyn) so `جمع(3,4)⇒7`
                                //      and `جمع(2.5,1.5)⇒4.0`; float-only ⇒ Float (native); int-only
                                //      ⇒ Integer. Order-independent across call sites.
                                else if ((argType == SadTypeKind::Float || argType == SadTypeKind::Integer) &&
                                         (paramType == SadTypeKind::Integer ||
                                          paramType == SadTypeKind::Float ||
                                          paramType == SadTypeKind::Any))
                                {
                                    static std::map<std::string, int> s_numSeen;
                                    const int kSawInt = 1;
                                    const int kSawFloat = 2;
                                    std::string key = funcName + "#" + std::to_string(i + paramOffset);
                                    int &seen = s_numSeen[key];
                                    seen |= (argType == SadTypeKind::Integer) ? kSawInt : kSawFloat;
                                    if (seen == (kSawInt | kSawFloat))
                                        paramType = SadTypeKind::Any;
                                    else if (seen == kSawFloat && paramType == SadTypeKind::Integer)
                                        paramType = SadTypeKind::Float;
                                    // (AR) صحيح فقط ⇒ يبقى Integer / (EN) int-only ⇒ stays Integer
                                }
                                else if (paramType == SadTypeKind::Integer && argType == SadTypeKind::Boolean)
                                {
                                    paramType = SadTypeKind::Boolean;
                                }
                                // (AR) ISSUE-076/084 (ب″): وسيطٌ ديناميّ (Any = حمولة ADT مجهولة
                                //      النوع سكونيًّا) ⇒ رقِّ المعامل إلى Any، فيُبنى جسم الدالة
                                //      عالمًا أنّ المعامل موسومٌ زمنَ التشغيل ⇒ حسابُه يفكّ التعليب
                                //      (`زد(س)` حيث س عشريّ مُعلَّب). لا يُنقِص الدقّة: قيمةٌ صحيحة
                                //      تبقى موسومة صحيحًا وتُفكّ صحيحًا (لا انحدار في البوّابة).
                                // (EN) ISSUE-076/084 (ب″): a dynamic (Any = statically-unknown ADT
                                //      payload) argument ⇒ widen the param to Any, so the function
                                //      body is built knowing the param is runtime-tagged ⇒ its
                                //      arithmetic unboxes (`زد(س)` where س is a boxed float). No loss:
                                //      an integer value stays int-tagged and decodes as int (no gate
                                //      regression).
                                else if (paramType == SadTypeKind::Integer && argType == SadTypeKind::Any)
                                {
                                    paramType = SadTypeKind::Any;
                                }
                                // ═══════════════════════════════════════════════════════════
                                // (AR) إصلاح: استنتاج نوع المصفوفة + نوع عناصرها
                                //      عند تمرير مصفوفة كوسيط، نحدّث نوع المعامل إلى Array
                                //      ونفحص عناصر المصفوفة (إن كانت ArrayExpr) لتحديد elementType
                                //      بدون هذا: forEach على مصفوفة نصوص يعامل العناصر كأرقام
                                // (EN) Fix: Infer array type + element type
                                //      When array literal is passed, update param to Array
                                //      and inspect elements to determine elementType
                                //      Without this: forEach over string array treats elements as integers
                                // ═══════════════════════════════════════════════════════════
                                else if ((paramType == SadTypeKind::Integer ||
                                          paramType == SadTypeKind::Array) &&
                                         argType == SadTypeKind::Array)
                                {
                                    // (AR) يشمل المعاملَ المصرَّح `مصفوفة` (paramType=Array) لا
                                    //      المُستنتَجَ فقط (Integer)؛ فبدونه لا يحمل المعاملُ
                                    //      المصرَّحُ نوعَ عنصرٍ ⇒ تُقرأ فهرستُه عدديًّا (قمامة).
                                    // (EN) Covers a declared `array` param (paramType=Array), not
                                    //      just an inferred one (Integer); without it a declared
                                    //      param carries no element type ⇒ its index reads as int.
                                    paramType = SadTypeKind::Array;
                                    // (AR) [وسم زمن-التشغيل] فحص عناصر المصفوفة لتحديد نوع
                                    //      العنصر. وسيطُ حرفيّةٍ مباشرة (ArrayExpr) ⇒ نمسح كلّ
                                    //      عناصره: مختلطٌ قياسيّ ⇒ Any (فيُبنى جسمُ الدالّة عالمًا
                                    //      أنّ المعاملَ موسومٌ زمنَ التشغيل، فتُقرأ فهرستُه موسومةً
                                    //      لا عدديًّا)؛ متجانسٌ ⇒ نوعُه؛ غير-قياسيٍّ مختلطٍ ⇒ نتركه
                                    //      (Void، حدٌّ موروثٌ ISSUE-067/070). نطابق بوّابةَ باني
                                    //      الحرفيّة. (سابقًا: العنصرُ الأوّل فقط ⇒ يُظلَّل المختلط.)
                                    // (EN) [runtime tags] inspect array elements to determine the
                                    //      element type. A direct literal arg (ArrayExpr) ⇒ scan
                                    //      all elements: scalar-heterogeneous ⇒ Any (so the body is
                                    //      built knowing the param is runtime-tagged and its index
                                    //      reads tagged, not as an integer); homogeneous ⇒ its type;
                                    //      non-scalar mixed ⇒ leave it (Void, inherited ISSUE-067/070
                                    //      limit). Mirrors the literal builder's gate. (Previously:
                                    //      only the first element ⇒ a mixed array was mis-typed.)
                                    if (auto *arrExpr = dynamic_cast<const Sad::AST::ArrayExpr *>(call->arguments[i].get()))
                                    {
                                        if (!arrExpr->elements.empty())
                                        {
                                            auto isScalarKind = [](SadTypeKind t) {
                                                return t == SadTypeKind::Integer || t == SadTypeKind::Float ||
                                                       t == SadTypeKind::String || t == SadTypeKind::Boolean ||
                                                       t == SadTypeKind::UInt8 || t == SadTypeKind::UInt64 ||
                                                       t == SadTypeKind::Null || t == SadTypeKind::Any;
                                            };
                                            SadTypeKind firstElemType = inferExprType(arrExpr->elements[0].get());
                                            bool homogeneous = true, allScalar = true, hasAny = false;
                                            for (const auto &el : arrExpr->elements)
                                            {
                                                SadTypeKind et = inferExprType(el.get());
                                                if (et != firstElemType)
                                                    homogeneous = false;
                                                if (!isScalarKind(et))
                                                    allScalar = false;
                                                if (et == SadTypeKind::Any)
                                                    hasAny = true;
                                            }
                                            SadTypeKind &pElem = funcInfo.parameters[i + paramOffset].elementType;
                                            // (AR) «الموقعُ الأوّل يفوز»: نضبط نوعَ العنصر مرّةً
                                            //      واحدةً (حين يكون Void) ولا نكتب فوقه من موقعِ
                                            //      نداءٍ لاحق. حرِجٌ: توسيعُ معاملٍ إلى Any من موقعٍ
                                            //      لاحقٍ يكسر موقعًا أسبقَ مرّر مصفوفةً متجانسةً غيرَ
                                            //      عدديّة (نصوصًا): تُقرأ حينها عبر مسار Any ووسومُها
                                            //      null ⇒ احتياطيُّ الخلفيّة يفترض Int ⇒ قمامة. هذا
                                            //      يطابق دلالةَ ما قبلُ (المعاملُ غيرُ المصرَّح كان
                                            //      يُجمَّد على أوّل موقع) ويُبقي تحسينَ المعامل
                                            //      المصرَّح والموقعِ الأوحد بلا انحدار.
                                            // (EN) "first call site wins": set the element type once
                                            //      (while Void) and never overwrite it from a later
                                            //      site. Critical: widening a param to Any from a
                                            //      later site breaks an EARLIER site that passed a
                                            //      homogeneous non-int (string) array — it would then
                                            //      read via the Any path with null tags ⇒ the backend
                                            //      Int fallback ⇒ garbage. This matches the prior
                                            //      semantics (an untyped param froze on its first
                                            //      site) and keeps the declared-param / single-site
                                            //      improvement without regressing multi-site.
                                            if (pElem == SadTypeKind::Void)
                                            {
                                                if (allScalar && ((!homogeneous && arrExpr->elements.size() > 1) || hasAny))
                                                    pElem = SadTypeKind::Any;
                                                else if (homogeneous)
                                                    pElem = firstElemType;
                                            }
                                        }
                                    }
                                    // ════════════════════════════════════════════════
                                    // (AR) [قفزةُ التمرير] وسيطٌ **متغيّرٌ** لا حرفيّة: مصفوفةٌ
                                    //      وصلت معاملًا إلى الدالّة الحاضنة ثمّ مُرِّرت كما هي.
                                    //
                                    //      كان الاستنتاجُ يعرف الحرفيّةَ ونتيجةَ النداء فقط، فينقطع
                                    //      عند أوّل تمريرٍ: `قرر(... ["src"])` يُكسِب معاملَ «قرر»
                                    //      نوعَ عنصرٍ نصّيًّا، ثمّ يُمرَّر إلى «مسموح_بالكتابة»
                                    //      فيصل بلا نوعِ عنصر ⇒ متغيّرُ «لكل» عليه عددٌ ⇒ يُبنى
                                    //      مسارٌ من عنوانٍ ⇒ يُرفض مسارٌ مسموحٌ به. أي أنّ العيبَ
                                    //      يظهر **قرارًا أمنيًّا خاطئًا** لا خطأَ نوع.
                                    //
                                    //      ننسخ نوعَ العنصر من معامل الدالّة الحاضنة. والتمريرات
                                    //      المتعدّدة (أدناه) تجعله يعبر أيّ عددٍ من القفزات.
                                    // (EN) [forwarding hop] The argument is a VARIABLE, not a
                                    //      literal: an array that arrived as the enclosing
                                    //      function's parameter and is passed straight through.
                                    //      Inference knew literals and call results only, so it
                                    //      stopped after one hop — and the symptom was a WRONG
                                    //      SECURITY DECISION (an allowed path denied), not a type
                                    //      error. Copy the element type from the enclosing
                                    //      function's parameter; the existing multi-pass loop then
                                    //      carries it across any number of hops.
                                    // ════════════════════════════════════════════════
                                    else if (auto *varArg = dynamic_cast<const Sad::AST::VariableExpr *>(call->arguments[i].get()))
                                    {
                                        SadTypeKind &pElem = funcInfo.parameters[i + paramOffset].elementType;
                                        if (pElem == SadTypeKind::Void && !b_.currentScanFuncName_.empty())
                                        {
                                            auto encIt = b_.functionTable_.find(b_.currentScanFuncName_);
                                            if (encIt != b_.functionTable_.end())
                                            {
                                                for (const auto &encParam : encIt->second.parameters)
                                                {
                                                    if (encParam.name == varArg->name &&
                                                        encParam.elementType != SadTypeKind::Void)
                                                    {
                                                        pElem = encParam.elementType;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    // (AR) [GAP 2] وسيطٌ نتيجةُ نداءٍ يُرجع مصفوفةً حرفيّة
                                    //      (خذ(يصنع())): ليس حرفيّةَ مصفوفةٍ مباشرةً بل CallExpr.
                                    //      نحلّ المُستدعى عبر functionTable_ ونمسح جملةَ الإرجاع في
                                    //      جسمه (astDecl) بحثًا عن حرفيّة مصفوفة، فنستنتج نوعَ عنصرها
                                    //      بنفس بوّابة «الموقع الأوّل يفوز» (Any للمختلط القياسيّ،
                                    //      نوعُه للمتجانس). بدونه: المعاملُ يبقى بلا نوعِ عنصرٍ ⇒ تُقرأ
                                    //      فهرستُه عدديًّا ⇒ قمامة. لا نمسّ الحالةَ التي حُلّ فيها
                                    //      الوسيطُ حرفيّةً مباشرةً (arrExpr أعلاه).
                                    // (EN) [GAP 2] A call-result arg returning an array literal
                                    //      (خذ(يصنع())): not a direct array literal but a CallExpr.
                                    //      Resolve the callee via functionTable_ and scan its return
                                    //      statement's body (astDecl) for an array literal, inferring
                                    //      its element type with the same first-site-wins gate (Any for
                                    //      scalar-mixed, its type for homogeneous). Without it the param
                                    //      carries no element type ⇒ its index reads as int ⇒ garbage.
                                    else if (auto *callArg = dynamic_cast<const Sad::AST::CallExpr *>(call->arguments[i].get()))
                                    {
                                        if (auto *calleeVar = dynamic_cast<const Sad::AST::VariableExpr *>(callArg->callee.get()))
                                        {
                                            auto cit = b_.functionTable_.find(calleeVar->name);
                                            if (cit != b_.functionTable_.end() && cit->second.astDecl &&
                                                cit->second.astDecl->body)
                                            {
                                                // (AR) امسح جمل الجسم العليا عن ReturnStmt قيمتُه حرفيّةُ مصفوفة
                                                // (EN) Scan top-level body statements for a ReturnStmt whose value is an array literal
                                                const Sad::AST::ArrayExpr *retArr = nullptr;
                                                if (auto *blk = dynamic_cast<const Sad::AST::BlockStmt *>(cit->second.astDecl->body.get()))
                                                {
                                                    for (const auto &st : blk->statements)
                                                    {
                                                        if (auto *ret = dynamic_cast<const Sad::AST::ReturnStmt *>(st.get()))
                                                            if (ret->value)
                                                                if (auto *ae = dynamic_cast<const Sad::AST::ArrayExpr *>(ret->value.get()))
                                                                    retArr = ae;
                                                    }
                                                }
                                                if (retArr && !retArr->elements.empty())
                                                {
                                                    auto isScalarKind = [](SadTypeKind t) {
                                                        return t == SadTypeKind::Integer || t == SadTypeKind::Float ||
                                                               t == SadTypeKind::String || t == SadTypeKind::Boolean ||
                                                               t == SadTypeKind::UInt8 || t == SadTypeKind::UInt64 ||
                                                               t == SadTypeKind::Null || t == SadTypeKind::Any;
                                                    };
                                                    SadTypeKind firstElemType = inferExprType(retArr->elements[0].get());
                                                    bool homogeneous = true, allScalar = true, hasAny = false;
                                                    for (const auto &el : retArr->elements)
                                                    {
                                                        SadTypeKind et = inferExprType(el.get());
                                                        if (et != firstElemType)
                                                            homogeneous = false;
                                                        if (!isScalarKind(et))
                                                            allScalar = false;
                                                        if (et == SadTypeKind::Any)
                                                            hasAny = true;
                                                    }
                                                    SadTypeKind &pElem = funcInfo.parameters[i + paramOffset].elementType;
                                                    if (pElem == SadTypeKind::Void)
                                                    {
                                                        if (allScalar && ((!homogeneous && retArr->elements.size() > 1) || hasAny))
                                                            pElem = SadTypeKind::Any;
                                                        // (AR) [إصلاح أميليا] المتجانسُ الصحيحُ يُترَك Void لا Integer:
                                                        //      القراءةُ الافتراضيّة عدديّة فيُقرأ صحيحًا على أيّ حال، وتجميدُه
                                                        //      Integer يَكبت توسيعَ موقعٍ لاحقٍ مختلطٍ إلى Any (يشارك المعاملَ)
                                                        //      ⇒ قمامة. نُجمّد المتجانسَ غيرَ الصحيح فقط (نصّ/عشري/منطقيّ):
                                                        //      قراءتُه تلزمها معرفةُ النوع، ولا يُقرأ صحيحًا افتراضًا.
                                                        // (EN) [Amelia fix] Leave homogeneous-int as Void, not Integer: the
                                                        //      default read path is int so it decodes correctly anyway, while
                                                        //      freezing Integer would suppress a later shared-param mixed site's
                                                        //      Any-widening ⇒ garbage. Freeze only homogeneous non-int
                                                        //      (string/float/bool), whose read needs the concrete type.
                                                        else if (homogeneous && firstElemType != SadTypeKind::Integer)
                                                            pElem = firstElemType;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                // (AR) إصلاح جذري: عند تضارب الأنواع (مثلاً STRING ثم BOOL)
                                //      نبقي STRING كنوع عام — التحويل يحدث عند الاستدعاء
                                //      في sir_builder_calls.cpp عبر BOOL_TO_STRING/I64_TO_STRING
                                // (EN) Radical fix: if types conflict (e.g. STRING then BOOL)
                                //      keep STRING as common type — conversion happens at call site
                                //      in sir_builder_calls.cpp via BOOL_TO_STRING/I64_TO_STRING
                                else if (paramType == SadTypeKind::String && argType != SadTypeKind::String)
                                {
                                    // (AR) STRING يبقى — التحويل يحدث عند موقع الاستدعاء
                                    // (EN) STRING stays — conversion happens at call site
#ifdef SIR_BUILDER_DEBUG
                                    std::cerr << "[TYPE-INFER] " << funcName << " param[" << i
                                              << "] '" << funcInfo.parameters[i + paramOffset].name
                                              << "': keeping STRING (call-site will convert "
                                              << static_cast<int>(argType) << ")" << std::endl;
#endif
                                }
                                // (AR) «أي» قمّةُ الشبكةِ فلا تُخفَّض إلى نصّ.
                                //
                                //      كانت هذه الذراعُ تقرأ «ليس Integer» فتبتلع `Any` معها،
                                //      فيكفي موقعُ نداءٍ واحدٌ بوسيطٍ نصّيٍّ ليصير معاملٌ
                                //      **مُصرَّحٌ `أي`** نصًّا في الجدول. والتوقيعُ المُصدَرُ
                                //      يبقى `%SadDyn` — فينشقُّ الجدولُ عن الكود: تُدرَج عند
                                //      كلِّ نداءٍ تاليٍ تحويلةٌ إلى نصّ ثمّ يُعادُ التغليفُ
                                //      بوسمِ «نصّ».
                                //
                                //      وأثرُه ليس انهيارًا بل **كذبًا صامتًا**: `ولد([1،2،3])`
                                //      في مكتبةِ جيسون كانت تُخرِج `["1","2","3"]` — جيسونٌ
                                //      صالحُ الشكلِ يقول «نصوص» عن أعداد، ولا يُخفِق إلّا حين
                                //      يقرأه طرفٌ آخر. وشرطُ ظهورِه أن يسبقَه نداءٌ بنصّ في
                                //      الملفِّ نفسِه، فيختفي عند تصغيرِ المثالِ ويبدو «رفرفةً».
                                //
                                //      والتصريحُ `أي` قرارُ المؤلِّفِ لا نقصُ معلومةٍ يُستنتَج:
                                //      لا موقعَ نداءٍ يُخوَّل نقضَه. أمّا `Integer` فافتراضٌ
                                //      يعني «غيرَ معلوم» — وترقيتُه هي المقصودةُ هنا.
                                // (EN) `Any` is the lattice top and must not be narrowed to String.
                                //
                                //      This arm tested "not Integer", which swallowed `Any` too, so a
                                //      single string-argument call site turned a parameter **declared
                                //      `any`** into a string in the table. The emitted signature stays
                                //      `%SadDyn`, so the table and the code split: every later call site
                                //      gets a to-string conversion inserted and is then re-boxed with
                                //      the string tag.
                                //
                                //      The effect is not a crash but a **silent lie**: `generate([1,2,3])`
                                //      in the JSON library emitted `["1","2","3"]` — well-formed JSON
                                //      that says "strings" about numbers, failing only when someone else
                                //      reads it. It only appears when a string call precedes it in the
                                //      same file, so it vanishes on a reduced example and looks flaky.
                                //
                                //      A declared `any` is the author's decision, not missing information
                                //      to be inferred: no call site is entitled to overrule it. `Integer`,
                                //      by contrast, is the "unknown" default — promoting that is the point.
                                else if (paramType != SadTypeKind::Integer &&
                                         paramType != SadTypeKind::Any &&
                                         argType == SadTypeKind::String)
                                {
                                    // (AR) ترقية إلى STRING — النص هو أوسع الأنواع القابلة للمقارنة
                                    // (EN) Promote to STRING — string is the widest comparable type
                                    paramType = SadTypeKind::String;
#ifdef SIR_BUILDER_DEBUG
                                    std::cerr << "[TYPE-INFER] " << funcName << " param[" << i
                                              << "] '" << funcInfo.parameters[i + paramOffset].name
                                              << "': " << static_cast<int>(paramType)
                                              << " -> STRING (widened)" << std::endl;
#endif
                                }
                                // ═══════════════════════════════════════════════════════════
                                // (AR) قمّةُ الشبكة: تعارضٌ لا ذراعَ له ⇒ «أي» لا تجمّدٌ صامت.
                                //      قبلَ هذه الذراعِ كانت الشبكةُ بلا قمّة: متى غادر المعاملُ
                                //      `Integer` إلى نوعٍ ملموسٍ (مصفوفة/منطقيّ/عشريّ) لم يَعُد أيُّ
                                //      موقعِ نداءٍ **مخالفٍ** يوسّعه، فيتجمّد على أوّلِ نوعٍ بلغه
                                //      وتُقرأ بقيّةُ المواقعِ بنوعٍ ليس نوعَها.
                                //      مقيسًا: `افحص([1،2])` ثمّ `افحص(صحيح)` ثمّ `افحص(2.5)`
                                //      كانت تُجيب «مصفوفة» ثلاثَ مرّات، والقيمُ تُقرأ مصفوفاتٍ
                                //      فتُخرِج قمامةً أو تُسقِط الثنائيّ.
                                //      و«أي» هنا ليست تخمينًا: هي النوعُ الذي يحمل وسمَه زمنَ
                                //      التشغيل، فيقرأه `نوع()` وتفكّه الحسابات — وهو ما يفعله
                                //      المفسّرُ أصلًا.
                                // (EN) Lattice top: a conflict with no arm ⇒ Any, not a silent freeze.
                                //      Before this arm the lattice had no top: once a param left
                                //      `Integer` for a concrete kind (Array/Boolean/Float), no later
                                //      *conflicting* call site could widen it, so it froze on the first
                                //      kind it reached and every other site was read as the wrong type.
                                //      Measured: `افحص([1،2])`, `افحص(صحيح)`, `افحص(2.5)` answered
                                //      «مصفوفة» three times, and the values were read as arrays —
                                //      emitting garbage or crashing the binary.
                                //
                                // (AR) `String` مستثنًى عمدًا: ذراعُه أعلاه تُبقيه لأنّ موقعَ النداءِ
                                //      يُدرِج التحويلَ (BOOL_TO_STRING/I64_TO_STRING)، فتوسيعُه هنا
                                //      يُبطِل تحويلًا يعمل. والاستثناءُ مقصورٌ عليه لا يُعمَّم.
                                // (EN) `String` is deliberately excluded: its arm above keeps it because
                                //      the call site inserts the conversion (BOOL_TO_STRING/I64_TO_STRING),
                                //      so widening here would defeat a conversion that works. The
                                //      exception is confined to it and is not generalised.
                                // ═══════════════════════════════════════════════════════════
                                else if (paramType != SadTypeKind::String &&
                                         paramType != SadTypeKind::Any &&
                                         argType != SadTypeKind::Void &&
                                         argType != paramType)
                                {
                                    paramType = SadTypeKind::Any;
#ifdef SIR_BUILDER_DEBUG
                                    std::cerr << "[TYPE-INFER] " << funcName << " param[" << i
                                              << "] '" << funcInfo.parameters[i + paramOffset].name
                                              << "': conflicting call sites -> ANY" << std::endl;
#endif
                                }
                            }

                            // ================================================================
                            // (AR) المرحلة 1.75: استنتاج أسماء الأصناف لمعاملات الدوال
                            //      عند تمرير كائن (متتبَّع في b_.classInstanceTypes_) كوسيط
                            //      نسجّل اسم الصنف في b_.paramClassTypes_ للاستخدام اللاحق
                            //      في b_.buildFunction عند بناء الجسم + inferReturnTypeFromBody
                            // (EN) Phase 1.75: Infer class names for function parameters
                            //      When an object (tracked in b_.classInstanceTypes_) is passed as arg
                            //      register class name in b_.paramClassTypes_ for later use
                            //      in b_.buildFunction when building body + inferReturnTypeFromBody
                            // ================================================================
                            for (size_t i = 0; i < call->arguments.size() && (i + paramOffset) < funcInfo.parameters.size(); i++)
                            {
                                const auto &arg = call->arguments[i];
                                std::string argClass;
                                if (auto *varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(arg.get()))
                                {
                                    // (AR) تحقق: هل هذا المتغير كائن مسجل في b_.classInstanceTypes_?
                                    // (EN) Check: is this variable an object tracked in b_.classInstanceTypes_?
                                    auto ciIt = b_.classInstanceTypes_.find(varExpr->name);
                                    if (ciIt != b_.classInstanceTypes_.end())
                                    {
                                        argClass = ciIt->second;
                                    }
                                    // ════════════════════════════════════════════════
                                    // (AR) 🔑 وإلّا فخريطةُ المسحِ نفسِها. `classInstanceTypes_`
                                    //      خريطةُ **زمنِ البناء**: تُملأ عند بناءِ
                                    //      `متغير أ = متجه(1)` — أي بعدَ هذا المسحِ كلِّه.
                                    //      فالبحثُ أعلاه يُخفِق دائمًا للمتغيّراتِ العامّة
                                    //      و`paramClassTypes_` تبقى فارغةً، فلا يُسجَّل صنفُ
                                    //      المعامِلِ ولا يجد جسمُ الدالّةِ صنفًا، فتنزل
                                    //      `القيمة == المتوقع` مقارنةَ **مؤشِّرَين**: جوابٌ
                                    //      خاطئٌ صامتٌ بلا رمزٍ ولا رسالة.
                                    //      والمسحُ يملك الجوابَ أصلًا: `scanClassOfVariable_`
                                    //      تُبنى في `scanCallSitesInStmt` من `جديد` ومن نداءِ
                                    //      الباني في جملةِ التصريحِ نفسِها.
                                    //      وهي مسطَّحةٌ بالاسمِ بلا نطاقٍ — قيدٌ مُعلَنٌ عندها،
                                    //      ونحن نرِثُه هنا ولا نزيده.
                                    // (EN) Otherwise consult the scan's own map.
                                    //      `classInstanceTypes_` is the BUILD-time map, filled
                                    //      when `var a = Vec(1)` is built — after this whole
                                    //      scan. So the lookup above always missed for globals,
                                    //      `paramClassTypes_` stayed empty, the parameter's class
                                    //      was never recorded, the function body found no class,
                                    //      and `value == expected` lowered to a POINTER compare:
                                    //      a silent wrong answer with no code and no message.
                                    //      The scan already knows: `scanClassOfVariable_` is built
                                    //      in `scanCallSitesInStmt` from `new` and from the
                                    //      constructor call in the declaration itself.
                                    //      It is flat by name with no scoping — a limitation
                                    //      declared at its definition, inherited here, not widened.
                                    // ════════════════════════════════════════════════
                                    else
                                    {
                                        auto scanIt = scanClassOfVariable_.find(varExpr->name);
                                        if (scanIt != scanClassOfVariable_.end() && scanIt->second)
                                        {
                                            argClass = scanIt->second->name;
                                        }
                                    }
                                    if (!argClass.empty())
                                    {
                                        b_.paramClassTypes_[funcName][funcInfo.parameters[i + paramOffset].name] = argClass;
                                    }
                                }
                                // (AR) يُسجَّل في كلِّ موقعٍ — ولو لم يكن الوسيطُ كائنًا — كي
                                //      يَظهر الاختلاف. وتسجيلُ المطابقِ وحدَه يجعل موقعًا واحدًا
                                //      «إجماعًا» فيبقى الربطُ بالصنفِ ويصيبُه الموقعُ العدديّ.
                                // (EN) Recorded at EVERY site, non-object args included, so the
                                //      disagreement is visible. Recording only the matching sites
                                //      would make one site a "unanimity", the class binding would
                                //      survive, and the numeric site would hit it.
                                scanFreeArgClasses_[funcName][i + paramOffset].insert(argClass);
                            }
                        }
                    }

                    // (AR) مسح الوسائط أيضاً (قد تحوي استدعاءات متداخلة)
                    // (EN) Scan arguments too (may contain nested calls)
                    for (const auto &arg : call->arguments)
                    {
                        scanCallSitesInExpr(arg.get());
                    }
                    return;
                }

                // ================================================================
                // (AR) تعبير الإنشاء (NewExpr) — استنتاج أنواع وسائط الباني
                //      مثل CallExpr لكن الاسم اسمُ الباني المفكوك (constructorNameFor) والمعامل الأول هو self
                // (EN) New expression (NewExpr) — infer constructor arg types
                //      Like CallExpr but the name is the mangled ctor (constructorNameFor) and first param is self
                // ================================================================
                if (auto *newExpr = dynamic_cast<const Sad::AST::NewExpr *>(expr))
                {
                    std::string ctorName = constructorNameFor(newExpr->className);
                    auto it = b_.functionTable_.find(ctorName);
                    if (it != b_.functionTable_.end())
                    {
                        auto &funcInfo = it->second;
                        // params[0] = self, params[1..N] = user params
                        for (size_t i = 0; i < newExpr->arguments.size(); i++)
                        {
                            size_t paramIdx = i + 1; // +1 لتخطي self
                            if (paramIdx >= funcInfo.parameters.size())
                                break;

                            SadTypeKind argType = inferExprType(newExpr->arguments[i].get());
                            SadTypeKind &paramType = funcInfo.parameters[paramIdx].type;

                            if (paramType == SadTypeKind::Integer && argType == SadTypeKind::String)
                                paramType = SadTypeKind::String;
                            else if (paramType == SadTypeKind::Integer && argType == SadTypeKind::Float)
                                paramType = SadTypeKind::Float;
                            else if (paramType == SadTypeKind::Integer && argType == SadTypeKind::Boolean)
                                paramType = SadTypeKind::Boolean;
                            else if (paramType == SadTypeKind::Integer && argType == SadTypeKind::Array)
                                paramType = SadTypeKind::Array;
                        }
                    }

                    // (AR) والترقيةُ في الشجرةِ كذلك: بانِي الصنفِ يقرأُ أنواعَ
                    //      معاملاتِه من الشجرةِ لا من `functionTable_`.
                    // (EN) Promote in the AST too: the class builder reads its constructor's
                    //      parameter types from the AST, not from `functionTable_`.
                    {
                        auto classIt = scanClassByName_.find(newExpr->className);
                        if (classIt != scanClassByName_.end())
                            for (const auto &member : classIt->second->members)
                                if (auto *ctor = dynamic_cast<Sad::AST::ConstructorDecl *>(member.get()))
                                {
                                    recordMemberParamArgs(ctor->parameters, newExpr->arguments, true);
                                    break;
                                }
                    }

                    // (AR) مسح وسائط NewExpr أيضاً
                    for (const auto &arg : newExpr->arguments)
                        scanCallSitesInExpr(arg.get());
                    return;
                }

                // ================================================================
                // (AR) استدعاء طريقة — MethodCallExpr
                //      استنتاج أنواع المعاملات للطرق الساكنة مثل: صنف.طريقة("نص")
                //      الاسم في b_.functionTable_ يكون "صنف.طريقة"
                // (EN) Method call — MethodCallExpr
                //      Infer param types for static methods like: Class.method("arg")
                //      Name in b_.functionTable_ is "Class.method"
                // ================================================================
                if (auto *methodCall = dynamic_cast<const Sad::AST::MethodCallExpr *>(expr))
                {
                    std::string funcName;
                    if (auto *varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(methodCall->object.get()))
                    {
                        funcName = varExpr->name + "." + methodCall->methodName;

                        // (AR) نداءٌ مؤهَّلٌ بفضاءِ وحدة («ح.تحية("سالم")»): الرمزُ مسطَّحٌ
                        //      في الجدول باسمه العاري لا بالمؤهَّل، فالمفتاحُ المؤهَّلُ
                        //      يغيب ⇒ يبقى المعاملُ Integer الافتراضيَّ ⇒ يُقرأ النصُّ
                        //      عنوانًا فيُطبع رقمٌ مكانه — ثمنُ قبولِ صيغةٍ جديدةٍ دون
                        //      إعلامِ الاستنتاجِ بها (ISSUE-090).
                        // (EN) A module-qualified call (`ح.تحية("سالم")`): the symbol lives in
                        //      the table under its bare, flattened name, so the qualified key
                        //      misses ⇒ the parameter stays the default Integer ⇒ the string is
                        //      read as an address and a number is printed in its place — the
                        //      price of accepting a new form without telling inference about it
                        //      (ISSUE-090).
                        // (AR) التشخيصُ يُهمَل هنا عمدًا — انظر النظيرَ في استنتاج النوع.
                        // (EN) The diagnostic is deliberately dropped — see the type-inference twin.
                        std::string ignoredAmbiguityDiagnostic;
                        if (b_.functionTable_.find(funcName) == b_.functionTable_.end() &&
                            b_.isModuleQualifiedSymbol(varExpr->name, methodCall->methodName,
                                                       ignoredAmbiguityDiagnostic))
                        {
                            funcName = methodCall->methodName;
                        }
                    }

                    if (!funcName.empty())
                    {
                        auto it = b_.functionTable_.find(funcName);
                        if (it != b_.functionTable_.end())
                        {
                            auto &funcInfo = it->second;

                            for (size_t i = 0; i < methodCall->arguments.size() && i < funcInfo.parameters.size(); i++)
                            {
                                SadTypeKind argType = inferExprType(methodCall->arguments[i].get());
                                SadTypeKind &paramType = funcInfo.parameters[i].type;

                                if (paramType == SadTypeKind::Integer && argType == SadTypeKind::String)
                                {
                                    paramType = SadTypeKind::String;
#ifdef SIR_BUILDER_DEBUG
                                    std::cerr << "[TYPE-INFER] " << funcName << " param[" << i
                                              << "] '" << funcInfo.parameters[i].name
                                              << "': I64 -> STRING (MethodCall)" << std::endl;
#endif
                                }
                                else if (paramType == SadTypeKind::Integer && argType == SadTypeKind::Float)
                                    paramType = SadTypeKind::Float;
                                else if (paramType == SadTypeKind::Integer && argType == SadTypeKind::Boolean)
                                    paramType = SadTypeKind::Boolean;
                                else if (paramType == SadTypeKind::String && argType != SadTypeKind::String)
                                {
                                    // (AR) STRING يبقى — التحويل يحدث عند موقع الاستدعاء
                                }
                                // (AR) نظيرُ ذراعِ الاستدعاءِ العاديّ: «أي» لا تُخفَّض هنا أيضًا.
                                // (EN) Twin of the plain-call arm: `Any` is not narrowed here either.
                                else if (paramType != SadTypeKind::Integer &&
                                         paramType != SadTypeKind::Any &&
                                         argType == SadTypeKind::String)
                                {
                                    paramType = SadTypeKind::String;
                                }
                            }
                        }
                    }

                    // (AR) وترقيةُ معاملاتِ الطريقةِ في الشجرة — تشملُ ما لا يبلغُه
                    //      المفتاحُ أعلاه: «صنف().طريقة(…)» و«متغيّر.طريقة(…)».
                    // (EN) Promote the method's parameters in the AST — covering what the key
                    //      above cannot reach: `Class().method(…)` and `variable.method(…)`.
                    refineCalledMember(methodCall->object.get(), methodCall->methodName,
                                       methodCall->arguments);

                    // (AR) مسح وسائط الطريقة (قد تحوي استدعاءات متداخلة)
                    for (const auto &arg : methodCall->arguments)
                        scanCallSitesInExpr(arg.get());
                    scanCallSitesInExpr(methodCall->object.get());
                    return;
                }

                // (AR) وصولٌ إلى عضوٍ بالصيغةِ الأخرى — `MemberExpr`. وغيابُها من المسحِ
                //      كان يقطعُ التعاودَ عندَ `ق(٢٫٥).قيمة`: فلا يُزارُ الإنشاءُ داخلَها
                //      أصلًا، ويبقى معامِلُ البانِي على الافتراضيِّ ⇒ عشريٌّ يُمرَّرُ في
                //      خانةِ صحيح. والصيغةُ الأخرى `MemberAccessExpr` مشمولةٌ أدناه —
                //      فالنقصُ كان في تغطيةِ العُقَدِ لا في منطقِ الترقية.
                // (EN) The other member-access form — `MemberExpr`. Its absence from the scan
                //      cut the recursion at `ق(2.5).قيمة`: the instantiation inside was never
                //      visited at all, so the constructor's parameter kept its default and a
                //      float was passed in an integer slot. The sibling form
                //      `MemberAccessExpr` is handled below — the gap was node coverage, not
                //      promotion logic.
                if (auto *memberExpr = dynamic_cast<const Sad::AST::MemberExpr *>(expr))
                {
                    scanCallSitesInExpr(memberExpr->object.get());
                    return;
                }

                // (AR) وصول إلى عضو — نمسح الكائن تعاودياً
                // (EN) Member access — scan object recursively
                if (auto *memberAccess = dynamic_cast<const Sad::AST::MemberAccessExpr *>(expr))
                {
                    scanCallSitesInExpr(memberAccess->object.get());
                    return;
                }

                // (AR) تعبير ثنائي
                // (EN) Binary expression
                if (auto *bin = dynamic_cast<const Sad::AST::BinaryExpr *>(expr))
                {
                    scanCallSitesInExpr(bin->left.get());
                    scanCallSitesInExpr(bin->right.get());
                    return;
                }

                // (AR) تعبير أحادي
                // (EN) Unary expression
                if (auto *unary = dynamic_cast<const Sad::AST::UnaryExpr *>(expr))
                {
                    scanCallSitesInExpr(unary->operand.get());
                    return;
                }
            }

            // ============================================================================
            // scanCallSitesInStmt - مسح الجمل بحثاً عن مواقع الاستدعاء تعاودياً
            // ============================================================================
            void TemplateBuilder::scanCallSitesInStmt(const Sad::AST::Statement *stmt)
            {
                if (!stmt)
                    return;

                // (AR) جملة كتلة
                // (EN) Block statement
                if (auto *block = dynamic_cast<const Sad::AST::BlockStmt *>(stmt))
                {
                    for (const auto &s : block->statements)
                    {
                        scanCallSitesInStmt(s.get());
                    }
                    return;
                }

                // (AR) جملة تعبير (تشمل استدعاءات الدوال المستقلة)
                // (EN) Expression statement (includes standalone function calls)
                if (auto *exprStmt = dynamic_cast<const Sad::AST::ExprStmt *>(stmt))
                {
                    scanCallSitesInExpr(exprStmt->expression.get());
                    return;
                }

                // (AR) إعلان متغير مع مُهيِّئ
                // (EN) Variable declaration with initializer
                if (auto *varDecl = dynamic_cast<const Sad::AST::VarDeclStmt *>(stmt))
                {
                    if (varDecl->initializer)
                    {
                        // (AR) رَبطُ الاسمِ بصنفِه حينَ يكونُ المُهيِّئُ إنشاءَ كائن: هو ما
                        //      يجعلُ `م.طريقة(...)` مالكُها معلومًا لا مخمَّنًا.
                        //
                        //      والخريطةُ مسطَّحةٌ بالاسمِ بلا نطاق: اسمٌ واحدٌ في دالّتَينِ
                        //      لصنفَينِ يتشاركُ المفتاح، ويُعادُ ربطُه بترتيبِ المسحِ فيصحُّ
                        //      عمليًّا. وهذا اتّكاءٌ مُعلَنٌ على ترتيبِ الزيارةِ لا على نطاقٍ
                        //      حقيقيّ؛ وسدُّه بخريطةٍ منطوقةٍ بالنطاقِ لا بحارسٍ إضافيّ.
                        // (EN) Bind the name to its class when the initializer instantiates one:
                        //      this is what makes the owner of `v.method(...)` known, not guessed.
                        //
                        //      The map is flat by name, with no scoping: one name used in two
                        //      functions for two classes shares the key and is rebound in scan
                        //      order, which happens to be correct. That is a declared reliance on
                        //      visit order rather than on real scoping; closing it needs a
                        //      scope-aware map, not an extra guard.
                        if (auto *newInit = dynamic_cast<const Sad::AST::NewExpr *>(varDecl->initializer.get()))
                        {
                            auto classIt = scanClassByName_.find(newInit->className);
                            if (classIt != scanClassByName_.end())
                                scanClassOfVariable_[varDecl->name] = classIt->second;
                        }
                        else if (auto *callInit = dynamic_cast<const Sad::AST::CallExpr *>(varDecl->initializer.get()))
                        {
                            if (auto *calleeVar = dynamic_cast<const Sad::AST::VariableExpr *>(callInit->callee.get()))
                            {
                                auto classIt = scanClassByName_.find(calleeVar->name);
                                if (classIt != scanClassByName_.end())
                                    scanClassOfVariable_[varDecl->name] = classIt->second;
                            }
                        }

                        // (AR) [GAP 3b] سجِّل نوعَ عنصر المصفوفة الحرفيّة المُهيّئة لهذا
                        //      المتغيّر، كي يُوسَّع أيُّ معاملٍ يُمرَّر إليه هذا المتغيّرُ
                        //      لاحقًا: مختلطٌ قياسيّ ⇒ Any (فتُقرأ فهرستُه موسومةً زمنَ
                        //      التشغيل لا عدديًّا)؛ متجانسٌ ⇒ نوعُه. نطابق تمامًا بوّابةَ
                        //      استنتاج المعامل من حرفيّةٍ مباشرة (نفس lambda القياسيّة
                        //      ونفس منطق التجانس). آمنٌ بعد الخيار ٢ (الحقل الخامس
                        //      homogKind أغلق فخَّ Any-عند-tags=null).
                        // (EN) [GAP 3b] Record the element type of this variable's literal
                        //      array initializer, so any param it is later passed to is
                        //      widened: scalar-heterogeneous ⇒ Any (its index reads
                        //      runtime-tagged, not as int); homogeneous ⇒ its type. Mirrors
                        //      the literal-arg param inference exactly (same scalar lambda,
                        //      same homogeneity logic). Safe after option 2 (the 5th field
                        //      homogKind closed the Any-at-tags==null hazard).
                        if (auto *arrExpr = dynamic_cast<const Sad::AST::ArrayExpr *>(varDecl->initializer.get()))
                        {
                            if (!arrExpr->elements.empty())
                            {
                                auto isScalarKind = [](SadTypeKind t) {
                                    return t == SadTypeKind::Integer || t == SadTypeKind::Float ||
                                           t == SadTypeKind::String || t == SadTypeKind::Boolean ||
                                           t == SadTypeKind::UInt8 || t == SadTypeKind::UInt64 ||
                                           t == SadTypeKind::Null || t == SadTypeKind::Any;
                                };
                                SadTypeKind firstElemType = inferExprType(arrExpr->elements[0].get());
                                bool homogeneous = true, allScalar = true, hasAny = false;
                                for (const auto &el : arrExpr->elements)
                                {
                                    SadTypeKind et = inferExprType(el.get());
                                    if (et != firstElemType)
                                        homogeneous = false;
                                    if (!isScalarKind(et))
                                        allScalar = false;
                                    if (et == SadTypeKind::Any)
                                        hasAny = true;
                                }
                                // (AR) [عيب أميليا 11أ/ب] نُسجّل Any حصرًا، لا نوعًا محدَّدًا.
                                //      تسجيلُ نوعٍ متجانسٍ محدَّد (نصّ/عشريّ) ثمّ تجميدُه
                                //      «أوّلَ-موقعٍ» على معاملٍ مشترَكٍ يُصيّر قراءةَ مصفوفةٍ
                                //      صحيحةٍ (متجانسة) في موقعٍ لاحقٍ عبر مسار النصّ/العشريّ
                                //      ⇒ انهيار (فكُّ عدد كـ char*) أو قمامة. أمّا Any فآمنٌ
                                //      للجميع بعد الخيار ٢: قراءةُ Any عند tags=null تقرأ
                                //      homogKind فتُبوَّب كلُّ مصفوفةٍ متجانسةٍ صحيحةً.
                                //      لذا نُوسِّع إلى Any عند: مختلطٍ قياسيّ، أو متجانسٍ
                                //      قياسيٍّ غيرِ صحيحٍ (نصّ/عشريّ/منطقيّ/بايت/طبيعي/عدم).
                                //      المتجانسُ الصحيحُ لا يُسجَّل (مسارُه العدديُّ الافتراضيّ
                                //      آمنٌ ومطابق)، وغيرُ القياسيّ (مصفوفةُ مصفوفات) يُترَك
                                //      لمساره الافتراضيّ (لا تجميدَ نوعٍ محدَّدٍ خطِرٍ).
                                // (EN) [Amelia 11a/b] Record ONLY Any, never a concrete type.
                                //      Recording a concrete homogeneous type (String/Float)
                                //      then freezing it first-site-wins onto a SHARED param
                                //      makes a later call passing a (homogeneous) int array
                                //      read via the string/float path ⇒ SIGSEGV (int deref as
                                //      char*) or garbage. Any is safe for all after option 2:
                                //      reading via Any at tags==null reads homogKind, so every
                                //      homogeneous array reads correctly. So widen to Any when:
                                //      scalar-mixed, or scalar-homogeneous-non-int. Homogeneous
                                //      int is not recorded (its default numeric path is safe
                                //      and matches); non-scalar (array of arrays) is left to
                                //      its default path (no dangerous concrete-type freeze).
                                bool recordAny = allScalar &&
                                                 (((!homogeneous && arrExpr->elements.size() > 1) || hasAny) ||
                                                  (homogeneous && firstElemType != SadTypeKind::Integer));
                                if (recordAny)
                                    b_.scanVarElementType_[b_.currentScanFuncName_ + "#" + varDecl->name] = SadTypeKind::Any;
                            }
                        }
                        // (AR) [GAP 4] إن كان المُهيّئُ لامدا، اربط اسمَ المتغيّر بعقدتها،
                        //      كي نجدَ معاملاتِها من اسم المتغيّر عند موقع النداء لاحقًا.
                        // (EN) [GAP 4] If the initializer is a lambda, bind the variable name
                        //      to its node, so we can find its params from the callee name at
                        //      a later call site.
                        if (auto *lam = dynamic_cast<const Sad::AST::LambdaExpr *>(varDecl->initializer.get()))
                            b_.scanLambdaVar_[b_.currentScanFuncName_ + "#" + varDecl->name] = lam;

                        // (AR) [موجة ABI المغاليق] ربطُ أصلِ مرجعِ الدالّةِ المسمّاة:
                        //      «متغير د = اسم_دالّة» يُسجَّل بمفتاحٍ مُنطاقٍ. وأيُّ تصريحٍ
                        //      ثانٍ بالمفتاحِ نفسِه من عقدةٍ مختلفةٍ — مرجعًا آخرَ كان أو
                        //      مُهيِّئًا من غيرِ جنسِه — يُسمِّمُ المفتاحَ، فلا يُنزَعُ
                        //      توسُّطُ اسمٍ مُظلَّلٍ أو مُعادٍ. (التفصيلُ عند تعريفِ
                        //      الخرائطِ في sir_builder_context.h.)
                        // (EN) [Closure-ABI wave] Bind named function-ref provenance:
                        //      «متغير د = funcName» records under a scoped key. Any second
                        //      declaration of the same key from a different node — another
                        //      ref or a non-ref initializer alike — poisons the key, so a
                        //      shadowed or redeclared name is never devirtualized. (Details
                        //      at the map definitions in sir_builder_context.h.)
                        {
                            const std::string bindKey =
                                b_.currentScanFuncName_ + "#" + varDecl->name;
                            auto declIt = b_.scanFuncRefDeclNode_.find(bindKey);
                            if (declIt != b_.scanFuncRefDeclNode_.end() &&
                                declIt->second != static_cast<const void *>(varDecl))
                            {
                                b_.scanFuncRefPoisoned_.insert(bindKey);
                            }
                            b_.scanFuncRefDeclNode_[bindKey] = varDecl;

                            auto *refVar = dynamic_cast<const Sad::AST::VariableExpr *>(
                                varDecl->initializer.get());
                            if (refVar &&
                                b_.functionTable_.find(refVar->name) != b_.functionTable_.end())
                            {
                                b_.scanFuncRefBindings_[bindKey] = refVar->name;
                            }
                        }

                        // (AR) [ز.٢٠] سجّل نوعَ المُهيّئ القياسيّ كي يحلَّ `inferExprType`
                        //      المتغيّرَ المحلّيَّ بدل السقوط إلى `Integer` الافتراضيّ.
                        //      النوعُ المصرَّح يسبق المستنتَجَ (`نص س = ...` قاطع).
                        //
                        //      🔑 **النصُّ وحدَه يُسجَّل.** جرّبتُ القياسيَّ كلَّه (عشريّ/منطقيّ/
                        //      بايت) فسقط توليدُ LLVM على تأكيدِ «نداءٌ بتوقيعٍ فاسد»: منطقُ
                        //      التضاربِ في تحديثِ المعامل (أعلاه) يدعم النصَّ حصرًا — النصُّ
                        //      يلتصق ويُحوَّل الوسيطُ في موقعِ النداء (BOOL_TO_STRING/
                        //      I64_TO_STRING) — ولا نظيرَ لذلك للعشريِّ والمنطقيّ، فترقيةُ
                        //      معامِلٍ مشترَكٍ إليهما تخالف توقيعَ الجسم المبنيّ.
                        //      وهو نفسُ درسِ «الموقع الأوّل يفوز» في GAP 3b: لا تُجمّد نوعًا
                        //      محدَّدًا لا يملك مسارَ مصالحةٍ عند موقعٍ لاحقٍ مختلف.
                        // (EN) [ز.٢٠] Record the initializer type so inferExprType can resolve
                        //      the local variable instead of defaulting to Integer. A declared
                        //      type wins over an inferred one.
                        //      🔑 STRING ONLY. Recording every scalar (Float/Boolean/Byte) was
                        //      tried and crashed LLVM codegen with "Calling a function with a
                        //      bad signature!": the conflict logic in the parameter update above
                        //      supports String exclusively — String sticks and the ARGUMENT is
                        //      converted at the call site (BOOL_TO_STRING/I64_TO_STRING) — with
                        //      no counterpart for Float/Boolean, so promoting a shared parameter
                        //      to those contradicts the already-built body's signature.
                        //      Same lesson as GAP 3b's first-site-wins: never freeze a concrete
                        //      type that has no reconciliation path at a differing later site.
                        {
                            const SadTypeKind declared = b_.astTypeToSIRType(varDecl->type);
                            const SadTypeKind initType =
                                (declared == SadTypeKind::String)
                                    ? declared
                                    : inferExprType(varDecl->initializer.get());
                            if (initType == SadTypeKind::String)
                            {
                                b_.scanLocalVarType_[b_.currentScanFuncName_ + "#" + varDecl->name] =
                                    SadTypeKind::String;
                            }
                        }

                        scanCallSitesInExpr(varDecl->initializer.get());
                    }
                    return;
                }

                // (AR) جملة شرطية
                // (EN) If statement
                if (auto *ifStmt = dynamic_cast<const Sad::AST::IfStmt *>(stmt))
                {
                    scanCallSitesInExpr(ifStmt->condition.get());
                    scanCallSitesInStmt(ifStmt->thenBranch.get());
                    if (ifStmt->elseBranch)
                        scanCallSitesInStmt(ifStmt->elseBranch.get());
                    return;
                }

                // (AR) حلقة while
                // (EN) While loop
                if (auto *whileStmt = dynamic_cast<const Sad::AST::WhileStmt *>(stmt))
                {
                    scanCallSitesInExpr(whileStmt->condition.get());
                    scanCallSitesInStmt(whileStmt->body.get());
                    return;
                }

                // (AR) حلقة for-range
                // (EN) For-range loop
                if (auto *forStmt = dynamic_cast<const Sad::AST::ForRangeStmt *>(stmt))
                {
                    scanCallSitesInExpr(forStmt->iterable.get());

                    // ================================================================
                    // (AR) إصلاح X08: استنتاج نوع متغير الحلقة من نوع عنصر المصفوفة
                    //      بدون هذا: لكل أ في ["نص1"، "نص2"] { دالة(أ) }
                    //                  inferExprType(أ) = Integer لأن أ غير مسجّل
                    //      مما يُبقي نوع معامل الدالة Integer بدلاً من String
                    // (EN) FIX X08: Infer loop variable type from iterable element type
                    //      Without this: for x in ["str1", "str2"] { func(x) }
                    //                    inferExprType(x) = Integer since x is not registered
                    //      This prevents param type from being updated to String
                    // ================================================================
                    SadTypeKind iterVarType = SadTypeKind::Integer; // افتراضي

                    // (AR) استنتاج نوع العنصر من ArrayExpr مباشرة
                    // (EN) Infer element type from direct ArrayExpr
                    if (auto *arrExpr = dynamic_cast<const Sad::AST::ArrayExpr *>(forStmt->iterable.get()))
                    {
                        if (!arrExpr->elements.empty())
                        {
                            SadTypeKind elemType = inferExprType(arrExpr->elements[0].get());
                            if (elemType != SadTypeKind::Integer) // Integer = افتراضي، يعني لم يُستنتج
                                iterVarType = elemType;
                        }
                    }
                    // (AR) أو من متغير عالمي مسجّل بـ elementType
                    // (EN) Or from a global variable registered with elementType
                    else if (auto *varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(forStmt->iterable.get()))
                    {
                        for (auto scopeIt = b_.scopeStack_.rbegin(); scopeIt != b_.scopeStack_.rend(); ++scopeIt)
                        {
                            auto it = scopeIt->find(varExpr->name);
                            if (it != scopeIt->end())
                            {
                                if (it->second.elementType != SadTypeKind::Integer &&
                                    it->second.elementType != SadTypeKind::Void)
                                {
                                    iterVarType = it->second.elementType;
                                }
                                break;
                            }
                        }
                    }

                    // (AR) تسجيل متغير الحلقة في نطاق مؤقت أثناء المسح
                    // (EN) Register loop variable in a temporary scope during scanning
                    if (iterVarType != SadTypeKind::Integer) // فقط إذا استنتجنا نوعاً محدداً
                    {
                        b_.enterScope();
                        VariableInfo iterVarInfo;
                        iterVarInfo.name = forStmt->variable;
                        iterVarInfo.type = iterVarType;
                        iterVarInfo.registerName = "%" + forStmt->variable;
                        b_.addVariable(iterVarInfo);
                        scanCallSitesInStmt(forStmt->body.get());
                        b_.exitScope();
                    }
                    else
                    {
                        scanCallSitesInStmt(forStmt->body.get());
                    }
                    return;
                }

                // (AR) جملة return
                // (EN) Return statement
                if (auto *retStmt = dynamic_cast<const Sad::AST::ReturnStmt *>(stmt))
                {
                    if (retStmt->value)
                        scanCallSitesInExpr(retStmt->value.get());
                    return;
                }

                // (AR) جملة حاول/امسك/أخيراً — يجب مسح كتل الثلاثة
                //      إصلاح X08: بدون هذا كانت الاستدعاءات داخل try لا تُفحص
                // (EN) TryCatch statement — must scan all three blocks
                //      FIX X08: Without this, calls inside try were not scanned
                if (auto *tryStmt = dynamic_cast<const Sad::AST::TryStmt *>(stmt))
                {
                    if (tryStmt->tryBlock)
                        scanCallSitesInStmt(tryStmt->tryBlock.get());
                    for (const auto &clause : tryStmt->catchClauses)
                    {
                        if (clause.body)
                            scanCallSitesInStmt(clause.body.get());
                    }
                    if (tryStmt->finallyBlock)
                        scanCallSitesInStmt(tryStmt->finallyBlock.get());
                    return;
                }

                // (AR) جملة أطلق (goroutine) — يجب فحص تعبير الاستدعاء وكتلة الجسم
                //      بدون هذا: أطلق دالة("نص") لا يُستنتج نوع المعامل كـ String
                //      مما يؤدي لمعاملة المعامل كـ Integer داخل الدالة
                // (EN) GoStmt (goroutine) — must scan call expression and block body
                //      Without this: go func("text") doesn't infer param as String
                //      causing the parameter to be treated as Integer inside the function
                if (auto *goStmt = dynamic_cast<const Sad::AST::GoStmt *>(stmt))
                {
                    if (goStmt->expression)
                        scanCallSitesInExpr(goStmt->expression.get());
                    if (goStmt->blockBody)
                        scanCallSitesInStmt(goStmt->blockBody.get());
                    return;
                }

                // (AR) جملة حالة/switch وطابق/match — يجب فحص أجسام الفروع
                //      بدون هذا: دالّة("نص") داخل فرعٍ لا يُستنتَج نوع معاملها ⇒ يُعامَل i64.
                // (EN) Switch/Match statements — must scan case bodies; without this a
                //      call inside an arm doesn't get its param types inferred (treated i64).
                if (auto *switchStmt = dynamic_cast<const Sad::AST::SwitchStmt *>(stmt))
                {
                    for (const auto &caseBranch : switchStmt->cases)
                        if (caseBranch.body)
                            scanCallSitesInStmt(caseBranch.body.get());
                    if (switchStmt->defaultCase)
                        scanCallSitesInStmt(switchStmt->defaultCase.get());
                    return;
                }
                if (auto *matchStmt = dynamic_cast<const Sad::AST::MatchStmt *>(stmt))
                {
                    for (const auto &matchCase : matchStmt->cases)
                    {
                        // (AR) ISSUE-076/084 (ب″): سجّل متغيرات ربط نمط حالة ADT في نطاقٍ مؤقّت
                        //      بنوعها (من adtEnumTable_ إن سُجِّل، وإلّا Any) قبل مسح الجسم، كي
                        //      يستنتج inferExprType نوعها فيُرقّى معامِلُ دالةٍ تتلقّاها (`زد(س)`)
                        //      إلى Any ⇒ يعمل حسابها في الخلف عبر فكّ التعليب الديناميّ (ISSUE-084:
                        //      تمرير حمولة عشريّة إلى دالةٍ مساعِدة). النوع في Phase 1.7 غالبًا Unknown
                        //      (لم يُبنَ موقع الإنشاء بعد) ⇒ Any — وهو المطلوب للمسار الديناميّ.
                        // (EN) ISSUE-076/084 (ب″): register ADT variant-pattern binding variables in
                        //      a temp scope with their type (from adtEnumTable_ if registered, else
                        //      Any) before scanning the body, so inferExprType resolves them and a
                        //      function receiving one (`زد(س)`) has its param widened to Any ⇒ its
                        //      arithmetic uses the backend dynamic unbox path (ISSUE-084: passing a
                        //      float payload to a helper). In Phase 1.7 the type is usually Unknown
                        //      (the construction site isn't built yet) ⇒ Any — exactly what the
                        //      dynamic path needs.
                        bool pushed = false;
                        if (auto *enumPat = dynamic_cast<const Sad::AST::EnumVariantPattern *>(matchCase.pattern.get()))
                        {
                            const ADTEnumInfo *adt = nullptr;
                            auto adtIt = b_.adtEnumTable_.find(enumPat->enumName);
                            if (adtIt != b_.adtEnumTable_.end())
                                adt = &adtIt->second;
                            b_.enterScope();
                            pushed = true;
                            for (size_t fi = 0; fi < enumPat->fieldPatterns.size(); ++fi)
                            {
                                if (auto *vp = dynamic_cast<const Sad::AST::VariablePattern *>(enumPat->fieldPatterns[fi].get()))
                                {
                                    SadTypeKind ft = SadTypeKind::Any;
                                    if (adt)
                                    {
                                        for (const auto &v : adt->variants)
                                        {
                                            if (v.name == enumPat->variantName)
                                            {
                                                SadTypeKind reg = v.fieldTypeAt(fi);
                                                if (reg != SadTypeKind::Unknown)
                                                    ft = reg;
                                                break;
                                            }
                                        }
                                    }
                                    VariableInfo bindInfo;
                                    bindInfo.name = vp->name;
                                    bindInfo.type = ft;
                                    bindInfo.registerName = "%" + vp->name;
                                    b_.addVariable(bindInfo);
                                }
                            }
                        }
                        for (const auto &bodyStmt : matchCase.body)
                            scanCallSitesInStmt(bodyStmt.get());
                        if (pushed)
                            b_.exitScope();
                    }
                    return;
                }
            }

            // ============================================================================

            // ============================================================================
            // inferLambdaParamFromExpr — استنتاج أنواع معاملات اللامبدا من التعبيرات
            // ============================================================================
            void TemplateBuilder::inferLambdaParamFromExpr(
                const ::Sad::AST::Expression *expr,
                const std::set<std::string> &paramNames,
                std::unordered_map<std::string, SadTypeKind> &result)
            {
                if (!expr)
                    return;

                // ────────────────────────────────────────────────────
                // (AR) تعبير ثنائي — الحالة الأكثر شيوعاً
                //      مثال: "مرحباً " + اسم → اسم يصبح String
                //      مثال: س + 3.14 → س يصبح Float
                // (EN) Binary expression — most common case
                // ────────────────────────────────────────────────────
                if (auto *bin = dynamic_cast<const ::Sad::AST::BinaryExpr *>(expr))
                {
                    inferLambdaParamFromExpr(bin->left.get(), paramNames, result);
                    inferLambdaParamFromExpr(bin->right.get(), paramNames, result);

                    // (AR) فحص الطرفين: إذا كان أحد الطرفين معامل لامبدا والآخر له نوع معروف
                    //      نستنتج نوع المعامل من السياق
                    //      مثال: "مرحباً " + اسم → اسم يصبح String
                    //      مثال: س + 3.14 → س يصبح Float
                    // (EN) Check both sides: if one side is a lambda param and other has known type
                    //      infer param type from context
                    auto checkSide = [&](const ::Sad::AST::Expression *paramSide, const ::Sad::AST::Expression *otherSide)
                    {
                        auto *varA = dynamic_cast<const ::Sad::AST::VariableExpr *>(paramSide);
                        if (!varA || paramNames.find(varA->name) == paramNames.end())
                            return;
                        if (result.count(varA->name) > 0)
                            return;
                        SadTypeKind otherType = inferExprType(otherSide);
                        if (otherType == SadTypeKind::String)
                            result[varA->name] = SadTypeKind::String;
                        else if (otherType == SadTypeKind::Float)
                            result[varA->name] = SadTypeKind::Float;
                    };
                    checkSide(bin->left.get(), bin->right.get());
                    checkSide(bin->right.get(), bin->left.get());
                    // (AR) أولوية الاستنتاج: عمليات المقارنة العددية (>، <، >=، <=) تُلغي String
                    //      مثال: إذا (عمر >= 30) → عمر = Integer حتى لو كان مستنتجاً كـ String
                    //      من جملة concat سابقة مثل ("نص" + عمر)
                    //      الحالة المطلوبة: lambda(اسم، عمر) { ارجع "نص"+عمر; إذا(عمر>=30)... }
                    //      اسم = String (من concat)، عمر = Integer (من المقارنة — أولوية أعلى)
                    // (EN) Inference priority: numeric comparison ops override String inference
                    //      e.g.: if (age >= 30) → age = Integer even if previously String
                    //      from a concat like ("text" + age)
                    {
                        using TT = Sad::Lexer::TokenType;
                        if (bin->op == TT::OP_GREATER || bin->op == TT::OP_LESS ||
                            bin->op == TT::OP_GREATER_EQUAL || bin->op == TT::OP_LESS_EQUAL)
                        {
                            auto numericOverride = [&](const ::Sad::AST::Expression *ps, const ::Sad::AST::Expression *os)
                            {
                                auto *v = dynamic_cast<const ::Sad::AST::VariableExpr *>(ps);
                                if (!v || paramNames.find(v->name) == paramNames.end())
                                    return;
                                SadTypeKind ot = inferExprType(os);
                                if (ot == SadTypeKind::Integer || ot == SadTypeKind::Float)
                                    result[v->name] = ot; // (AR) يُلغي String لصالح النوع العددي
                            };
                            numericOverride(bin->left.get(), bin->right.get());
                            numericOverride(bin->right.get(), bin->left.get());
                        }
                    }
                    return;
                }

                // (AR) استدعاء دالة
                if (auto *call = dynamic_cast<const ::Sad::AST::CallExpr *>(expr))
                {
                    for (const auto &arg : call->arguments)
                        inferLambdaParamFromExpr(arg.get(), paramNames, result);
                    inferLambdaParamFromExpr(call->callee.get(), paramNames, result);

                    std::string funcName;
                    if (auto *varExpr = dynamic_cast<const ::Sad::AST::VariableExpr *>(call->callee.get()))
                        funcName = varExpr->name;

                    if (!funcName.empty())
                    {
                        auto funcIt = b_.functionTable_.find(funcName);
                        if (funcIt != b_.functionTable_.end())
                        {
                            const auto &funcInfo = funcIt->second;
                            for (size_t i = 0; i < call->arguments.size() && i < funcInfo.parameters.size(); i++)
                            {
                                auto *argVar = dynamic_cast<const ::Sad::AST::VariableExpr *>(call->arguments[i].get());
                                if (!argVar || paramNames.find(argVar->name) == paramNames.end())
                                    continue;
                                if (result.count(argVar->name) > 0)
                                    continue;

                                SadTypeKind targetType = funcInfo.parameters[i].type;
                                if (targetType != SadTypeKind::Integer)
                                    result[argVar->name] = targetType;
                            }
                        }
                    }
                    return;
                }

                // (AR) استدعاء طريقة — نمسح كل الأجزاء
                if (auto *methodCall = dynamic_cast<const ::Sad::AST::MethodCallExpr *>(expr))
                {
                    inferLambdaParamFromExpr(methodCall->object.get(), paramNames, result);
                    for (const auto &arg : methodCall->arguments)
                        inferLambdaParamFromExpr(arg.get(), paramNames, result);

                    auto *objVar = dynamic_cast<const ::Sad::AST::VariableExpr *>(methodCall->object.get());
                    if (objVar && paramNames.count(objVar->name) > 0 && result.count(objVar->name) == 0)
                    {
                        if (kStringMethodNames.count(methodCall->methodName) > 0)
                            result[objVar->name] = SadTypeKind::String;
                        else if (kArrayMethodNames.count(methodCall->methodName) > 0)
                            result[objVar->name] = SadTypeKind::Array;
                    }
                    return;
                }

                // (AR) تعبير أحادي
                if (auto *unary = dynamic_cast<const ::Sad::AST::UnaryExpr *>(expr))
                {
                    inferLambdaParamFromExpr(unary->operand.get(), paramNames, result);
                    return;
                }
                // (AR) تعبير شرطي ثلاثي
                if (auto *tern = dynamic_cast<const ::Sad::AST::TernaryExpr *>(expr))
                {
                    inferLambdaParamFromExpr(tern->condition.get(), paramNames, result);
                    inferLambdaParamFromExpr(tern->trueExpr.get(), paramNames, result);
                    inferLambdaParamFromExpr(tern->falseExpr.get(), paramNames, result);
                    return;
                }
                // (AR) وصول فهرس
                if (auto *idx = dynamic_cast<const ::Sad::AST::IndexExpr *>(expr))
                {
                    inferLambdaParamFromExpr(idx->object.get(), paramNames, result);
                    inferLambdaParamFromExpr(idx->index.get(), paramNames, result);
                    return;
                }
                // (AR) وصول عضو
                if (auto *mem = dynamic_cast<const ::Sad::AST::MemberAccessExpr *>(expr))
                {
                    inferLambdaParamFromExpr(mem->object.get(), paramNames, result);
                    return;
                }
                // (AR) مصفوفة حرفية
                if (auto *arrLit = dynamic_cast<const ::Sad::AST::ArrayExpr *>(expr))
                {
                    for (const auto &el : arrLit->elements)
                        inferLambdaParamFromExpr(el.get(), paramNames, result);
                    return;
                }
                // (AR) قيمة حرفية — لا تحوي معاملات، لا شيء يُستنتج
            }

            // ============================================================================
            // inferLambdaParamFromStmt — استنتاج أنواع معاملات اللامبدا من الجمل
            // ============================================================================
            void TemplateBuilder::inferLambdaParamFromStmt(
                const ::Sad::AST::Statement *stmt,
                const std::set<std::string> &paramNames,
                std::unordered_map<std::string, SadTypeKind> &result)
            {
                if (!stmt)
                    return;

                if (auto *block = dynamic_cast<const ::Sad::AST::BlockStmt *>(stmt))
                {
                    for (const auto &s : block->statements)
                        inferLambdaParamFromStmt(s.get(), paramNames, result);
                    return;
                }
                if (auto *exprStmt = dynamic_cast<const ::Sad::AST::ExprStmt *>(stmt))
                {
                    if (exprStmt->expression)
                        inferLambdaParamFromExpr(exprStmt->expression.get(), paramNames, result);
                    return;
                }
                if (auto *retStmt = dynamic_cast<const ::Sad::AST::ReturnStmt *>(stmt))
                {
                    if (retStmt->value)
                        inferLambdaParamFromExpr(retStmt->value.get(), paramNames, result);
                    return;
                }
                if (auto *varDecl = dynamic_cast<const ::Sad::AST::VarDeclStmt *>(stmt))
                {
                    if (varDecl->initializer)
                        inferLambdaParamFromExpr(varDecl->initializer.get(), paramNames, result);
                    return;
                }
                if (auto *ifStmt = dynamic_cast<const ::Sad::AST::IfStmt *>(stmt))
                {
                    inferLambdaParamFromExpr(ifStmt->condition.get(), paramNames, result);
                    inferLambdaParamFromStmt(ifStmt->thenBranch.get(), paramNames, result);
                    if (ifStmt->elseBranch)
                        inferLambdaParamFromStmt(ifStmt->elseBranch.get(), paramNames, result);
                    return;
                }
                if (auto *whileStmt = dynamic_cast<const ::Sad::AST::WhileStmt *>(stmt))
                {
                    inferLambdaParamFromExpr(whileStmt->condition.get(), paramNames, result);
                    inferLambdaParamFromStmt(whileStmt->body.get(), paramNames, result);
                    return;
                }
                if (auto *forStmt = dynamic_cast<const ::Sad::AST::ForRangeStmt *>(stmt))
                {
                    inferLambdaParamFromExpr(forStmt->iterable.get(), paramNames, result);
                    inferLambdaParamFromStmt(forStmt->body.get(), paramNames, result);
                    return;
                }
            }

            // ============================================================================
            // b_.inferLambdaParamTypes — استنتاج أنواع معاملات اللامبدا من الجسم
            // ============================================================================
            std::unordered_map<std::string, SadTypeKind> SIRBuilder::inferLambdaParamTypes(
                ::Sad::AST::LambdaExpr *lambdaExpr,
                const std::set<std::string> &paramNames)
            {
                std::unordered_map<std::string, SadTypeKind> result;

                // (AR) الخطوة 1: أخذ أنواع AST الصريحة
                for (const auto &param : lambdaExpr->parameters)
                {
                    if (param.type != Types::SadTypeKind::Unknown)
                        result[param.name] = astTypeToSIRType(param.type);
                }
                if (result.size() == lambdaExpr->parameters.size())
                    return result;

                // (AR) الخطوة 2: تحليل جسم اللامبدا
                if (lambdaExpr->body)
                    inferLambdaParamFromExpr(lambdaExpr->body.get(), paramNames, result);
                if (lambdaExpr->blockBody)
                    inferLambdaParamFromStmt(lambdaExpr->blockBody.get(), paramNames, result);

#ifndef NDEBUG
                for (const auto &kv : result)
                {
                    SAD_DEBUG_LOG_LINE("[DEBUG] Lambda param '" << kv.first << "' inferred type: "
                              << static_cast<int>(kv.second));
                }
#endif

                return result;
            }

            // ============================================================================
            // inferParamTypesFromCallSites - المرحلة 1.7: استنتاج أنواع المعاملات
            // ============================================================================
            // (AR) يمسح البرنامج بأكمله بحثاً عن مواقع الاستدعاء ويحدّث
            //      أنواع المعاملات في b_.functionTable_ عندما يكون النوع I64
            //      (من Unknown) والوسيط الفعلي أكثر تحديداً
            // (EN) Scans the entire program for call sites and updates parameter
            //      types in b_.functionTable_ when type is I64 (from Unknown)
            //      and actual argument is more specific
            // ============================================================================
            void TemplateBuilder::inferParamTypesFromCallSites(AST::ProgramNode *program)
            {
                if (!program)
                    return;

                // (AR) دالّة مسحٍ لقائمة جملٍ واحدة (تُطبَّق على البرنامج الرئيس ثمّ على
                //      أجسام الوحدات المستوردة المجموعة في preRegisterImportedSignatures)
                // (EN) Scan one statement list (applied to the main program then to the
                //      imported module bodies collected in preRegisterImportedSignatures)
                // ════════════════════════════════════════════════════════════════════
                // (AR) فهرسةُ الأصنافِ أوّلًا بالاسمِ لحلِّ «صنف(…)» و«صنف().طريقة(…)».
                //      ولا تُفهرَسُ أسماءُ الطرائق: مالكُ «متغيّر.طريقة(…)» يُحسَمُ من رَبطِ
                //      المتغيّرِ بصنفِه، ومالكُ «هذا.طريقة(…)» هو الصنفُ المحيط — يقينًا
                //      لا بتفرّدِ اسمٍ يُخمَّنُ به.
                // (EN) Index the classes by name first, to resolve `Class(…)` and
                //      `Class().method(…)`. Method names are NOT indexed: the owner of
                //      `variable.method(…)` comes from the variable-to-class binding, and the
                //      owner of `this.method(…)` is the enclosing class — with certainty,
                //      not by guessing at name uniqueness.
                // ════════════════════════════════════════════════════════════════════
                scanClassByName_.clear();
                scanClassOfVariable_.clear();
                b_.paramClassBindings_.clear();
                scanMemberArgKinds_.clear();
                // (AR) 🔑 ونظيرتاهما للدوالِّ الحرّة. المسحُ يجري ثلاثةَ أشواط، و
                //      `inferExprType` يتغيّر بينها عمدًا (وهو ما يقوده
                //      `reinferReturnTypes`). فبلا مسحٍ يتراكم كِنْهانِ لخانةٍ لم
                //      يختلف عليها موقعانِ قطُّ — أوضحُها معامِلٌ تعاوديّ: يُسجَّل
                //      كِنْهُه قبلَ الترقيةِ في الشوطِ الأوّلِ وبعدَها في الثاني من
                //      **موقعٍ واحد** — فيُقرَأ ذلك «خلافًا» ويُنقَض إلى «أي» بلا خلاف.
                // (EN) Their free-function counterparts. The scan runs three passes and
                //      inferExprType deliberately changes between them (that is what
                //      reinferReturnTypes drives). Without clearing, two kinds accumulate
                //      for a slot no two sites ever disagreed on — clearest with a
                //      recursive parameter, which records its pre-promotion kind on pass 1
                //      and its promoted kind on pass 2 from a SINGLE site, and is then
                //      revoked to Any on a disagreement that never happened.
                scanFreeArgKinds_.clear();
                scanFreeArgClasses_.clear();
                {
                    std::function<void(Sad::AST::StmtList *)> indexClasses =
                        [&](Sad::AST::StmtList *stmts)
                    {
                        if (!stmts)
                            return;
                        for (const auto &stmt : *stmts)
                        {
                            auto *classDecl = asClassDecl(stmt.get());
                            if (!classDecl)
                                continue;
                            scanClassByName_[classDecl->name] = classDecl;
                        }
                    };
                    // (AR) والفهرسةُ تشملُ الوحداتِ المستوردةَ كما يشملُها المسحُ نفسُه:
                    //      فهرسٌ أضيقُ من المسحِ يعني مالكًا لا يُحسَمُ حيثُ يُمسَحُ نداؤه.
                    // (EN) Indexing covers imported modules exactly as the scan does: an index
                    //      narrower than the scan means an owner left unresolved where its call
                    //      site is nonetheless scanned.
                    indexClasses(program);
                    for (Sad::AST::StmtList *body : b_.importedModuleBodies_)
                        indexClasses(body);
                }

                auto scanStmtList = [&](Sad::AST::StmtList *stmts)
                {
                    if (!stmts)
                        return;
                    for (const auto &stmt : *stmts)
                    {
                        if (!stmt)
                            continue;

                        // (AR) مسحُ أجسامِ أعضاءِ الأصناف — كانت خارجَ المسحِ بالكلّيّة،
                        //      فنداءٌ داخلَ طريقةٍ لا يُرقّي شيئًا.
                        // (EN) Scan class member bodies — they were outside the scan entirely,
                        //      so a call inside a method promoted nothing.
                        if (auto *classDecl = asClassDecl(stmt.get()))
                        {
                            Sad::AST::ClassDecl *previousEnclosing = scanEnclosingClass_;
                            scanEnclosingClass_ = classDecl;
                            for (const auto &member : classDecl->members)
                            {
                                if (auto *ctor = dynamic_cast<Sad::AST::ConstructorDecl *>(member.get()))
                                {
                                    if (ctor->body)
                                        scanCallSitesInStmt(ctor->body.get());
                                }
                                else if (auto *method = dynamic_cast<Sad::AST::MethodDecl *>(member.get()))
                                {
                                    if (method->body)
                                        scanCallSitesInStmt(method->body.get());
                                }
                            }
                            scanEnclosingClass_ = previousEnclosing;
                            continue;
                        }

                        // (AR) مسح أجسام الدوال
                        // (EN) Scan function bodies
                        AST::FunctionDecl *funcDecl = nullptr;
                        if (auto fd = dynamic_cast<Sad::AST::FunctionDecl *>(stmt.get()))
                        {
                            funcDecl = fd;
                        }
                        else if (auto exportDecl = dynamic_cast<Sad::AST::ExportDecl *>(stmt.get()))
                        {
                            if (exportDecl->declaration)
                            {
                                funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(exportDecl->declaration.get());
                            }
                        }
                        else if (auto exportStmt = dynamic_cast<Sad::AST::ExportStmt *>(stmt.get()))
                        {
                            if (exportStmt->declaration)
                            {
                                funcDecl = dynamic_cast<Sad::AST::FunctionDecl *>(exportStmt->declaration.get());
                            }
                        }

                        if (funcDecl && funcDecl->body)
                        {
                            b_.currentScanFuncName_ = funcDecl->name;
                            scanCallSitesInStmt(funcDecl->body.get());
                            b_.currentScanFuncName_.clear();
                        }

                        // (AR) مسح الجمل التنفيذية في المستوى الأعلى
                        // (EN) Scan top-level executable statements
                        scanCallSitesInStmt(stmt.get());
                    }
                };

                // ════════════════════════════════════════════════════════════════════
                // (AR) [ز.٣٥] إعادةُ استنتاجِ أنواعِ الإرجاعِ بين التمريرات.
                //
                //      نوعُ الإرجاعِ المُستنتَجُ يُحسَب **قبل** هذه المرحلة، أي حين تكون
                //      معاملاتُ الدالّةِ ما تزال Integer الافتراضيّة. فدالّةٌ كـ
                //          دالة حرف_عند(المصدر، الموضع)   ارجع المصدر.جزء(الموضع، 1)   نهاية
                //      يُستنتَج إرجاعُها Integer لأنّ `المصدر` عددٌ وقتَ الحساب. ثمّ تُصحَّح
                //      معاملاتُها هنا إلى نصّ — لكنّ نوعَ الإرجاعِ يبقى على خطئه، فيصير
                //      كلُّ معاملٍ لا يُغذّيه إلّا نداءٌ لها عددًا أيضًا:
                //          دالة هو_رقم(الحرف)   ارجع الحرف >= "0"   نهاية
                //          هو_رقم(حرف_عند("42"، 0))   ⇒ **مقارنةُ مؤشّرين عدديًّا**
                //      ونتيجتُها خطأٌ صامتٌ بلا تشخيص: المفسّرُ «صحيح» والمصرّفُ «خطأ».
                //      وكان يُخفيه أيُّ موقعِ نداءٍ آخرَ بحرفيّةٍ نصّيّة، فيبدو العيبُ عشوائيًّا.
                //
                //      و`inferReturnTypeFromBody` تقرأ أنواعَ المعاملاتِ من `functionTable_`
                //      (لا من الشجرة)، فإعادةُ تشغيلِها بعد كلِّ تمريرةٍ تلتقط التصحيحَ
                //      وتجعل المرحلةَ نقطةً ثابتةً بحقٍّ لا تمريرتين مستقلّتين.
                //
                //      تقتصر إعادةُ الحسابِ على ما نوعُ إرجاعِه **غيرُ مصرَّحٍ في المصدر**؛
                //      فالمصرَّحُ عقدُ الكاتبِ ولا يُخمَّن فوقه. ولا نكتب نوعًا فارغًا فوق
                //      نوعٍ قائمٍ كي لا تُفقَد معلومةٌ صحّت في تمريرةٍ سابقة.
                // (EN) [ز.٣٥] Re-infer return types between passes.
                //      An inferred return type is computed BEFORE this phase, while the
                //      function's parameters are still the default Integer — so a helper
                //      returning `src.substr(i, 1)` infers Integer, and every parameter fed
                //      only by a call to it stays Integer too, turning a string comparison
                //      into a pointer comparison: a SILENT wrong answer (interpreter true,
                //      compiler false) that any other string-literal call site would mask.
                //      `inferReturnTypeFromBody` reads parameter types from `functionTable_`,
                //      so re-running it per pass makes this phase a genuine fixed point.
                //      Only source-undeclared return types are recomputed (a declared one is
                //      the author's contract), and a Void result never overwrites a type
                //      already established by an earlier pass.
                // ════════════════════════════════════════════════════════════════════
                //      ⚠️ تحديثٌ **متزامن**: تُحسَب الأنواعُ كلُّها أوّلًا ثمّ تُكتَب دفعةً.
                //      لو كُتب كلُّ نوعٍ فورَ حسابه لَقرأت الدالّةُ التاليةُ نتيجةَ سابقتِها،
                //      وترتيبُ `functionTable_` (unordered_map) غيرُ مضمونٍ بين البناءاتِ
                //      والمنصّات ⇒ **مخرَجُ ترجمةٍ يتغيّر بلا تغيُّرِ مصدر**. والتحديثُ
                //      الدفعيُّ يجعل كلَّ تمريرةٍ دالّةً من الحالةِ السابقةِ وحدَها.
                //      وتُعاد التمريراتُ حتّى الثباتِ بسقفٍ صريح، فلا تنتهي على نتيجةٍ
                //      وسيطةٍ لمجرّدِ نفادِ العدد.
                // (EN) ⚠️ SYNCHRONOUS update: compute every type first, then write them as a
                //      batch. Writing each result immediately would let the next function read
                //      the previous one's, and `functionTable_` is an unordered_map whose
                //      iteration order is not guaranteed across builds/platforms — compiler
                //      output could change without a source change. Batching makes each pass a
                //      function of the previous state alone, and passes repeat until a fixed
                //      point (with an explicit cap) rather than stopping mid-convergence.
                auto reinferReturnTypes = [&]() -> bool
                {
                    std::vector<std::pair<std::string, SadTypeKind>> pending;
                    for (auto &entry : b_.functionTable_)
                    {
                        const Sad::AST::FunctionDecl *decl = entry.second.astDecl;
                        if (!decl || !decl->body)
                            continue;
                        if (decl->returnType != Types::SadTypeKind::Unknown &&
                            decl->returnType != Types::SadTypeKind::Void)
                            continue;
                        // (AR) [موجة ABI المغاليق] اسمُ النطاقِ يُضبَطُ كما في المسحِ كي
                        //      تُصيبَ مفاتيحُ حلِّ الأصلِ المُنطاقةُ في استنتاجِ العائد.
                        // (EN) [Closure-ABI wave] Scope name set as during scanning, so the
                        //      scoped provenance keys resolve inside return inference.
                        b_.currentScanFuncName_ = decl->name;
                        const SadTypeKind fresh = inferReturnTypeFromBody(decl->body.get(), decl);
                        b_.currentScanFuncName_.clear();
                        if (fresh != SadTypeKind::Void && fresh != entry.second.returnType)
                            pending.emplace_back(entry.first, fresh);
                    }
                    for (const auto &upd : pending)
                        b_.functionTable_[upd.first].returnType = upd.second;
                    return !pending.empty();
                };

                // (AR) مسح كل الجمل — عدة تمريرات للاستنتاج المتعدي. نمسح البرنامج
                //      الرئيس وأجسام الوحدات المستوردة في كلّ تمريرة كي ينتشر النوع
                //      عبر حدود الوحدات (رسالة⇒تحية) تمامًا كانتشاره داخل وحدةٍ واحدة.
                // (EN) Scan all statements — multiple passes for transitive inference. We
                //      scan the main program and the imported module bodies on every pass
                //      so a type propagates across module boundaries (رسالة⇒تحية) exactly
                //      as it does within a single module.
                // (AR) السقفُ ثلاثُ تمريراتٍ كما كان، لكنّنا نخرج مبكّرًا متى استقرّت أنواعُ
                //      الإرجاع — فالمشروعُ الكبيرُ لا يدفع ثمنَ مسحٍ كاملٍ لا يغيّر شيئًا.
                //      (لا نخرج قبل تمريرتين: الأولى تُصحّح المعاملات، والثانية تلتقط أثرَها.)
                // (EN) The cap stays at three passes, but we exit early once return types have
                //      settled, so a large project does not pay for a full scan that changes
                //      nothing. Never before two passes: the first fixes parameters, the second
                //      picks up their effect.
                for (int pass = 0; pass < 3; pass++)
                {
                    // (AR) [موجة ABI المغاليق] تُعلِمُ التمريرةُ حلَّ النداءِ غيرِ المباشرِ
                    //      في scanCallSitesInExpr — يُفعَّلُ من الثانيةِ بعد اكتمالِ التسميم.
                    // (EN) [Closure-ABI wave] Informs the indirect-call resolution in
                    //      scanCallSitesInExpr — active from the second pass, once
                    //      poisoning is complete.
                    b_.scanPassIndex_ = pass;
                    scanStmtList(program);
                    for (Sad::AST::StmtList *body : b_.importedModuleBodies_)
                        scanStmtList(body);
                    applyAgreedMemberParamTypes();
                    applyAgreedFreeParamTypes();
                    // (AR) [موجة الجسر الموسوم] بعدَ الإجماعَين وقبلَ إعادةِ استنتاجِ
                    //      العائدِ: التوسيعُ يغلبُ إجماعَ المواقعِ المرئيّةِ (الهروبُ
                    //      يعني مواقعَ غيرَ مرئيّة)، وإعادةُ الاستنتاجِ تلتقطُ أثرَه.
                    // (EN) [Tagged-bridge wave] After both unanimity passes and before
                    //      return re-inference: widening overrides visible-site
                    //      unanimity (escape means invisible sites exist), and the
                    //      re-inference picks up its effect.
                    applyEscapedFuncParamWidening();
                    const bool changed = reinferReturnTypes();
                    if (!changed && pass >= 1)
                        break;
                }
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
