// ═══════════════════════════════════════════════════════════════════════════════
// نظام الجسيمات — التنفيذ
// Particle System — Implementation
// ═══════════════════════════════════════════════════════════════════════════════
#include "sad_particles.h"
#include <SDL.h>
#include <algorithm>
#include <random>
#include <cmath>

namespace sad { namespace particles {

// ─── مولد أرقام عشوائية ───
static std::mt19937 g_rng(42);
static float randFloat(float lo, float hi) {
    if (lo >= hi) return lo;
    std::uniform_real_distribution<float> d(lo, hi);
    return d(g_rng);
}
static int randInt(int lo, int hi) {
    std::uniform_int_distribution<int> d(lo, hi);
    return d(g_rng);
}

// ─── حالة المُصدِر ───
struct EmitterState {
    EmitterConfig config;
    std::vector<Particle> pool;
    bool active = false;
    bool paused = false;
    float emitAccum = 0.0f;  // تراكم الإصدار الجزئي
};

static std::unordered_map<int, EmitterState> g_emitters;
static int g_nextId = 1;

// ═══════════════════════════════════════════════════════════════════
// مساعدات داخلية
// ═══════════════════════════════════════════════════════════════════

// استيفاء لون بين لونين حسب نسبة t (0-1)
static Color lerpColor(const Color& a, const Color& b, float t) {
    t = std::max(0.0f, std::min(1.0f, t));
    return {
        (uint8_t)(a.r + (b.r - a.r) * t),
        (uint8_t)(a.g + (b.g - a.g) * t),
        (uint8_t)(a.b + (b.b - a.b) * t),
        (uint8_t)(a.a + (b.a - a.a) * t)
    };
}

// إنشاء جسيم جديد من إعدادات المُصدِر
static Particle spawnParticle(const EmitterConfig& cfg) {
    Particle p;
    p.alive = true;
    p.age = 0.0f;
    p.lifetime = randFloat(cfg.lifetimeMin, cfg.lifetimeMax);
    
    // ─── الموضع حسب شكل الإصدار ───
    switch (cfg.shape) {
        case EmitShape::Point:
            p.x = cfg.x;
            p.y = cfg.y;
            break;
        case EmitShape::Circle: {
            float angle = randFloat(0, 6.2832f);
            float radius = randFloat(0, cfg.shapeRadius);
            p.x = cfg.x + cosf(angle) * radius;
            p.y = cfg.y + sinf(angle) * radius;
            break;
        }
        case EmitShape::Ring: {
            float angle = randFloat(0, 6.2832f);
            p.x = cfg.x + cosf(angle) * cfg.shapeRadius;
            p.y = cfg.y + sinf(angle) * cfg.shapeRadius;
            break;
        }
        case EmitShape::Rectangle:
            p.x = cfg.x + randFloat(-cfg.shapeWidth / 2, cfg.shapeWidth / 2);
            p.y = cfg.y + randFloat(-cfg.shapeHeight / 2, cfg.shapeHeight / 2);
            break;
        case EmitShape::Line:
            p.x = cfg.x + randFloat(-cfg.shapeWidth / 2, cfg.shapeWidth / 2);
            p.y = cfg.y;
            break;
    }
    
    // ─── السرعة والاتجاه ───
    float speed = randFloat(cfg.speedMin, cfg.speedMax);
    float angle = randFloat(cfg.directionMin, cfg.directionMax) * 3.14159f / 180.0f;
    p.vx = cosf(angle) * speed;
    p.vy = -sinf(angle) * speed; // سالب لأن Y مقلوب في SDL
    
    // ─── التسارع (الجاذبية + الرياح) ───
    p.ax = cfg.gravityX + cfg.windX;
    p.ay = cfg.gravityY + cfg.windY;
    
    // ─── الحجم ───
    p.sizeStart = randFloat(cfg.sizeStartMin, cfg.sizeStartMax);
    p.sizeEnd = randFloat(cfg.sizeEndMin, cfg.sizeEndMax);
    p.size = p.sizeStart;
    
    // ─── اللون ───
    if (cfg.randomColor) {
        p.colorStart = {(uint8_t)randInt(50, 255), (uint8_t)randInt(50, 255), 
                        (uint8_t)randInt(50, 255), cfg.colorStart.a};
        p.colorEnd = {p.colorStart.r, p.colorStart.g, p.colorStart.b, 0};
    } else {
        p.colorStart = cfg.colorStart;
        p.colorEnd = cfg.colorEnd;
    }
    
    // ─── الشفافية ───
    p.alphaStart = cfg.alphaStart;
    p.alphaEnd = cfg.alphaEnd;
    
    // ─── الدوران ───
    p.rotation = randFloat(cfg.rotationMin, cfg.rotationMax);
    p.rotationSpeed = randFloat(cfg.rotationSpeedMin, cfg.rotationSpeedMax);
    
    return p;
}

// تحديث جسيم واحد
static void updateParticle(Particle& p, const EmitterConfig& cfg, float dt) {
    if (!p.alive) return;
    
    p.age += dt;
    if (p.age >= p.lifetime) {
        p.alive = false;
        return;
    }
    
    float t = p.age / p.lifetime; // 0 إلى 1
    
    // ─── الاضطراب (turbulence) ───
    float turbX = 0, turbY = 0;
    if (cfg.turbulence > 0) {
        turbX = randFloat(-cfg.turbulence, cfg.turbulence) * 100.0f;
        turbY = randFloat(-cfg.turbulence, cfg.turbulence) * 100.0f;
    }
    
    // ─── تحديث السرعة ───
    p.vx += (p.ax + turbX) * dt;
    p.vy += (p.ay + turbY) * dt;
    
    // ─── مقاومة الهواء ───
    if (cfg.drag > 0) {
        float factor = 1.0f - cfg.drag * dt;
        if (factor < 0) factor = 0;
        p.vx *= factor;
        p.vy *= factor;
    }
    
    // ─── تحديث الموضع ───
    p.x += p.vx * dt;
    p.y += p.vy * dt;
    
    // ─── استيفاء الحجم ───
    p.size = p.sizeStart + (p.sizeEnd - p.sizeStart) * t;
    if (p.size < 0) p.size = 0;
    
    // ─── الدوران ───
    p.rotation += p.rotationSpeed * dt;
}

// ═══════════════════════════════════════════════════════════════════
// الواجهة العامة
// ═══════════════════════════════════════════════════════════════════

int createEmitter(float x, float y) {
    int id = g_nextId++;
    EmitterState& e = g_emitters[id];
    e.config.x = x;
    e.config.y = y;
    e.active = false;
    e.paused = false;
    e.pool.reserve(e.config.maxParticles);
    return id;
}

void destroyEmitter(int id) { g_emitters.erase(id); }
void destroyAllEmitters() { g_emitters.clear(); }

// ─── ضبط الخصائص ───
void setEmitRate(int id, float rate) { if (g_emitters.count(id)) g_emitters[id].config.emitRate = rate; }
void setMaxParticles(int id, int max) { if (g_emitters.count(id)) g_emitters[id].config.maxParticles = max; }
void setLifetime(int id, float mn, float mx) { if (g_emitters.count(id)) { g_emitters[id].config.lifetimeMin = mn; g_emitters[id].config.lifetimeMax = mx; } }
void setSpeed(int id, float mn, float mx) { if (g_emitters.count(id)) { g_emitters[id].config.speedMin = mn; g_emitters[id].config.speedMax = mx; } }
void setDirection(int id, float mn, float mx) { if (g_emitters.count(id)) { g_emitters[id].config.directionMin = mn; g_emitters[id].config.directionMax = mx; } }
void setGravity(int id, float gx, float gy) { if (g_emitters.count(id)) { g_emitters[id].config.gravityX = gx; g_emitters[id].config.gravityY = gy; } }
void setWind(int id, float wx, float wy) { if (g_emitters.count(id)) { g_emitters[id].config.windX = wx; g_emitters[id].config.windY = wy; } }
void setDrag(int id, float d) { if (g_emitters.count(id)) g_emitters[id].config.drag = d; }
void setTurbulence(int id, float t) { if (g_emitters.count(id)) g_emitters[id].config.turbulence = t; }

void setColorStart(int id, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (g_emitters.count(id)) g_emitters[id].config.colorStart = {r, g, b, a};
}
void setColorEnd(int id, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (g_emitters.count(id)) g_emitters[id].config.colorEnd = {r, g, b, a};
}
void setSize(int id, float sMin, float sMax, float eMin, float eMax) {
    if (!g_emitters.count(id)) return;
    auto& c = g_emitters[id].config;
    c.sizeStartMin = sMin; c.sizeStartMax = sMax;
    c.sizeEndMin = eMin; c.sizeEndMax = eMax;
}
void setAlpha(int id, float start, float end) {
    if (g_emitters.count(id)) { g_emitters[id].config.alphaStart = start; g_emitters[id].config.alphaEnd = end; }
}
void setRotation(int id, float rMin, float rMax, float sMin, float sMax) {
    if (!g_emitters.count(id)) return;
    auto& c = g_emitters[id].config;
    c.rotationMin = rMin; c.rotationMax = rMax;
    c.rotationSpeedMin = sMin; c.rotationSpeedMax = sMax;
}
void setEmitShape(int id, const std::string& shape, float w, float h) {
    if (!g_emitters.count(id)) return;
    auto& c = g_emitters[id].config;
    if (shape == "circle" || shape == "\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd8\xa9") { c.shape = EmitShape::Circle; c.shapeRadius = w; }
    else if (shape == "ring" || shape == "\xd8\xad\xd9\x84\xd9\x82\xd8\xa9") { c.shape = EmitShape::Ring; c.shapeRadius = w; }
    else if (shape == "rect" || shape == "\xd9\x85\xd8\xb3\xd8\xaa\xd8\xb7\xd9\x8a\xd9\x84") { c.shape = EmitShape::Rectangle; c.shapeWidth = w; c.shapeHeight = h; }
    else if (shape == "line" || shape == "\xd8\xae\xd8\xb7") { c.shape = EmitShape::Line; c.shapeWidth = w; }
    else { c.shape = EmitShape::Point; }
}
void setBlendMode(int id, const std::string& mode) {
    if (!g_emitters.count(id)) return;
    auto& c = g_emitters[id].config;
    if (mode == "additive" || mode == "\xd8\xac\xd9\x85\xd8\xb9\xd9\x8a") c.blend = BlendMode::Additive;
    else if (mode == "multiply" || mode == "\xd8\xb6\xd8\xb1\xd8\xa8\xd9\x8a") c.blend = BlendMode::Multiply;
    else if (mode == "screen" || mode == "\xd8\xb4\xd8\xa7\xd8\xb4\xd8\xa9") c.blend = BlendMode::Screen;
    else c.blend = BlendMode::Normal;
}
void setLooping(int id, bool loop) { if (g_emitters.count(id)) g_emitters[id].config.isLooping = loop; }
void setPosition(int id, float x, float y) { if (g_emitters.count(id)) { g_emitters[id].config.x = x; g_emitters[id].config.y = y; } }

// ─── التحكم ───
void startEmitter(int id) { if (g_emitters.count(id)) { g_emitters[id].active = true; g_emitters[id].paused = false; } }
void stopEmitter(int id) { if (g_emitters.count(id)) g_emitters[id].active = false; }
void pauseEmitter(int id) { if (g_emitters.count(id)) g_emitters[id].paused = true; }
void resumeEmitter(int id) { if (g_emitters.count(id)) g_emitters[id].paused = false; }

void burstEmitter(int id, int count) {
    if (!g_emitters.count(id)) return;
    auto& e = g_emitters[id];
    for (int i = 0; i < count; i++) {
        if ((int)e.pool.size() >= e.config.maxParticles) break;
        e.pool.push_back(spawnParticle(e.config));
    }
}

// ─── الاستعلام ───
bool isEmitterActive(int id) { return g_emitters.count(id) && g_emitters[id].active; }
int getParticleCount(int id) {
    if (!g_emitters.count(id)) return 0;
    int count = 0;
    for (auto& p : g_emitters[id].pool) if (p.alive) count++;
    return count;
}
int getTotalParticleCount() {
    int total = 0;
    for (auto& [id, e] : g_emitters)
        for (auto& p : e.pool) if (p.alive) total++;
    return total;
}

// ═══════════════════════════════════════════════════════════════════
// التحديث والرسم
// ═══════════════════════════════════════════════════════════════════

void updateAndRenderAll(void* renderer, double deltaMs) {
    if (!renderer) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    float dt = (float)(deltaMs / 1000.0);
    
    for (auto& [id, emitter] : g_emitters) {
        if (!emitter.active || emitter.paused) continue;
        auto& cfg = emitter.config;
        
        // ─── إصدار جسيمات جديدة ───
        emitter.emitAccum += cfg.emitRate * dt;
        while (emitter.emitAccum >= 1.0f) {
            emitter.emitAccum -= 1.0f;
            // البحث عن خانة ميتة أو إضافة جديدة
            bool found = false;
            for (auto& p : emitter.pool) {
                if (!p.alive) { p = spawnParticle(cfg); found = true; break; }
            }
            if (!found && (int)emitter.pool.size() < cfg.maxParticles) {
                emitter.pool.push_back(spawnParticle(cfg));
            }
        }
        
        // ─── تحديث ورسم كل جسيم ───
        // ضبط وضع المزج
        if (cfg.blend == BlendMode::Additive) {
            SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_ADD);
        } else if (cfg.blend == BlendMode::Multiply) {
            SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_MOD);
        } else {
            SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        }
        
