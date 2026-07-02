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
            // مصانع إضافيّة (م-مصانع) / Extra factories
            case SIROpcode::BUILTIN_UI_IMAGE:
                return cg_.emitUiImage(inst);
            case SIROpcode::BUILTIN_UI_ICON:
                return cg_.emitUiIcon(inst);
            case SIROpcode::BUILTIN_UI_TEXT_BUTTON:
                return cg_.emitUiTextButton(inst);
            case SIROpcode::BUILTIN_UI_GRID:
                return cg_.emitUiGrid(inst);
            case SIROpcode::BUILTIN_UI_CENTER:
                return cg_.emitUiCenter(inst);
            case SIROpcode::BUILTIN_UI_PADDING:
                return cg_.emitUiPadding(inst);
            case SIROpcode::BUILTIN_UI_ALIGN:
                return cg_.emitUiAlign(inst);
            case SIROpcode::BUILTIN_UI_EXPANDED:
                return cg_.emitUiExpanded(inst);
            case SIROpcode::BUILTIN_UI_FLEXIBLE:
                return cg_.emitUiFlexible(inst);
            case SIROpcode::BUILTIN_UI_SIZED_BOX:
                return cg_.emitUiSizedBox(inst);
            case SIROpcode::BUILTIN_UI_WRAP:
                return cg_.emitUiWrap(inst);
            case SIROpcode::BUILTIN_UI_BOX:
                return cg_.emitUiBox(inst);
            case SIROpcode::BUILTIN_UI_SCROLL_VIEW:
                return cg_.emitUiScrollView(inst);
            case SIROpcode::BUILTIN_UI_BOTTOM_NAV:
                return cg_.emitUiBottomNav(inst);
            case SIROpcode::BUILTIN_UI_SNACKBAR:
                return cg_.emitUiSnackbar(inst);
            case SIROpcode::BUILTIN_UI_TOOLTIP:
                return cg_.emitUiTooltip(inst);
            case SIROpcode::BUILTIN_UI_PROGRESS:
                return cg_.emitUiProgress(inst);
            case SIROpcode::BUILTIN_UI_LAZY_COLUMN:
                return cg_.emitUiLazyColumn(inst);
            case SIROpcode::BUILTIN_UI_LAZY_ROW:
                return cg_.emitUiLazyRow(inst);
            case SIROpcode::BUILTIN_UI_LIST_VIEW:
                return cg_.emitUiListView(inst);
            case SIROpcode::BUILTIN_UI_TEXT_AREA:
                return cg_.emitUiTextArea(inst);
            case SIROpcode::BUILTIN_UI_DRAWER:
                return cg_.emitUiDrawer(inst);
            case SIROpcode::BUILTIN_UI_SAFE_AREA:
                return cg_.emitUiSafeArea(inst);
            case SIROpcode::BUILTIN_UI_SURFACE:
                return cg_.emitUiSurface(inst);
            // أثر المعدّلات (م-أ3ر، L1): خاصّيّة عامّة بالاسم
            case SIROpcode::BUILTIN_UI_SET_PROP_STR:
                return cg_.emitUiSetPropStr(inst);
            case SIROpcode::BUILTIN_UI_SET_PROP_INT:
                return cg_.emitUiSetPropInt(inst);
            case SIROpcode::BUILTIN_UI_SET_PROP_NUM:
                return cg_.emitUiSetPropNum(inst);
            case SIROpcode::BUILTIN_UI_SET_PROP_BOOL:
                return cg_.emitUiSetPropBool(inst);
            // أثر المعدّلات (م-أ3ر، L2): ربط حدث عند_* بالعقدة
            case SIROpcode::BUILTIN_UI_ADD_EVENT:
                return cg_.emitUiAddEvent(inst);
            // أثر المعدّلات (م-أ3ر، L3): سلسلة التحريك
            case SIROpcode::BUILTIN_UI_ANIM_BEGIN:
                return cg_.emitUiAnimBegin(inst);
            case SIROpcode::BUILTIN_UI_ANIM_DURATION:
                return cg_.emitUiAnimDuration(inst);
            case SIROpcode::BUILTIN_UI_ANIM_EASING:
                return cg_.emitUiAnimEasing(inst);
            case SIROpcode::BUILTIN_UI_ANIM_DELAY:
                return cg_.emitUiAnimDelay(inst);
            case SIROpcode::BUILTIN_UI_ANIM_REPEAT:
                return cg_.emitUiAnimRepeat(inst);
            case SIROpcode::BUILTIN_UI_ANIM_AUTO_REVERSE:
                return cg_.emitUiAnimAutoReverse(inst);
            // أثر المعدّلات (م-أ3ر): خاصّيّة متعدّدة الوسائط (دمج بفواصل)
            case SIROpcode::BUILTIN_UI_PROP_JOIN_ADD:
                return cg_.emitUiPropJoinAdd(inst);
            case SIROpcode::BUILTIN_UI_PROP_JOIN_COMMIT:
                return cg_.emitUiPropJoinCommit(inst);
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
            case SIROpcode::BUILTIN_UI_APP_RUN:
                return cg_.emitUiAppRun(inst);
            case SIROpcode::BUILTIN_UI_PRINT_TREE:
                return cg_.emitUiPrintTree(inst);
            case SIROpcode::BUILTIN_UI_APP_DESTROY:
                return cg_.emitUiAppDestroy(inst);
            case SIROpcode::BUILTIN_UI_WIDGET_DESTROY:
                return cg_.emitUiWidgetDestroy(inst);
            // (AR) دوال الثيم — ترتيب الحالات مطابق لترتيب التعداد في sir_types.h
            case SIROpcode::BUILTIN_UI_TOGGLE_THEME:
                return cg_.emitUiToggleTheme(inst);
            case SIROpcode::BUILTIN_UI_DARK_MODE:
                return cg_.emitUiDarkMode(inst);
            case SIROpcode::BUILTIN_UI_LIGHT_MODE:
                return cg_.emitUiLightMode(inst);
            case SIROpcode::BUILTIN_UI_IS_DARK:
                return cg_.emitUiIsDark(inst);
            case SIROpcode::BUILTIN_UI_NAVIGATE:
                return cg_.emitUiNavigate(inst);
            case SIROpcode::BUILTIN_UI_NAV_BACK:
                return cg_.emitUiNavBack(inst);
            case SIROpcode::BUILTIN_UI_NAV_ROOT:
                return cg_.emitUiNavRoot(inst);
            case SIROpcode::BUILTIN_UI_REPLACE_PAGE:
                return cg_.emitUiReplacePage(inst);
            case SIROpcode::BUILTIN_UI_PAGE_COUNT:
                return cg_.emitUiPageCount(inst);
            case SIROpcode::BUILTIN_UI_CURRENT_PAGE:
                return cg_.emitUiCurrentPage(inst);
            case SIROpcode::BUILTIN_UI_NAVIGATE_TRANSITION:
                return cg_.emitUiNavigateTransition(inst);
            case SIROpcode::BUILTIN_UI_BACK_TRANSITION:
                return cg_.emitUiBackTransition(inst);

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
