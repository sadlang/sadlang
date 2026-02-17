// ═══════════════════════════════════════════════════════════════════════════════
// نظام الجسيمات — لغة ص
// Particle System — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// نظام جسيمات متقدم لا يوجد نظيره في Flutter — يوفر:
//   - مُصدِرات جسيمات (Emitters) قابلة للتخصيص بالكامل
//   - أشكال إصدار: نقطة، دائرة، مستطيل، خط
//   - تأثيرات جاهزة: نار 🔥، دخان 💨، مطر 🌧️، ثلج ❄️، كونفيتي 🎊، شرارات ✨
//   - تأثيرات فيزيائية: جاذبية، رياح، اضطراب (turbulence)
//   - مزج ألوان (Color Blending) مع تدرج على مدار حياة الجسيم
//   - أداء عالي: حتى 10,000 جسيم بسلاسة
//
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <vector>
#include <string>
#include <cmath>
#include <unordered_map>
#include <functional>
#include <cstdint>

namespace sad { namespace particles {

// ═══════════════════════════════════════════════════════════════════
// هياكل البيانات الأساسية
// ═══════════════════════════════════════════════════════════════════

// لون RGBA
struct Color {
    uint8_t r, g, b, a;
};

// شكل منطقة الإصدار
enum class EmitShape {
    Point,      // نقطة واحدة
    Circle,     // دائرة (الجسيمات تنطلق من محيط أو داخل الدائرة)
    Rectangle,  // مستطيل
    Line,       // خط
    Ring        // حلقة (من المحيط فقط)
};

// وضع المزج (Blend Mode) — كيف تُرسم الجسيمات فوق بعضها
enum class BlendMode {
    Normal,     // عادي — شفافية ألفا
    Additive,   // جمعي — يضيء (مثالي للنار والشرارات)
    Multiply,   // ضربي — يعتّم
    Screen      // شاشة — يفتّح
};

// ═══════════════════════════════════════════════════════════════════
// بنية الجسيم الواحد
// ═══════════════════════════════════════════════════════════════════
struct Particle {
    // الموضع
    float x, y;
    
    // السرعة (بالبكسل/ثانية)
    float vx, vy;
    
    // التسارع (للقوى المستمرة)
    float ax, ay;
    
    // العمر
    float lifetime;     // العمر الأقصى (بالثانية)
    float age;          // العمر الحالي
    
    // الحجم
    float size;         // الحجم الحالي
    float sizeStart;    // الحجم عند الولادة
    float sizeEnd;      // الحجم عند الموت
    
    // اللون
    Color colorStart;   // اللون عند الولادة
    Color colorEnd;     // اللون عند الموت
    
    // الدوران
    float rotation;     // الزاوية الحالية (بالدرجات)
    float rotationSpeed; // سرعة الدوران (درجة/ثانية)
    
    // الشفافية
    float alphaStart;   // الشفافية عند الولادة (0-1)
    float alphaEnd;     // الشفافية عند الموت (0-1)
    
    // هل الجسيم حي؟
    bool alive;
};

// ═══════════════════════════════════════════════════════════════════
// إعدادات المُصدِر
// ═══════════════════════════════════════════════════════════════════
struct EmitterConfig {
    // ─── موضع الإصدار ───
    float x = 0, y = 0;            // مركز الإصدار
    EmitShape shape = EmitShape::Point;
    float shapeWidth = 0;           // عرض منطقة الإصدار
    float shapeHeight = 0;          // ارتفاع منطقة الإصدار
    float shapeRadius = 0;          // نصف قطر الدائرة/الحلقة
    
    // ─── معدل الإصدار ───
    float emitRate = 50.0f;         // عدد الجسيمات في الثانية
    int maxParticles = 500;         // الحد الأقصى للجسيمات
    int burstCount = 0;             // إذا > 0: إطلاق دفعة واحدة
    
    // ─── عمر الجسيم ───
    float lifetimeMin = 1.0f;       // أقل عمر (ثانية)
    float lifetimeMax = 3.0f;       // أكبر عمر (ثانية)
    
    // ─── السرعة ───
    float speedMin = 20.0f;         // أقل سرعة (بكسل/ثانية)
    float speedMax = 80.0f;         // أكبر سرعة
    float directionMin = 0.0f;      // أقل اتجاه (درجة) — 0 = يمين، 90 = أعلى
    float directionMax = 360.0f;    // أكبر اتجاه
    