        for (auto& p : emitter.pool) {
            if (!p.alive) continue;
            
            updateParticle(p, cfg, dt);
            if (!p.alive) continue;
            
            // ─── حساب اللون والشفافية الحالية ───
            float t = p.age / p.lifetime;
            Color c = lerpColor(p.colorStart, p.colorEnd, t);
            float alpha = p.alphaStart + (p.alphaEnd - p.alphaStart) * t;
            
            // ظهور تدريجي
            if (cfg.fadeIn && p.age < cfg.fadeInDuration) {
                alpha *= p.age / cfg.fadeInDuration;
            }
            
            c.a = (uint8_t)(alpha * 255.0f);
            
            // ─── رسم الجسيم كمربع ─── 
            SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
            
            int sz = (int)p.size;
            if (sz < 1) sz = 1;
            SDL_Rect rect = { (int)(p.x - sz/2), (int)(p.y - sz/2), sz, sz };
            SDL_RenderFillRect(r, &rect);
        }
        
        // ─── إذا ليس حلقي: إيقاف عند انتهاء كل الجسيمات ───
        if (!cfg.isLooping) {
            bool anyAlive = false;
            for (auto& p : emitter.pool) if (p.alive) { anyAlive = true; break; }
            if (!anyAlive && emitter.emitAccum < 1.0f) emitter.active = false;
        }
    }
    
