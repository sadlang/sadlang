/*
 * ============================================================================
 *  وحدة المؤقتات — PIT / RTC / مؤقت النظام — لغة ص
 *  Timer Module — PIT / RTC / System Timer — Sad Language
 * ============================================================================
 *
 * @brief (AR) وحدة إدارة المؤقتات بدون نظام تشغيل
 *             المؤقت القابل للبرمجة (PIT 8254)، ساعة الوقت الحقيقي (RTC CMOS)،
 *             مؤقت النظام العام (System Tick)
 *
 * @brief (EN) Bare-metal timer management module
 *             PIT 8254, RTC CMOS, and System Tick timer
 *
 * @details
 * (AR) توفر هذه الوحدة واجهة موحدة للمؤقتات. على سطح المكتب: محاكاة.
 *      في النواة: وصول مباشر لمنافذ I/O.
 *
 *      المؤقتات المدعومة:
 *        1. PIT (8254) — مؤقت قابل للبرمجة بتردد ~1.193 MHz
 *        2. RTC (CMOS) — ساعة الوقت الحقيقي مع تاريخ ووقت
 *        3. System Tick — عدّاد دقات النظام (uptime)
 *        4. Stopwatch — ساعة إيقاف للقياس
 *
 * (EN) Unified timer API. Simulated on desktop, direct I/O on bare-metal.
 *      PIT, RTC, System Tick, and Stopwatch support.
 * ============================================================================
 */

#ifndef SAD_LOW_LEVEL_TIMER_H
#define SAD_LOW_LEVEL_TIMER_H

#include <cstdint>
#include <string>
#include <vector>

namespace Sad {
namespace LowLevel {

// ============================================================================
// (AR) ثوابت المؤقتات / (EN) Timer Constants
// ============================================================================
namespace TimerConstants {
    // (AR) تردد PIT الأساسي / (EN) PIT base frequency
    constexpr uint32_t PIT_BASE_FREQ = 1193182;
    // (AR) منفذ PIT / (EN) PIT I/O ports
    constexpr uint16_t PIT_CHANNEL0 = 0x40;
    constexpr uint16_t PIT_CHANNEL2 = 0x42;
    constexpr uint16_t PIT_COMMAND = 0x43;
    // (AR) منافذ RTC / (EN) RTC I/O ports
    constexpr uint16_t RTC_ADDRESS = 0x70;
    constexpr uint16_t RTC_DATA    = 0x71;
    // (AR) سجلات RTC / (EN) RTC registers
    constexpr uint8_t RTC_SECONDS  = 0x00;
    constexpr uint8_t RTC_MINUTES  = 0x01;
    constexpr uint8_t RTC_HOURS    = 0x02;
    constexpr uint8_t RTC_DAY      = 0x07;
    constexpr uint8_t RTC_MONTH    = 0x08;
    constexpr uint8_t RTC_YEAR     = 0x09;
    constexpr uint8_t RTC_STATUS_A = 0x0A;
    constexpr uint8_t RTC_STATUS_B = 0x0B;
    // (AR) معدل النبضات الافتراضي / (EN) Default tick rate
    constexpr int DEFAULT_TICK_RATE = 1000; // (AR) 1000 Hz
}

// ============================================================================
// (AR) تاريخ ووقت RTC / (EN) RTC Date/Time
// ============================================================================
struct RTCDateTime {
    int year;      // (AR) السنة (2000-2099) / (EN) Year
    int month;     // (AR) الشهر (1-12) / (EN) Month
    int day;       // (AR) اليوم (1-31) / (EN) Day
    int hours;     // (AR) الساعات (0-23) / (EN) Hours
    int minutes;   // (AR) الدقائق (0-59) / (EN) Minutes
    int seconds;   // (AR) الثواني (0-59) / (EN) Seconds
    int dayOfWeek; // (AR) يوم الأسبوع (0=أحد) / (EN) Day of week (0=Sun)
};

// ============================================================================
// (AR) معلومات ساعة الإيقاف / (EN) Stopwatch Info
// ============================================================================
struct StopwatchInfo {
    int id;
    bool running;
    uint64_t startTick;
    uint64_t elapsedMs;
};

// ============================================================================
// (AR) مدير المؤقتات — نمط المفرد / (EN) Timer Manager — Singleton
// ============================================================================
class TimerManager {
public:
    static TimerManager& getInstance() {
        static TimerManager instance;
        return instance;
    }

    // ════════════════════════════════════════════════════════════════
    // (AR) 1. PIT — المؤقت القابل للبرمجة / (EN) 1. PIT
    // ════════════════════════════════════════════════════════════════
    int pitInit(int freqHz);
    int getPITFrequency() const { return pitFreq_; }
    uint64_t getPITCount() const { return pitTicks_; }
    int pitSetFrequency(int freqHz);

    // ════════════════════════════════════════════════════════════════
    // (AR) 2. RTC — ساعة الوقت الحقيقي / (EN) 2. RTC
    // ════════════════════════════════════════════════════════════════
    RTCDateTime rtcRead();
    int getRTCSeconds() const;
    int getRTCMinutes() const;
    int getRTCHours() const;
    int getRTCDay() const;
    int getRTCMonth() const;
    int getRTCYear() const;
    std::string getRTCTimeString() const;
    std::string getRTCDateString() const;
    int rtcSetAlarm(int hours, int minutes, int seconds);

    // ════════════════════════════════════════════════════════════════
    // (AR) 3. مؤقت النظام / (EN) 3. System Timer
    // ════════════════════════════════════════════════════════════════
    uint64_t getSystemTicks() const { return sysTicks_; }
    uint64_t getUptimeMs() const;
    uint64_t getUptimeSeconds() const;
    int sleepMs(int ms);
    int setTickRate(int hz);
    int getTickRate() const { return tickRate_; }

    // ════════════════════════════════════════════════════════════════
    // (AR) 4. ساعة الإيقاف / (EN) 4. Stopwatch
    // ════════════════════════════════════════════════════════════════
    int createStopwatch();
    int startStopwatch(int swId);
    int stopStopwatch(int swId);
    int resetStopwatch(int swId);
    uint64_t getStopwatchMs(int swId) const;
    bool isStopwatchRunning(int swId) const;

    // ════════════════════════════════════════════════════════════════
    // (AR) 5. تقرير وإعادة تعيين / (EN) 5. Report & Reset
    // ════════════════════════════════════════════════════════════════
    std::string generateReport() const;
    void tick(); // (AR) استدعاء من IRQ0 / (EN) Called from IRQ0 handler
    void reset();

private:
    TimerManager();
    ~TimerManager() = default;
    TimerManager(const TimerManager&) = delete;
    TimerManager& operator=(const TimerManager&) = delete;

    // (AR) قراءة RTC ثابتة / (EN) Const RTC read helper
    RTCDateTime rtcReadHelper() const;

    int pitFreq_;
    uint64_t pitTicks_;
    uint64_t sysTicks_;
    int tickRate_;
    std::vector<StopwatchInfo> stopwatches_;
};

} // namespace LowLevel
} // namespace Sad

#endif // SAD_LOW_LEVEL_TIMER_H
