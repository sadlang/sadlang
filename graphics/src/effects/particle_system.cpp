// ==============================================================================
// particle_system.cpp - تنفيذ نظام الجسيمات
// Particle System Implementation
// ==============================================================================

#include "../../include/effects/particle_system.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace sad {
namespace graphics {

// ==============================================================================
// FloatRange
// ==============================================================================

Float32 FloatRange::Random() const {
    if (min == max) return min;
    Float32 r = static_cast<Float32>(std::rand()) / RAND_MAX;
    return min + r * (max - min);
}

// ==============================================================================
// ColorGradient
// ==============================================================================

Color ColorGradient::Evaluate(Float32 t) const {
    if (keys.empty()) return Color::White;
    if (keys.size() == 1) return keys[0].color;
    
    // ضبط t في المجال [0, 1] / Clamp t to [0, 1]
    t = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);
    
    // البحث عن المفتاحين المحيطين / Find surrounding keys
    for (size_t i = 0; i < keys.size() - 1; ++i) {
        if (t >= keys[i].time && t <= keys[i + 1].time) {
            Float32 range = keys[i + 1].time - keys[i].time;
            if (range < 0.0001f) return keys[i].color;
            
            Float32 localT = (t - keys[i].time) / range;
            const Color& c0 = keys[i].color;
            const Color& c1 = keys[i + 1].color;
            
            return Color(
                c0.r + (c1.r - c0.r) * localT,
                c0.g + (c1.g - c0.g) * localT,
                c0.b + (c1.b - c0.b) * localT,
                c0.a + (c1.a - c0.a) * localT
            );
        }
    }
    
    return keys.back().color;
}

void ColorGradient::AddKey(Float32 time, const Color& color) {
    keys.push_back({time, color});
    // ترتيب حسب الوقت / Sort by time
    std::sort(keys.begin(), keys.end(),
              [](const ColorKey& a, const ColorKey& b) { return a.time < b.time; });
}

// ==============================================================================
// المُنشئات / Constructors
// ==============================================================================

ParticleSystem::ParticleSystem()
    : m_position(0.0f, 0.0f)
    , m_aliveCount(0)
    , m_emitAccumulator(0.0f)
    , m_active(false)
    , m_paused(false)
{
}

ParticleSystem::ParticleSystem(const ParticleEmitterConfig& config)
    : m_config(config)
    , m_position(0.0f, 0.0f)
    , m_aliveCount(0)
    , m_emitAccumulator(0.0f)
    , m_active(false)
    , m_paused(false)
{
    m_particles.resize(config.maxParticles);
    for (auto& p : m_particles) p.alive = false;
}

// ==============================================================================
// الإعدادات / Configuration
// ==============================================================================

void ParticleSystem::SetConfig(const ParticleEmitterConfig& config) {
    m_config = config;
    m_particles.resize(config.maxParticles);
    for (auto& p : m_particles) p.alive = false;
    m_aliveCount = 0;
}

void ParticleSystem::SetPosition(Float32 x, Float32 y) {
    m_position.x = x;
    m_position.y = y;
}

void ParticleSystem::SetPosition(const Vec2& pos) {
    m_position = pos;
}

// ==============================================================================
// التحكم / Control
// ==============================================================================

void ParticleSystem::Start() {
    m_active = true;
    m_paused = false;
    m_emitAccumulator = 0.0f;
    
    // انفجار أولي إذا كان مطلوباً / Initial burst if configured
    if (m_config.burstCount > 0) {
        Burst(m_config.burstCount);
    }
}

void ParticleSystem::Stop() {
    m_active = false;
}

void ParticleSystem::Pause() {
    m_paused = true;
}

void ParticleSystem::Resume() {
    m_paused = false;
}

void ParticleSystem::Burst(Int32 count) {
    for (Int32 i = 0; i < count; ++i) {
        SpawnParticle();
    }
}

void ParticleSystem::Reset() {
    for (auto& p : m_particles) p.alive = false;
    m_aliveCount = 0;
    m_emitAccumulator = 0.0f;
    m_active = false;
    m_paused = false;
}

// ==============================================================================
// التحديث / Update
// ==============================================================================

void ParticleSystem::Update(Float32 deltaTime) {
    if (m_paused) return;
    
    // إنشاء جسيمات جديدة / Spawn new particles
    if (m_active && m_config.emissionRate > 0.0f) {
        m_emitAccumulator += deltaTime * m_config.emissionRate;
        
        while (m_emitAccumulator >= 1.0f) {
            SpawnParticle();
            m_emitAccumulator -= 1.0f;
        }
    }
    
    // تحديث الجسيمات الحية / Update alive particles
    m_aliveCount = 0;
    for (auto& p : m_particles) {
        if (!p.alive) continue;
        
        UpdateParticle(p, deltaTime);
        
        if (p.alive) {
            ++m_aliveCount;
        }
    }
}

