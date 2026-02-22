// ==============================================================================
// particle_system.h - نظام الجسيمات
// Particle System
// ==============================================================================
// الوصف: نظام جسيمات متقدم للتأثيرات البصرية (نار، دخان، انفجارات، شرارات)
// Description: Advanced particle system for visual effects (fire, smoke, explosions, sparks)
// ==============================================================================

#pragma once

#include "../core/types.h"
#include "../texture/texture.h"
#include <memory>
#include <vector>
#include <functional>

namespace sad {
namespace graphics {

// ==============================================================================
// شكل الانبعاث / Emission Shape
// ==============================================================================
enum class EmitterShape {
    Point,        // نقطة / Single point
    Circle,       // دائرة / Circle
    Rectangle,    // مستطيل / Rectangle
    Line,         // خط / Line
    Ring          // حلقة / Ring (circle edge)
};

// ==============================================================================
// وضع المزج / Blend Mode
// ==============================================================================
enum class ParticleBlendMode {
    Alpha,        // مزج شفافية عادي / Normal alpha blending
    Additive,     // مزج جمعي (للإضاءة) / Additive blending (for glow)
    Multiply      // مزج ضربي / Multiplicative blending
};

// ==============================================================================
// مجال قيمة (أدنى - أقصى) / Value Range (min - max)
// ==============================================================================
struct FloatRange {
    Float32 min = 0.0f;
    Float32 max = 0.0f;
    
    FloatRange() = default;
    FloatRange(Float32 val) : min(val), max(val) {}
    FloatRange(Float32 _min, Float32 _max) : min(_min), max(_max) {}
    
    // الحصول على قيمة عشوائية في المجال / Get random value in range
    Float32 Random() const;
    
    // الاستيفاء بين الحدين / Interpolate between bounds
    Float32 Lerp(Float32 t) const { return min + (max - min) * t; }
};

// ==============================================================================
// تدرج لوني / Color Gradient
// ==============================================================================
struct ColorKey {
    Float32 time;    // الوقت (0-1) / Time (0-1)
    Color color;     // اللون / Color
};

struct ColorGradient {
    std::vector<ColorKey> keys;
    
    ColorGradient() {
        keys.push_back({0.0f, Color::White});
        keys.push_back({1.0f, Color::White});
    }
    
    /// الحصول على اللون عند وقت معين / Get color at given time
    Color Evaluate(Float32 t) const;
    
    /// إضافة مفتاح لوني / Add color key
    void AddKey(Float32 time, const Color& color);
    
    /// مسح جميع المفاتيح / Clear all keys
    void Clear() { keys.clear(); }
};

// ==============================================================================
// خصائص الباعث / Emitter Properties
// ==============================================================================
struct ParticleEmitterConfig {
    // الشكل / Shape
    EmitterShape shape = EmitterShape::Point;
    Float32 shapeWidth = 0.0f;       // عرض الشكل / Shape width
    Float32 shapeHeight = 0.0f;      // ارتفاع الشكل / Shape height
    Float32 shapeRadius = 0.0f;      // نصف قطر الشكل / Shape radius
    
    // معدل الانبعاث / Emission rate
    Float32 emissionRate = 10.0f;    // جسيمات/ثانية / Particles per second
    Int32 burstCount = 0;            // عدد الانفجار الفوري / Burst particle count
    Int32 maxParticles = 1000;       // أقصى عدد / Max particle count
    
    // عمر الجسيم / Particle lifetime
    FloatRange lifetime = {1.0f, 2.0f};  // عمر بالثواني / Lifetime in seconds
    
    // السرعة / Speed
    FloatRange speed = {50.0f, 100.0f};
    FloatRange direction = {0.0f, 6.28318f}; // زاوية بالراديان / Angle in radians
    Float32 spreadAngle = 6.28318f;           // زاوية الانتشار / Spread angle (full circle)
    Float32 emissionAngle = 0.0f;             // اتجاه الانبعاث الرئيسي / Main emission direction
    
    // التسارع / Acceleration
    Vec2 gravity = Vec2(0.0f, 0.0f);         // الجاذبية / Gravity
    Float32 radialAcceleration = 0.0f;        // تسارع شعاعي / Radial acceleration
    Float32 tangentialAcceleration = 0.0f;    // تسارع مماسي / Tangential acceleration
    Float32 damping = 0.0f;                   // تخميد (0 = بدون) / Damping (0 = none)
    
    // الحجم / Size
    FloatRange startSize = {10.0f, 10.0f};
    FloatRange endSize = {0.0f, 0.0f};
    
    // الدوران / Rotation
    FloatRange startRotation = {0.0f, 0.0f};
    FloatRange rotationSpeed = {0.0f, 0.0f};
    
    // اللون / Color
    ColorGradient colorOverLifetime;
    Float32 startAlpha = 1.0f;
    Float32 endAlpha = 0.0f;
    
    // المزج / Blending
    ParticleBlendMode blendMode = ParticleBlendMode::Alpha;
    
