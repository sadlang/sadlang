// ============================================================================
// builtins_ui.cpp
// (AR) مصانع عناصر الواجهة المضمّنة: عمود، صف، نص_عنصر، زر، تطبيق، ...
// (EN) UI Widget Factory builtins: column, row, text-widget, button, app, ...
// (EN) Unified UI Widget Factory Functions (SIR builder layer)
// ============================================================================

#include "sir_builder.h"
#include "builders/builtin_builder.h"
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
// (AR) لا <iostream> ههنا عمدًا: الملفُّ لم يعد يطبعُ تشخيصًا، والتشخيصُ كلُّه
//      يُدفَع إلى `errors_`. وبقاءُ الترويسةِ يُبقي البابَ مفتوحًا لعودةِ النمط.
#include <filesystem>
#include <optional>

#include "builtin_registry.h"
#include "builders/builtin_arity_check.h" // (AR) حارسُ الرتبةِ الواحدُ + الثوابتُ المُولَّدة
#include "error_manager.h" // (AR) buildBilingualMessage من كتالوج الأخطاء (مصدر الحقيقة)
#include "error_catalog.h" // (AR) RenderContext (حاملُ placeholders)
#include "error_codes.h"   // (AR) ErrorCode::SEM_WRONG_ARG_COUNT
// (AR) جدولُ مصانعِ العناصرِ المولَّدُ من language-truth — رأسٌ في الطبقةِ الأساسِ
//      (المحلّل) لا في مكتبةِ الرسومات، فلا يكسر طبقيّةَ المصرِّف.
#include "generated/ui_parser_nodes_generated.h"
#include "sad_ui/prop_keys.h" // (AR) مفاتيحُ الخصائصِ + جدولُ المفاتيحِ العدديّة (مُولَّد من SoT)
// (AR) قرارُ خفضِ قيمةِ الخاصّيّة — مشتركٌ مع مسارِ المعدّلاتِ فلا نسختانِ تتباعدان.
#include "builders/ui_prop_lowering.h"

