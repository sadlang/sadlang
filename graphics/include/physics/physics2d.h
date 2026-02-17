// ==============================================================================
// physics2d.h - نظام الفيزياء ثنائي الأبعاد / 2D Physics System
// ==============================================================================
// الوصف: نظام فيزياء متكامل للألعاب ثنائية الأبعاد
// Description: Complete physics system for 2D games
// ==============================================================================
// المميزات / Features:
//   - أجسام صلبة / Rigid bodies
//   - أشكال التصادم / Collision shapes
//   - كشف التصادم / Collision detection
//   - الجاذبية والقوى / Gravity and forces
//   - المفاصل والقيود / Joints and constraints
//   - الاستعلام المكاني / Spatial queries
// ==============================================================================

#ifndef SAD_GRAPHICS_PHYSICS2D_H
#define SAD_GRAPHICS_PHYSICS2D_H

#include "../core/types.h"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace sad {
namespace graphics {

// ==============================================================================
// التصريحات المسبقة / Forward Declarations
// ==============================================================================

class RigidBody2D;
class Collider2D;
class PhysicsWorld2D;

// ==============================================================================
// المعرّفات / Identifiers
// ==============================================================================

/// معرّف الجسم / Body identifier
using BodyId = UInt32;

/// معرّف شكل التصادم / Collider identifier
using ColliderId = UInt32;

/// معرّف غير صالح / Invalid identifier
constexpr BodyId INVALID_BODY_ID = 0;
constexpr ColliderId INVALID_COLLIDER_ID = 0;

// ==============================================================================
// طبقات التصادم / Collision Layers
// ==============================================================================

/// قناع طبقة التصادم / Collision layer mask
using CollisionMask = UInt32;

/// الطبقات الافتراضية / Default layers
namespace CollisionLayer {
    constexpr CollisionMask None      = 0;
    constexpr CollisionMask Default   = 1 << 0;
    constexpr CollisionMask Player    = 1 << 1;
    constexpr CollisionMask Enemy     = 1 << 2;
    constexpr CollisionMask Platform  = 1 << 3;
    constexpr CollisionMask Projectile = 1 << 4;
    constexpr CollisionMask Trigger   = 1 << 5;
    constexpr CollisionMask All       = 0xFFFFFFFF;
}

// ==============================================================================
// أنواع الأجسام / Body Types
// ==============================================================================

/// نوع الجسم الفيزيائي / Physics body type
enum class BodyType {
    Static,             // ثابت - لا يتحرك / Static - doesn't move
    Kinematic,          // حركي - يتحرك برمجياً / Kinematic - moved programmatically
    Dynamic             // ديناميكي - يتأثر بالفيزياء / Dynamic - affected by physics
};

// ==============================================================================
// أنواع أشكال التصادم / Collider Shape Types
// ==============================================================================

/// نوع شكل التصادم / Collider shape type
enum class ShapeType {
    Circle,             // دائرة / Circle
    Box,                // مستطيل / Rectangle
    Polygon,            // مضلع / Polygon
    Edge,               // حافة / Edge
    Chain               // سلسلة حواف / Chain of edges
};

// ==============================================================================
// بيانات الشكل / Shape Data
// ==============================================================================

/// بيانات الدائرة / Circle data
struct CircleShape {
    Vec2 center;                // مركز الدائرة / Circle center (local)
    Float32 radius = 1.0f;      // نصف القطر / Radius
};

/// بيانات المستطيل / Box data
struct BoxShape {
    Vec2 center;                // المركز / Center (local)
    Vec2 halfExtents;           // نصف الأبعاد / Half extents
    Float32 angle = 0.0f;       // زاوية الدوران / Rotation angle
};

/// بيانات المضلع / Polygon data
struct PolygonShape {
    std::vector<Vec2> vertices; // الرؤوس (محلية) / Vertices (local)
};

/// بيانات الحافة / Edge data
struct EdgeShape {
    Vec2 start;                 // نقطة البداية / Start point
    Vec2 end;                   // نقطة النهاية / End point
};

// ==============================================================================
// خصائص المادة / Material Properties
// ==============================================================================

/// خصائص المادة الفيزيائية / Physics material properties
struct PhysicsMaterial {
    Float32 density = 1.0f;         // الكثافة / Density
    Float32 friction = 0.3f;        // الاحتكاك / Friction
    Float32 restitution = 0.0f;     // الارتداد (0-1) / Restitution (bounciness)
    
