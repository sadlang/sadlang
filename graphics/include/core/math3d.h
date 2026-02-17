// ============================================================================
// math3d.h - رياضيات ثلاثية الأبعاد متقدمة
// Advanced 3D Mathematics
// ============================================================================
// الوصف: يحتوي على الهياكل الرياضية المتقدمة للرسومات ثلاثية الأبعاد
// Description: Contains advanced mathematical structures for 3D graphics
// ============================================================================

#ifndef SAD_GRAPHICS_MATH3D_H
#define SAD_GRAPHICS_MATH3D_H

#include "types.h"
#include <algorithm>
#include <limits>
#include <cmath>
#include <array>
#include <vector>

namespace sad {
namespace graphics {

// ============================================================================
// الثوابت الرياضية / Mathematical Constants
// ============================================================================

/// النسبة π / Pi constant
constexpr Float32 PI = 3.14159265358979323846f;
/// نصف π / Half Pi
constexpr Float32 HALF_PI = PI / 2.0f;
/// π × 2 / Two Pi
constexpr Float32 TWO_PI = PI * 2.0f;
/// تحويل الدرجات إلى راديان / Degrees to radians
constexpr Float32 DEG_TO_RAD = PI / 180.0f;
/// تحويل الراديان إلى درجات / Radians to degrees
constexpr Float32 RAD_TO_DEG = 180.0f / PI;
/// قيمة إبسيلون للمقارنة العائمة / Epsilon for float comparison
constexpr Float32 EPSILON = 1e-6f;
/// ما لا نهاية / Infinity
constexpr Float32 INFINITY_F = std::numeric_limits<Float32>::infinity();

// ============================================================================
// دوال رياضية مساعدة / Math Helper Functions
// ============================================================================

/// تحويل درجات إلى راديان / Convert degrees to radians
inline Float32 Radians(Float32 degrees) { return degrees * DEG_TO_RAD; }

/// تحويل راديان إلى درجات / Convert radians to degrees
inline Float32 Degrees(Float32 radians) { return radians * RAD_TO_DEG; }

/// حصر قيمة ضمن نطاق / Clamp value to range
inline Float32 Clamp(Float32 value, Float32 min_, Float32 max_) {
    return (value < min_) ? min_ : (value > max_) ? max_ : value;
}

/// حصر قيمة بين 0 و 1 / Saturate (clamp 0-1)
inline Float32 Saturate(Float32 value) { return Clamp(value, 0.0f, 1.0f); }

/// الاستيفاء الخطي / Linear interpolation
inline Float32 Lerp(Float32 a, Float32 b, Float32 t) { return a + (b - a) * t; }

/// الاستيفاء الناعم / Smooth step interpolation
inline Float32 SmoothStep(Float32 edge0, Float32 edge1, Float32 x) {
    Float32 t = Saturate((x - edge0) / (edge1 - edge0));
    return t * t * (3.0f - 2.0f * t);
}

/// الاستيفاء الأكثر نعومة / Smoother step
inline Float32 SmootherStep(Float32 edge0, Float32 edge1, Float32 x) {
    Float32 t = Saturate((x - edge0) / (edge1 - edge0));
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

/// التفاف الزاوية / Wrap angle to [-π, π]
inline Float32 WrapAngle(Float32 radians) {
    radians = std::fmod(radians + PI, TWO_PI);
    if (radians < 0) radians += TWO_PI;
    return radians - PI;
}

/// أقرب قوة لـ 2 / Next power of 2
inline UInt32 NextPowerOf2(UInt32 n) {
    n--;
    n |= n >> 1; n |= n >> 2; n |= n >> 4; n |= n >> 8; n |= n >> 16;
    return n + 1;
}

/// هل القوة 2 / Is power of 2
inline bool IsPowerOf2(UInt32 n) { return n && !(n & (n - 1)); }

/// الإشارة / Sign
inline Float32 Sign(Float32 value) { return (value > 0.0f) ? 1.0f : (value < 0.0f) ? -1.0f : 0.0f; }

/// القيمة المطلقة / Absolute value
inline Float32 Abs(Float32 value) { return std::abs(value); }

/// الحد الأدنى / Minimum
inline Float32 Min(Float32 a, Float32 b) { return (a < b) ? a : b; }

/// الحد الأقصى / Maximum
inline Float32 Max(Float32 a, Float32 b) { return (a > b) ? a : b; }

/// المسافة بين عددين / Distance between two values
inline Float32 Distance(Float32 a, Float32 b) { return Abs(a - b); }

// ============================================================================
// الرباعية (Quaternion) - للدورات ثلاثية الأبعاد
// Quaternion - For 3D Rotations
// ============================================================================

/// رباعية للدوران ثلاثي الأبعاد / Quaternion for 3D rotations
struct Quat {
    Float32 x, y, z, w;
    
    // ============================================
    // المنشئات / Constructors
    // ============================================
    
    /// المنشئ الافتراضي - رباعية الهوية / Default - identity quaternion
    Quat() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    
    /// منشئ بقيم / Constructor with values
    Quat(Float32 x_, Float32 y_, Float32 z_, Float32 w_) : x(x_), y(y_), z(z_), w(w_) {}
    
    /// منشئ من محور وزاوية / Constructor from axis and angle
    static Quat FromAxisAngle(const Vec3& axis, Float32 angleRadians) {
        Float32 halfAngle = angleRadians * 0.5f;
        Float32 s = std::sin(halfAngle);
        Vec3 n = axis.Normalized();
        return Quat(n.x * s, n.y * s, n.z * s, std::cos(halfAngle));
    }
    
    /// منشئ من زوايا أويلر (YXZ) / Constructor from Euler angles (YXZ order)
    static Quat FromEuler(Float32 pitch, Float32 yaw, Float32 roll) {
        Float32 cy = std::cos(yaw * 0.5f);
        Float32 sy = std::sin(yaw * 0.5f);
        Float32 cp = std::cos(pitch * 0.5f);
        Float32 sp = std::sin(pitch * 0.5f);
        Float32 cr = std::cos(roll * 0.5f);
        Float32 sr = std::sin(roll * 0.5f);
        
        return Quat(
            cy * sp * cr + sy * cp * sr,
            sy * cp * cr - cy * sp * sr,
            cy * cp * sr - sy * sp * cr,
            cy * cp * cr + sy * sp * sr
        );
    }
    
    /// منشئ من Vec3 (زوايا أويلر) / Constructor from Vec3 (Euler angles)
    static Quat FromEuler(const Vec3& eulerRadians) {
        return FromEuler(eulerRadians.x, eulerRadians.y, eulerRadians.z);
    }
    
    /// منشئ من مصفوفة دوران / Constructor from rotation matrix
    static Quat FromMatrix(const Mat4& m);
    
    /// منشئ للنظر نحو اتجاه / Constructor to look at direction
    static Quat LookRotation(const Vec3& forward, const Vec3& up = Vec3::Up());
    
    // ============================================
    // العمليات / Operations
    // ============================================
    
    /// ضرب الرباعيات / Quaternion multiplication
    Quat operator*(const Quat& q) const {
        return Quat(
            w * q.x + x * q.w + y * q.z - z * q.y,
            w * q.y - x * q.z + y * q.w + z * q.x,
            w * q.z + x * q.y - y * q.x + z * q.w,
            w * q.w - x * q.x - y * q.y - z * q.z
        );
    }
    
    /// تدوير متجه / Rotate vector
    Vec3 operator*(const Vec3& v) const {
        Vec3 qv(x, y, z);
        Vec3 uv = qv.Cross(v);
        Vec3 uuv = qv.Cross(uv);
        return v + ((uv * w) + uuv) * 2.0f;
    }
    
    /// الجمع / Addition
    Quat operator+(const Quat& q) const { return Quat(x + q.x, y + q.y, z + q.z, w + q.w); }
    
    /// الضرب بعدد / Scalar multiplication
    Quat operator*(Float32 s) const { return Quat(x * s, y * s, z * s, w * s); }
    
    /// السلب / Negation
    Quat operator-() const { return Quat(-x, -y, -z, -w); }
    
    /// المساواة / Equality
    bool operator==(const Quat& q) const { return x == q.x && y == q.y && z == q.z && w == q.w; }
    bool operator!=(const Quat& q) const { return !(*this == q); }
    
    // ============================================
    // الدوال الرياضية / Math Functions
    // ============================================
    
    /// الجداء النقطي / Dot product
    Float32 Dot(const Quat& q) const { return x * q.x + y * q.y + z * q.z + w * q.w; }
    
    /// مربع الطول / Squared length
    Float32 LengthSq() const { return x * x + y * y + z * z + w * w; }
    
    /// الطول / Length
    Float32 Length() const { return std::sqrt(LengthSq()); }
    
    /// التطبيع / Normalize
    Quat Normalized() const {
        Float32 len = Length();
        if (len < EPSILON) return Identity();
        Float32 inv = 1.0f / len;
        return Quat(x * inv, y * inv, z * inv, w * inv);
    }
    
    /// تطبيع في المكان / Normalize in place
    void Normalize() { *this = Normalized(); }
    
    /// المقلوب / Conjugate
    Quat Conjugate() const { return Quat(-x, -y, -z, w); }
    
    /// المعكوس / Inverse
    Quat Inverse() const {
        Float32 lenSq = LengthSq();
        if (lenSq < EPSILON) return Identity();
        Float32 inv = 1.0f / lenSq;
        return Quat(-x * inv, -y * inv, -z * inv, w * inv);
    }
    
    /// تحويل إلى زوايا أويلر / Convert to Euler angles
    Vec3 ToEuler() const {
        Vec3 angles;
        
        // الدحرجة (roll - X axis rotation)
        Float32 sinr_cosp = 2.0f * (w * x + y * z);
        Float32 cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
        angles.z = std::atan2(sinr_cosp, cosr_cosp);
        
        // الميلان (pitch - Y axis rotation)
        Float32 sinp = 2.0f * (w * y - z * x);
        if (std::abs(sinp) >= 1.0f)
            angles.x = std::copysign(HALF_PI, sinp);
        else
            angles.x = std::asin(sinp);
        
        // الانعراج (yaw - Z axis rotation)
        Float32 siny_cosp = 2.0f * (w * z + x * y);
        Float32 cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
        angles.y = std::atan2(siny_cosp, cosy_cosp);
        
        return angles;
    }
    
    /// تحويل إلى محور وزاوية / Convert to axis-angle
    void ToAxisAngle(Vec3& axis, Float32& angle) const {
        Float32 scale = std::sqrt(x * x + y * y + z * z);
        if (scale < EPSILON) {
            axis = Vec3::UnitX();
            angle = 0.0f;
        } else {
            axis = Vec3(x / scale, y / scale, z / scale);
            angle = 2.0f * std::acos(Clamp(w, -1.0f, 1.0f));
        }
    }
    
    /// تحويل إلى مصفوفة / Convert to matrix
    Mat4 ToMatrix() const;
    
    /// الزاوية بين رباعيتين / Angle between two quaternions
    Float32 AngleTo(const Quat& q) const {
        Float32 dot = Abs(Dot(q));
        return std::acos(Clamp(dot, -1.0f, 1.0f)) * 2.0f;
    }
    
    // ============================================
    // الاستيفاء / Interpolation
    // ============================================
    
    /// الاستيفاء الخطي / Linear interpolation (LERP)
    Quat Lerp(const Quat& q, Float32 t) const {
        Float32 dot = Dot(q);
        Quat target = (dot < 0.0f) ? -q : q;
        return (*this * (1.0f - t) + target * t).Normalized();
    }
    
    /// الاستيفاء الكروي / Spherical interpolation (SLERP)
    Quat Slerp(const Quat& q, Float32 t) const {
        Float32 dot = Dot(q);
        Quat target = (dot < 0.0f) ? -q : q;
        dot = Abs(dot);
        
        if (dot > 0.9995f) {
            return Lerp(target, t);
        }
        
        Float32 theta = std::acos(Clamp(dot, -1.0f, 1.0f));
        Float32 sinTheta = std::sin(theta);
        Float32 scale0 = std::sin((1.0f - t) * theta) / sinTheta;
        Float32 scale1 = std::sin(t * theta) / sinTheta;
        
        return (*this * scale0 + target * scale1).Normalized();
    }
    
    // ============================================
    // الاتجاهات / Directions
    // ============================================
    
    /// متجه الأمام / Forward vector
    Vec3 Forward() const { return *this * Vec3::Forward(); }
    
    /// متجه الأعلى / Up vector
    Vec3 Up() const { return *this * Vec3::Up(); }
    
    /// متجه اليمين / Right vector
    Vec3 Right() const { return *this * Vec3::Right(); }
    
    // ============================================
    // الثوابت / Constants
    // ============================================
    
    /// رباعية الهوية / Identity quaternion
    static Quat Identity() { return Quat(0.0f, 0.0f, 0.0f, 1.0f); }
};

inline Quat operator*(Float32 s, const Quat& q) { return q * s; }

// ============================================================================
// التحويل ثلاثي الأبعاد / 3D Transform
// ============================================================================

/// تحويل ثلاثي الأبعاد كامل / Complete 3D Transform
struct Transform3D {
    Vec3 position;   ///< الموقع / Position
    Quat rotation;   ///< الدوران / Rotation
    Vec3 scale;      ///< المقياس / Scale
    
    /// المنشئ الافتراضي / Default constructor
    Transform3D() : position(Vec3::Zero()), rotation(Quat::Identity()), scale(Vec3::One()) {}
    
    /// منشئ بموقع فقط / Constructor with position only
    explicit Transform3D(const Vec3& pos)
        : position(pos), rotation(Quat::Identity()), scale(Vec3::One()) {}
    
    /// منشئ بموقع ودوران / Constructor with position and rotation
    Transform3D(const Vec3& pos, const Quat& rot)
        : position(pos), rotation(rot), scale(Vec3::One()) {}
    
    /// منشئ كامل / Full constructor
    Transform3D(const Vec3& pos, const Quat& rot, const Vec3& scl)
        : position(pos), rotation(rot), scale(scl) {}
    
    /// تحويل إلى مصفوفة / Convert to matrix
    Mat4 ToMatrix() const {
        Mat4 result = rotation.ToMatrix();
        result.m[0] *= scale.x; result.m[1] *= scale.x; result.m[2] *= scale.x;
        result.m[4] *= scale.y; result.m[5] *= scale.y; result.m[6] *= scale.y;
        result.m[8] *= scale.z; result.m[9] *= scale.z; result.m[10] *= scale.z;
        result.m[12] = position.x;
        result.m[13] = position.y;
        result.m[14] = position.z;
        return result;
    }
    
    /// المصفوفة المعكوسة / Inverse matrix
    Mat4 ToInverseMatrix() const {
        Vec3 invScale(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);
        Quat invRot = rotation.Inverse();
        Vec3 invPos = invRot * (-position * invScale);
        
        Mat4 result = invRot.ToMatrix();
        result.m[0] *= invScale.x; result.m[1] *= invScale.x; result.m[2] *= invScale.x;
        result.m[4] *= invScale.y; result.m[5] *= invScale.y; result.m[6] *= invScale.y;
        result.m[8] *= invScale.z; result.m[9] *= invScale.z; result.m[10] *= invScale.z;
        result.m[12] = invPos.x;
        result.m[13] = invPos.y;
        result.m[14] = invPos.z;
        return result;
    }
    
    /// تحويل نقطة / Transform point
    Vec3 TransformPoint(const Vec3& p) const {
        return position + rotation * (p * scale);
    }
    
    /// تحويل اتجاه / Transform direction
    Vec3 TransformDirection(const Vec3& d) const {
        return rotation * (d * scale);
    }
    
    /// تحويل عكسي لنقطة / Inverse transform point
    Vec3 InverseTransformPoint(const Vec3& p) const {
        return rotation.Inverse() * (p - position) / scale;
    }
    
    /// تحويل عكسي لاتجاه / Inverse transform direction
    Vec3 InverseTransformDirection(const Vec3& d) const {
        return rotation.Inverse() * d / scale;
    }
    
    /// دمج تحويلين / Combine transforms
    Transform3D operator*(const Transform3D& t) const {
        return Transform3D(
            TransformPoint(t.position),
            rotation * t.rotation,
            scale * t.scale
        );
    }
    
    /// الاستيفاء / Interpolation
    Transform3D Lerp(const Transform3D& t, Float32 alpha) const {
        return Transform3D(
            position.Lerp(t.position, alpha),
            rotation.Slerp(t.rotation, alpha),
            scale.Lerp(t.scale, alpha)
        );
    }
    
    /// متجه الأمام / Forward vector
    Vec3 Forward() const { return rotation.Forward(); }
    
    /// متجه الأعلى / Up vector
    Vec3 Up() const { return rotation.Up(); }
    
    /// متجه اليمين / Right vector
    Vec3 Right() const { return rotation.Right(); }
    
    /// التحويل الهوية / Identity transform
    static Transform3D Identity() { return Transform3D(); }
};

// ============================================================================
// الشعاع / Ray
// ============================================================================

/// شعاع في الفضاء ثلاثي الأبعاد / Ray in 3D space
struct Ray {
    Vec3 origin;    ///< نقطة البداية / Start point
    Vec3 direction; ///< الاتجاه (معياري) / Direction (normalized)
    
    /// المنشئ الافتراضي / Default constructor
    Ray() : origin(Vec3::Zero()), direction(Vec3::Forward()) {}
    
    /// منشئ بنقطة واتجاه / Constructor with origin and direction
    Ray(const Vec3& orig, const Vec3& dir) : origin(orig), direction(dir.Normalized()) {}
    
    /// الحصول على نقطة على الشعاع / Get point on ray
    Vec3 GetPoint(Float32 t) const { return origin + direction * t; }
    
    /// أقرب نقطة على الشعاع من نقطة معينة / Closest point on ray to a point
    Vec3 ClosestPoint(const Vec3& point) const {
        Vec3 toPoint = point - origin;
        Float32 t = Max(0.0f, toPoint.Dot(direction));
        return GetPoint(t);
    }
    
    /// المسافة إلى نقطة / Distance to point
    Float32 DistanceToPoint(const Vec3& point) const {
        return (point - ClosestPoint(point)).Length();
    }
};

// ============================================================================
// المستوى / Plane
// ============================================================================

/// مستوى في الفضاء ثلاثي الأبعاد / Plane in 3D space
struct Plane {
    Vec3 normal;   ///< المتجه العمودي / Normal vector
    Float32 d;     ///< المسافة من الأصل / Distance from origin
    
    /// المنشئ الافتراضي (مستوى XZ) / Default (XZ plane)
    Plane() : normal(Vec3::UnitY()), d(0.0f) {}
    
    /// منشئ بعمودي ومسافة / Constructor with normal and distance
    Plane(const Vec3& n, Float32 dist) : normal(n.Normalized()), d(dist) {}
    
    /// منشئ بعمودي ونقطة / Constructor with normal and point
    Plane(const Vec3& n, const Vec3& point) : normal(n.Normalized()), d(-normal.Dot(point)) {}
    
    /// منشئ من ثلاث نقاط / Constructor from three points
    static Plane FromPoints(const Vec3& a, const Vec3& b, const Vec3& c) {
        Vec3 normal = (b - a).Cross(c - a).Normalized();
        return Plane(normal, a);
    }
    
    /// المسافة الموقعة من نقطة / Signed distance from point
    Float32 SignedDistance(const Vec3& point) const {
        return normal.Dot(point) + d;
    }
    
    /// المسافة المطلقة من نقطة / Absolute distance from point
    Float32 Distance(const Vec3& point) const {
        return Abs(SignedDistance(point));
    }
    
    /// إسقاط نقطة على المستوى / Project point onto plane
    Vec3 ProjectPoint(const Vec3& point) const {
        return point - normal * SignedDistance(point);
    }
    
    /// تقاطع مع شعاع / Intersection with ray
    bool Raycast(const Ray& ray, Float32& t) const {
        Float32 denom = normal.Dot(ray.direction);
        if (Abs(denom) < EPSILON) return false;
        t = -(normal.Dot(ray.origin) + d) / denom;
        return t >= 0.0f;
    }
    
    /// تصنيف النقطة / Classify point
    enum class Side { Front, Back, OnPlane };
    Side ClassifyPoint(const Vec3& point) const {
        Float32 dist = SignedDistance(point);
        if (dist > EPSILON) return Side::Front;
        if (dist < -EPSILON) return Side::Back;
        return Side::OnPlane;
    }
};

// ============================================================================
// الكرة / Sphere
// ============================================================================

/// كرة في الفضاء ثلاثي الأبعاد / Sphere in 3D space
struct Sphere {
    Vec3 center;    ///< المركز / Center
    Float32 radius; ///< نصف القطر / Radius
    
    /// المنشئ الافتراضي / Default constructor
    Sphere() : center(Vec3::Zero()), radius(1.0f) {}
    
    /// منشئ بمركز ونصف قطر / Constructor with center and radius
    Sphere(const Vec3& c, Float32 r) : center(c), radius(r) {}
    
    /// هل تحتوي نقطة / Contains point
    bool Contains(const Vec3& point) const {
        return (point - center).LengthSq() <= radius * radius;
    }
    
    /// هل تتقاطع مع كرة أخرى / Intersects another sphere
    bool Intersects(const Sphere& other) const {
        Float32 totalRadius = radius + other.radius;
        return (center - other.center).LengthSq() <= totalRadius * totalRadius;
    }
    
    /// تقاطع مع شعاع / Ray intersection
    bool Raycast(const Ray& ray, Float32& t1, Float32& t2) const {
        Vec3 oc = ray.origin - center;
        Float32 a = ray.direction.Dot(ray.direction);
        Float32 b = 2.0f * oc.Dot(ray.direction);
        Float32 c = oc.Dot(oc) - radius * radius;
        Float32 discriminant = b * b - 4.0f * a * c;
        
        if (discriminant < 0.0f) return false;
        
        Float32 sqrtD = std::sqrt(discriminant);
        t1 = (-b - sqrtD) / (2.0f * a);
        t2 = (-b + sqrtD) / (2.0f * a);
        return true;
    }
    
    /// تقاطع مع شعاع (أقرب نقطة فقط) / Ray intersection (closest point only)
    bool Raycast(const Ray& ray, Float32& t) const {
        Float32 t1, t2;
        if (!Raycast(ray, t1, t2)) return false;
        t = (t1 >= 0.0f) ? t1 : t2;
        return t >= 0.0f;
    }
    
    /// إنشاء كرة محيطة بمجموعة نقاط / Create bounding sphere from points
    static Sphere FromPoints(const Vec3* points, size_t count) {
        if (count == 0) return Sphere();
        
        // حساب المركز الوسطي / Calculate centroid
        Vec3 center = Vec3::Zero();
        for (size_t i = 0; i < count; i++) {
            center = center + points[i];
        }
        center = center / static_cast<Float32>(count);
        
        // إيجاد أبعد نقطة / Find farthest point
        Float32 maxDistSq = 0.0f;
        for (size_t i = 0; i < count; i++) {
            Float32 distSq = (points[i] - center).LengthSq();
            maxDistSq = Max(maxDistSq, distSq);
        }
        
        return Sphere(center, std::sqrt(maxDistSq));
    }
};

// ============================================================================
// صندوق الحدود المحاذي للمحاور / Axis-Aligned Bounding Box (AABB)
// ============================================================================

/// صندوق حدود محاذي للمحاور / Axis-Aligned Bounding Box
struct AABB {
    Vec3 min; ///< الركن الأدنى / Minimum corner
    Vec3 max; ///< الركن الأعلى / Maximum corner
    
    /// المنشئ الافتراضي / Default constructor
    AABB() : min(Vec3::Zero()), max(Vec3::Zero()) {}
    
    /// منشئ بالأركان / Constructor with corners
    AABB(const Vec3& minCorner, const Vec3& maxCorner) : min(minCorner), max(maxCorner) {}
    
    /// منشئ من مركز ونصف أبعاد / Constructor from center and half-extents
    static AABB FromCenterExtents(const Vec3& center, const Vec3& extents) {
        return AABB(center - extents, center + extents);
    }
    
    /// منشئ من مجموعة نقاط / Constructor from points
    static AABB FromPoints(const Vec3* points, size_t count) {
        if (count == 0) return AABB();
        
        Vec3 min = points[0];
        Vec3 max = points[0];
        
        for (size_t i = 1; i < count; i++) {
            min.x = Min(min.x, points[i].x);
            min.y = Min(min.y, points[i].y);
            min.z = Min(min.z, points[i].z);
            max.x = Max(max.x, points[i].x);
            max.y = Max(max.y, points[i].y);
            max.z = Max(max.z, points[i].z);
        }
        
        return AABB(min, max);
    }
    
    /// المركز / Center
    Vec3 Center() const { return (min + max) * 0.5f; }
    
    /// الأبعاد / Size
    Vec3 Size() const { return max - min; }
    
    /// نصف الأبعاد / Half extents
    Vec3 Extents() const { return (max - min) * 0.5f; }
    
    /// هل يحتوي نقطة / Contains point
    bool Contains(const Vec3& point) const {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y &&
               point.z >= min.z && point.z <= max.z;
    }
    
    /// هل يحتوي صندوق آخر / Contains another AABB
    bool Contains(const AABB& other) const {
        return Contains(other.min) && Contains(other.max);
    }
    
    /// هل يتقاطع مع صندوق آخر / Intersects another AABB
    bool Intersects(const AABB& other) const {
        return min.x <= other.max.x && max.x >= other.min.x &&
               min.y <= other.max.y && max.y >= other.min.y &&
               min.z <= other.max.z && max.z >= other.min.z;
    }
    
    /// هل يتقاطع مع كرة / Intersects sphere
    bool Intersects(const Sphere& sphere) const {
        Vec3 closest = ClosestPoint(sphere.center);
        return (closest - sphere.center).LengthSq() <= sphere.radius * sphere.radius;
    }
    
    /// أقرب نقطة في الصندوق من نقطة معينة / Closest point on AABB to a point
    Vec3 ClosestPoint(const Vec3& point) const {
        return Vec3(
            Clamp(point.x, min.x, max.x),
            Clamp(point.y, min.y, max.y),
            Clamp(point.z, min.z, max.z)
        );
    }
    
    /// المسافة إلى نقطة / Distance to point
    Float32 Distance(const Vec3& point) const {
        return (point - ClosestPoint(point)).Length();
    }
    
    /// تقاطع مع شعاع / Ray intersection
    bool Raycast(const Ray& ray, Float32& tMin, Float32& tMax) const {
        tMin = 0.0f;
        tMax = INFINITY_F;
        
        for (int i = 0; i < 3; i++) {
            Float32 invD = 1.0f / ray.direction[i];
            Float32 t0 = (min[i] - ray.origin[i]) * invD;
            Float32 t1 = (max[i] - ray.origin[i]) * invD;
            
            if (invD < 0.0f) {
                Float32 temp = t0;
                t0 = t1;
                t1 = temp;
            }
            
            tMin = Max(tMin, t0);
            tMax = Min(tMax, t1);
            
            if (tMax < tMin) return false;
        }
        
        return true;
    }
    
    /// تقاطع مع شعاع (أقرب نقطة فقط) / Ray intersection (closest point only)
    bool Raycast(const Ray& ray, Float32& t) const {
        Float32 tMin, tMax;
        if (!Raycast(ray, tMin, tMax)) return false;
        t = (tMin >= 0.0f) ? tMin : tMax;
        return t >= 0.0f;
    }
    
    /// توسيع لاحتواء نقطة / Expand to include point
    void Expand(const Vec3& point) {
        min.x = Min(min.x, point.x);
        min.y = Min(min.y, point.y);
        min.z = Min(min.z, point.z);
        max.x = Max(max.x, point.x);
        max.y = Max(max.y, point.y);
        max.z = Max(max.z, point.z);
    }
    
    /// توسيع لاحتواء صندوق آخر / Expand to include another AABB
    void Expand(const AABB& other) {
        min.x = Min(min.x, other.min.x);
        min.y = Min(min.y, other.min.y);
        min.z = Min(min.z, other.min.z);
        max.x = Max(max.x, other.max.x);
        max.y = Max(max.y, other.max.y);
        max.z = Max(max.z, other.max.z);
    }
    
    /// اتحاد صندوقين / Union of two AABBs
    static AABB Union(const AABB& a, const AABB& b) {
        return AABB(
            Vec3(Min(a.min.x, b.min.x), Min(a.min.y, b.min.y), Min(a.min.z, b.min.z)),
            Vec3(Max(a.max.x, b.max.x), Max(a.max.y, b.max.y), Max(a.max.z, b.max.z))
        );
    }
    
    /// تقاطع صندوقين / Intersection of two AABBs
    static AABB Intersection(const AABB& a, const AABB& b) {
        return AABB(
            Vec3(Max(a.min.x, b.min.x), Max(a.min.y, b.min.y), Max(a.min.z, b.min.z)),
            Vec3(Min(a.max.x, b.max.x), Min(a.max.y, b.max.y), Min(a.max.z, b.max.z))
        );
    }
    
    /// الأركان الثمانية / Eight corners
    std::array<Vec3, 8> GetCorners() const {
        return {{
            Vec3(min.x, min.y, min.z),
            Vec3(max.x, min.y, min.z),
            Vec3(min.x, max.y, min.z),
            Vec3(max.x, max.y, min.z),
            Vec3(min.x, min.y, max.z),
            Vec3(max.x, min.y, max.z),
            Vec3(min.x, max.y, max.z),
            Vec3(max.x, max.y, max.z)
        }};
    }
    
    /// تحويل AABB بمصفوفة / Transform AABB by matrix
    AABB Transformed(const Mat4& m) const {
        auto corners = GetCorners();
        Vec3 newMin = m.TransformPoint(corners[0]);
        Vec3 newMax = newMin;
        
        for (int i = 1; i < 8; i++) {
            Vec3 p = m.TransformPoint(corners[i]);
            newMin.x = Min(newMin.x, p.x);
            newMin.y = Min(newMin.y, p.y);
            newMin.z = Min(newMin.z, p.z);
            newMax.x = Max(newMax.x, p.x);
            newMax.y = Max(newMax.y, p.y);
            newMax.z = Max(newMax.z, p.z);
        }
        
        return AABB(newMin, newMax);
    }
};

// ============================================================================
// صندوق الحدود الموجه / Oriented Bounding Box (OBB)
// ============================================================================

/// صندوق حدود موجه / Oriented Bounding Box
struct OBB {
    Vec3 center;        ///< المركز / Center
    Vec3 extents;       ///< نصف الأبعاد / Half extents
    Quat orientation;   ///< التوجيه / Orientation
    
    /// المنشئ الافتراضي / Default constructor
    OBB() : center(Vec3::Zero()), extents(Vec3(0.5f)), orientation(Quat::Identity()) {}
    
    /// منشئ كامل / Full constructor
    OBB(const Vec3& c, const Vec3& ext, const Quat& orient)
        : center(c), extents(ext), orientation(orient) {}
    
    /// إنشاء من AABB / Create from AABB
    static OBB FromAABB(const AABB& aabb) {
        return OBB(aabb.Center(), aabb.Extents(), Quat::Identity());
    }
    
    /// المحاور المحلية / Local axes
    Vec3 AxisX() const { return orientation * Vec3::UnitX(); }
    Vec3 AxisY() const { return orientation * Vec3::UnitY(); }
    Vec3 AxisZ() const { return orientation * Vec3::UnitZ(); }
    
    /// الأركان الثمانية / Eight corners
    std::array<Vec3, 8> GetCorners() const {
        Vec3 ax = AxisX() * extents.x;
        Vec3 ay = AxisY() * extents.y;
        Vec3 az = AxisZ() * extents.z;
        
        return {{
            center - ax - ay - az,
            center + ax - ay - az,
            center - ax + ay - az,
            center + ax + ay - az,
            center - ax - ay + az,
            center + ax - ay + az,
            center - ax + ay + az,
            center + ax + ay + az
        }};
    }
    
    /// هل يحتوي نقطة / Contains point
    bool Contains(const Vec3& point) const {
        Vec3 local = orientation.Inverse() * (point - center);
        return Abs(local.x) <= extents.x &&
               Abs(local.y) <= extents.y &&
               Abs(local.z) <= extents.z;
    }
    
    /// أقرب نقطة في الصندوق / Closest point on OBB
    Vec3 ClosestPoint(const Vec3& point) const {
        Vec3 local = orientation.Inverse() * (point - center);
        local.x = Clamp(local.x, -extents.x, extents.x);
        local.y = Clamp(local.y, -extents.y, extents.y);
        local.z = Clamp(local.z, -extents.z, extents.z);
        return center + orientation * local;
    }
    
    /// تحويل إلى AABB / Convert to AABB
    AABB ToAABB() const {
        auto corners = GetCorners();
        return AABB::FromPoints(corners.data(), 8);
    }
};

// ============================================================================
// هرم الرؤية / View Frustum
// ============================================================================

/// هرم الرؤية للكاميرا / Camera view frustum
struct Frustum {
    /// المستويات الستة / Six planes
    enum PlaneIndex { Near = 0, Far, Left, Right, Top, Bottom, Count };
    Plane planes[Count];
    
    /// المنشئ الافتراضي / Default constructor
    Frustum() = default;
    
    /// إنشاء من مصفوفة View-Projection / Create from View-Projection matrix
    static Frustum FromMatrix(const Mat4& viewProj) {
        Frustum f;
        
        // المستوى الأيسر / Left plane
        f.planes[Left] = Plane(
            Vec3(viewProj.m[3] + viewProj.m[0],
                 viewProj.m[7] + viewProj.m[4],
                 viewProj.m[11] + viewProj.m[8]),
            viewProj.m[15] + viewProj.m[12]
        );
        
        // المستوى الأيمن / Right plane
        f.planes[Right] = Plane(
            Vec3(viewProj.m[3] - viewProj.m[0],
                 viewProj.m[7] - viewProj.m[4],
                 viewProj.m[11] - viewProj.m[8]),
            viewProj.m[15] - viewProj.m[12]
        );
        
        // المستوى السفلي / Bottom plane
        f.planes[Bottom] = Plane(
            Vec3(viewProj.m[3] + viewProj.m[1],
                 viewProj.m[7] + viewProj.m[5],
                 viewProj.m[11] + viewProj.m[9]),
            viewProj.m[15] + viewProj.m[13]
        );
        
        // المستوى العلوي / Top plane
        f.planes[Top] = Plane(
            Vec3(viewProj.m[3] - viewProj.m[1],
                 viewProj.m[7] - viewProj.m[5],
                 viewProj.m[11] - viewProj.m[9]),
            viewProj.m[15] - viewProj.m[13]
        );
        
        // المستوى القريب / Near plane
        f.planes[Near] = Plane(
            Vec3(viewProj.m[3] + viewProj.m[2],
                 viewProj.m[7] + viewProj.m[6],
                 viewProj.m[11] + viewProj.m[10]),
            viewProj.m[15] + viewProj.m[14]
        );
        
        // المستوى البعيد / Far plane
        f.planes[Far] = Plane(
            Vec3(viewProj.m[3] - viewProj.m[2],
                 viewProj.m[7] - viewProj.m[6],
                 viewProj.m[11] - viewProj.m[10]),
            viewProj.m[15] - viewProj.m[14]
        );
        
        // تطبيع جميع المستويات / Normalize all planes
        for (int i = 0; i < Count; i++) {
            Float32 len = f.planes[i].normal.Length();
            if (len > EPSILON) {
                f.planes[i].normal = f.planes[i].normal / len;
                f.planes[i].d /= len;
            }
        }
        
        return f;
    }
    
    /// نتيجة اختبار التقاطع / Intersection test result
    enum class TestResult { Outside, Inside, Intersecting };
    
    /// اختبار نقطة / Test point
    bool ContainsPoint(const Vec3& point) const {
        for (int i = 0; i < Count; i++) {
            if (planes[i].SignedDistance(point) < 0.0f) {
                return false;
            }
        }
        return true;
    }
    
    /// اختبار كرة / Test sphere
    TestResult TestSphere(const Sphere& sphere) const {
        bool allInside = true;
        
        for (int i = 0; i < Count; i++) {
            Float32 dist = planes[i].SignedDistance(sphere.center);
            if (dist < -sphere.radius) return TestResult::Outside;
            if (dist < sphere.radius) allInside = false;
        }
        
        return allInside ? TestResult::Inside : TestResult::Intersecting;
    }
    
    /// اختبار صندوق / Test AABB
    TestResult TestAABB(const AABB& aabb) const {
        bool allInside = true;
        
        for (int i = 0; i < Count; i++) {
            const Plane& plane = planes[i];
            
            // إيجاد النقطة الأقرب والأبعد عن المستوى / Find closest and farthest points
            Vec3 pVertex(
                (plane.normal.x >= 0) ? aabb.max.x : aabb.min.x,
                (plane.normal.y >= 0) ? aabb.max.y : aabb.min.y,
                (plane.normal.z >= 0) ? aabb.max.z : aabb.min.z
            );
            
            Vec3 nVertex(
                (plane.normal.x >= 0) ? aabb.min.x : aabb.max.x,
                (plane.normal.y >= 0) ? aabb.min.y : aabb.max.y,
                (plane.normal.z >= 0) ? aabb.min.z : aabb.max.z
            );
            
            if (plane.SignedDistance(pVertex) < 0.0f) {
                return TestResult::Outside;
            }
            
            if (plane.SignedDistance(nVertex) < 0.0f) {
                allInside = false;
            }
        }
        
        return allInside ? TestResult::Inside : TestResult::Intersecting;
    }
    
    /// اختبار سريع لكرة (فقط داخل/خارج) / Fast sphere test (only inside/outside)
    bool IntersectsSphere(const Sphere& sphere) const {
        return TestSphere(sphere) != TestResult::Outside;
    }
    
    /// اختبار سريع لصندوق / Fast AABB test
    bool IntersectsAABB(const AABB& aabb) const {
        return TestAABB(aabb) != TestResult::Outside;
    }
};

// ============================================================================
// القطع الخطي / Line Segment
// ============================================================================

/// قطعة خطية / Line segment
struct LineSegment {
    Vec3 start; ///< نقطة البداية / Start point
    Vec3 end;   ///< نقطة النهاية / End point
    
    /// المنشئ الافتراضي / Default constructor
    LineSegment() : start(Vec3::Zero()), end(Vec3::UnitX()) {}
    
    /// منشئ بنقطتين / Constructor with two points
    LineSegment(const Vec3& s, const Vec3& e) : start(s), end(e) {}
    
    /// الطول / Length
    Float32 Length() const { return (end - start).Length(); }
    
    /// مربع الطول / Squared length
    Float32 LengthSq() const { return (end - start).LengthSq(); }
    
    /// الاتجاه / Direction
    Vec3 Direction() const { return (end - start).Normalized(); }
    
    /// المركز / Center
    Vec3 Center() const { return (start + end) * 0.5f; }
    
    /// نقطة على القطعة / Point on segment
    Vec3 GetPoint(Float32 t) const { return start.Lerp(end, Clamp(t, 0.0f, 1.0f)); }
    
    /// أقرب نقطة إلى نقطة معينة / Closest point to a point
    Vec3 ClosestPoint(const Vec3& point) const {
        Vec3 ab = end - start;
        Float32 t = (point - start).Dot(ab) / ab.LengthSq();
        return GetPoint(t);
    }
    
    /// المسافة إلى نقطة / Distance to point
    Float32 DistanceToPoint(const Vec3& point) const {
        return (point - ClosestPoint(point)).Length();
    }
};

// ============================================================================
// المثلث / Triangle
// ============================================================================

/// مثلث في الفضاء ثلاثي الأبعاد / Triangle in 3D space
struct Triangle {
    Vec3 v0, v1, v2; ///< الرؤوس الثلاثة / Three vertices
    
    /// المنشئ الافتراضي / Default constructor
    Triangle() : v0(Vec3::Zero()), v1(Vec3::UnitX()), v2(Vec3::UnitY()) {}
    
    /// منشئ بثلاث نقاط / Constructor with three points
    Triangle(const Vec3& a, const Vec3& b, const Vec3& c) : v0(a), v1(b), v2(c) {}
    
    /// المتجه العمودي / Normal vector
    Vec3 Normal() const {
        return (v1 - v0).Cross(v2 - v0).Normalized();
    }
    
    /// المساحة / Area
    Float32 Area() const {
        return (v1 - v0).Cross(v2 - v0).Length() * 0.5f;
    }
    
    /// المركز / Centroid
    Vec3 Centroid() const {
        return (v0 + v1 + v2) / 3.0f;
    }
    
    /// تقاطع مع شعاع (Möller–Trumbore) / Ray intersection (Möller–Trumbore algorithm)
    bool Raycast(const Ray& ray, Float32& t, Float32& u, Float32& v) const {
        Vec3 edge1 = v1 - v0;
        Vec3 edge2 = v2 - v0;
        Vec3 h = ray.direction.Cross(edge2);
        Float32 a = edge1.Dot(h);
        
        if (Abs(a) < EPSILON) return false;
        
        Float32 f = 1.0f / a;
        Vec3 s = ray.origin - v0;
        u = f * s.Dot(h);
        
        if (u < 0.0f || u > 1.0f) return false;
        
        Vec3 q = s.Cross(edge1);
        v = f * ray.direction.Dot(q);
        
        if (v < 0.0f || u + v > 1.0f) return false;
        
        t = f * edge2.Dot(q);
        return t >= 0.0f;
    }
    
    /// تقاطع مع شعاع (بدون إحداثيات barycentric) / Ray intersection (without barycentric coords)
    bool Raycast(const Ray& ray, Float32& t) const {
        Float32 u, v;
        return Raycast(ray, t, u, v);
    }
    
    /// أقرب نقطة على المثلث / Closest point on triangle
    Vec3 ClosestPoint(const Vec3& point) const {
        Vec3 ab = v1 - v0;
        Vec3 ac = v2 - v0;
        Vec3 ap = point - v0;
        
        Float32 d1 = ab.Dot(ap);
        Float32 d2 = ac.Dot(ap);
        if (d1 <= 0.0f && d2 <= 0.0f) return v0;
        
        Vec3 bp = point - v1;
        Float32 d3 = ab.Dot(bp);
        Float32 d4 = ac.Dot(bp);
        if (d3 >= 0.0f && d4 <= d3) return v1;
        
        Float32 vc = d1 * d4 - d3 * d2;
        if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
            Float32 v = d1 / (d1 - d3);
            return v0 + ab * v;
        }
        
        Vec3 cp = point - v2;
        Float32 d5 = ab.Dot(cp);
        Float32 d6 = ac.Dot(cp);
        if (d6 >= 0.0f && d5 <= d6) return v2;
        
        Float32 vb = d5 * d2 - d1 * d6;
        if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
            Float32 w = d2 / (d2 - d6);
            return v0 + ac * w;
        }
        
        Float32 va = d3 * d6 - d5 * d4;
        if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
            Float32 w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            return v1 + (v2 - v1) * w;
        }
        
