// ═══════════════════════════════════════════════════════════════════════════════
// نظام التحريك — محرك الحركة والانتقالات لواجهات لغة ص
// ─────────────────────────────────────────────────────────────────────────────
// 
// يوفر هذا النظام:
//   ١. تحريكات قيم رقمية (من → إلى) مع منحنيات تسهيل متعددة
//   ٢. تحريكات ألوان (انتقال سلس بين لونين)
//   ٣. تشغيل متعدد (عدة تحريكات في نفس الوقت)
//   ٤. تسلسلات (تحريكات متتابعة واحدة تلو الأخرى)
//   ٥. تحريكات نابضية (Spring Physics) واقعية
//   ٦. منحنيات تسهيل احترافية (١٦ نوع)
//
// مستوحى من:
//   - Flutter AnimationController / Tween / CurvedAnimation
//   - CSS Transitions / Web Animations API
//   - React Native Animated
//
// الترخيص: MIT — لغة ص
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cmath>

namespace sad { namespace anim {

// ═══════════════════════════════════════════════════════════════════
// القسم ١: أنواع منحنيات التسهيل
// ─────────────────────────────────────────────────────────────────
// منحنى التسهيل يحدد كيف تتغير القيمة بمرور الوقت:
//   - خطي: سرعة ثابتة (بدون تسريع أو تباطؤ)
//   - سهولة_دخول: يبدأ بطيئاً ثم يتسارع
//   - سهولة_خروج: يبدأ سريعاً ثم يتباطأ
//   - سهولة: يجمع بين الدخول والخروج (الأكثر طبيعية)
//   - ارتداد: يرتد عند النهاية كالكرة
//   - مرونة: يتمدد ويتقلص كالنابض
//   - ظهر: يتجاوز الهدف قليلاً ثم يعود
//   - دائري: منحنى دائري هندسي
// ═══════════════════════════════════════════════════════════════════

enum class EasingType {
    // ── أساسي ──
    Linear,         // خطي — سرعة ثابتة
    
    // ── تكعيبي (Cubic) ──
    EaseIn,         // سهولة_دخول — بداية بطيئة
    EaseOut,        // سهولة_خروج — نهاية بطيئة
    EaseInOut,      // سهولة — بداية ونهاية بطيئة
    
    // ── ارتداد (Bounce) ──
    BounceIn,       // ارتداد_دخول — يرتد في البداية
    BounceOut,      // ارتداد_خروج — يرتد في النهاية
    BounceInOut,    // ارتداد — يرتد في البداية والنهاية
    
    // ── مرونة (Elastic) ──
    ElasticIn,      // مرن_دخول — تمدد نابضي في البداية
    ElasticOut,     // مرن_خروج — تمدد نابضي في النهاية
    ElasticInOut,   // مرن — تمدد نابضي في البداية والنهاية
    
    // ── رجوع (Back) ──
    BackIn,         // رجوع_دخول — يتراجع قبل الانطلاق
    BackOut,        // رجوع_خروج — يتجاوز الهدف ثم يعود
    BackInOut,      // رجوع — يتراجع ويتجاوز
    
    // ── دائري (Circular) ──
    CircIn,         // دائري_دخول — منحنى دائري بطيء ثم سريع
    CircOut,        // دائري_خروج — منحنى دائري سريع ثم بطيء
    CircInOut       // دائري — منحنى دائري متماثل
};

// ═══════════════════════════════════════════════════════════════════
// القسم ٢: حالة التحريك
// ─────────────────────────────────────────────────────────────────
// تحدد في أي مرحلة التحريك حالياً
// ═══════════════════════════════════════════════════════════════════

enum class AnimState {
    Idle,       // خامل — لم يبدأ بعد أو انتهى
    Playing,    // يعمل — التحريك جارٍ
    Paused,     // متوقف مؤقتاً
    Completed   // مكتمل — وصل للنهاية
};

// ═══════════════════════════════════════════════════════════════════
// القسم ٣: هيكل التحريك الرئيسي
// ─────────────────────────────────────────────────────────────────
// يمثل تحريكاً واحداً لقيمة رقمية
// يمكن أن يكون:
//   - تحريك عددي (مثل: شفافية من ٠ إلى ٢٥٥)
//   - تحريك موضع (مثل: x من -١٠٠ إلى ٣٠٠)
//   - تحريك حجم (مثل: عرض من ٥٠ إلى ٢٠٠)
// ═══════════════════════════════════════════════════════════════════

struct Animation {
    int id = 0;                     // المعرّف الفريد
    
    // ── القيم ──
    double fromValue = 0.0;         // قيمة البداية
    double toValue = 1.0;           // قيمة النهاية
    double currentValue = 0.0;      // القيمة الحالية (محسوبة تلقائياً)
    
