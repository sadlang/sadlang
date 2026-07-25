/*
 * ============================================================================
 * UICodeGen - LLVM IR for UI widget operations
 * Phase 8 Step 2 - 41 methods (App, Widgets, Layout, Setters)
 * ============================================================================
 */
#ifndef SAD_LLVM_UI_CODEGEN_H
#define SAD_LLVM_UI_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class UICodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit UICodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    UICodeGen(const UICodeGen &) = delete;
    UICodeGen &operator=(const UICodeGen &) = delete;

    llvm::Value *emitUiAddChild(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiAppBar(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiAppCreate(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiAppDestroy(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiAppLayout(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiAppRender(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiAppRun(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiPrintTree(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiToggleTheme(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiDarkMode(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiLightMode(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiIsDark(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiNavigate(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiNavBack(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiNavRoot(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiReplacePage(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiPageCount(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiCurrentPage(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiNavigateTransition(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiBackTransition(std::shared_ptr<SIRInstruction> inst);
    // (إكمال corui) الانتقال الكامل + الحالة + النافذة + توليد الويب
    llvm::Value *emitUiNavigateExitTransition(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiUpdateState(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiStopPropagation(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetState(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetTitle(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiCloseWindow(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiGenWeb(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiAppSetRoot(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiButton(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiButtonVariant(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiCard(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiCheckbox(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiClearChildren(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiColumn(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiContainer(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiDialog(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiDivider(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiFab(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiIconButton(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiRemoveChild(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiRow(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiScaffold(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetAlignment(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetBackground(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetBorder(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetElevation(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetFlex(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetForeground(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetOpacity(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetPadding(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetSize(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetSpacing(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetText(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetVisibility(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSlider(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSpacer(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiStack(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSwitch(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiText(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiTextField(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiTextStyled(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiWidgetDestroy(std::shared_ptr<SIRInstruction> inst);

    // (AR) مصانع إضافيّة (م-مصانع) — مطابقة فجوة المترجم بالمفسّر (24 عنصرًا).
    // (EN) Extra factories (م-مصانع) closing the compiler↔interpreter gap.
    // (AR) مساعد للمصانع البسيطة بلا معاملات (نظير عمود/صف).
    // (EN) Helper for no-arg simple factories (like column/row).
    llvm::Value *emitSimpleUiFactory(std::shared_ptr<SIRInstruction> inst, const char *runtimeName);
    llvm::Value *emitUiImage(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiIcon(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiTextButton(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiGrid(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiCenter(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiPadding(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiAlign(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiExpanded(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiFlexible(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSizedBox(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiWrap(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiBox(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiScrollView(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiBottomNav(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSnackbar(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiTooltip(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiProgress(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiLazyColumn(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiLazyRow(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiListView(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiTextArea(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiDrawer(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSpinBox(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiGroupBox(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSpinner(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiStatusBar(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiWindow(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiTitleBar(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiScrollBar(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiTaskbar(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiStartMenu(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSystemTray(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiMenu(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiToolbar(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSegmentedControl(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiTreeView(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSplitView(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiDataTable(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiBreadcrumb(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiPicker(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiDatePicker(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiStepper(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiRatingBar(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiAlert(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSafeArea(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSurface(std::shared_ptr<SIRInstruction> inst);

    // (AR) أثر المعدّلات (م-أ3ر، L1): خاصّيّة عامّة بالاسم.
    llvm::Value *emitUiSetPropStr(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetPropInt(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetPropNum(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetPropBool(std::shared_ptr<SIRInstruction> inst);
    // (AR) أثر المعدّلات (م-أ3ر، L2): ربط حدث عند_* بالعقدة.
    llvm::Value *emitUiAddEvent(std::shared_ptr<SIRInstruction> inst);
    // (AR) أثر المعدّلات (م-أ3ر، L3): سلسلة التحريك.
    llvm::Value *emitUiAnimBegin(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiAnimDuration(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiAnimEasing(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetEventPhase(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiSetEventData(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiAnimDelay(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiAnimRepeat(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiAnimAutoReverse(std::shared_ptr<SIRInstruction> inst);
    // (AR) أثر المعدّلات (م-أ3ر): خاصّيّة متعدّدة الوسائط (دمج بفواصل).
    llvm::Value *emitUiPropJoinAdd(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitUiPropJoinCommit(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
