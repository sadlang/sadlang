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
#include <iostream>
#include <filesystem>
#include <optional>

#include "builtin_registry.h"
#include "error_manager.h" // (AR) buildBilingualMessage من كتالوج الأخطاء (مصدر الحقيقة)
#include "error_catalog.h" // (AR) RenderContext (حاملُ placeholders)
#include "error_codes.h"   // (AR) ErrorCode::SEM_WRONG_ARG_COUNT
namespace Bn = Sad::Builtins::Names;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // (AR) يُبلّغ عن «عدد معاملات خاطئ» (SEM005): الرسالة تُبنى من كتالوج
            //      language-truth/errors (مصدر الحقيقة الوحيد — لا نصوصَ يدويّة) عبر
            //      buildBilingualMessage، ثمّ تُطبَع فورًا. هذا **نفس نمط الواجهة
            //      الخلفيّة** (llvm_codegen_context.cpp:47: reportError(
            //      buildBilingualMessage(code, ctx)) الذي يطبع على stderr). لا نستعمل
            //      reportFromCatalog (الـbatch) لأنّ المترجم لا يفرّغه (لا printAll).
            //      name=اسم المُدمَجة، expected=العدد المطلوب، found=المُمرَّر فعلًا.
            static void reportUiWrongArgCount(const std::string &name, int expected, size_t found)
            {
                Sad::Errors::RenderContext ctx;
                ctx.placeholders = {
                    {"name", name},
                    {"expected", std::to_string(expected)},
                    {"found", std::to_string(found)}};
                std::cerr << Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                                 Sad::Errors::ErrorCode::SEM_WRONG_ARG_COUNT, ctx)
                          << std::endl;
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
                auto lowerValueWidget = [&](SIROpcode factoryOp) -> BuildResult {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(factoryOp); // بلا operands ⇒ عقدة فارغة
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    if (!argResults.empty())
                    {
                        SIROpcode op =
                            argResults[0].type == SadTypeKind::Integer ? SIROpcode::BUILTIN_UI_SET_PROP_INT
                          : argResults[0].type == SadTypeKind::Float   ? SIROpcode::BUILTIN_UI_SET_PROP_NUM
                          : argResults[0].type == SadTypeKind::Boolean ? SIROpcode::BUILTIN_UI_SET_PROP_BOOL
                                                                       : SIROpcode::BUILTIN_UI_SET_PROP_STR;
                        SIRInstruction sp(op);
                        sp.operands.push_back(SIROperand::Register(r, SadTypeKind::Pointer)); // العنصر
                        sp.operands.push_back(SIROperand::ConstantString("\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9")); // قيمة
                        sp.operands.push_back(argOperands[0]);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(sp);
                    }
                    return BuildResult(r, SadTypeKind::Pointer);
                };

                // =====================================================================
                // (AR) نظام الواجهة الموحد — مصانع العناصر / Unified UI Widget Factories
                // (EN) Returns SadWidget* pointer for each widget creation function
                // =====================================================================

                // ─── عمود() / sad_column() ───
                if (funcName == Bn::UIWidgets::COLUMN)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_COLUMN);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── صف() / sad_row() ───
                if (funcName == Bn::UIWidgets::ROW)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_ROW);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── رصة() / sad_stack() ───
                // (AR) نطابق الاسم المعياريّ «رصة» (UIWidgets::STACK) فقط، مطابقةً
                //      للمفسّر الذي يسجّل المعياريّ حصرًا؛ المرادف التاريخيّ «مكدس»
                //      (CompilerUi::UI_2) مرفوض في المحرّكين معًا (تكافؤ تامّ، بلا
                //      تباعد باتجاهين). اللغة لم تُنشَر فلا حاجة لتوافق legacy.
                // (EN) Match canonical «رصة» (STACK) only, mirroring the interpreter;
                //      the legacy alias «مكدس» (UI_2) is rejected by both engines.
                if (funcName == Bn::UIWidgets::STACK)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_STACK);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── حاوية() / sad_container() ───
                if (funcName == Bn::UIWidgets::CONTAINER)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_CONTAINER);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

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
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_TEXT);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── نص_منسق(نص,حجم,أحمر,أخضر,أزرق,شفافية) / sad_text_styled(...) ───
                if (funcName == Bn::CompilerUi::UI_5)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_TEXT_STYLED);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── زر(عنوان,دالة_ضغط,بيانات) / sad_button(label,cb,data) ───
                if (funcName == Bn::UIWidgets::BUTTON)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_BUTTON);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── زر_نوع(عنوان,نوع,لون,دالة,بيانات) / sad_button_variant(...) ───
                if (funcName == Bn::CompilerUi::UI_7)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_BUTTON_VARIANT);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── زر_أيقونة() / sad_icon_button(icon,cb,data) ───
                // (AR) نطابق الاسم المعياريّ «زر_أيقونة» (UIWidgets::ICON_BUTTON،
                //      بهمزة) فقط، مطابقةً للمفسّر؛ المرادف التاريخيّ «زر_ايقونة»
                //      (CompilerUi::UI_8، يختلف ببايت الهمزة) مرفوض في المحرّكين معًا.
                // (EN) Match canonical «زر_أيقونة» (ICON_BUTTON, hamza) only;
                //      the legacy alias «زر_ايقونة» (UI_8) is rejected by both engines.
                if (funcName == Bn::UIWidgets::ICON_BUTTON)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_ICON_BUTTON);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── زر_عائم(ايقونة,لون_r,لون_g,لون_b,لون_a,دالة,بيانات) / sad_fab(...) ───
                if (funcName == Bn::UIWidgets::FAB)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_FAB);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── حقل_نص(تلميح,دالة,بيانات) / sad_text_field(hint,cb,data) ───
                if (funcName == Bn::UIWidgets::TEXT_FIELD)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_TEXT_FIELD);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
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

                // ─── بطاقة() / sad_card() ───
                if (funcName == Bn::UIWidgets::CARD)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_CARD);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── هيكل() / sad_scaffold() ───
                if (funcName == Bn::UIWidgets::SCAFFOLD)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SCAFFOLD);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── شريط_تطبيق(عنوان) / sad_app_bar(title) ───
                if (funcName == Bn::UIWidgets::APP_BAR)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_BAR);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
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
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_IMAGE);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── أيقونة(اسم) / sad_icon(name) ───
                if (funcName == Bn::UIWidgets::ICON)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_ICON);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── زر_نصي(عنوان) / sad_text_button(label) ───
                if (funcName == Bn::UIWidgets::TEXT_BUTTON)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_TEXT_BUTTON);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── شبكة() / sad_grid() ───
                if (funcName == Bn::UIWidgets::GRID)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_GRID);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── وسط() / sad_center() ───
                if (funcName == Bn::UIWidgets::CENTER)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_CENTER);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── حشوة() / sad_padding() ───
                if (funcName == Bn::UIWidgets::PADDING)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_PADDING);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── محاذاة() / sad_align() ───
                if (funcName == Bn::UIWidgets::ALIGN)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_ALIGN);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── موسع() / sad_expanded() ───
                if (funcName == Bn::UIWidgets::EXPANDED)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_EXPANDED);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── مرن() / sad_flexible() ───
                if (funcName == Bn::UIWidgets::FLEXIBLE)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_FLEXIBLE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

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

                // ─── التفاف() / sad_wrap() ───
                if (funcName == Bn::UIWidgets::WRAP)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_WRAP);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── صندوق() / sad_box() ───
                if (funcName == Bn::UIWidgets::BOX)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_BOX);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── عرض_تمرير() / sad_scroll_view() ───
                if (funcName == Bn::UIWidgets::SCROLL_VIEW)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SCROLL_VIEW);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── تنقل_سفلي() / sad_bottom_nav() ───
                if (funcName == Bn::UIWidgets::BOTTOM_NAV)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_BOTTOM_NAV);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── شريط_إشعار(رسالة) / sad_snackbar(msg) ───
                if (funcName == Bn::UIWidgets::SNACKBAR)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SNACKBAR);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── تلميح(نص) / sad_tooltip(text) ───
                if (funcName == Bn::UIWidgets::TOOLTIP)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_TOOLTIP);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
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

                // ─── عمود_كسول() / sad_lazy_column() ───
                if (funcName == Bn::UIWidgets::LAZY_COLUMN)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_LAZY_COLUMN);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── صف_كسول() / sad_lazy_row() ───
                if (funcName == Bn::UIWidgets::LAZY_ROW)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_LAZY_ROW);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── قائمة() / sad_list_view() ───
                if (funcName == Bn::UIWidgets::LIST_VIEW)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_LIST_VIEW);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── منطقة_نص(تلميح) / sad_text_area(hint) ───
                if (funcName == Bn::UIWidgets::TEXT_AREA)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_TEXT_AREA);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── درج() / sad_drawer() ───
                if (funcName == Bn::UIWidgets::DRAWER)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_DRAWER);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── منطقة_آمنة() / sad_safe_area() ───
                if (funcName == Bn::UIWidgets::SAFE_AREA)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SAFE_AREA);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── سطح() / sad_surface() ───
                if (funcName == Bn::UIWidgets::SURFACE)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SURFACE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // =====================================================================
                // (AR) إدارة شجرة العناصر / Tree Management
                // =====================================================================

                // ─── أضف_ابن(أب, ابن) / sad_add_child(parent, child) ───
                if (funcName == Bn::CompilerUi::UI_20)
                {
                    if (argResults.size() < 2)
                    {
                        reportUiWrongArgCount("أضف_ابن", 2, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.size() < 2)
                    {
                        reportUiWrongArgCount("أزل_ابن", 2, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.empty())
                    {
                        reportUiWrongArgCount("امسح_الأبناء", 1, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.size() < 2)
                    {
                        reportUiWrongArgCount("عين_النص", 2, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.size() < 3)
                    {
                        reportUiWrongArgCount("عين_الحجم", 3, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.size() < 2)
                    {
                        reportUiWrongArgCount("عين_المرونة", 2, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.size() < 5)
                    {
                        reportUiWrongArgCount("عين_الخلفية", 5, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.size() < 5)
                    {
                        reportUiWrongArgCount("عين_اللون", 5, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.size() < 2)
                    {
                        reportUiWrongArgCount("عين_التباعد", 2, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.size() < 5)
                    {
                        reportUiWrongArgCount("عين_الحشوة", 5, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.size() < 3)
                    {
                        reportUiWrongArgCount("عين_المحاذاة", 3, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.size() < 2)
                    {
                        reportUiWrongArgCount("عين_الحدود", 2, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.size() < 2)
                    {
                        reportUiWrongArgCount("عين_الارتفاع", 2, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.size() < 2)
                    {
                        reportUiWrongArgCount("عين_الشفافية", 2, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.size() < 2)
                    {
                        reportUiWrongArgCount("عين_الظهور", 2, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.size() < 2)
                    {
                        reportUiWrongArgCount("عين_الجذر", 2, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.size() < 3)
                    {
                        reportUiWrongArgCount("خطط", 3, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.empty())
                    {
                        reportUiWrongArgCount("ارسم", 1, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
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
                    if (argResults.empty())
                    {
                        reportUiWrongArgCount("تشغيل_تطبيق", 1, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_RUN);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── طباعة_شجرة(عنصر) / sad_print_tree(root) — تصحيح ───
                if (funcName == Bn::UICore::PRINT_TREE)
                {
                    if (argResults.empty())
                    {
                        reportUiWrongArgCount("طباعة_شجرة", 1, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_PRINT_TREE);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── دمر_تطبيق(تطبيق) / sad_app_destroy(app) ───
                if (funcName == Bn::CompilerUi::UI_39)
                {
                    if (argResults.empty())
                    {
                        reportUiWrongArgCount("دمر_تطبيق", 1, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_DESTROY);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── دمر_عنصر(عنصر) / sad_widget_destroy(widget) ───
                if (funcName == Bn::CompilerUi::UI_40)
                {
                    if (argResults.empty())
                    {
                        reportUiWrongArgCount("دمر_عنصر", 1, argResults.size());
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_WIDGET_DESTROY);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }


                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad