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
};

}} // namespace Sad::LLVM
#endif