// ==============================================================================
// إنشاء جسيم / Spawn Particle
// ==============================================================================

void ParticleSystem::SpawnParticle() {
    // البحث عن جسيم ميت / Find dead particle
    Particle* p = nullptr;
    for (auto& particle : m_particles) {
        if (!particle.alive) {
            p = &particle;
            break;
        }
    }
    
    if (!p) return; // لا مكان / No room
    
    // تهيئة الجسيم / Initialize particle
    p->alive = true;
    ++m_aliveCount;
    p->age = 0.0f;
    p->lifetime = m_config.lifetime.Random();
    p->position = GetEmissionPosition();
    p->startSize = m_config.startSize.Random();
    p->endSize = m_config.endSize.Random();
    p->size = p->startSize;
    p->rotation = m_config.startRotation.Random();
    p->rotationSpeed = m_config.rotationSpeed.Random();
    
    // حساب السرعة من الاتجاه والسرعة / Calculate velocity from direction and speed
    Float32 speed = m_config.speed.Random();
    Float32 angle = m_config.emissionAngle + 
                    RandomFloat(-m_config.spreadAngle * 0.5f, m_config.spreadAngle * 0.5f);
    p->velocity.x = std::cos(angle) * speed;
    p->velocity.y = std::sin(angle) * speed;
    
    // اللون الابتدائي / Initial color
    p->color = m_config.colorOverLifetime.Evaluate(0.0f);
    p->color.a = m_config.startAlpha;
}

// ==============================================================================
// تحديث جسيم / Update Particle
// ==============================================================================

void ParticleSystem::UpdateParticle(Particle& p, Float32 deltaTime) {
    p.age += deltaTime;
    
    // هل مات / Is it dead
    if (p.age >= p.lifetime) {
        p.alive = false;
        return;
    }
    
    Float32 t = p.age / p.lifetime; // نسبة العمر (0-1) / Life ratio (0-1)
    
    // تطبيق الجاذبية / Apply gravity
    p.velocity.x += m_config.gravity.x * deltaTime;
    p.velocity.y += m_config.gravity.y * deltaTime;
    
    // التسارع الشعاعي / Radial acceleration
    if (m_config.radialAcceleration != 0.0f) {
        Vec2 dir = p.position - m_position;
        Float32 len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0.001f) {
            dir.x /= len;
            dir.y /= len;
            p.velocity.x += dir.x * m_config.radialAcceleration * deltaTime;
            p.velocity.y += dir.y * m_config.radialAcceleration * deltaTime;
        }
    }
    
    // التسارع المماسي / Tangential acceleration
    if (m_config.tangentialAcceleration != 0.0f) {
        Vec2 dir = p.position - m_position;
        Float32 len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        if (len > 0.001f) {
            dir.x /= len;
            dir.y /= len;
            // العمودي / Perpendicular
            p.velocity.x += (-dir.y) * m_config.tangentialAcceleration * deltaTime;
            p.velocity.y += dir.x * m_config.tangentialAcceleration * deltaTime;
        }
    }
    
    // التخميد / Damping
    if (m_config.damping > 0.0f) {
        Float32 factor = 1.0f - m_config.damping * deltaTime;
        if (factor < 0.0f) factor = 0.0f;
        p.velocity.x *= factor;
        p.velocity.y *= factor;
    }
    
    // تحديث الموقع / Update position
    p.position.x += p.velocity.x * deltaTime;
    p.position.y += p.velocity.y * deltaTime;
    
    // تحديث الحجم (استيفاء) / Update size (interpolation)
    p.size = p.startSize + (p.endSize - p.startSize) * t;
    
    // تحديث الدوران / Update rotation
    p.rotation += p.rotationSpeed * deltaTime;
    
    // تحديث اللون / Update color
    p.color = m_config.colorOverLifetime.Evaluate(t);
    p.color.a = m_config.startAlpha + (m_config.endAlpha - m_config.startAlpha) * t;
}

// ==============================================================================
// موقع الانبعاث / Emission Position
// ==============================================================================