    // ─── الفيزياء ───
    float gravityX = 0.0f;          // الجاذبية الأفقية
    float gravityY = 0.0f;          // الجاذبية الرأسية (موجب = للأسفل)
    float windX = 0.0f;             // قوة الرياح الأفقية
    float windY = 0.0f;             // قوة الرياح الرأسية
    float drag = 0.0f;              // مقاومة الهواء (0 = لا مقاومة، 1 = توقف فوري)
    float turbulence = 0.0f;        // اضطراب عشوائي (0-1)
    
    // ─── الحجم ───
    float sizeStartMin = 4.0f;
    float sizeStartMax = 8.0f;
    float sizeEndMin = 0.0f;
    float sizeEndMax = 2.0f;
    
    // ─── اللون ───
    Color colorStart = {255, 255, 255, 255};
    Color colorEnd = {255, 255, 255, 0};
    bool randomColor = false;       // لون عشوائي لكل جسيم
    
    // ─── الشفافية ───
    float alphaStart = 1.0f;
    float alphaEnd = 0.0f;
    bool fadeIn = false;            // ظهور تدريجي عند الولادة
    float fadeInDuration = 0.1f;    // مدة الظهور التدريجي (بالثانية)
    
    // ─── الدوران ───
    float rotationMin = 0.0f;
    float rotationMax = 0.0f;
    float rotationSpeedMin = 0.0f;
    float rotationSpeedMax = 0.0f;
    
    // ─── الرسم ───
    BlendMode blend = BlendMode::Normal;
    bool isLooping = true;          // تكرار مستمر أم دفعة واحدة
};

// ═══════════════════════════════════════════════════════════════════
// واجهة برمجة نظام الجسيمات (API)
// ═══════════════════════════════════════════════════════════════════

// إنشاء مُصدِر جديد — يُرجع معرّفه
int createEmitter(float x, float y);

// حذف مُصدِر
void destroyEmitter(int emitterId);

// حذف جميع المُصدِرات
void destroyAllEmitters();

// ─── ضبط خصائص المُصدِر ───
void setEmitRate(int id, float rate);
void setMaxParticles(int id, int max);
void setLifetime(int id, float minLife, float maxLife);
void setSpeed(int id, float minSpeed, float maxSpeed);
void setDirection(int id, float minAngle, float maxAngle);
void setGravity(int id, float gx, float gy);
void setWind(int id, float wx, float wy);
void setDrag(int id, float drag);
void setTurbulence(int id, float turb);
void setColorStart(int id, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
void setColorEnd(int id, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0);
void setSize(int id, float startMin, float startMax, float endMin, float endMax);
void setAlpha(int id, float start, float end);
void setRotation(int id, float minRot, float maxRot, float minSpeed, float maxSpeed);
void setEmitShape(int id, const std::string& shape, float w, float h);
void setBlendMode(int id, const std::string& mode);
void setLooping(int id, bool loop);
void setPosition(int id, float x, float y);

// ─── التحكم ───
void startEmitter(int id);
void stopEmitter(int id);
void pauseEmitter(int id);
void resumeEmitter(int id);
void burstEmitter(int id, int count); // إطلاق دفعة واحدة

// ─── الاستعلام ───
bool isEmitterActive(int id);
int getParticleCount(int id);
int getTotalParticleCount();

// ─── التحديث والرسم ───
// يُستدعى كل إطار — يُحدّث فيزياء الجسيمات ويرسمها
void updateAndRenderAll(void* renderer, double deltaMs);

// ═══════════════════════════════════════════════════════════════════
// تأثيرات جاهزة — بسطر واحد!
// Flutter ليس لديه أي من هذه ✨
// ═══════════════════════════════════════════════════════════════════

// 🔥 نار — لهب واقعي صاعد
int presetFire(float x, float y, float intensity = 1.0f);

// 💨 دخان — سحابة رمادية صاعدة
int presetSmoke(float x, float y, float density = 1.0f);

// 🌧️ مطر — قطرات متساقطة
int presetRain(float width, float intensity = 1.0f);

// ❄️ ثلج — رقائق متساقطة ببطء
int presetSnow(float width, float intensity = 1.0f);

// 🎊 كونفيتي — احتفال ملون
int presetConfetti(float x, float y, int count = 100);

// ✨ شرارات — بريق متألق
int presetSparkle(float x, float y, float radius = 50.0f);

// 💥 انفجار — دفعة واحدة من الجسيمات في كل الاتجاهات
int presetExplosion(float x, float y, float power = 1.0f);

// 🌊 فقاعات — فقاعات صاعدة
int presetBubbles(float x, float y, float rate = 10.0f);

// ⭐ مسار نجمي — جسيمات تتبع الماوس
int presetTrail(float x, float y);

// 🌸 أزهار — بتلات متساقطة
int presetPetals(float width, float intensity = 1.0f);

}} // namespace sad::particles