#include <unordered_map>
#include <algorithm> // (AR) std::min — حدُّ أريّةِ المصنع
namespace Bn = Sad::Builtins::Names;
// (AR) رتبةُ المدمجِ من حقلِ `arity` في مصدرِ الحقيقةِ — ثابتٌ مُولَّدٌ لا رقمٌ
//      يُكتَب عند الفحص. وكانت الواجهةُ آخرَ من بقي يكتبُ الرقمَ بيدِه.
namespace Ar = Sad::Builtins::Arity;
namespace uiprops = Sad::Compiler::Frontend::UIProps;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // (AR) كانت ههنا نسخةٌ خامسةٌ من حارسِ الرتبة (`reportUiWrongArgCount`).
            //      وحّدت التبعةَ فصارت تدفعُ إلى `errors_` كأشقائها — لكنّها بقيت
            //      تأخذُ العددَ **رقمًا حرفيًّا** في موضعِ النداء، والاسمَ نسخةً
            //      عربيّةً ثانيةً مكتوبةً باليد (وبعضُها بايتاتٌ مهرَّبةٌ \xd8…).
            //
            // 🔑    وحارسُ مصدرِ الحقيقةِ كان يعلنُ «لا رقمَ حرفيًّا» وهو صادقٌ في
            //       حدودِ ما يرى: مرساتُه `check\w*Arity\(` واسمُ هذه لا يطابقُها.
            //       فسبعةٌ وعشرون رقمًا مرّت من تحتِ بصرِه — أخضرُ لأنّ الشرطَ لا
            //       يمكنُ أن يكونَ خطأً، لا لأنّ الموضعَ سليم. حُذِفت النسخةُ إلى
            //       `checkBuiltinArity`، ووُسِّعت مرساةُ الحارسِ لتلحقَ بأيِّ حارسٍ
            //       يقبلُ عددًا — فالمرساةُ على الفعلِ لا على هجائِه.
            // (EN) The fifth arity helper: it failed the build correctly but took
            //      its count as a literal and escaped the SoT guard's anchor.
            //      Folded into checkBuiltinArity; the anchor was widened.

            // (AR) وسيط التنقّل (انتقل/استبدل/انتقل_بتحريك) يجب أن يكون **عنصرًا** (لقطة،
            //      Pointer) أو **دالّة بناء صفحة** (Function). أيّ نوعٍ آخر (عدد/نص/لاشيء…)
            //      ليس صفحةً: يُحلّ إلى i64 غير-إغلاق فيُعامَل خطأً كبانٍ (inttoptr لقيمةٍ
            //      عدديّة ⇒ قراءة {fn,env} من مؤشّرٍ باطل ⇒ انهيار)، أو كلقطةٍ باطلة. نرفضه
            //      هنا بتشخيصٍ من الكتالوج (نظير المفسّر الذي يرفضه أيضًا) ⇒ تكافؤٌ لا انهيار.
            //      — إصلاح مراجعة Amelia (HIGH-2). يُرجع true إن كان الوسيط صالحًا.
            //      والتشخيصُ يُدفَع إلى `errors_` لا يُطبَع: رفضٌ يمضي بناؤه ليس رفضًا.
            [[nodiscard]] static bool checkUiNavArgType(std::vector<std::string> &errors,
                                            const std::string &name, SadTypeKind t)
            {
                if (t == SadTypeKind::Pointer || t == SadTypeKind::Function)
                    return true;
                Sad::Errors::RenderContext ctx;
                ctx.placeholders = {
                    {"expected", name + ": \xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1 \xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9 \xd8\xa3\xd9\x88 \xd8\xaf\xd8\xa7\xd9\x84\xd9\x91\xd8\xa9 \xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1 \xd8\xb5\xd9\x81\xd8\xad\xd8\xa9"}, // عنصر واجهة أو دالّة بناء صفحة
                    {"found", std::string(Sad::Compiler::SIR::sirTypeToString(t))}};
                errors.push_back(
                    Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                        Sad::Errors::ErrorCode::SEM_TYPE_MISMATCH, ctx));
                return false;
            }

            // (AR) هل النوعُ مؤشِّريٌّ ومعروفٌ **قطعًا** أنّه ليس نصًّا؟ يُستعمَل في
            //      أبوابِ توليدِ كودِ المنصّة حيث يُمرَّر الوسيطُ الثاني إلى
            //      const char*: عنصرُ واجهةٍ (Pointer) أو كائنٌ في موضعِ الاسمِ
            //      يعني قراءةَ ذاكرةٍ عشوائيّةٍ ⇒ انهيار، بينما المفسّرُ يتجاهله.
            //      نُسقِطه هنا لنُطابق المفسّرَ؛ والمجهولُ/أيّ **لا** يُسقَط كي لا
            //      نبتلعَ نصًّا مشروعًا لم يُستنتَج نوعُه. — إصلاح مراجعة Amelia.
            [[nodiscard]] static bool isNonStringPointerLikeType(SadTypeKind t)
            {
                switch (t)
                {
                case SadTypeKind::Pointer:
                case SadTypeKind::Reference:
                case SadTypeKind::MutableRef:
                case SadTypeKind::Class:
                case SadTypeKind::Struct:
                case SadTypeKind::Array:
                case SadTypeKind::Map:
                case SadTypeKind::Tuple:
                case SadTypeKind::Function:
                case SadTypeKind::Closure:
                case SadTypeKind::Widget:
                case SadTypeKind::Window:
                case SadTypeKind::Event:
                    return true;
                default:
                    return false;
                }
            }

            // (AR) حارس نوع عنوان النافذة (إصلاح Amelia M2، تكافؤ مع المفسّر الذي يرفض
            //      !isString): نرفض الأنواع العدديّة/المنطقيّة القطعيّة **فقط** (النصّ لا
            //      يُستنتَج أبدًا عددًا/منطقيًّا ⇒ لا رفضٌ زائف على نصّ صحيح)؛ ما عداها
            //      (نص/مؤشّر/غير محدَّد) يُقبَل. المتوقَّع مشتقٌّ من sirTypeToString(String)
            //      لا نصًّا حرفيًّا. يُرجع true إن كان الوسيط مقبولًا.
            [[nodiscard]] static bool checkUiTitleArgType(std::vector<std::string> &errors,
                                            const std::string &name, SadTypeKind t)
            {
                if (t != SadTypeKind::Integer && t != SadTypeKind::Float && t != SadTypeKind::Boolean)
                    return true;
                Sad::Errors::RenderContext ctx;
                ctx.placeholders = {
                    {"expected", name + ": " + std::string(Sad::Compiler::SIR::sirTypeToString(SadTypeKind::String))},
                    {"found", std::string(Sad::Compiler::SIR::sirTypeToString(t))}};
                errors.push_back(
                    Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                        Sad::Errors::ErrorCode::SEM_TYPE_MISMATCH, ctx));
                return false;
            }

            std::optional<BuildResult> BuiltinBuilder::buildBuiltinSystem_UI(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {
                // (AR) مصنع «قيمة واحدة» (منزلق/خانة_اختيار/مفتاح): المفسّر يضبط مفتاح
                //      «قيمة» بنوع الوسيط (MAKE_WIDGET_WITH_PROP_FN(...,"قيمة") ⇒
                //      setIRPropertyFromValue). نحاكيه: نُصدر عقدة المصنع فارغةً (بلا
                //      operands) ثمّ SET_PROP(«قيمة») بحسب نوع الوسيط (int/num/bool/str)
                //      ⇒ طباعة_شجرة تطابق المفسّر تمامًا. يعيد مقبض العنصر.
                // (AR) النوعُ لم يُحسَمْ وقتَ الترجمة: `أي`/`مجهول` — نتيجةُ `6 / 2`
                //   مثلًا، إذ قد تكون صحيحًا أو عشريًّا زمنَ التشغيل.
                // (EN) Compile-time-undecided type (Any/Unknown), e.g. the result of `/`.
                auto isUndecidedAtCompileTime = [](SadTypeKind kind) {
                    return kind == SadTypeKind::Any || kind == SadTypeKind::Unknown;
                };

                // ════════════════════════════════════════════════════════════════
                // (AR) اختيارُ أوپكودِ ضبطِ الخاصّيّةِ **الأولى** بحسبِ نوعِ الوسيط.
                //   كان الاختيارُ سلسلةً ثلاثيّةً تسألُ عن `رقم`/`عشري`/`منطقي`
                //   وتُسقِطُ ما عداها إلى STR — فكلُّ نوعٍ عدديٍّ محدَّدِ الحجمِ
                //   (`طبيعي64`، `بايت`، `حرف`، عدد٨/١٦/٣٢/٦٤…) كان يُخزَّن **نصًّا**
                //   فيقرؤه المُرسِّم فارغًا: `ترقيم_صفحات(صفحاتٌ_طبيعي64)` يطبع
                //   «قيمة: ""» بينما المفسّرُ يطبع «قيمة: 4». تباعُدٌ صامتٌ لا خطأَ
                //   فيه ولا تحذير — من عائلةِ العيبِ نفسِها التي أغلقها #395.
                //   والمفسّرُ لا يعرفُ أحجامًا أصلًا: يفحصُ **بنيةَ القيمة**، فكلُّ
                //   ما هو عددٌ صحيحٌ عندَه عددٌ صحيح.
                //   أمّا غيرُ المحسومِ وقتَ الترجمةِ فيُحسَمُ **بالمفتاحِ من مصدرِ
                //   الحقيقة** (`value_type: عدد` ⇒ isNumericPropKey) — وهو النهجُ
                //   المُستقرُّ في مسارِ المعدّلات (call_method_dispatch.cpp) ولم
                //   يكن قد بلغَ مسارَ الخاصّيّةِ الأولى.
                // (EN) Pick the primary-property SET_PROP opcode from the argument type.
                //   The old ternary asked only about Integer/Float/Boolean and dumped
                //   everything else into STR, so every sized numeric type was stored as
                //   text and read back empty — a silent divergence from the interpreter,
                //   which inspects value structure and knows nothing of widths.
                //   Undecided types are resolved BY KEY from the SoT numeric-key table,
                //   the approach already established on the modifier path.
                // ════════════════════════════════════════════════════════════════
                //   وقرارُ الاختيارِ نفسُه **يعيشُ في موضعٍ واحد** (ui_prop_lowering.h)
                //   ويستدعيه مسارُ المعدّلاتِ أيضًا: ترقيةُ أحدِ المسارَين دونَ الآخرِ
                //   هي بعينِها ما أنتجَ تباعُدَي #396 و#400 الصامتَين.
                // (EN) The choice itself lives in ONE place (ui_prop_lowering.h) and is
                //   shared with the modifier path — upgrading one path and not the other
                //   is exactly what produced the #396/#400 silent divergences.
                auto primaryPropOpcode = [&](SadTypeKind kind, const char *propKey) -> SIROpcode {
                    (void)propKey; // (AR) الحسمُ بالنوعِ والوسمِ لا بالمفتاح
                    return uiprops::propOpcodeForValueType(kind);
                };

                // ════════════════════════════════════════════════════════════════
                // (AR) مصنعٌ خانتُه الأولى **نصّيّة** (`زر(عنوان)`، `صورة(مصدر)`،
                //   `أيقونة(اسم)`، `حقل_نص(تلميح)`…). العلّةُ التي أُغلِقت لـ`نص_عنصر`
                //   وحدَه كانت حيّةً في ثلاثةَ عشرَ مصنعًا غيرِه: توقيعُ وقتِ التشغيلِ
                //   `const char*`، وجسرُ الوسائطِ يحوّلُ الصحيحَ بـ`IntToPtr`، فيُقرأُ
                //   العددُ عنوانًا ⇒ **SIGSEGV** (مقيس: `زر(42)` · `أيقونة(42)` ·
                //   `صورة(42)` · `حقل_نص(42)` تُسقِطُ البرنامجَ بـ0xC0000005 بينما
                //   المفسّرُ يطبعُ `زر(عنوان: 42)`).
                //   العلاجُ **بنيويٌّ لا موضعيّ**: الخانةُ النصّيّةُ لا تستقبلُ إلّا
                //   نصًّا؛ فإن لم يكن الوسيطُ نصًّا وُضِعَ في الخانةِ نائبٌ فارغٌ —
                //   **وبقيت الوسائطُ الباقيةُ في مواضعِها** فلا يُزَحُّ ردُّ النداءِ إلى
                //   خانةِ العنوان — ثمّ كُتِبتِ الخاصّيّةُ الأولى بالأوپكودِ المناسبِ
                //   للنوع، وهو المسارُ نفسُه الذي يسلكُه المفسّرُ
                //   (MAKE_WIDGET_WITH_PROP_FN ⇒ setIRPropertyFromValue).
                // (EN) A factory whose FIRST SLOT IS TEXT. The bug closed for نص_عنصر
                //   alone was alive in thirteen other factories: the runtime signature
                //   is `const char*` and the arg bridge IntToPtr's an integer, so the
                //   number is dereferenced ⇒ SIGSEGV. Structural fix: the text slot only
                //   ever receives text; a non-text first argument leaves an empty
                //   placeholder there (later arguments keep their positions, so the
                //   callback is not shifted into the label slot) and is written as the
                //   primary property with the type-appropriate opcode — the interpreter's
                //   own path.
                // ════════════════════════════════════════════════════════════════
                //   و`maxFactoryOperands` يحفظُ **أريّةَ كلِّ مصنعٍ كما كانت**: منها ما
                //   يستقبلُ خانةً واحدةً فقط (`صورة`/`أيقونة`/`تلميح`…) ومنها ما يستقبلُ
                //   ردَّ نداءٍ وبياناتٍ بعدَ العنوان (`زر`). زيادةُ الخانةِ تُفسِدُ توقيعَ
                //   النداءِ في الخلفيّة، فلا تُعمَّمُ الأريّةُ بحجّةِ التوحيد.
                auto lowerTextSlotFactory = [&](SIROpcode factoryOp, const char *primaryKey,
                                                size_t maxFactoryOperands) -> BuildResult {
                    const bool firstArgIsText =
                        !argResults.empty() && argResults[0].type == SadTypeKind::String;
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(factoryOp);
                    const size_t factoryOperandCount =
                        maxFactoryOperands == 0 ? argOperands.size()
                                                : std::min(argOperands.size(), maxFactoryOperands);
                    for (size_t i = 0; i < factoryOperandCount; ++i)
                    {
                        if (i == 0 && !firstArgIsText)
                            inst.operands.push_back(SIROperand::ConstantString(uiprops::kEmptyTextSlotPlaceholder));
                        else
                            inst.operands.push_back(argOperands[i]);
                    }
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    if (!argOperands.empty() && !firstArgIsText)
                    {
                        SIROperand valueOp = argOperands[0];
                        const bool asText = uiprops::lowerArrayValueToString(
                            b_, valueOp, argResults[0].type, argResults[0].elementType);
                        SIRInstruction sp(asText ? SIROpcode::BUILTIN_UI_SET_PROP_STR
                                                 : primaryPropOpcode(argResults[0].type, primaryKey));
                        sp.operands.push_back(SIROperand::Register(r, SadTypeKind::Pointer));
                        sp.operands.push_back(SIROperand::ConstantString(primaryKey));
                        sp.operands.push_back(valueOp);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(sp);
                        if (asText)
                            uiprops::releaseLoweredArrayString(b_, valueOp);
                    }
                    return BuildResult(r, SadTypeKind::Pointer);
                };

                auto lowerValueWidget = [&](SIROpcode factoryOp) -> BuildResult {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(factoryOp); // بلا operands ⇒ عقدة فارغة
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    if (!argResults.empty())
                    {
                        // (AR) المصفوفةُ قيمةَ خاصّيّةٍ تُصيَّرُ نصًّا كما يفعلُ المفسّر
                        //      (`toString`) — وإلّا بلغَ مؤشّرُها خانةَ `const char*`.
                        SIROperand valueOp = argOperands[0];
                        const bool asText = uiprops::lowerArrayValueToString(
                            b_, valueOp, argResults[0].type, argResults[0].elementType);
                        SIROpcode op = asText
                                           ? SIROpcode::BUILTIN_UI_SET_PROP_STR
                                           : primaryPropOpcode(argResults[0].type, sad::ui::props::VALUE);
                        SIRInstruction sp(op);
                        sp.operands.push_back(SIROperand::Register(r, SadTypeKind::Pointer)); // العنصر
                        // (AR) مفتاحُ «قيمة» من مصدرِ الحقيقةِ لا سلسلةً حرفيّة.
                        sp.operands.push_back(SIROperand::ConstantString(sad::ui::props::VALUE));
                        sp.operands.push_back(valueOp);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(sp);
                        if (asText)
                            uiprops::releaseLoweredArrayString(b_, valueOp);
                    }
                    return BuildResult(r, SadTypeKind::Pointer);
                };

                // (AR) مصنع حاوية: يُنشئ عقدة الحاوية ثمّ **يربط كلّ وسيطٍ عنصريّ
                //      (Pointer) كابن** — نظير MAKE_SIMPLE_WIDGET_FN في المفسّر
                //      (widget_builtins.cpp:65-68: addChildBuilder لكلّ وسيطٍ عنصر).
                //      كان المترجم يُسقط الأبناء الموضعيّين صمتًا (حاوية فارغة ⇒ تباعُد
                //      مفسّر↔مترجم)؛ هذا يوحّدهما بإصدار sad_add_child(الحاوية، الابن)
                //      لكلّ وسيطٍ نوعه Pointer (حارس النوع نظير isWidgetBuilder). نفس
                //      الرموز — لا توسيع SoT. المسار المُستضاف يستهلك sad_add_child نفسه.
                // (AR) الوسيطُ «شبيهُ عنصر»: مؤشّرٌ صريحٌ، أو نوعٌ يتقرّر زمنَ التشغيل
                //   (Any/Unknown/Integer — مقبضُ عنصرٍ عابرٌ حدَّ وحدةٍ أو معامِلًا بلا
                //   تصريح)، أو مصفوفةٌ تُنشَر. مَن كان كذلك فهو **ابنٌ** لا قيمةُ
                //   خاصّيّةٍ أولى — وهو نظيرُ isWidgetLike في المفسّر تمامًا.
                //   بدونِ هذا التوحيدِ كان المسارُ العامُّ يكتب مصفوفةً في خاصّيّةٍ
                //   نصّيّةٍ (قراءةٌ خارجَ الحدّ) ويُسقِط ابنًا ديناميَّ النوعِ صامتًا.
                auto isWidgetLikeArg = [](SadTypeKind kind) {
                    return kind == SadTypeKind::Pointer || kind == SadTypeKind::Any ||
                           kind == SadTypeKind::Unknown || kind == SadTypeKind::Integer ||
                           kind == SadTypeKind::Array;
                };


                // (AR) `extraOperand` لأوپكودِ الإنشاءِ العامّ (رقمُ نوعِ العقدة)؛
                //   و`primaryProp` مفتاحُ الخاصّيّةِ الأولى إن كان للمصنعِ واحدة.
                auto lowerContainer = [&](SIROpcode factoryOp,
                                          const SIROperand *extraOperand = nullptr,
                                          const char *primaryProp = nullptr) -> BuildResult {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(factoryOp);
                    if (extraOperand)
                        inst.operands.push_back(*extraOperand);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);

                    // (AR) الوسيطُ الأوّلُ غيرُ شبيهِ العنصرِ = قيمةُ الخاصّيّةِ الأولى
                    //   (`رقاقة("مسوّدة")`)، ويُستثنى من حلقةِ الأبناءِ أدناه.
                    //
                    //   والصحيحُ (Integer) **قيمةُ خاصّيّةٍ هنا وابنٌ في الحلقةِ أدناه**،
                    //   وليس في هذا تناقض: المفسّرُ يفحص بنيةَ القيمةِ لا نوعَها
                    //   الساكن، و`isWidgetLike` عندَه لا تعدُّ الصحيحَ عنصرًا أبدًا
                    //   (widget_builtins.cpp: Class-ودجت أو Array فقط) ⇒
                    //   `ترقيم_صفحات(3)` يكتب «قيمة=3». فلو عددناه شبيهَ عنصرٍ هنا
                    //   لأصدرنا `ADD_CHILD(w, 3)` ولأسقطه `sad_ui_runtime` صامتًا
                    //   (`isRegisteredWidget(3)` كاذب) ⇒ الخاصّيّةُ لا تُضبَط والعنصرُ
                    //   يُرسَم بالافتراضيّ: تباعُدٌ صامتٌ بين المحرّكَين.
                    //   وحُجّةُ «مقبضِ العنصرِ في خانةِ i64» تخصّ **الحاويات** (لا
                    //   خاصّيّةَ أولى لها فلا تبلغ هذا الفرعَ أصلًا)، فتبقى سليمةً في
                    //   حلقةِ الأبناء.
                    //   وكذلك النوعُ غيرُ المحسومِ وقتَ الترجمة: `ترقيم_صفحات(6 / 2)`
                    //   كان يُعَدُّ شبيهَ عنصرٍ فيصيرُ `ADD_CHILD(w, 3)` ويُسقِطُه
                    //   `sad_ui_runtime` صامتًا ⇒ عقدةٌ **بلا خاصّيّةٍ إطلاقًا**
                    //   (`ترقيم_صفحات` مجرّدةً) بينما المفسّرُ يطبع «قيمة: 3».
                    //   المفسّرُ يفحصُ بنيةَ القيمةِ زمنَ التشغيلِ فالعددُ ليس عنصرًا
                    //   أبدًا؛ ونحن نحسمُ بالمفتاحِ كما في مسارِ المعدّلات. ويبقى
                    //   حدٌّ مُعلَنٌ: قيمةٌ ديناميّةٌ تحملُ **عنصرًا** فعليًّا وتُمرَّرُ
                    //   أوّلَ وسيطٍ لمصنعٍ ذي خاصّيّةٍ أولى ستُكتَبُ خاصّيّةً لا ابنًا.
                    //   والحاوياتُ — وهي موضعُ الأبناءِ الحقيقيّ — لا خاصّيّةَ أولى
                    //   لها فلا تبلغُ هذا الفرعَ أصلًا.
                    const bool firstArgIsPropValue =
                        !argResults.empty() && (!isWidgetLikeArg(argResults[0].type) ||
                                                argResults[0].type == SadTypeKind::Integer ||
                                                isUndecidedAtCompileTime(argResults[0].type));
                    size_t firstChild = 0;
                    if (primaryProp && primaryProp[0] != '\0' && firstArgIsPropValue)
                    {
                        const SIROpcode setOp = primaryPropOpcode(argResults[0].type, primaryProp);
                        SIRInstruction setProp(setOp);
                        setProp.operands.push_back(SIROperand::Register(r, SadTypeKind::Pointer));
                        setProp.operands.push_back(SIROperand::ConstantString(primaryProp));
                        setProp.operands.push_back(argOperands[0]);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(setProp);
                        firstChild = 1;
                    }

                    for (size_t i = firstChild; i < argResults.size(); ++i)
                    {
                        if (argResults[i].type == SadTypeKind::Pointer ||
                            argResults[i].type == SadTypeKind::Any ||
                            argResults[i].type == SadTypeKind::Unknown ||
                            argResults[i].type == SadTypeKind::Integer)
                        {
                            // (AR) وسيطٌ عنصريّ مفرد ⇒ sad_add_child(الحاوية، الابن).
                            // (AR) يشمل الوسيطَ **ديناميَّ النوع** (Any/Unknown): معامِلُ دالّةٍ
                            //      بلا تصريح نوع، أو قيمةٌ عائدة من دالّة مستوردة. كان القيدُ
                            //      على Pointer وحده يُسقِط مثل هذا الابن **صمتًا** فيُرسَم اللوح
                            //      فارغًا (مقيس بالبكسل على fb0: تمرير ودجت معامَلًا عبر حدّ
                            //      وحدة ⇒ الأب يظهر والابن يختفي). و**الصحيح (Integer)** مشمولٌ
                            //      لأنّ مقبض العنصر يسري في خانة i64 حين يمرّ عبر معامِلٍ بلا
                            //      تصريح نوع أو عائدِ وحدةٍ مستوردة — وهي الحالة التي يوثّقها
                            //      coerceUiArgToParam أصلًا في الخلف. الأمان محفوظ بحارس وقت
                            //      التشغيل: sad_add_child يتجاهل ما ليس عنصرًا مُسجَّلًا في
                            //      g_widgets (isRegisteredWidget = مسحُ سجلٍّ لا فكُّ مؤشّر)،
                            //      فيطابق تجاهُلَ المفسّر لغير-الودجت — نفس حُجّة نشر المصفوفة.
                            // (EN) Includes dynamically-typed args (Any/Unknown): an untyped
                            //      function parameter or a value returned from an imported
                            //      module function. Restricting to Pointer silently dropped such
                            //      children (panel drawn, child invisible — measured on fb0).
                            //      Safety holds via the runtime guard in sad_add_child, which
                            //      ignores non-registered widgets — same argument as array spread.
                            SIRInstruction add(SIROpcode::BUILTIN_UI_ADD_CHILD);
                            add.operands.push_back(SIROperand::Register(r, SadTypeKind::Pointer)); // الأب
                            add.operands.push_back(argOperands[i]);                                // الابن
                            if (b_.currentBlock_)
                                b_.currentBlock_->instructions.push_back(add);
                        }
                        else if (argResults[i].type == SadTypeKind::Array)
                        {
                            // (AR) **نشر مصفوفة إلى أبناء الحاوية**: لولب وقت-تشغيل يمرّ على
                            //      عناصر المصفوفة (طولها ديناميّ) فيُضيف كلّ عنصرٍ ابنًا بترتيبه —
                            //      مطابقٌ لـaddChildOrSpread في المفسّر (widget_builtins.cpp). يعيد
                            //      استعمال أوپكودات قائمة فقط (ARRAY_LEN/ARRAY_GET/LT/ADD_I64/
                            //      ADD_CHILD) — لا أوپكود جديد ولا توسيع SoT. عدّاد على المكدّس
                            //      (ALLOC/STORE/LOAD) نظير نمط foreach (statement_for_range.cpp).
                            //      النشر **مستوًى واحد فقط**: ARRAY_GET نتيجتُه Pointer، والعنصر
                            //      المتداخل (مصفوفة) أو غير-الودجت يُسقطه حارس sad_add_child وقت
                            //      التشغيل (غير مُسجَّل) — مطابقٌ لتجاهُل المفسّر. راجع RFC «نشر
                            //      المصفوفة إلى أبناء الحاوية».
                            // (EN) **Spread array into container children**: a runtime loop over
                            //      the (dynamic-length) array adds each element as a child, in
                            //      order — matching the interpreter's addChildOrSpread. Reuses
                            //      existing opcodes only (no new opcode / no SoT growth). Spread is
                            //      ONE LEVEL: ARRAY_GET yields Pointer; a nested-array or non-widget
                            //      element is dropped by the runtime sad_add_child guard — matching
                            //      the interpreter's skip.
                            std::string condL = b_.newLabel("spread_cond");
                            std::string bodyL = b_.newLabel("spread_body");
                            std::string incL  = b_.newLabel("spread_inc");
                            std::string exitL = b_.newLabel("spread_exit");
                            auto condB = b_.createBasicBlock(condL);
                            auto bodyB = b_.createBasicBlock(bodyL);
                            auto incB  = b_.createBasicBlock(incL);
                            auto exitB = b_.createBasicBlock(exitL);
                            if (b_.currentFunction_)
                            {
                                b_.currentFunction_->addBasicBlock(condB);
                                b_.currentFunction_->addBasicBlock(bodyB);
                                b_.currentFunction_->addBasicBlock(incB);
                                b_.currentFunction_->addBasicBlock(exitB);
                            }
                            // (AR) عدّاد فريد على المكدّس، مُصفَّر (نمط غير-SSA آمن عبر الكتل).
                            std::string idxSuffix = condL.substr(condL.find_last_of('_') + 1);
                            std::string idxAlloc = std::string("%") + kSlotNamespaceSeparator + "spread_idx" + kSlotNamespaceSeparator + idxSuffix;
                            {
                                SIRInstruction a(SIROpcode::ALLOC);
                                a.result = SIROperand::Register(idxAlloc, SadTypeKind::Integer);
                                if (b_.currentBlock_) b_.currentBlock_->instructions.push_back(a);
                                SIRInstruction s(SIROpcode::STORE);
                                s.operands.push_back(SIROperand::ConstantI64(0));
                                s.operands.push_back(SIROperand::Register(idxAlloc, SadTypeKind::Integer));
                                if (b_.currentBlock_) b_.currentBlock_->instructions.push_back(s);
                                SIRInstruction brIn = SIRInstruction::Branch(SIROperand::Label(condL));
                                if (b_.currentBlock_) b_.currentBlock_->instructions.push_back(brIn);
                            }
                            // (AR) كتلة الشرط: idx < طول(المصفوفة) ؟
                            b_.currentBlock_ = condB;
                            {
                                std::string loadedIdx = b_.newTempRegister();
                                SIRInstruction ld(SIROpcode::LOAD);
                                ld.result = SIROperand::Register(loadedIdx, SadTypeKind::Integer);
                                ld.operands.push_back(SIROperand::Register(idxAlloc, SadTypeKind::Integer));
                                b_.currentBlock_->instructions.push_back(ld);
                                std::string lenReg = b_.newTempRegister();
                                SIRInstruction len(SIROpcode::ARRAY_LEN);
                                len.result = SIROperand::Register(lenReg, SadTypeKind::Integer);
                                len.operands.push_back(argOperands[i]);
                                b_.currentBlock_->instructions.push_back(len);
                                std::string condReg = b_.newTempRegister();
                                SIRInstruction cmp = SIRInstruction::Binary(
                                    SIROpcode::LT,
                                    SIROperand::Register(condReg, SadTypeKind::Boolean),
                                    SIROperand::Register(loadedIdx, SadTypeKind::Integer),
                                    SIROperand::Register(lenReg, SadTypeKind::Integer));
                                b_.currentBlock_->instructions.push_back(cmp);
                                SIRInstruction brc = SIRInstruction::BranchCond(
                                    SIROperand::Register(condReg, SadTypeKind::Boolean),
                                    SIROperand::Label(bodyL), SIROperand::Label(exitL));
                                b_.currentBlock_->instructions.push_back(brc);
                            }
                            // (AR) كتلة الجسم: child = المصفوفة[idx] (مؤشّر)، ثمّ add_child.
                            b_.currentBlock_ = bodyB;
                            {
                                std::string loadedIdx = b_.newTempRegister();
                                SIRInstruction ld(SIROpcode::LOAD);
                                ld.result = SIROperand::Register(loadedIdx, SadTypeKind::Integer);
                                ld.operands.push_back(SIROperand::Register(idxAlloc, SadTypeKind::Integer));
                                b_.currentBlock_->instructions.push_back(ld);
                                std::string childReg = b_.newTempRegister();
                                SIRInstruction get(SIROpcode::ARRAY_GET);
                                get.result = SIROperand::Register(childReg, SadTypeKind::Pointer);
                                get.operands.push_back(argOperands[i]);
                                get.operands.push_back(SIROperand::Register(loadedIdx, SadTypeKind::Integer));
                                b_.currentBlock_->instructions.push_back(get);
                                SIRInstruction add(SIROpcode::BUILTIN_UI_ADD_CHILD);
                                add.operands.push_back(SIROperand::Register(r, SadTypeKind::Pointer));
                                add.operands.push_back(SIROperand::Register(childReg, SadTypeKind::Pointer));
                                b_.currentBlock_->instructions.push_back(add);
                                SIRInstruction brInc = SIRInstruction::Branch(SIROperand::Label(incL));
                                b_.currentBlock_->instructions.push_back(brInc);
                            }
                            // (AR) كتلة الزيادة: idx = idx + 1 ثمّ عُد للشرط.
                            b_.currentBlock_ = incB;
                            {
                                std::string loadedIdx = b_.newTempRegister();
                                SIRInstruction ld(SIROpcode::LOAD);
                                ld.result = SIROperand::Register(loadedIdx, SadTypeKind::Integer);
                                ld.operands.push_back(SIROperand::Register(idxAlloc, SadTypeKind::Integer));
                                b_.currentBlock_->instructions.push_back(ld);
                                std::string newIdx = b_.newTempRegister();
                                SIRInstruction inc = SIRInstruction::Binary(
                                    SIROpcode::ADD_I64,
                                    SIROperand::Register(newIdx, SadTypeKind::Integer),
                                    SIROperand::Register(loadedIdx, SadTypeKind::Integer),
                                    SIROperand::ConstantI64(1));
                                b_.currentBlock_->instructions.push_back(inc);
                                SIRInstruction st(SIROpcode::STORE);
                                st.operands.push_back(SIROperand::Register(newIdx, SadTypeKind::Integer));
                                st.operands.push_back(SIROperand::Register(idxAlloc, SadTypeKind::Integer));
                                b_.currentBlock_->instructions.push_back(st);
                                SIRInstruction brBack = SIRInstruction::Branch(SIROperand::Label(condL));
                                b_.currentBlock_->instructions.push_back(brBack);
                            }
                            // (AR) الاستمرار بعد اللولب في كتلة الخروج.
                            b_.currentBlock_ = exitB;
                        }
                        // (AR) غير ذلك (لا عنصر ولا مصفوفة) ⇒ تجاهُل آمن (نظير isWidgetBuilder).
                    }
                    return BuildResult(r, SadTypeKind::Pointer);
                };

                // =====================================================================
                // (AR) نظام الواجهة الموحد — مصانع العناصر / Unified UI Widget Factories
                // (EN) Returns SadWidget* pointer for each widget creation function
                // =====================================================================

                // ─── عمود(أبناء...) / sad_column() + sad_add_child لكلّ ابن ───
                if (funcName == Bn::UIWidgets::COLUMN)
                    return lowerContainer(SIROpcode::BUILTIN_UI_COLUMN);

                // ─── صف(أبناء...) / sad_row() ───
                if (funcName == Bn::UIWidgets::ROW)
                    return lowerContainer(SIROpcode::BUILTIN_UI_ROW);

                // ─── رصة() / sad_stack() ───
                // (AR) نطابق الاسم المعياريّ «رصة» (UIWidgets::STACK) فقط، مطابقةً
                //      للمفسّر الذي يسجّل المعياريّ حصرًا؛ المرادف التاريخيّ «مكدس»
                //      (CompilerUi::UI_2) مرفوض في المحرّكين معًا (تكافؤ تامّ، بلا
                //      تباعد باتجاهين). اللغة لم تُنشَر فلا حاجة لتوافق legacy.
                // (EN) Match canonical «رصة» (STACK) only, mirroring the interpreter;
                //      the legacy alias «مكدس» (UI_2) is rejected by both engines.
                if (funcName == Bn::UIWidgets::STACK)
                    return lowerContainer(SIROpcode::BUILTIN_UI_STACK);

                // ─── حاوية(أبناء...) / sad_container() ───
                if (funcName == Bn::UIWidgets::CONTAINER)
                    return lowerContainer(SIROpcode::BUILTIN_UI_CONTAINER);

                // ─── نص_عنصر(نص) / نص_عرض(نص) / sad_text(text) ───
                // (AR) الاسم المعياريّ للعنصر النصّيّ في مصدر الحقيقة هو «نص_عنصر»
                //      (UIWidgets::TEXT_WIDGET) وهو ما تستعمله البرامج والمفسّر؛
                //      «نص_عرض» (CompilerUi::UI_4) اسم تاريخيّ مرادف. كان المترجم
                //      يطابق المرادف فقط فيسقط «نص_عنصر» إلى VOID (تباعد صامت
                //      مفسّر↔مترجم — RFC 0001، P0-3). نوحّد المطابقة على الاسمين
                //      مع تقديم الاسم المعياريّ في الفحص (المسار الأكثر شيوعًا).
                // (EN) Canonical text-widget name is «نص_عنصر» (TEXT_WIDGET), used
                //      by programs and the interpreter; «نص_عرض» (UI_4) is a legacy
                //      alias. Match both to fix the silent interp↔compiler divergence;
                //      the canonical name is tested first (the common path).
                if (funcName == Bn::UIWidgets::TEXT_WIDGET ||
                    funcName == Bn::CompilerUi::UI_4)
                {
                    // (AR) #٤٠٠: الوسيطُ غيرُ النصّيِّ **لا يُمرَّرُ إلى المصنع**.
                    //   `sad_text` توقيعُها `const char*`، وجسرُ الوسائطِ يحوّلُ الصحيحَ
                    //   بـ`IntToPtr` فيُقرأُ العنوانُ ٤٢ نصًّا: `نص_عنصر(42)` ⇒ **SIGSEGV**
                    //   بينما المفسّرُ يطبعُ «نص(محتوى: 42)» (يفحصُ بنيةَ القيمةِ فيخزّنُها
                    //   صحيحًا). فنُنشئُ العقدةَ فارغةً ثمّ نكتبُ «محتوى» بالأوپكودِ الذي
                    //   يناسبُ النوعَ — وهو المسارُ الذي رقّاه #396 للخاصّيّةِ الأولى.
                    // (EN) #400: a non-string argument must not reach the factory: `sad_text`
                    //   takes `const char*` and the arg bridge IntToPtr's an integer, so
                    //   `نص_عنصر(42)` dereferences address 42 (SIGSEGV) while the interpreter
                    //   prints `نص(محتوى: 42)`. Create the node empty and write «محتوى»
                    //   with the type-appropriate opcode instead.
                    const bool contentArgIsText =
                        !argResults.empty() && argResults[0].type == SadTypeKind::String;
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_TEXT);
                    if (!argOperands.empty() && contentArgIsText)
                        inst.operands.push_back(argOperands[0]);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    if (!argOperands.empty() && !contentArgIsText)
                    {
                        // (AR) والمصفوفةُ تُصيَّرُ نصًّا أوّلًا: بدونِه يُختارُ لها
                        //      STR فيُقرأُ مؤشّرُ `SadArray` بـ`std::string(const char*)`
                        //      ⇒ قراءةٌ خارجَ الحدّ. المفسّرُ يُصيِّرُها بـ`toString`.
                        SIROperand valueOp = argOperands[0];
                        const bool asText = uiprops::lowerArrayValueToString(
                            b_, valueOp, argResults[0].type, argResults[0].elementType);
                        SIRInstruction sp(
                            asText ? SIROpcode::BUILTIN_UI_SET_PROP_STR
                                   : primaryPropOpcode(argResults[0].type, sad::ui::props::CONTENT));
                        sp.operands.push_back(SIROperand::Register(r, SadTypeKind::Pointer));
                        sp.operands.push_back(SIROperand::ConstantString(sad::ui::props::CONTENT));
                        sp.operands.push_back(valueOp);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(sp);
                        if (asText)
                            uiprops::releaseLoweredArrayString(b_, valueOp);
                    }
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── نص_منسق(نص,حجم,أحمر,أخضر,أزرق,شفافية) / sad_text_styled(...) ───
                if (funcName == Bn::CompilerUi::UI_5)
                {
                        return lowerTextSlotFactory(SIROpcode::BUILTIN_UI_TEXT_STYLED, sad::ui::props::CONTENT, 0);
                    }

                // ─── زر(عنوان,دالة_ضغط,بيانات) / sad_button(label,cb,data) ───
                if (funcName == Bn::UIWidgets::BUTTON)
                {
                        return lowerTextSlotFactory(SIROpcode::BUILTIN_UI_BUTTON, sad::ui::props::TITLE, 0);
                    }

                // ─── زر_نوع(عنوان,نوع,لون,دالة,بيانات) / sad_button_variant(...) ───
                if (funcName == Bn::CompilerUi::UI_7)
                {
                        return lowerTextSlotFactory(SIROpcode::BUILTIN_UI_BUTTON_VARIANT, sad::ui::props::TITLE, 0);
                    }

                // ─── زر_أيقونة() / sad_icon_button(icon,cb,data) ───
                // (AR) نطابق الاسم المعياريّ «زر_أيقونة» (UIWidgets::ICON_BUTTON،
                //      بهمزة) فقط، مطابقةً للمفسّر؛ المرادف التاريخيّ «زر_ايقونة»
                //      (CompilerUi::UI_8، يختلف ببايت الهمزة) مرفوض في المحرّكين معًا.
                // (EN) Match canonical «زر_أيقونة» (ICON_BUTTON, hamza) only;
                //      the legacy alias «زر_ايقونة» (UI_8) is rejected by both engines.
                if (funcName == Bn::UIWidgets::ICON_BUTTON)
                {
                        return lowerTextSlotFactory(SIROpcode::BUILTIN_UI_ICON_BUTTON, sad::ui::props::ICON, 0);
                    }

                // ─── زر_عائم(ايقونة,لون_r,لون_g,لون_b,لون_a,دالة,بيانات) / sad_fab(...) ───
                if (funcName == Bn::UIWidgets::FAB)
                {
                        return lowerTextSlotFactory(SIROpcode::BUILTIN_UI_FAB, sad::ui::props::ICON, 0);
                    }

                // ─── حقل_نص(تلميح,دالة,بيانات) / sad_text_field(hint,cb,data) ───
                if (funcName == Bn::UIWidgets::TEXT_FIELD)
                {
                        return lowerTextSlotFactory(SIROpcode::BUILTIN_UI_TEXT_FIELD, sad::ui::props::HINT, 0);
                    }

                // ─── خانة_اختيار() / sad_checkbox(cb,data) ───
                // (AR) نطابق الاسم المعياريّ «خانة_اختيار» (UIWidgets::CHECKBOX) فقط،
                //      مطابقةً للمفسّر؛ المرادف التاريخيّ «مربع_تحقق» (CompilerUi::UI_11)
                //      مرفوض في المحرّكين معًا.
                // (EN) Match canonical «خانة_اختيار» (CHECKBOX) only; the legacy
                //      alias «مربع_تحقق» (UI_11) is rejected by both engines.
                // (AR) خانة_اختيار(قيمة): مفتاح «قيمة» بنوعه (نظير منزلق) — لا يُعامَل
                //      الوسيط كردّ نداء (widget_builtins.cpp:151).
                if (funcName == Bn::UIWidgets::CHECKBOX)
                    return lowerValueWidget(SIROpcode::BUILTIN_UI_CHECKBOX);

                // ─── مفتاح() / sad_switch_toggle(cb,data) ───
                // (AR) نطابق الاسم المعياريّ «مفتاح» (UIWidgets::TOGGLE) فقط، مطابقةً
                //      للمفسّر؛ المرادف التاريخيّ «مبدل» (CompilerUi::UI_12) مرفوض في
                //      المحرّكين معًا (وخادم LSP يقترح «مفتاح» بديلًا لـ«مبدل»).
                // (EN) Match canonical «مفتاح» (TOGGLE) only; the legacy alias
                //      «مبدل» (UI_12) is rejected by both engines (LSP suggests مفتاح).
                // (AR) مفتاح(قيمة): مفتاح «قيمة» بنوعه (نظير منزلق) — لا يُعامَل الوسيط
                //      كردّ نداء (widget_builtins.cpp:147).
                if (funcName == Bn::UIWidgets::TOGGLE)
                    return lowerValueWidget(SIROpcode::BUILTIN_UI_SWITCH);

                // ─── منزلق(قيمة) / sad_slider ───
                // (AR) مواءمةٌ مع المفسّر: «منزلق(قيمة)» يضبط مفتاح «قيمة» بنوع الوسيط
                //      (widget_builtins.cpp:155 setIRPropertyFromValue)، لا (أدنى,أقصى).
                //      ننشئ عقدة Slider فارغة (SLIDER بلا operands) ثمّ نضبط «قيمة» عبر
                //      SET_PROP بحسب النوع (int/num/str) — طباعة_شجرة تطابق المفسّر تمامًا.
                if (funcName == Bn::UIWidgets::SLIDER)
                    return lowerValueWidget(SIROpcode::BUILTIN_UI_SLIDER);

                // ─── بطاقة(أبناء...) / sad_card() ───
                if (funcName == Bn::UIWidgets::CARD)
                    return lowerContainer(SIROpcode::BUILTIN_UI_CARD);

                // ─── هيكل(أبناء...) / sad_scaffold() ───
                if (funcName == Bn::UIWidgets::SCAFFOLD)
                    return lowerContainer(SIROpcode::BUILTIN_UI_SCAFFOLD);

                // ─── شريط_تطبيق(عنوان) / sad_app_bar(title) ───
                if (funcName == Bn::UIWidgets::APP_BAR)
                {
                        return lowerTextSlotFactory(SIROpcode::BUILTIN_UI_APP_BAR, sad::ui::props::TITLE, 1);
                    }

                // ─── فاصل() / sad_spacer() ───
                if (funcName == Bn::UIWidgets::SPACER)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SPACER);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── فاصل_خط() / sad_divider() ───
                // (AR) نطابق الاسم المعياريّ «فاصل_خط» (UIWidgets::DIVIDER) فقط،
                //      مطابقةً للمفسّر؛ المرادف التاريخيّ «خط_فاصل» (CompilerUi::UI_18)
                //      مرفوض في المحرّكين معًا.
                // (EN) Match canonical «فاصل_خط» (DIVIDER) only; the legacy alias
                //      «خط_فاصل» (UI_18) is rejected by both engines.
                if (funcName == Bn::UIWidgets::DIVIDER)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_DIVIDER);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── حوار(عنوان,رسالة) / sad_dialog(title,msg) ───
                if (funcName == Bn::UIWidgets::DIALOG)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_DIALOG);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // =====================================================================
                // (AR) مصانع إضافيّة (م-مصانع) — سدّ فجوة المترجم: 24 عنصرًا يدعمها
                //      المفسّر (widget_builtins.cpp) ولم يكن المترجم يلوّنها بالاسم.
                //      نطابق الاسم المعياريّ (UIWidgets::*) فقط، مطابقةً للمفسّر.
                //      العناصر البسيطة: لا معاملات (الأبناء يُضافون عبر الشجرة).
                //      عناصر بخاصّيّة: معامل أوّل اختياريّ (نظير نص_عنصر/صورة).
                // (EN) Extra factories closing the compiler gap vs the interpreter.
                //      Canonical-name match only. Simple widgets: no operands;
                //      prop widgets: optional first operand.
                // =====================================================================

                // ─── صورة(مصدر) / sad_image(source) ───
                if (funcName == Bn::UIWidgets::IMAGE)
                {
                        return lowerTextSlotFactory(SIROpcode::BUILTIN_UI_IMAGE, sad::ui::props::SOURCE, 1);
                    }

                // ─── أيقونة(اسم) / sad_icon(name) ───
                if (funcName == Bn::UIWidgets::ICON)
                {
                        return lowerTextSlotFactory(SIROpcode::BUILTIN_UI_ICON, sad::ui::props::NAME, 1);
                    }

                // ─── زر_نصي(عنوان) / sad_text_button(label) ───
                if (funcName == Bn::UIWidgets::TEXT_BUTTON)
                {
                        return lowerTextSlotFactory(SIROpcode::BUILTIN_UI_TEXT_BUTTON, sad::ui::props::TITLE, 1);
                    }

                // ─── شبكة(أبناء...) / sad_grid() ───
                if (funcName == Bn::UIWidgets::GRID)
                    return lowerContainer(SIROpcode::BUILTIN_UI_GRID);

                // ─── وسط(ابن) / sad_center() ───
                if (funcName == Bn::UIWidgets::CENTER)
                    return lowerContainer(SIROpcode::BUILTIN_UI_CENTER);

                // ─── حشوة(ابن) / sad_padding() ───
                if (funcName == Bn::UIWidgets::PADDING)
                    return lowerContainer(SIROpcode::BUILTIN_UI_PADDING);

                // ─── محاذاة(ابن) / sad_align() ───
                if (funcName == Bn::UIWidgets::ALIGN)
                    return lowerContainer(SIROpcode::BUILTIN_UI_ALIGN);

                // ─── موسع(ابن) / sad_expanded() ───
                if (funcName == Bn::UIWidgets::EXPANDED)
                    return lowerContainer(SIROpcode::BUILTIN_UI_EXPANDED);

                // ─── مرن(ابن) / sad_flexible() ───
                if (funcName == Bn::UIWidgets::FLEXIBLE)
                    return lowerContainer(SIROpcode::BUILTIN_UI_FLEXIBLE);

                // ─── مقاس(عرض,ارتفاع) / sad_sized_box(w,h) ───
                if (funcName == Bn::UIWidgets::SIZED_BOX)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SIZED_BOX);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── التفاف(أبناء...) / sad_wrap() ───
                if (funcName == Bn::UIWidgets::WRAP)
                    return lowerContainer(SIROpcode::BUILTIN_UI_WRAP);

                // ─── صندوق(أبناء...) / sad_box() ───
                if (funcName == Bn::UIWidgets::BOX)
                    return lowerContainer(SIROpcode::BUILTIN_UI_BOX);

                // ─── عرض_تمرير(ابن) / sad_scroll_view() ───
                if (funcName == Bn::UIWidgets::SCROLL_VIEW)
                    return lowerContainer(SIROpcode::BUILTIN_UI_SCROLL_VIEW);

                // ─── تنقل_سفلي(أبناء...) / sad_bottom_nav() ───
                if (funcName == Bn::UIWidgets::BOTTOM_NAV)
                    return lowerContainer(SIROpcode::BUILTIN_UI_BOTTOM_NAV);

                // ─── شريط_إشعار(رسالة) / sad_snackbar(msg) ───
                if (funcName == Bn::UIWidgets::SNACKBAR)
                {
                        return lowerTextSlotFactory(SIROpcode::BUILTIN_UI_SNACKBAR, sad::ui::props::MESSAGE, 1);
                    }

                // ─── تلميح(نص) / sad_tooltip(text) ───
                if (funcName == Bn::UIWidgets::TOOLTIP)
                {
                        return lowerTextSlotFactory(SIROpcode::BUILTIN_UI_TOOLTIP, sad::ui::props::TEXT, 1);
                    }

                // ─── شريط_تقدم(قيمة) / sad_progress(value) ───
                if (funcName == Bn::UIWidgets::PROGRESS)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_PROGRESS);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── عمود_كسول(أبناء...) / sad_lazy_column() ───
                if (funcName == Bn::UIWidgets::LAZY_COLUMN)
                    return lowerContainer(SIROpcode::BUILTIN_UI_LAZY_COLUMN);

                // ─── صف_كسول(أبناء...) / sad_lazy_row() ───
                if (funcName == Bn::UIWidgets::LAZY_ROW)
                    return lowerContainer(SIROpcode::BUILTIN_UI_LAZY_ROW);

                // ─── قائمة(أبناء...) / sad_list_view() ───
                if (funcName == Bn::UIWidgets::LIST_VIEW)
                    return lowerContainer(SIROpcode::BUILTIN_UI_LIST_VIEW);

                // ─── منطقة_نص(تلميح) / sad_text_area(hint) ───
                if (funcName == Bn::UIWidgets::TEXT_AREA)
                {
                        return lowerTextSlotFactory(SIROpcode::BUILTIN_UI_TEXT_AREA, sad::ui::props::HINT, 1);
                    }

                // ─── درج(أبناء...) / sad_drawer() ───
                if (funcName == Bn::UIWidgets::DRAWER)
                    return lowerContainer(SIROpcode::BUILTIN_UI_DRAWER);

                // ── تحكّم متقدّم + تغذية راجعة (م٥-ب/م٦-ب): عناصر حاوية بسيطة ──
                if (funcName == Bn::UIWidgets::SPIN_BOX)
                    return lowerContainer(SIROpcode::BUILTIN_UI_SPIN_BOX);
                if (funcName == Bn::UIWidgets::GROUP_BOX)
                    return lowerContainer(SIROpcode::BUILTIN_UI_GROUP_BOX);
                if (funcName == Bn::UIWidgets::SPINNER)
                    return lowerContainer(SIROpcode::BUILTIN_UI_SPINNER);
                if (funcName == Bn::UIWidgets::STATUS_BAR)
                    return lowerContainer(SIROpcode::BUILTIN_UI_STATUS_BAR);

                // ── ودجات سطح المكتب (م١–م٦): عناصر حاوية بسيطة ──
                if (funcName == Bn::UIWidgets::WINDOW)
                    return lowerContainer(SIROpcode::BUILTIN_UI_WINDOW);
                if (funcName == Bn::UIWidgets::TITLE_BAR)
                    return lowerContainer(SIROpcode::BUILTIN_UI_TITLE_BAR);
                if (funcName == Bn::UIWidgets::SCROLL_BAR)
                    return lowerContainer(SIROpcode::BUILTIN_UI_SCROLL_BAR);
                if (funcName == Bn::UIWidgets::TASKBAR)
                    return lowerContainer(SIROpcode::BUILTIN_UI_TASKBAR);
                if (funcName == Bn::UIWidgets::START_MENU)
                    return lowerContainer(SIROpcode::BUILTIN_UI_START_MENU);
                if (funcName == Bn::UIWidgets::SYSTEM_TRAY)
                    return lowerContainer(SIROpcode::BUILTIN_UI_SYSTEM_TRAY);
                if (funcName == Bn::UIWidgets::MENU)
                    return lowerContainer(SIROpcode::BUILTIN_UI_MENU);
                if (funcName == Bn::UIWidgets::TOOLBAR)
                    return lowerContainer(SIROpcode::BUILTIN_UI_TOOLBAR);
                if (funcName == Bn::UIWidgets::SEGMENTED_CONTROL)
                    return lowerContainer(SIROpcode::BUILTIN_UI_SEGMENTED_CONTROL);
                if (funcName == Bn::UIWidgets::TREE_VIEW)
                    return lowerContainer(SIROpcode::BUILTIN_UI_TREE_VIEW);
                if (funcName == Bn::UIWidgets::SPLIT_VIEW)
                    return lowerContainer(SIROpcode::BUILTIN_UI_SPLIT_VIEW);
                if (funcName == Bn::UIWidgets::DATA_TABLE)
                    return lowerContainer(SIROpcode::BUILTIN_UI_DATA_TABLE);
                if (funcName == Bn::UIWidgets::BREADCRUMB)
                    return lowerContainer(SIROpcode::BUILTIN_UI_BREADCRUMB);
                if (funcName == Bn::UIWidgets::PICKER)
                    return lowerContainer(SIROpcode::BUILTIN_UI_PICKER);
                if (funcName == Bn::UIWidgets::DATE_PICKER)
                    return lowerContainer(SIROpcode::BUILTIN_UI_DATE_PICKER);
                if (funcName == Bn::UIWidgets::STEPPER)
                    return lowerContainer(SIROpcode::BUILTIN_UI_STEPPER);
                if (funcName == Bn::UIWidgets::RATING_BAR)
                    return lowerContainer(SIROpcode::BUILTIN_UI_RATING_BAR);
                if (funcName == Bn::UIWidgets::ALERT)
                    return lowerContainer(SIROpcode::BUILTIN_UI_ALERT);

                // ─── منطقة_آمنة(ابن) / sad_safe_area() ───
                if (funcName == Bn::UIWidgets::SAFE_AREA)
                    return lowerContainer(SIROpcode::BUILTIN_UI_SAFE_AREA);

                // ─── سطح(أبناء...) / sad_surface() ───
                if (funcName == Bn::UIWidgets::SURFACE)
                    return lowerContainer(SIROpcode::BUILTIN_UI_SURFACE);

                // =====================================================================
                // (AR) إدارة شجرة العناصر / Tree Management
                // =====================================================================

                // ─── أضف_ابن(أب, ابن) / sad_add_child(parent, child) ───
                if (funcName == Bn::CompilerUi::UI_20)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_20, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_ADD_CHILD);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── أزل_ابن(أب, ابن) / sad_remove_child(parent, child) ───
                if (funcName == Bn::CompilerUi::UI_21)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_21, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_REMOVE_CHILD);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── امسح_الأبناء(عنصر) / sad_clear_children(widget) ───
                if (funcName == Bn::CompilerUi::UI_22)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_22, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_CLEAR_CHILDREN);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // =====================================================================
                // (AR) ضبط خصائص العناصر / Property Setters
                // =====================================================================

                // ─── عين_النص(عنصر, نص) / sad_set_text(widget, text) ───
                if (funcName == Bn::CompilerUi::UI_23)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_23, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_TEXT);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── عين_الحجم(عنصر, عرض, ارتفاع) / sad_set_size(widget, w, h) ───
                if (funcName == Bn::CompilerUi::UI_24)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_24, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_SIZE);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── عين_المرونة(عنصر, مرونة) / sad_set_flex(widget, flex) ───
                if (funcName == Bn::CompilerUi::UI_25)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_25, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_FLEX);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── عين_الخلفية(عنصر, أحمر, أخضر, أزرق, شفافية) / sad_set_background(w, r,g,b,a) ───
                if (funcName == Bn::CompilerUi::UI_26)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_26, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_BACKGROUND);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── عين_اللون(عنصر, أحمر, أخضر, أزرق, شفافية) / sad_set_foreground(w, r,g,b,a) ───
                if (funcName == Bn::CompilerUi::UI_27)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_27, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_FOREGROUND);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── عين_التباعد(عنصر, تباعد) / sad_set_spacing(w, spacing) ───
                if (funcName == Bn::CompilerUi::UI_28)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_28, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_SPACING);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── عين_الحشوة(عنصر, فوق, يمين, تحت, يسار) / sad_set_padding(w, t,r,b,l) ───
                if (funcName == Bn::CompilerUi::UI_29)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_29, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_PADDING);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── عين_المحاذاة(عنصر, رئيسية, فرعية) / sad_set_alignment(w, main, cross) ───
                if (funcName == Bn::CompilerUi::UI_30)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_30, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_ALIGNMENT);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── عين_الحدود(عنصر, سمك, أحمر, أخضر, أزرق, شفافية) / sad_set_border(...) ───
                if (funcName == Bn::CompilerUi::UI_31)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_31, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_BORDER);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── عين_الارتفاع(عنصر, ارتفاع) / sad_set_elevation(w, elev) ───
                if (funcName == Bn::CompilerUi::UI_32)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_32, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_ELEVATION);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── عين_الشفافية(عنصر, شفافية) / sad_set_opacity(w, opacity) ───
                if (funcName == Bn::CompilerUi::UI_33)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_33, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_OPACITY);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── عين_الظهور(عنصر, مرئي) / sad_set_visibility(w, visible) ───
                if (funcName == Bn::CompilerUi::UI_34)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_34, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_VISIBILITY);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // =====================================================================
                // (AR) إدارة التطبيق / App Management
                // =====================================================================

                // ─── انشئ_تطبيق() / sad_app_create() ───
                if (funcName == Bn::CompilerUi::UI_35)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_CREATE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── عين_الجذر(تطبيق, عنصر) / sad_app_set_root(app, widget) ───
                if (funcName == Bn::CompilerUi::UI_36)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_36, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_SET_ROOT);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── خطط(تطبيق, عرض, ارتفاع) / sad_app_layout(app, w, h) ───
                if (funcName == Bn::CompilerUi::UI_37)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_37, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_LAYOUT);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── ارسم(تطبيق) / sad_app_render(app) ───
                if (funcName == Bn::CompilerUi::UI_38)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_38, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_RENDER);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── تشغيل_تطبيق(عنصر) / sad_app_run(root) ───
                // (AR) نقطة الدخول العامّة (نظير runApp): جسرٌ يُشغّل حلقة سطح المكتب في
                //      المكتبة (DesktopWindow) ويُرسِل الأحداث إلى ردود النداء المُترجَمة.
                if (funcName == Bn::UICore::RUN_APP)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::UICore::RUN_APP, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_RUN);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── طباعة_شجرة(عنصر) / sad_print_tree(root) — تصحيح ───
                if (funcName == Bn::UICore::PRINT_TREE)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::UICore::PRINT_TREE, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_PRINT_TREE);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── م-تحكّم: دوال الثيم — جسرٌ إلى حالة الثيم المكتبيّة (sad::ui::*) ───
                // (AR) المنطق في المكتبة؛ هذه تُصدر نداءً فقط. تبديل/داكن/فاتح بلا وسائط
                //      وبلا إرجاع؛ هل_داكن بلا وسائط ويُرجع منطقيًّا (bool).
                if (funcName == Bn::UICore::TOGGLE_THEME ||
                    funcName == Bn::UICore::DARK_MODE ||
                    funcName == Bn::UICore::LIGHT_MODE)
                {
                    SIROpcode op = funcName == Bn::UICore::TOGGLE_THEME ? SIROpcode::BUILTIN_UI_TOGGLE_THEME
                                 : funcName == Bn::UICore::DARK_MODE    ? SIROpcode::BUILTIN_UI_DARK_MODE
                                                                        : SIROpcode::BUILTIN_UI_LIGHT_MODE;
                    SIRInstruction inst(op); // بلا operands
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::UICore::IS_DARK)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_IS_DARK); // بلا operands، يُرجع bool
                    inst.result = SIROperand::Register(r, SadTypeKind::Boolean);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Boolean);
                }

                // ─── التقط_مفتاح() / sad_next_key() → int (رمز مفتاح منتظِر أو 0) ───
                // (AR) جسر لوحة المفاتيح إلى كود ص: يعيد رمز أوّل مفتاحٍ منتظِرٍ ويستهلكه.
                //      بلا operands، يُرجع i64. حالته متغيّرة (يستنزف الطابور) ⇒ مُصنَّف
                //      ذا أثرٍ في cse_pass فلا تُدمَج نداءاته. [[التقط_مفتاح]]
                if (funcName == Bn::UICore::NEXT_KEY)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_NEXT_KEY); // بلا operands، يُرجع int
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                // ─── م-تحكّم: التنقّل — جسرٌ إلى مكدّس التنقّل المكتبيّ (sad::ui::nav) ───
                // (AR) المنطق في المكتبة؛ هذه تُصدر نداءً فقط. انتقل/استبدل يأخذان صفحةً
                //      (مقبض العنصر)؛ عودة/عودة_للبداية بلا وسائط؛ عدد_الصفحات يُرجع i64.
                if (funcName == Bn::UICore::NAVIGATE || funcName == Bn::UICore::REPLACE_PAGE)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), (funcName == Bn::UICore::NAVIGATE ? Ar::UICore::NAVIGATE : Ar::UICore::REPLACE_PAGE), argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    // (AR) الوسيط قد يكون **لقطة عنصر** (ptr) أو **بانِي صفحة** (دالّة ص
                    //      تُرجع عنصرًا ⇒ إغلاق i64). يميّزهما الخافض (emitUiNavigate/
                    //      ReplacePage) بنوع المُعامل: إغلاق ⇒ sad_navigate_builder (م1-ج،
                    //      تفاعليّة عبر buildCurrent)؛ عنصر ⇒ sad_navigate (لقطة). دوال ص
                    //      تُرجع widgets فعلًا (ارجع زر(..) ⇒ define ptr) فالبانِي مدعوم.
                    // (AR) حارس نوع (HIGH-2): ارفض ما ليس عنصرًا/دالّة (منع انهيار المترجم).
                    if (!checkUiNavArgType(b_.errors_,
                            funcName == Bn::UICore::NAVIGATE ? "\xd8\xa7\xd9\x86\xd8\xaa\xd9\x82\xd9\x84"    // انتقل
                                                             : "\xd8\xa7\xd8\xb3\xd8\xaa\xd8\xa8\xd8\xaf\xd9\x84", // استبدل
                            argResults[0].type))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(funcName == Bn::UICore::NAVIGATE ? SIROpcode::BUILTIN_UI_NAVIGATE
                                                                        : SIROpcode::BUILTIN_UI_REPLACE_PAGE);
                    inst.operands.push_back(argOperands[0]); // الصفحة (عنصر أو بانٍ)
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                // (م2) انتقل_بتحريك(صفحة, نوع, مدة؟) — تنقّل + انتقال بصريّ.
                if (funcName == Bn::UICore::NAVIGATE_TRANSITION)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::UICore::NAVIGATE_TRANSITION, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    // (AR) حارس نوع (HIGH-2): الصفحة عنصر أو دالّة بناء (منع انهيار المترجم).
                    if (!checkUiNavArgType(b_.errors_, "\xd8\xa7\xd9\x86\xd8\xaa\xd9\x82\xd9\x84_\xd8\xa8\xd8\xaa\xd8\xad\xd8\xb1\xd9\x8a\xd9\x83", argResults[0].type)) // انتقل_بتحريك
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_NAVIGATE_TRANSITION);
                    inst.operands.push_back(argOperands[0]); // الصفحة
                    inst.operands.push_back(argOperands[1]); // نوع الانتقال
                    if (argResults.size() > 2)
                        inst.operands.push_back(argOperands[2]); // مدة (اختياريّة)
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                // (م2) عودة_بتحريك(نوع؟, مدة؟) — عودة + انتقال بصريّ.
                if (funcName == Bn::UICore::BACK_TRANSITION)
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_BACK_TRANSITION);
                    if (!argResults.empty())
                        inst.operands.push_back(argOperands[0]); // نوع (اختياريّ)
                    if (argResults.size() > 1)
                        inst.operands.push_back(argOperands[1]); // مدة (اختياريّة)
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::UICore::BACK || funcName == Bn::UICore::BACK_TO_ROOT)
                {
                    SIRInstruction inst(funcName == Bn::UICore::BACK ? SIROpcode::BUILTIN_UI_NAV_BACK
                                                                     : SIROpcode::BUILTIN_UI_NAV_ROOT);
                    if (b_.currentBlock_) // نداء void بلا وسائط (نتيجة عودة المنطقيّة تُهمَل كالمفسّر)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::UICore::PAGE_COUNT)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_PAGE_COUNT); // بلا operands، يُرجع i64
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::UICore::CURRENT_PAGE)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_CURRENT_PAGE); // بلا operands، يُرجع SadWidget
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ═════════════════════════════════════════════════════════════════
                // (AR) إكمال corui — الانتقال الكامل + الحالة + النافذة + توليد الويب.
                //      المنطق كلّه في المكتبة (nav/windowController/HtmlCodegen)؛ هذه
                //      تُصدر نداءً فقط (جسرٌ رفيع، نظير المفسّر). أسماء الرسائل مشتقّة من
                //      ثوابت مصدر الحقيقة (Bn::UICore::*) لا نصوصًا حرفيّة.
                // ═════════════════════════════════════════════════════════════════

                // (إكمال) انتقل_بتحريك_كامل(صفحة, دخول, خروج, مدة؟) — تنقّل + دخول/خروج.
                if (funcName == Bn::UICore::NAVIGATE_EXIT_TRANSITION)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::UICore::NAVIGATE_EXIT_TRANSITION, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    // (HIGH-2) حارس نوع: الصفحة عنصر أو دالّة بناء (منع انهيار المترجم).
                    if (!checkUiNavArgType(b_.errors_, std::string(Bn::UICore::NAVIGATE_EXIT_TRANSITION), argResults[0].type))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_NAVIGATE_EXIT_TRANSITION);
                    inst.operands.push_back(argOperands[0]); // الصفحة (عنصر أو بانٍ)
                    inst.operands.push_back(argOperands[1]); // نوع الدخول
                    inst.operands.push_back(argOperands[2]); // نوع الخروج
                    if (argResults.size() > 3)
                        inst.operands.push_back(argOperands[3]); // مدة (اختياريّة)
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                // (إكمال) تحديث_حالة() — يطلب إعادة رسم (بلا وسائط).
                if (funcName == Bn::UICore::UPDATE_STATE)
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_UPDATE_STATE); // بلا operands
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                // (rfcs#51) أوقف_الانتشار() — يوقف انتشار الحدث الجاري (بلا وسائط).
                if (funcName == Bn::UICore::STOP_PROPAGATION)
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_STOP_PROPAGATION);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                // (إكمال) عين_الحالة(دالّة_تحديث؟) — نداء دالّة التحديث تزامنيًّا ثمّ إعادة رسم.
                //   نمرّر المُعامل فقط إن كان **دالّة** (Function): الخافض يستدعي ثانك الإغلاق
                //   `void(i64 __env)` تزامنيًّا ثمّ sad_update_state. غير الدالّة يُتجاهَل
                //   والفعل يقتصر على إعادة الرسم.
                //   ⚠ حدٌّ موثَّق (Amelia M3، تباعد ضيّق مقبول): **كائنٌ قابل للاستدعاء**
                //     (مثيل صنف بـ()، isCallable في المفسّر) ليس Function فلا يُستدعى هنا —
                //     المفسّر يستدعيه. لا نوسّع الحارس إلى Object لأنّ ثانك الإغلاق يفترض
                //     تخطيط إغلاق {fn@0,env@1}؛ استدعاؤه على كائنٍ مختلف التخطيط = انهيار
                //     (أسوأ من تجاهلٍ صامت). دعمُه الصحيح يحتاج مسار buildCallableObjectInvoke
                //     (شريحة لاحقة). الاستعمال الشائع (دالّة/لامدا) مدعومٌ ومُختبَر.
                if (funcName == Bn::UICore::SET_STATE)
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_STATE);
                    if (!argResults.empty() && argResults[0].type == SadTypeKind::Function)
                        inst.operands.push_back(argOperands[0]); // دالّة التحديث (إغلاق)
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                // (إكمال) عنوان_النافذة(نص) — يطلب تغيير عنوان النافذة عبر المتحكّم المشترك.
                if (funcName == Bn::UICore::SET_TITLE)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::UICore::SET_TITLE, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    // (Amelia M2) حارس نوع نصّ (تكافؤ مع المفسّر !isString): يرفض العنوان
                    //   العدديّ/المنطقيّ (وإلّا مرّ non-pointer فتحوّل إلى null صامتًا).
                    if (!checkUiTitleArgType(b_.errors_, std::string(Bn::UICore::SET_TITLE), argResults[0].type))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SET_TITLE);
                    inst.operands.push_back(argOperands[0]); // العنوان (نص)
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                // (إكمال) أغلق_النافذة() — يطلب إغلاق النافذة (بلا وسائط).
                if (funcName == Bn::UICore::CLOSE_WINDOW)
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_CLOSE_WINDOW); // بلا operands
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                // (إكمال) أبوابُ توليدِ كودِ المنصّة: توليد_ويب/أندرويد/آي_أو_إس/ماك
                //   (عنصر, اسم؟) ⇒ نصٌّ (String). أربعتُها متطابقةُ الشكلِ والحراسة،
                //   ولا تفترق إلّا في الأوپكود ⇒ جدولٌ واحدٌ بدل أربعِ نسخٍ متباعدة.
                {
                    // (AR) الرتبةُ تُحمَل في الجدولِ نفسِه لا تُكتَب رقمًا عند الفحص:
                    //      الأربعةُ متطابقةُ المدى اليومَ، وحملُها هنا يجعلُ افتراقَ
                    //      أحدِها غدًا سطرًا في مصدرِ الحقيقةِ لا رقمًا يسهو عنه القارئ.
                    struct PlatformCodegen
                    {
                        std::string_view name;
                        SIROpcode opcode;
                        Sad::Builtins::Arity::Range arity;
                    };
                    static const PlatformCodegen kPlatformCodegens[] = {
                        {Bn::UICore::GEN_WEB, SIROpcode::BUILTIN_UI_GEN_WEB, Ar::UICore::GEN_WEB},
                        {Bn::UICore::GEN_ANDROID, SIROpcode::BUILTIN_UI_GEN_ANDROID, Ar::UICore::GEN_ANDROID},
                        {Bn::UICore::GEN_IOS, SIROpcode::BUILTIN_UI_GEN_IOS, Ar::UICore::GEN_IOS},
                        {Bn::UICore::GEN_MACOS, SIROpcode::BUILTIN_UI_GEN_MACOS, Ar::UICore::GEN_MACOS},
                    };
                    for (const auto &[name, opcode, arity] : kPlatformCodegens)
                    {
                        if (funcName != name)
                            continue;
                        if (!checkBuiltinArity(b_.errors_, std::string(funcName), arity, argResults.size()))
                            return BuildResult("", SadTypeKind::Void);
                        // (Amelia مراجعة2، MEDIUM) حارس نوع الوسيط: عنصر (Pointer) أو دالّة بناء
                        //   (Function) فقط. بدونه يُصنّف bridgeUiPageBuilder أيّ i64 (عدد) بانيًا
                        //   ⇒ inttoptr لقيمةٍ عدديّة ثمّ قراءة {fn,env} من عنوانٍ باطل ⇒ انهيار
                        //   (بينما المفسّر يرفضه بأمان). نظير حرّاس انتقل/انتقل_بتحريك.
                        if (!checkUiNavArgType(b_.errors_, std::string(name), argResults[0].type))
                            return BuildResult("", SadTypeKind::Void);
                        std::string r = b_.newTempRegister();
                        SIRInstruction inst(opcode);
                        inst.operands.push_back(argOperands[0]); // العنصر (أو بانٍ)
                        // (Amelia مراجعة١، متوسطة) الوسيطُ الثاني يُمرَّر إلى
                        //   const char*. بدونَ حارسٍ يمرُّ **أيُّ** مؤشّرٍ (عنصرُ
                        //   واجهةٍ مثلًا) في موضعِ النصّ ⇒ قراءةُ ذاكرةٍ عشوائيّةٍ
                        //   وانهيار، بينما المفسّرُ يتجاهله بأمان
                        //   (args[1]->isString()). نُسقِط هنا الأنواعَ المؤشِّريّةَ
                        //   المعروفةَ **قطعًا** أنّها ليست نصًّا فنُطابق المفسّر؛
                        //   ونُبقي المجهولَ/أيّ فلا نُسقِط نصًّا مشروعًا لم يُستنتَج
                        //   نوعُه (وهو ما يُحدِث تباعُدًا في الاتّجاهِ المضادّ).
                        //   القيمُ غيرُ المؤشِّريّة (عددٌ مثلًا) يُصفّرها فحصُ
                        //   isPointerTy في باعثِ LLVM أصلًا.
                        if (argResults.size() > 1
                            && !isNonStringPointerLikeType(argResults[1].type))
                            inst.operands.push_back(argOperands[1]); // الاسم (اختياريّ)
                        inst.result = SIROperand::Register(r, SadTypeKind::String);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(inst);
                        return BuildResult(r, SadTypeKind::String);
                    }
                }

                // ─── دمر_تطبيق(تطبيق) / sad_app_destroy(app) ───
                if (funcName == Bn::CompilerUi::UI_39)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_39, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_DESTROY);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── دمر_عنصر(عنصر) / sad_widget_destroy(widget) ───
                if (funcName == Bn::CompilerUi::UI_40)
                {
                    if (!checkBuiltinArity(b_.errors_, std::string(funcName), Ar::CompilerUi::UI_40, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_WIDGET_DESTROY);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }


                // ═════════════════════════════════════════════════════════════
                // (AR) الاحتياطيُّ الشامل: أيُّ مصنعِ عنصرٍ في مصدرِ الحقيقةِ لم
                //   يُطابِقه أوپكودٌ مخصَّصٌ أعلاه يُخفَّض إلى الأوپكودِ العامِّ
                //   حاملًا **رقمَ نوعِ عقدتِه** من الجدولِ المولَّد.
                //
                //   قبلَ ذلك كانت ٤٨ عقدةً تُصيَّر في خمسةِ مساراتٍ ولا يُنشئها
                //   المصرِّفُ أصلًا؛ وكان سدُّ ذلك يعني ٤٨ أوپكودًا و٤٨ دالّةَ ABI
                //   — أي مضاعفةَ ما على الخلفيّةِ السياديّةِ أن تعالجه لا سدَّه.
                //
                //   الجدولُ مولَّدٌ من language-truth: لا اسمَ عنصرٍ حرفيٌّ هنا،
                //   ولا رقمَ عقدةٍ مكتوبٌ بيد. عنصرٌ جديدٌ في مصدرِ الحقيقةِ يصير
                //   قابلًا للترجمةِ دونَ لمسِ هذا الملفّ.
                // ═════════════════════════════════════════════════════════════
                {
                    struct UiFactoryInfo
                    {
                        int nodeIndex;
                        const char *primaryProp; // فارغةٌ = لا خاصّيّةَ أولى
                    };
#define SAD_UI_FACTORY_ENTRY(Name, NodeIndex, PropKey) {Name, UiFactoryInfo{NodeIndex, PropKey}},
                    static const std::unordered_map<std::string, UiFactoryInfo> kUiFactories = {
                        SAD_UI_WIDGET_FACTORY_LIST(SAD_UI_FACTORY_ENTRY)};
#undef SAD_UI_FACTORY_ENTRY
                    static_assert(SAD_UI_WIDGET_FACTORY_COUNT > 0,
                                  "جدولُ مصانعِ العناصرِ المولَّدُ فارغ");

                    const auto factory = kUiFactories.find(funcName);
                    if (factory != kUiFactories.end())
                    {
                        // (AR) نُعيد استعمالَ lowerContainer نفسِها لا نسخةً منها:
                        //   نشرُ المصفوفةِ وقبولُ الابنِ ديناميِّ النوعِ منطقٌ مقيسٌ
                        //   بالبكسل، وأيُّ نسخةٍ ثانيةٍ منه تتباعد عنه صامتةً — وهو
                        //   عينُ الداءِ الذي تعالجه هذه الحملة.
                        const auto nodeIndexOperand =
                            SIROperand::ConstantI64(factory->second.nodeIndex);
                        return lowerContainer(SIROpcode::BUILTIN_UI_WIDGET_BY_TYPE,
                                              &nodeIndexOperand,
                                              factory->second.primaryProp);
                    }
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad