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
                if (funcName == "uefi_تهيئة" || funcName == "uefi_initialize")
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
                if (funcName == "uefi_إنهاء_خدمات_إقلاع" || funcName == "uefi_exit_boot_services")
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
                if (funcName == "uefi_هل_مهيأ" || funcName == "uefi_is_initialized")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_IS_INITIALIZED);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "uefi_خدمات_إقلاع_منتهية" || funcName == "uefi_boot_services_exited")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_BS_EXITED);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "uefi_إعادة_تشغيل" || funcName == "uefi_reset_system")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_RESET_SYSTEM);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]); // reset type
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // --- 16b. إدارة الذاكرة ---
                if (funcName == "uefi_تخصيص_صفحات" || funcName == "uefi_allocate_pages")
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
                if (funcName == "uefi_تحرير_صفحات" || funcName == "uefi_free_pages")
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
                if (funcName == "uefi_تخصيص_كتلة" || funcName == "uefi_allocate_pool")
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
                if (funcName == "uefi_تحرير_كتلة" || funcName == "uefi_free_pool")
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
                if (funcName == "uefi_خريطة_ذاكرة" || funcName == "uefi_get_memory_map")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GET_MEMORY_MAP);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }
                if (funcName == "uefi_مفتاح_خريطة" || funcName == "uefi_get_memory_map_key")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GET_MEMMAP_KEY);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "uefi_ذاكرة_متاحة" || funcName == "uefi_total_memory")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_TOTAL_MEMORY);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                // --- 16c. بروتوكول الرسوميات GOP ---
                if (funcName == "uefi_تهيئة_رسوميات" || funcName == "uefi_init_gop")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_INIT_GOP);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "uefi_تعيين_وضع_رسوميات" || funcName == "uefi_set_gop_mode")
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
                if (funcName == "uefi_استعلام_وضع" || funcName == "uefi_query_gop_mode")
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
                if (funcName == "uefi_عدد_أوضاع_رسوميات" || funcName == "uefi_gop_mode_count")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GOP_MODE_COUNT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "uefi_وضع_رسوميات_حالي" || funcName == "uefi_current_gop_mode")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_CURRENT_GOP_MODE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "uefi_عنوان_إطار" || funcName == "uefi_framebuffer_base")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FRAMEBUFFER_BASE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "uefi_حجم_إطار" || funcName == "uefi_framebuffer_size")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FRAMEBUFFER_SIZE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "uefi_ملء_شاشة" || funcName == "uefi_fill_screen")
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
                if (funcName == "uefi_رسم_مستطيل" || funcName == "uefi_draw_rect")
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
                if (funcName == "uefi_الوقت" || funcName == "uefi_get_time")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GET_TIME);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }
                if (funcName == "uefi_تعيين_وقت" || funcName == "uefi_set_time")
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
                if (funcName == "uefi_قراءة_متغير" || funcName == "uefi_get_variable")
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
                if (funcName == "uefi_كتابة_متغير" || funcName == "uefi_set_variable")
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
                if (funcName == "uefi_فتح_وحدة_تخزين" || funcName == "uefi_open_volume")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_OPEN_VOLUME);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "uefi_فتح_ملف" || funcName == "uefi_open_file")
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
                if (funcName == "uefi_قراءة_ملف" || funcName == "uefi_read_file")
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
                if (funcName == "uefi_كتابة_ملف" || funcName == "uefi_write_file")
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
                if (funcName == "uefi_إغلاق_ملف" || funcName == "uefi_close_file")
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
                if (funcName == "uefi_معلومات_ملف" || funcName == "uefi_file_info")
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
                if (funcName == "uefi_بحث_بروتوكول" || funcName == "uefi_locate_protocol")
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
                if (funcName == "uefi_إصدار" || funcName == "uefi_revision")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_REVISION);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }
                if (funcName == "uefi_بائع" || funcName == "uefi_firmware_vendor")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_VENDOR);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }
                if (funcName == "uefi_إصدار_بائع" || funcName == "uefi_firmware_revision")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FW_REVISION);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "uefi_تقرير" || funcName == "uefi_report")
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
                if (funcName == "acpi_تهيئة" || funcName == "acpi_init")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_INIT_FULL);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "acpi_تهيئة_من_rsdp" || funcName == "acpi_init_from_rsdp")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_INIT_RSDP);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "acpi_تفعيل" || funcName == "acpi_enable")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_ENABLE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "acpi_تعطيل" || funcName == "acpi_disable")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_DISABLE);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "acpi_هل_مهيأ" || funcName == "acpi_is_initialized")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_IS_INITIALIZED);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "acpi_إصدار" || funcName == "acpi_version")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_VERSION);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                // 17b: إدارة الطاقة
                if (funcName == "acpi_إعادة_تشغيل" || funcName == "acpi_reboot")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_REBOOT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "acpi_نوم" || funcName == "acpi_sleep")
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
                if (funcName == "acpi_تأخير" || funcName == "acpi_delay_us")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_DELAY_US);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // 17c: مؤقت ومعالجات
                if (funcName == "acpi_قراءة_مؤقت" || funcName == "acpi_read_pm_timer")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_READ_PM_TIMER);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "acpi_مؤقت_32بت" || funcName == "acpi_is_pm_timer_32bit")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_IS_PM_32BIT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "acpi_عدد_معالجات" || funcName == "acpi_processor_count")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_PROCESSOR_COUNT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "acpi_عنوان_apic" || funcName == "acpi_local_apic_address")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_LOCAL_APIC_ADDR);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                // 17d: PCIe ECAM
                if (funcName == "acpi_ecam_قاعدة" || funcName == "acpi_ecam_base")
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
                if (funcName == "apic_مدعوم" || funcName == "apic_supported")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SUPPORTED);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "apic_x2_مدعوم" || funcName == "apic_x2_supported")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_X2_SUPPORTED);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "apic_معرّف" || funcName == "apic_id")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_ID);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "apic_عدد_io" || funcName == "apic_io_count")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_IO_COUNT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }

                // 18b: مؤقت APIC
                if (funcName == "apic_تهيئة_مؤقت" || funcName == "apic_init_timer")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_INIT_TIMER);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "apic_بدء_مؤقت" || funcName == "apic_start_timer")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_START_TIMER);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "apic_إيقاف_مؤقت" || funcName == "apic_stop_timer")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_STOP_TIMER);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "apic_عداد_مؤقت" || funcName == "apic_timer_count")
                {
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_TIMER_COUNT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == "apic_معايرة_مؤقت" || funcName == "apic_calibrate_timer")
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
                if (funcName == "apic_أولوية_مهمة" || funcName == "apic_set_priority")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SET_PRIORITY);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "apic_عطّل_pic" || funcName == "apic_disable_pic")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_DISABLE_PIC);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "apic_قناع_irq" || funcName == "apic_mask_irq")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_MASK_IRQ);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "apic_إلغاء_قناع_irq" || funcName == "apic_unmask_irq")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_UNMASK_IRQ);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "apic_وجّه_irq" || funcName == "apic_route_irq")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_ROUTE_IRQ);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }

                // 18d: IPI متقدم
                if (funcName == "apic_أرسل_للكل" || funcName == "apic_send_ipi_all")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_IPI_ALL);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "apic_أرسل_init" || funcName == "apic_send_init")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_INIT_IPI);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "apic_أرسل_sipi" || funcName == "apic_send_sipi")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_SIPI);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "apic_انتظر_تسليم" || funcName == "apic_wait_delivery")
                {
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_WAIT_DELIVERY);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == "apic_تهيئة_io" || funcName == "apic_init_io")
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
