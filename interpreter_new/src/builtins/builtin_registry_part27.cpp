/**
 * @file builtin_registry_part27.cpp
 * @brief (AR) دوال المؤقتات — PIT 8254، RTC، مؤقت النظام، ساعة الإيقاف
 * @brief (EN) Timer functions — PIT 8254, RTC, System Timer, Stopwatch
 */

#include "interpreter_core.h"
#include "value.h"
#include "timer.h"

#include <vector>
#include <memory>
#include <string>

namespace Sad {
namespace Interpreter {

void registerBuiltinsPart27(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // =================================================================
    // 1. PIT — المؤقت القابل للبرمجة
    // =================================================================

    // 1. timer_pit_init / مؤقت_تهيئة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int freq = args.size() > 0 ? args[0]->toInt() : 1000;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().pitInit(freq));
        };
        fm.registerBuiltinFunction("timer_pit_init", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xaa\xd9\x87\xd9\x8a\xd8\xa6\xd8\xa9", f);
    }

    // 2. timer_pit_freq / مؤقت_التردد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getPITFrequency());
        };
        fm.registerBuiltinFunction("timer_pit_freq", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb1\xd8\xaf\xd8\xaf", f);
    }

    // 3. timer_pit_count / مؤقت_العداد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(static_cast<int>(
                LowLevel::TimerManager::getInstance().getPITCount()));
        };
        fm.registerBuiltinFunction("timer_pit_count", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xa7\xd9\x84\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf", f);
    }

    // 4. timer_pit_set_freq / مؤقت_تعيين_تردد
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int freq = args.size() > 0 ? args[0]->toInt() : 1000;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().pitSetFrequency(freq));
        };
        fm.registerBuiltinFunction("timer_pit_set_freq", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd8\xaa\xd8\xb1\xd8\xaf\xd8\xaf", f);
    }

    // =================================================================
    // 2. RTC — ساعة الوقت الحقيقي
    // =================================================================

    // 5. timer_rtc_seconds / مؤقت_الثواني
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getRTCSeconds());
        };
        fm.registerBuiltinFunction("timer_rtc_seconds", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xa7\xd9\x84\xd8\xab\xd9\x88\xd8\xa7\xd9\x86\xd9\x8a", f);
    }

    // 6. timer_rtc_minutes / مؤقت_الدقائق
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getRTCMinutes());
        };
        fm.registerBuiltinFunction("timer_rtc_minutes", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xa7\xd9\x84\xd8\xaf\xd9\x82\xd8\xa7\xd8\xa6\xd9\x82", f);
    }

    // 7. timer_rtc_hours / مؤقت_الساعات
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getRTCHours());
        };
        fm.registerBuiltinFunction("timer_rtc_hours", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xa7\xd9\x84\xd8\xb3\xd8\xa7\xd8\xb9\xd8\xa7\xd8\xaa", f);
    }

    // 8. timer_rtc_day / مؤقت_اليوم
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getRTCDay());
        };
        fm.registerBuiltinFunction("timer_rtc_day", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xa7\xd9\x84\xd9\x8a\xd9\x88\xd9\x85", f);
    }

    // 9. timer_rtc_month / مؤقت_الشهر
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getRTCMonth());
        };
        fm.registerBuiltinFunction("timer_rtc_month", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xa7\xd9\x84\xd8\xb4\xd9\x87\xd8\xb1", f);
    }

    // 10. timer_rtc_year / مؤقت_السنة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getRTCYear());
        };
        fm.registerBuiltinFunction("timer_rtc_year", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xa7\xd9\x84\xd8\xb3\xd9\x86\xd8\xa9", f);
    }

    // 11. timer_rtc_time / مؤقت_الوقت
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getRTCTimeString());
        };
        fm.registerBuiltinFunction("timer_rtc_time", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xa7\xd9\x84\xd9\x88\xd9\x82\xd8\xaa", f);
    }

    // 12. timer_rtc_date / مؤقت_التاريخ
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getRTCDateString());
        };
        fm.registerBuiltinFunction("timer_rtc_date", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xae", f);
    }

    // 13. timer_rtc_alarm / مؤقت_منبه
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int h = args.size() > 0 ? args[0]->toInt() : 0;
            int m = args.size() > 1 ? args[1]->toInt() : 0;
            int s = args.size() > 2 ? args[2]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().rtcSetAlarm(h, m, s));
        };
        fm.registerBuiltinFunction("timer_rtc_alarm", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd9\x85\xd9\x86\xd8\xa8\xd9\x87", f);
    }

    // =================================================================
    // 3. مؤقت النظام / System Timer
    // =================================================================

    // 14. timer_ticks / مؤقت_نبضات
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(static_cast<int>(
                LowLevel::TimerManager::getInstance().getSystemTicks()));
        };
        fm.registerBuiltinFunction("timer_ticks", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd9\x86\xd8\xa8\xd8\xb6\xd8\xa7\xd8\xaa", f);
    }

    // 15. timer_uptime_ms / مؤقت_وقت_التشغيل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(static_cast<int>(
                LowLevel::TimerManager::getInstance().getUptimeMs()));
        };
        fm.registerBuiltinFunction("timer_uptime_ms", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd9\x88\xd9\x82\xd8\xaa_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84", f);
    }

    // 16. timer_uptime_sec / مؤقت_ثواني_التشغيل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(static_cast<int>(
                LowLevel::TimerManager::getInstance().getUptimeSeconds()));
        };
        fm.registerBuiltinFunction("timer_uptime_sec", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xab\xd9\x88\xd8\xa7\xd9\x86\xd9\x8a_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb4\xd8\xba\xd9\x8a\xd9\x84", f);
    }

    // 17. timer_sleep / مؤقت_انتظار
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int ms = args.size() > 0 ? args[0]->toInt() : 1;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().sleepMs(ms));
        };
        fm.registerBuiltinFunction("timer_sleep", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xa7\xd8\xb1", f);
    }

    // 18. timer_tick_rate / مؤقت_معدل_النبضات
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().getTickRate());
        };
        fm.registerBuiltinFunction("timer_tick_rate", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd9\x85\xd8\xb9\xd8\xaf\xd9\x84_\xd8\xa7\xd9\x84\xd9\x86\xd8\xa8\xd8\xb6\xd8\xa7\xd8\xaa", f);
    }

    // 19. timer_set_tick_rate / مؤقت_تعيين_معدل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int hz = args.size() > 0 ? args[0]->toInt() : 1000;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().setTickRate(hz));
        };
        fm.registerBuiltinFunction("timer_set_tick_rate", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86_\xd9\x85\xd8\xb9\xd8\xaf\xd9\x84", f);
    }

    // 20. timer_tick / مؤقت_نقرة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            LowLevel::TimerManager::getInstance().tick();
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction("timer_tick", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd9\x86\xd9\x82\xd8\xb1\xd8\xa9", f);
    }

    // =================================================================
    // 4. ساعة الإيقاف / Stopwatch
    // =================================================================

    // 21. timer_sw_create / مؤقت_ساعة_جديدة
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().createStopwatch());
        };
        fm.registerBuiltinFunction("timer_sw_create", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xb3\xd8\xa7\xd8\xb9\xd8\xa9_\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf\xd8\xa9", f);
    }

    // 22. timer_sw_start / مؤقت_ساعة_بدء
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().startStopwatch(id));
        };
        fm.registerBuiltinFunction("timer_sw_start", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xb3\xd8\xa7\xd8\xb9\xd8\xa9_\xd8\xa8\xd8\xaf\xd8\xa1", f);
    }

    // 23. timer_sw_stop / مؤقت_ساعة_ايقاف
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().stopStopwatch(id));
        };
        fm.registerBuiltinFunction("timer_sw_stop", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xb3\xd8\xa7\xd8\xb9\xd8\xa9_\xd8\xa7\xd9\x8a\xd9\x82\xd8\xa7\xd9\x81", f);
    }

    // 24. timer_sw_reset / مؤقت_ساعة_صفر
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().resetStopwatch(id));
        };
        fm.registerBuiltinFunction("timer_sw_reset", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xb3\xd8\xa7\xd8\xb9\xd8\xa9_\xd8\xb5\xd9\x81\xd8\xb1", f);
    }

    // 25. timer_sw_ms / مؤقت_ساعة_ملي
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(static_cast<int>(
                LowLevel::TimerManager::getInstance().getStopwatchMs(id)));
        };
        fm.registerBuiltinFunction("timer_sw_ms", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xb3\xd8\xa7\xd8\xb9\xd8\xa9_\xd9\x85\xd9\x84\xd9\x8a", f);
    }

    // 26. timer_sw_running / مؤقت_ساعة_تعمل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            int id = args.size() > 0 ? args[0]->toInt() : 0;
            return std::make_shared<Data::Value>(
                LowLevel::TimerManager::getInstance().isStopwatchRunning(id) ? 1 : 0);
        };
        fm.registerBuiltinFunction("timer_sw_running", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xb3\xd8\xa7\xd8\xb9\xd8\xa9_\xd8\xaa\xd8\xb9\xd9\x85\xd9\x84", f);
    }

    // =================================================================
    // 5. تقرير وإعادة تعيين / Report & Reset
    // =================================================================

    // 27. timer_report / مؤقت_تقرير
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            return std::make_shared<Data::Value>(LowLevel::TimerManager::getInstance().generateReport());
        };
        fm.registerBuiltinFunction("timer_report", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xaa\xd9\x82\xd8\xb1\xd9\x8a\xd8\xb1", f);
    }

    // 28. timer_reset / مؤقت_اعادة_تعيين
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            LowLevel::TimerManager::getInstance().reset();
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction("timer_reset", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd8\xa7\xd8\xb9\xd8\xa7\xd8\xaf\xd8\xa9_\xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86", f);
    }

    // 29. timer_rtc_read / مؤقت_قراءة_rtc
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
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
        fm.registerBuiltinFunction("timer_rtc_read", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd9\x82\xd8\xb1\xd8\xa7\xd8\xa1\xd8\xa9_rtc", f);
    }

    // 30. timer_pit_enabled / مؤقت_مفعل
    {
        auto f = [](const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            (void)args;
            // (AR) PIT مفعل إذا كان التردد > 0 / (EN) PIT is enabled if freq > 0
            return std::make_shared<Data::Value>(
                LowLevel::TimerManager::getInstance().getPITFrequency() > 0 ? 1 : 0);
        };
        fm.registerBuiltinFunction("timer_pit_enabled", f);
        fm.registerBuiltinFunction("\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa_\xd9\x85\xd9\x81\xd8\xb9\xd9\x84", f);
    }

} // registerBuiltinsPart27

} // namespace Interpreter
} // namespace Sad