    /// مادة افتراضية / Default material
    static PhysicsMaterial Default() { return PhysicsMaterial(); }
    
    /// مادة مطاطية / Bouncy material
    static PhysicsMaterial Bouncy() {
        PhysicsMaterial m;
        m.restitution = 0.8f;
        return m;
    }
    
    /// مادة جليدية / Icy material
    static PhysicsMaterial Icy() {
        PhysicsMaterial m;
        m.friction = 0.01f;
        return m;
    }
    
    /// مادة لزجة / Sticky material
    static PhysicsMaterial Sticky() {
        PhysicsMaterial m;
        m.friction = 1.0f;
        return m;
    }
};

// ==============================================================================
// بيانات التصادم / Collision Data
// ==============================================================================

/// نقطة التماس / Contact point
struct ContactPoint {
    Vec2 position;              // موقع التماس / Contact position
    Vec2 normal;                // اتجاه التصادم / Collision normal
    Float32 penetration = 0.0f; // عمق الاختراق / Penetration depth
};

/// بيانات التصادم / Collision data
struct CollisionInfo {
    RigidBody2D* bodyA = nullptr;
    RigidBody2D* bodyB = nullptr;
    Collider2D* colliderA = nullptr;
    Collider2D* colliderB = nullptr;
    std::vector<ContactPoint> contacts;
    Vec2 relativeVelocity;          // السرعة النسبية / Relative velocity
    Float32 totalImpulse = 0.0f;    // القوة الإجمالية / Total impulse
};

// ==============================================================================
// أنواع الاستدعاءات / Callback Types
// ==============================================================================

using CollisionCallback = std::function<void(const CollisionInfo& info)>;
using TriggerCallback = std::function<void(RigidBody2D* body, Collider2D* trigger)>;
using RaycastCallback = std::function<bool(RigidBody2D* body, const Vec2& point, const Vec2& normal)>;

// ==============================================================================
// نتيجة الشعاع / Raycast Result
// ==============================================================================

/// نتيجة إسقاط الشعاع / Raycast hit result
struct RaycastHit {
    bool hit = false;               // هل أصاب شيئاً / Did it hit something
    RigidBody2D* body = nullptr;    // الجسم المصاب / Hit body
    Collider2D* collider = nullptr; // شكل التصادم / Hit collider
    Vec2 point;                     // نقطة الإصابة / Hit point
    Vec2 normal;                    // اتجاه السطح / Surface normal
    Float32 distance = 0.0f;        // المسافة / Distance
    Float32 fraction = 0.0f;        // النسبة من الشعاع / Fraction of ray
};

// ==============================================================================
// شكل التصادم / Collider
// ==============================================================================

/// شكل التصادم / Collision shape
class Collider2D {
public:
    // ============================================================================
    // الإنشاء / Construction
    // ============================================================================
    
    /// منشئ دائرة / Circle constructor
    static std::shared_ptr<Collider2D> CreateCircle(Float32 radius, const Vec2& center = Vec2());
    
    /// منشئ مستطيل / Box constructor
    static std::shared_ptr<Collider2D> CreateBox(Float32 width, Float32 height, const Vec2& center = Vec2());
    
    /// منشئ مضلع / Polygon constructor
    static std::shared_ptr<Collider2D> CreatePolygon(const std::vector<Vec2>& vertices);
    
    /// منشئ حافة / Edge constructor
    static std::shared_ptr<Collider2D> CreateEdge(const Vec2& start, const Vec2& end);
    
    // ============================================================================
    // الخصائص / Properties
    // ============================================================================
    
    /// الحصول على المعرّف / Get ID
    ColliderId GetId() const { return m_id; }
    
    /// الحصول على النوع / Get type
    ShapeType GetType() const { return m_type; }
    
    /// الحصول على الجسم المالك / Get owning body
    RigidBody2D* GetBody() const { return m_body; }
    