        Float32 denom = 1.0f / (va + vb + vc);
        Float32 v_ = vb * denom;
        Float32 w_ = vc * denom;
        return v0 + ab * v_ + ac * w_;
    }
    
    /// الإحداثيات Barycentric / Barycentric coordinates
    Vec3 Barycentric(const Vec3& point) const {
        Vec3 e0 = v1 - v0;
        Vec3 e1 = v2 - v0;
        Vec3 e2 = point - v0;
        
        Float32 d00 = e0.Dot(e0);
        Float32 d01 = e0.Dot(e1);
        Float32 d11 = e1.Dot(e1);
        Float32 d20 = e2.Dot(e0);
        Float32 d21 = e2.Dot(e1);
        
        Float32 denom = d00 * d11 - d01 * d01;
        Float32 v = (d11 * d20 - d01 * d21) / denom;
        Float32 w = (d00 * d21 - d01 * d20) / denom;
        Float32 u = 1.0f - v - w;
        
        return Vec3(u, v, w);
    }
};

// ============================================================================
// الكبسولة / Capsule
// ============================================================================

/// كبسولة (اسطوانة بنهايات كروية) / Capsule (cylinder with spherical ends)
struct Capsule {
    Vec3 start;     ///< نقطة البداية / Start point
    Vec3 end;       ///< نقطة النهاية / End point
    Float32 radius; ///< نصف القطر / Radius
    