    // إعادة وضع المزج العادي
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
}

// ═══════════════════════════════════════════════════════════════════
// تأثيرات جاهزة
// ═══════════════════════════════════════════════════════════════════

int presetFire(float x, float y, float intensity) {
    int id = createEmitter(x, y);
    auto& c = g_emitters[id].config;
    c.emitRate = 80 * intensity;
    c.maxParticles = (int)(300 * intensity);
    c.lifetimeMin = 0.3f; c.lifetimeMax = 1.2f;
    c.speedMin = 30; c.speedMax = 80;
    c.directionMin = 70; c.directionMax = 110; // للأعلى
    c.gravityY = -20; // طفو خفيف
    c.drag = 0.5f;
    c.turbulence = 0.3f;
    c.sizeStartMin = 6 * intensity; c.sizeStartMax = 12 * intensity;
    c.sizeEndMin = 1; c.sizeEndMax = 3;
    c.colorStart = {255, 180, 0, 255};   // برتقالي
    c.colorEnd = {255, 50, 0, 0};         // أحمر شفاف
    c.alphaStart = 1.0f; c.alphaEnd = 0.0f;
    c.blend = BlendMode::Additive;
    c.shape = EmitShape::Circle; c.shapeRadius = 10 * intensity;
    startEmitter(id);
    return id;
}