Vec2 ParticleSystem::GetEmissionPosition() const {
    switch (m_config.shape) {
        case EmitterShape::Point:
            return m_position;
            
        case EmitterShape::Circle: {
            Float32 angle = RandomFloat(0.0f, 6.28318f);
            Float32 radius = RandomFloat(0.0f, m_config.shapeRadius);
            return Vec2(
                m_position.x + std::cos(angle) * radius,
                m_position.y + std::sin(angle) * radius
            );
        }
        
        case EmitterShape::Ring: {
            Float32 angle = RandomFloat(0.0f, 6.28318f);
            return Vec2(
                m_position.x + std::cos(angle) * m_config.shapeRadius,
                m_position.y + std::sin(angle) * m_config.shapeRadius
            );
        }
        
        case EmitterShape::Rectangle:
            return Vec2(
                m_position.x + RandomFloat(-m_config.shapeWidth * 0.5f, m_config.shapeWidth * 0.5f),
                m_position.y + RandomFloat(-m_config.shapeHeight * 0.5f, m_config.shapeHeight * 0.5f)
            );
            
        case EmitterShape::Line:
            return Vec2(
                m_position.x + RandomFloat(-m_config.shapeWidth * 0.5f, m_config.shapeWidth * 0.5f),
                m_position.y
            );
            
        default:
            return m_position;
    }
}

// ==============================================================================
// قيمة عشوائية / Random Value
// ==============================================================================

Float32 ParticleSystem::RandomFloat(Float32 min, Float32 max) {
    Float32 r = static_cast<Float32>(std::rand()) / RAND_MAX;
    return min + r * (max - min);
}

// ==============================================================================
// الإعدادات المسبقة / Presets
// ==============================================================================

ParticleEmitterConfig ParticleSystem::PresetFire() {
    ParticleEmitterConfig c;
    c.shape = EmitterShape::Circle;
    c.shapeRadius = 5.0f;
    c.emissionRate = 50.0f;
    c.maxParticles = 500;
    c.lifetime = {0.5f, 1.5f};
    c.speed = {20.0f, 60.0f};
    c.emissionAngle = -1.5708f; // لأعلى / Upward
    c.spreadAngle = 0.8f;
    c.gravity = Vec2(0.0f, -30.0f);
    c.startSize = {8.0f, 15.0f};
    c.endSize = {2.0f, 4.0f};
    c.startAlpha = 0.8f;
    c.endAlpha = 0.0f;
    c.blendMode = ParticleBlendMode::Additive;
    
    c.colorOverLifetime.Clear();
    c.colorOverLifetime.AddKey(0.0f, Color(1.0f, 1.0f, 0.3f, 1.0f)); // أصفر / Yellow
    c.colorOverLifetime.AddKey(0.3f, Color(1.0f, 0.5f, 0.0f, 1.0f)); // برتقالي / Orange
    c.colorOverLifetime.AddKey(0.7f, Color(0.8f, 0.1f, 0.0f, 0.8f)); // أحمر / Red
    c.colorOverLifetime.AddKey(1.0f, Color(0.3f, 0.0f, 0.0f, 0.0f)); // أحمر داكن / Dark red
    
    return c;
}

ParticleEmitterConfig ParticleSystem::PresetSmoke() {
    ParticleEmitterConfig c;
    c.shape = EmitterShape::Circle;
    c.shapeRadius = 3.0f;
    c.emissionRate = 20.0f;
    c.maxParticles = 300;
    c.lifetime = {2.0f, 4.0f};
    c.speed = {10.0f, 25.0f};
    c.emissionAngle = -1.5708f;
    c.spreadAngle = 0.6f;
    c.gravity = Vec2(0.0f, -10.0f);
    c.startSize = {5.0f, 10.0f};
    c.endSize = {20.0f, 40.0f};
    c.startAlpha = 0.3f;
    c.endAlpha = 0.0f;
    c.blendMode = ParticleBlendMode::Alpha;
    
    c.colorOverLifetime.Clear();
    c.colorOverLifetime.AddKey(0.0f, Color(0.5f, 0.5f, 0.5f, 0.3f));
    c.colorOverLifetime.AddKey(1.0f, Color(0.2f, 0.2f, 0.2f, 0.0f));
    
    return c;
}

ParticleEmitterConfig ParticleSystem::PresetSparks() {
    ParticleEmitterConfig c;
    c.shape = EmitterShape::Point;
    c.emissionRate = 0.0f; // انفجار فقط / Burst only
    c.burstCount = 50;
    c.maxParticles = 200;
    c.lifetime = {0.3f, 0.8f};
    c.speed = {100.0f, 300.0f};
    c.spreadAngle = 6.28318f; // كل الاتجاهات / All directions
    c.gravity = Vec2(0.0f, 200.0f);
    c.startSize = {2.0f, 4.0f};
    c.endSize = {0.5f, 1.0f};
    c.startAlpha = 1.0f;
    c.endAlpha = 0.0f;
    c.blendMode = ParticleBlendMode::Additive;
    
    c.colorOverLifetime.Clear();
    c.colorOverLifetime.AddKey(0.0f, Color(1.0f, 1.0f, 0.5f, 1.0f));
    c.colorOverLifetime.AddKey(0.5f, Color(1.0f, 0.6f, 0.1f, 1.0f));
    c.colorOverLifetime.AddKey(1.0f, Color(0.8f, 0.2f, 0.0f, 0.0f));
    
    return c;
}

