// ============================================================================
// sir_builder_builtins_os_hardware.cpp
// (AR) وحدات العتاد: PCI، DMA، الشاشة (Framebuffer)، ACPI، التزامن المنخفض
//      هذا الملف انفصل عن sir_builder_builtins_async.cpp لتقليل حجمه
// (EN) Hardware Modules: PCI, DMA, Framebuffer, ACPI, Low-level Sync
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

            std::optional<BuildResult> BuiltinBuilder::buildBuiltinSystem_OsHardware(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {

                // ─── 15e. وحدة PCI ───
                if (funcName == Bn::CompilerHw::HW_0)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_ENUMERATE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerHw::HW_1)
                {
                    if (argResults.size() < 4)
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_READ_CONFIG);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // bus
                    inst.operands.push_back(argOperands[1]); // device
                    inst.operands.push_back(argOperands[2]); // function
                    inst.operands.push_back(argOperands[3]); // offset
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerHw::HW_2)
                {
                    if (argResults.size() < 5)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_WRITE_CONFIG);
                    inst.operands.push_back(argOperands[0]); // bus
                    inst.operands.push_back(argOperands[1]); // device
                    inst.operands.push_back(argOperands[2]); // function
                    inst.operands.push_back(argOperands[3]); // offset
                    inst.operands.push_back(argOperands[4]); // value
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_3)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_GET_DEVICE_COUNT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerHw::HW_4)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15f. وحدة DMA المتقدمة ───
                if (funcName == Bn::CompilerHw::HW_5)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_DMA_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_6)
                {
                    if (argResults.size() < 3)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_DMA_TRANSFER);
                    inst.operands.push_back(argOperands[0]); // source
                    inst.operands.push_back(argOperands[1]); // destination
                    inst.operands.push_back(argOperands[2]); // size
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_7)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_DMA_STATUS);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerHw::HW_8)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_DMA_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15g. وحدة الشاشة / Framebuffer ───
                if (funcName == Bn::CompilerHw::HW_9)
                {
                    if (argResults.size() < 2)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_INIT);
                    inst.operands.push_back(argOperands[0]); // width
                    inst.operands.push_back(argOperands[1]); // height
                    if (argResults.size() > 2)
                        inst.operands.push_back(argOperands[2]); // bpp
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_10)
                {
                    if (argResults.size() < 3)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_SET_PIXEL);
                    inst.operands.push_back(argOperands[0]); // x
                    inst.operands.push_back(argOperands[1]); // y
                    inst.operands.push_back(argOperands[2]); // color
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::UIPlatform::DRAW_RECT)
                {
                    if (argResults.size() < 5)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_DRAW_RECT);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_11)
                {
                    if (argResults.size() < 5)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_FILL_RECT);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::UIPlatform::DRAW_LINE)
                {
                    if (argResults.size() < 5)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_DRAW_LINE);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_12)
                {
                    if (argResults.size() < 3)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_DRAW_STRING);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_13)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_CLEAR);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]); // color (optional)
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_14)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15h. وحدة ACPI ───
                if (funcName == Bn::CompilerHw::HW_15)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_16)
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_FIND_TABLE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // table signature
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerHw::HW_17)
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_SHUTDOWN);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_18)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15i. وحدة التزامن / Sync ───
                if (funcName == Bn::CompilerHw::HW_19)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SPINLOCK_INIT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerHw::HW_20)
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SPINLOCK_LOCK);
                    inst.operands.push_back(argOperands[0]); // lock ptr
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_21)
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SPINLOCK_UNLOCK);
                    inst.operands.push_back(argOperands[0]); // lock ptr
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_22)
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MUTEX_INIT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerHw::HW_23)
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MUTEX_LOCK);
                    inst.operands.push_back(argOperands[0]); // mutex ptr
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_24)
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MUTEX_UNLOCK);
                    inst.operands.push_back(argOperands[0]); // mutex ptr
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_25)
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SEMAPHORE_INIT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // count
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerHw::HW_26)
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_BARRIER_INIT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // count
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