int presetSmoke(float x, float y, float density) {
    int id = createEmitter(x, y);
    auto& c = g_emitters[id].config;
    c.emitRate = 20 * density;
    c.maxParticles = (int)(200 * density);
    c.lifetimeMin = 2.0f; c.lifetimeMax = 5.0f;
    c.speedMin = 10; c.speedMax = 30;
    c.directionMin = 70; c.directionMax = 110;
    c.gravityY = -5;
    c.drag = 0.8f;
    c.turbulence = 0.4f;
    c.sizeStartMin = 8; c.sizeStartMax = 15;
    c.sizeEndMin = 20; c.sizeEndMax = 40; // يكبر
    c.colorStart = {120, 120, 120, 200};
    c.colorEnd = {80, 80, 80, 0};
    c.alphaStart = 0.6f; c.alphaEnd = 0.0f;
    c.fadeIn = true; c.fadeInDuration = 0.3f;
    startEmitter(id);
    return id;
}

int presetRain(float width, float intensity) {
    int id = createEmitter(width / 2, -10);
    auto& c = g_emitters[id].config;
    c.emitRate = 100 * intensity;
    c.maxParticles = (int)(500 * intensity);
    c.lifetimeMin = 0.5f; c.lifetimeMax = 1.5f;
    c.speedMin = 400; c.speedMax = 600;
    c.directionMin = 255; c.directionMax = 275; // للأسفل مع ميلان
    c.sizeStartMin = 2; c.sizeStartMax = 3;
    c.sizeEndMin = 2; c.sizeEndMax = 3;
    c.colorStart = {150, 200, 255, 200};
    c.colorEnd = {100, 150, 255, 100};
    c.shape = EmitShape::Line; c.shapeWidth = width;
    startEmitter(id);
    return id;
}

