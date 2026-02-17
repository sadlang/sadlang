// ==============================================================================
// tween.h - نظام التحريك السلس (Tweening/Easing)
// Tween & Easing System
// ==============================================================================
// الوصف: دوال تسهيل متعددة مع مدير أنيميشن مركزي
// Description: Multiple easing functions with centralized animation manager
// ==============================================================================

#pragma once

#include "../core/types.h"
#include <memory>
#include <vector>
#include <functional>
#include <string>

namespace sad {
namespace graphics {

// ==============================================================================
// أنواع التسهيل / Easing Types
// ==============================================================================
enum class EaseType {
    Linear,          // خطي / Linear
    
    // تربيعي / Quadratic
    EaseInQuad,
    EaseOutQuad,
    EaseInOutQuad,
    
    // تكعيبي / Cubic
    EaseInCubic,
    EaseOutCubic,
    EaseInOutCubic,
    
    // رباعي / Quartic
    EaseInQuart,
    EaseOutQuart,
    EaseInOutQuart,
    
    // خماسي / Quintic
    EaseInQuint,
    EaseOutQuint,
    EaseInOutQuint,
    
    // جيبي / Sine
    EaseInSine,
    EaseOutSine,
    EaseInOutSine,
    
    // أسي / Exponential
    EaseInExpo,
    EaseOutExpo,
    EaseInOutExpo,
    
    // دائري / Circular
    EaseInCirc,
    EaseOutCirc,
    EaseInOutCirc,
    
    // ارتداد للخلف / Back
    EaseInBack,
    EaseOutBack,
    EaseInOutBack,
    
    // مطاطي / Elastic
    EaseInElastic,
    EaseOutElastic,
    EaseInOutElastic,
    
    // ارتداد / Bounce
    EaseInBounce,
    EaseOutBounce,
    EaseInOutBounce
};

// ==============================================================================
// دوال التسهيل الثابتة / Static Easing Functions
// ==============================================================================
namespace Ease {
    /// تقييم دالة التسهيل / Evaluate easing function
    /// t: القيمة المدخلة (0-1) / Input value (0-1)
    /// returns: القيمة المُسهّلة / Eased value
    Float32 Evaluate(EaseType type, Float32 t);
    
    // دوال فردية / Individual functions
    Float32 Linear(Float32 t);
    Float32 InQuad(Float32 t);
    Float32 OutQuad(Float32 t);
    Float32 InOutQuad(Float32 t);
    Float32 InCubic(Float32 t);
    Float32 OutCubic(Float32 t);
    Float32 InOutCubic(Float32 t);
    Float32 InQuart(Float32 t);
    Float32 OutQuart(Float32 t);
    Float32 InOutQuart(Float32 t);
    Float32 InQuint(Float32 t);
    Float32 OutQuint(Float32 t);
    Float32 InOutQuint(Float32 t);
    Float32 InSine(Float32 t);
    Float32 OutSine(Float32 t);
    Float32 InOutSine(Float32 t);
    Float32 InExpo(Float32 t);
    Float32 OutExpo(Float32 t);
    Float32 InOutExpo(Float32 t);
    Float32 InCirc(Float32 t);
    Float32 OutCirc(Float32 t);
    Float32 InOutCirc(Float32 t);
    Float32 InBack(Float32 t);
    Float32 OutBack(Float32 t);
    Float32 InOutBack(Float32 t);
    Float32 InElastic(Float32 t);
    Float32 OutElastic(Float32 t);
    Float32 InOutElastic(Float32 t);
    Float32 InBounce(Float32 t);
    Float32 OutBounce(Float32 t);
    Float32 InOutBounce(Float32 t);
}

// ==============================================================================
// حالة الـ tween / Tween State
// ==============================================================================
enum class TweenState {
    Pending,     // في الانتظار / Waiting to start
    Running,     // يعمل / Running
    Paused,      // متوقف مؤقتاً / Paused
    Completed    // انتهى / Completed
};

// ==============================================================================
// عنصر Tween / Tween Item
// ==============================================================================
struct Tween {
    UInt32 id = 0;                              // المعرّف / Identifier
    Float32* target = nullptr;                  // المؤشر للقيمة / Pointer to value
    Float32 startValue = 0.0f;                  // القيمة الابتدائية / Start value
    Float32 endValue = 0.0f;                    // القيمة النهائية / End value
    Float32 duration = 0.0f;                    // المدة بالثواني / Duration in seconds
    Float32 delay = 0.0f;                       // التأخير بالثواني / Delay in seconds
    Float32 elapsed = 0.0f;                     // الوقت المنقضي / Elapsed time
    EaseType easeType = EaseType::Linear;       // نوع التسهيل / Easing type
    TweenState state = TweenState::Pending;     // الحالة / State
    Int32 repeatCount = 0;                      // عدد التكرارات (0=مرة واحدة, -1=لانهائي) / Repeat count
    Int32 currentRepeat = 0;                    // التكرار الحالي / Current repeat
    bool yoyo = false;                          // ذهاب وإياب / Ping-pong
    bool reversed = false;                      // هل معكوس حالياً / Currently reversed
    