    // ── التوقيت ──
    double durationMs = 300.0;      // المدة بالمللي ثانية
    double elapsedMs = 0.0;         // الوقت المنقضي
    double delayMs = 0.0;           // تأخير قبل البدء
    double delayElapsed = 0.0;      // وقت التأخير المنقضي
    
    // ── المنحنى والحالة ──
    EasingType easing = EasingType::EaseInOut;  // منحنى التسهيل
    AnimState state = AnimState::Idle;          // الحالة الحالية
    
    // ── خيارات التكرار ──
    bool loop = false;              // تكرار لانهائي
    bool yoyo = false;              // ذهاب وإياب (مثل البندول)
    int repeatCount = 0;            // عدد مرات التكرار (٠ = بدون)
    int currentRepeat = 0;          // عداد التكرار الحالي
    bool reversing = false;         // هل نحن في مرحلة العودة (yoyo)
    
    // ── ربط بعنصر واجهة (اختياري) ──
    int targetWidgetId = -1;        // معرّف العنصر المستهدف
    std::string targetProperty;     // اسم الخاصية المستهدفة (شفافية، عرض، ...)
};

// ═══════════════════════════════════════════════════════════════════
// القسم ٤: تحريك الألوان
// ─────────────────────────────────────────────────────────────────
// ينتقل بسلاسة بين لونين (كل مكون: أحمر، أخضر، أزرق، شفافية)
// ═══════════════════════════════════════════════════════════════════

struct RGBA {
    int r = 0, g = 0, b = 0, a = 255;
};

struct ColorAnimation {
    int id = 0;                     // المعرّف الفريد
    
    RGBA fromColor = {0,0,0,255};   // اللون الابتدائي
    RGBA toColor = {255,255,255,255}; // اللون النهائي
    RGBA currentColor = {0,0,0,255};  // اللون الحالي
    
    double durationMs = 300.0;      // المدة بالمللي ثانية
    double elapsedMs = 0.0;         // الوقت المنقضي
    double delayMs = 0.0;           // تأخير قبل البدء
    double delayElapsed = 0.0;      // وقت التأخير المنقضي
    
    EasingType easing = EasingType::EaseInOut;
    AnimState state = AnimState::Idle;
    
    bool loop = false;
    bool yoyo = false;
    int repeatCount = 0;
    int currentRepeat = 0;
    bool reversing = false;
};

// ═══════════════════════════════════════════════════════════════════
// القسم ٥: تحريك نابضي (Spring Physics)
// ─────────────────────────────────────────────────────────────────
// يحاكي فيزياء النابض الحقيقية:
//   - الصلابة (stiffness): كلما زادت، كان التحريك أسرع
//   - التخميد (damping): كلما زاد، قلّ التذبذب
//   - الكتلة (mass): كلما زادت، كان التحريك أبطأ
// مستوحى من Flutter spring simulation
// ═══════════════════════════════════════════════════════════════════

struct SpringAnimation {
    int id = 0;
    
    double targetValue = 0.0;       // القيمة المستهدفة
    double currentValue = 0.0;      // القيمة الحالية
    double velocity = 0.0;          // السرعة الحالية
    
    // ── معاملات الفيزياء ──
    double stiffness = 180.0;       // الصلابة (Flutter default)
    double damping = 12.0;          // التخميد
    double mass = 1.0;              // الكتلة
    
    AnimState state = AnimState::Idle;
    double tolerance = 0.001;       // حد التقريب للتوقف
    
    int targetWidgetId = -1;
    std::string targetProperty;
};

// ═══════════════════════════════════════════════════════════════════
// القسم ٦: تسلسل تحريكات
// ─────────────────────────────────────────────────────────────────
// يسمح بتشغيل عدة تحريكات:
//   - متسلسلة (واحدة تلو الأخرى)
//   - متوازية (جميعها في نفس الوقت)
// ═══════════════════════════════════════════════════════════════════

struct AnimationSequence {
    int id = 0;
    
    enum Mode { Sequential, Parallel };
    Mode mode = Sequential;
    
