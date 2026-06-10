/**
 * @file builtin_kernel_acpi.cpp
 * @brief (AR) وحدة ACPI — إدارة الطاقة والمقاطعات المتقدمة
 * @brief (EN) ACPI module — Advanced Configuration and Power Interface
 *
 * @note يتطلب: builtin_common.h أو headers متخصصة
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "interpreter_core.h"
#include "value.h"
#include "acpi.h"
#include "apic.h"

#include <memory>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Sad
{
    namespace Interpreter
    {

// ═══════════════════════════════════════════════════════════════
// SEH wrappers (Windows only) — must be separate functions, not lambdas
// ═══════════════════════════════════════════════════════════════
#ifdef _WIN32

        // Generic SEH wrapper for int-returning void functions via function pointer
        typedef int (*IntFuncPtr)(void *ctx);
        static int sehCallInt(IntFuncPtr fn, void *ctx)
        {
            __try
            {
                return fn(ctx);
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                return -1;
            }
        }

        static int sehACPIInit()
        {
            __try
            {
                auto &a = LowLevel::ACPIManager::getInstance();
                a.initialize();
                return 0;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                return -1;
            }
        }

        static int sehACPIInitRSDP(uint64_t addr)
        {
            __try
            {
                auto &a = LowLevel::ACPIManager::getInstance();
                a.initializeFromRSDP(addr);
                return 0;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                return -1;
            }
        }

        static double sehACPIFindTable(const char *sig)
        {
            __try
            {
                auto &a = LowLevel::ACPIManager::getInstance();
                auto p = a.findTable(sig);
                return static_cast<double>(reinterpret_cast<uintptr_t>(p));
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                return 0.0;
            }
        }

        static int sehACPIEnable()
        {
            __try
            {
                auto &a = LowLevel::ACPIManager::getInstance();
                return a.enableACPI() ? 0 : -1;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                return -1;
            }
        }

        static int sehACPISleep(int s)
        {
            __try
            {
                auto &a = LowLevel::ACPIManager::getInstance();
                return a.enterSleepState(static_cast<LowLevel::SleepState>(s)) ? 0 : -1;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                return -1;
            }
        }

        static int sehAPICInit(uint64_t base)
        {
            __try
            {
                auto &a = LowLevel::APICManager::getInstance();
                return a.initLocalAPIC(base) ? 0 : -1;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                // Reset base pointer so subsequent MMIO calls hit null-check safely
                auto &a = LowLevel::APICManager::getInstance();
                a.resetLocalAPICBase();
                return -1;
            }
        }

        static int sehAPICInitIO(uint8_t id, uint64_t base, uint32_t gsib)
        {
            auto &a = LowLevel::APICManager::getInstance();
            __try
            {
                return a.initIOAPIC(id, base, gsib) ? 0 : -1;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                // Remove partially-added IO APIC entry with bad MMIO pointer
                a.popLastIOAPIC();
                return -1;
            }
        }

        static int sehAPICDisablePIC()
        {
            __try
            {
                auto &a = LowLevel::APICManager::getInstance();
                a.disablePIC();
                return 0;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                auto &a = LowLevel::APICManager::getInstance();
                a.resetLocalAPICBase();
                return -1;
            }
        }

        static uint32_t sehAPICCalibrateTimer(uint32_t hz)
        {
            __try
            {
                auto &a = LowLevel::APICManager::getInstance();
                return a.calibrateTimer(hz);
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                auto &a = LowLevel::APICManager::getInstance();
                a.resetLocalAPICBase();
                return 0;
            }
        }

        static int sehAPICMaskIRQ(uint8_t irq)
        {
            __try
            {
                auto &a = LowLevel::APICManager::getInstance();
                a.maskIRQ(irq);
                return 0;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                return -1;
            }
        }

        static int sehAPICUnmaskIRQ(uint8_t irq)
        {
            __try
            {
                auto &a = LowLevel::APICManager::getInstance();
                a.unmaskIRQ(irq);
                return 0;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                return -1;
            }
        }

        static int sehAPICRouteIRQ(uint8_t irq, uint8_t vec, uint8_t dest)
        {
            __try
            {
                auto &a = LowLevel::APICManager::getInstance();
                a.routeIRQ(irq, vec, dest);
                return 0;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                return -1;
            }
        }

#endif // _WIN32

        // ═══════════════════════════════════════════════════════════════
        // Registration
        // ═══════════════════════════════════════════════════════════════
        void registerBuiltinsKernelACPI(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // ═══════════════════════════════════════════════════════════════
            // SEH-safe overrides for part19 functions (Windows only)
            // ═══════════════════════════════════════════════════════════════
#ifdef _WIN32
            {
                // (AR) تهيئة APIC عبر SEH / (EN) SEH-safe APIC init
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    uint64_t base = args.empty() ? 0xFEE00000ULL : static_cast<uint64_t>(args[0]->toDouble());
                    return std::make_shared<Data::Value>(sehAPICInit(base));
                };
            }
            {
                // تعطيل وحدة التحكم PIC القديمة عبر SEH
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    return std::make_shared<Data::Value>(sehAPICDisablePIC());
                };
            }
            {
                // معايرة مؤقت APIC عبر SEH
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    uint32_t hz = args.empty() ? 1000 : static_cast<uint32_t>(args[0]->toInt());
                    return std::make_shared<Data::Value>(static_cast<double>(sehAPICCalibrateTimer(hz)));
                };
            }
#endif

            // ═══════════════════════════════════════════════════════════════
            // ACPI — 14 extended functions
            // ═══════════════════════════════════════════════════════════════
            // 1. acpi_تهيئة / acpi_init
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
#ifdef _WIN32
                    return std::make_shared<Data::Value>(sehACPIInit());
#else
                    auto &a = LowLevel::ACPIManager::getInstance();
                    a.initialize();
                    return std::make_shared<Data::Value>(0);
#endif
                };
            }

            // 2. acpi_تهيئة_من_rsdp / acpi_init_from_rsdp
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(-1);
                    uint64_t addr = static_cast<uint64_t>(args[0]->toDouble());
#ifdef _WIN32
                    return std::make_shared<Data::Value>(sehACPIInitRSDP(addr));
#else
                    auto &a = LowLevel::ACPIManager::getInstance();
                    a.initializeFromRSDP(addr);
                    return std::make_shared<Data::Value>(0);
#endif
                };
            }

            // 3. acpi_بحث_جدول / acpi_find_table
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.empty())
                        return std::make_shared<Data::Value>(0);
#ifdef _WIN32
                    return std::make_shared<Data::Value>(sehACPIFindTable(args[0]->toString().c_str()));
#else
                    auto &a = LowLevel::ACPIManager::getInstance();
                    auto ptr = a.findTable(args[0]->toString().c_str());
                    return std::make_shared<Data::Value>(static_cast<double>(reinterpret_cast<uintptr_t>(ptr)));
#endif
                };
            }

            // 4. acpi_تفعيل / acpi_enable
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
#ifdef _WIN32
                    return std::make_shared<Data::Value>(sehACPIEnable());
#else
                    auto &a = LowLevel::ACPIManager::getInstance();
                    return std::make_shared<Data::Value>(a.enableACPI() ? 0 : -1);
#endif
                };
            }

            // 5. acpi_تعطيل / acpi_disable
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    return std::make_shared<Data::Value>(0);
                };
            }

            // 6. acpi_نوم / acpi_sleep
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    int state = args.empty() ? 3 : args[0]->toInt();
#ifdef _WIN32
                    return std::make_shared<Data::Value>(sehACPISleep(state));
#else
                    auto &a = LowLevel::ACPIManager::getInstance();
                    return std::make_shared<Data::Value>(a.enterSleepState(static_cast<LowLevel::SleepState>(state)) ? 0 : -1);
#endif
                };
            }

            // 7. acpi_قراءة_مؤقت / acpi_read_pm_timer
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    auto &a = LowLevel::ACPIManager::getInstance();
                    return std::make_shared<Data::Value>(static_cast<double>(a.readPMTimer()));
                };
            }

            // 8. acpi_مؤقت_32بت / acpi_is_pm_timer_32bit
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    auto &a = LowLevel::ACPIManager::getInstance();
                    return std::make_shared<Data::Value>(a.isPMTimer32Bit() ? 1 : 0);
                };
            }

            // 9. acpi_تأخير / acpi_delay_us
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    uint32_t us = args.empty() ? 1000 : static_cast<uint32_t>(args[0]->toInt());
                    auto &a = LowLevel::ACPIManager::getInstance();
                    a.delayMicroseconds(us);
                    return std::make_shared<Data::Value>(0);
                };
            }

            // 10. acpi_عدد_معالجات / acpi_processor_count
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    auto &a = LowLevel::ACPIManager::getInstance();
                    return std::make_shared<Data::Value>(static_cast<int>(a.getProcessorCount()));
                };
            }

            // 11. acpi_إصدار / acpi_version
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    auto &a = LowLevel::ACPIManager::getInstance();
                    return std::make_shared<Data::Value>(static_cast<int>(a.getACPIVersion()));
                };
            }

            // 12. acpi_هل_مهيأ / acpi_is_initialized
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    auto &a = LowLevel::ACPIManager::getInstance();
                    return std::make_shared<Data::Value>(a.isInitialized() ? 1 : 0);
                };
            }

            // 13. acpi_عنوان_apic / acpi_local_apic_address
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    auto &a = LowLevel::ACPIManager::getInstance();
                    return std::make_shared<Data::Value>(static_cast<double>(a.getLocalAPICAddress()));
                };
            }

            // 14. acpi_ecam_قاعدة / acpi_ecam_base
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    uint16_t seg = args.size() > 0 ? static_cast<uint16_t>(args[0]->toInt()) : 0;
                    uint8_t bus = args.size() > 1 ? static_cast<uint8_t>(args[1]->toInt()) : 0;
                    auto &a = LowLevel::ACPIManager::getInstance();
                    return std::make_shared<Data::Value>(static_cast<double>(a.getECAMBase(seg, bus)));
                };
            }

            // ═══════════════════════════════════════════════════════════════
            // APIC — 16 extended functions
            // ═══════════════════════════════════════════════════════════════
            // 1. apic_مدعوم / apic_supported
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    auto &a = LowLevel::APICManager::getInstance();
                    return std::make_shared<Data::Value>(a.isAPICSupported() ? 1 : 0);
                };
            }

            // 2. apic_x2_مدعوم / apic_x2_supported
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    auto &a = LowLevel::APICManager::getInstance();
                    return std::make_shared<Data::Value>(a.isX2APICSupported() ? 1 : 0);
                };
            }

            // 3. apic_معرّف / apic_id
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    auto &a = LowLevel::APICManager::getInstance();
                    return std::make_shared<Data::Value>(static_cast<int>(a.getAPICId()));
                };
            }

            // 4. apic_أولوية_مهمة / apic_set_priority
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    uint8_t prio = args.empty() ? 0 : static_cast<uint8_t>(args[0]->toInt());
                    auto &a = LowLevel::APICManager::getInstance();
                    a.setTaskPriority(prio);
                    return std::make_shared<Data::Value>(0);
                };
            }

            // 5. apic_تهيئة_مؤقت / apic_init_timer
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    uint8_t vec = args.size() > 0 ? static_cast<uint8_t>(args[0]->toInt()) : 32;
                    int mode = args.size() > 1 ? args[1]->toInt() : 0;
                    int div = args.size() > 2 ? args[2]->toInt() : 3;
                    auto &a = LowLevel::APICManager::getInstance();
                    a.initTimer(vec, static_cast<LowLevel::TimerMode>(mode), static_cast<LowLevel::TimerDivide>(div));
                    return std::make_shared<Data::Value>(0);
                };
            }

            // 6. apic_بدء_مؤقت / apic_start_timer
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    uint32_t count = args.empty() ? 1000000 : static_cast<uint32_t>(args[0]->toInt());
                    auto &a = LowLevel::APICManager::getInstance();
                    a.startTimer(count);
                    return std::make_shared<Data::Value>(0);
                };
            }

            // 7. apic_إيقاف_مؤقت / apic_stop_timer
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    auto &a = LowLevel::APICManager::getInstance();
                    a.stopTimer();
                    return std::make_shared<Data::Value>(0);
                };
            }

            // 8. apic_عداد_مؤقت / apic_timer_count
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    auto &a = LowLevel::APICManager::getInstance();
                    return std::make_shared<Data::Value>(static_cast<int>(a.getTimerCount()));
                };
            }

            // 9. apic_أرسل_للكل / apic_send_ipi_all
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    uint8_t vec = args.empty() ? 0 : static_cast<uint8_t>(args[0]->toInt());
                    bool self = args.size() > 1 ? (args[1]->toInt() != 0) : false;
                    auto &a = LowLevel::APICManager::getInstance();
                    a.sendIPIToAll(vec, self);
                    return std::make_shared<Data::Value>(0);
                };
            }

            // 10. apic_أرسل_init / apic_send_init
            {
                // تهيئة APIC المحلي عبر SEH (آمن ضد الأعطال)
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    uint8_t dest = args.empty() ? 0 : static_cast<uint8_t>(args[0]->toInt());
                    auto &a = LowLevel::APICManager::getInstance();
                    a.sendINIT(dest);
                    return std::make_shared<Data::Value>(0);
                };
            }

            // 11. apic_أرسل_sipi / apic_send_sipi
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                        return std::make_shared<Data::Value>(-1);
                    uint8_t dest = static_cast<uint8_t>(args[0]->toInt());
                    uint8_t page = static_cast<uint8_t>(args[1]->toInt());
                    auto &a = LowLevel::APICManager::getInstance();
                    a.sendSIPI(dest, page);
                    return std::make_shared<Data::Value>(0);
                };
            }

            // 12. apic_انتظر_تسليم / apic_wait_delivery
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    auto &a = LowLevel::APICManager::getInstance();
                    a.waitForDelivery();
                    return std::make_shared<Data::Value>(0);
                };
            }

            // 13. apic_قناع_irq / apic_mask_irq
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    uint8_t irq = args.empty() ? 0 : static_cast<uint8_t>(args[0]->toInt());
#ifdef _WIN32
                    return std::make_shared<Data::Value>(sehAPICMaskIRQ(irq));
#else
                    auto &a = LowLevel::APICManager::getInstance();
                    a.maskIRQ(irq);
                    return std::make_shared<Data::Value>(0);
#endif
                };
            }

            // 14. apic_إلغاء_قناع_irq / apic_unmask_irq
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    uint8_t irq = args.empty() ? 0 : static_cast<uint8_t>(args[0]->toInt());
#ifdef _WIN32
                    return std::make_shared<Data::Value>(sehAPICUnmaskIRQ(irq));
#else
                    auto &a = LowLevel::APICManager::getInstance();
                    a.unmaskIRQ(irq);
                    return std::make_shared<Data::Value>(0);
#endif
                };
            }

            // 15. apic_عدد_io / apic_io_count
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    auto &a = LowLevel::APICManager::getInstance();
                    return std::make_shared<Data::Value>(static_cast<int>(a.getIOAPICCount()));
                };
            }

            // 16. apic_تهيئة_io / apic_init_io
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 3)
                        return std::make_shared<Data::Value>(-1);
                    uint8_t id = static_cast<uint8_t>(args[0]->toInt());
                    uint64_t base = static_cast<uint64_t>(args[1]->toDouble());
                    uint32_t gsib = static_cast<uint32_t>(args[2]->toInt());
#ifdef _WIN32
                    return std::make_shared<Data::Value>(sehAPICInitIO(id, base, gsib));
#else
                    auto &a = LowLevel::APICManager::getInstance();
                    return std::make_shared<Data::Value>(a.initIOAPIC(id, base, gsib) ? 0 : -1);
#endif
                };
            }

            // ═══════════════════════════════════════════════════════════════
            // Diacritic-free alias for part19 apic_وجّه_irq (lexer strips shaddah)
            // ═══════════════════════════════════════════════════════════════
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 3)
                        ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
                    uint8_t irq = static_cast<uint8_t>(args[0]->toInt());
                    uint8_t vec = static_cast<uint8_t>(args[1]->toInt());
                    uint8_t dest = static_cast<uint8_t>(args[2]->toInt());
#ifdef _WIN32
                    return std::make_shared<Data::Value>(sehAPICRouteIRQ(irq, vec, dest));
#else
                    auto &apic = LowLevel::APICManager::getInstance();
                    apic.routeIRQ(irq, vec, dest);
                    return std::make_shared<Data::Value>(0);
#endif
                };
                // وجه without shaddah = \xd9\x88\xd8\xac\xd9\x87
            }

        } // registerBuiltinsKernelACPI

    } // namespace Interpreter
} // namespace Sad
