// ═══════════════════════════════════════════════════════════════════════════════
// محرك الفيزياء — لغة ص
// Physics Engine — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// محرك فيزياء ثنائي الأبعاد مدمج — Flutter ليس لديه أي محرك فيزياء مدمج!
//   - أجسام صلبة (Rigid Bodies) مع كتلة وسرعة وقوى
//   - كشف التصادم (Collision Detection): AABB + دائرة-دائرة + دائرة-مستطيل
//   - استجابة التصادم مع معامل الارتداد (restitution)
//   - الجاذبية والاحتكاك ومقاومة الهواء
//   - قيود (Constraints): مسافة ثابتة، نابض
//   - محاكاة فيزيائية بنظام Verlet المتكامل
//   - حتى 500 جسم بأداء عالٍ
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <cmath>
#include <functional>

namespace sad { namespace physics {

// ═══════════════════════════════════════════════════════════════════
// هياكل البيانات الأساسية
// ═══════════════════════════════════════════════════════════════════

// متجه ثنائي الأبعاد
struct Vec2 {
    float x = 0, y = 0;
    Vec2() = default;
    Vec2(float x, float y) : x(x), y(y) {}
    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(float s) const { return {x * s, y * s}; }
    float dot(const Vec2& o) const { return x * o.x + y * o.y; }
    float length() const { return std::sqrt(x * x + y * y); }
    float lengthSq() const { return x * x + y * y; }
    Vec2 normalized() const {
        float l = length();
        return l > 0.0001f ? Vec2{x / l, y / l} : Vec2{0, 0};
    }
};

// شكل الجسم
enum class BodyShape {
    Circle,     // دائرة
    Rectangle,  // مستطيل (AABB)
    Polygon     // مضلع   (مستقبلي)
};

// نوع الجسم
enum class BodyType {
    Dynamic,    // متحرك — يتأثر بالقوى والتصادمات
    Static,     // ثابت — لا يتحرك أبداً (أرضيات، جدران)
    Kinematic   // حركي — يتحرك يدوياً لكن لا تؤثر عليه القوى
};

// بنية الجسم الفيزيائي
struct Body {
    int id = 0;
    
    // ─── الموضع والسرعة ───
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    Vec2 force;      // القوة المتراكمة لهذا الإطار
    
    // ─── الخصائص الفيزيائية ───
    float mass = 1.0f;
    float invMass = 1.0f;       // 1/mass (0 للأجسام الثابتة)
    float restitution = 0.5f;   // معامل الارتداد (0 = لا ارتداد، 1 = مرن تماماً)
    float friction = 0.3f;      // الاحتكاك (0-1)
    float drag = 0.01f;         // مقاومة الهواء
    
    // ─── الشكل ───
    BodyShape shape = BodyShape::Circle;
    // للدائرة:
    float radius = 16.0f;
    // للمستطيل:
    float width = 32.0f;
    float height = 32.0f;
    
    // ─── النوع ───
    BodyType type = BodyType::Dynamic;
    
    // ─── حالة ───
    bool active = true;
    bool collidable = true;
    float rotation = 0.0f;      // الزاوية (بالراديان)
    float angularVelocity = 0.0f;
    
    // ─── بيانات المستخدم ───
    std::string tag;            // تسمية المستخدم (مثل "لاعب" أو "عدو")
    int userData = 0;           // قيمة مخصصة
    
    // ─── ألوان الرسم ───
    uint8_t colorR = 100, colorG = 150, colorB = 255, colorA = 255;
    bool drawOutline = false;
};

// معلومات التصادم
struct CollisionInfo {
    int bodyA;
    int bodyB;
    Vec2 normal;        // اتجاه التصادم (من A إلى B)
    float penetration;  // عمق التداخل
};

// نوع القيد
enum class ConstraintType {
    Distance,   // مسافة ثابتة بين جسمين
    Spring,     // نابض مرن
    Pin         // تثبيت في نقطة
};

// بنية القيد
struct Constraint {
    int id = 0;
    ConstraintType type;
    int bodyA;          // الجسم الأول
    int bodyB;          // الجسم الثاني (-1 للتثبيت)
    float targetLength; // الطول المستهدف
    float stiffness;    // الصلابة (0-1)
    float damping;      // التخميد
    Vec2 anchor;        // نقطة التثبيت (للدبوس)
    bool active = true;
};

// دالة استدعاء التصادم
using CollisionCallback = std::function<void(int bodyA, int bodyB)>;

// ═══════════════════════════════════════════════════════════════════
// واجهة برمجة محرك الفيزياء
// ═══════════════════════════════════════════════════════════════════

// ─── إنشاء الأجسام ───
int createCircle(float x, float y, float radius, float mass = 1.0f);
int createBox(float x, float y, float w, float h, float mass = 1.0f);
int createStaticPlatform(float x, float y, float w, float h);
int createStaticCircle(float x, float y, float radius);
int createKinematic(float x, float y, float w, float h);

// ─── حذف ───
void destroyBody(int id);
void destroyAllBodies();

// ─── خصائص الجسم ───
void setPosition(int id, float x, float y);
void setVelocity(int id, float vx, float vy);
void setMass(int id, float mass);
void setRestitution(int id, float rest);
void setFriction(int id, float fric);
void setDrag(int id, float drag);
void setRotation(int id, float angle);
void setTag(int id, const std::string& tag);
void setBodyColor(int id, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
void setCollidable(int id, bool collidable);
void setActive(int id, bool active);

// ─── القراءة ───
float getX(int id);
float getY(int id);
float getVX(int id);
float getVY(int id);
float getAngle(int id);
std::string getTag(int id);
bool isColliding(int idA, int idB);

// ─── القوى ───
void applyForce(int id, float fx, float fy);
void applyImpulse(int id, float ix, float iy);
void applyTorque(int id, float torque);

// ─── إعدادات العالم ───
void setWorldGravity(float gx, float gy);
void setWorldBounds(float left, float top, float right, float bottom);
void setWorldDamping(float damping);
void enableWorldBounds(bool enable);

// ─── القيود ───
int createDistanceConstraint(int bodyA, int bodyB, float length, float stiffness = 0.8f);
int createSpringConstraint(int bodyA, int bodyB, float length, float stiffness = 0.3f, float damping = 0.1f);
int createPinConstraint(int body, float anchorX, float anchorY);
void destroyConstraint(int id);

// ─── التصادمات ───
void setCollisionCallback(CollisionCallback cb);
int getCollisionCount();

// ─── المحاكاة ───
void step(float deltaMs);

// ─── الرسم التصحيحي ───
void debugRender(void* renderer);

// ─── الاستعلام ───
int getBodyCount();
int findBodyAt(float x, float y);             // أوجد جسم عند النقطة
std::vector<int> findBodiesInArea(float x, float y, float w, float h);

// ─── إعادة التهيئة ───
void resetWorld();

}} // namespace sad::physics
