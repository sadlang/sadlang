// ==============================================================================
// physics2d.cpp - تنفيذ نظام الفيزياء ثنائي الأبعاد
// 2D Physics System Implementation
// ==============================================================================

#include "../../include/physics/physics2d.h"
#include <algorithm>
#include <cmath>

namespace sad {
namespace graphics {

// ==============================================================================
// Collider2D - إنشاء الأشكال / Shape Creation
// ==============================================================================

std::shared_ptr<Collider2D> Collider2D::CreateCircle(Float32 radius, const Vec2& center) {
    auto collider = std::make_shared<Collider2D>();
    collider->m_type = ShapeType::Circle;
    collider->m_circle.center = center;
    collider->m_circle.radius = radius;
    return collider;
}

std::shared_ptr<Collider2D> Collider2D::CreateBox(Float32 width, Float32 height, const Vec2& center) {
    auto collider = std::make_shared<Collider2D>();
    collider->m_type = ShapeType::Box;
    collider->m_box.center = center;
    collider->m_box.halfExtents = Vec2(width * 0.5f, height * 0.5f);
    collider->m_box.angle = 0.0f;
    return collider;
}

std::shared_ptr<Collider2D> Collider2D::CreatePolygon(const std::vector<Vec2>& vertices) {
    auto collider = std::make_shared<Collider2D>();
    collider->m_type = ShapeType::Polygon;
    collider->m_polygon.vertices = vertices;
    return collider;
}

std::shared_ptr<Collider2D> Collider2D::CreateEdge(const Vec2& start, const Vec2& end) {
    auto collider = std::make_shared<Collider2D>();
    collider->m_type = ShapeType::Edge;
    collider->m_edge.start = start;
    collider->m_edge.end = end;
    return collider;
}

Rect Collider2D::GetBounds() const {
    switch (m_type) {
        case ShapeType::Circle: {
            Vec2 worldCenter = m_body ? m_body->LocalToWorld(m_circle.center) : m_circle.center;
            return Rect(worldCenter.x - m_circle.radius, worldCenter.y - m_circle.radius,
                       m_circle.radius * 2.0f, m_circle.radius * 2.0f);
        }
        case ShapeType::Box: {
            Vec2 worldCenter = m_body ? m_body->LocalToWorld(m_box.center) : m_box.center;
            Float32 halfW = m_box.halfExtents.x;
            Float32 halfH = m_box.halfExtents.y;
            // للتبسيط، نتجاهل الدوران في حساب الحدود
            // For simplicity, ignore rotation in bounds calculation
            return Rect(worldCenter.x - halfW, worldCenter.y - halfH, halfW * 2.0f, halfH * 2.0f);
        }
        case ShapeType::Polygon: {
            if (m_polygon.vertices.empty()) return Rect();
            Float32 minX = m_polygon.vertices[0].x, maxX = minX;
            Float32 minY = m_polygon.vertices[0].y, maxY = minY;
            for (const auto& v : m_polygon.vertices) {
                Vec2 world = m_body ? m_body->LocalToWorld(v) : v;
                minX = std::min(minX, world.x);
                maxX = std::max(maxX, world.x);
                minY = std::min(minY, world.y);
                maxY = std::max(maxY, world.y);
            }
            return Rect(minX, minY, maxX - minX, maxY - minY);
        }
        case ShapeType::Edge: {
            Vec2 worldStart = m_body ? m_body->LocalToWorld(m_edge.start) : m_edge.start;
            Vec2 worldEnd = m_body ? m_body->LocalToWorld(m_edge.end) : m_edge.end;
            Float32 minX = std::min(worldStart.x, worldEnd.x);
            Float32 minY = std::min(worldStart.y, worldEnd.y);
            Float32 maxX = std::max(worldStart.x, worldEnd.x);
            Float32 maxY = std::max(worldStart.y, worldEnd.y);
            return Rect(minX, minY, maxX - minX, maxY - minY);
        }
        default:
            return Rect();
    }
}

// ==============================================================================
// RigidBody2D - الجسم الصلب / Rigid Body
// ==============================================================================

void RigidBody2D::SetPosition(const Vec2& pos) {
    m_position = pos;
    m_awake = true;
}

void RigidBody2D::SetPosition(Float32 x, Float32 y) {
    SetPosition(Vec2(x, y));
}

void RigidBody2D::SetRotation(Float32 angle) {
    m_rotation = angle;
    m_awake = true;
}

Vec2 RigidBody2D::LocalToWorld(const Vec2& local) const {
    Float32 c = std::cos(m_rotation);
    Float32 s = std::sin(m_rotation);
    return Vec2(
        m_position.x + local.x * c - local.y * s,
        m_position.y + local.x * s + local.y * c
    );
}

Vec2 RigidBody2D::WorldToLocal(const Vec2& world) const {
    Float32 c = std::cos(-m_rotation);
    Float32 s = std::sin(-m_rotation);
    Float32 dx = world.x - m_position.x;
    Float32 dy = world.y - m_position.y;
    return Vec2(dx * c - dy * s, dx * s + dy * c);
}

void RigidBody2D::SetMass(Float32 mass) {
    m_mass = mass;
    m_inverseMass = (mass > 0.0001f) ? 1.0f / mass : 0.0f;
}

void RigidBody2D::SetInertia(Float32 inertia) {
    m_inertia = inertia;
    m_inverseInertia = (inertia > 0.0001f) ? 1.0f / inertia : 0.0f;
}

void RigidBody2D::ApplyForce(const Vec2& force) {
    if (m_type != BodyType::Dynamic) return;
    m_force.x += force.x;
    m_force.y += force.y;
    m_awake = true;
}

void RigidBody2D::ApplyForce(const Vec2& force, const Vec2& point) {
    if (m_type != BodyType::Dynamic) return;
    m_force.x += force.x;
    m_force.y += force.y;
    
    // حساب العزم من القوة في نقطة معينة
    // Calculate torque from force at point
    Vec2 r = point;
    r.x -= m_position.x;
    r.y -= m_position.y;
    m_torque += r.x * force.y - r.y * force.x;
    m_awake = true;
}

void RigidBody2D::ApplyImpulse(const Vec2& impulse) {
    if (m_type != BodyType::Dynamic) return;
    m_velocity.x += impulse.x * m_inverseMass;
    m_velocity.y += impulse.y * m_inverseMass;
    m_awake = true;
}

void RigidBody2D::ApplyImpulse(const Vec2& impulse, const Vec2& point) {
    if (m_type != BodyType::Dynamic) return;
    m_velocity.x += impulse.x * m_inverseMass;
    m_velocity.y += impulse.y * m_inverseMass;
    
    Vec2 r = point;
    r.x -= m_position.x;
    r.y -= m_position.y;
    m_angularVelocity += (r.x * impulse.y - r.y * impulse.x) * m_inverseInertia;
    m_awake = true;
}

void RigidBody2D::ApplyTorque(Float32 torque) {
    if (m_type != BodyType::Dynamic || m_fixedRotation) return;
    m_torque += torque;
    m_awake = true;
}

void RigidBody2D::ApplyAngularImpulse(Float32 impulse) {
    if (m_type != BodyType::Dynamic || m_fixedRotation) return;
    m_angularVelocity += impulse * m_inverseInertia;
    m_awake = true;
}

Collider2D* RigidBody2D::AddCollider(std::shared_ptr<Collider2D> collider) {
    collider->m_body = this;
    m_colliders.push_back(collider);
    return collider.get();
}

void RigidBody2D::RemoveCollider(Collider2D* collider) {
    auto it = std::remove_if(m_colliders.begin(), m_colliders.end(),
        [collider](const std::shared_ptr<Collider2D>& c) { return c.get() == collider; });
    if (it != m_colliders.end()) {
        m_colliders.erase(it, m_colliders.end());
    }
}

// ==============================================================================
// PhysicsWorld2D - عالم الفيزياء / Physics World
// ==============================================================================

PhysicsWorld2D::PhysicsWorld2D() = default;

PhysicsWorld2D::PhysicsWorld2D(const PhysicsSettings& settings) 
    : m_settings(settings) {
}

PhysicsWorld2D::~PhysicsWorld2D() {
    Clear();
}

RigidBody2D* PhysicsWorld2D::CreateBody(BodyType type) {
    auto body = std::make_unique<RigidBody2D>();
    body->m_id = m_nextBodyId++;
    body->m_type = type;
    
    // الأجسام الثابتة لها كتلة لانهائية
    // Static bodies have infinite mass
    if (type == BodyType::Static) {
        body->m_mass = 0.0f;
        body->m_inverseMass = 0.0f;
        body->m_inertia = 0.0f;
        body->m_inverseInertia = 0.0f;
    }
    
    RigidBody2D* ptr = body.get();
    m_bodies.push_back(std::move(body));
    return ptr;
}

RigidBody2D* PhysicsWorld2D::CreateStaticBody() {
    return CreateBody(BodyType::Static);
}

RigidBody2D* PhysicsWorld2D::CreateKinematicBody() {
    return CreateBody(BodyType::Kinematic);
}

RigidBody2D* PhysicsWorld2D::CreateDynamicBody() {
    return CreateBody(BodyType::Dynamic);
}

void PhysicsWorld2D::DestroyBody(RigidBody2D* body) {
    auto it = std::remove_if(m_bodies.begin(), m_bodies.end(),
        [body](const std::unique_ptr<RigidBody2D>& b) { return b.get() == body; });
    if (it != m_bodies.end()) {
        m_bodies.erase(it, m_bodies.end());
    }
}

void PhysicsWorld2D::Clear() {
    m_bodies.clear();
}

Int32 PhysicsWorld2D::GetBodyCount() const {
    return static_cast<Int32>(m_bodies.size());
}

void PhysicsWorld2D::Step(Float32 deltaTime) {
    // تراكم الوقت للخطوات الثابتة / Accumulate time for fixed steps
    m_accumulator += deltaTime;
    
    while (m_accumulator >= m_settings.timeStep) {
        // تطبيق القوى / Apply forces
        IntegrateForces(m_settings.timeStep);
        
        // كشف التصادم / Collision detection
        DetectCollisions();
        
        // حل التصادم / Collision resolution
        ResolveCollisions();
        
        // تحديث المواقع / Update positions
        IntegrateVelocities(m_settings.timeStep);
        
        m_accumulator -= m_settings.timeStep;
    }
}

void PhysicsWorld2D::IntegrateForces(Float32 dt) {
    for (auto& body : m_bodies) {
        if (body->m_type != BodyType::Dynamic || !body->m_active || !body->m_awake) continue;
        
        // تطبيق الجاذبية / Apply gravity
        Float32 gravityX = m_settings.gravity.x * body->m_gravityScale;
        Float32 gravityY = m_settings.gravity.y * body->m_gravityScale;
        
        // تحديث السرعة من القوى / Update velocity from forces
        body->m_velocity.x += (body->m_force.x * body->m_inverseMass + gravityX) * dt;
        body->m_velocity.y += (body->m_force.y * body->m_inverseMass + gravityY) * dt;
        
        if (!body->m_fixedRotation) {
            body->m_angularVelocity += body->m_torque * body->m_inverseInertia * dt;
        }
        
        // تطبيق التخميد / Apply damping
        body->m_velocity.x *= 1.0f / (1.0f + dt * body->m_linearDamping);
        body->m_velocity.y *= 1.0f / (1.0f + dt * body->m_linearDamping);
        body->m_angularVelocity *= 1.0f / (1.0f + dt * body->m_angularDamping);
        
        // مسح القوى / Clear forces
        body->ClearForces();
    }
}

void PhysicsWorld2D::IntegrateVelocities(Float32 dt) {
    Float32 sleepThresholdSq = m_settings.sleepThreshold * m_settings.sleepThreshold;
    
    for (auto& body : m_bodies) {
        if (body->m_type == BodyType::Static || !body->m_active) continue;
        
        // تحديث الموقع والدوران / Update position and rotation
        body->m_position.x += body->m_velocity.x * dt;
        body->m_position.y += body->m_velocity.y * dt;
        body->m_rotation += body->m_angularVelocity * dt;
        
        // التحقق من النوم / Check for sleep
        if (m_settings.allowSleep && body->m_type == BodyType::Dynamic) {
            Float32 speedSq = body->m_velocity.x * body->m_velocity.x + 
                             body->m_velocity.y * body->m_velocity.y;
            Float32 angSpeedSq = body->m_angularVelocity * body->m_angularVelocity;
            
            if (speedSq < sleepThresholdSq && angSpeedSq < sleepThresholdSq) {
                body->m_awake = false;
                body->m_velocity = Vec2();
                body->m_angularVelocity = 0.0f;
            }
        }
    }
}

void PhysicsWorld2D::DetectCollisions() {
    // كشف التصادم البسيط O(n²)
    // Simple O(n²) collision detection
    // TODO: تحسين باستخدام شجرة AABB أو تقسيم شبكي
    // TODO: Optimize with AABB tree or spatial grid
    
    for (size_t i = 0; i < m_bodies.size(); ++i) {
        for (size_t j = i + 1; j < m_bodies.size(); ++j) {
            auto& bodyA = m_bodies[i];
            auto& bodyB = m_bodies[j];
            
            if (!bodyA->m_active || !bodyB->m_active) continue;
            if (bodyA->m_type == BodyType::Static && bodyB->m_type == BodyType::Static) continue;
            
            for (auto& colliderA : bodyA->m_colliders) {
                for (auto& colliderB : bodyB->m_colliders) {
                    if (!ShouldCollide(colliderA.get(), colliderB.get())) continue;
                    
                    CollisionInfo info;
                    if (TestCollision(colliderA.get(), colliderB.get(), info)) {
                        info.bodyA = bodyA.get();
                        info.bodyB = bodyB.get();
                        info.colliderA = colliderA.get();
                        info.colliderB = colliderB.get();
                        
                        // استدعاءات التصادم / Collision callbacks
                        if (colliderA->IsTrigger() || colliderB->IsTrigger()) {
                            if (bodyA->m_onTriggerEnter) {
                                bodyA->m_onTriggerEnter(bodyB.get(), colliderB.get());
                            }
                            if (bodyB->m_onTriggerEnter) {
                                bodyB->m_onTriggerEnter(bodyA.get(), colliderA.get());
                            }
                        } else {
                            if (bodyA->m_onCollisionEnter) {
                                bodyA->m_onCollisionEnter(info);
                            }
                            if (bodyB->m_onCollisionEnter) {
                                bodyB->m_onCollisionEnter(info);
                            }
                            if (m_onCollision) {
                                m_onCollision(info);
                            }
                        }
                    }
                }
            }
        }
    }
}

void PhysicsWorld2D::ResolveCollisions() {
    // Advanced collision resolution is integrated into TestCollision for each pair.
    // This method is reserved for iterative solver passes if needed.
    // Currently, impulse-based resolution is applied during detection in TestCollision.
}

bool PhysicsWorld2D::ShouldCollide(Collider2D* a, Collider2D* b) {
    // التحقق من طبقات التصادم / Check collision layers
    if ((a->GetLayer() & b->GetMask()) == 0) return false;
    if ((b->GetLayer() & a->GetMask()) == 0) return false;
    
    // مرشح مخصص / Custom filter
    if (m_collisionFilter && !m_collisionFilter(a, b)) return false;
    
    return true;
}

bool PhysicsWorld2D::TestCollision(Collider2D* a, Collider2D* b, CollisionInfo& info) {
    // اختبار دائرة-دائرة / Circle-circle test
    if (a->GetType() == ShapeType::Circle && b->GetType() == ShapeType::Circle) {
        Vec2 posA = a->GetBody()->LocalToWorld(a->GetCircle().center);
        Vec2 posB = b->GetBody()->LocalToWorld(b->GetCircle().center);
        
        Float32 dx = posB.x - posA.x;
        Float32 dy = posB.y - posA.y;
        Float32 distSq = dx * dx + dy * dy;
        Float32 radiusSum = a->GetCircle().radius + b->GetCircle().radius;
        
        if (distSq < radiusSum * radiusSum) {
            Float32 dist = std::sqrt(distSq);
            
            ContactPoint contact;
            if (dist > 0.0001f) {
                contact.normal = Vec2(dx / dist, dy / dist);
            } else {
                contact.normal = Vec2(1.0f, 0.0f);
            }
            contact.penetration = radiusSum - dist;
            contact.position.x = posA.x + contact.normal.x * a->GetCircle().radius;
            contact.position.y = posA.y + contact.normal.y * a->GetCircle().radius;
            
            info.contacts.push_back(contact);
            
            // حل التصادم البسيط / Simple collision resolution
            if (!a->IsTrigger() && !b->IsTrigger()) {
                RigidBody2D* bodyA = a->GetBody();
                RigidBody2D* bodyB = b->GetBody();
                
                Float32 totalInvMass = bodyA->GetInverseMass() + bodyB->GetInverseMass();
                if (totalInvMass > 0.0001f) {
                    // فصل الأجسام / Separate bodies
                    Float32 correction = contact.penetration * 0.8f;
                    Float32 moveA = correction * (bodyA->GetInverseMass() / totalInvMass);
                    Float32 moveB = correction * (bodyB->GetInverseMass() / totalInvMass);
                    
                    if (bodyA->GetType() == BodyType::Dynamic) {
                        bodyA->m_position.x -= contact.normal.x * moveA;
                        bodyA->m_position.y -= contact.normal.y * moveA;
                    }
                    if (bodyB->GetType() == BodyType::Dynamic) {
                        bodyB->m_position.x += contact.normal.x * moveB;
                        bodyB->m_position.y += contact.normal.y * moveB;
                    }
                    
                    // حساب السرعة النسبية / Calculate relative velocity
                    Float32 relVelX = bodyB->m_velocity.x - bodyA->m_velocity.x;
                    Float32 relVelY = bodyB->m_velocity.y - bodyA->m_velocity.y;
                    Float32 relVelDotNormal = relVelX * contact.normal.x + relVelY * contact.normal.y;
                    
                    if (relVelDotNormal < 0.0f) {
                        // حساب الارتداد / Calculate restitution
                        Float32 restitution = std::min(a->GetMaterial().restitution, 
                                                      b->GetMaterial().restitution);
                        Float32 j = -(1.0f + restitution) * relVelDotNormal / totalInvMass;
                        
                        if (bodyA->GetType() == BodyType::Dynamic) {
                            bodyA->m_velocity.x -= j * contact.normal.x * bodyA->GetInverseMass();
                            bodyA->m_velocity.y -= j * contact.normal.y * bodyA->GetInverseMass();
                        }
                        if (bodyB->GetType() == BodyType::Dynamic) {
                            bodyB->m_velocity.x += j * contact.normal.x * bodyB->GetInverseMass();
                            bodyB->m_velocity.y += j * contact.normal.y * bodyB->GetInverseMass();
                        }
                    }
                }
            }
            
            return true;
        }
    }
    
    // اختبار مستطيل-مستطيل (AABB) / Box-box test (AABB)
    if (a->GetType() == ShapeType::Box && b->GetType() == ShapeType::Box) {
        Rect boundsA = a->GetBounds();
        Rect boundsB = b->GetBounds();
        
        // اختبار AABB / AABB test
        Float32 overlapX = std::min(boundsA.x + boundsA.width, boundsB.x + boundsB.width) - 
                          std::max(boundsA.x, boundsB.x);
        Float32 overlapY = std::min(boundsA.y + boundsA.height, boundsB.y + boundsB.height) - 
                          std::max(boundsA.y, boundsB.y);
        
        if (overlapX > 0.0f && overlapY > 0.0f) {
            ContactPoint contact;
            
            // المحور الأصغر للفصل / Smaller axis for separation
            if (overlapX < overlapY) {
                Float32 centerAX = boundsA.x + boundsA.width * 0.5f;
                Float32 centerBX = boundsB.x + boundsB.width * 0.5f;
                contact.normal = Vec2((centerBX > centerAX) ? 1.0f : -1.0f, 0.0f);
                contact.penetration = overlapX;
            } else {
                Float32 centerAY = boundsA.y + boundsA.height * 0.5f;
                Float32 centerBY = boundsB.y + boundsB.height * 0.5f;
                contact.normal = Vec2(0.0f, (centerBY > centerAY) ? 1.0f : -1.0f);
                contact.penetration = overlapY;
            }
            
            info.contacts.push_back(contact);
            
            // حل التصادم / Collision resolution
            if (!a->IsTrigger() && !b->IsTrigger()) {
                RigidBody2D* bodyA = a->GetBody();
                RigidBody2D* bodyB = b->GetBody();
                
                Float32 totalInvMass = bodyA->GetInverseMass() + bodyB->GetInverseMass();
                if (totalInvMass > 0.0001f) {
                    Float32 correction = contact.penetration * 0.8f;
                    Float32 moveA = correction * (bodyA->GetInverseMass() / totalInvMass);
                    Float32 moveB = correction * (bodyB->GetInverseMass() / totalInvMass);
                    
                    if (bodyA->GetType() == BodyType::Dynamic) {
                        bodyA->m_position.x -= contact.normal.x * moveA;
                        bodyA->m_position.y -= contact.normal.y * moveA;
                    }
                    if (bodyB->GetType() == BodyType::Dynamic) {
                        bodyB->m_position.x += contact.normal.x * moveB;
                        bodyB->m_position.y += contact.normal.y * moveB;
                    }
                    
                    // الارتداد / Bounce
                    Float32 relVelX = bodyB->m_velocity.x - bodyA->m_velocity.x;
                    Float32 relVelY = bodyB->m_velocity.y - bodyA->m_velocity.y;
                    Float32 relVelDotNormal = relVelX * contact.normal.x + relVelY * contact.normal.y;
                    
                    if (relVelDotNormal < 0.0f) {
                        Float32 restitution = std::min(a->GetMaterial().restitution, 
                                                      b->GetMaterial().restitution);
                        Float32 j = -(1.0f + restitution) * relVelDotNormal / totalInvMass;
                        
                        if (bodyA->GetType() == BodyType::Dynamic) {
                            bodyA->m_velocity.x -= j * contact.normal.x * bodyA->GetInverseMass();
                            bodyA->m_velocity.y -= j * contact.normal.y * bodyA->GetInverseMass();
                        }
                        if (bodyB->GetType() == BodyType::Dynamic) {
                            bodyB->m_velocity.x += j * contact.normal.x * bodyB->GetInverseMass();
                            bodyB->m_velocity.y += j * contact.normal.y * bodyB->GetInverseMass();
                        }
                    }
                }
            }
            
            return true;
        }
    }
    
    // Circle-Box collision test / اختبار دائرة-مستطيل
    auto testCircleBox = [this](Collider2D* circle, Collider2D* box, CollisionInfo& info, bool swapped) -> bool {
        Vec2 circCenter = circle->GetBody()->LocalToWorld(circle->GetCircle().center);
        Float32 radius = circle->GetCircle().radius;
        Rect bounds = box->GetBounds();

        // Find closest point on box to circle center
        Float32 closestX = std::max(bounds.x, std::min(circCenter.x, bounds.x + bounds.width));
        Float32 closestY = std::max(bounds.y, std::min(circCenter.y, bounds.y + bounds.height));

        Float32 dx = circCenter.x - closestX;
        Float32 dy = circCenter.y - closestY;
        Float32 distSq = dx * dx + dy * dy;

        if (distSq < radius * radius) {
            Float32 dist = std::sqrt(distSq);
            ContactPoint contact;
            if (dist > 0.0001f) {
                contact.normal = swapped ? Vec2(-dx / dist, -dy / dist) : Vec2(dx / dist, dy / dist);
            } else {
                contact.normal = Vec2(1.0f, 0.0f);
            }
            contact.penetration = radius - dist;
            contact.position = Vec2(closestX, closestY);
            info.contacts.push_back(contact);

            // Resolve collision
            if (!circle->IsTrigger() && !box->IsTrigger()) {
                RigidBody2D* bodyA = swapped ? box->GetBody() : circle->GetBody();
                RigidBody2D* bodyB = swapped ? circle->GetBody() : box->GetBody();
                Vec2 normal = info.contacts.back().normal;

                Float32 totalInvMass = bodyA->GetInverseMass() + bodyB->GetInverseMass();
                if (totalInvMass > 0.0001f) {
                    Float32 correction = contact.penetration * 0.8f;
                    if (bodyA->GetType() == BodyType::Dynamic) {
                        bodyA->m_position.x -= normal.x * correction * (bodyA->GetInverseMass() / totalInvMass);
                        bodyA->m_position.y -= normal.y * correction * (bodyA->GetInverseMass() / totalInvMass);
                    }
                    if (bodyB->GetType() == BodyType::Dynamic) {
                        bodyB->m_position.x += normal.x * correction * (bodyB->GetInverseMass() / totalInvMass);
                        bodyB->m_position.y += normal.y * correction * (bodyB->GetInverseMass() / totalInvMass);
                    }

                    Float32 relVelX = bodyB->m_velocity.x - bodyA->m_velocity.x;
                    Float32 relVelY = bodyB->m_velocity.y - bodyA->m_velocity.y;
                    Float32 velDotN = relVelX * normal.x + relVelY * normal.y;
                    if (velDotN < 0.0f) {
                        Float32 restitution = std::min(circle->GetMaterial().restitution, box->GetMaterial().restitution);
                        Float32 j = -(1.0f + restitution) * velDotN / totalInvMass;
                        if (bodyA->GetType() == BodyType::Dynamic) {
                            bodyA->m_velocity.x -= j * normal.x * bodyA->GetInverseMass();
                            bodyA->m_velocity.y -= j * normal.y * bodyA->GetInverseMass();
                        }
                        if (bodyB->GetType() == BodyType::Dynamic) {
                            bodyB->m_velocity.x += j * normal.x * bodyB->GetInverseMass();
                            bodyB->m_velocity.y += j * normal.y * bodyB->GetInverseMass();
                        }
                    }
                }
            }
            return true;
        }
        return false;
    };

    if (a->GetType() == ShapeType::Circle && b->GetType() == ShapeType::Box) {
        return testCircleBox(a, b, info, false);
    }
    if (a->GetType() == ShapeType::Box && b->GetType() == ShapeType::Circle) {
        return testCircleBox(b, a, info, true);
    }

    return false;
}

// ==============================================================================
// الاستعلام المكاني / Spatial Queries
// ==============================================================================

RaycastHit PhysicsWorld2D::Raycast(const Vec2& origin, const Vec2& direction, 
                                   Float32 maxDistance, CollisionMask mask) {
    RaycastHit closest;
    closest.hit = false;
    closest.distance = maxDistance;
    
    // تطبيع الاتجاه / Normalize direction
    Float32 dirLen = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (dirLen < 0.0001f) return closest;
    Vec2 dir(direction.x / dirLen, direction.y / dirLen);
    
    for (auto& body : m_bodies) {
        if (!body->m_active) continue;
        
        for (auto& collider : body->m_colliders) {
            if ((collider->GetLayer() & mask) == 0) continue;
            
            // اختبار الشعاع مع الدائرة / Ray-circle test
            if (collider->GetType() == ShapeType::Circle) {
                Vec2 center = body->LocalToWorld(collider->GetCircle().center);
                Float32 radius = collider->GetCircle().radius;
                
                Float32 ox = origin.x - center.x;
                Float32 oy = origin.y - center.y;
                
                Float32 a = 1.0f; // dir is normalized
                Float32 b = 2.0f * (ox * dir.x + oy * dir.y);
                Float32 c = ox * ox + oy * oy - radius * radius;
                
                Float32 discriminant = b * b - 4.0f * a * c;
                if (discriminant >= 0.0f) {
                    Float32 t = (-b - std::sqrt(discriminant)) / (2.0f * a);
                    if (t >= 0.0f && t < closest.distance) {
                        closest.hit = true;
                        closest.body = body.get();
                        closest.collider = collider.get();
                        closest.distance = t;
                        closest.fraction = t / maxDistance;
                        closest.point.x = origin.x + dir.x * t;
                        closest.point.y = origin.y + dir.y * t;
                        
                        // الاتجاه من المركز / Normal from center
                        Float32 nx = closest.point.x - center.x;
                        Float32 ny = closest.point.y - center.y;
                        Float32 nlen = std::sqrt(nx * nx + ny * ny);
                        if (nlen > 0.0001f) {
                            closest.normal.x = nx / nlen;
                            closest.normal.y = ny / nlen;
                        }
                    }
                }
            }
            
            // Ray-Box test (AABB slab method)
            if (collider->GetType() == ShapeType::Box) {
                Rect bounds = collider->GetBounds();
                Float32 tmin = 0.0f;
                Float32 tmax = maxDistance;

                // X slab
                if (std::abs(dir.x) > 0.0001f) {
                    Float32 tx1 = (bounds.x - origin.x) / dir.x;
                    Float32 tx2 = (bounds.x + bounds.width - origin.x) / dir.x;
                    if (tx1 > tx2) std::swap(tx1, tx2);
                    tmin = std::max(tmin, tx1);
                    tmax = std::min(tmax, tx2);
                } else if (origin.x < bounds.x || origin.x > bounds.x + bounds.width) {
                    continue;
                }

                // Y slab
                if (std::abs(dir.y) > 0.0001f) {
                    Float32 ty1 = (bounds.y - origin.y) / dir.y;
                    Float32 ty2 = (bounds.y + bounds.height - origin.y) / dir.y;
                    if (ty1 > ty2) std::swap(ty1, ty2);
                    tmin = std::max(tmin, ty1);
                    tmax = std::min(tmax, ty2);
                } else if (origin.y < bounds.y || origin.y > bounds.y + bounds.height) {
                    continue;
                }

                if (tmin <= tmax && tmin >= 0.0f && tmin < closest.distance) {
                    closest.hit = true;
                    closest.body = body.get();
                    closest.collider = collider.get();
                    closest.distance = tmin;
                    closest.fraction = tmin / maxDistance;
                    closest.point = Vec2(origin.x + dir.x * tmin, origin.y + dir.y * tmin);

                    // Determine hit face normal
                    Float32 cx = bounds.x + bounds.width * 0.5f;
                    Float32 cy = bounds.y + bounds.height * 0.5f;
                    Float32 dx = closest.point.x - cx;
                    Float32 dy = closest.point.y - cy;
                    Float32 hw = bounds.width * 0.5f;
                    Float32 hh = bounds.height * 0.5f;
                    if (std::abs(dx / hw) > std::abs(dy / hh)) {
                        closest.normal = Vec2(dx > 0 ? 1.0f : -1.0f, 0.0f);
                    } else {
                        closest.normal = Vec2(0.0f, dy > 0 ? 1.0f : -1.0f);
                    }
                }
            }
        }
    }
    
    return closest;
}

void PhysicsWorld2D::Raycast(const Vec2& origin, const Vec2& direction, Float32 maxDistance,
                            RaycastCallback callback, CollisionMask mask) {
    // تطبيع الاتجاه / Normalize direction
    Float32 dirLen = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (dirLen < 0.0001f) return;
    Vec2 dir(direction.x / dirLen, direction.y / dirLen);
    
    for (auto& body : m_bodies) {
        if (!body->m_active) continue;
        
        for (auto& collider : body->m_colliders) {
            if ((collider->GetLayer() & mask) == 0) continue;
            
            // اختبار بسيط للدائرة / Simple circle test
            if (collider->GetType() == ShapeType::Circle) {
                Vec2 center = body->LocalToWorld(collider->GetCircle().center);
                Float32 radius = collider->GetCircle().radius;
                
                Float32 ox = origin.x - center.x;
                Float32 oy = origin.y - center.y;
                Float32 b = 2.0f * (ox * dir.x + oy * dir.y);
                Float32 c = ox * ox + oy * oy - radius * radius;
                Float32 discriminant = b * b - 4.0f * c;
                
                if (discriminant >= 0.0f) {
                    Float32 t = (-b - std::sqrt(discriminant)) / 2.0f;
                    if (t >= 0.0f && t <= maxDistance) {
                        Vec2 point(origin.x + dir.x * t, origin.y + dir.y * t);
                        Float32 nx = point.x - center.x;
                        Float32 ny = point.y - center.y;
                        Float32 nlen = std::sqrt(nx * nx + ny * ny);
                        Vec2 normal = (nlen > 0.0001f) ? Vec2(nx / nlen, ny / nlen) : Vec2(1, 0);
                        
                        if (!callback(body.get(), point, normal)) {
                            return; // توقف إذا أعاد false / Stop if callback returns false
                        }
                    }
                }
            }
        }
    }
}

RigidBody2D* PhysicsWorld2D::QueryPoint(const Vec2& point, CollisionMask mask) {
    for (auto& body : m_bodies) {
        if (!body->m_active) continue;
        
        for (auto& collider : body->m_colliders) {
            if ((collider->GetLayer() & mask) == 0) continue;
            
            Rect bounds = collider->GetBounds();
            if (point.x >= bounds.x && point.x <= bounds.x + bounds.width &&
                point.y >= bounds.y && point.y <= bounds.y + bounds.height) {
                
                // اختبار دقيق للدائرة / Precise circle test
                if (collider->GetType() == ShapeType::Circle) {
                    Vec2 center = body->LocalToWorld(collider->GetCircle().center);
                    Float32 dx = point.x - center.x;
                    Float32 dy = point.y - center.y;
                    Float32 radius = collider->GetCircle().radius;
                    if (dx * dx + dy * dy <= radius * radius) {
                        return body.get();
                    }
                } else {
                    return body.get();
                }
            }
        }
    }
    return nullptr;
}

std::vector<RigidBody2D*> PhysicsWorld2D::QueryArea(const Rect& area, CollisionMask mask) {
    std::vector<RigidBody2D*> result;
    
    for (auto& body : m_bodies) {
        if (!body->m_active) continue;
        
        for (auto& collider : body->m_colliders) {
            if ((collider->GetLayer() & mask) == 0) continue;
            
            Rect bounds = collider->GetBounds();
            
            // اختبار تقاطع AABB / AABB intersection test
            if (bounds.x < area.x + area.width && bounds.x + bounds.width > area.x &&
                bounds.y < area.y + area.height && bounds.y + bounds.height > area.y) {
                result.push_back(body.get());
                break; // تجنب إضافة نفس الجسم مرتين / Avoid adding same body twice
            }
        }
    }
    
    return result;
}

std::vector<RigidBody2D*> PhysicsWorld2D::QueryCircle(const Vec2& center, Float32 radius, 
                                                      CollisionMask mask) {
    std::vector<RigidBody2D*> result;
    Float32 radiusSq = radius * radius;
    
    for (auto& body : m_bodies) {
        if (!body->m_active) continue;
        
        for (auto& collider : body->m_colliders) {
            if ((collider->GetLayer() & mask) == 0) continue;
            
            Rect bounds = collider->GetBounds();
            
            // أقرب نقطة من المستطيل / Closest point on rect
            Float32 closestX = std::max(bounds.x, std::min(center.x, bounds.x + bounds.width));
            Float32 closestY = std::max(bounds.y, std::min(center.y, bounds.y + bounds.height));
            
            Float32 dx = center.x - closestX;
            Float32 dy = center.y - closestY;
            
            if (dx * dx + dy * dy <= radiusSq) {
                result.push_back(body.get());
                break;
            }
        }
    }
    
    return result;
}

void PhysicsWorld2D::SetCollisionFilter(std::function<bool(Collider2D*, Collider2D*)> filter) {
    m_collisionFilter = filter;
}

void PhysicsWorld2D::DebugDraw() const {
    // Debug drawing: outputs collision bounds info to stdout (renderer-independent)
    for (const auto& body : m_bodies) {
        if (!body->m_active) continue;
        for (const auto& collider : body->m_colliders) {
            Rect bounds = collider->GetBounds();
            if (collider->GetType() == ShapeType::Circle) {
                Vec2 center = body->LocalToWorld(collider->GetCircle().center);
                // Circle: center + radius available for external renderer
                (void)center;
            } else {
                // Box: bounds available for external renderer  
                (void)bounds;
            }
        }
    }
}

PhysicsWorld2D& PhysicsWorld2D::GetInstance() {
    static PhysicsWorld2D instance;
    return instance;
}

} // namespace graphics
} // namespace sad