    /// المنشئ الافتراضي / Default constructor
    Capsule() : start(Vec3::Zero()), end(Vec3::UnitY()), radius(0.5f) {}
    
    /// منشئ كامل / Full constructor
    Capsule(const Vec3& s, const Vec3& e, Float32 r) : start(s), end(e), radius(r) {}
    
    /// الطول الكلي / Total length (including caps)
    Float32 Length() const { return (end - start).Length() + 2.0f * radius; }
    
    /// المركز / Center
    Vec3 Center() const { return (start + end) * 0.5f; }
    
    /// هل تحتوي نقطة / Contains point
    bool Contains(const Vec3& point) const {
        LineSegment segment(start, end);
        return segment.DistanceToPoint(point) <= radius;
    }
    
    /// تحويل إلى AABB / Convert to AABB
    AABB ToAABB() const {
        Vec3 r(radius);
        return AABB(
            Vec3(Min(start.x, end.x) - radius, Min(start.y, end.y) - radius, Min(start.z, end.z) - radius),
            Vec3(Max(start.x, end.x) + radius, Max(start.y, end.y) + radius, Max(start.z, end.z) + radius)
        );
    }
};

// ============================================================================
// ضوضاء بيرلين / Perlin Noise
// ============================================================================

/// مولد ضوضاء بيرلين / Perlin noise generator
class PerlinNoise {
public:
    /// المنشئ مع بذرة / Constructor with seed
    explicit PerlinNoise(UInt32 seed = 0) {
        // تهيئة جدول التبديل / Initialize permutation table
        for (int i = 0; i < 256; i++) {
            p[i] = static_cast<u8>(i);
        }
        
        // خلط الجدول / Shuffle the table
        if (seed != 0) {
            for (int i = 255; i > 0; i--) {
                seed = seed * 1103515245 + 12345;
                int j = (seed >> 16) % (i + 1);
                u8 temp = p[i];
                p[i] = p[j];
                p[j] = temp;
            }
        }
        
        // تكرار الجدول / Duplicate the table
        for (int i = 0; i < 256; i++) {
            p[256 + i] = p[i];
        }
    }
    
