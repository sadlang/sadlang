// ==============================================================================
// timer.h - نظام المؤقتات / Timer System
// ==============================================================================
// الوصف: مؤقتات دقيقة للألعاب مع دعم delta time والتكرار
// Description: Precise game timers with delta time and repeat support
// ==============================================================================

#pragma once

#include "../core/types.h"
#include <functional>
#include <vector>
#include <memory>

namespace sad {
namespace graphics {

// ==============================================================================
// مؤقت عالي الدقة / High-precision Timer
// ==============================================================================
class Timer {
public:
    Timer();
    ~Timer() = default;
    
    /// بدء / إعادة تعيين المؤقت / Start / Reset timer
    void Start();
    
    /// إيقاف مؤقت / Pause
    void Pause();
    
    /// استئناف / Resume
    void Resume();
    
    /// إعادة تعيين / Reset
    void Reset();
    
    /// تحديث (يُستدعى كل إطار) / Update (called each frame)
    void Update();
    
    /// الوقت المنقضي منذ البداية (بالثواني) / Elapsed time since start (seconds)
    Float64 GetElapsed() const;
    
    /// الوقت بين الإطارات (بالثواني) / Delta time between frames (seconds)
    Float32 GetDeltaTime() const { return m_deltaTime; }
    
    /// عدد الإطارات في الثانية / Frames per second
    Float32 GetFPS() const { return m_fps; }
    
    /// عدد الإطارات في الثانية (متوسط ناعم) / Smoothed FPS
    Float32 GetSmoothedFPS() const { return m_smoothedFps; }
    
    /// هل مُتوقف / Is paused
    bool IsPaused() const { return m_paused; }
    
    /// عدد الإطارات الكلي / Total frame count
    UInt64 GetFrameCount() const { return m_frameCount; }
    
    /// الوقت الإجمالي (متأثر بالإيقاف) / Total time (affected by pause)
    Float64 GetTotalTime() const { return m_totalTime; }
    
    /// تعيين الحد الأقصى لـ delta time (لمنع القفزات)
    /// Set max delta time (to prevent jumps)
    void SetMaxDeltaTime(Float32 maxDT) { m_maxDeltaTime = maxDT; }
    
    /// تعيين عامل سرعة الوقت / Set time scale factor
    void SetTimeScale(Float32 scale) { m_timeScale = scale; }
    Float32 GetTimeScale() const { return m_timeScale; }
    
    /// Delta time مع عامل السرعة / Scaled delta time
    Float32 GetScaledDeltaTime() const { return m_deltaTime * m_timeScale; }

private:
    Float64 m_startTime;
    Float64 m_lastFrameTime;
    Float64 m_totalTime;
    Float32 m_deltaTime;
    Float32 m_maxDeltaTime;
    Float32 m_timeScale;
    Float32 m_fps;
    Float32 m_smoothedFps;
    UInt64 m_frameCount;
    bool m_paused;
    
    // للحصول على الوقت الحالي / Get current time
    static Float64 GetCurrentTime();
};

// ==============================================================================
// مؤقت مؤجل / Scheduled Timer
// ==============================================================================
struct ScheduledEvent {
    UInt32 id;
    Float32 interval;           // الفاصل الزمني بالثواني / Interval in seconds
    Float32 elapsed;            // الوقت المنقضي / Elapsed time
    Int32 repeatCount;          // عدد التكرارات (-1 = لانهائي, 0 = مرة واحدة) / Repeat count
    Int32 currentRepeat;        // التكرار الحالي / Current repeat
    bool active;                // نشط / Active
    std::function<void()> callback; // الدالة المُنفذة / Callback function
};

// ==============================================================================
// مدير المؤقتات المُجدولة / Scheduled Timer Manager
// ==============================================================================
class TimerManager {
public:
    TimerManager();
    ~TimerManager() = default;
    
    /// جدولة حدث بعد فترة / Schedule event after delay
    /// delay: التأخير بالثواني / Delay in seconds
    /// callback: الدالة المُنفذة / Callback
    /// returns: معرّف الحدث / Event ID
    UInt32 SetTimeout(Float32 delay, std::function<void()> callback);
    
    /// جدولة حدث متكرر / Schedule repeating event
    /// interval: الفاصل الزمني / Interval
    /// callback: الدالة المُنفذة / Callback
    /// repeatCount: عدد التكرارات (-1 = لانهائي) / Repeat count
    UInt32 SetInterval(Float32 interval, std::function<void()> callback,
                       Int32 repeatCount = -1);
    
    /// إلغاء حدث / Cancel event
    void CancelEvent(UInt32 id);
    
    /// إلغاء الكل / Cancel all
    void CancelAll();
    
    /// تحديث / Update
    /// deltaTime: وقت الإطار / Frame time
    void Update(Float32 deltaTime);
    
    /// عدد الأحداث النشطة / Active event count
    Int32 GetActiveCount() const;
    
    /// المدير العام / Global instance
    static TimerManager& GetInstance();

private:
    std::vector<ScheduledEvent> m_events;
    UInt32 m_nextId;
};

} // namespace graphics
} // namespace sad

// اختصار / Shortcut
namespace SadGraphics {
    using Timer = sad::graphics::Timer;
    using TimerManager = sad::graphics::TimerManager;
}
