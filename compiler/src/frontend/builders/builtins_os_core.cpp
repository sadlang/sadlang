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
namespace Bn = Sad::Builtins::Names;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            std::optional<BuildResult> BuiltinBuilder::buildBuiltinSystem_OsCore(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {

                // ─── 15a. وحدة المعالج المتقدمة / Advanced CPU Module ───
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_0)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_GET_INFO);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_1)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_GET_FEATURES);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_2)
                {
                    if (argResults.empty())
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
                    if (argResults.size() < 2)
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
                    if (argResults.empty())
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
                    if (argResults.size() < 2)
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
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_INVLPG);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_6)
                {
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
                    if (argResults.empty())
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
                    SIRInstruction inst(SIROpcode::LOWLEVEL_GDT_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_8)
                {
                    // (AR) وسيط اختياريّ: مؤشّر واصف الجدول (limit+base). في الوضع
                    //      الحرّ يلزم لإصدار lgdt؛ المستضاف يتجاهله (sad_ll_gdt_load).
                    SIRInstruction inst(SIROpcode::LOWLEVEL_GDT_LOAD);
                    if (!argResults.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_9)
                {
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
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_11)
                {
                    if (argResults.size() < 2)
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
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_UNMAP);
                    inst.operands.push_back(argOperands[0]); // virtual address
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_13)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_FLUSH_TLB);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_14)
                {
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
                    SIRInstruction inst(SIROpcode::LOWLEVEL_IDT_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_16)
                {
                    // (AR) وسيط اختياريّ: مؤشّر واصف الجدول (limit+base). في الوضع
                    //      الحرّ يلزم لإصدار lidt؛ المستضاف يتجاهله (sad_ll_idt_load).
                    SIRInstruction inst(SIROpcode::LOWLEVEL_IDT_LOAD);
                    if (!argResults.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_17)
                {
                    if (argResults.size() < 2)
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
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_IDT_ENABLE_IRQ);
                    inst.operands.push_back(argOperands[0]); // IRQ number
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerCpuCtl::CPUCTL_19)
                {
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
