/**
 * ════════════════════════════════════════════════════════════════
 * ملفّ: interpreter/src/ui/ui_widget_method_call.cpp
 * ════════════════════════════════════════════════════════════════
 *
 * (AR) seam 3 من م2-أ (sadlang-rfcs#10): معالجة سلسلة معدّلات WidgetBuilder
 *      (حجم/لون/حرّك/عند_*…) المُستخرجة من موزّع الـOOP في القلب.
 *      يُجمّع في مكتبة الجسر وحدها (يصل لأعضاء ev الخاصّة عبر الصداقة).
 * (EN) seam 3 of Phase 2-A: WidgetBuilder modifier-chain handling moved out of the
 *      core OOP method-call dispatch. Compiled only in the bridge library.
 * ════════════════════════════════════════════════════════════════
 */

#include "expression_evaluator.h"
#include "class_nodes.h" // (AR) تعريف MethodCallExpr / (EN) MethodCallExpr definition
#include "value.h"       // (AR) Value/toObject/toString… (تضمين صريح لا عبوريّ)
#include "object_instance.h"
#include "widget_builder.h" // (AR) WidgetBuilder + sad_ui/ir.h (IRAnimation/easing) عبوريًّا
#include "sad_ui/ui_modifiers.h" // (AR) مصدر الحقيقة لأسماء المعدّلات (مُولَّد من language-truth)
#include "ui_eval_bridge_impl.h"
#include "sad_ui/prop_keys.h" // (AR) جردُ المفاتيحِ المولَّدُ من مصدر الحقيقة

#include <iostream>
#include <unordered_set>

namespace Sad
{
    namespace Interpreter
    {
        using namespace Data;
        using namespace AST;

        namespace
        {
            // ═══════════════════════════════════════════════════════════════
            // (AR) جردُ المفاتيحِ القانونيّة — مولَّدٌ من language-truth/ui_props.yaml.
            //   يُبنى مرّةً واحدةً ويُستعمَل للتحقّقِ من اسمِ المعدّل. الأحداثُ
            //   («عند_…») ليست خصائصَ فتُستثنى، فلا يُحذَّر منها خطأً.
            // ═══════════════════════════════════════════════════════════════
            bool isKnownPropertyKey(const std::string &name)
            {
#define SAD_UI_PROP_KEY_ENTRY(Id) sad::ui::props::Id,
                static const std::unordered_set<std::string> kKnownKeys = {
                    SAD_UI_PROP_KEY_LIST(SAD_UI_PROP_KEY_ENTRY)};
#undef SAD_UI_PROP_KEY_ENTRY
                static_assert(SAD_UI_PROP_KEY_COUNT > 0, "جردُ مفاتيحِ الخصائصِ فارغ");
                return kKnownKeys.count(name) != 0;
            }

            // (AR) تحذيرٌ مرّةً واحدةً لكلِّ اسمٍ مجهول: برنامجٌ يبني شجرةً في لولبٍ
            //   كان سيغرق المخرَجَ بآلافِ الأسطرِ المتطابقة فيُفقِد التحذيرَ قيمتَه.
            void warnUnknownModifier(const std::string &name)
            {
                static std::unordered_set<std::string> warned;
                if (!warned.insert(name).second)
                    return;
                std::cerr << "\xe2\x9a\xa0 " // ⚠
                          << "\xd9\x85\xd8\xb9\xd8\xaf\xd9\x91\xd9\x84\xd9\x8c "
                             "\xd9\x85\xd8\xac\xd9\x87\xd9\x88\xd9\x84\xd8\xa7\xd9\x8b: " // «معدّلٌ مجهولٌ: »
                          << name << '\n';
            }

            // (AR) يضيف قيمةً كابنٍ إن كانت عنصر واجهة (WidgetBuilder)، عبر
            //   addChildBuilder كي تُسجَّل أحداث الأبناء المتداخلين. يوحّد منطق
            //   .ابن()/.أبناء() فلا يتكرّر، ويجعل .ابن(أ، ب) يضيف الاثنين بدل
            //   إسقاط ب صامتًا (تكافؤ مع .أبناء والصيغة الموضعيّة).
            inline void addChildIfWidget(Sad::Interpreter::WidgetBuilder *wb, const Value &v)
            {
                if (!v.isObject())
                    return;
                auto *childObj = v.toObject();
                if (childObj && isWidgetBuilder(childObj))
                    wb->addChildBuilder(static_cast<Sad::Interpreter::WidgetBuilder *>(childObj));
            }
        } // namespace

