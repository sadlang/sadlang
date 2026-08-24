// ============================================================================
// sir_builder_builtins_os_core.cpp
// (AR) وحدات نظام التشغيل الأساسية: المعالج، جداول الواصفات، الترحيل، المقاطعات
//      هذا الملف انفصل عن sir_builder_builtins_async.cpp لتقليل حجمه
// (EN) Core OS Modules: CPU, GDT, Paging, IDT
//      Split from sir_builder_builtins_async.cpp
// ============================================================================

#include "sir_builder.h"
#include "builders/builtin_builder.h"
#include "sir_builder.h"
#include <iostream>

#include "builtin_registry.h"
#include "builtin_arity.h"
#include "error_manager.h" // (AR) buildBilingualMessage من كتالوج الأخطاء (مصدر الحقيقة)
#include "error_catalog.h" // (AR) RenderContext (حاملُ placeholders)
#include "error_codes.h"   // (AR) ErrorCode::SEM_WRONG_ARG_COUNT
namespace Bn = Sad::Builtins::Names;
// (AR) رتبُ المدمجاتِ مُولَّدةٌ من مصدرِ الحقيقة — لا أرقامَ حرفيّةً هنا.
// (EN) Builtin arities are generated from SoT — no literals here.
namespace Ar = Sad::Builtins::Arity;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // (AR) حارسُ رتبةِ مدمجاتِ النواة (سدُّ ح٤): قبل هذا الحارس كان الوسيطُ
            //      الناقصُ يُعيد BuildResult فارغًا فيتبخّرُ النداءُ صامتًا (66 نداءً
            //      بدل 67 والخروجُ صفر)، والزائدُ يُقبَل ويُهمَل — أخطرُ صنفِ عيوبٍ في
            //      كودِ نواةٍ: سطرُ أمانٍ «يُنفَّذ» وهو غيرُ موجود. الرسالةُ من الكتالوج
            //      (SEM005 — لا نصوصَ يدويّة) وتُدفَع إلى errors_ فيُفشِل hasErrors()
            //      البناءَ (نمطُ حرف_من_رمز في builtins_strings_arrays). المدى
            //      [min, max] المُعلَنُ في `arity` بمصدرِ الحقيقة لأنّ بعضَ المدمجاتِ بوسيطٍ اختياريٍّ معلَن
            //      (حمل_جدول_واصفات/مقاطعات: واصفٌ يلزم حرًّا ويُهمَل مستضافًا؛
            //      رحل_صفحة: أعلامٌ اختياريّة) — {expected} يُظهر الحدَّ المخروق.
            // (EN) OS-core builtin arity guard (gap ح٤): under-arity used to vanish
            //      silently, over-arity was accepted and ignored. Message from the
            //      error catalog (SEM005), pushed to errors_ so the build fails.
            //      (errors_ خاصّةٌ في SIRBuilder وBuiltinBuilder صديقتُها؛ الدالّةُ
            //      الحرّةُ هنا تتلقّى المرجعَ من موضعِ النداءِ العضويِّ الصديق.)
            [[nodiscard]] static bool checkOsCoreArity(std::vector<std::string> &errors,
                                         const std::string &name,
                                         const Ar::Range &range, size_t found)
            {
                if (found >= range.min && found <= range.max)
                    return true;
                Sad::Errors::RenderContext ectx;
                ectx.placeholders = {
                    {"name", name},
                    {"expected", std::to_string(found < range.min ? range.min : range.max)},
                    {"found", std::to_string(found)}};
                errors.push_back(
                    Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                        Sad::Errors::ErrorCode::SEM_WRONG_ARG_COUNT, ectx));
                return false;
            }

            std::optional<BuildResult> BuiltinBuilder::buildBuiltinSystem_OsCore(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {

                // ─── 15a. وحدة المعالج المتقدمة / Advanced CPU Module ───
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_0)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_GET_INFO);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_1)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_1, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_GET_FEATURES);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_2)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_2, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_READ_MSR);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_3)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_3, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_WRITE_MSR);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_4)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_4, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_READ_CR);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_5)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_5, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_WRITE_CR);
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::KernelCpu::CPU_7)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::KernelCpu::CPU_7, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_INVLPG);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_6)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_6, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }
                // ─── عنوان_رمز: عنوان رمز رابط خارجيّ ⇒ رقم ───
                // (AR) الوسيط سلسلة حرفيّة ثابتة (اسم الرمز)؛ يصل كـConstantString
                //      في operands[0].name. الخفض (backend) يُصدر ptrtoint على رمز
                //      خارجيّ. الوسيط غير الثابت يُرفَض في الخفض (خطأ SEM).
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_20)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_20, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SYMBOL_ADDR);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                // ─── 15b. وحدة GDT ───
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_7)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_7, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_GDT_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_8)
                {
                    // (AR) وسيط اختياريّ: مؤشّر واصف الجدول (limit+base). في الوضع
                    //      الحرّ يلزم لإصدار lgdt؛ المستضاف يتجاهله (sad_ll_gdt_load).
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_8, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_GDT_LOAD);
                    if (!argResults.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                // (AR) [RFC 0059] حمل_سجل_المهمة (ltr): منتقي واصفِ TSS إلزاميّ —
                //      نظيرُ lgdt/lidt لكن معاملُه منتقٍ 16-بتّيّ لا مؤشّرُ واصف.
                //      بوّابةُ مكدّساتِ IST التي تعتمدُها معالجاتُ المقاطعةِ الجادّة.
                // (EN) [RFC 0059] load-task-register (ltr): a mandatory 16-bit TSS
                //      selector — the gate for the IST stacks interrupt handlers need.
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_21)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_21, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    // (AR) بوّابةُ الوضع: `ltr` تعليمةُ حلقةٍ صفريّةٍ تُصدرها الخلفيّةُ
                    //      أسمبليًّا خامًّا بلا ذراعٍ مستضافةٍ (خلافَ lgdt/lidt اللتين
                    //      تنادِيان رمزَ وقتِ تشغيلٍ مستضافًا). فإصدارُها في وحدةِ
                    //      حلقةٍ ثالثةٍ يعني ‎#GP‎ عند التنفيذ — رفضٌ زمنَ الترجمةِ
                    //      أصدقُ من ثنائيٍّ ينهارُ عند أوّلِ نداء.
                    // (EN) Mode gate: ltr is a ring-0 instruction emitted as raw inline
                    //      asm with no hosted arm — emitting it into a ring-3 module
                    //      means #GP at run time, so reject at compile time.
                    if (!b_.isFreestandingMode())
                    {
                        Sad::Errors::RenderContext ltrCtx;
                        ltrCtx.placeholders = {
                            {"detail",
                             "«" + funcName + "»: تعليمةُ حلقةٍ صفريّة (ltr) لا تُصدَر في "
                             "وحدةٍ مستضافةٍ فوقَ نظامِ تشغيل — صرّف بالوضعِ الحرّ"}};
                        b_.errors_.push_back(
                            Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                                Sad::Errors::ErrorCode::SEM_INTERRUPT_HANDLER_CONTRACT, ltrCtx));
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::LOWLEVEL_TASK_REGISTER_LOAD);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_9)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_9, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_GDT_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15c. وحدة الترحيل / Paging ───
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_10)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_10, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_11)
                {
                    // (AR) الوسيط الثالث (الأعلام) اختياريٌّ معلَن ⇒ المدى [2, 3].
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_11, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_MAP);
                    inst.operands.push_back(argOperands[0]); // virtual address
                    inst.operands.push_back(argOperands[1]); // physical address
                    if (argResults.size() > 2)
                        inst.operands.push_back(argOperands[2]); // flags
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_12)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_12, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_UNMAP);
                    inst.operands.push_back(argOperands[0]); // virtual address
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_13)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_13, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_FLUSH_TLB);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_14)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_14, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15d. وحدة المقاطعات المتقدمة / Advanced Interrupts (IDT) ───
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_15)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_15, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_IDT_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_16)
                {
                    // (AR) وسيط اختياريّ: مؤشّر واصف الجدول (limit+base). في الوضع
                    //      الحرّ يلزم لإصدار lidt؛ المستضاف يتجاهله (sad_ll_idt_load).
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_16, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_IDT_LOAD);
                    if (!argResults.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_17)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_17, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_IDT_REGISTER_ISR);
                    inst.operands.push_back(argOperands[0]); // ISR number
                    inst.operands.push_back(argOperands[1]); // handler function ptr
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_18)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_18, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_IDT_ENABLE_IRQ);
                    inst.operands.push_back(argOperands[0]); // IRQ number
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_19)
                {
                    if (!checkOsCoreArity(b_.errors_, funcName, Ar::CompilerCpuCtl::CPUCTL_19, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_IDT_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
