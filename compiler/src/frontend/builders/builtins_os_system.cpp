// ============================================================================
// sir_builder_builtins_os_system.cpp
// (AR) وحدات النظام: المجدول، الإقلاع، نظام الملفات، APIC، HPET، استدعاءات النظام، الذاكرة المتقدمة
//      هذا الملف انفصل عن sir_builder_builtins_async.cpp لتقليل حجمه
// (EN) System Modules: Scheduler, Boot, VFS, APIC, HPET, Syscall, Advanced Memory
//      Split from sir_builder_builtins_async.cpp
// ============================================================================

#include "sir_builder.h"
#include "builders/builtin_builder.h"
#include "builders/builtin_arity_check.h"
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

            // (AR) حارسُ رتبةِ مدمجاتِ النظام (سدُّ ح٤ جزء ب): قبل هذا الحارس كان الوسيطُ
            //      الناقصُ يُعيد BuildResult فارغًا فيتبخّرُ النداءُ صامتًا،
            //      والزائدُ يُقبَل ويُهمَل — أخطرُ صنفِ عيوبٍ في
            //      كودِ نواةٍ: سطرُ أمانٍ «يُنفَّذ» وهو غيرُ موجود. الرسالةُ من الكتالوج
            //      (SEM005 — لا نصوصَ يدويّة) وتُدفَع إلى errors_ فيُفشِل hasErrors()
            //      البناءَ (نمطُ حرف_من_رمز في builtins_strings_arrays). المدى
            //      [min, max] المُعلَنُ في `arity` بمصدرِ الحقيقة لأنّ بعضَ المدمجاتِ بوسيطٍ اختياريٍّ معلَن
            //      (انشئ_عملية: الأولويّة؛ حمل_قرص: نوع النظام؛ افتح_ملف_نواة:
            //      الأعلام؛ رحل_منطقة: الأعلام) — {expected} يُظهر الحدَّ المخروق.
            // (EN) OS-system builtin arity guard (gap ح٤ part B): under-arity used to
            //      vanish silently, over-arity was accepted and ignored. Message from the
            //      error catalog (SEM005), pushed to errors_ so the build fails.
            //      (errors_ خاصّةٌ في SIRBuilder وBuiltinBuilder صديقتُها؛ الدالّةُ
            //      الحرّةُ هنا تتلقّى المرجعَ من موضعِ النداءِ العضويِّ الصديق.)

            std::optional<BuildResult> BuiltinBuilder::buildBuiltinSystem_OsSystem(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {

                // ─── 15j. وحدة المجدول / Scheduler ───
                if (funcName == Bn::CompilerSys::SYS_0)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_0, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_1)
                {
                    // (AR) الوسيط الثاني (الأولويّة) اختياريٌّ معلَن ⇒ المدى [1, 2].
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_1, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_CREATE_PROC);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // name
                    if (argResults.size() > 1)
                        inst.operands.push_back(argOperands[1]); // priority
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer); // PID
                }
                if (funcName == Bn::CompilerSys::SYS_2)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_2, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_CREATE_THREAD);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // name
                    inst.operands.push_back(argOperands[1]); // entry function
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer); // TID
                }
                if (funcName == Bn::CompilerSys::SYS_3)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_3, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_YIELD);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_4)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_4, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_SLEEP);
                    inst.operands.push_back(argOperands[0]); // milliseconds
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_5)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_5, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15k. وحدة الإقلاع / Boot ───
                if (funcName == Bn::CompilerSys::SYS_6)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_6, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_BOOT_GET_INFO);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerSys::SYS_7)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_7, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_BOOT_GET_MEMORY_MAP);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerSys::SYS_8)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_8, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_BOOT_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15l. وحدة نظام الملفات الافتراضي / VFS ───
                if (funcName == Bn::CompilerSys::SYS_9)
                {
                    // (AR) الوسيط الثالث (نوع نظام الملفّات) اختياريٌّ معلَن ⇒ المدى [2, 3].
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_9, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_MOUNT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // device
                    inst.operands.push_back(argOperands[1]); // mount point
                    if (argResults.size() > 2)
                        inst.operands.push_back(argOperands[2]); // fs type
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerSys::SYS_10)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_10, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_UNMOUNT);
                    inst.operands.push_back(argOperands[0]); // mount point
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_11)
                {
                    // (AR) الوسيط الثاني (الأعلام) اختياريٌّ معلَن ⇒ المدى [1, 2].
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_11, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_OPEN);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // path
                    if (argResults.size() > 1)
                        inst.operands.push_back(argOperands[1]); // flags
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer); // fd
                }
                if (funcName == Bn::CompilerSys::SYS_12)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_12, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_READ);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // fd
                    inst.operands.push_back(argOperands[1]); // buffer/size
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer); // bytes read
                }
                if (funcName == Bn::CompilerSys::SYS_13)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_13, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_WRITE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // fd
                    inst.operands.push_back(argOperands[1]); // data
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer); // bytes written
                }
                if (funcName == Bn::CompilerSys::SYS_14)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_14, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_CLOSE);
                    inst.operands.push_back(argOperands[0]); // fd
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_15)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_15, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15m. وحدة APIC ───
                if (funcName == Bn::CompilerSys::SYS_16)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_16, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_17)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_17, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_EOI);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_18)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_18, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_IPI);
                    inst.operands.push_back(argOperands[0]); // target CPU
                    inst.operands.push_back(argOperands[1]); // vector
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_19)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_19, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SET_TIMER);
                    inst.operands.push_back(argOperands[0]); // divisor
                    inst.operands.push_back(argOperands[1]); // count
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_20)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_20, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15n. وحدة HPET ───
                if (funcName == Bn::CompilerSys::SYS_21)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_21, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_HPET_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_22)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_22, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_HPET_READ);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerSys::SYS_23)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_23, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_HPET_SLEEP);
                    inst.operands.push_back(argOperands[0]); // nanoseconds
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_24)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_24, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_HPET_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15o. وحدة استدعاءات النظام / Syscall ───
                if (funcName == Bn::CompilerSys::SYS_25)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_25, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SYSCALL_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_26)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_26, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SYSCALL_REGISTER);
                    inst.operands.push_back(argOperands[0]); // syscall number
                    inst.operands.push_back(argOperands[1]); // handler function
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_27)
                {
                    // (AR) رقم الاستدعاء يلزم؛ ويليه حتّى ستّةُ معاملاتٍ اختياريّةٍ
                    //      (عرف استدعاءات النظام x86-64) ⇒ المدى [1, 7].
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_27, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SYSCALL_INVOKE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerSys::SYS_28)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_28, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SYSCALL_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15p. عمليات الذاكرة المتقدمة / Advanced Memory ───
                if (funcName == Bn::CompilerSys::SYS_29)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_29, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MEM_ALLOC_PHYS);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // size
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer); // physical address
                }
                if (funcName == Bn::CompilerSys::SYS_30)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_30, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MEM_FREE_PHYS);
                    inst.operands.push_back(argOperands[0]); // physical address
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_31)
                {
                    // (AR) الوسيط الرابع (الأعلام) اختياريٌّ معلَن ⇒ المدى [3, 4].
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_31, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MEM_MAP_REGION);
                    inst.operands.push_back(argOperands[0]); // physical address
                    inst.operands.push_back(argOperands[1]); // virtual address
                    inst.operands.push_back(argOperands[2]); // size
                    if (argResults.size() > 3)
                        inst.operands.push_back(argOperands[3]); // flags
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_32)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName, Ar::CompilerSys::SYS_32, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MEM_GET_REPORT);
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