int presetSnow(float width, float intensity) {
    int id = createEmitter(width / 2, -10);
    auto& c = g_emitters[id].config;
    c.emitRate = 30 * intensity;
    c.maxParticles = (int)(300 * intensity);
    c.lifetimeMin = 3.0f; c.lifetimeMax = 8.0f;
    c.speedMin = 15; c.speedMax = 40;
    c.directionMin = 250; c.directionMax = 290;
    c.turbulence = 0.5f;
    c.windX = 10;
    c.sizeStartMin = 3; c.sizeStartMax = 6;
    c.sizeEndMin = 2; c.sizeEndMax = 5;
    c.colorStart = {255, 255, 255, 230};
    c.colorEnd = {200, 220, 255, 100};
    c.rotationSpeedMin = -90; c.rotationSpeedMax = 90;
    c.shape = EmitShape::Line; c.shapeWidth = width;
    startEmitter(id);
    return id;
}

int presetConfetti(float x, float y, int count) {
    int id = createEmitter(x, y);
    auto& c = g_emitters[id].config;
    c.maxParticles = count;
    c.lifetimeMin = 2.0f; c.lifetimeMax = 4.0f;
    c.speedMin = 100; c.speedMax = 300;
    c.directionMin = 30; c.directionMax = 150; // نصف دائرة للأعلى
    c.gravityY = 150; // يسقط
    c.drag = 0.3f;
    c.sizeStartMin = 4; c.sizeStartMax = 8;
    c.sizeEndMin = 3; c.sizeEndMax = 6;
    c.randomColor = true;
    c.alphaStart = 1.0f; c.alphaEnd = 0.3f;
    c.rotationSpeedMin = -360; c.rotationSpeedMax = 360;
    c.isLooping = false;
    burstEmitter(id, count);
    g_emitters[id].active = true;
    return id;
}