    /// هل هو مُشغّل (لا يصطدم فعلياً) / Is trigger (doesn't physically collide)
    bool IsTrigger() const { return m_isTrigger; }
    void SetTrigger(bool trigger) { m_isTrigger = trigger; }
    
    /// خصائص المادة / Material properties
    const PhysicsMaterial& GetMaterial() const { return m_material; }
    void SetMaterial(const PhysicsMaterial& material) { m_material = material; }
    
    /// طبقة التصادم / Collision layer
    CollisionMask GetLayer() const { return m_layer; }
    void SetLayer(CollisionMask layer) { m_layer = layer; }
    
    /// قناع التصادم / Collision mask
    CollisionMask GetMask() const { return m_mask; }
    void SetMask(CollisionMask mask) { m_mask = mask; }
    
    /// بيانات المستخدم / User data
    void* GetUserData() const { return m_userData; }
    void SetUserData(void* data) { m_userData = data; }
    
    // ============================================================================
    // بيانات الشكل / Shape Data
    // ============================================================================
    
    /// الحصول على بيانات الدائرة / Get circle data
    const CircleShape& GetCircle() const { return m_circle; }
    
    /// الحصول على بيانات المستطيل / Get box data
    const BoxShape& GetBox() const { return m_box; }
    
    /// الحصول على بيانات المضلع / Get polygon data
    const PolygonShape& GetPolygon() const { return m_polygon; }
    
    /// الحصول على بيانات الحافة / Get edge data
    const EdgeShape& GetEdge() const { return m_edge; }
    
    /// حساب صندوق الإحاطة / Calculate bounding box
    Rect GetBounds() const;

private:
    friend class RigidBody2D;
    friend class PhysicsWorld2D;
    
    ColliderId m_id = INVALID_COLLIDER_ID;
    ShapeType m_type = ShapeType::Circle;
    RigidBody2D* m_body = nullptr;
    bool m_isTrigger = false;
    PhysicsMaterial m_material;
    CollisionMask m_layer = CollisionLayer::Default;
    CollisionMask m_mask = CollisionLayer::All;
    void* m_userData = nullptr;
    
    // بيانات الأشكال / Shape data (union-like, only one is valid based on m_type)
    CircleShape m_circle;
    BoxShape m_box;
    PolygonShape m_polygon;
    EdgeShape m_edge;
};

// ==============================================================================
// الجسم الصلب / Rigid Body
// ==============================================================================

/// الجسم الصلب ثنائي الأبعاد / 2D rigid body
class RigidBody2D {
public:
    // ============================================================================
    // الإنشاء والتدمير / Construction and Destruction
    // ============================================================================
    
    RigidBody2D() = default;
    ~RigidBody2D() = default;
    
    // ============================================================================
    // الخصائص الأساسية / Basic Properties
    // ============================================================================
    
    /// الحصول على المعرّف / Get ID
    BodyId GetId() const { return m_id; }
    
    /// الحصول على نوع الجسم / Get body type
    BodyType GetType() const { return m_type; }
    void SetType(BodyType type) { m_type = type; }
    
    /// هل الجسم نشط / Is body active
    bool IsActive() const { return m_active; }
    void SetActive(bool active) { m_active = active; }
    
    /// هل الجسم قيد الاستيقاظ / Is body awake
    bool IsAwake() const { return m_awake; }
    void SetAwake(bool awake) { m_awake = awake; }
    
    /// هل الجسم يدور / Can body rotate
    bool IsFixedRotation() const { return m_fixedRotation; }
    void SetFixedRotation(bool fixed) { m_fixedRotation = fixed; }
    
    // ============================================================================
    // التحويلات / Transform
    // ============================================================================
    
    /// الموقع / Position
    const Vec2& GetPosition() const { return m_position; }
    void SetPosition(const Vec2& pos);
    void SetPosition(Float32 x, Float32 y);
    
    /// الدوران / Rotation (radians)
    Float32 GetRotation() const { return m_rotation; }
    void SetRotation(Float32 angle);
    
    /// تحويل من المحلي إلى العالمي / Transform local to world
    Vec2 LocalToWorld(const Vec2& local) const;
    
