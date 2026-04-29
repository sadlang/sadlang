/**
 * @file timer.cpp
 * @brief (AR) تنفيذ مدير المؤقتات — PIT، RTC، مؤقت النظام، ساعة الإيقاف
 *        محاكاة آمنة على سطح المكتب
 * @brief (EN) Timer Manager implementation — PIT, RTC, System Timer, Stopwatch
 *        Safe simulation on desktop
 */

#include "timer.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int

namespace Sad {
namespace LowLevel {

// ============================================================================
// (AR) المُنشئ / (EN) Constructor
// ============================================================================
TimerManager::TimerManager()
    : pitFreq_(0)
    , pitTicks_(0)
    , sysTicks_(0)
    , tickRate_(TimerConstants::DEFAULT_TICK_RATE)
{}

// ============================================================================
// (AR) 1. PIT / (EN) 1. PIT
// ============================================================================

int TimerManager::pitInit(int freqHz) {
    if (freqHz <= 0) freqHz = 1000;
    if (freqHz > static_cast<int>(TimerConstants::PIT_BASE_FREQ))
        freqHz = static_cast<int>(TimerConstants::PIT_BASE_FREQ);
    pitFreq_ = freqHz;
    pitTicks_ = 0;
    return 0;
}

int TimerManager::pitSetFrequency(int freqHz) {
    return pitInit(freqHz);
}

// ============================================================================
// (AR) 2. RTC / (EN) 2. RTC
// ============================================================================

RTCDateTime TimerManager::rtcRead() {
    return rtcReadHelper();
}

RTCDateTime TimerManager::rtcReadHelper() const {
    RTCDateTime dt;
    // (AR) استخدام وقت النظام الفعلي / (EN) Use actual system time
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &tt);
#else
    localtime_r(&tt, &tm);
#endif
    dt.year = tm.tm_year + 1900;
    dt.month = tm.tm_mon + 1;
    dt.day = tm.tm_mday;
    dt.hours = tm.tm_hour;
    dt.minutes = tm.tm_min;
    dt.seconds = tm.tm_sec;
    dt.dayOfWeek = tm.tm_wday;
    return dt;
}

int TimerManager::getRTCSeconds() const { return rtcReadHelper().seconds; }
int TimerManager::getRTCMinutes() const { return rtcReadHelper().minutes; }
int TimerManager::getRTCHours() const   { return rtcReadHelper().hours; }
int TimerManager::getRTCDay() const     { return rtcReadHelper().day; }
int TimerManager::getRTCMonth() const   { return rtcReadHelper().month; }
int TimerManager::getRTCYear() const    { return rtcReadHelper().year; }

std::string TimerManager::getRTCTimeString() const {
    auto dt = rtcReadHelper();
    std::ostringstream ss;
    ss << std::setw(2) << std::setfill('0') << dt.hours << ":"
       << std::setw(2) << std::setfill('0') << dt.minutes << ":"
       << std::setw(2) << std::setfill('0') << dt.seconds;
    return ss.str();
}

std::string TimerManager::getRTCDateString() const {
    auto dt = rtcReadHelper();
    std::ostringstream ss;
    ss << dt.year << "-"
       << std::setw(2) << std::setfill('0') << dt.month << "-"
       << std::setw(2) << std::setfill('0') << dt.day;
    return ss.str();
}

int TimerManager::rtcSetAlarm(int hours, int minutes, int seconds) {
    (void)hours; (void)minutes; (void)seconds;
    // (AR) محاكاة — لا إنذار حقيقي / (EN) Simulation — no real alarm
    return 0;
}

// ============================================================================
// (AR) 3. مؤقت النظام / (EN) 3. System Timer
// ============================================================================

uint64_t TimerManager::getUptimeMs() const {
    if (tickRate_ <= 0) return 0;
    return (sysTicks_ * 1000ULL) / static_cast<uint64_t>(tickRate_);
}

uint64_t TimerManager::getUptimeSeconds() const {
    return getUptimeMs() / 1000;
}

int TimerManager::sleepMs(int ms) {
    // (AR) محاكاة النوم — يزيد العدّاد / (EN) Simulated sleep — increments counter
    if (ms <= 0) return 0;
    uint64_t ticks = (static_cast<uint64_t>(ms) * static_cast<uint64_t>(tickRate_)) / 1000;
    sysTicks_ += ticks;
    pitTicks_ += ticks;
    return 0;
}

int TimerManager::setTickRate(int hz) {
    if (hz <= 0) hz = 1;
    if (hz > 10000) hz = 10000;
    tickRate_ = hz;
    return 0;
}

// ============================================================================
// (AR) 4. ساعة الإيقاف / (EN) 4. Stopwatch
// ============================================================================

int TimerManager::createStopwatch() {
    StopwatchInfo sw;
    sw.id = Sad::Security::SafeArithmetic::assertSafeCast<int>(stopwatches_.size(), "timer_size");
    sw.running = false;
    sw.startTick = 0;
    sw.elapsedMs = 0;
    stopwatches_.push_back(sw);
    return sw.id;
}

int TimerManager::startStopwatch(int swId) {
    if (swId < 0 || swId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(stopwatches_.size(), "timer_size")) return -1;
    stopwatches_[swId].running = true;
    stopwatches_[swId].startTick = sysTicks_;
    return 0;
}

int TimerManager::stopStopwatch(int swId) {
    if (swId < 0 || swId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(stopwatches_.size(), "timer_size")) return -1;
    if (stopwatches_[swId].running) {
        uint64_t elapsed = sysTicks_ - stopwatches_[swId].startTick;
        if (tickRate_ > 0)
            stopwatches_[swId].elapsedMs += (elapsed * 1000) / static_cast<uint64_t>(tickRate_);
        stopwatches_[swId].running = false;
    }
    return 0;
}

int TimerManager::resetStopwatch(int swId) {
    if (swId < 0 || swId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(stopwatches_.size(), "timer_size")) return -1;
    stopwatches_[swId].running = false;
    stopwatches_[swId].startTick = 0;
    stopwatches_[swId].elapsedMs = 0;
    return 0;
}

uint64_t TimerManager::getStopwatchMs(int swId) const {
    if (swId < 0 || swId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(stopwatches_.size(), "timer_size")) return 0;
    uint64_t total = stopwatches_[swId].elapsedMs;
    if (stopwatches_[swId].running && tickRate_ > 0) {
        uint64_t running = sysTicks_ - stopwatches_[swId].startTick;
        total += (running * 1000) / static_cast<uint64_t>(tickRate_);
    }
    return total;
}

bool TimerManager::isStopwatchRunning(int swId) const {
    if (swId < 0 || swId >= Sad::Security::SafeArithmetic::assertSafeCast<int>(stopwatches_.size(), "timer_size")) return false;
    return stopwatches_[swId].running;
}

// ============================================================================
// (AR) 5. tick و تقرير / (EN) 5. Tick & Report
// ============================================================================

void TimerManager::tick() {
    sysTicks_++;
    pitTicks_++;
}

std::string TimerManager::generateReport() const {
    std::ostringstream ss;
    ss << "=== \xd8\xaa\xd9\x82\xd8\xb1\xd9\x8a\xd8\xb1 \xd8\xa7\xd9\x84\xd9\x85\xd8\xa4\xd9\x82\xd8\xaa\xd8\xa7\xd8\xaa / Timer Report ===\n";
    ss << "PIT: " << pitFreq_ << " Hz, ticks=" << pitTicks_ << "\n";
    ss << "System: rate=" << tickRate_ << " Hz, ticks=" << sysTicks_
       << ", uptime=" << getUptimeMs() << " ms\n";
    ss << "Stopwatches: " << stopwatches_.size() << "\n";
    return ss.str();
}

void TimerManager::reset() {
    pitFreq_ = 0;
    pitTicks_ = 0;
    sysTicks_ = 0;
    tickRate_ = TimerConstants::DEFAULT_TICK_RATE;
    stopwatches_.clear();
}

} // namespace LowLevel
} // namespace Sad