        // (AR) يُرجِع true إن كان objectValue عنصر WidgetBuilder وعولِج الاستدعاء.
        bool UIEvalBridgeImpl::tryWidgetMethodCall(ExpressionEvaluator &ev,
                                                   const Data::Value &objectValue,
                                                   MethodCallExpr &node)
        {
                if (objectValue.isObject())
                {
                    auto *objPtr = objectValue.toObject();
                    if (objPtr && isWidgetBuilder(objPtr))
                    {
                        auto *wb = static_cast<Sad::Interpreter::WidgetBuilder *>(objPtr);
                        const std::string &m = node.methodName;

                        // (AR) تقييم المعاملات
                        std::vector<Value> args;
                        for (auto &arg : node.arguments)
                        {
                            arg->accept(ev);
                            args.push_back(ev.lastResult_);
                        }

                        // (AR) طريقة ابن/أبناء — إضافة عناصر فرعية. كلتاهما تعالج
                        //   كلّ الوسائط عبر addChildIfWidget⇒addChildBuilder فتُسجَّل
                        //   أحداث الأبناء المتداخلين (تكافؤ الصيغتين والموضعيّة).
                        if (sad::ui::mods::isChild(m))
                        {
                            for (auto &a : args)
                                addChildIfWidget(wb, a);
                            ev.lastResult_ = objectValue;
                            return true;
                        }

                        // (AR) طريقة عند_* — تسجيل حدث
                        if (sad::ui::mods::isEvent(m))
                        {
                            // عند_النقر، عند_التغيير...
                            if (!args.empty())
                            {
                                // (AR) نحفظ المعالج كحقل على WidgetBuilder
                                //      ونضيف حدث بمعرّف مؤقت — UIBridge يسجّله لاحقاً
                                std::string eventKey = "__event_" + m;
                                wb->fields[eventKey] = args[0];
                                wb->addIREvent(m, eventKey);
                                // (AR) وسيطٌ ثانٍ اختياريّ = «بيانات» الحرّة لهذا
                                //      المعالِج، تصل الحقلَ «بيانات» في بنية «حدث».
                                if (args.size() > 1)
                                    wb->setLastEventUserData(args[1].toString());
                            }
                            ev.lastResult_ = objectValue;
                            return true;
                        }

                        // (AR) طريقة تفرّع — طور انتشار آخر معالِجٍ سُجِّل
                        //      (نظير .مدة/.منحنى مع آخر حركة). الطور مُعلَن على
                        //      المعالِج لا على الحدث؛ وبلا هذا المعدّل لا انتشار
                        //      البتّة ⇒ الاشتراك صريح فلا ينحدر برنامجٌ قائم.
                        if (sad::ui::mods::isPropagation(m))
                        {
                            if (!args.empty())
                                wb->setLastEventPropagation(
                                    sad::ui::stringToEventPropagation(args[0].toString()));
                            ev.lastResult_ = objectValue;
                            return true;
                        }

                        // ═══════════════════════════════════════════════════════
                        // (AR) معدّلات التحريك — .حرّك("نوع").مدة(ثوان).منحنى("اسم")
                        //      تُنشئ IRAnimation على IRNode عبر WidgetBuilder
                        // (EN) Animation modifiers — .حرّك("type").مدة(sec).منحنى("name")
                        //      Supports compound: .حرّك("ظهور,دوران") or .حرّك("ظهور", "دوران")
                        // ═══════════════════════════════════════════════════════
                        // حرّك / animate — بدء تسلسل تحريك جديد (أو مركّب بفاصلة)
                        if (sad::ui::mods::isAnimate(m))
                        {
                            // (AR) جمع كل أسماء الأنواع من الوسائط
                            std::vector<std::string> types;

                            // (AR) معالجة كل وسيط — قد يحتوي فاصلة داخلية
                            for (size_t ai = 0; ai < args.size(); ++ai)
                            {
                                std::string raw = args[ai].toString();

                                // (AR) تقسيم بالفاصلة (ASCII ',' فقط — الفاصلة العربية ،
                                //      تُحوَّل تلقائياً بواسطة المحلل المعجمي)
                                size_t start = 0;
                                for (size_t i = 0; i <= raw.size(); ++i)
                                {
                                    if (i == raw.size() || raw[i] == ',')
                                    {
                                        // (AR) استخراج الجزء وتنظيفه من المسافات
                                        std::string part;
                                        for (size_t j = start; j < i; ++j)
                                            if (raw[j] != ' ' && raw[j] != '\t')
                                                part += raw[j];
                                        if (!part.empty())
                                            types.push_back(part);
                                        start = i + 1;
                                    }
                                }
                            }

                            // (AR) احتياطيّ — إذا لم تنتج أيّ أنواع: النوع الافتراضيّ
                            //   (FadeIn) مشتقٌّ من مصدر الحقيقة عبر animationTypeToString
                            //   (لا ليترال إنجليزيّ؛ يعيد الاسم القانونيّ «ظهور»).
                            if (types.empty())
                                types.push_back(sad::ui::animationTypeToString(sad::ui::AnimationType::FadeIn));

                            // (AR) وضع المجموعة المركبة إذا كان هناك أكثر من نوع
                            bool isCompound = types.size() > 1;
                            if (isCompound)
                                wb->beginCompoundBatch();

                            for (const auto &t : types)
                                wb->startAnimationChain(t);

                            if (isCompound)
                                wb->endCompoundBatch();

                            ev.lastResult_ = objectValue;
                            return true;
                        }

                        // مدة / duration — تعيين مدة التحريك (تُطبّق على كل المجموعة المركبة)
                        if (sad::ui::mods::isDuration(m))
                        {
                            if (wb->isInAnimationChain() && !args.empty())
                            {
                                float dur = static_cast<float>(args[0].toDouble());
                                wb->applyToActiveAnimations([dur](sad::ui::IRAnimation &a)
                                                            { a.duration = dur; });
                            }
                            ev.lastResult_ = objectValue;
                            return true;
                        }

                        // منحنى / easing — تعيين منحنى التحريك (تُطبّق على كل المجموعة المركبة)
                        if (sad::ui::mods::isEasing(m))
                        {
                            if (wb->isInAnimationChain() && !args.empty())
                            {
                                auto curve = sad::ui::stringToEasingCurve(args[0].toString());
                                wb->applyToActiveAnimations([curve](sad::ui::IRAnimation &a)
                                                            { a.easing = curve; });
                            }
                            ev.lastResult_ = objectValue;
                            return true;
                        }

                        // تأخير / delay — تعيين التأخير قبل التحريك (تُطبّق على كل المجموعة المركبة)
                        if (sad::ui::mods::isDelay(m))
                        {
                            if (wb->isInAnimationChain() && !args.empty())
                            {
                                float d = static_cast<float>(args[0].toDouble());
                                wb->applyToActiveAnimations([d](sad::ui::IRAnimation &a)
                                                            { a.delay = d; });
                            }
                            ev.lastResult_ = objectValue;
                            return true;
                        }

                        // تكرار / repeat — عدد تكرارات التحريك (تُطبّق على كل المجموعة المركبة)
                        if (sad::ui::mods::isRepeat(m))
                        {
                            if (wb->isInAnimationChain() && !args.empty())
                            {
                                int r = static_cast<int>(args[0].toInt());
                                wb->applyToActiveAnimations([r](sad::ui::IRAnimation &a)
                                                            { a.repeatCount = r; });
                            }
                            ev.lastResult_ = objectValue;
                            return true;
                        }

                        // عكس_تلقائي / autoReverse — عكس الحركة تلقائياً (تُطبّق على كل المجموعة المركبة)
                        if (sad::ui::mods::isAutoReverse(m))
                        {
                            if (wb->isInAnimationChain())
                            {
                                bool rev = args.empty() ? true : args[0].toBool();
                                wb->applyToActiveAnimations([rev](sad::ui::IRAnimation &a)
                                                            { a.autoReverse = rev; });
                            }
                            ev.lastResult_ = objectValue;
                            return true;
                        }

                        // (AR) إذا كان في تسلسل تحريك ووصلنا لمعدّل غير تحريكي — نُنهي التحريك أولاً
                        if (wb->isInAnimationChain())
                        {
                            wb->commitAnimation();
                        }

                        // ═════════════════════════════════════════════════════
                        // (AR) تحقّقُ اسمِ المعدّل قبلَ كتابتِه (ث٥).
                        //
                        //   كان فضاءُ المعدّلاتِ مفتوحًا بلا تحقّقٍ البتّة: أيُّ اسمٍ
                        //   يُكتَب خاصّيّةً على العقدة. فخطأٌ إملائيٌّ («نصف_قطرر»)
                        //   يُبتلَع صامتًا، وأثرُه بكسلٌ مفقودٌ لا رسالةُ خطأ —
                        //   وهو أسوأُ أصنافِ العطبِ في هذه المكتبة.
                        //
                        //   تحذيرٌ لا رفضٌ في هذه المرحلة (توصيةُ التقرير): المنعُ
                        //   قد يكسر برنامجًا يكتب خاصّيّةً يقرؤها مُصيِّرٌ مستقبليّ،
                        //   والتحذيرُ يكشف الخطأَ دونَ أن يُسقِطَ عملًا قائمًا.
                        //   الجردُ مولَّدٌ من مصدرِ الحقيقة، فلا قائمةَ ثانيةٌ تتعفّن.
                        // ═════════════════════════════════════════════════════
                        if (!isKnownPropertyKey(m))
                        {
                            warnUnknownModifier(m);
                        }

                        // (AR) أي طريقة أخرى → معدّل خاصية بصرية
                        //      الوسيط الوحيد → خاصية بقيمة
                        //      بدون وسائط → خاصية منطقية true
                        //      عدة وسائط → قيم مفصولة بفواصل
                        if (args.empty())
                        {
                            wb->setIRProperty(m, true);
                        }
                        else if (args.size() == 1)
                        {
                            wb->setIRPropertyFromValue(m, args[0]);
                        }
                        else
                        {
                            std::string combined;
                            for (size_t i = 0; i < args.size(); i++)
                            {
                                if (i > 0)
                                    combined += ",";
                                combined += args[i].toString();
                            }
                            wb->setIRProperty(m, combined);
                        }

                        ev.lastResult_ = objectValue;
                        return true;
                    }
                }
            return false;
        }
    }
}