    /// تحويل من العالمي إلى المحلي / Transform world to local
    Vec2 WorldToLocal(const Vec2& world) const;
    
    // ============================================================================
    // الحركة / Motion
    // ============================================================================
    
    /// السرعة الخطية / Linear velocity
    const Vec2& GetVelocity() const { return m_velocity; }
    void SetVelocity(const Vec2& vel) { m_velocity = vel; }
    void SetVelocity(Float32 x, Float32 y) { m_velocity = Vec2(x, y); }
    
    /// السرعة الزاوية / Angular velocity
    Float32 GetAngularVelocity() const { return m_angularVelocity; }
    void SetAngularVelocity(Float32 omega) { m_angularVelocity = omega; }
    
    /// التخميد الخطي / Linear damping
    Float32 GetLinearDamping() const { return m_linearDamping; }
    void SetLinearDamping(Float32 damping) { m_linearDamping = damping; }
    
    /// التخميد الزاوي / Angular damping
    Float32 GetAngularDamping() const { return m_angularDamping; }
    void SetAngularDamping(Float32 damping) { m_angularDamping = damping; }
    
    /// مقياس الجاذبية / Gravity scale
    Float32 GetGravityScale() const { return m_gravityScale; }
    void SetGravityScale(Float32 scale) { m_gravityScale = scale; }
    
    // ============================================================================
    // الكتلة / Mass
    // ============================================================================
    
    /// الكتلة / Mass
    Float32 GetMass() const { return m_mass; }
    void SetMass(Float32 mass);
    
    /// معكوس الكتلة / Inverse mass
    Float32 GetInverseMass() const { return m_inverseMass; }
    
    /// عزم القصور الذاتي / Inertia
    Float32 GetInertia() const { return m_inertia; }
    void SetInertia(Float32 inertia);
    
    /// معكوس عزم القصور الذاتي / Inverse inertia
    Float32 GetInverseInertia() const { return m_inverseInertia; }
    
    // ============================================================================
    // القوى / Forces
    // ============================================================================
    
    /// تطبيق قوة / Apply force
    void ApplyForce(const Vec2& force);
    void ApplyForce(const Vec2& force, const Vec2& point);
    
    /// تطبيق دفعة / Apply impulse
    void ApplyImpulse(const Vec2& impulse);
    void ApplyImpulse(const Vec2& impulse, const Vec2& point);
    
    /// تطبيق عزم دوران / Apply torque
    void ApplyTorque(Float32 torque);
    
    /// تطبيق دفعة زاوية / Apply angular impulse
    void ApplyAngularImpulse(Float32 impulse);
    
    /// مسح القوى / Clear forces
    void ClearForces() { m_force = Vec2(); m_torque = 0.0f; }
    
    // ============================================================================
    // أشكال التصادم / Colliders
    // ============================================================================
    
    /// إضافة شكل تصادم / Add collider
    Collider2D* AddCollider(std::shared_ptr<Collider2D> collider);
    
    /// إزالة شكل تصادم / Remove collider
    void RemoveCollider(Collider2D* collider);
    
    /// الحصول على أشكال التصادم / Get colliders
    const std::vector<std::shared_ptr<Collider2D>>& GetColliders() const { return m_colliders; }
    
    // ============================================================================
    // الاستدعاءات / Callbacks
    // ============================================================================
    
    /// عند بدء التصادم / On collision start
    void SetOnCollisionEnter(CollisionCallback callback) { m_onCollisionEnter = callback; }
    
    /// عند انتهاء التصادم / On collision end
    void SetOnCollisionExit(CollisionCallback callback) { m_onCollisionExit = callback; }
    
    /// عند بدء التشغيل / On trigger enter
    void SetOnTriggerEnter(TriggerCallback callback) { m_onTriggerEnter = callback; }
    
    /// عند انتهاء التشغيل / On trigger exit
    void SetOnTriggerExit(TriggerCallback callback) { m_onTriggerExit = callback; }
    
    // ============================================================================
    // بيانات المستخدم / User Data
    // ============================================================================
    
