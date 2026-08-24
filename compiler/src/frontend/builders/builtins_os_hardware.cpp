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
#include "error_manager.h" // (AR) buildBilingualMessage من كتالوج الأخطاء (مصدر الحقيقة)
#include "error_catalog.h" // (AR) RenderContext (حاملُ placeholders)
#include "error_codes.h"   // (AR) ErrorCode::SEM_WRONG_ARG_COUNT
namespace Bn = Sad::Builtins::Names;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // (AR) حارسُ رتبةِ مدمجاتِ العتاد (سدُّ ح٤ جزء ب): قبل هذا الحارس كان الوسيطُ
            //      الناقصُ يُعيد BuildResult فارغًا فيتبخّرُ النداءُ صامتًا،
            //      والزائدُ يُقبَل ويُهمَل — أخطرُ صنفِ عيوبٍ في
            //      كودِ نواةٍ: سطرُ أمانٍ «يُنفَّذ» وهو غيرُ موجود. الرسالةُ من الكتالوج
            //      (SEM005 — لا نصوصَ يدويّة) وتُدفَع إلى errors_ فيُفشِل hasErrors()
            //      البناءَ (نمطُ حرف_من_رمز في builtins_strings_arrays). المدى
            //      [minArgs, maxArgs] لأنّ بعضَ المدمجاتِ بوسيطٍ اختياريٍّ معلَن
            //      (هيئ_شاشة: عمقُ البكسل اختياريّ؛ امسح_شاشة: اللونُ اختياريّ)
            //      — {expected} يُظهر الحدَّ المخروق.
            // (EN) OS-hardware builtin arity guard (gap ح٤ part B): under-arity used to
            //      vanish silently, over-arity was accepted and ignored. Message from the
            //      error catalog (SEM005), pushed to errors_ so the build fails.
            //      (errors_ خاصّةٌ في SIRBuilder وBuiltinBuilder صديقتُها؛ الدالّةُ
            //      الحرّةُ هنا تتلقّى المرجعَ من موضعِ النداءِ العضويِّ الصديق.)
            [[nodiscard]] static bool checkOsHardwareArity(std::vector<std::string> &errors,
                                         const std::string &name,
                                         size_t minArgs, size_t maxArgs, size_t found)
            {
                if (found >= minArgs && found <= maxArgs)
                    return true;
                Sad::Errors::RenderContext ectx;
                ectx.placeholders = {
                    {"name", name},
                    {"expected", std::to_string(found < minArgs ? minArgs : maxArgs)},
                    {"found", std::to_string(found)}};
                errors.push_back(
                    Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                        Sad::Errors::ErrorCode::SEM_WRONG_ARG_COUNT, ectx));
                return false;
            }

            std::optional<BuildResult> BuiltinBuilder::buildBuiltinSystem_OsHardware(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {

                // ─── 15e. وحدة PCI ───
                if (funcName == Bn::CompilerHw::HW_0)
                {
                    if (!checkOsHardwareArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_ENUMERATE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerHw::HW_1)
                {
                    if (!checkOsHardwareArity(b_.errors_, funcName, 4, 4, argResults.size()))
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
                    if (!checkOsHardwareArity(b_.errors_, funcName, 5, 5, argResults.size()))
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
                    if (!checkOsHardwareArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_PCI_GET_DEVICE_COUNT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerHw::HW_4)
                {
                    if (!checkOsHardwareArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkOsHardwareArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_DMA_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_6)
                {
                    if (!checkOsHardwareArity(b_.errors_, funcName, 3, 3, argResults.size()))
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
                    if (!checkOsHardwareArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_DMA_STATUS);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerHw::HW_8)
                {
                    if (!checkOsHardwareArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    // (AR) الوسيط الثالث (عمق البكسل bpp) اختياريٌّ معلَن ⇒ المدى [2, 3].
                    if (!checkOsHardwareArity(b_.errors_, funcName, 2, 3, argResults.size()))
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
                    if (!checkOsHardwareArity(b_.errors_, funcName, 3, 3, argResults.size()))
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
                    if (!checkOsHardwareArity(b_.errors_, funcName, 5, 5, argResults.size()))
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
                    if (!checkOsHardwareArity(b_.errors_, funcName, 5, 5, argResults.size()))
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
                    if (!checkOsHardwareArity(b_.errors_, funcName, 5, 5, argResults.size()))
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
                    // (AR) توقيع زمن التشغيل sad_ll_fb_draw_string(x، y، نص) ثلاثيٌّ حصرًا.
                    if (!checkOsHardwareArity(b_.errors_, funcName, 3, 3, argResults.size()))
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
                    // (AR) اللون اختياريٌّ معلَن (optional في تعليق الذراع) ⇒ المدى [0, 1].
                    if (!checkOsHardwareArity(b_.errors_, funcName, 0, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_FB_CLEAR);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]); // color (optional)
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_14)
                {
                    if (!checkOsHardwareArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkOsHardwareArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_INIT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_16)
                {
                    if (!checkOsHardwareArity(b_.errors_, funcName, 1, 1, argResults.size()))
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
                    if (!checkOsHardwareArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_SHUTDOWN);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_18)
                {
                    if (!checkOsHardwareArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkOsHardwareArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SPINLOCK_INIT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerHw::HW_20)
                {
                    if (!checkOsHardwareArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SPINLOCK_LOCK);
                    inst.operands.push_back(argOperands[0]); // lock ptr
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_21)
                {
                    if (!checkOsHardwareArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_SPINLOCK_UNLOCK);
                    inst.operands.push_back(argOperands[0]); // lock ptr
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_22)
                {
                    if (!checkOsHardwareArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MUTEX_INIT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerHw::HW_23)
                {
                    if (!checkOsHardwareArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MUTEX_LOCK);
                    inst.operands.push_back(argOperands[0]); // mutex ptr
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_24)
                {
                    if (!checkOsHardwareArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_MUTEX_UNLOCK);
                    inst.operands.push_back(argOperands[0]); // mutex ptr
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerHw::HW_25)
                {
                    if (!checkOsHardwareArity(b_.errors_, funcName, 1, 1, argResults.size()))
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
                    if (!checkOsHardwareArity(b_.errors_, funcName, 1, 1, argResults.size()))
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
