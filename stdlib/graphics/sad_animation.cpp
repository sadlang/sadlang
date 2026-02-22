// ═══════════════════════════════════════════════════════════════════════════════
// نظام التحريك — التنفيذ الكامل
// ─────────────────────────────────────────────────────────────────────────────
//
// هذا الملف يحتوي على:
//   ١. تنفيذ ١٦ منحنى تسهيل (Easing Functions)
//   ٢. محرك التحريك الرقمي والألوان والنوابض
//   ٣. نظام التسلسلات والتشغيل المتوازي
//   ٤. تحريكات جاهزة (ظهور، انزلاق، نبضة، اهتزاز)
//   ٥. ربط تلقائي بعناصر الواجهة
//
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_animation.h"
#include "sad_ui_framework.h"
#include <algorithm>
#include <cmath>
#include <string>

namespace sad { namespace anim {

// ═══════════════════════════════════════════════════════════════════
// القسم ١: المتغيرات العامة (سجلات التحريكات)
// ─────────────────────────────────────────────────────────────────
// كل نوع تحريك له سجل خاص (unordered_map) للوصول السريع بالمعرّف
// المعرّف التالي مشترك بين جميع الأنواع لتجنب التعارض
// ═══════════════════════════════════════════════════════════════════

static std::unordered_map<int, Animation>          g_animations;       // التحريكات الرقمية
static std::unordered_map<int, ColorAnimation>     g_colorAnims;       // تحريكات الألوان
static std::unordered_map<int, SpringAnimation>    g_springAnims;      // التحريكات النابضية
static std::unordered_map<int, AnimationSequence>  g_sequences;        // التسلسلات
static int g_nextAnimId = 1;                                           // المعرّف التالي


// ═══════════════════════════════════════════════════════════════════
// القسم ٢: تنفيذ منحنيات التسهيل (١٦ منحنى)
// ─────────────────────────────────────────────────────────────────
//
// كل منحنى يأخذ t (٠ إلى ١) ويُرجع القيمة المحوَّلة.
//
// المرجع الرياضي:
//   - Robert Penner's Easing Equations
//   - https://easings.net/
//
// الثوابت:
//   c1 = 1.70158         (معامل الرجوع الافتراضي)
//   c2 = c1 * 1.525      (معامل الرجوع المعدّل للنسخة المزدوجة)
//   c3 = c1 + 1          (معامل الرجوع + ١)
//   c4 = (2π) / 3        (معامل المرونة — ثلث دورة)
//   c5 = (2π) / 4.5      (معامل المرونة المزدوج)
//
// ═══════════════════════════════════════════════════════════════════

// ── ثوابت رياضية ──
static constexpr double PI = 3.14159265358979323846;
static constexpr double c1 = 1.70158;
static constexpr double c2 = c1 * 1.525;
static constexpr double c3 = c1 + 1.0;
static constexpr double c4 = (2.0 * PI) / 3.0;
static constexpr double c5 = (2.0 * PI) / 4.5;


// ── دالة مساعدة: ارتداد خروج (تُستخدم من ارتداد دخول/مزدوج) ──
static double bounceOutHelper(double t) {
    // الارتداد يُقسَّم لأربع مراحل بنسب مختلفة
    // كل مرحلة هي قطع مكافئ (parabola) بارتفاع أقل
    constexpr double n1 = 7.5625;
    constexpr double d1 = 2.75;
    
    if (t < 1.0 / d1) {
        // المرحلة ١: القفزة الأولى (الأعلى)
        return n1 * t * t;
    } else if (t < 2.0 / d1) {
        // المرحلة ٢: القفزة الثانية
        double t2 = t - 1.5 / d1;
        return n1 * t2 * t2 + 0.75;
    } else if (t < 2.5 / d1) {
        // المرحلة ٣: القفزة الثالثة
        double t2 = t - 2.25 / d1;
        return n1 * t2 * t2 + 0.9375;
    } else {
        // المرحلة ٤: القفزة الأخيرة (الأصغر)
        double t2 = t - 2.625 / d1;
        return n1 * t2 * t2 + 0.984375;
    }
}


double evaluateEasing(EasingType type, double t) {
    // تأكد أن t محصور بين ٠ و ١
    t = std::max(0.0, std::min(1.0, t));
    
    switch (type) {
    
    // ════════════════════════════════════════════
    // خطي — بدون أي منحنى، سرعة ثابتة
    // ════════════════════════════════════════════
    case EasingType::Linear:
        return t;
    
    // ════════════════════════════════════════════
    // تكعيبي (Cubic) — المنحنيات الأكثر شيوعاً
    // تستخدم t³ لمنحنى ناعم وطبيعي
    // ════════════════════════════════════════════
    case EasingType::EaseIn:
        // يبدأ بطيئاً جداً ثم يتسارع بحدة
        // الصيغة: t³
        return t * t * t;
        
    case EasingType::EaseOut:
        // يبدأ سريعاً ثم يتباطأ بنعومة
        // الصيغة: 1 - (1-t)³
        { double p = 1.0 - t; return 1.0 - p * p * p; }
        
    case EasingType::EaseInOut:
        // يجمع بين الاثنين — الأكثر استخداماً في الواجهات
        // النصف الأول: تسارع، النصف الثاني: تباطؤ
        // الصيغة: 4t³ (t < 0.5)  أو  1 - (-2t+2)³/2 (t ≥ 0.5)
        if (t < 0.5) return 4.0 * t * t * t;
        { double p = -2.0 * t + 2.0; return 1.0 - p * p * p / 2.0; }
    
    // ════════════════════════════════════════════
    // ارتداد (Bounce) — كارتداد كرة على الأرض
    // ════════════════════════════════════════════
    case EasingType::BounceOut:
        return bounceOutHelper(t);
        
    case EasingType::BounceIn:
        // عكس ارتداد الخروج
        return 1.0 - bounceOutHelper(1.0 - t);
        
    case EasingType::BounceInOut:
        if (t < 0.5)
            return (1.0 - bounceOutHelper(1.0 - 2.0 * t)) / 2.0;
        return (1.0 + bounceOutHelper(2.0 * t - 1.0)) / 2.0;
    
    // ════════════════════════════════════════════
    // مرونة (Elastic) — كنابض معدني يتذبذب
    // يستخدم sin() لإنشاء التذبذب
    // ════════════════════════════════════════════
    case EasingType::ElasticIn:
        if (t <= 0.0) return 0.0;
        if (t >= 1.0) return 1.0;
        // الصيغة: -2^(10t-10) × sin((10t-10.75) × c4)
        return -std::pow(2.0, 10.0*t - 10.0) * std::sin((10.0*t - 10.75) * c4);
        
    case EasingType::ElasticOut:
        if (t <= 0.0) return 0.0;
        if (t >= 1.0) return 1.0;
        // الصيغة: 2^(-10t) × sin((10t-0.75) × c4) + 1
        return std::pow(2.0, -10.0*t) * std::sin((10.0*t - 0.75) * c4) + 1.0;
        
    case EasingType::ElasticInOut:
        if (t <= 0.0) return 0.0;
        if (t >= 1.0) return 1.0;
        if (t < 0.5)
            return -(std::pow(2.0, 20.0*t - 10.0) * std::sin((20.0*t - 11.125) * c5)) / 2.0;
        return (std::pow(2.0, -20.0*t + 10.0) * std::sin((20.0*t - 11.125) * c5)) / 2.0 + 1.0;
    
    // ════════════════════════════════════════════
    // رجوع (Back) — يتجاوز الحد قليلاً ثم يعود
    // يعطي إحساساً بالحيوية والواقعية
    // ════════════════════════════════════════════
    case EasingType::BackIn:
        // يتراجع للخلف قبل الانطلاق للأمام
        // الصيغة: c3×t³ - c1×t²
        return c3 * t * t * t - c1 * t * t;
        
    case EasingType::BackOut:
        // يتجاوز الهدف قليلاً ثم يعود إليه
        { double p = t - 1.0;
          return 1.0 + c3 * p * p * p + c1 * p * p; }
        
    case EasingType::BackInOut:
        if (t < 0.5)
            return (std::pow(2.0*t, 2.0) * ((c2+1.0)*2.0*t - c2)) / 2.0;
        { double p = 2.0*t - 2.0;
          return (p * p * ((c2+1.0)*p + c2) + 2.0) / 2.0; }
    
    // ════════════════════════════════════════════
    // دائري (Circular) — منحنى على شكل ربع دائرة
    // تسارع/تباطؤ يتبع المنحنى الدائري
    // ════════════════════════════════════════════
    case EasingType::CircIn:
        // الصيغة: 1 - √(1 - t²)
        return 1.0 - std::sqrt(1.0 - t * t);
        
    case EasingType::CircOut:
        // الصيغة: √(1 - (t-1)²)
        { double p = t - 1.0; return std::sqrt(1.0 - p * p); }
        
    case EasingType::CircInOut:
        if (t < 0.5)
            return (1.0 - std::sqrt(1.0 - std::pow(2.0*t, 2.0))) / 2.0;
        return (std::sqrt(1.0 - std::pow(-2.0*t + 2.0, 2.0)) + 1.0) / 2.0;
    
    default:
        return t;
    }
}


// ═══════════════════════════════════════════════════════════════════
// القسم ٣: تحويل أسماء منحنيات التسهيل
// ─────────────────────────────────────────────────────────────────
// يدعم الأسماء بالعربية والإنجليزية
// ═══════════════════════════════════════════════════════════════════

EasingType easingFromName(const std::string& name) {
    // ── خطي ──
    if (name == "\xd8\xae\xd8\xb7\xd9\x8a" || name == "linear")          // خطي
        return EasingType::Linear;
    
    // ── تكعيبي ──
    if (name == "\xd8\xb3\xd9\x87\xd9\x88\xd9\x84\xd8\xa9_\xd8\xaf\xd8\xae\xd9\x88\xd9\x84" || name == "ease_in")    // سهولة_دخول
        return EasingType::EaseIn;
    if (name == "\xd8\xb3\xd9\x87\xd9\x88\xd9\x84\xd8\xa9_\xd8\xae\xd8\xb1\xd9\x88\xd8\xac" || name == "ease_out")   // سهولة_خروج
        return EasingType::EaseOut;
    if (name == "\xd8\xb3\xd9\x87\xd9\x88\xd9\x84\xd8\xa9" || name == "ease_in_out" || name == "ease")  // سهولة
        return EasingType::EaseInOut;
    
    // ── ارتداد ──
    if (name == "\xd8\xa7\xd8\xb1\xd8\xaa\xd8\xaf\xd8\xa7\xd8\xaf_\xd8\xaf\xd8\xae\xd9\x88\xd9\x84" || name == "bounce_in")   // ارتداد_دخول
        return EasingType::BounceIn;
    if (name == "\xd8\xa7\xd8\xb1\xd8\xaa\xd8\xaf\xd8\xa7\xd8\xaf_\xd8\xae\xd8\xb1\xd9\x88\xd8\xac" || name == "bounce_out" || name == "\xd8\xa7\xd8\xb1\xd8\xaa\xd8\xaf\xd8\xa7\xd8\xaf")  // ارتداد_خروج / ارتداد
        return EasingType::BounceOut;
    if (name == "\xd8\xa7\xd8\xb1\xd8\xaa\xd8\xaf\xd8\xa7\xd8\xaf_\xd9\x85\xd8\xb2\xd8\xaf\xd9\x88\xd8\xac" || name == "bounce_in_out")  // ارتداد_مزدوج
        return EasingType::BounceInOut;
    
    // ── مرونة ──
    if (name == "\xd9\x85\xd8\xb1\xd9\x86_\xd8\xaf\xd8\xae\xd9\x88\xd9\x84" || name == "elastic_in")   // مرن_دخول
        return EasingType::ElasticIn;
    if (name == "\xd9\x85\xd8\xb1\xd9\x86_\xd8\xae\xd8\xb1\xd9\x88\xd8\xac" || name == "elastic_out" || name == "\xd9\x85\xd8\xb1\xd9\x86")  // مرن_خروج / مرن
        return EasingType::ElasticOut;
    if (name == "\xd9\x85\xd8\xb1\xd9\x86_\xd9\x85\xd8\xb2\xd8\xaf\xd9\x88\xd8\xac" || name == "elastic_in_out")  // مرن_مزدوج
        return EasingType::ElasticInOut;
    
    // ── رجوع ──
    if (name == "\xd8\xb1\xd8\xac\xd9\x88\xd8\xb9_\xd8\xaf\xd8\xae\xd9\x88\xd9\x84" || name == "back_in")   // رجوع_دخول
        return EasingType::BackIn;
    if (name == "\xd8\xb1\xd8\xac\xd9\x88\xd8\xb9_\xd8\xae\xd8\xb1\xd9\x88\xd8\xac" || name == "back_out" || name == "\xd8\xb1\xd8\xac\xd9\x88\xd8\xb9")  // رجوع_خروج / رجوع
        return EasingType::BackOut;
    if (name == "\xd8\xb1\xd8\xac\xd9\x88\xd8\xb9_\xd9\x85\xd8\xb2\xd8\xaf\xd9\x88\xd8\xac" || name == "back_in_out")  // رجوع_مزدوج
        return EasingType::BackInOut;
    
    // ── دائري ──
    if (name == "\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a_\xd8\xaf\xd8\xae\xd9\x88\xd9\x84" || name == "circ_in")   // دائري_دخول
        return EasingType::CircIn;
    if (name == "\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a_\xd8\xae\xd8\xb1\xd9\x88\xd8\xac" || name == "circ_out" || name == "\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a")  // دائري_خروج / دائري
        return EasingType::CircOut;
    if (name == "\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a_\xd9\x85\xd8\xb2\xd8\xaf\xd9\x88\xd8\xac" || name == "circ_in_out")  // دائري_مزدوج
        return EasingType::CircInOut;
    
    // ── افتراضي ──
    return EasingType::EaseInOut;
}


// ═══════════════════════════════════════════════════════════════════
// القسم ٤: إنشاء التحريكات
// ═══════════════════════════════════════════════════════════════════

int createAnimation(double from, double to, double durationMs, EasingType easing) {
    int id = g_nextAnimId++;
    Animation a;
    a.id = id;
    a.fromValue = from;
    a.toValue = to;
    a.currentValue = from;
    a.durationMs = std::max(1.0, durationMs);  // الحد الأدنى ١ مللي ثانية
    a.easing = easing;
    a.state = AnimState::Idle;
    g_animations[id] = a;
    return id;
}

int createColorAnimation(RGBA from, RGBA to, double durationMs, EasingType easing) {
    int id = g_nextAnimId++;
    ColorAnimation ca;
    ca.id = id;
    ca.fromColor = from;
    ca.toColor = to;
    ca.currentColor = from;
    ca.durationMs = std::max(1.0, durationMs);
    ca.easing = easing;
    ca.state = AnimState::Idle;
    g_colorAnims[id] = ca;
    return id;
}

int createSpringAnimation(double target, double stiffness, double damping, double mass) {
    int id = g_nextAnimId++;
    SpringAnimation sa;
    sa.id = id;
    sa.targetValue = target;
    sa.currentValue = 0.0;  // يبدأ من الصفر
    sa.velocity = 0.0;
    sa.stiffness = std::max(1.0, stiffness);
    sa.damping = std::max(0.1, damping);
    sa.mass = std::max(0.1, mass);
    sa.state = AnimState::Idle;
    g_springAnims[id] = sa;
    return id;
}

int createSequence(const std::vector<int>& animIds, bool parallel) {
    int id = g_nextAnimId++;
    AnimationSequence seq;
    seq.id = id;
    seq.animationIds = animIds;
    seq.mode = parallel ? AnimationSequence::Parallel : AnimationSequence::Sequential;
    seq.state = AnimState::Idle;
    g_sequences[id] = seq;
    return id;
}


// ═══════════════════════════════════════════════════════════════════
// القسم ٥: التحكم في التحريكات
// ─────────────────────────────────────────────────────────────────
// دوال التشغيل والإيقاف تعمل على أي نوع تحريك
// (رقمي، لوني، نابضي، تسلسل)
// ═══════════════════════════════════════════════════════════════════

// ── دالة مساعدة: تشغيل أي نوع بالمعرّف ──
void animPlay(int id) {
    // تحريك رقمي؟
    auto it = g_animations.find(id);
    if (it != g_animations.end()) {
        it->second.state = AnimState::Playing;
        return;
    }
    // تحريك لوني؟
    auto cit = g_colorAnims.find(id);
    if (cit != g_colorAnims.end()) {
        cit->second.state = AnimState::Playing;
        return;
    }
    // تحريك نابضي؟
    auto sit = g_springAnims.find(id);
    if (sit != g_springAnims.end()) {
        sit->second.state = AnimState::Playing;
        return;
    }
    // تسلسل؟
    auto qit = g_sequences.find(id);
    if (qit != g_sequences.end()) {
        qit->second.state = AnimState::Playing;
        qit->second.currentIndex = 0;
        // في وضع المتوازي: شغّل الكل
        if (qit->second.mode == AnimationSequence::Parallel) {
            for (int aid : qit->second.animationIds) animPlay(aid);
        } else {
            // في وضع المتسلسل: شغّل الأول فقط
            if (!qit->second.animationIds.empty())
                animPlay(qit->second.animationIds[0]);
        }
        return;
    }
}

void animPause(int id) {
    auto it = g_animations.find(id);
    if (it != g_animations.end() && it->second.state == AnimState::Playing) {
        it->second.state = AnimState::Paused; return;
    }
    auto cit = g_colorAnims.find(id);
    if (cit != g_colorAnims.end() && cit->second.state == AnimState::Playing) {
        cit->second.state = AnimState::Paused; return;
    }
    auto sit = g_springAnims.find(id);
    if (sit != g_springAnims.end() && sit->second.state == AnimState::Playing) {
        sit->second.state = AnimState::Paused; return;
    }
}

void animResume(int id) {
    auto it = g_animations.find(id);
    if (it != g_animations.end() && it->second.state == AnimState::Paused) {
        it->second.state = AnimState::Playing; return;
    }
    auto cit = g_colorAnims.find(id);
    if (cit != g_colorAnims.end() && cit->second.state == AnimState::Paused) {
        cit->second.state = AnimState::Playing; return;
    }
    auto sit = g_springAnims.find(id);
    if (sit != g_springAnims.end() && sit->second.state == AnimState::Paused) {
        sit->second.state = AnimState::Playing; return;
    }
}

void animStop(int id) {
    auto it = g_animations.find(id);
    if (it != g_animations.end()) {
        auto& a = it->second;
        a.state = AnimState::Idle;
        a.elapsedMs = 0;
        a.delayElapsed = 0;
        a.currentValue = a.fromValue;
        a.currentRepeat = 0;
        a.reversing = false;
        return;
    }
    auto cit = g_colorAnims.find(id);
    if (cit != g_colorAnims.end()) {
        auto& a = cit->second;
        a.state = AnimState::Idle;
        a.elapsedMs = 0;
        a.delayElapsed = 0;
        a.currentColor = a.fromColor;
        a.currentRepeat = 0;
        a.reversing = false;
        return;
    }
    auto sit = g_springAnims.find(id);
    if (sit != g_springAnims.end()) {
        auto& a = sit->second;
        a.state = AnimState::Idle;
        a.currentValue = 0;
        a.velocity = 0;
        return;
    }
}

void animRestart(int id) {
    animStop(id);
    animPlay(id);
}


// ═══════════════════════════════════════════════════════════════════
// القسم ٦: الخصائص والقراءة
// ═══════════════════════════════════════════════════════════════════

void animSetLoop(int id, bool loop) {
    auto it = g_animations.find(id);
    if (it != g_animations.end()) { it->second.loop = loop; return; }
    auto cit = g_colorAnims.find(id);
    if (cit != g_colorAnims.end()) { cit->second.loop = loop; return; }
    auto qit = g_sequences.find(id);
    if (qit != g_sequences.end()) { qit->second.loop = loop; return; }
}

void animSetYoyo(int id, bool yoyo) {
    auto it = g_animations.find(id);
    if (it != g_animations.end()) { it->second.yoyo = yoyo; return; }
    auto cit = g_colorAnims.find(id);
    if (cit != g_colorAnims.end()) { cit->second.yoyo = yoyo; return; }
}

void animSetRepeat(int id, int count) {
    auto it = g_animations.find(id);
    if (it != g_animations.end()) { it->second.repeatCount = count; return; }
    auto cit = g_colorAnims.find(id);
    if (cit != g_colorAnims.end()) { cit->second.repeatCount = count; return; }
}

void animSetDelay(int id, double delayMs) {
    auto it = g_animations.find(id);
    if (it != g_animations.end()) { it->second.delayMs = delayMs; return; }
    auto cit = g_colorAnims.find(id);
    if (cit != g_colorAnims.end()) { cit->second.delayMs = delayMs; return; }
}

void animBindToWidget(int id, int widgetId, const std::string& property) {
    auto it = g_animations.find(id);
    if (it != g_animations.end()) {
        it->second.targetWidgetId = widgetId;
        it->second.targetProperty = property;
        return;
    }
    auto sit = g_springAnims.find(id);
    if (sit != g_springAnims.end()) {
        sit->second.targetWidgetId = widgetId;
        sit->second.targetProperty = property;
        return;
    }
}

double animGetValue(int id) {
    auto it = g_animations.find(id);
    if (it != g_animations.end()) return it->second.currentValue;
    auto sit = g_springAnims.find(id);
    if (sit != g_springAnims.end()) return sit->second.currentValue;
    return 0.0;
}

RGBA animGetColor(int id) {
    auto cit = g_colorAnims.find(id);
    if (cit != g_colorAnims.end()) return cit->second.currentColor;
    return {0, 0, 0, 255};
}

bool animIsPlaying(int id) {
    auto it = g_animations.find(id);
    if (it != g_animations.end()) return it->second.state == AnimState::Playing;
    auto cit = g_colorAnims.find(id);
    if (cit != g_colorAnims.end()) return cit->second.state == AnimState::Playing;
    auto sit = g_springAnims.find(id);
    if (sit != g_springAnims.end()) return sit->second.state == AnimState::Playing;
    auto qit = g_sequences.find(id);
    if (qit != g_sequences.end()) return qit->second.state == AnimState::Playing;
    return false;
}

bool animIsCompleted(int id) {
    auto it = g_animations.find(id);
    if (it != g_animations.end()) return it->second.state == AnimState::Completed;
    auto cit = g_colorAnims.find(id);
    if (cit != g_colorAnims.end()) return cit->second.state == AnimState::Completed;
    auto sit = g_springAnims.find(id);
    if (sit != g_springAnims.end()) return sit->second.state == AnimState::Completed;
    auto qit = g_sequences.find(id);
    if (qit != g_sequences.end()) return qit->second.state == AnimState::Completed;
    return false;
}

double animGetProgress(int id) {
    auto it = g_animations.find(id);
    if (it != g_animations.end()) {
        if (it->second.durationMs <= 0) return 1.0;
        return std::min(1.0, it->second.elapsedMs / it->second.durationMs);
    }
    auto cit = g_colorAnims.find(id);
    if (cit != g_colorAnims.end()) {
        if (cit->second.durationMs <= 0) return 1.0;
        return std::min(1.0, cit->second.elapsedMs / cit->second.durationMs);
    }
    return 0.0;
}


// ═══════════════════════════════════════════════════════════════════
// القسم ٧: تطبيق القيمة على عنصر واجهة
// ─────────────────────────────────────────────────────────────────
// عندما يكون التحريك مربوطاً بعنصر واجهة، نُطبّق القيمة
// الحالية على الخاصية المحددة بشكل تلقائي كل إطار
// ═══════════════════════════════════════════════════════════════════

static void applyAnimValueToWidget(int widgetId, const std::string& prop, double val) {
    if (widgetId < 0) return;
    
    // نستخدم الواجهة العامة لتعديل الخصائص
    // الخصائص المدعومة: شفافية، عرض، ارتفاع، حجم، حشوة، تباعد، انحناء، حدود
    sad::ui::Widget* w = sad::ui::getWidget(widgetId);
    if (!w) return;
    
    // ── شفافية (opacity) ──
    // تعمل على جميع ألوان العنصر (خلفية + نص + زر)
    if (prop == "\xd8\xb4\xd9\x81\xd8\xa7\xd9\x81\xd9\x8a\xd8\xa9" || prop == "opacity" || prop == "alpha") {
        int a = std::max(0, std::min(255, (int)val));
        w->bgColor.a = a;
        w->textColor.a = a;
        w->btnColor.a = a;
        w->btnHover.a = a;
        w->btnPress.a = a;
    }
    // ── عرض ──
    else if (prop == "\xd8\xb9\xd8\xb1\xd8\xb6" || prop == "width") {
        w->fixedWidth = (float)val;
    }
    // ── ارتفاع ──
    else if (prop == "\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9" || prop == "height") {
        w->fixedHeight = (float)val;
    }
    // ── حجم الخط ──
    else if (prop == "\xd8\xad\xd8\xac\xd9\x85" || prop == "\xd8\xad\xd8\xac\xd9\x85_\xd8\xae\xd8\xb7" || prop == "fontSize" || prop == "size") {
        w->fontSize = (float)val;
    }
    // ── الحشوة ──
    else if (prop == "\xd8\xad\xd8\xb4\xd9\x88\xd8\xa9" || prop == "padding") {
        w->padding = (float)val;
    }
    // ── التباعد ──
    else if (prop == "\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xaf" || prop == "spacing") {
        w->spacing = (float)val;
    }
    // ── انحناء الحواف ──
    else if (prop == "\xd8\xa7\xd9\x86\xd8\xad\xd9\x86\xd8\xa7\xd8\xa1" || prop == "borderRadius" || prop == "radius") {
        w->borderRadius = (float)val;
    }
    // ── سمك الحدود ──
    else if (prop == "\xd8\xad\xd8\xaf\xd9\x88\xd8\xaf" || prop == "borderWidth") {
        w->borderWidth = (float)val;
    }
    // ── القيمة (منزلق، شريط تقدم) ──
    else if (prop == "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9" || prop == "value") {
        w->value = (float)val;
    }
    // ── موضع التمرير ──
    else if (prop == "\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1" || prop == "scrollY") {
        w->scrollY = (float)val;
    }
    // ── مرونة (flex) ──
    else if (prop == "\xd9\x85\xd8\xb1\xd9\x88\xd9\x86\xd8\xa9" || prop == "flex") {
        w->flex = (float)val;
    }
}

// تطبيق لون على عنصر واجهة
static void applyAnimColorToWidget(int widgetId, const std::string& prop, RGBA color) {
    if (widgetId < 0) return;
    sad::ui::Widget* w = sad::ui::getWidget(widgetId);
    if (!w) return;
    
    if (prop == "\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9" || prop == "bgColor" || prop == "background") {
        w->bgColor = {color.r, color.g, color.b, color.a};
    }
    else if (prop == "\xd9\x84\xd9\x88\xd9\x86_\xd9\x86\xd8\xb5" || prop == "textColor") {
        w->textColor = {color.r, color.g, color.b, color.a};
    }
    else if (prop == "\xd9\x84\xd9\x88\xd9\x86_\xd8\xb2\xd8\xb1" || prop == "btnColor") {
        w->btnColor = {color.r, color.g, color.b, color.a};
    }
}


// ═══════════════════════════════════════════════════════════════════
// القسم ٨: تحديث التحريكات (قلب المحرك)
// ─────────────────────────────────────────────────────────────────
//
// يُستدعى كل إطار مع فارق الوقت (deltaMs).
// يمر على جميع التحريكات النشطة ويحدث قيمها.
//
// خوارزمية التحديث لتحريك رقمي:
//   ١. إذا كان هناك تأخير، انتظر حتى ينتهي
//   ٢. أضف deltaMs إلى الوقت المنقضي
//   ٣. احسب التقدم الخطي: progress = elapsed / duration
//   ٤. طبّق منحنى التسهيل: eased = evaluateEasing(progress)
//   ٥. احسب القيمة: current = from + (to - from) × eased
//   ٦. إذا وصل للنهاية: تحقق من التكرار / yoyo / الاكتمال
//   ٧. إذا مربوط بعنصر: طبّق القيمة على العنصر
//
// ═══════════════════════════════════════════════════════════════════

// ── دالة مساعدة: تحديث تحريك رقمي واحد ──
static bool updateSingleAnimation(Animation& a, double deltaMs) {
    if (a.state != AnimState::Playing) return false;
    
    // الخطوة ١: معالجة التأخير
    if (a.delayElapsed < a.delayMs) {
        a.delayElapsed += deltaMs;
        if (a.delayElapsed < a.delayMs) return true;  // لا يزال في فترة التأخير
        // بدأ فعلياً، احسب الفائض
        deltaMs = a.delayElapsed - a.delayMs;
    }
    
    // الخطوة ٢: تحديث الوقت المنقضي
    a.elapsedMs += deltaMs;
    
    // الخطوة ٣: حساب التقدم الخطي
    double progress = a.elapsedMs / a.durationMs;
    
    if (progress >= 1.0) {
        // ── وصلنا للنهاية ──
        progress = 1.0;
        
        // هل نعمل في وضع الذهاب والإياب (yoyo)؟
        if (a.yoyo && !a.reversing) {
            // انتقل لمرحلة العودة
            a.reversing = true;
            a.elapsedMs = 0;
            // لا نبدّل القيمتين — سنعكس التقدم يدوياً
            return true;
        }
        
        // هل هناك تكرار متبقي؟
        if (a.loop || (a.repeatCount > 0 && a.currentRepeat < a.repeatCount)) {
            if (!a.loop) a.currentRepeat++;
            a.elapsedMs = 0;
            a.reversing = false;
            return true;
        }
        
        // اكتمل التحريك
        a.state = AnimState::Completed;
    }
    
    // الخطوة ٤: تطبيق منحنى التسهيل
    double eased = evaluateEasing(a.easing, progress);
    
    // في وضع yoyo العكسي: اعكس التقدم
    if (a.reversing) eased = 1.0 - eased;
    
    // الخطوة ٥: حساب القيمة الحالية
    a.currentValue = a.fromValue + (a.toValue - a.fromValue) * eased;
    
    // الخطوة ٦: تطبيق على عنصر الواجهة (إذا مربوط)
    if (a.targetWidgetId >= 0 && !a.targetProperty.empty()) {
        applyAnimValueToWidget(a.targetWidgetId, a.targetProperty, a.currentValue);
    }
    
    return (a.state == AnimState::Playing);
}

// ── دالة مساعدة: تحديث تحريك لون واحد ──
static bool updateSingleColorAnimation(ColorAnimation& a, double deltaMs) {
    if (a.state != AnimState::Playing) return false;
    
    // معالجة التأخير
    if (a.delayElapsed < a.delayMs) {
        a.delayElapsed += deltaMs;
        if (a.delayElapsed < a.delayMs) return true;
        deltaMs = a.delayElapsed - a.delayMs;
    }
    
    a.elapsedMs += deltaMs;
    double progress = std::min(1.0, a.elapsedMs / a.durationMs);
    
    if (progress >= 1.0) {
        if (a.yoyo && !a.reversing) {
            a.reversing = true;
            a.elapsedMs = 0;
            return true;
        }
        if (a.loop || (a.repeatCount > 0 && a.currentRepeat < a.repeatCount)) {
            if (!a.loop) a.currentRepeat++;
            a.elapsedMs = 0;
            a.reversing = false;
            return true;
        }
        a.state = AnimState::Completed;
    }
    
    double eased = evaluateEasing(a.easing, progress);
    if (a.reversing) eased = 1.0 - eased;
    
    // حساب اللون الحالي: خلط خطي لكل مكوّن
    // اللون = من + (إلى - من) × التقدم
    auto lerp = [](int from, int to, double t) -> int {
        return std::max(0, std::min(255, (int)(from + (to - from) * t)));
    };
    a.currentColor.r = lerp(a.fromColor.r, a.toColor.r, eased);
    a.currentColor.g = lerp(a.fromColor.g, a.toColor.g, eased);
    a.currentColor.b = lerp(a.fromColor.b, a.toColor.b, eased);
    a.currentColor.a = lerp(a.fromColor.a, a.toColor.a, eased);
    
    return (a.state == AnimState::Playing);
}

// ── دالة مساعدة: تحديث تحريك نابضي واحد ──
// يستخدم طريقة أويلر المحسّنة (Semi-implicit Euler) لحل معادلة النابض:
//   F = -stiffness × (x - target) - damping × v
//   a = F / mass
//   v += a × dt
//   x += v × dt
static bool updateSingleSpringAnimation(SpringAnimation& a, double deltaMs) {
    if (a.state != AnimState::Playing) return false;
    
    double dt = deltaMs / 1000.0;  // تحويل إلى ثوانٍ
    
    // حساب القوة = -صلابة × الإزاحة - تخميد × السرعة
    double displacement = a.currentValue - a.targetValue;
    double springForce = -a.stiffness * displacement;
    double dampingForce = -a.damping * a.velocity;
    double acceleration = (springForce + dampingForce) / a.mass;
    
    // تحديث السرعة والموضع (Semi-implicit Euler)
    a.velocity += acceleration * dt;
    a.currentValue += a.velocity * dt;
    
    // هل وصلنا للاستقرار؟
    if (std::abs(a.velocity) < a.tolerance && std::abs(displacement) < a.tolerance) {
        a.currentValue = a.targetValue;
        a.velocity = 0;
        a.state = AnimState::Completed;
    }
    
    // تطبيق على عنصر الواجهة
    if (a.targetWidgetId >= 0 && !a.targetProperty.empty()) {
        applyAnimValueToWidget(a.targetWidgetId, a.targetProperty, a.currentValue);
    }
    
    return (a.state == AnimState::Playing);
}


// ═══════════════════════════════════════════════════════════════════
// الدالة الرئيسية: تحديث جميع التحريكات النشطة
// ─────────────────────────────────────────────────────────────────
// تُستدعى مرة واحدة كل إطار من app_update()
// ═══════════════════════════════════════════════════════════════════

int updateAllAnimations(double deltaMs) {
    int activeCount = 0;
    
    // ── تحديث التحريكات الرقمية ──
    for (auto& [id, a] : g_animations) {
        if (updateSingleAnimation(a, deltaMs)) activeCount++;
    }
    
    // ── تحديث تحريكات الألوان ──
    for (auto& [id, a] : g_colorAnims) {
        if (updateSingleColorAnimation(a, deltaMs)) activeCount++;
    }
    
    // ── تحديث التحريكات النابضية ──
    for (auto& [id, a] : g_springAnims) {
        if (updateSingleSpringAnimation(a, deltaMs)) activeCount++;
    }
    
    // ── تحديث التسلسلات ──
    for (auto& [id, seq] : g_sequences) {
        if (seq.state != AnimState::Playing) continue;
        
        if (seq.mode == AnimationSequence::Sequential) {
            // في الوضع المتسلسل: تحقق إذا التحريك الحالي اكتمل
            if (seq.currentIndex < (int)seq.animationIds.size()) {
                int currentAnimId = seq.animationIds[seq.currentIndex];
                if (animIsCompleted(currentAnimId)) {
                    // انتقل للتحريك التالي
                    seq.currentIndex++;
                    if (seq.currentIndex < (int)seq.animationIds.size()) {
                        animPlay(seq.animationIds[seq.currentIndex]);
                    } else if (seq.loop) {
                        // أعد التسلسل من البداية
                        seq.currentIndex = 0;
                        for (int aid : seq.animationIds) animStop(aid);
                        animPlay(seq.animationIds[0]);
                    } else {
                        seq.state = AnimState::Completed;
                    }
                }
                activeCount++;
            }
        } else {
            // في الوضع المتوازي: تحقق إذا الكل اكتملوا
            bool allDone = true;
            for (int aid : seq.animationIds) {
                if (!animIsCompleted(aid)) { allDone = false; break; }
            }
            if (allDone) {
                if (seq.loop) {
                    for (int aid : seq.animationIds) animRestart(aid);
                } else {
                    seq.state = AnimState::Completed;
                }
            } else {
                activeCount++;
            }
        }
    }
    
    return activeCount;
}


// ═══════════════════════════════════════════════════════════════════
// القسم ٩: التنظيف
// ═══════════════════════════════════════════════════════════════════

void animDestroy(int id) {
    g_animations.erase(id);
    g_colorAnims.erase(id);
    g_springAnims.erase(id);
    g_sequences.erase(id);
}

void animDestroyAll() {
    g_animations.clear();
    g_colorAnims.clear();
    g_springAnims.clear();
    g_sequences.clear();
}

int animActiveCount() {
    int count = 0;
    for (auto& [id, a] : g_animations) if (a.state == AnimState::Playing) count++;
    for (auto& [id, a] : g_colorAnims) if (a.state == AnimState::Playing) count++;
    for (auto& [id, a] : g_springAnims) if (a.state == AnimState::Playing) count++;
    for (auto& [id, a] : g_sequences) if (a.state == AnimState::Playing) count++;
    return count;
}


// ═══════════════════════════════════════════════════════════════════
// القسم ١٠: تحريكات جاهزة (مختصرات شائعة)
// ─────────────────────────────────────────────────────────────────
// كل دالة تُنشئ تحريكاً أو أكثر، تربطه بعنصر الواجهة،
// وتبدأ التشغيل فوراً. يُرجع معرّف التحريك.
// ═══════════════════════════════════════════════════════════════════

int animFadeIn(int widgetId, double durationMs) {
    // ظهور تدريجي: شفافية من ٠ إلى ٢٥٥
    int id = createAnimation(0, 255, durationMs, EasingType::EaseOut);
    animBindToWidget(id, widgetId, "opacity");
    animPlay(id);
    return id;
}

int animFadeOut(int widgetId, double durationMs) {
    // اختفاء تدريجي: شفافية من ٢٥٥ إلى ٠
    int id = createAnimation(255, 0, durationMs, EasingType::EaseIn);
    animBindToWidget(id, widgetId, "opacity");
    animPlay(id);
    return id;
}

int animSlideFromRight(int widgetId, double distance, double durationMs) {
    // انزلاق من اليمين: العرض يزداد تدريجياً
    // لأن الواجهة RTL، اليمين هو جهة البداية الطبيعية
    int id = createAnimation(distance, 0, durationMs, EasingType::EaseOut);
    animBindToWidget(id, widgetId, "padding");
    animPlay(id);
    return id;
}

int animSlideFromLeft(int widgetId, double distance, double durationMs) {
    int id = createAnimation(-distance, 0, durationMs, EasingType::EaseOut);
    animBindToWidget(id, widgetId, "padding");
    animPlay(id);
    return id;
}

int animSlideFromTop(int widgetId, double distance, double durationMs) {
    int id = createAnimation(-distance, 0, durationMs, EasingType::EaseOut);
    animBindToWidget(id, widgetId, "padding");
    animPlay(id);
    return id;
}

int animSlideFromBottom(int widgetId, double distance, double durationMs) {
    int id = createAnimation(distance, 0, durationMs, EasingType::EaseOut);
    animBindToWidget(id, widgetId, "padding");
    animPlay(id);
    return id;
}

int animScaleIn(int widgetId, double durationMs) {
    // تكبير: حجم الخط من ١ إلى حجمه الطبيعي
    sad::ui::Widget* w = sad::ui::getWidget(widgetId);
    float targetSize = w ? w->fontSize : 20.0f;
    int id = createAnimation(1, targetSize, durationMs, EasingType::BackOut);
    animBindToWidget(id, widgetId, "fontSize");
    animPlay(id);
    return id;
}

int animPulse(int widgetId, double scale, double durationMs) {
    // نبضة: حجم الخط يكبر قليلاً ثم يعود
    sad::ui::Widget* w = sad::ui::getWidget(widgetId);
    float baseSize = w ? w->fontSize : 20.0f;
    int id = createAnimation(baseSize, baseSize * scale, durationMs, EasingType::EaseInOut);
    animSetYoyo(id, true);
    animBindToWidget(id, widgetId, "fontSize");
    animPlay(id);
    return id;
}

int animShake(int widgetId, double intensity, double durationMs) {
    // اهتزاز: الحشوة تتأرجح يميناً ويساراً
    // نستخدم تحريك مرن لإنشاء التذبذب
    int id = createSpringAnimation(0, 400, 8, 0.5);
    // ابدأ بإزاحة أولية
    auto sit = g_springAnims.find(id);
    if (sit != g_springAnims.end()) {
        sit->second.currentValue = intensity;
        sit->second.velocity = 0;
    }
    animBindToWidget(id, widgetId, "padding");
    animPlay(id);
    return id;
}


}} // namespace sad::anim