    // النسيج (اختياري) / Texture (optional)
    TextureRef texture;
};

// ==============================================================================
// جسيم واحد / Single Particle
// ==============================================================================
struct Particle {
    Vec2 position;           // الموقع / Position
    Vec2 velocity;           // السرعة / Velocity
    Float32 lifetime;        // العمر الكلي / Total lifetime
    Float32 age;             // العمر الحالي / Current age
    Float32 size;            // الحجم الحالي / Current size
    Float32 startSize;       // الحجم الابتدائي / Start size
    Float32 endSize;         // الحجم النهائي / End size
    Float32 rotation;        // الدوران الحالي / Current rotation
    Float32 rotationSpeed;   // سرعة الدوران / Rotation speed
    Color color;             // اللون الحالي / Current color
    bool alive;              // حي أم ميت / Alive or dead
};

// ==============================================================================
// فئة نظام الجسيمات / Particle System Class
// ==============================================================================
class ParticleSystem {
public:
    // ==============================================================================
    // الإنشاء / Construction
    // ==============================================================================
    
    /// المُنشئ / Constructor
    ParticleSystem();
    
    /// المُنشئ بإعدادات / Constructor with config
    explicit ParticleSystem(const ParticleEmitterConfig& config);
    
    /// المدمر / Destructor
    ~ParticleSystem() = default;
    
    // ==============================================================================
    // الإعدادات / Configuration
    // ==============================================================================
    
    /// تعيين إعدادات الباعث / Set emitter configuration
    void SetConfig(const ParticleEmitterConfig& config);
    
    /// الحصول على الإعدادات / Get configuration
    const ParticleEmitterConfig& GetConfig() const { return m_config; }
    ParticleEmitterConfig& GetConfig() { return m_config; }
    
    /// تعيين الموقع / Set position
    void SetPosition(Float32 x, Float32 y);
    void SetPosition(const Vec2& pos);
    
    /// الحصول على الموقع / Get position
    Vec2 GetPosition() const { return m_position; }
    
    // ==============================================================================
    // التحكم / Control
    // ==============================================================================
    
    /// بدء الانبعاث / Start emitting
    void Start();
    
    /// إيقاف الانبعاث / Stop emitting
    void Stop();
    
    /// إيقاف مؤقت / Pause
    void Pause();
    
    /// استئناف / Resume
    void Resume();
    
    /// هل يعمل / Is running
    bool IsActive() const { return m_active; }
    
    /// هل متوقف مؤقتاً / Is paused
    bool IsPaused() const { return m_paused; }
    
    /// انفجار فوري لعدد من الجسيمات / Burst particles immediately
    /// count: عدد الجسيمات / Particle count
    void Burst(Int32 count);
    
    /// إعادة تعيين / Reset
    void Reset();
    
    // ==============================================================================
    // التحديث / Update
    // ==============================================================================
    
    /// تحديث الجسيمات / Update particles
    /// deltaTime: الوقت المنقضي بالثواني / Elapsed time in seconds
    void Update(Float32 deltaTime);
    
    // ==============================================================================
    // الاستعلام / Query
    // ==============================================================================
    
    /// عدد الجسيمات الحية / Number of alive particles
    Int32 GetAliveCount() const { return m_aliveCount; }
    
    /// الحصول على قائمة الجسيمات (للرسم الخارجي)
    /// Get particle list (for external rendering)
    const std::vector<Particle>& GetParticles() const { return m_particles; }
    
    /// هل انتهى النظام (توقف الانبعاث وكل الجسيمات ماتت)
    /// Is system finished (stopped emitting and all particles dead)
    bool IsFinished() const { return !m_active && m_aliveCount == 0; }
    
    // ==============================================================================
    // إعدادات مسبقة / Presets
    // ==============================================================================
    
    /// نار / Fire
    static ParticleEmitterConfig PresetFire();
    
    /// دخان / Smoke
    static ParticleEmitterConfig PresetSmoke();
    
    /// شرارات / Sparks
    static ParticleEmitterConfig PresetSparks();
    
    /// انفجار / Explosion
    static ParticleEmitterConfig PresetExplosion();
    
    /// ثلج / Snow
    static ParticleEmitterConfig PresetSnow();
    
    /// مطر / Rain
    static ParticleEmitterConfig PresetRain();
    
    // ==============================================================================
    // إنشاء ثابت / Static Creation
    // ==============================================================================
    
    static std::shared_ptr<ParticleSystem> Create(const ParticleEmitterConfig& config);

private:
    ParticleEmitterConfig m_config;
    Vec2 m_position;
    std::vector<Particle> m_particles;
    Int32 m_aliveCount;
    Float32 m_emitAccumulator;
    bool m_active;
    bool m_paused;
    
    // إنشاء جسيم جديد / Spawn new particle
    void SpawnParticle();
    
    // تحديث جسيم واحد / Update single particle
    void UpdateParticle(Particle& p, Float32 deltaTime);
    
    // الحصول على موقع الانبعاث / Get emission position
    Vec2 GetEmissionPosition() const;
    
    // قيمة عشوائية / Random value
    static Float32 RandomFloat(Float32 min, Float32 max);
};

// أنواع مستعارة / Type aliases
using ParticleSystemRef = std::shared_ptr<ParticleSystem>;

} // namespace graphics
} // namespace sad

// اختصار / Shortcut
namespace SadGraphics {
    using ParticleSystem = sad::graphics::ParticleSystem;
    using ParticleSystemRef = sad::graphics::ParticleSystemRef;
    using ParticleEmitterConfig = sad::graphics::ParticleEmitterConfig;
    using EmitterShape = sad::graphics::EmitterShape;
    using ParticleBlendMode = sad::graphics::ParticleBlendMode;
    using FloatRange = sad::graphics::FloatRange;
    using ColorGradient = sad::graphics::ColorGradient;
}
