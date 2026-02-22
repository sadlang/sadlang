// ==============================================================================
// tween.cpp - تنفيذ نظام التحريك السلس
// Tween & Easing System Implementation
// ==============================================================================

#include "../../include/core/tween.h"
#include <cmath>
#include <algorithm>

namespace sad {
namespace graphics {

// ==============================================================================
// ثوابت رياضية / Math constants
// ==============================================================================
static constexpr Float32 PI = 3.14159265358979323846f;
static constexpr Float32 HALF_PI = PI * 0.5f;
static constexpr Float32 TWO_PI = PI * 2.0f;

// ==============================================================================
// دوال التسهيل / Easing Functions
// ==============================================================================

namespace Ease {

Float32 Linear(Float32 t) { return t; }

Float32 InQuad(Float32 t) { return t * t; }
Float32 OutQuad(Float32 t) { return t * (2.0f - t); }
Float32 InOutQuad(Float32 t) {
    return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
}

Float32 InCubic(Float32 t) { return t * t * t; }
Float32 OutCubic(Float32 t) { Float32 t1 = t - 1.0f; return t1 * t1 * t1 + 1.0f; }
Float32 InOutCubic(Float32 t) {
    return t < 0.5f ? 4.0f * t * t * t : (t - 1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f;
}

Float32 InQuart(Float32 t) { return t * t * t * t; }
Float32 OutQuart(Float32 t) { Float32 t1 = t - 1.0f; return 1.0f - t1 * t1 * t1 * t1; }
Float32 InOutQuart(Float32 t) {
    Float32 t1 = t - 1.0f;
    return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - 8.0f * t1 * t1 * t1 * t1;
}

Float32 InQuint(Float32 t) { return t * t * t * t * t; }
Float32 OutQuint(Float32 t) { Float32 t1 = t - 1.0f; return 1.0f + t1 * t1 * t1 * t1 * t1; }
Float32 InOutQuint(Float32 t) {
    Float32 t1 = t - 1.0f;
    return t < 0.5f ? 16.0f * t * t * t * t * t : 1.0f + 16.0f * t1 * t1 * t1 * t1 * t1;
}

Float32 InSine(Float32 t) { return 1.0f - std::cos(t * HALF_PI); }
Float32 OutSine(Float32 t) { return std::sin(t * HALF_PI); }
Float32 InOutSine(Float32 t) { return 0.5f * (1.0f - std::cos(PI * t)); }

Float32 InExpo(Float32 t) { return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * (t - 1.0f)); }
Float32 OutExpo(Float32 t) { return t == 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t); }
Float32 InOutExpo(Float32 t) {
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    return t < 0.5f
        ? 0.5f * std::pow(2.0f, 20.0f * t - 10.0f)
        : 1.0f - 0.5f * std::pow(2.0f, -20.0f * t + 10.0f);
}

Float32 InCirc(Float32 t) { return 1.0f - std::sqrt(1.0f - t * t); }
Float32 OutCirc(Float32 t) { Float32 t1 = t - 1.0f; return std::sqrt(1.0f - t1 * t1); }
Float32 InOutCirc(Float32 t) {
    return t < 0.5f
        ? 0.5f * (1.0f - std::sqrt(1.0f - 4.0f * t * t))
        : 0.5f * (std::sqrt(1.0f - (2.0f * t - 2.0f) * (2.0f * t - 2.0f)) + 1.0f);
}

Float32 InBack(Float32 t) {
    constexpr Float32 s = 1.70158f;
    return t * t * ((s + 1.0f) * t - s);
}
Float32 OutBack(Float32 t) {
    constexpr Float32 s = 1.70158f;
    Float32 t1 = t - 1.0f;
    return t1 * t1 * ((s + 1.0f) * t1 + s) + 1.0f;
}
Float32 InOutBack(Float32 t) {
    constexpr Float32 s = 1.70158f * 1.525f;
    Float32 t2 = t * 2.0f;
    if (t2 < 1.0f) return 0.5f * (t2 * t2 * ((s + 1.0f) * t2 - s));
    Float32 t2m2 = t2 - 2.0f;
    return 0.5f * (t2m2 * t2m2 * ((s + 1.0f) * t2m2 + s) + 2.0f);
}

Float32 InElastic(Float32 t) {
    if (t == 0.0f || t == 1.0f) return t;
    return -std::pow(2.0f, 10.0f * (t - 1.0f)) * std::sin((t - 1.075f) * TWO_PI / 0.3f);
}
Float32 OutElastic(Float32 t) {
    if (t == 0.0f || t == 1.0f) return t;
    return std::pow(2.0f, -10.0f * t) * std::sin((t - 0.075f) * TWO_PI / 0.3f) + 1.0f;
}
Float32 InOutElastic(Float32 t) {
    if (t == 0.0f || t == 1.0f) return t;
    Float32 t2 = t * 2.0f;
    if (t2 < 1.0f)
        return -0.5f * std::pow(2.0f, 10.0f * (t2 - 1.0f)) * std::sin((t2 - 1.1f) * TWO_PI / 0.4f);
    return std::pow(2.0f, -10.0f * (t2 - 1.0f)) * std::sin((t2 - 1.1f) * TWO_PI / 0.4f) * 0.5f + 1.0f;
}

Float32 OutBounce(Float32 t) {
    if (t < 1.0f / 2.75f)
        return 7.5625f * t * t;
    else if (t < 2.0f / 2.75f) {
        Float32 t1 = t - 1.5f / 2.75f;
        return 7.5625f * t1 * t1 + 0.75f;
    } else if (t < 2.5f / 2.75f) {
        Float32 t1 = t - 2.25f / 2.75f;
        return 7.5625f * t1 * t1 + 0.9375f;
    } else {
        Float32 t1 = t - 2.625f / 2.75f;
        return 7.5625f * t1 * t1 + 0.984375f;
    }
}
Float32 InBounce(Float32 t) { return 1.0f - OutBounce(1.0f - t); }
Float32 InOutBounce(Float32 t) {
    return t < 0.5f
        ? 0.5f * InBounce(t * 2.0f)
        : 0.5f * OutBounce(t * 2.0f - 1.0f) + 0.5f;
}

Float32 Evaluate(EaseType type, Float32 t) {
    // ضبط t في [0, 1] / Clamp t to [0, 1]
    t = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);
    
