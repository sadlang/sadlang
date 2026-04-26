// ============================================================================
// sir_builder_builtins_embedded.cpp
// (AR) Embedded builtins: serial, GPIO, timer, DMA, memory barriers
// (EN) Advanced Embedded Builtin Functions (18 functions)
// ============================================================================

#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <optional>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            std::optional<BuildResult> SIRBuilder::buildBuiltinSystem_Embedded(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {
                // ========================================================================
                // (AR) دوال Embedded المتقدمة — Advanced Embedded Builtin Functions (18)
                // (EN) Advanced Embedded Builtin Functions (18 functions)
                // ========================================================================
                // تسلسلي / Serial I/O — 4 دوال
                // GPIO — 3 دوال
                // مؤقت / Timer — 3 دوال
                // تحكم بالنظام / System Control — 3 دوال
                // حواجز ذاكرة / Memory Barriers — 3 دوال
                // DMA — 2 دوال
                // ========================================================================

                // ──────────────────────────────────────────────
                // 1. تسلسلي_هيئ(منفذ، سرعة) — تهيئة منفذ تسلسلي
                //    serial_init(port, baud_rate) — initialize serial port
                // ──────────────────────────────────────────────
                if (!isUserDefinedFunction && (funcName == "تسلسلي_هيئ" || funcName == "serial_init"))
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة تسلسلي_هيئ تتطلب 2 معاملات: منفذ، سرعة" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_SERIAL_INIT);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin تسلسلي_هيئ()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // 2. تسلسلي_ارسل(منفذ، بايت) — إرسال بايت عبر التسلسلي
                //    serial_send(port, byte) — send byte via serial
                // ──────────────────────────────────────────────
                if (!isUserDefinedFunction && (funcName == "تسلسلي_ارسل" || funcName == "serial_send" || funcName == "serial_write"))
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة تسلسلي_ارسل تتطلب 2 معاملات: منفذ، بايت" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_SERIAL_WRITE);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(inst);
                    }
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // 3. تسلسلي_استقبل(منفذ) — استقبال بايت من التسلسلي
                //    serial_receive(port) — receive byte from serial
                // ──────────────────────────────────────────────
                if (!isUserDefinedFunction && (funcName == "تسلسلي_استقبل" || funcName == "serial_receive" || funcName == "serial_read"))
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دالة تسلسلي_استقبل تتطلب معامل: منفذ" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_SERIAL_READ);
                    inst.operands.push_back(argOperands[0]);
                    inst.result = resultOp;
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin تسلسلي_استقبل()" << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // ──────────────────────────────────────────────
                // 4. تسلسلي_جاهز(منفذ) — فحص جاهزية البيانات
                //    serial_ready(port) — check if data available
                // ──────────────────────────────────────────────
                if (!isUserDefinedFunction && (funcName == "تسلسلي_جاهز" || funcName == "serial_ready"))
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دالة تسلسلي_جاهز تتطلب معامل: منفذ" << std::endl;
                        return BuildResult("", SadTypeKind::Boolean);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Boolean);
                    SIRInstruction inst(SIROpcode::BUILTIN_SERIAL_READY);
                    inst.operands.push_back(argOperands[0]);
                    inst.result = resultOp;
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin تسلسلي_جاهز()" << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Boolean);
                }

                // ──────────────────────────────────────────────
                // 5. منفذ_رقمي_اكتب(رقم، قيمة) — كتابة GPIO
                //    gpio_write(pin, value) — write to GPIO pin
                // ──────────────────────────────────────────────
                if (funcName == "منفذ_رقمي_اكتب" || funcName == "gpio_write" || funcName == "digital_write")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة منفذ_رقمي_اكتب تتطلب 2 معاملات: رقم_المنفذ، قيمة" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_GPIO_WRITE);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin منفذ_رقمي_اكتب()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // 6. منفذ_رقمي_اقرأ(رقم) — قراءة GPIO
                //    gpio_read(pin) — read from GPIO pin
                // ──────────────────────────────────────────────
                if (funcName == "منفذ_رقمي_اقرأ" || funcName == "gpio_read" || funcName == "digital_read")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دالة منفذ_رقمي_اقرأ تتطلب معامل: رقم_المنفذ" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_GPIO_READ);
                    inst.operands.push_back(argOperands[0]);
                    inst.result = resultOp;
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin منفذ_رقمي_اقرأ()" << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // ──────────────────────────────────────────────
                // 7. حدد_وضع_منفذ(رقم، وضع) — تحديد وضع GPIO
                //    gpio_mode(pin, mode) — set GPIO pin mode (0=input, 1=output)
                // ──────────────────────────────────────────────
                if (funcName == "حدد_وضع_منفذ" || funcName == "gpio_mode" || funcName == "pin_mode")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[خطأ] دالة حدد_وضع_منفذ تتطلب 2 معاملات: رقم_المنفذ، الوضع" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_GPIO_MODE);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin حدد_وضع_منفذ()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // 8. مؤقت_هيئ(تردد) — تهيئة مؤقت العتاد
                //    timer_init(freq) — initialize hardware timer
                // ──────────────────────────────────────────────
                if (funcName == "مؤقت_هيئ" || funcName == "timer_init")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دالة مؤقت_هيئ تتطلب معامل: التردد" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_TIMER_INIT);
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin مؤقت_هيئ()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // 9. مؤقت_قراءة() — قراءة قيمة المؤقت الحالية
                //    timer_read() — read current timer value
                // ──────────────────────────────────────────────
                if (funcName == "مؤقت_قراءة" || funcName == "timer_read")
                {
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_TIMER_READ);
                    inst.result = resultOp;
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin مؤقت_قراءة()" << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // ──────────────────────────────────────────────
                // 10. مؤقت_انتظر(ميكروثانية) — انتظار عدد ميكروثوان
                //     timer_wait(us) — wait for microseconds
                // ──────────────────────────────────────────────
                if (funcName == "مؤقت_انتظر" || funcName == "timer_wait" || funcName == "delay_us")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دالة مؤقت_انتظر تتطلب معامل: ميكروثوان" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_TIMER_WAIT);
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin مؤقت_انتظر()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // 11. اعد_تشغيل() — إعادة تشغيل النظام
                //     reset() / reboot() — system reset
                // ──────────────────────────────────────────────
                if (funcName == "اعد_تشغيل" || funcName == "reset" || funcName == "reboot")
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_RESET);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin اعد_تشغيل()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // 12. معرف_المعالج() — الحصول على معرّف المعالج
                //     cpu_id() / cpuid() — get CPU identification
                // ──────────────────────────────────────────────
                if (funcName == "معرف_المعالج" || funcName == "cpu_id" || funcName == "cpuid")
                {
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_CPUID);
                    if (!argOperands.empty())
                        inst.operands.push_back(argOperands[0]);
                    inst.result = resultOp;
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin معرف_المعالج()" << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // ──────────────────────────────────────────────
                // 13. عداد_الدورات() — قراءة عداد الساعة (TSC)
                //     rdtsc() / cycle_count() — read timestamp counter
                // ──────────────────────────────────────────────
                if (funcName == "عداد_الدورات" || funcName == "rdtsc" || funcName == "cycle_count")
                {
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_RDTSC);
                    inst.result = resultOp;
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin عداد_الدورات()" << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // ──────────────────────────────────────────────
                // 14. حاجز_ذاكرة() — حاجز ذاكرة كامل (mfence)
                //     memory_barrier() / mfence() — full memory fence
                // ──────────────────────────────────────────────
                if (funcName == "حاجز_ذاكرة" || funcName == "memory_barrier" || funcName == "mfence")
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_MFENCE);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin حاجز_ذاكرة()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // 15. حاجز_قراءة() — حاجز قراءة ذاكرة (lfence)
                //     read_barrier() / lfence() — load fence
                // ──────────────────────────────────────────────
                if (funcName == "حاجز_قراءة" || funcName == "read_barrier" || funcName == "lfence")
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_LFENCE);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin حاجز_قراءة()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // 16. حاجز_كتابة() — حاجز كتابة ذاكرة (sfence)
                //     write_barrier() / sfence() — store fence
                // ──────────────────────────────────────────────
                if (funcName == "حاجز_كتابة" || funcName == "write_barrier" || funcName == "sfence")
                {
                    SIRInstruction inst(SIROpcode::BUILTIN_SFENCE);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin حاجز_كتابة()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // 17. نقل_مباشر_هيئ(قناة، مصدر، وجهة، حجم) — تهيئة DMA
                //     dma_init(channel, src, dest, size) — initialize DMA channel
                // ──────────────────────────────────────────────
                if (funcName == "نقل_مباشر_هيئ" || funcName == "dma_init")
                {
                    if (argResults.size() < 4)
                    {
                        std::cerr << "[خطأ] دالة نقل_مباشر_هيئ تتطلب 4 معاملات: قناة، مصدر، وجهة، حجم" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_DMA_INIT);
                    for (auto &op : argOperands)
                        inst.operands.push_back(op);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin نقل_مباشر_هيئ()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                // ──────────────────────────────────────────────
                // 18. نقل_مباشر_ابدأ(قناة) — بدء نقل DMA
                //     dma_start(channel) — start DMA transfer
                // ──────────────────────────────────────────────
                if (funcName == "نقل_مباشر_ابدأ" || funcName == "dma_start")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[خطأ] دالة نقل_مباشر_ابدأ تتطلب معامل: رقم_القناة" << std::endl;
                        return BuildResult("", SadTypeKind::Void);
                    }
                    SIRInstruction inst(SIROpcode::BUILTIN_DMA_START);
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] builtin نقل_مباشر_ابدأ()" << std::endl;
#endif
                    return BuildResult("", SadTypeKind::Void);
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad