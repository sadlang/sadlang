/*
 * (AR) مولد تعليمات LLVM — المنصات: Android / UI / توجيهات (@حجم، @ذري) / وحدات
 * (EN) LLVM instruction emitter — Platform: Android / UI / Directives (@sizeof, @atomic) / Modules
 * مستخرج من: llvm_codegen_instructions.cpp (سطور 1100-1375)
 */

#include "llvm_codegen.h"
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
         * (AR) تنفيذ تعليمات المنصات والتوجيهات (Android، UI، @حجم، @ذري، وحدات)
         * (EN) Emit platform and directive instructions (Android, UI, @sizeof, @atomic, modules)
         * @return nullptr إذا لم يتعرف على الـ opcode / nullptr if opcode not handled
         */
        llvm::Value *LLVMCodeGen::emitInstructionPlatform(std::shared_ptr<SIRInstruction> inst)
        {
            switch (inst->opcode)
            {
                // ====================================================================
                // القسم 19: عمليات أندرويد / Android Operations
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
            // القسم 20: نظام الواجهة الموحد / Unified UI System (Always enabled)
            // ====================================================================
            case SIROpcode::BUILTIN_UI_COLUMN:
                return emitUiColumn(inst);
            case SIROpcode::BUILTIN_UI_ROW:
                return emitUiRow(inst);
            case SIROpcode::BUILTIN_UI_STACK:
                return emitUiStack(inst);
            case SIROpcode::BUILTIN_UI_CONTAINER:
                return emitUiContainer(inst);
            case SIROpcode::BUILTIN_UI_TEXT:
                return emitUiText(inst);
            case SIROpcode::BUILTIN_UI_TEXT_STYLED:
                return emitUiTextStyled(inst);
            case SIROpcode::BUILTIN_UI_BUTTON:
                return emitUiButton(inst);
            case SIROpcode::BUILTIN_UI_BUTTON_VARIANT:
                return emitUiButtonVariant(inst);
            case SIROpcode::BUILTIN_UI_ICON_BUTTON:
                return emitUiIconButton(inst);
            case SIROpcode::BUILTIN_UI_FAB:
                return emitUiFab(inst);
            case SIROpcode::BUILTIN_UI_TEXT_FIELD:
                return emitUiTextField(inst);
            case SIROpcode::BUILTIN_UI_CHECKBOX:
                return emitUiCheckbox(inst);
            case SIROpcode::BUILTIN_UI_SWITCH:
                return emitUiSwitch(inst);
            case SIROpcode::BUILTIN_UI_SLIDER:
                return emitUiSlider(inst);
            case SIROpcode::BUILTIN_UI_CARD:
                return emitUiCard(inst);
            case SIROpcode::BUILTIN_UI_SCAFFOLD:
                return emitUiScaffold(inst);
            case SIROpcode::BUILTIN_UI_APP_BAR:
                return emitUiAppBar(inst);
            case SIROpcode::BUILTIN_UI_SPACER:
                return emitUiSpacer(inst);
            case SIROpcode::BUILTIN_UI_DIVIDER:
                return emitUiDivider(inst);
            case SIROpcode::BUILTIN_UI_DIALOG:
                return emitUiDialog(inst);
            case SIROpcode::BUILTIN_UI_ADD_CHILD:
                return emitUiAddChild(inst);
            case SIROpcode::BUILTIN_UI_REMOVE_CHILD:
                return emitUiRemoveChild(inst);
            case SIROpcode::BUILTIN_UI_CLEAR_CHILDREN:
                return emitUiClearChildren(inst);
            case SIROpcode::BUILTIN_UI_SET_TEXT:
                return emitUiSetText(inst);
            case SIROpcode::BUILTIN_UI_SET_SIZE:
                return emitUiSetSize(inst);
            case SIROpcode::BUILTIN_UI_SET_FLEX:
                return emitUiSetFlex(inst);
            case SIROpcode::BUILTIN_UI_SET_BACKGROUND:
                return emitUiSetBackground(inst);
            case SIROpcode::BUILTIN_UI_SET_FOREGROUND:
                return emitUiSetForeground(inst);
            case SIROpcode::BUILTIN_UI_SET_SPACING:
                return emitUiSetSpacing(inst);
            case SIROpcode::BUILTIN_UI_SET_PADDING:
                return emitUiSetPadding(inst);
            case SIROpcode::BUILTIN_UI_SET_ALIGNMENT:
                return emitUiSetAlignment(inst);
            case SIROpcode::BUILTIN_UI_SET_BORDER:
                return emitUiSetBorder(inst);
            case SIROpcode::BUILTIN_UI_SET_ELEVATION:
                return emitUiSetElevation(inst);
            case SIROpcode::BUILTIN_UI_SET_OPACITY:
                return emitUiSetOpacity(inst);
            case SIROpcode::BUILTIN_UI_SET_VISIBILITY:
                return emitUiSetVisibility(inst);
            case SIROpcode::BUILTIN_UI_APP_CREATE:
                return emitUiAppCreate(inst);
            case SIROpcode::BUILTIN_UI_APP_SET_ROOT:
                return emitUiAppSetRoot(inst);
            case SIROpcode::BUILTIN_UI_APP_LAYOUT:
                return emitUiAppLayout(inst);
            case SIROpcode::BUILTIN_UI_APP_RENDER:
                return emitUiAppRender(inst);
            case SIROpcode::BUILTIN_UI_APP_DESTROY:
                return emitUiAppDestroy(inst);
            case SIROpcode::BUILTIN_UI_WIDGET_DESTROY:
                return emitUiWidgetDestroy(inst);

            // ====================================================================
            // القسم 21: التوجيهات / Directives (@حجم, @ذري)
            // ====================================================================
            case SIROpcode::Sizeof:
                return emitSizeof(inst);
            case SIROpcode::AtomicLoad:
                return emitAtomicLoad(inst);
            case SIROpcode::AtomicStore:
                return emitAtomicStore(inst);
            case SIROpcode::AtomicAdd:
                return emitAtomicAdd(inst);
            case SIROpcode::AtomicSub:
                return emitAtomicSub(inst);
            case SIROpcode::AtomicExchange:
                return emitAtomicExchange(inst);
            case SIROpcode::AtomicCmpXchg:
                return emitAtomicCmpXchg(inst);

            // ====================================================================
            // القسم 22: نظام الوحدات / Module System
            // ====================================================================
            case SIROpcode::MODULE_LOAD:
            {
                // (AR) تحميل وحدة — يُنشئ تعريفاً خارجياً لدالة تهيئة الوحدة
                // (EN) Module load — creates extern declaration for module init function
                if (!inst->operands.empty())
                {
                    std::string moduleName = inst->operands[0].name;
                    // تحويل اسم الوحدة لاسم دالة تهيئة صالح
                    std::string initFnName = "__sad_module_init_" + moduleName;
                    // تعريف خارجي لدالة التهيئة
                    auto *fnType = llvm::FunctionType::get(
                        llvm::Type::getVoidTy(*context_), false);
                    module_->getOrInsertFunction(initFnName, fnType);
                }
                return nullptr;
            }
            case SIROpcode::MODULE_INIT:
            {
                // (AR) تهيئة وحدة — يستدعي دالة تهيئة الوحدة المستوردة
                // (EN) Module init — calls the imported module's init function
                if (!inst->operands.empty())
                {
                    std::string moduleName = inst->operands[0].name;
                    std::string initFnName = "__sad_module_init_" + moduleName;
                    auto *fn = module_->getFunction(initFnName);
                    if (fn)
                    {
                        builder_->CreateCall(fn);
                    }
                }
                return nullptr;
            }
            case SIROpcode::MODULE_SYMBOL:
            {
                // (AR) رمز وحدة — يُعلن عن رمز خارجي مستورد من وحدة أخرى
                // (EN) Module symbol — declares external symbol imported from another module
                if (inst->operands.size() >= 2)
                {
                    std::string symbolName = inst->operands[0].name;
                    // تعريف خارجي للرمز — سيحلّه الرابط لاحقاً
                    auto *fnType = llvm::FunctionType::get(
                        llvm::Type::getInt64Ty(*context_),
                        {}, true); // variadic للمرونة
                    module_->getOrInsertFunction(symbolName, fnType);
                }
                return nullptr;
            }
            case SIROpcode::MODULE_EXPORT:
            {
                // (AR) تصدير وحدة — يضع ربطاً خارجياً على الدالة المصدّرة
                // (EN) Module export — sets external linkage on exported function
                if (!inst->operands.empty())
                {
                    std::string symbolName = inst->operands[0].name;
                    auto *fn = module_->getFunction(symbolName);
                    if (fn)
                    {
                        fn->setLinkage(llvm::GlobalValue::ExternalLinkage);
                    }
                }
                return nullptr;
            }

            default:
                return nullptr; // (AR) غير مدعوم هنا / (EN) not handled here
            }
        }

    } // namespace LLVM
} // namespace Sad