    std::vector<int> animationIds;  // قائمة معرّفات التحريكات
    int currentIndex = 0;           // التحريك الحالي (للمتسلسل)
    AnimState state = AnimState::Idle;
    bool loop = false;              // تكرار التسلسل بالكامل
};

// ═══════════════════════════════════════════════════════════════════
// القسم ٧: دوال منحنيات التسهيل
// ─────────────────────────────────────────────════════════════════
// تأخذ قيمة t بين ٠.٠ و ١.٠ وتعيد القيمة المحولة
// ═══════════════════════════════════════════════════════════════════

// حساب قيمة منحنى التسهيل
// t: التقدم الخطي (٠.٠ إلى ١.٠)
// يُرجع: القيمة بعد تطبيق المنحنى
double evaluateEasing(EasingType type, double t);

// تحويل اسم عربي إلى نوع تسهيل
// يدعم الأسماء العربية والإنجليزية:
//   "خطي" / "linear"
//   "سهولة_دخول" / "ease_in"
//   "سهولة_خروج" / "ease_out"
//   "سهولة" / "ease_in_out"
//   "ارتداد" / "bounce_out"
//   "مرن" / "elastic_out"
//   إلخ...
EasingType easingFromName(const std::string& name);

// ═══════════════════════════════════════════════════════════════════
// القسم ٨: واجهة برمجة نظام التحريك
// ─────────────────────────────────────────────────────────────────
// جميع الدوال الخارجية لإنشاء التحريكات وإدارتها
// ═══════════════════════════════════════════════════════════════════

// ───── إنشاء التحريكات ─────

// إنشاء تحريك رقمي جديد
// from: قيمة البداية
// to: قيمة النهاية
// durationMs: المدة بالمللي ثانية
// easing: منحنى التسهيل
// يُرجع: معرّف التحريك
int createAnimation(double from, double to, double durationMs, 
                    EasingType easing = EasingType::EaseInOut);

// إنشاء تحريك لون
int createColorAnimation(RGBA from, RGBA to, double durationMs,
                         EasingType easing = EasingType::EaseInOut);

// إنشاء تحريك نابضي
int createSpringAnimation(double target, double stiffness = 180.0,
                          double damping = 12.0, double mass = 1.0);

// إنشاء تسلسل تحريكات
int createSequence(const std::vector<int>& animIds, bool parallel = false);

// ───── التحكم في التحريكات ─────

void animPlay(int animId);          // تشغيل
void animPause(int animId);         // إيقاف مؤقت
void animResume(int animId);        // استئناف
void animStop(int animId);          // إيقاف وإعادة للبداية
void animRestart(int animId);       // إعادة تشغيل من البداية

// ───── الخصائص ─────

void animSetLoop(int animId, bool loop);      // تكرار لانهائي
void animSetYoyo(int animId, bool yoyo);      // ذهاب وإياب
void animSetRepeat(int animId, int count);     // تكرار محدد
void animSetDelay(int animId, double delayMs); // تأخير قبل البدء

// ربط التحريك بخاصية عنصر واجهة
// عند كل تحديث، يُطبَّق القيمة تلقائياً على العنصر
void animBindToWidget(int animId, int widgetId, const std::string& property);

// ───── القراءة ─────

double animGetValue(int animId);              // القيمة الحالية
RGBA   animGetColor(int colorAnimId);         // اللون الحالي
bool   animIsPlaying(int animId);             // هل يعمل؟
bool   animIsCompleted(int animId);           // هل اكتمل؟
double animGetProgress(int animId);           // التقدم (٠.٠ إلى ١.٠)

// ───── التحديث (يُستدعى كل إطار) ─────

// تحديث جميع التحريكات النشطة
// deltaMs: الوقت المنقضي منذ الإطار السابق بالمللي ثانية
// يُرجع: عدد التحريكات النشطة حالياً
int updateAllAnimations(double deltaMs);

// ───── التنظيف ─────

void animDestroy(int animId);        // حذف تحريك محدد
void animDestroyAll();               // حذف جميع التحريكات
int  animActiveCount();              // عدد التحريكات النشطة

// ═══════════════════════════════════════════════════════════════════
// القسم ٩: تحريكات جاهزة (مختصرات)
// ─────────────────────────────────────────────────────────────────
// تحريكات شائعة جداً يمكن استخدامها بسطر واحد
// مستوحاة من animate.css و Flutter animations
// ═══════════════════════════════════════════════════════════════════

// ظهور تدريجي (شفافية من ٠ إلى ٢٥٥)
int animFadeIn(int widgetId, double durationMs = 300);

// اختفاء تدريجي (شفافية من ٢٥٥ إلى ٠)
int animFadeOut(int widgetId, double durationMs = 300);

// انزلاق من اليمين (لواجهات RTL العربية)
int animSlideFromRight(int widgetId, double distance, double durationMs = 400);

// انزلاق من اليسار
int animSlideFromLeft(int widgetId, double distance, double durationMs = 400);

// انزلاق من الأعلى
int animSlideFromTop(int widgetId, double distance, double durationMs = 400);

// انزلاق من الأسفل
int animSlideFromBottom(int widgetId, double distance, double durationMs = 400);

// تكبير (من حجم صغير إلى الحجم الطبيعي)
int animScaleIn(int widgetId, double durationMs = 300);

// نبضة (يكبر قليلاً ثم يعود — لفت الانتباه)
int animPulse(int widgetId, double scale = 1.1, double durationMs = 600);

// اهتزاز (يمين ويسار — للتنبيه على خطأ)
int animShake(int widgetId, double intensity = 10.0, double durationMs = 500);

}} // namespace sad::anim