int presetSparkle(float x, float y, float radius) {
    int id = createEmitter(x, y);
    auto& c = g_emitters[id].config;
    c.emitRate = 40;
    c.maxParticles = 200;
    c.lifetimeMin = 0.3f; c.lifetimeMax = 0.8f;
    c.speedMin = 5; c.speedMax = 20;
    c.directionMin = 0; c.directionMax = 360;
    c.sizeStartMin = 2; c.sizeStartMax = 5;
    c.sizeEndMin = 0; c.sizeEndMax = 1;
    c.colorStart = {255, 255, 200, 255};
    c.colorEnd = {255, 200, 100, 0};
    c.blend = BlendMode::Additive;
    c.shape = EmitShape::Circle; c.shapeRadius = radius;
    startEmitter(id);
    return id;
}

int presetExplosion(float x, float y, float power) {
    int id = createEmitter(x, y);
    auto& c = g_emitters[id].config;
    c.maxParticles = (int)(150 * power);
    c.lifetimeMin = 0.3f; c.lifetimeMax = 1.5f;
    c.speedMin = 100 * power; c.speedMax = 400 * power;
    c.directionMin = 0; c.directionMax = 360;
    c.gravityY = 100;
    c.drag = 1.5f;
    c.sizeStartMin = 4 * power; c.sizeStartMax = 10 * power;
    c.sizeEndMin = 0; c.sizeEndMax = 1;
    c.colorStart = {255, 200, 50, 255};
    c.colorEnd = {255, 80, 0, 0};
    c.blend = BlendMode::Additive;
    c.isLooping = false;
    burstEmitter(id, (int)(150 * power));
    g_emitters[id].active = true;
    return id;
}

int presetBubbles(float x, float y, float rate) {
    int id = createEmitter(x, y);
    auto& c = g_emitters[id].config;
    c.emitRate = rate;
    c.maxParticles = (int)(rate * 10);
    c.lifetimeMin = 2.0f; c.lifetimeMax = 5.0f;
    c.speedMin = 20; c.speedMax = 50;
    c.directionMin = 70; c.directionMax = 110;
    c.turbulence = 0.4f;
    c.sizeStartMin = 5; c.sizeStartMax = 12;
    c.sizeEndMin = 8; c.sizeEndMax = 18;
    c.colorStart = {200, 230, 255, 150};
    c.colorEnd = {180, 210, 255, 50};
    c.fadeIn = true; c.fadeInDuration = 0.5f;
    startEmitter(id);
    return id;
}

int presetTrail(float x, float y) {
    int id = createEmitter(x, y);
    auto& c = g_emitters[id].config;
    c.emitRate = 60;
    c.maxParticles = 300;
    c.lifetimeMin = 0.2f; c.lifetimeMax = 0.6f;
    c.speedMin = 5; c.speedMax = 15;
    c.directionMin = 0; c.directionMax = 360;
    c.drag = 2.0f;
    c.sizeStartMin = 4; c.sizeStartMax = 8;
    c.sizeEndMin = 0; c.sizeEndMax = 1;
    c.colorStart = {100, 200, 255, 255};
    c.colorEnd = {50, 100, 255, 0};
    c.blend = BlendMode::Additive;
    startEmitter(id);
    return id;
}

int presetPetals(float width, float intensity) {
    int id = createEmitter(width / 2, -20);
    auto& c = g_emitters[id].config;
    c.emitRate = 10 * intensity;
    c.maxParticles = (int)(100 * intensity);
    c.lifetimeMin = 4.0f; c.lifetimeMax = 8.0f;
    c.speedMin = 10; c.speedMax = 25;
    c.directionMin = 240; c.directionMax = 300;
    c.turbulence = 0.6f;
    c.windX = 15;
    c.sizeStartMin = 5; c.sizeStartMax = 10;
    c.sizeEndMin = 4; c.sizeEndMax = 8;
    c.colorStart = {255, 180, 200, 220};
    c.colorEnd = {255, 150, 180, 80};
    c.rotationSpeedMin = -120; c.rotationSpeedMax = 120;
    c.shape = EmitShape::Line; c.shapeWidth = width;
    startEmitter(id);
    return id;
}

}} // namespace sad::particles