    void* GetUserData() const { return m_userData; }
    void SetUserData(void* data) { m_userData = data; }
    
    /// العلامة / Tag
    const std::string& GetTag() const { return m_tag; }
    void SetTag(const std::string& tag) { m_tag = tag; }

private:
    friend class PhysicsWorld2D;
    
    BodyId m_id = INVALID_BODY_ID;
    BodyType m_type = BodyType::Dynamic;
    bool m_active = true;
    bool m_awake = true;
    bool m_fixedRotation = false;
    
    // التحويلات / Transform
    Vec2 m_position;
    Float32 m_rotation = 0.0f;
    
    // الحركة / Motion
    Vec2 m_velocity;
    Float32 m_angularVelocity = 0.0f;
    Float32 m_linearDamping = 0.0f;
    Float32 m_angularDamping = 0.0f;
    Float32 m_gravityScale = 1.0f;
    
    // الكتلة / Mass
    Float32 m_mass = 1.0f;
    Float32 m_inverseMass = 1.0f;
    Float32 m_inertia = 1.0f;
    Float32 m_inverseInertia = 1.0f;
    
    // القوى / Forces
    Vec2 m_force;
    Float32 m_torque = 0.0f;
    
    // أشكال التصادم / Colliders
    std::vector<std::shared_ptr<Collider2D>> m_colliders;
    
    // الاستدعاءات / Callbacks
    CollisionCallback m_onCollisionEnter;
    CollisionCallback m_onCollisionExit;
    TriggerCallback m_onTriggerEnter;
    TriggerCallback m_onTriggerExit;
    
    // بيانات المستخدم / User data
    void* m_userData = nullptr;
    std::string m_tag;
};

// ==============================================================================
// عالم الفيزياء / Physics World
// ==============================================================================

/// إعدادات عالم الفيزياء / Physics world settings
struct PhysicsSettings {
    Vec2 gravity = Vec2(0.0f, 980.0f);  // الجاذبية (بكسل/ث²) / Gravity (pixels/s²)
    Int32 velocityIterations = 8;        // تكرارات السرعة / Velocity iterations
    Int32 positionIterations = 3;        // تكرارات الموقع / Position iterations
    Float32 timeStep = 1.0f / 60.0f;     // خطوة الوقت / Time step
    bool allowSleep = true;              // السماح بالنوم / Allow sleeping
    Float32 sleepThreshold = 0.5f;       // عتبة النوم / Sleep threshold
};

/// عالم الفيزياء ثنائي الأبعاد / 2D physics world
class PhysicsWorld2D {
public:
    // ============================================================================
    // الإنشاء والتدمير / Construction and Destruction
    // ============================================================================
    
    PhysicsWorld2D();
    PhysicsWorld2D(const PhysicsSettings& settings);
    ~PhysicsWorld2D();
    
    // ============================================================================
    // الإعدادات / Settings
    // ============================================================================
    
    /// الحصول على الإعدادات / Get settings
    const PhysicsSettings& GetSettings() const { return m_settings; }
    
    /// تعيين الجاذبية / Set gravity
    void SetGravity(const Vec2& gravity) { m_settings.gravity = gravity; }
    void SetGravity(Float32 x, Float32 y) { m_settings.gravity = Vec2(x, y); }
    
    /// الحصول على الجاذبية / Get gravity
    const Vec2& GetGravity() const { return m_settings.gravity; }
    
    // ============================================================================
    // إدارة الأجسام / Body Management
    // ============================================================================
    
    /// إنشاء جسم / Create body
    RigidBody2D* CreateBody(BodyType type = BodyType::Dynamic);
    
    /// إنشاء جسم ثابت / Create static body
    RigidBody2D* CreateStaticBody();
    
    /// إنشاء جسم حركي / Create kinematic body
    RigidBody2D* CreateKinematicBody();
    
    /// إنشاء جسم ديناميكي / Create dynamic body
    RigidBody2D* CreateDynamicBody();
    
    /// تدمير جسم / Destroy body
    void DestroyBody(RigidBody2D* body);
    
    /// تدمير جميع الأجسام / Destroy all bodies
    void Clear();
    
    /// عدد الأجسام / Body count
    Int32 GetBodyCount() const;
    