    /// ضوضاء أحادية البعد / 1D noise
    Float32 Noise1D(Float32 x) const {
        int X = static_cast<int>(std::floor(x)) & 255;
        x -= std::floor(x);
        Float32 u = Fade(x);
        return Lerp(Grad1D(p[X], x), Grad1D(p[X + 1], x - 1), u);
    }
    
    /// ضوضاء ثنائية البعد / 2D noise
    Float32 Noise2D(Float32 x, Float32 y) const {
        int X = static_cast<int>(std::floor(x)) & 255;
        int Y = static_cast<int>(std::floor(y)) & 255;
        x -= std::floor(x);
        y -= std::floor(y);
        Float32 u = Fade(x);
        Float32 v = Fade(y);
        
        int A = p[X] + Y;
        int B = p[X + 1] + Y;
        
        return Lerp(
            Lerp(Grad2D(p[A], x, y), Grad2D(p[B], x - 1, y), u),
            Lerp(Grad2D(p[A + 1], x, y - 1), Grad2D(p[B + 1], x - 1, y - 1), u),
            v
        );
    }
    
    /// ضوضاء ثلاثية البعد / 3D noise
    Float32 Noise3D(Float32 x, Float32 y, Float32 z) const {
        int X = static_cast<int>(std::floor(x)) & 255;
        int Y = static_cast<int>(std::floor(y)) & 255;
        int Z = static_cast<int>(std::floor(z)) & 255;
        x -= std::floor(x);
        y -= std::floor(y);
        z -= std::floor(z);
        Float32 u = Fade(x);
        Float32 v = Fade(y);
        Float32 w = Fade(z);
        
        int A = p[X] + Y;
        int AA = p[A] + Z;
        int AB = p[A + 1] + Z;
        int B = p[X + 1] + Y;
        int BA = p[B] + Z;
        int BB = p[B + 1] + Z;
        
        return Lerp(
            Lerp(
                Lerp(Grad3D(p[AA], x, y, z), Grad3D(p[BA], x - 1, y, z), u),
                Lerp(Grad3D(p[AB], x, y - 1, z), Grad3D(p[BB], x - 1, y - 1, z), u),
                v
            ),
            Lerp(
                Lerp(Grad3D(p[AA + 1], x, y, z - 1), Grad3D(p[BA + 1], x - 1, y, z - 1), u),
                Lerp(Grad3D(p[AB + 1], x, y - 1, z - 1), Grad3D(p[BB + 1], x - 1, y - 1, z - 1), u),
                v
            ),
            w
        );
    }
    
