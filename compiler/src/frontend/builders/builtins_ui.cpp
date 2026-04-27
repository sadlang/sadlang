// ============================================================================
// sir_builder_builtins_ui.cpp
// (AR) UI Widget Factory builtins: column, row, text, button, app
// (EN) Unified UI Widget Factory Functions
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

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            std::optional<BuildResult> BuiltinBuilder::buildBuiltinSystem_UI(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {

                // =====================================================================
                // (AR) نظام الواجهة الموحد — مصانع العناصر / Unified UI Widget Factories
                // (EN) Returns SadWidget* pointer for each widget creation function
                // =====================================================================

                // ─── عمود() / sad_column() ───
                if (funcName == "عمود" || funcName == "ui_column")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_COLUMN);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── صف() / sad_row() ───
                if (funcName == "صف" || funcName == "ui_row")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_ROW);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── مكدس() / sad_stack() ───
                if (funcName == "مكدس" || funcName == "ui_stack")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_STACK);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── حاوية() / sad_container() ───
                if (funcName == "حاوية" || funcName == "ui_container")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_CONTAINER);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── نص_عرض(نص) / sad_text(text) ───
                if (funcName == "نص_عرض" || funcName == "ui_text")
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
                if (funcName == "نص_منسق" || funcName == "ui_text_styled")
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
                if (funcName == "زر" || funcName == "ui_button")
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
                if (funcName == "زر_نوع" || funcName == "ui_button_variant")
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

                // ─── زر_ايقونة(ايقونة,دالة,بيانات) / sad_icon_button(icon,cb,data) ───
                if (funcName == "زر_ايقونة" || funcName == "ui_icon_button")
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
                if (funcName == "زر_عائم" || funcName == "ui_fab")
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
                if (funcName == "حقل_نص" || funcName == "ui_text_field")
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

                // ─── مربع_تحقق(دالة,بيانات) / sad_checkbox(cb,data) ───
                if (funcName == "مربع_تحقق" || funcName == "ui_checkbox")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_CHECKBOX);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── مبدل(دالة,بيانات) / sad_switch_toggle(cb,data) ───
                if (funcName == "مبدل" || funcName == "ui_switch")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SWITCH);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── منزلق(حد_أدنى,حد_أقصى,دالة,بيانات) / sad_slider(min,max,cb,data) ───
                if (funcName == "منزلق" || funcName == "ui_slider")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SLIDER);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── بطاقة() / sad_card() ───
                if (funcName == "بطاقة" || funcName == "ui_card")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_CARD);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── هيكل() / sad_scaffold() ───
                if (funcName == "هيكل" || funcName == "ui_scaffold")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SCAFFOLD);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── شريط_تطبيق(عنوان) / sad_app_bar(title) ───
                if (funcName == "شريط_تطبيق" || funcName == "ui_app_bar")
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
                if (funcName == "فاصل" || funcName == "ui_spacer")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_SPACER);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── خط_فاصل() / sad_divider() ───
                if (funcName == "خط_فاصل" || funcName == "ui_divider")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_DIVIDER);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── حوار(عنوان,رسالة) / sad_dialog(title,msg) ───
                if (funcName == "حوار" || funcName == "ui_dialog")
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
                // (AR) إدارة شجرة العناصر / Tree Management
                // =====================================================================

                // ─── أضف_ابن(أب, ابن) / sad_add_child(parent, child) ───
                if (funcName == "أضف_ابن" || funcName == "ui_add_child")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] أضف_ابن تتطلب معاملين: الأب والابن" << std::endl;
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
                if (funcName == "أزل_ابن" || funcName == "ui_remove_child")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] أزل_ابن تتطلب معاملين" << std::endl;
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
                if (funcName == "امسح_الأبناء" || funcName == "ui_clear_children")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] امسح_الأبناء تتطلب معامل واحد" << std::endl;
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
                if (funcName == "عين_النص" || funcName == "ui_set_text")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] عين_النص تتطلب معاملين: العنصر والنص" << std::endl;
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
                if (funcName == "عين_الحجم" || funcName == "ui_set_size")
                {
                    if (argResults.size() < 3)
                    {
                        std::cerr << "[خطأ] عين_الحجم تتطلب 3 معاملات" << std::endl;
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
                if (funcName == "عين_المرونة" || funcName == "ui_set_flex")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] عين_المرونة تتطلب معاملين" << std::endl;
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
                if (funcName == "عين_الخلفية" || funcName == "ui_set_background")
                {
                    if (argResults.size() < 5)
                    {
                        std::cerr << "[خطأ] عين_الخلفية تتطلب 5 معاملات: العنصر + 4 ألوان" << std::endl;
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
                if (funcName == "عين_اللون" || funcName == "ui_set_foreground")
                {
                    if (argResults.size() < 5)
                    {
                        std::cerr << "[خطأ] عين_اللون تتطلب 5 معاملات" << std::endl;
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
                if (funcName == "عين_التباعد" || funcName == "ui_set_spacing")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] عين_التباعد تتطلب معاملين" << std::endl;
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
                if (funcName == "عين_الحشوة" || funcName == "ui_set_padding")
                {
                    if (argResults.size() < 5)
                    {
                        std::cerr << "[خطأ] عين_الحشوة تتطلب 5 معاملات" << std::endl;
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
                if (funcName == "عين_المحاذاة" || funcName == "ui_set_alignment")
                {
                    if (argResults.size() < 3)
                    {
                        std::cerr << "[خطأ] عين_المحاذاة تتطلب 3 معاملات" << std::endl;
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
                if (funcName == "عين_الحدود" || funcName == "ui_set_border")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] عين_الحدود تتطلب معاملين على الأقل" << std::endl;
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
                if (funcName == "عين_الارتفاع" || funcName == "ui_set_elevation")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] عين_الارتفاع تتطلب معاملين" << std::endl;
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
                if (funcName == "عين_الشفافية" || funcName == "ui_set_opacity")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] عين_الشفافية تتطلب معاملين" << std::endl;
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
                if (funcName == "عين_الظهور" || funcName == "ui_set_visibility")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] عين_الظهور تتطلب معاملين" << std::endl;
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
                if (funcName == "انشئ_تطبيق" || funcName == "ui_app_create")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_CREATE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Pointer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Pointer);
                }

                // ─── عين_الجذر(تطبيق, عنصر) / sad_app_set_root(app, widget) ───
                if (funcName == "عين_الجذر" || funcName == "ui_app_set_root")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] عين_الجذر تتطلب معاملين: التطبيق والعنصر الجذر" << std::endl;
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
                if (funcName == "خطط" || funcName == "ui_app_layout")
                {
                    if (argResults.size() < 3)
                    {
                        std::cerr << "[خطأ] خطط تتطلب 3 معاملات" << std::endl;
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
                if (funcName == "ارسم" || funcName == "ui_app_render")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] ارسم تتطلب معامل واحد: التطبيق" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_RENDER);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── دمر_تطبيق(تطبيق) / sad_app_destroy(app) ───
                if (funcName == "دمر_تطبيق" || funcName == "ui_app_destroy")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دمر_تطبيق تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_UI_APP_DESTROY);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // ─── دمر_عنصر(عنصر) / sad_widget_destroy(widget) ───
                if (funcName == "دمر_عنصر" || funcName == "ui_widget_destroy")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دمر_عنصر تتطلب معامل واحد" << std::endl;
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