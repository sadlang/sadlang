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
                if (funcName == "معلومات_المعالج" || funcName == "cpu_get_info")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_GET_INFO);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "ميزات_المعالج" || funcName == "cpu_get_features")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_GET_FEATURES);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "اقرأ_سجل_نموذج" || funcName == "read_msr")
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
                if (funcName == "اكتب_سجل_نموذج" || funcName == "write_msr")
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
                if (funcName == "اقرأ_سجل_تحكم" || funcName == "read_cr")
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
                if (funcName == "اكتب_سجل_تحكم" || funcName == "write_cr")
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
                if (funcName == "ابطل_صفحة" || funcName == "invlpg")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_INVLPG);
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "تقرير_المعالج" || funcName == "cpu_report")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_CPU_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15b. وحدة GDT ───
                if (funcName == "هيئ_جدول_واصفات" || funcName == "gdt_init")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_GDT_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "حمل_جدول_واصفات" || funcName == "gdt_load")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_GDT_LOAD);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "تقرير_واصفات" || funcName == "gdt_report")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_GDT_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15c. وحدة الترحيل / Paging ───
                if (funcName == "هيئ_ترحيل" || funcName == "paging_init")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "رحل_صفحة" || funcName == "paging_map")
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
                if (funcName == "الغ_ترحيل" || funcName == "paging_unmap")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_UNMAP);
                    inst.operands.push_back(argOperands[0]); // virtual address
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "افرغ_ذاكرة_ترجمة" || funcName == "paging_flush_tlb")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_FLUSH_TLB);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "تقرير_ترحيل" || funcName == "paging_report")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PAGING_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15d. وحدة المقاطعات المتقدمة / Advanced Interrupts (IDT) ───
                if (funcName == "هيئ_جدول_مقاطعات" || funcName == "idt_init")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_IDT_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "حمل_جدول_مقاطعات" || funcName == "idt_load")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_IDT_LOAD);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "سجل_معالج_مقاطعة" || funcName == "register_isr")
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
                if (funcName == "فعل_طلب_مقاطعة" || funcName == "enable_irq")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_IDT_ENABLE_IRQ);
                    inst.operands.push_back(argOperands[0]); // IRQ number
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "تقرير_مقاطعات" || funcName == "idt_report")
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