    switch (type) {
        case EaseType::Linear:         return Linear(t);
        case EaseType::EaseInQuad:     return InQuad(t);
        case EaseType::EaseOutQuad:    return OutQuad(t);
        case EaseType::EaseInOutQuad:  return InOutQuad(t);
        case EaseType::EaseInCubic:    return InCubic(t);
        case EaseType::EaseOutCubic:   return OutCubic(t);
        case EaseType::EaseInOutCubic: return InOutCubic(t);
        case EaseType::EaseInQuart:    return InQuart(t);
        case EaseType::EaseOutQuart:   return OutQuart(t);
        case EaseType::EaseInOutQuart: return InOutQuart(t);
        case EaseType::EaseInQuint:    return InQuint(t);
        case EaseType::EaseOutQuint:   return OutQuint(t);
        case EaseType::EaseInOutQuint: return InOutQuint(t);
        case EaseType::EaseInSine:     return InSine(t);
        case EaseType::EaseOutSine:    return OutSine(t);
        case EaseType::EaseInOutSine:  return InOutSine(t);
        case EaseType::EaseInExpo:     return InExpo(t);
        case EaseType::EaseOutExpo:    return OutExpo(t);
        case EaseType::EaseInOutExpo:  return InOutExpo(t);
        case EaseType::EaseInCirc:     return InCirc(t);
        case EaseType::EaseOutCirc:    return OutCirc(t);
        case EaseType::EaseInOutCirc:  return InOutCirc(t);
        case EaseType::EaseInBack:     return InBack(t);
        case EaseType::EaseOutBack:    return OutBack(t);
        case EaseType::EaseInOutBack:  return InOutBack(t);
        case EaseType::EaseInElastic:  return InElastic(t);
        case EaseType::EaseOutElastic: return OutElastic(t);
        case EaseType::EaseInOutElastic: return InOutElastic(t);
        case EaseType::EaseInBounce:   return InBounce(t);
        case EaseType::EaseOutBounce:  return OutBounce(t);
        case EaseType::EaseInOutBounce: return InOutBounce(t);
        default:                       return t;
    }
}

} // namespace Ease

// ==============================================================================
// TweenManager - التنفيذ / Implementation
// ==============================================================================

TweenManager::TweenManager() : m_nextId(1) {}

TweenManager& TweenManager::GetInstance() {
    static TweenManager instance;
    return instance;
}

UInt32 TweenManager::To(Float32* target, Float32 endValue, Float32 duration,
                         EaseType easeType) {
    return To(target, endValue, duration, 0.0f, easeType);
}

UInt32 TweenManager::To(Float32* target, Float32 endValue, Float32 duration,
                         Float32 delay, EaseType easeType) {
    Tween tween;
    tween.id = m_nextId++;
    tween.target = target;
    tween.startValue = target ? *target : 0.0f;
    tween.endValue = endValue;
    tween.duration = duration;
    tween.delay = delay;
    tween.elapsed = 0.0f;
    tween.easeType = easeType;
    tween.state = (delay > 0.0f) ? TweenState::Pending : TweenState::Running;
    tween.repeatCount = 0;
    tween.currentRepeat = 0;
    tween.yoyo = false;
    tween.reversed = false;
    
    m_tweens.push_back(tween);
    return tween.id;
}

void TweenManager::TweenPosition(Float32* x, Float32* y, Float32 endX, Float32 endY,
                                   Float32 duration, EaseType easeType) {
    To(x, endX, duration, easeType);
    To(y, endY, duration, easeType);
}

