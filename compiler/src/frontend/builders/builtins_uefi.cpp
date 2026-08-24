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

            // (AR) حارسُ رتبةِ مدمجاتِ UEFI/ACPI/APIC الموسّع (سدُّ ح٤ جزء ب): قبل هذا
            //      الحارس لم يكن في هذا الملفِّ فحصٌ أصلًا — الوسيطُ الناقصُ كان
            //      يُسقَط دفعُه صامتًا (`if (!argOperands.empty())`) فيرقّعه زمنُ
            //      التشغيل بقيمٍ افتراضيّةٍ أو يقرأ قمامةً، والزائدُ يُقبَل ويُهمَل —
            //      أخطرُ صنفِ عيوبٍ في كودِ إقلاعِ نواة: سطرُ أمانٍ «يُنفَّذ» وهو غيرُ
            //      موجود. الرسالةُ من الكتالوج (SEM005 — لا نصوصَ يدويّة) وتُدفَع إلى
            //      errors_ فيُفشِل hasErrors() البناءَ (نمطُ حرف_من_رمز في
            //      builtins_strings_arrays). المدى [minArgs, maxArgs] لأنّ بعضَ
            //      المدمجاتِ بوسائطَ اختياريّةٍ ذاتِ افتراضاتٍ معلنةٍ في الخلفيّة
            //      (uefi_كتابة_متغير: السمات؛ apic_تهيئة_مؤقت: الثلاثة كلُّها) —
            //      {expected} يُظهر الحدَّ المخروق.
            // (EN) UEFI builtin arity guard (gap ح٤ part B): this file had zero checks;
            //      missing operands were silently dropped and runtime defaults papered
            //      over them, excess was accepted and ignored. Message from the error
            //      catalog (SEM005), pushed to errors_ so the build fails.
            //      (errors_ خاصّةٌ في SIRBuilder وBuiltinBuilder صديقتُها؛ الدالّةُ
            //      الحرّةُ هنا تتلقّى المرجعَ من موضعِ النداءِ العضويِّ الصديق.)
            [[nodiscard]] static bool checkUefiArity(std::vector<std::string> &errors,
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
                    // (AR) نمطان معلنان: بلا وسيطٍ (اكتشافٌ ذاتيّ) أو بمقبضِ الصورة وجدولِ النظام معًا.
                    //      المجموعةُ الشرعيّة {0، 2} لا المدى [0،2]: نداءٌ بوسيطٍ واحدٍ كان
                    //      «يمرّ» ويتبخّرُ وسيطُه في الخفض (الخفضُ يستهلك عند size >= 2 فقط) —
                    //      عينُ صنفِ العيبِ الذي وُجد الحارسُ ليقتله، فنرفضه بفحصٍ ثانٍ.
                    if (!checkUefiArity(b_.errors_, funcName, 0, 2, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    if (argResults.size() == 1 &&
                        !checkUefiArity(b_.errors_, funcName, 2, 2, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_IS_INITIALIZED);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_3)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_BS_EXITED);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_4)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
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
                    if (!checkUefiArity(b_.errors_, funcName, 3, 3, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 2, 2, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 2, 2, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GET_MEMORY_MAP);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }
                if (funcName == Bn::CompilerUefi::UEFI_10)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GET_MEMMAP_KEY);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_11)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_INIT_GOP);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_13)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GOP_MODE_COUNT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_16)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_CURRENT_GOP_MODE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_17)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FRAMEBUFFER_BASE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_18)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FRAMEBUFFER_SIZE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_19)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 3, 3, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 7, 7, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    // (AR) توقيع GOP Blt الكامل: عازل، عمليّة، سص المصدر، سص الوجهة، عرض، ارتفاع.
                    if (!checkUefiArity(b_.errors_, funcName, 8, 8, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_GET_TIME);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }
                if (funcName == Bn::CompilerUefi::UEFI_22)
                {
                    // (AR) توقيع زمن التشغيل سداسيٌّ حصرًا: سنة، شهر، يوم، ساعة، دقيقة، ثانية.
                    if (!checkUefiArity(b_.errors_, funcName, 6, 6, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    // (AR) الوسيط الثالث (السمات) اختياريٌّ معلَن بافتراضٍ في الخلفيّة ⇒ المدى [2, 3].
                    if (!checkUefiArity(b_.errors_, funcName, 2, 3, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_OPEN_VOLUME);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_26)
                {
                    // (AR) الوسيط الثالث (نمط الفتح) اختياريٌّ بافتراضِ القراءة ⇒ المدى [2, 3].
                    if (!checkUefiArity(b_.errors_, funcName, 2, 3, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    // (AR) الوسيط الثاني (الحجم) اختياريٌّ بافتراضِ 4096 ⇒ المدى [1, 2].
                    if (!checkUefiArity(b_.errors_, funcName, 1, 2, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkUefiArity(b_.errors_, funcName, 3, 3, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_REVISION);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }
                if (funcName == Bn::CompilerUefi::UEFI_33)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_VENDOR);
                    inst.result = SIROperand::Register(r, SadTypeKind::String);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::String);
                }
                if (funcName == Bn::CompilerUefi::UEFI_34)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_UEFI_FW_REVISION);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_35)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::String);
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
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_INIT_FULL);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_37)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_INIT_RSDP);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_38)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_ENABLE);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_39)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_DISABLE);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_40)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_IS_INITIALIZED);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_41)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_REBOOT);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_43)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
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
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_READ_PM_TIMER);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_46)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_IS_PM_32BIT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_47)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_ACPI_PROCESSOR_COUNT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_48)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    // (AR) الوسيطان (المقطع والناقل) اختياريّان بافتراضِ الصفر ⇒ المدى [0, 2].
                    if (!checkUefiArity(b_.errors_, funcName, 0, 2, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SUPPORTED);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_51)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_X2_SUPPORTED);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_52)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_ID);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_53)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    // (AR) الثلاثة (المتّجه، النمط، القاسم) اختياريّةٌ كلُّها بافتراضاتٍ معلنةٍ ⇒ المدى [0, 3].
                    if (!checkUefiArity(b_.errors_, funcName, 0, 3, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_INIT_TIMER);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_55)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_START_TIMER);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_56)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_STOP_TIMER);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_57)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    std::string r = b_.newTempRegister();
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_TIMER_COUNT);
                    inst.result = SIROperand::Register(r, SadTypeKind::Integer);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(r, SadTypeKind::Integer);
                }
                if (funcName == Bn::CompilerUefi::UEFI_58)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SET_PRIORITY);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_60)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_DISABLE_PIC);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_61)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_MASK_IRQ);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_62)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_UNMASK_IRQ);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_63)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 3, 3, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
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
                    // (AR) الوسيط الثاني (شمول الذات) اختياريٌّ بافتراضِ الصفر ⇒ المدى [1, 2].
                    if (!checkUefiArity(b_.errors_, funcName, 1, 2, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_IPI_ALL);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_65)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 1, 1, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_INIT_IPI);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_66)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 2, 2, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_SEND_SIPI);
                    for (auto &a : argOperands)
                        inst.operands.push_back(a);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_67)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 0, 0, argResults.size()))
                        return BuildResult("", SadTypeKind::Void);
                    SIRInstruction inst(SIROpcode::LOWLEVEL_APIC_WAIT_DELIVERY);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult("", SadTypeKind::Void);
                }
                if (funcName == Bn::CompilerUefi::UEFI_68)
                {
                    if (!checkUefiArity(b_.errors_, funcName, 3, 3, argResults.size()))
                        return BuildResult("", SadTypeKind::Integer);
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
