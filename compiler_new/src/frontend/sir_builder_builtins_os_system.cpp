// ============================================================================
// sir_builder_builtins_os_system.cpp
// (AR) وحدات النظام: المجدول، الإقلاع، نظام الملفات، APIC، HPET، استدعاءات النظام، الذاكرة المتقدمة
//      هذا الملف انفصل عن sir_builder_builtins_async.cpp لتقليل حجمه
// (EN) System Modules: Scheduler, Boot, VFS, APIC, HPET, Syscall, Advanced Memory
//      Split from sir_builder_builtins_async.cpp
// ============================================================================

#include "sir_builder.h"
#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            std::optional<BuildResult> SIRBuilder::buildBuiltinSystem_OsSystem(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {

                // ─── 15j. وحدة المجدول / Scheduler ───
                if (funcName == "هيئ_مجدول" || funcName == "sched_init")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_INIT);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "انشئ_عملية" || funcName == "sched_create_process")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_CREATE_PROC);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // name
                    if (argResults.size() > 1)
                        inst.operands.push_back(argOperands[1]); // priority
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer); // PID
                }
                if (funcName == "انشئ_خيط_نواة" || funcName == "sched_create_thread")
                {
                    if (argResults.size() < 2)
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_CREATE_THREAD);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // name
                    inst.operands.push_back(argOperands[1]); // entry function
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer); // TID
                }
                if (funcName == "تنازل" || funcName == "sched_yield")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_YIELD);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "نوم_مجدول" || funcName == "sched_sleep")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_SLEEP);
                    inst.operands.push_back(argOperands[0]); // milliseconds
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "تقرير_مجدول" || funcName == "sched_report")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SCHED_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15k. وحدة الإقلاع / Boot ───
                if (funcName == "معلومات_اقلاع" || funcName == "boot_info")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_BOOT_GET_INFO);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "خريطة_ذاكرة_اقلاع" || funcName == "boot_memory_map")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_BOOT_GET_MEMORY_MAP);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "تقرير_اقلاع" || funcName == "boot_report")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_BOOT_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15l. وحدة نظام الملفات الافتراضي / VFS ───
                if (funcName == "حمل_قرص" || funcName == "vfs_mount")
                {
                    if (argResults.size() < 2)
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_MOUNT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // device
                    inst.operands.push_back(argOperands[1]); // mount point
                    if (argResults.size() > 2)
                        inst.operands.push_back(argOperands[2]); // fs type
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "افصل_قرص" || funcName == "vfs_unmount")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_UNMOUNT);
                    inst.operands.push_back(argOperands[0]); // mount point
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "افتح_ملف_نواة" || funcName == "vfs_open")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_OPEN);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // path
                    if (argResults.size() > 1)
                        inst.operands.push_back(argOperands[1]); // flags
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer); // fd
                }
                if (funcName == "اقرأ_ملف_نواة" || funcName == "vfs_read")
                {
                    if (argResults.size() < 2)
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_READ);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // fd
                    inst.operands.push_back(argOperands[1]); // buffer/size
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer); // bytes read
                }
                if (funcName == "اكتب_ملف_نواة" || funcName == "vfs_write")
                {
                    if (argResults.size() < 2)
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_WRITE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // fd
                    inst.operands.push_back(argOperands[1]); // data
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer); // bytes written
                }
                if (funcName == "اغلق_ملف_نواة" || funcName == "vfs_close")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_CLOSE);
                    inst.operands.push_back(argOperands[0]); // fd
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "تقرير_ملفات" || funcName == "vfs_report")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_VFS_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15m. وحدة APIC ───
                if (funcName == "هيئ_متحكم_مقاطعات" || funcName == "apic_init")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_INIT);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "ارسل_نهاية_مقاطعة" || funcName == "apic_send_eoi")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_EOI);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "ارسل_مقاطعة_معالج" || funcName == "apic_send_ipi")
                {
                    if (argResults.size() < 2)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_IPI);
                    inst.operands.push_back(argOperands[0]); // target CPU
                    inst.operands.push_back(argOperands[1]); // vector
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "اضبط_مؤقت_متحكم" || funcName == "apic_set_timer")
                {
                    if (argResults.size() < 2)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SET_TIMER);
                    inst.operands.push_back(argOperands[0]); // divisor
                    inst.operands.push_back(argOperands[1]); // count
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "تقرير_متحكم_مقاطعات" || funcName == "apic_report")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15n. وحدة HPET ───
                if (funcName == "هيئ_مؤقت_دقيق" || funcName == "hpet_init")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_HPET_INIT);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "اقرأ_مؤقت_دقيق" || funcName == "hpet_read")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_HPET_READ);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "نوم_دقيق" || funcName == "hpet_sleep")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_HPET_SLEEP);
                    inst.operands.push_back(argOperands[0]); // nanoseconds
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "تقرير_مؤقت_دقيق" || funcName == "hpet_report")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_HPET_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15o. وحدة استدعاءات النظام / Syscall ───
                if (funcName == "هيئ_استدعاءات" || funcName == "syscall_init")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SYSCALL_INIT);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "سجل_استدعاء" || funcName == "syscall_register")
                {
                    if (argResults.size() < 2)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SYSCALL_REGISTER);
                    inst.operands.push_back(argOperands[0]); // syscall number
                    inst.operands.push_back(argOperands[1]); // handler function
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "نفذ_استدعاء" || funcName == "syscall_invoke")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SYSCALL_INVOKE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "تقرير_استدعاءات" || funcName == "syscall_report")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SYSCALL_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15p. عمليات الذاكرة المتقدمة / Advanced Memory ───
                if (funcName == "خصص_فيزيائي" || funcName == "alloc_physical")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MEM_ALLOC_PHYS);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // size
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer); // physical address
                }
                if (funcName == "حرر_فيزيائي" || funcName == "free_physical")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MEM_FREE_PHYS);
                    inst.operands.push_back(argOperands[0]); // physical address
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "رحل_منطقة" || funcName == "map_region")
                {
                    if (argResults.size() < 3)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MEM_MAP_REGION);
                    inst.operands.push_back(argOperands[0]); // physical address
                    inst.operands.push_back(argOperands[1]); // virtual address
                    inst.operands.push_back(argOperands[2]); // size
                    if (argResults.size() > 3)
                        inst.operands.push_back(argOperands[3]); // flags
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "تقرير_ذاكرة_نواة" || funcName == "mem_report")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MEM_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