    std::function<void()> onComplete;           // عند الانتهاء / On complete callback
    std::function<void(Float32)> onUpdate;      // عند التحديث / On update callback
};

// ==============================================================================
// مدير الـ Tweens / Tween Manager
// ==============================================================================
class TweenManager {
public:
    /// المُنشئ / Constructor
    TweenManager();
    
    /// المدمر / Destructor
    ~TweenManager() = default;
    
    // ==============================================================================
    // إنشاء Tweens / Creating Tweens
    // ==============================================================================
    
    /// إنشاء tween لقيمة عشرية / Create tween for float value
    /// target: مؤشر للقيمة / Pointer to value
    /// endValue: القيمة النهائية / End value
    /// duration: المدة بالثواني / Duration in seconds
    /// easeType: نوع التسهيل / Easing type
    /// returns: معرّف الـ tween / Tween ID
    UInt32 To(Float32* target, Float32 endValue, Float32 duration,
              EaseType easeType = EaseType::EaseOutQuad);
    
    /// إنشاء tween بتأخير / Create tween with delay
    UInt32 To(Float32* target, Float32 endValue, Float32 duration,
              Float32 delay, EaseType easeType = EaseType::EaseOutQuad);
    
    /// إنشاء tween للموقع (Vec2) / Create position tween
    /// x, y: مؤشرات لقيمتي X و Y / Pointers to X and Y values
    /// endX, endY: القيم النهائية / End values
    void TweenPosition(Float32* x, Float32* y, Float32 endX, Float32 endY,
                       Float32 duration, EaseType easeType = EaseType::EaseOutQuad);
    
    // ==============================================================================
    // إعدادات Tween / Tween Settings
    // ==============================================================================
    
    /// تعيين التكرار / Set repeat
    /// id: معرّف الـ tween / Tween ID
    /// count: عدد التكرارات (-1 = لانهائي) / Repeat count (-1 = infinite)
    /// yoyo: ذهاب وإياب / Ping-pong
    void SetRepeat(UInt32 id, Int32 count, bool yoyo = false);
    
    /// تعيين callback الانتهاء / Set complete callback
    void OnComplete(UInt32 id, std::function<void()> callback);
    
    /// تعيين callback التحديث / Set update callback
    void OnUpdate(UInt32 id, std::function<void(Float32)> callback);
    
    // ==============================================================================
    // التحكم / Control
    // ==============================================================================
    
    /// تحديث جميع الـ tweens / Update all tweens
    /// deltaTime: الوقت المنقضي / Elapsed time
    void Update(Float32 deltaTime);
    
    /// إيقاف tween / Stop tween
    void Cancel(UInt32 id);
    
    /// إيقاف مؤقت / Pause tween
    void PauseTween(UInt32 id);
    
    /// استئناف / Resume tween
    void ResumeTween(UInt32 id);
    
    /// إيقاف جميع الـ tweens / Stop all tweens
    void CancelAll();
    
    /// إيقاف مؤقت للكل / Pause all
    void PauseAll();
    
    /// استئناف الكل / Resume all
    void ResumeAll();
    
    /// إكمال فوري / Complete immediately
    void Complete(UInt32 id);
    
    // ==============================================================================
    // الاستعلام / Query
    // ==============================================================================
    
    /// هل الـ tween يعمل / Is tween running
    bool IsRunning(UInt32 id) const;
    
    /// عدد الـ tweens النشطة / Active tween count
    Int32 GetActiveCount() const;
    
    // ==============================================================================
    // إنشاء ثابت / Static
    // ==============================================================================
    
    /// الحصول على المدير العام / Get global manager
    static TweenManager& GetInstance();

private:
    std::vector<Tween> m_tweens;
    UInt32 m_nextId;
    
    Tween* FindTween(UInt32 id);
    const Tween* FindTween(UInt32 id) const;
    void CleanupCompleted();
};

} // namespace graphics
} // namespace sad

// اختصار / Shortcut
namespace SadGraphics {
    using TweenManager = sad::graphics::TweenManager;
    using EaseType = sad::graphics::EaseType;
    using TweenState = sad::graphics::TweenState;
    namespace Ease = sad::graphics::Ease;
}
