/*
 * (AR) …ˆ„״¯ ״×״¹„…״§״× LLVM ג€” ״§„…†״µ״§״×: Android / UI / ״×ˆ״¬‡״§״× (@״­״¬…״ @״°״±) / ˆ״­״¯״§״×
 * (EN) LLVM instruction emitter ג€” Platform: Android / UI / Directives (@sizeof, @atomic) / Modules
 * …״³״×״®״±״¬ …†: llvm_codegen_instructions.cpp (״³״·ˆ״± 1100-1375)
 */

#include "llvm_codegen.h"
#include "builders/platform/instr_platform_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InlineAsm.h>
#include <iostream>
#include <fstream>

using namespace Sad::Compiler::SIR;

namespace Sad
{
    namespace LLVM
    {

        /**
         * (AR) ״×†״° ״×״¹„…״§״× ״§„…†״µ״§״× ˆ״§„״×ˆ״¬‡״§״× (Android״ UI״ @״­״¬…״ @״°״±״ ˆ״­״¯״§״×)
         * (EN) Emit platform and directive instructions (Android, UI, @sizeof, @atomic, modules)
         * @return nullptr ״¥״°״§ „… ״×״¹״± ״¹„‰ ״§„€ opcode / nullptr if opcode not handled
         */
        llvm::Value *InstrPlatformCodeGen::emitInstructionPlatform(std::shared_ptr<SIRInstruction> inst)
        {
            switch (inst->opcode)
            {
                // ====================================================================
                // ״§„‚״³… 19: ״¹…„״§״× ״£†״¯״±ˆ״¯ / Android Operations
                // ====================================================================
#ifdef ENABLE_ANDROID_CODEGEN
            case SIROpcode::ANDROID_ALLOC:
                return emitAndroidAlloc(inst);
            case SIROpcode::ANDROID_FREE:
                return emitAndroidFree(inst);
            case SIROpcode::ANDROID_STRING_CREATE:
                return emitAndroidStringCreate(inst);
            case SIROpcode::ANDROID_STRING_CONCAT:
                return emitAndroidStringConcat(inst);
            case SIROpcode::ANDROID_STRING_LENGTH:
                return emitAndroidStringLength(inst);
            case SIROpcode::ANDROID_STRING_SUBSTR:
                return emitAndroidStringSubstr(inst);
            case SIROpcode::ANDROID_STRING_COMPARE:
                return emitAndroidStringCompare(inst);
            case SIROpcode::ANDROID_STRING_FREE:
                return emitAndroidStringFree(inst);
            case SIROpcode::ANDROID_ARRAY_CREATE:
                return emitAndroidArrayCreate(inst);
            case SIROpcode::ANDROID_ARRAY_GET:
                return emitAndroidArrayGet(inst);
            case SIROpcode::ANDROID_ARRAY_SET:
                return emitAndroidArraySet(inst);
            case SIROpcode::ANDROID_ARRAY_LENGTH:
                return emitAndroidArrayLength(inst);
            case SIROpcode::ANDROID_ARRAY_PUSH:
                return emitAndroidArrayPush(inst);
            case SIROpcode::ANDROID_ARRAY_POP:
                return emitAndroidArrayPop(inst);
            case SIROpcode::ANDROID_ARRAY_FREE:
                return emitAndroidArrayFree(inst);
            case SIROpcode::ANDROID_MAP_CREATE:
                return emitAndroidMapCreate(inst);
            case SIROpcode::ANDROID_MAP_GET:
                return emitAndroidMapGet(inst);
            case SIROpcode::ANDROID_MAP_SET:
                return emitAndroidMapSet(inst);
            case SIROpcode::ANDROID_MAP_HAS:
                return emitAndroidMapHas(inst);
            case SIROpcode::ANDROID_MAP_DELETE:
                return emitAndroidMapDelete(inst);
            case SIROpcode::ANDROID_MAP_SIZE:
                return emitAndroidMapSize(inst);
            case SIROpcode::ANDROID_MAP_FREE:
                return emitAndroidMapFree(inst);
            case SIROpcode::ANDROID_NET_CONNECT:
                return emitAndroidNetConnect(inst);
            case SIROpcode::ANDROID_NET_SEND:
                return emitAndroidNetSend(inst);
            case SIROpcode::ANDROID_NET_RECV:
                return emitAndroidNetRecv(inst);
            case SIROpcode::ANDROID_NET_CLOSE:
                return emitAndroidNetClose(inst);
            case SIROpcode::ANDROID_HTTP_REQUEST:
                return emitAndroidHttpRequest(inst);
            case SIROpcode::ANDROID_WS_CONNECT:
                return emitAndroidWsConnect(inst);
            case SIROpcode::ANDROID_WS_SEND:
                return emitAndroidWsSend(inst);
            case SIROpcode::ANDROID_WS_RECV:
                return emitAndroidWsRecv(inst);
            case SIROpcode::ANDROID_THREAD_CREATE:
                return emitAndroidThreadCreate(inst);
            case SIROpcode::ANDROID_THREAD_JOIN:
                return emitAndroidThreadJoin(inst);
            case SIROpcode::ANDROID_MUTEX_CREATE:
                return emitAndroidMutexCreate(inst);
            case SIROpcode::ANDROID_MUTEX_LOCK:
                return emitAndroidMutexLock(inst);
            case SIROpcode::ANDROID_MUTEX_UNLOCK:
                return emitAndroidMutexUnlock(inst);
            case SIROpcode::ANDROID_CHANNEL_CREATE:
                return emitAndroidChannelCreate(inst);
            case SIROpcode::ANDROID_CHANNEL_SEND:
                return emitAndroidChannelSend(inst);
            case SIROpcode::ANDROID_CHANNEL_RECV:
                return emitAndroidChannelRecv(inst);
            case SIROpcode::ANDROID_UI_INIT:
                return emitAndroidUiInit(inst);
            case SIROpcode::ANDROID_UI_CREATE_WIDGET:
                return emitAndroidUiCreateWidget(inst);
            case SIROpcode::ANDROID_UI_SET_TEXT:
                return emitAndroidUiSetText(inst);
            case SIROpcode::ANDROID_UI_SET_CALLBACK:
                return emitAndroidUiSetCallback(inst);
            case SIROpcode::ANDROID_UI_SHOW:
                return emitAndroidUiShow(inst);
            case SIROpcode::ANDROID_UI_HIDE:
                return emitAndroidUiHide(inst);
            case SIROpcode::ANDROID_UI_UPDATE:
                return emitAndroidUiUpdate(inst);
            case SIROpcode::ANDROID_UI_RUN:
                return emitAndroidUiRun(inst);
            case SIROpcode::ANDROID_LOG:
                return emitAndroidLog(inst);
            case SIROpcode::ANDROID_PRINT:
                return emitAndroidPrint(inst);
#endif // ENABLE_ANDROID_CODEGEN

            // ====================================================================
            // ״§„‚״³… 20: †״¸״§… ״§„ˆ״§״¬‡״© ״§„…ˆ״­״¯ / Unified UI System (Always enabled)
            // ====================================================================
            case SIROpcode::BUILTIN_UI_COLUMN:
                return cg_.emitUiColumn(inst);
            case SIROpcode::BUILTIN_UI_ROW:
                return cg_.emitUiRow(inst);
            case SIROpcode::BUILTIN_UI_STACK:
                return cg_.emitUiStack(inst);
            case SIROpcode::BUILTIN_UI_CONTAINER:
                return cg_.emitUiContainer(inst);
            case SIROpcode::BUILTIN_UI_TEXT:
                return cg_.emitUiText(inst);
            case SIROpcode::BUILTIN_UI_TEXT_STYLED:
                return cg_.emitUiTextStyled(inst);
            case SIROpcode::BUILTIN_UI_BUTTON:
                return cg_.emitUiButton(inst);
            case SIROpcode::BUILTIN_UI_BUTTON_VARIANT:
                return cg_.emitUiButtonVariant(inst);
            case SIROpcode::BUILTIN_UI_ICON_BUTTON:
                return cg_.emitUiIconButton(inst);
            case SIROpcode::BUILTIN_UI_FAB:
                return cg_.emitUiFab(inst);
            case SIROpcode::BUILTIN_UI_TEXT_FIELD:
                return cg_.emitUiTextField(inst);
            case SIROpcode::BUILTIN_UI_CHECKBOX:
                return cg_.emitUiCheckbox(inst);
            case SIROpcode::BUILTIN_UI_SWITCH:
                return cg_.emitUiSwitch(inst);
            case SIROpcode::BUILTIN_UI_SLIDER:
                return cg_.emitUiSlider(inst);
            case SIROpcode::BUILTIN_UI_CARD:
                return cg_.emitUiCard(inst);
            case SIROpcode::BUILTIN_UI_SCAFFOLD:
                return cg_.emitUiScaffold(inst);
            case SIROpcode::BUILTIN_UI_APP_BAR:
                return cg_.emitUiAppBar(inst);
            case SIROpcode::BUILTIN_UI_SPACER:
                return cg_.emitUiSpacer(inst);
            case SIROpcode::BUILTIN_UI_DIVIDER:
                return cg_.emitUiDivider(inst);
            case SIROpcode::BUILTIN_UI_DIALOG:
                return cg_.emitUiDialog(inst);
            case SIROpcode::BUILTIN_UI_ADD_CHILD:
                return cg_.emitUiAddChild(inst);
            case SIROpcode::BUILTIN_UI_REMOVE_CHILD:
                return cg_.emitUiRemoveChild(inst);
            case SIROpcode::BUILTIN_UI_CLEAR_CHILDREN:
                return cg_.emitUiClearChildren(inst);
            case SIROpcode::BUILTIN_UI_SET_TEXT:
                return cg_.emitUiSetText(inst);
            case SIROpcode::BUILTIN_UI_SET_SIZE:
                return cg_.emitUiSetSize(inst);
            case SIROpcode::BUILTIN_UI_SET_FLEX:
                return cg_.emitUiSetFlex(inst);
            case SIROpcode::BUILTIN_UI_SET_BACKGROUND:
                return cg_.emitUiSetBackground(inst);
            case SIROpcode::BUILTIN_UI_SET_FOREGROUND:
                return cg_.emitUiSetForeground(inst);
            case SIROpcode::BUILTIN_UI_SET_SPACING:
                return cg_.emitUiSetSpacing(inst);
            case SIROpcode::BUILTIN_UI_SET_PADDING:
                return cg_.emitUiSetPadding(inst);
            case SIROpcode::BUILTIN_UI_SET_ALIGNMENT:
                return cg_.emitUiSetAlignment(inst);
            case SIROpcode::BUILTIN_UI_SET_BORDER:
                return cg_.emitUiSetBorder(inst);
            case SIROpcode::BUILTIN_UI_SET_ELEVATION:
                return cg_.emitUiSetElevation(inst);
            case SIROpcode::BUILTIN_UI_SET_OPACITY:
                return cg_.emitUiSetOpacity(inst);
            case SIROpcode::BUILTIN_UI_SET_VISIBILITY:
                return cg_.emitUiSetVisibility(inst);
            case SIROpcode::BUILTIN_UI_APP_CREATE:
                return cg_.emitUiAppCreate(inst);
            case SIROpcode::BUILTIN_UI_APP_SET_ROOT:
                return cg_.emitUiAppSetRoot(inst);
            case SIROpcode::BUILTIN_UI_APP_LAYOUT:
                return cg_.emitUiAppLayout(inst);
            case SIROpcode::BUILTIN_UI_APP_RENDER:
                return cg_.emitUiAppRender(inst);
            case SIROpcode::BUILTIN_UI_APP_DESTROY:
                return cg_.emitUiAppDestroy(inst);
            case SIROpcode::BUILTIN_UI_WIDGET_DESTROY:
                return cg_.emitUiWidgetDestroy(inst);

            // ====================================================================
            // ״§„‚״³… 21: ״§„״×ˆ״¬‡״§״× / Directives (@״­״¬…, @״°״±)
            // ====================================================================
            case SIROpcode::Sizeof:
                return cg_.emitSizeof(inst);
            case SIROpcode::AtomicLoad:
                return cg_.emitAtomicLoad(inst);
            case SIROpcode::AtomicStore:
                return cg_.emitAtomicStore(inst);
            case SIROpcode::AtomicAdd:
                return cg_.emitAtomicAdd(inst);
            case SIROpcode::AtomicSub:
                return cg_.emitAtomicSub(inst);
            case SIROpcode::AtomicExchange:
                return cg_.emitAtomicExchange(inst);
            case SIROpcode::AtomicCmpXchg:
                return cg_.emitAtomicCmpXchg(inst);

            // ====================================================================
            // ״§„‚״³… 22: †״¸״§… ״§„ˆ״­״¯״§״× / Module System
            // ====================================================================
            case SIROpcode::MODULE_LOAD:
            {
                // (AR) ״×״­…„ ˆ״­״¯״© ג€” †״´״¦ ״×״¹״±״§‹ ״®״§״±״¬״§‹ „״¯״§„״© ״×‡״¦״© ״§„ˆ״­״¯״©
                // (EN) Module load ג€” creates extern declaration for module init function
                if (!inst->operands.empty())
                {
                    std::string moduleName = inst->operands[0].name;
                    // ״×״­ˆ„ ״§״³… ״§„ˆ״­״¯״© „״§״³… ״¯״§„״© ״×‡״¦״© ״µ״§„״­
                    std::string initFnName = "__sad_module_init_" + moduleName;
                    // ״×״¹״± ״®״§״±״¬ „״¯״§„״© ״§„״×‡״¦״©
                    auto *fnType = llvm::FunctionType::get(
                        llvm::Type::getVoidTy(*cg_.context_), false);
                    cg_.module_->getOrInsertFunction(initFnName, fnType);
                }
                return nullptr;
            }
            case SIROpcode::MODULE_INIT:
            {
                // (AR) ״×‡״¦״© ˆ״­״¯״© ג€” ״³״×״¯״¹ ״¯״§„״© ״×‡״¦״© ״§„ˆ״­״¯״© ״§„…״³״×ˆ״±״¯״©
                // (EN) Module init ג€” calls the imported module's init function
                if (!inst->operands.empty())
                {
                    std::string moduleName = inst->operands[0].name;
                    std::string initFnName = "__sad_module_init_" + moduleName;
                    auto *fn = cg_.module_->getFunction(initFnName);
                    if (fn)
                    {
                        cg_.builder_->CreateCall(fn);
                    }
                }
                return nullptr;
            }
            case SIROpcode::MODULE_SYMBOL:
            {
                // (AR) ״±…״² ˆ״­״¯״© ג€” ״¹„† ״¹† ״±…״² ״®״§״±״¬ …״³״×ˆ״±״¯ …† ˆ״­״¯״© ״£״®״±‰
                // (EN) Module symbol ג€” declares external symbol imported from another module
                if (inst->operands.size() >= 2)
                {
                    std::string symbolName = inst->operands[0].name;
                    // ״×״¹״± ״®״§״±״¬ „„״±…״² ג€” ״³״­„‘‡ ״§„״±״§״¨״· „״§״­‚״§‹
                    auto *fnType = llvm::FunctionType::get(
                        llvm::Type::getInt64Ty(*cg_.context_),
                        {}, true); // variadic „„…״±ˆ†״©
                    cg_.module_->getOrInsertFunction(symbolName, fnType);
                }
                return nullptr;
            }
            case SIROpcode::MODULE_EXPORT:
            {
                // (AR) ״×״µ״¯״± ˆ״­״¯״© ג€” ״¶״¹ ״±״¨״·״§‹ ״®״§״±״¬״§‹ ״¹„‰ ״§„״¯״§„״© ״§„…״µ״¯‘״±״©
                // (EN) Module export ג€” sets external linkage on exported function
                if (!inst->operands.empty())
                {
                    std::string symbolName = inst->operands[0].name;
                    auto *fn = cg_.module_->getFunction(symbolName);
                    if (fn)
                    {
                        fn->setLinkage(llvm::GlobalValue::ExternalLinkage);
                    }
                }
                return nullptr;
            }

            default:
                return nullptr; // (AR) ״÷״± …״¯״¹ˆ… ‡†״§ / (EN) not handled here
            }
        }

    } // namespace LLVM
} // namespace Sad