ParticleEmitterConfig ParticleSystem::PresetExplosion() {
    ParticleEmitterConfig c;
    c.shape = EmitterShape::Point;
    c.emissionRate = 0.0f;
    c.burstCount = 100;
    c.maxParticles = 200;
    c.lifetime = {0.5f, 1.5f};
    c.speed = {150.0f, 400.0f};
    c.spreadAngle = 6.28318f;
    c.gravity = Vec2(0.0f, 50.0f);
    c.damping = 2.0f;
    c.startSize = {10.0f, 20.0f};
    c.endSize = {2.0f, 5.0f};
    c.startAlpha = 1.0f;
    c.endAlpha = 0.0f;
    c.blendMode = ParticleBlendMode::Additive;
    
    c.colorOverLifetime.Clear();
    c.colorOverLifetime.AddKey(0.0f, Color(1.0f, 1.0f, 1.0f, 1.0f)); // أبيض / White
    c.colorOverLifetime.AddKey(0.1f, Color(1.0f, 0.9f, 0.3f, 1.0f)); // أصفر / Yellow
    c.colorOverLifetime.AddKey(0.4f, Color(1.0f, 0.3f, 0.0f, 0.8f)); // برتقالي / Orange
    c.colorOverLifetime.AddKey(1.0f, Color(0.2f, 0.0f, 0.0f, 0.0f)); // أسود / Black
    
    return c;
}

ParticleEmitterConfig ParticleSystem::PresetSnow() {
    ParticleEmitterConfig c;
    c.shape = EmitterShape::Rectangle;
    c.shapeWidth = 800.0f;
    c.shapeHeight = 10.0f;
    c.emissionRate = 30.0f;
    c.maxParticles = 500;
    c.lifetime = {3.0f, 6.0f};
    c.speed = {20.0f, 40.0f};
    c.emissionAngle = 1.5708f; // لأسفل / Downward
    c.spreadAngle = 0.3f;
    c.gravity = Vec2(0.0f, 5.0f);
    c.startSize = {3.0f, 6.0f};
    c.endSize = {3.0f, 6.0f};
    c.startAlpha = 0.8f;
    c.endAlpha = 0.0f;
    c.blendMode = ParticleBlendMode::Alpha;
    c.rotationSpeed = {-1.0f, 1.0f};
    
    c.colorOverLifetime.Clear();
    c.colorOverLifetime.AddKey(0.0f, Color::White);
    c.colorOverLifetime.AddKey(1.0f, Color(0.9f, 0.9f, 1.0f, 0.0f));
    
    return c;
}

ParticleEmitterConfig ParticleSystem::PresetRain() {
    ParticleEmitterConfig c;
    c.shape = EmitterShape::Rectangle;
    c.shapeWidth = 800.0f;
    c.shapeHeight = 5.0f;
    c.emissionRate = 100.0f;
    c.maxParticles = 1000;
    c.lifetime = {0.5f, 1.5f};
    c.speed = {300.0f, 500.0f};
    c.emissionAngle = 1.5708f; // لأسفل / Downward
    c.spreadAngle = 0.1f;
    c.gravity = Vec2(0.0f, 200.0f);
    c.startSize = {1.0f, 2.0f};
    c.endSize = {1.0f, 2.0f};
    c.startAlpha = 0.6f;
    c.endAlpha = 0.2f;
    c.blendMode = ParticleBlendMode::Alpha;
    
    c.colorOverLifetime.Clear();
    c.colorOverLifetime.AddKey(0.0f, Color(0.6f, 0.7f, 0.9f, 0.6f));
    c.colorOverLifetime.AddKey(1.0f, Color(0.4f, 0.5f, 0.8f, 0.2f));
    
    return c;
}

// ==============================================================================
// إنشاء ثابت / Static Creation
// ==============================================================================

std::shared_ptr<ParticleSystem> ParticleSystem::Create(const ParticleEmitterConfig& config) {
    return std::make_shared<ParticleSystem>(config);
}

} // namespace graphics
} // namespace sad
