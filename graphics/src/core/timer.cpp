// ==============================================================================
// timer.cpp - تنفيذ نظام المؤقتات
// Timer System Implementation
// ==============================================================================

#include "../../include/core/timer.h"
#include <algorithm>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#undef GetTickCount
#undef GetCurrentTime
#else
#include <chrono>
#endif

namespace sad {
namespace graphics {

// ==============================================================================
// Timer
// ==============================================================================

Timer::Timer()
    : m_startTime(0.0)
    , m_lastFrameTime(0.0)
    , m_totalTime(0.0)
    , m_deltaTime(0.0f)
    , m_maxDeltaTime(0.1f)   // الحد الأقصى 100ms / Max 100ms
    , m_timeScale(1.0f)
    , m_fps(0.0f)
    , m_smoothedFps(60.0f)
    , m_frameCount(0)
    , m_paused(false)
{
}

void Timer::Start() {
    m_startTime = GetCurrentTime();
    m_lastFrameTime = m_startTime;
    m_totalTime = 0.0;
    m_deltaTime = 0.0f;
    m_frameCount = 0;
    m_paused = false;
}

void Timer::Pause() {
    m_paused = true;
}

void Timer::Resume() {
    if (m_paused) {
        m_paused = false;
        m_lastFrameTime = GetCurrentTime();
    }
}

void Timer::Reset() {
    m_startTime = GetCurrentTime();
    m_lastFrameTime = m_startTime;
    m_totalTime = 0.0;
    m_deltaTime = 0.0f;
    m_frameCount = 0;
}

void Timer::Update() {
    Float64 currentTime = GetCurrentTime();
    
    if (m_paused) {
        m_deltaTime = 0.0f;
        m_lastFrameTime = currentTime;
        return;
    }
    
    Float64 rawDelta = currentTime - m_lastFrameTime;
    m_lastFrameTime = currentTime;
    
    // تقييد delta time / Clamp delta time
    m_deltaTime = static_cast<Float32>(rawDelta);
    if (m_deltaTime > m_maxDeltaTime) m_deltaTime = m_maxDeltaTime;
    if (m_deltaTime < 0.0f) m_deltaTime = 0.0f;
    
    m_totalTime += m_deltaTime;
    m_frameCount++;
    
    // حساب FPS / Calculate FPS
    if (m_deltaTime > 0.0f) {
        m_fps = 1.0f / m_deltaTime;
        // تنعيم FPS (متوسط متحرك أسي) / Smooth FPS (exponential moving average)
        m_smoothedFps = m_smoothedFps * 0.95f + m_fps * 0.05f;
    }
}

Float64 Timer::GetElapsed() const {
    return GetCurrentTime() - m_startTime;
}

Float64 Timer::GetCurrentTime() {
#ifdef _WIN32
    static LARGE_INTEGER frequency{};
    static bool initialized = false;
    if (!initialized) {
        QueryPerformanceFrequency(&frequency);
        initialized = true;
    }
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return static_cast<Float64>(counter.QuadPart) / frequency.QuadPart;
#else
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<Float64>(duration).count();
#endif
}

// ==============================================================================
// TimerManager
// ==============================================================================

TimerManager::TimerManager() : m_nextId(1) {}

TimerManager& TimerManager::GetInstance() {
    static TimerManager instance;
    return instance;
}

UInt32 TimerManager::SetTimeout(Float32 delay, std::function<void()> callback) {
    ScheduledEvent event;
    event.id = m_nextId++;
    event.interval = delay;
    event.elapsed = 0.0f;
    event.repeatCount = 0;
    event.currentRepeat = 0;
    event.active = true;
    event.callback = callback;
    m_events.push_back(event);
    return event.id;
}

UInt32 TimerManager::SetInterval(Float32 interval, std::function<void()> callback,
                                  Int32 repeatCount) {
    ScheduledEvent event;
    event.id = m_nextId++;
    event.interval = interval;
    event.elapsed = 0.0f;
    event.repeatCount = repeatCount;
    event.currentRepeat = 0;
    event.active = true;
    event.callback = callback;
    m_events.push_back(event);
    return event.id;
}

void TimerManager::CancelEvent(UInt32 id) {
    for (auto& e : m_events) {
        if (e.id == id) {
            e.active = false;
            break;
        }
    }
}

void TimerManager::CancelAll() {
    for (auto& e : m_events) e.active = false;
}

void TimerManager::Update(Float32 deltaTime) {
    for (auto& event : m_events) {
        if (!event.active) continue;
        
        event.elapsed += deltaTime;
        
        if (event.elapsed >= event.interval) {
            event.elapsed -= event.interval;
            
            if (event.callback) {
                event.callback();
            }
            
            if (event.repeatCount == 0) {
                // مرة واحدة / One-shot
                event.active = false;
            } else if (event.repeatCount > 0) {
                event.currentRepeat++;
                if (event.currentRepeat >= event.repeatCount) {
                    event.active = false;
                }
            }
            // repeatCount == -1: لانهائي / Infinite
        }
    }
    
    // تنظيف غير النشطة / Cleanup inactive
    m_events.erase(
        std::remove_if(m_events.begin(), m_events.end(),
            [](const ScheduledEvent& e) { return !e.active; }),
        m_events.end()
    );
}

Int32 TimerManager::GetActiveCount() const {
    Int32 count = 0;
    for (const auto& e : m_events) {
        if (e.active) ++count;
    }
    return count;
}

} // namespace graphics
} // namespace sad
