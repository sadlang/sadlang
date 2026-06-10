// ============================================================================
// sir_builder_builtins_uefi.cpp
// (AR) بروتوكول UEFI + ACPI الموسّع + APIC الموسّع
//      هذا الملف انفصل عن sir_builder_builtins_async.cpp لتقليل حجمه
// (EN) UEFI Protocol + Extended ACPI + Extended APIC
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

            std::optional<BuildResult> BuiltinBuilder::buildBuiltinSystem_Uefi(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {

                // =================================================================
                // القسم 16: بروتوكول الإقلاع الموحد UEFI / UEFI Boot Protocol
                // =================================================================

                // --- 16a. التهيئة والتحكم ---
                if (funcName == Bn::CompilerUefi::UEFI_0)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_INIT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_1)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_EXIT_BOOT_SERVICES);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]); // map key
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_2)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_IS_INITIALIZED);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_3)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_BS_EXITED);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_4)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_RESET_SYSTEM);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]); // reset type
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // --- 16b. إدارة الذاكرة ---
                if (funcName == Bn::CompilerUefi::UEFI_5)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_ALLOC_PAGES);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_6)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FREE_PAGES);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_7)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_ALLOC_POOL);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_8)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FREE_POOL);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_9)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GET_MEMORY_MAP);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }
                if (funcName == Bn::CompilerUefi::UEFI_10)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GET_MEMMAP_KEY);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_11)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_TOTAL_MEMORY);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                // --- 16c. بروتوكول الرسوميات GOP ---
                if (funcName == Bn::CompilerUefi::UEFI_12)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_INIT_GOP);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_13)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_SET_GOP_MODE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_14)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_QUERY_GOP_MODE);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }
                if (funcName == Bn::CompilerUefi::UEFI_15)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GOP_MODE_COUNT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_16)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_CURRENT_GOP_MODE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_17)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FRAMEBUFFER_BASE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_18)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FRAMEBUFFER_SIZE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_19)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FILL_SCREEN);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op); // r, g, b
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_20)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_DRAW_RECT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op); // x, y, w, h, r, g, b
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "uefi_blt" || funcName == "uefi_gop_blt")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GOP_BLT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                // --- 16d. خدمات وقت التشغيل ---
                if (funcName == Bn::CompilerUefi::UEFI_21)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GET_TIME);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }
                if (funcName == Bn::CompilerUefi::UEFI_22)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_SET_TIME);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_23)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GET_VARIABLE);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]); // var name
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }
                if (funcName == Bn::CompilerUefi::UEFI_24)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_SET_VARIABLE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op); // name, value, [attrs]
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                // --- 16e. نظام الملفات ---
                if (funcName == Bn::CompilerUefi::UEFI_25)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_OPEN_VOLUME);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_26)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_OPEN_FILE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_27)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_READ_FILE);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }
                if (funcName == Bn::CompilerUefi::UEFI_28)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_WRITE_FILE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_29)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_CLOSE_FILE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_30)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FILE_INFO);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // --- 16f. بروتوكولات ومعلومات ---
                if (funcName == Bn::CompilerUefi::UEFI_31)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_LOCATE_PROTOCOL);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_32)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_REVISION);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }
                if (funcName == Bn::CompilerUefi::UEFI_33)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_VENDOR);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }
                if (funcName == Bn::CompilerUefi::UEFI_34)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FW_REVISION);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_35)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── القسم 17: ACPI الموسّع / Extended ACPI ───

                // 17a: تهيئة وتفعيل
                if (funcName == Bn::CompilerUefi::UEFI_36)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_INIT_FULL);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_37)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_INIT_RSDP);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_38)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_ENABLE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_39)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_DISABLE);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_40)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_IS_INITIALIZED);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_41)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_VERSION);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                // 17b: إدارة الطاقة
                if (funcName == Bn::CompilerUefi::UEFI_42)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_REBOOT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_43)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_SLEEP);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_44)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_DELAY_US);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // 17c: مؤقت ومعالجات
                if (funcName == Bn::CompilerUefi::UEFI_45)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_READ_PM_TIMER);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_46)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_IS_PM_32BIT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_47)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_PROCESSOR_COUNT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_48)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_LOCAL_APIC_ADDR);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                // 17d: PCIe ECAM
                if (funcName == Bn::CompilerUefi::UEFI_49)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_ECAM_BASE);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (argOperands.size() > 1)
                        inst.operands.push_back(argOperands[1]);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                // ─── القسم 18: APIC الموسّع / Extended APIC ───

                // 18a: استعلام ومعلومات
                if (funcName == Bn::CompilerUefi::UEFI_50)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SUPPORTED);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_51)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_X2_SUPPORTED);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_52)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_ID);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_53)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_IO_COUNT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                // 18b: مؤقت APIC
                if (funcName == Bn::CompilerUefi::UEFI_54)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_INIT_TIMER);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_55)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_START_TIMER);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_56)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_STOP_TIMER);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_57)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_TIMER_COUNT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_58)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_CALIBRATE);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                // 18c: أولوية ومقاطعات
                if (funcName == Bn::CompilerUefi::UEFI_59)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SET_PRIORITY);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_60)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_DISABLE_PIC);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_61)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_MASK_IRQ);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_62)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_UNMASK_IRQ);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_63)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_ROUTE_IRQ);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // 18d: IPI متقدم
                if (funcName == Bn::CompilerUefi::UEFI_64)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_IPI_ALL);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_65)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_INIT_IPI);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_66)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_SIPI);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_67)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_WAIT_DELIVERY);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_68)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_INIT_IO);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
