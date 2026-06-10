// ============================================================================
// sir_builder_builtins_os_system.cpp
// (AR) وحدات النظام: المجدول، الإقلاع، نظام الملفات، APIC، HPET، استدعاءات النظام، الذاكرة المتقدمة
//      هذا الملف انفصل عن sir_builder_builtins_async.cpp لتقليل حجمه
// (EN) System Modules: Scheduler, Boot, VFS, APIC, HPET, Syscall, Advanced Memory
//      Split from sir_builder_builtins_async.cpp
// ============================================================================

#include "sir_builder.h"
#include "builders/builtin_builder.h"
#include "sir_builder.h"
#include <iostream>

#include "builtin_registry.h"
namespace Bn = Sad::Builtins::Names;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            std::optional<BuildResult> BuiltinBuilder::buildBuiltinSystem_OsSystem(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {

                // ─── 15j. وحدة المجدول / Scheduler ───
                if (funcName == Bn::CompilerSys::SYS_0)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_1)
                {
                    if (argResults.empty())
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
                    if (argResults.size() < 2)
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
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_YIELD);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_4)
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_SLEEP);
                    inst.operands.push_back(argOperands[0]); // milliseconds
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_5)
                {
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
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_BOOT_GET_INFO);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerSys::SYS_7)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_BOOT_GET_MEMORY_MAP);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerSys::SYS_8)
                {
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
                    if (argResults.size() < 2)
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
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_UNMOUNT);
                    inst.operands.push_back(argOperands[0]); // mount point
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_11)
                {
                    if (argResults.empty())
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
                    if (argResults.size() < 2)
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
                    if (argResults.size() < 2)
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
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_CLOSE);
                    inst.operands.push_back(argOperands[0]); // fd
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_15)
                {
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
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_17)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_EOI);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_18)
                {
                    if (argResults.size() < 2)
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
                    if (argResults.size() < 2)
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
                    SIRInstruction inst(SIROpcode::LOWLEVEL_HPET_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_22)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_HPET_READ);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerSys::SYS_23)
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_HPET_SLEEP);
                    inst.operands.push_back(argOperands[0]); // nanoseconds
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_24)
                {
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
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SYSCALL_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_26)
                {
                    if (argResults.size() < 2)
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
                    if (argResults.empty())
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
                    if (argResults.empty())
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
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MEM_FREE_PHYS);
                    inst.operands.push_back(argOperands[0]); // physical address
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerSys::SYS_31)
                {
                    if (argResults.size() < 3)
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
