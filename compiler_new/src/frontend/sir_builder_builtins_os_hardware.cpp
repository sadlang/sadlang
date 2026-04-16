// ============================================================================
// sir_builder_builtins_os_hardware.cpp
// (AR) وحدات العتاد: PCI، DMA، الشاشة (Framebuffer)، ACPI، التزامن المنخفض
//      هذا الملف انفصل عن sir_builder_builtins_async.cpp لتقليل حجمه
// (EN) Hardware Modules: PCI, DMA, Framebuffer, ACPI, Low-level Sync
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

            std::optional<BuildResult> SIRBuilder::buildBuiltinSystem_OsHardware(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {

                // ─── 15e. وحدة PCI ───
                if (funcName == "عدد_أجهزة_ناقل" || funcName == "pci_enumerate")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_ENUMERATE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "اقرأ_اعدادات_ناقل" || funcName == "pci_read_config")
                {
                    if (argResults.size() < 4)
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_READ_CONFIG);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // bus
                    inst.operands.push_back(argOperands[1]); // device
                    inst.operands.push_back(argOperands[2]); // function
                    inst.operands.push_back(argOperands[3]); // offset
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "اكتب_اعدادات_ناقل" || funcName == "pci_write_config")
                {
                    if (argResults.size() < 5)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_WRITE_CONFIG);
                    inst.operands.push_back(argOperands[0]); // bus
                    inst.operands.push_back(argOperands[1]); // device
                    inst.operands.push_back(argOperands[2]); // function
                    inst.operands.push_back(argOperands[3]); // offset
                    inst.operands.push_back(argOperands[4]); // value
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "عدد_الأجهزة" || funcName == "pci_device_count")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_GET_DEVICE_COUNT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "تقرير_ناقل" || funcName == "pci_report")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15f. وحدة DMA المتقدمة ───
                if (funcName == "هيئ_نقل_مباشر" || funcName == "dma_init_full")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_DMA_INIT);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "ابدأ_نقل" || funcName == "dma_transfer")
                {
                    if (argResults.size() < 3)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_DMA_TRANSFER);
                    inst.operands.push_back(argOperands[0]); // source
                    inst.operands.push_back(argOperands[1]); // destination
                    inst.operands.push_back(argOperands[2]); // size
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "حالة_نقل" || funcName == "dma_status")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_DMA_STATUS);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "تقرير_نقل" || funcName == "dma_report")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_DMA_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15g. وحدة الشاشة / Framebuffer ───
                if (funcName == "هيئ_شاشة" || funcName == "fb_init")
                {
                    if (argResults.size() < 2)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_INIT);
                    inst.operands.push_back(argOperands[0]); // width
                    inst.operands.push_back(argOperands[1]); // height
                    if (argResults.size() > 2)
                        inst.operands.push_back(argOperands[2]); // bpp
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "ارسم_نقطة" || funcName == "fb_set_pixel")
                {
                    if (argResults.size() < 3)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_SET_PIXEL);
                    inst.operands.push_back(argOperands[0]); // x
                    inst.operands.push_back(argOperands[1]); // y
                    inst.operands.push_back(argOperands[2]); // color
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "ارسم_مستطيل" || funcName == "fb_draw_rect")
                {
                    if (argResults.size() < 5)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_DRAW_RECT);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "املأ_مستطيل" || funcName == "fb_fill_rect")
                {
                    if (argResults.size() < 5)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_FILL_RECT);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "ارسم_خط" || funcName == "fb_draw_line")
                {
                    if (argResults.size() < 5)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_DRAW_LINE);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "ارسم_نص" || funcName == "fb_draw_string")
                {
                    if (argResults.size() < 3)
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_DRAW_STRING);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "امسح_شاشة" || funcName == "fb_clear")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_CLEAR);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]); // color (optional)
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "تقرير_شاشة" || funcName == "fb_report")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15h. وحدة ACPI ───
                if (funcName == "هيئ_طاقة" || funcName == "acpi_init")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_INIT);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "ابحث_جدول_طاقة" || funcName == "acpi_find_table")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_FIND_TABLE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // table signature
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "اطفئ" || funcName == "acpi_shutdown")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_SHUTDOWN);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "تقرير_طاقة" || funcName == "acpi_report")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_GET_REPORT);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }

                // ─── 15i. وحدة التزامن / Sync ───
                if (funcName == "هيئ_قفل_دوار" || funcName == "spinlock_init")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SPINLOCK_INIT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "اقفل_دوار" || funcName == "spinlock_lock")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SPINLOCK_LOCK);
                    inst.operands.push_back(argOperands[0]); // lock ptr
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "افتح_قفل_دوار" || funcName == "spinlock_unlock")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SPINLOCK_UNLOCK);
                    inst.operands.push_back(argOperands[0]); // lock ptr
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "هيئ_كابح" || funcName == "mutex_init")
                {
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MUTEX_INIT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "اقفل_كابح" || funcName == "mutex_lock")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MUTEX_LOCK);
                    inst.operands.push_back(argOperands[0]); // mutex ptr
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "افتح_كابح" || funcName == "mutex_unlock")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MUTEX_UNLOCK);
                    inst.operands.push_back(argOperands[0]); // mutex ptr
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "هيئ_اشارة" || funcName == "semaphore_init")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SEMAPHORE_INIT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // count
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "هيئ_حاجز" || funcName == "barrier_init")
                {
                    if (argResults.empty())
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_BARRIER_INIT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    inst.operands.push_back(argOperands[0]); // count
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