    /// الحصول على الأجسام / Get bodies
    const std::vector<std::unique_ptr<RigidBody2D>>& GetBodies() const { return m_bodies; }
    
    // ============================================================================
    // المحاكاة / Simulation
    // ============================================================================
    
    /// خطوة المحاكاة / Step simulation
    void Step(Float32 deltaTime);
    
    /// خطوة ثابتة / Fixed step
    void FixedStep() { Step(m_settings.timeStep); }
    
    // ============================================================================
    // الاستعلام المكاني / Spatial Queries
    // ============================================================================
    
    /// إسقاط شعاع / Raycast
    RaycastHit Raycast(const Vec2& origin, const Vec2& direction, Float32 maxDistance,
                       CollisionMask mask = CollisionLayer::All);
    
    /// إسقاط شعاع مع استدعاء / Raycast with callback
    void Raycast(const Vec2& origin, const Vec2& direction, Float32 maxDistance,
                 RaycastCallback callback, CollisionMask mask = CollisionLayer::All);
    
    /// استعلام نقطة / Point query
    RigidBody2D* QueryPoint(const Vec2& point, CollisionMask mask = CollisionLayer::All);
    
    /// استعلام منطقة / Area query
    std::vector<RigidBody2D*> QueryArea(const Rect& area, CollisionMask mask = CollisionLayer::All);
    
    /// استعلام دائرة / Circle query
    std::vector<RigidBody2D*> QueryCircle(const Vec2& center, Float32 radius, 
                                          CollisionMask mask = CollisionLayer::All);
    
    // ============================================================================
    // الاستدعاءات العامة / Global Callbacks
    // ============================================================================
    
    /// عند أي تصادم / On any collision
    void SetOnCollision(CollisionCallback callback) { m_onCollision = callback; }
    
    /// معالجة التصادم المخصصة / Custom collision handling
    void SetCollisionFilter(std::function<bool(Collider2D*, Collider2D*)> filter);
    
    // ============================================================================
    // التصحيح / Debug
    // ============================================================================
    
    /// رسم حدود التصادم / Draw collision bounds
    void DebugDraw() const;
    
    /// الحصول على النمط الواحد / Get singleton instance
    static PhysicsWorld2D& GetInstance();

private:
    // ============================================================================
    // البيانات الداخلية / Internal Data
    // ============================================================================
    
    PhysicsSettings m_settings;
    std::vector<std::unique_ptr<RigidBody2D>> m_bodies;
    BodyId m_nextBodyId = 1;
    ColliderId m_nextColliderId = 1;
    
    CollisionCallback m_onCollision;
    std::function<bool(Collider2D*, Collider2D*)> m_collisionFilter;
    
    Float32 m_accumulator = 0.0f;
    
    // ============================================================================
    // الدوال الداخلية / Internal Functions
    // ============================================================================
    
    void IntegrateForces(Float32 dt);
    void IntegrateVelocities(Float32 dt);
    void DetectCollisions();
    void ResolveCollisions();
    bool ShouldCollide(Collider2D* a, Collider2D* b);
    bool TestCollision(Collider2D* a, Collider2D* b, CollisionInfo& info);
};

// ==============================================================================
// دوال مساعدة / Helper Functions
// ==============================================================================

/// حساب كتلة الدائرة / Calculate circle mass
inline Float32 CalculateCircleMass(Float32 radius, Float32 density) {
    return 3.14159265358979f * radius * radius * density;
}

/// حساب كتلة المستطيل / Calculate box mass
inline Float32 CalculateBoxMass(Float32 width, Float32 height, Float32 density) {
    return width * height * density;
}

/// حساب عزم القصور الذاتي للدائرة / Calculate circle inertia
inline Float32 CalculateCircleInertia(Float32 mass, Float32 radius) {
    return 0.5f * mass * radius * radius;
}

/// حساب عزم القصور الذاتي للمستطيل / Calculate box inertia
inline Float32 CalculateBoxInertia(Float32 mass, Float32 width, Float32 height) {
    return (1.0f / 12.0f) * mass * (width * width + height * height);
}

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_PHYSICS2D_H
