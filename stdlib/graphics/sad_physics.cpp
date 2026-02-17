// ═══════════════════════════════════════════════════════════════════════════════
// محرك الفيزياء — التنفيذ
// Physics Engine — Implementation
// ═══════════════════════════════════════════════════════════════════════════════
#include "sad_physics.h"
#include <SDL.h>
#include <algorithm>
#include <cmath>

namespace sad { namespace physics {

// ═══════════════════════════════════════════════════════════════════
// الحالة العامة
// ═══════════════════════════════════════════════════════════════════

static std::unordered_map<int, Body> g_bodies;
static std::unordered_map<int, Constraint> g_constraints;
static int g_nextBodyId = 1;
static int g_nextConstraintId = 1;

// إعدادات العالم
static Vec2 g_gravity = {0, 300.0f};   // الجاذبية الافتراضية (للأسفل)
static float g_worldDamping = 0.99f;
static bool g_boundsEnabled = false;
static float g_boundsLeft = 0, g_boundsTop = 0, g_boundsRight = 800, g_boundsBottom = 600;

// التصادمات
static std::vector<CollisionInfo> g_collisions;
static CollisionCallback g_collisionCallback = nullptr;

// ═══════════════════════════════════════════════════════════════════
// مساعدات فيزياء التصادم
// ═══════════════════════════════════════════════════════════════════

static bool aabbOverlap(const Body& a, const Body& b) {
    float aLeft = a.position.x - a.width / 2;
    float aRight = a.position.x + a.width / 2;
    float aTop = a.position.y - a.height / 2;
    float aBottom = a.position.y + a.height / 2;
    float bLeft = b.position.x - b.width / 2;
    float bRight = b.position.x + b.width / 2;
    float bTop = b.position.y - b.height / 2;
    float bBottom = b.position.y + b.height / 2;
    return aLeft < bRight && aRight > bLeft && aTop < bBottom && aBottom > bTop;
}

static bool circleCircle(const Body& a, const Body& b, Vec2& normal, float& penetration) {
    Vec2 d = b.position - a.position;
    float dist = d.length();
    float sumR = a.radius + b.radius;
    if (dist >= sumR) return false;
    penetration = sumR - dist;
    normal = dist > 0.0001f ? d.normalized() : Vec2{1, 0};
    return true;
}

static bool aabbAABB(const Body& a, const Body& b, Vec2& normal, float& penetration) {
    float dx = b.position.x - a.position.x;
    float dy = b.position.y - a.position.y;
    float overlapX = (a.width / 2 + b.width / 2) - std::abs(dx);
    float overlapY = (a.height / 2 + b.height / 2) - std::abs(dy);
    if (overlapX <= 0 || overlapY <= 0) return false;
    
    penetration = std::min(overlapX, overlapY);
    if (overlapX < overlapY) {
        normal = {dx > 0 ? 1.0f : -1.0f, 0};
    } else {
        normal = {0, dy > 0 ? 1.0f : -1.0f};
    }
    return true;
}

static bool circleAABB(const Body& circle, const Body& box, Vec2& normal, float& penetration) {
    float closestX = std::max(box.position.x - box.width / 2,
                    std::min(circle.position.x, box.position.x + box.width / 2));
    float closestY = std::max(box.position.y - box.height / 2,
                    std::min(circle.position.y, box.position.y + box.height / 2));
    
    Vec2 d = {circle.position.x - closestX, circle.position.y - closestY};
    float dist = d.length();
    if (dist >= circle.radius) return false;
    
    penetration = circle.radius - dist;
    normal = dist > 0.0001f ? d.normalized() : Vec2{0, -1};
    return true;
}

static void resolveCollision(Body& a, Body& b, const Vec2& normal, float penetration) {
    // فصل الأجسام المتداخلة
    float totalInvMass = a.invMass + b.invMass;
    if (totalInvMass <= 0) return;
    
    Vec2 correction = normal * (penetration / totalInvMass * 0.8f);
    a.position = a.position - correction * a.invMass;
    b.position = b.position + correction * b.invMass;
    
    // حساب السرعة النسبية
    Vec2 relVel = b.velocity - a.velocity;
    float velAlongNormal = relVel.dot(normal);
    
    // لا نحل إذا كانت الأجسام تبتعد عن بعضها
    if (velAlongNormal > 0) return;
    
    // معامل الارتداد (أقل قيمة)
    float e = std::min(a.restitution, b.restitution);
    
    // حساب قوة الدفع (impulse)
    float j = -(1.0f + e) * velAlongNormal / totalInvMass;
    
    Vec2 impulse = normal * j;
    a.velocity = a.velocity - impulse * a.invMass;
    b.velocity = b.velocity + impulse * b.invMass;
    
    // الاحتكاك
    Vec2 tangent = {-normal.y, normal.x};
    float velAlongTangent = relVel.dot(tangent);
    float frictionCoeff = std::sqrt(a.friction * b.friction);
    float jt = -velAlongTangent / totalInvMass;
    jt = std::max(-j * frictionCoeff, std::min(jt, j * frictionCoeff));
    
    Vec2 frictionImpulse = tangent * jt;
    a.velocity = a.velocity - frictionImpulse * a.invMass;
    b.velocity = b.velocity + frictionImpulse * b.invMass;
}

// ═══════════════════════════════════════════════════════════════════
// الواجهة العامة — الإنشاء
// ═══════════════════════════════════════════════════════════════════

int createCircle(float x, float y, float radius, float mass) {
    int id = g_nextBodyId++;
    Body& b = g_bodies[id];
    b.id = id;
    b.position = {x, y};
    b.shape = BodyShape::Circle;
    b.radius = radius;
    b.mass = mass;
    b.invMass = mass > 0 ? 1.0f / mass : 0.0f;
    b.type = BodyType::Dynamic;
    return id;
}

int createBox(float x, float y, float w, float h, float mass) {
    int id = g_nextBodyId++;
    Body& b = g_bodies[id];
    b.id = id;
    b.position = {x, y};
    b.shape = BodyShape::Rectangle;
    b.width = w;
    b.height = h;
    b.mass = mass;
    b.invMass = mass > 0 ? 1.0f / mass : 0.0f;
    b.type = BodyType::Dynamic;
    return id;
}

int createStaticPlatform(float x, float y, float w, float h) {
    int id = g_nextBodyId++;
    Body& b = g_bodies[id];
    b.id = id;
    b.position = {x, y};
    b.shape = BodyShape::Rectangle;
    b.width = w;
    b.height = h;
    b.mass = 0;
    b.invMass = 0;
    b.type = BodyType::Static;
    b.colorR = 80; b.colorG = 80; b.colorB = 80;
    return id;
}

int createStaticCircle(float x, float y, float radius) {
    int id = g_nextBodyId++;
    Body& b = g_bodies[id];
    b.id = id;
    b.position = {x, y};
    b.shape = BodyShape::Circle;
    b.radius = radius;
    b.mass = 0;
    b.invMass = 0;
    b.type = BodyType::Static;
    return id;
}

int createKinematic(float x, float y, float w, float h) {
    int id = g_nextBodyId++;
    Body& b = g_bodies[id];
    b.id = id;
    b.position = {x, y};
    b.shape = BodyShape::Rectangle;
    b.width = w;
    b.height = h;
    b.mass = 0;
    b.invMass = 0;
    b.type = BodyType::Kinematic;
    return id;
}

// ─── حذف ───
void destroyBody(int id) { g_bodies.erase(id); }
void destroyAllBodies() { g_bodies.clear(); g_constraints.clear(); }

// ─── خصائص ───
void setPosition(int id, float x, float y) { if (g_bodies.count(id)) g_bodies[id].position = {x, y}; }
void setVelocity(int id, float vx, float vy) { if (g_bodies.count(id)) g_bodies[id].velocity = {vx, vy}; }
void setMass(int id, float mass) {
    if (!g_bodies.count(id)) return;
    g_bodies[id].mass = mass;
    g_bodies[id].invMass = mass > 0 ? 1.0f / mass : 0.0f;
}
void setRestitution(int id, float rest) { if (g_bodies.count(id)) g_bodies[id].restitution = rest; }
void setFriction(int id, float fric) { if (g_bodies.count(id)) g_bodies[id].friction = fric; }
void setDrag(int id, float drag) { if (g_bodies.count(id)) g_bodies[id].drag = drag; }
void setRotation(int id, float angle) { if (g_bodies.count(id)) g_bodies[id].rotation = angle; }
void setTag(int id, const std::string& tag) { if (g_bodies.count(id)) g_bodies[id].tag = tag; }
void setBodyColor(int id, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (!g_bodies.count(id)) return;
    g_bodies[id].colorR = r; g_bodies[id].colorG = g;
    g_bodies[id].colorB = b; g_bodies[id].colorA = a;
}
void setCollidable(int id, bool col) { if (g_bodies.count(id)) g_bodies[id].collidable = col; }
void setActive(int id, bool act) { if (g_bodies.count(id)) g_bodies[id].active = act; }

// ─── القراءة ───
float getX(int id) { return g_bodies.count(id) ? g_bodies[id].position.x : 0; }
float getY(int id) { return g_bodies.count(id) ? g_bodies[id].position.y : 0; }
float getVX(int id) { return g_bodies.count(id) ? g_bodies[id].velocity.x : 0; }
float getVY(int id) { return g_bodies.count(id) ? g_bodies[id].velocity.y : 0; }
float getAngle(int id) { return g_bodies.count(id) ? g_bodies[id].rotation : 0; }
std::string getTag(int id) { return g_bodies.count(id) ? g_bodies[id].tag : ""; }

bool isColliding(int idA, int idB) {
    for (auto& c : g_collisions)
        if ((c.bodyA == idA && c.bodyB == idB) || (c.bodyA == idB && c.bodyB == idA))
            return true;
    return false;
}

// ─── القوى ───
void applyForce(int id, float fx, float fy) {
    if (g_bodies.count(id) && g_bodies[id].type == BodyType::Dynamic)
        g_bodies[id].force = g_bodies[id].force + Vec2{fx, fy};
}
void applyImpulse(int id, float ix, float iy) {
    if (!g_bodies.count(id) || g_bodies[id].type != BodyType::Dynamic) return;
    auto& b = g_bodies[id];
    b.velocity = b.velocity + Vec2{ix * b.invMass, iy * b.invMass};
}
void applyTorque(int id, float torque) {
    if (g_bodies.count(id) && g_bodies[id].type == BodyType::Dynamic)
        g_bodies[id].angularVelocity += torque * g_bodies[id].invMass;
}

// ─── إعدادات العالم ───
void setWorldGravity(float gx, float gy) { g_gravity = {gx, gy}; }
void setWorldBounds(float l, float t, float r, float b) {
    g_boundsLeft = l; g_boundsTop = t; g_boundsRight = r; g_boundsBottom = b;
    g_boundsEnabled = true;
}
void setWorldDamping(float d) { g_worldDamping = d; }
void enableWorldBounds(bool en) { g_boundsEnabled = en; }

// ─── القيود ───
int createDistanceConstraint(int bodyA, int bodyB, float length, float stiffness) {
    int id = g_nextConstraintId++;
    Constraint& c = g_constraints[id];
    c.id = id;
    c.type = ConstraintType::Distance;
    c.bodyA = bodyA;
    c.bodyB = bodyB;
    c.targetLength = length;
    c.stiffness = stiffness;
    c.damping = 0;
    return id;
}

int createSpringConstraint(int bodyA, int bodyB, float length, float stiffness, float damping) {
    int id = g_nextConstraintId++;
    Constraint& c = g_constraints[id];
    c.id = id;
    c.type = ConstraintType::Spring;
    c.bodyA = bodyA;
    c.bodyB = bodyB;
    c.targetLength = length;
    c.stiffness = stiffness;
    c.damping = damping;
    return id;
}

int createPinConstraint(int body, float anchorX, float anchorY) {
    int id = g_nextConstraintId++;
    Constraint& c = g_constraints[id];
    c.id = id;
    c.type = ConstraintType::Pin;
    c.bodyA = body;
    c.bodyB = -1;
    c.anchor = {anchorX, anchorY};
    c.stiffness = 1.0f;
    return id;
}

void destroyConstraint(int id) { g_constraints.erase(id); }

// ─── التصادمات ───
void setCollisionCallback(CollisionCallback cb) { g_collisionCallback = cb; }
int getCollisionCount() { return (int)g_collisions.size(); }

// ═══════════════════════════════════════════════════════════════════
// المحاكاة الرئيسية
// ═══════════════════════════════════════════════════════════════════

void step(float deltaMs) {
    float dt = deltaMs / 1000.0f;
    if (dt <= 0 || dt > 0.1f) dt = 0.016f; // حد أقصى 100ms
    
    g_collisions.clear();
    
    // ─── تحديث الأجسام المتحركة ───
    for (auto& [id, body] : g_bodies) {
        if (!body.active || body.type != BodyType::Dynamic) continue;
        
        // تطبيق الجاذبية
        body.force = body.force + g_gravity * body.mass;
        
        // تحديث السرعة من القوى
        body.acceleration = body.force * body.invMass;
        body.velocity = body.velocity + body.acceleration * dt;
        
        // مقاومة الهواء
        body.velocity = body.velocity * (1.0f - body.drag * dt);
        
        // تخميد عام
        body.velocity = body.velocity * g_worldDamping;
        
        // تحديث الموضع
        body.position = body.position + body.velocity * dt;
        
        // تحديث الدوران
        body.rotation += body.angularVelocity * dt;
        
        // إعادة تعيين القوة
        body.force = {0, 0};
        
        // حدود العالم
        if (g_boundsEnabled) {
            if (body.shape == BodyShape::Circle) {
                if (body.position.x - body.radius < g_boundsLeft) {
                    body.position.x = g_boundsLeft + body.radius;
                    body.velocity.x = -body.velocity.x * body.restitution;
                }
                if (body.position.x + body.radius > g_boundsRight) {
                    body.position.x = g_boundsRight - body.radius;
                    body.velocity.x = -body.velocity.x * body.restitution;
                }
                if (body.position.y - body.radius < g_boundsTop) {
                    body.position.y = g_boundsTop + body.radius;
                    body.velocity.y = -body.velocity.y * body.restitution;
                }
                if (body.position.y + body.radius > g_boundsBottom) {
                    body.position.y = g_boundsBottom - body.radius;
                    body.velocity.y = -body.velocity.y * body.restitution;
                }
            } else {
                float hw = body.width / 2, hh = body.height / 2;
                if (body.position.x - hw < g_boundsLeft) {
                    body.position.x = g_boundsLeft + hw;
                    body.velocity.x = -body.velocity.x * body.restitution;
                }
                if (body.position.x + hw > g_boundsRight) {
                    body.position.x = g_boundsRight - hw;
                    body.velocity.x = -body.velocity.x * body.restitution;
                }
                if (body.position.y - hh < g_boundsTop) {
                    body.position.y = g_boundsTop + hh;
                    body.velocity.y = -body.velocity.y * body.restitution;
                }
                if (body.position.y + hh > g_boundsBottom) {
                    body.position.y = g_boundsBottom - hh;
                    body.velocity.y = -body.velocity.y * body.restitution;
                }
            }
        }
    }
    
    // ─── القيود ───
    for (auto& [cid, con] : g_constraints) {
        if (!con.active) continue;
        if (!g_bodies.count(con.bodyA)) continue;
        
        if (con.type == ConstraintType::Pin) {
            auto& a = g_bodies[con.bodyA];
            if (a.type == BodyType::Static) continue;
            Vec2 diff = con.anchor - a.position;
            a.position = a.position + diff * con.stiffness;
            continue;
        }
        
        if (!g_bodies.count(con.bodyB)) continue;
        auto& a = g_bodies[con.bodyA];
        auto& b = g_bodies[con.bodyB];
        
        Vec2 diff = b.position - a.position;
        float dist = diff.length();
        if (dist < 0.0001f) continue;
        
        Vec2 dir = diff.normalized();
        float error = dist - con.targetLength;
        
        if (con.type == ConstraintType::Distance) {
            float correction = error * con.stiffness * 0.5f;
            if (a.type == BodyType::Dynamic) a.position = a.position + dir * correction;
            if (b.type == BodyType::Dynamic) b.position = b.position - dir * correction;
        } else if (con.type == ConstraintType::Spring) {
            float force = error * con.stiffness;
            Vec2 relVel = b.velocity - a.velocity;
            float dampForce = relVel.dot(dir) * con.damping;
            float total = force + dampForce;
            if (a.type == BodyType::Dynamic) a.velocity = a.velocity + dir * (total * a.invMass * dt);
            if (b.type == BodyType::Dynamic) b.velocity = b.velocity - dir * (total * b.invMass * dt);
        }
    }
    
    // ─── كشف وحل التصادمات ───
    std::vector<int> bodyIds;
    bodyIds.reserve(g_bodies.size());
    for (auto& [id, b] : g_bodies) {
        if (b.active && b.collidable) bodyIds.push_back(id);
    }
    
    for (size_t i = 0; i < bodyIds.size(); i++) {
        for (size_t j = i + 1; j < bodyIds.size(); j++) {
            Body& a = g_bodies[bodyIds[i]];
            Body& b = g_bodies[bodyIds[j]];
            
            // لا نحل بين جسمين ثابتين
            if (a.type != BodyType::Dynamic && b.type != BodyType::Dynamic) continue;
            
            Vec2 normal;
            float penetration;
            bool collided = false;
            
            if (a.shape == BodyShape::Circle && b.shape == BodyShape::Circle) {
                collided = circleCircle(a, b, normal, penetration);
            } else if (a.shape == BodyShape::Rectangle && b.shape == BodyShape::Rectangle) {
                collided = aabbAABB(a, b, normal, penetration);
            } else if (a.shape == BodyShape::Circle && b.shape == BodyShape::Rectangle) {
                collided = circleAABB(a, b, normal, penetration);
                normal = normal * -1.0f; // عكس الاتجاه
            } else if (a.shape == BodyShape::Rectangle && b.shape == BodyShape::Circle) {
                collided = circleAABB(b, a, normal, penetration);
            }
            
            if (collided) {
                resolveCollision(a, b, normal, penetration);
                g_collisions.push_back({a.id, b.id, normal, penetration});
                
                if (g_collisionCallback) {
                    g_collisionCallback(a.id, b.id);
                }
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
// الرسم التصحيحي
// ═══════════════════════════════════════════════════════════════════

void debugRender(void* renderer) {
    if (!renderer) return;
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    
    for (auto& [id, body] : g_bodies) {
        if (!body.active) continue;
        
        SDL_SetRenderDrawColor(r, body.colorR, body.colorG, body.colorB, body.colorA);
        
        if (body.shape == BodyShape::Circle) {
            // رسم دائرة بخوارزمية Midpoint
            int cx = (int)body.position.x;
            int cy = (int)body.position.y;
            int rad = (int)body.radius;
            int dx = rad, dy = 0;
            int err = 1 - dx;
            while (dx >= dy) {
                // رسم خطوط ملء
                SDL_RenderDrawLine(r, cx - dx, cy + dy, cx + dx, cy + dy);
                SDL_RenderDrawLine(r, cx - dx, cy - dy, cx + dx, cy - dy);
                SDL_RenderDrawLine(r, cx - dy, cy + dx, cx + dy, cy + dx);
                SDL_RenderDrawLine(r, cx - dy, cy - dx, cx + dy, cy - dx);
                dy++;
                if (err < 0) { err += 2 * dy + 1; }
                else { dx--; err += 2 * (dy - dx) + 1; }
            }
        } else {
            // رسم مستطيل
            SDL_Rect rect = {
                (int)(body.position.x - body.width / 2),
                (int)(body.position.y - body.height / 2),
                (int)body.width, (int)body.height
            };
            if (body.drawOutline) {
                SDL_RenderDrawRect(r, &rect);
            } else {
                SDL_RenderFillRect(r, &rect);
            }
        }
    }
    
    // رسم القيود
    SDL_SetRenderDrawColor(r, 255, 255, 0, 150);
    for (auto& [cid, con] : g_constraints) {
        if (!con.active) continue;
        if (!g_bodies.count(con.bodyA)) continue;
        
        Vec2 p1 = g_bodies[con.bodyA].position;
        Vec2 p2;
        if (con.type == ConstraintType::Pin) {
            p2 = con.anchor;
        } else if (g_bodies.count(con.bodyB)) {
            p2 = g_bodies[con.bodyB].position;
        } else continue;
        
        SDL_RenderDrawLine(r, (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
    }
}

// ─── الاستعلام ───
int getBodyCount() { return (int)g_bodies.size(); }

int findBodyAt(float x, float y) {
    for (auto& [id, b] : g_bodies) {
        if (!b.active) continue;
        if (b.shape == BodyShape::Circle) {
            float dx = x - b.position.x, dy = y - b.position.y;
            if (dx * dx + dy * dy <= b.radius * b.radius) return id;
        } else {
            if (x >= b.position.x - b.width / 2 && x <= b.position.x + b.width / 2 &&
                y >= b.position.y - b.height / 2 && y <= b.position.y + b.height / 2)
                return id;
        }
    }
    return -1;
}

std::vector<int> findBodiesInArea(float x, float y, float w, float h) {
    std::vector<int> result;
    for (auto& [id, b] : g_bodies) {
        if (!b.active) continue;
        if (b.position.x >= x && b.position.x <= x + w &&
            b.position.y >= y && b.position.y <= y + h)
            result.push_back(id);
    }
    return result;
}

void resetWorld() {
    g_bodies.clear();
    g_constraints.clear();
    g_collisions.clear();
    g_nextBodyId = 1;
    g_nextConstraintId = 1;
    g_gravity = {0, 300.0f};
    g_worldDamping = 0.99f;
    g_boundsEnabled = false;
    g_collisionCallback = nullptr;
}

}} // namespace sad::physics