    /// ضوضاء متعددة الأوكتافات / Fractal Brownian Motion (fBm)
    Float32 FBM2D(Float32 x, Float32 y, int octaves, Float32 lacunarity = 2.0f, Float32 persistence = 0.5f) const {
        Float32 total = 0.0f;
        Float32 amplitude = 1.0f;
        Float32 frequency = 1.0f;
        Float32 maxValue = 0.0f;
        
        for (int i = 0; i < octaves; i++) {
            total += Noise2D(x * frequency, y * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }
        
        return total / maxValue;
    }
    
    /// fBm ثلاثي الأبعاد / 3D fBm
    Float32 FBM3D(Float32 x, Float32 y, Float32 z, int octaves, Float32 lacunarity = 2.0f, Float32 persistence = 0.5f) const {
        Float32 total = 0.0f;
        Float32 amplitude = 1.0f;
        Float32 frequency = 1.0f;
        Float32 maxValue = 0.0f;
        
        for (int i = 0; i < octaves; i++) {
            total += Noise3D(x * frequency, y * frequency, z * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }
        
        return total / maxValue;
    }
    
private:
    u8 p[512]; ///< جدول التبديل / Permutation table
    
    /// دالة التلاشي / Fade function
    static Float32 Fade(Float32 t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }
    
    /// التدرج أحادي البعد / 1D gradient
    static Float32 Grad1D(int hash, Float32 x) {
        return (hash & 1) ? x : -x;
    }
    
    /// التدرج ثنائي البعد / 2D gradient
    static Float32 Grad2D(int hash, Float32 x, Float32 y) {
        int h = hash & 3;
        Float32 u = (h < 2) ? x : y;
        Float32 v = (h < 2) ? y : x;
        return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
    }
    
    /// التدرج ثلاثي البعد / 3D gradient
    static Float32 Grad3D(int hash, Float32 x, Float32 y, Float32 z) {
        int h = hash & 15;
        Float32 u = (h < 8) ? x : y;
        Float32 v = (h < 4) ? y : ((h == 12 || h == 14) ? x : z);
        return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
    }
};

// ============================================================================
// منحنيات بيزيه / Bézier Curves
// ============================================================================

/// منحنى بيزيه التربيعي / Quadratic Bézier curve
struct QuadraticBezier {
    Vec3 p0, p1, p2; ///< نقاط التحكم / Control points
    
    /// المنشئ الافتراضي / Default constructor
    QuadraticBezier() : p0(Vec3::Zero()), p1(Vec3::UnitX() * 0.5f), p2(Vec3::UnitX()) {}
    
    /// منشئ بنقاط / Constructor with points
    QuadraticBezier(const Vec3& a, const Vec3& b, const Vec3& c) : p0(a), p1(b), p2(c) {}
    
    /// الحصول على نقطة / Get point at t
    Vec3 GetPoint(Float32 t) const {
        Float32 u = 1.0f - t;
        return p0 * (u * u) + p1 * (2.0f * u * t) + p2 * (t * t);
    }
    
    /// المشتقة (الظل) / Derivative (tangent)
    Vec3 GetTangent(Float32 t) const {
        Float32 u = 1.0f - t;
        return (p1 - p0) * (2.0f * u) + (p2 - p1) * (2.0f * t);
    }
    
    /// تقسيم المنحنى / Split curve at t
    void Split(Float32 t, QuadraticBezier& left, QuadraticBezier& right) const {
        Vec3 q0 = p0.Lerp(p1, t);
        Vec3 q1 = p1.Lerp(p2, t);
        Vec3 r = q0.Lerp(q1, t);
        
        left = QuadraticBezier(p0, q0, r);
        right = QuadraticBezier(r, q1, p2);
    }
};

/// منحنى بيزيه التكعيبي / Cubic Bézier curve
struct CubicBezier {
    Vec3 p0, p1, p2, p3; ///< نقاط التحكم / Control points
    
    /// المنشئ الافتراضي / Default constructor
    CubicBezier() 
        : p0(Vec3::Zero()), p1(Vec3::UnitX() * 0.33f), 
          p2(Vec3::UnitX() * 0.66f), p3(Vec3::UnitX()) {}
    
    /// منشئ بنقاط / Constructor with points
    CubicBezier(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d) 
        : p0(a), p1(b), p2(c), p3(d) {}
    
    /// الحصول على نقطة / Get point at t
    Vec3 GetPoint(Float32 t) const {
        Float32 u = 1.0f - t;
        Float32 tt = t * t;
        Float32 uu = u * u;
        Float32 uuu = uu * u;
        Float32 ttt = tt * t;
        
        return p0 * uuu + p1 * (3.0f * uu * t) + p2 * (3.0f * u * tt) + p3 * ttt;
    }
    
    /// المشتقة (الظل) / Derivative (tangent)
    Vec3 GetTangent(Float32 t) const {
        Float32 u = 1.0f - t;
        Float32 tt = t * t;
        Float32 uu = u * u;
        
        return (p1 - p0) * (3.0f * uu) + (p2 - p1) * (6.0f * u * t) + (p3 - p2) * (3.0f * tt);
    }
    
    /// المشتقة الثانية / Second derivative
    Vec3 GetAcceleration(Float32 t) const {
        Float32 u = 1.0f - t;
        return (p2 - p1 * 2.0f + p0) * (6.0f * u) + (p3 - p2 * 2.0f + p1) * (6.0f * t);
    }
    
    /// تقسيم المنحنى / Split curve at t
    void Split(Float32 t, CubicBezier& left, CubicBezier& right) const {
        Vec3 q0 = p0.Lerp(p1, t);
        Vec3 q1 = p1.Lerp(p2, t);
        Vec3 q2 = p2.Lerp(p3, t);
        
        Vec3 r0 = q0.Lerp(q1, t);
        Vec3 r1 = q1.Lerp(q2, t);
        
        Vec3 s = r0.Lerp(r1, t);
        
        left = CubicBezier(p0, q0, r0, s);
        right = CubicBezier(s, r1, q2, p3);
    }
    
    /// الطول التقريبي / Approximate length
    Float32 ApproxLength(int segments = 10) const {
        Float32 length = 0.0f;
        Vec3 prev = p0;
        
        for (int i = 1; i <= segments; i++) {
            Float32 t = static_cast<Float32>(i) / segments;
            Vec3 curr = GetPoint(t);
            length += (curr - prev).Length();
            prev = curr;
        }
        
        return length;
    }
};

/// مسار بيزيه (سلسلة من منحنيات بيزيه) / Bézier path (chain of Bézier curves)
class BezierPath {
public:
    /// إضافة نقطة / Add point
    void AddPoint(const Vec3& point) {
        m_points.push_back(point);
    }
    
    /// إضافة نقطة مع مقابض / Add point with handles
    void AddPoint(const Vec3& point, const Vec3& handleIn, const Vec3& handleOut) {
        m_points.push_back(point);
        m_handlesIn.push_back(handleIn);
        m_handlesOut.push_back(handleOut);
    }
    
    /// الحصول على نقطة على المسار / Get point on path
    Vec3 GetPoint(Float32 t) const {
        if (m_points.size() < 2) return m_points.empty() ? Vec3::Zero() : m_points[0];
        
        t = Clamp(t, 0.0f, 1.0f);
        Float32 segmentT = t * (m_points.size() - 1);
        int segment = static_cast<int>(segmentT);
        if (segment >= static_cast<int>(m_points.size()) - 1) segment = static_cast<int>(m_points.size()) - 2;
        Float32 localT = segmentT - segment;
        
        // منحنى بيزيه تكعيبي بين النقاط / Cubic Bézier between points
        const Vec3& p0 = m_points[segment];
        const Vec3& p3 = m_points[segment + 1];
        Vec3 p1 = p0 + (segment < static_cast<int>(m_handlesOut.size()) ? m_handlesOut[segment] : (p3 - p0) * 0.33f);
        Vec3 p2 = p3 + (segment + 1 < static_cast<int>(m_handlesIn.size()) ? m_handlesIn[segment + 1] : (p0 - p3) * 0.33f);
        
        return CubicBezier(p0, p1, p2, p3).GetPoint(localT);
    }
    
    /// عدد النقاط / Point count
    size_t PointCount() const { return m_points.size(); }
    
    /// مسح المسار / Clear path
    void Clear() {
        m_points.clear();
        m_handlesIn.clear();
        m_handlesOut.clear();
    }
    
private:
    std::vector<Vec3> m_points;      ///< نقاط المسار / Path points
    std::vector<Vec3> m_handlesIn;   ///< مقابض الدخول / In handles
    std::vector<Vec3> m_handlesOut;  ///< مقابض الخروج / Out handles
};

// ============================================================================
// دوال التحويل للرباعية / Quaternion Conversion Functions
// ============================================================================

/// تحويل الرباعية إلى مصفوفة / Convert quaternion to matrix
inline Mat4 Quat::ToMatrix() const {
    Mat4 m = Mat4::Identity();
    
    Float32 xx = x * x, yy = y * y, zz = z * z;
    Float32 xy = x * y, xz = x * z, yz = y * z;
    Float32 wx = w * x, wy = w * y, wz = w * z;
    
    m.m[0] = 1.0f - 2.0f * (yy + zz);
    m.m[1] = 2.0f * (xy + wz);
    m.m[2] = 2.0f * (xz - wy);
    
    m.m[4] = 2.0f * (xy - wz);
    m.m[5] = 1.0f - 2.0f * (xx + zz);
    m.m[6] = 2.0f * (yz + wx);
    
    m.m[8] = 2.0f * (xz + wy);
    m.m[9] = 2.0f * (yz - wx);
    m.m[10] = 1.0f - 2.0f * (xx + yy);
    
    m.m[15] = 1.0f;
    
    return m;
}

/// إنشاء رباعية من مصفوفة دوران / Create quaternion from rotation matrix
inline Quat Quat::FromMatrix(const Mat4& m) {
    Float32 trace = m.m[0] + m.m[5] + m.m[10];
    Quat q;
    
    if (trace > 0.0f) {
        Float32 s = 0.5f / std::sqrt(trace + 1.0f);
        q.w = 0.25f / s;
        q.x = (m.m[6] - m.m[9]) * s;
        q.y = (m.m[8] - m.m[2]) * s;
        q.z = (m.m[1] - m.m[4]) * s;
    } else if (m.m[0] > m.m[5] && m.m[0] > m.m[10]) {
        Float32 s = 2.0f * std::sqrt(1.0f + m.m[0] - m.m[5] - m.m[10]);
        q.w = (m.m[6] - m.m[9]) / s;
        q.x = 0.25f * s;
        q.y = (m.m[4] + m.m[1]) / s;
        q.z = (m.m[8] + m.m[2]) / s;
    } else if (m.m[5] > m.m[10]) {
        Float32 s = 2.0f * std::sqrt(1.0f + m.m[5] - m.m[0] - m.m[10]);
        q.w = (m.m[8] - m.m[2]) / s;
        q.x = (m.m[4] + m.m[1]) / s;
        q.y = 0.25f * s;
        q.z = (m.m[9] + m.m[6]) / s;
    } else {
        Float32 s = 2.0f * std::sqrt(1.0f + m.m[10] - m.m[0] - m.m[5]);
        q.w = (m.m[1] - m.m[4]) / s;
        q.x = (m.m[8] + m.m[2]) / s;
        q.y = (m.m[9] + m.m[6]) / s;
        q.z = 0.25f * s;
    }
    
    return q.Normalized();
}

/// إنشاء رباعية للنظر نحو اتجاه / Create quaternion to look at direction
inline Quat Quat::LookRotation(const Vec3& forward, const Vec3& up) {
    Vec3 fwd = forward.Normalized();
    Vec3 right = up.Cross(fwd).Normalized();
    Vec3 upVec = fwd.Cross(right);
    
    Mat4 m = Mat4::Identity();
    m.m[0] = right.x; m.m[4] = upVec.x; m.m[8] = fwd.x;
    m.m[1] = right.y; m.m[5] = upVec.y; m.m[9] = fwd.y;
    m.m[2] = right.z; m.m[6] = upVec.z; m.m[10] = fwd.z;
    
    return FromMatrix(m);
}

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_MATH3D_H
