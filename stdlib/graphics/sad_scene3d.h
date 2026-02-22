// ═══════════════════════════════════════════════════════════════════════════════
// نظام المشاهد ثلاثي الأبعاد — لغة ص
// 3D Scene System — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// نظام رسم ثلاثي الأبعاد فوق SDL2 — Flutter لا يدعم 3D إطلاقاً!
//   - إسقاط منظوري (Perspective Projection)
//   - دوران ثلاثي المحاور (X, Y, Z)
//   - إضاءة أساسية (محيطية + اتجاهية)
//   - أشكال ثلاثية الأبعاد: مكعب، كرة، مخروط، أسطوانة، مستوى
//   - كاميرا قابلة للتحريك والتدوير
//   - ترتيب العمق (Z-sorting)
//   - شبكة (Wireframe) و ملء (Filled)
//   - حتى 1000 كائن ثلاثي الأبعاد
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <cmath>

namespace sad { namespace scene3d {

// ═══════════════════════════════════════════════════════════════════
// هياكل البيانات الأساسية
// ═══════════════════════════════════════════════════════════════════

// متجه ثلاثي الأبعاد
struct Vec3 {
    float x = 0, y = 0, z = 0;
    Vec3() = default;
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vec3 operator*(float s) const { return {x * s, y * s, z * s}; }
    float dot(const Vec3& o) const { return x * o.x + y * o.y + z * o.z; }
    Vec3 cross(const Vec3& o) const {
        return {y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x};
    }
    float length() const { return std::sqrt(x * x + y * y + z * z); }
    Vec3 normalized() const {
        float l = length();
        return l > 0.0001f ? Vec3{x / l, y / l, z / l} : Vec3{0, 0, 0};
    }
};

// مصفوفة 4×4 (للتحويلات ثلاثية الأبعاد)
struct Mat4 {
    float m[4][4] = {};
    static Mat4 identity();
    static Mat4 translation(float x, float y, float z);
    static Mat4 scale(float sx, float sy, float sz);
    static Mat4 rotationX(float angle); // بالراديان
    static Mat4 rotationY(float angle);
    static Mat4 rotationZ(float angle);
    static Mat4 perspective(float fov, float aspect, float near, float far);
    static Mat4 lookAt(const Vec3& eye, const Vec3& target, const Vec3& up);
    Mat4 operator*(const Mat4& o) const;
    Vec3 transform(const Vec3& v) const;
    Vec3 transformPoint(const Vec3& v) const; // مع القسمة المنظورية
};

// ─── نوع الكائن ───
enum class ObjectType {
    Cube,       // مكعب
    Sphere,     // كرة
    Cone,       // مخروط
    Cylinder,   // أسطوانة
    Plane,      // مستوى
    Line3D,     // خط ثلاثي الأبعاد
    Point3D,    // نقطة ثلاثية الأبعاد
    Custom      // مخصص (رؤوس يدوية)
};

// ─── وضع الرسم ───
enum class RenderMode {
    Filled,     // ملء
    Wireframe,  // شبكي
    Points      // نقاط فقط
};

// ─── مثلث ثلاثي الأبعاد (للرسم) ───
struct Triangle3D {
    Vec3 v[3];          // الرؤوس
    Vec3 normal;        // المتجه العمودي
    uint8_t r, g, b, a; // اللون
    float depth;        // العمق المتوسط (للترتيب)
};

// ─── الكائن ثلاثي الأبعاد ───
struct Object3D {
    int id = 0;
    ObjectType type = ObjectType::Cube;
    RenderMode mode = RenderMode::Filled;
    
    // التحويلات
    Vec3 position;
    Vec3 rotation;      // أويلر (بالدرجات)
    Vec3 scaleVec = {1, 1, 1};
    
    // اللون
    uint8_t r = 100, g = 150, b = 255, a = 255;
    
    // الحجم (حسب النوع)
    float size = 1.0f;      // للمكعب والكرة
    float radius = 0.5f;    // للكرة والمخروط والأسطوانة
    float height = 1.0f;    // للمخروط والأسطوانة
    int segments = 12;      // عدد أقسام الكرة/المخروط/الأسطوانة
    
    bool visible = true;
    bool castShadow = true;
    std::string tag;
};

// ─── الكاميرا ───
struct Camera {
    Vec3 position = {0, 2, -5};
    Vec3 target = {0, 0, 0};
    Vec3 up = {0, 1, 0};
    float fov = 60.0f;      // زاوية الرؤية (بالدرجات)
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
};

// ─── الإضاءة ───
struct Light {
    Vec3 direction = {-0.5f, -1.0f, 0.5f}; // اتجاه الضوء
    float ambient = 0.3f;                     // الإضاءة المحيطية (0-1)
    float diffuse = 0.7f;                     // الإضاءة الانتشارية (0-1)
    uint8_t r = 255, g = 255, b = 255;       // لون الضوء
};

// ═══════════════════════════════════════════════════════════════════
// واجهة برمجة المشاهد ثلاثية الأبعاد
// ═══════════════════════════════════════════════════════════════════

// ─── إدارة الكائنات ───
int createCube(float x, float y, float z, float size = 1.0f);
int createSphere(float x, float y, float z, float radius = 0.5f, int segments = 12);
int createCone(float x, float y, float z, float radius = 0.5f, float height = 1.0f);
int createCylinder(float x, float y, float z, float radius = 0.5f, float height = 1.0f);
int createPlane(float x, float y, float z, float size = 5.0f);
int createLine3D(float x1, float y1, float z1, float x2, float y2, float z2);
int createPoint3D(float x, float y, float z, float size = 3.0f);
void destroyObject(int id);
void destroyAllObjects();

// ─── تحويلات الكائن ───
void setObjectPosition(int id, float x, float y, float z);
void setObjectRotation(int id, float rx, float ry, float rz);     // بالدرجات
void setObjectScale(int id, float sx, float sy, float sz);
void setObjectColor(int id, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
void setObjectMode(int id, const std::string& mode); // "filled", "wireframe", "points"
void setObjectVisible(int id, bool visible);

// ─── تحريك الكائنات ───
void rotateObject(int id, float dx, float dy, float dz);   // إضافة دوران
void moveObject(int id, float dx, float dy, float dz);     // إضافة موضع

// ─── الكاميرا ───
void setCameraPosition(float x, float y, float z);
void setCameraTarget(float x, float y, float z);
void setCameraFOV(float fov);
void orbitCamera(float angleH, float angleV, float distance); // دوران حول الهدف
void moveCameraForward(float amount);
void moveCameraRight(float amount);
void moveCameraUp(float amount);

// ─── الإضاءة ───
void setLightDirection(float x, float y, float z);
void setLightColor(uint8_t r, uint8_t g, uint8_t b);
void setAmbientLight(float amount);
void setDiffuseLight(float amount);

// ─── الرسم ───
void render(void* renderer, int screenWidth, int screenHeight);

// ─── أدوات ───
void setBackgroundColor(uint8_t r, uint8_t g, uint8_t b);
int getObjectCount();
void resetScene();

// ─── أدوات رسم ثلاثي الأبعاد مباشرة ───
void drawLine3D(void* renderer, int scrW, int scrH,
                float x1, float y1, float z1,
                float x2, float y2, float z2,
                uint8_t r, uint8_t g, uint8_t b);
void drawGrid(void* renderer, int scrW, int scrH, float size, int divisions);
void drawAxes(void* renderer, int scrW, int scrH, float length);

}} // namespace sad::scene3d