void TweenManager::SetRepeat(UInt32 id, Int32 count, bool yoyo) {
    Tween* t = FindTween(id);
    if (t) {
        t->repeatCount = count;
        t->yoyo = yoyo;
    }
}

void TweenManager::OnComplete(UInt32 id, std::function<void()> callback) {
    Tween* t = FindTween(id);
    if (t) t->onComplete = callback;
}

void TweenManager::OnUpdate(UInt32 id, std::function<void(Float32)> callback) {
    Tween* t = FindTween(id);
    if (t) t->onUpdate = callback;
}

void TweenManager::Update(Float32 deltaTime) {
    for (auto& tween : m_tweens) {
        if (tween.state == TweenState::Completed || tween.state == TweenState::Paused) continue;
        
        // معالجة التأخير / Handle delay
        if (tween.state == TweenState::Pending) {
            tween.delay -= deltaTime;
            if (tween.delay <= 0.0f) {
                tween.state = TweenState::Running;
                deltaTime = -tween.delay; // الوقت الزائد / Overflow time
                tween.delay = 0.0f;
            } else {
                continue;
            }
        }
        
        // تحديث الوقت / Update time
        tween.elapsed += deltaTime;
        
        Float32 t = (tween.duration > 0.0f) ? (tween.elapsed / tween.duration) : 1.0f;
        if (t > 1.0f) t = 1.0f;
        
        // تسهيل / Ease
        Float32 easedT = Ease::Evaluate(tween.easeType, tween.reversed ? (1.0f - t) : t);
        
        // تعيين القيمة / Set value
        if (tween.target) {
            *tween.target = tween.startValue + (tween.endValue - tween.startValue) * easedT;
        }
        
        // callback التحديث / Update callback
        if (tween.onUpdate) {
            tween.onUpdate(easedT);
        }
        
        // اكتمال / Completion
        if (t >= 1.0f) {
            // تكرار / Repeat
            if (tween.repeatCount == -1 || tween.currentRepeat < tween.repeatCount) {
                tween.elapsed = 0.0f;
                tween.currentRepeat++;
                
                if (tween.yoyo) {
                    tween.reversed = !tween.reversed;
                }
            } else {
                tween.state = TweenState::Completed;
                if (tween.target) {
                    *tween.target = tween.reversed ? tween.startValue : tween.endValue;
                }
                if (tween.onComplete) {
                    tween.onComplete();
                }
            }
        }
    }
    
    CleanupCompleted();
}

void TweenManager::Cancel(UInt32 id) {
    auto it = std::find_if(m_tweens.begin(), m_tweens.end(),
        [id](const Tween& t) { return t.id == id; });
    if (it != m_tweens.end()) {
        it->state = TweenState::Completed;
    }
}

void TweenManager::PauseTween(UInt32 id) {
    Tween* t = FindTween(id);
    if (t && t->state == TweenState::Running) {
        t->state = TweenState::Paused;
    }
}

void TweenManager::ResumeTween(UInt32 id) {
    Tween* t = FindTween(id);
    if (t && t->state == TweenState::Paused) {
        t->state = TweenState::Running;
    }
}

void TweenManager::CancelAll() {
    for (auto& t : m_tweens) t.state = TweenState::Completed;
}

void TweenManager::PauseAll() {
    for (auto& t : m_tweens) {
        if (t.state == TweenState::Running) t.state = TweenState::Paused;
    }
}

void TweenManager::ResumeAll() {
    for (auto& t : m_tweens) {
        if (t.state == TweenState::Paused) t.state = TweenState::Running;
    }
}

void TweenManager::Complete(UInt32 id) {
    Tween* t = FindTween(id);
    if (t) {
        t->elapsed = t->duration;
        if (t->target) {
            *t->target = t->reversed ? t->startValue : t->endValue;
        }
        t->state = TweenState::Completed;
        if (t->onComplete) t->onComplete();
    }
}

bool TweenManager::IsRunning(UInt32 id) const {
    const Tween* t = FindTween(id);
    return t && (t->state == TweenState::Running || t->state == TweenState::Pending);
}

Int32 TweenManager::GetActiveCount() const {
    Int32 count = 0;
    for (const auto& t : m_tweens) {
        if (t.state == TweenState::Running || t.state == TweenState::Pending) {
            ++count;
        }
    }
    return count;
}

Tween* TweenManager::FindTween(UInt32 id) {
    for (auto& t : m_tweens) {
        if (t.id == id) return &t;
    }
    return nullptr;
}

const Tween* TweenManager::FindTween(UInt32 id) const {
    for (const auto& t : m_tweens) {
        if (t.id == id) return &t;
    }
    return nullptr;
}

void TweenManager::CleanupCompleted() {
    m_tweens.erase(
        std::remove_if(m_tweens.begin(), m_tweens.end(),
            [](const Tween& t) { return t.state == TweenState::Completed; }),
        m_tweens.end()
    );
}

} // namespace graphics
} // namespace sad
