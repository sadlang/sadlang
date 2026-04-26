/**
 * @file builtin_kernel_timers.cpp
 * @brief (AR) وحدة المؤقتات — إدارة PIT وAPIC timer
 * @brief (EN) Timers module — PIT and APIC timer management
 *
 * @note يتطلب: builtin_common.h أو headers متخصصة
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "interpreter_core.h"
#include "builtin_registry.h"
#include "value.h"
#include "timer.h"

#include <memory>
#include <string>
#include <vector>

namespace Sad
{
    namespace Interpreter
    {

        // (AR) اختصار لأسماء ثوابت المؤقتات
        namespace Bt = Builtins::Names::KernelTimers;

        void registerBuiltinsKernelTimers(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // ═══════════════════════════════════════════════════════════════
            // 1. PIT — المؤقت القابل للبرمجة
            // ═══════════════════════════════════════════════════════════════
            // 1. timer_pit_init / مؤقت_تهيئة
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    int freq = args.size() > 0 ? args[0]->toInt() : 1000;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().pitInit(freq));
                };
                fm.registerBuiltinFunction(std::string(Bt::PIT_INIT), f);
            }

            // 2. timer_pit_freq / مؤقت_التردد
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getPITFrequency());
                };
                fm.registerBuiltinFunction(std::string(Bt::PIT_FREQ), f);
            }

            // 3. timer_pit_count / مؤقت_العداد
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    return std::make_shared<Data::Value>(static_cast<int>(
                        LowLevel::TimerManager::getInstance().getPITCount()));
                };
                fm.registerBuiltinFunction(std::string(Bt::PIT_COUNT), f);
            }

            // 4. timer_pit_set_freq / مؤقت_تعيين_تردد
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    int freq = args.size() > 0 ? args[0]->toInt() : 1000;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().pitSetFrequency(freq));
                };
                fm.registerBuiltinFunction(std::string(Bt::PIT_SET_FREQ), f);
            }

            // ═══════════════════════════════════════════════════════════════
            // 2. RTC — ساعة الوقت الحقيقي
            // ═══════════════════════════════════════════════════════════════
            // 5. timer_rtc_seconds / مؤقت_الثواني
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getRTCSeconds());
                };
                fm.registerBuiltinFunction(std::string(Bt::RTC_SECONDS), f);
            }

            // 6. timer_rtc_minutes / مؤقت_الدقائق
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getRTCMinutes());
                };
                fm.registerBuiltinFunction(std::string(Bt::RTC_MINUTES), f);
            }

            // 7. timer_rtc_hours / مؤقت_الساعات
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getRTCHours());
                };
                fm.registerBuiltinFunction(std::string(Bt::RTC_HOURS), f);
            }

            // 8. timer_rtc_day / مؤقت_اليوم
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getRTCDay());
                };
                fm.registerBuiltinFunction(std::string(Bt::RTC_DAY), f);
            }

            // 9. timer_rtc_month / مؤقت_الشهر
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getRTCMonth());
                };
                fm.registerBuiltinFunction(std::string(Bt::RTC_MONTH), f);
            }

            // 10. timer_rtc_year / مؤقت_السنة
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getRTCYear());
                };
                fm.registerBuiltinFunction(std::string(Bt::RTC_YEAR), f);
            }

            // 11. timer_rtc_time / مؤقت_الوقت
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getRTCTimeString());
                };
                fm.registerBuiltinFunction(std::string(Bt::RTC_TIME), f);
            }

            // 12. timer_rtc_date / مؤقت_التاريخ
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getRTCDateString());
                };
                fm.registerBuiltinFunction(std::string(Bt::RTC_DATE), f);
            }

            // 13. timer_rtc_alarm / مؤقت_منبه
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    int h = args.size() > 0 ? args[0]->toInt() : 0;
                    int m = args.size() > 1 ? args[1]->toInt() : 0;
                    int s = args.size() > 2 ? args[2]->toInt() : 0;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().rtcSetAlarm(h, m, s));
                };
                fm.registerBuiltinFunction(std::string(Bt::RTC_ALARM), f);
            }

            // ═══════════════════════════════════════════════════════════════
            // 3. مؤقت النظام / System Timer
            // ═══════════════════════════════════════════════════════════════
            // 14. timer_ticks / مؤقت_نبضات
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    return std::make_shared<Data::Value>(static_cast<int>(
                        LowLevel::TimerManager::getInstance().getSystemTicks()));
                };
                fm.registerBuiltinFunction(std::string(Bt::SYS_TICKS), f);
            }

            // 15. timer_uptime_ms / مؤقت_وقت_التشغيل
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    return std::make_shared<Data::Value>(static_cast<int>(
                        LowLevel::TimerManager::getInstance().getUptimeMs()));
                };
                fm.registerBuiltinFunction(std::string(Bt::UPTIME_MS), f);
            }

            // 16. timer_uptime_sec / مؤقت_ثواني_التشغيل
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    return std::make_shared<Data::Value>(static_cast<int>(
                        LowLevel::TimerManager::getInstance().getUptimeSeconds()));
                };
                fm.registerBuiltinFunction(std::string(Bt::UPTIME_SEC), f);
            }

            // 17. timer_sleep / مؤقت_انتظار
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    int ms = args.size() > 0 ? args[0]->toInt() : 1;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().sleepMs(ms));
                };
                fm.registerBuiltinFunction(std::string(Bt::SLEEP), f);
            }

            // 18. timer_tick_rate / مؤقت_معدل_النبضات
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getTickRate());
                };
                fm.registerBuiltinFunction(std::string(Bt::TICK_RATE), f);
            }

            // 19. timer_set_tick_rate / مؤقت_تعيين_معدل
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    int hz = args.size() > 0 ? args[0]->toInt() : 1000;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().setTickRate(hz));
                };
                fm.registerBuiltinFunction(std::string(Bt::SET_TICK_RATE), f);
            }

            // 20. timer_tick / مؤقت_نقرة
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    LowLevel::TimerManager::getInstance().tick();
                    return std::make_shared<Data::Value>(0);
                };
                fm.registerBuiltinFunction(std::string(Bt::TICK), f);
            }

            // ═══════════════════════════════════════════════════════════════
            // 4. ساعة الإيقاف / Stopwatch
            // ═══════════════════════════════════════════════════════════════
            // 21. timer_sw_create / مؤقت_ساعة_جديدة
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().createStopwatch());
                };
                fm.registerBuiltinFunction(std::string(Bt::SW_CREATE), f);
            }

            // 22. timer_sw_start / مؤقت_ساعة_بدء
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    int id = args.size() > 0 ? args[0]->toInt() : 0;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().startStopwatch(id));
                };
                fm.registerBuiltinFunction(std::string(Bt::SW_START), f);
            }

            // 23. timer_sw_stop / مؤقت_ساعة_ايقاف
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    int id = args.size() > 0 ? args[0]->toInt() : 0;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().stopStopwatch(id));
                };
                fm.registerBuiltinFunction(std::string(Bt::SW_STOP), f);
            }

            // 24. timer_sw_reset / مؤقت_ساعة_صفر
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    int id = args.size() > 0 ? args[0]->toInt() : 0;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().resetStopwatch(id));
                };
                fm.registerBuiltinFunction(std::string(Bt::SW_RESET), f);
            }

            // 25. timer_sw_ms / مؤقت_ساعة_ملي
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    int id = args.size() > 0 ? args[0]->toInt() : 0;
                    return std::make_shared<Data::Value>(static_cast<int>(
                        LowLevel::TimerManager::getInstance().getStopwatchMs(id)));
                };
                fm.registerBuiltinFunction(std::string(Bt::SW_MS), f);
            }

            // 26. timer_sw_running / مؤقت_ساعة_تعمل
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    int id = args.size() > 0 ? args[0]->toInt() : 0;
                    return std::make_shared<Data::Value>(
                        LowLevel::TimerManager::getInstance().isStopwatchRunning(id) ? 1 : 0);
                };
                fm.registerBuiltinFunction(std::string(Bt::SW_RUNNING), f);
            }

            // ═══════════════════════════════════════════════════════════════
            // 5. تقرير وإعادة تعيين / Report & Reset
            // ═══════════════════════════════════════════════════════════════
            // 27. timer_report / مؤقت_تقرير
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().generateReport());
                };
                fm.registerBuiltinFunction(std::string(Bt::REPORT), f);
            }

            // 28. timer_reset / مؤقت_اعادة_تعيين
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    LowLevel::TimerManager::getInstance().reset();
                    return std::make_shared<Data::Value>(0);
                };
                fm.registerBuiltinFunction(std::string(Bt::TIMER_RESET), f);
            }

            // 29. timer_rtc_read / مؤقت_قراءة_rtc
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    auto dt = LowLevel::TimerManager::getInstance().rtcRead();
                    // (AR) ارجع نص تاريخ+وقت / (EN) Return date+time string
                    std::string result = std::to_string(dt.year) + "-" +
                                         (dt.month < 10 ? "0" : "") + std::to_string(dt.month) + "-" +
                                         (dt.day < 10 ? "0" : "") + std::to_string(dt.day) + " " +
                                         (dt.hours < 10 ? "0" : "") + std::to_string(dt.hours) + ":" +
                                         (dt.minutes < 10 ? "0" : "") + std::to_string(dt.minutes) + ":" +
                                         (dt.seconds < 10 ? "0" : "") + std::to_string(dt.seconds);
                    return std::make_shared<Data::Value>(result);
                };
                fm.registerBuiltinFunction(std::string(Bt::RTC_READ), f);
            }

            // 30. timer_pit_enabled / مؤقت_مفعل
            {
                auto f = [](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                {
                    (void)args;
                    // (AR) PIT مفعل إذا كان التردد > 0 / (EN) PIT is enabled if freq > 0
                    return std::make_shared<Data::Value>(
                        LowLevel::TimerManager::getInstance().getPITFrequency() > 0 ? 1 : 0);
                };
                fm.registerBuiltinFunction(std::string(Bt::PIT_ENABLED), f);
            }

        } // registerBuiltinsKernelTimers

    } // namespace Interpreter
} // namespace Sad
