// ============================================================================
// types.h - الأنواع الأساسية لمكتبة الرسومات
// Core Types for Graphics Library
// ============================================================================
// الوصف: يحتوي على تعريفات الأنواع والثوابت الأساسية المشتركة
// Description: Contains fundamental type definitions and shared constants
// ============================================================================

#ifndef SAD_GRAPHICS_TYPES_H
#define SAD_GRAPHICS_TYPES_H

#include <cstdint>      // لأنواع الأعداد الصحيحة ذات الحجم المحدد / Fixed-size integer types
#include <cmath>        // للدوال الرياضية / Math functions
#include <string>       // لنوع النصوص / String type
#include <memory>       // للمؤشرات الذكية / Smart pointers
#include <functional>   // لأنواع الدوال / Function types

// ============================================================================
// مساحة الاسم الرئيسية / Main namespace
// ============================================================================
namespace sad {
namespace graphics {

// ============================================================================
// الأنواع الأساسية / Fundamental Types
// ============================================================================

/// نوع الأعداد الصحيحة 32 بت / 32-bit integer type
using Int32 = std::int32_t;

/// نوع الأعداد الصحيحة غير الموقعة 32 بت / 32-bit unsigned integer type
using UInt32 = std::uint32_t;

/// نوع الأعداد الصحيحة 64 بت / 64-bit integer type
using Int64 = std::int64_t;

/// نوع الأعداد الصحيحة غير الموقعة 64 بت / 64-bit unsigned integer type
using UInt64 = std::uint64_t;

/// نوع الأعداد العشرية دقة مفردة / Single precision floating point
using Float32 = float;

/// نوع الأعداد العشرية دقة مزدوجة / Double precision floating point
using Float64 = double;

// ============================================================================
// الأنواع المختصرة / Short Type Aliases
// ============================================================================

/// 8-bit unsigned integer
using u8 = std::uint8_t;

/// 16-bit unsigned integer
using u16 = std::uint16_t;

/// 32-bit unsigned integer
using u32 = std::uint32_t;

/// 64-bit unsigned integer
using u64 = std::uint64_t;

/// 8-bit signed integer
using i8 = std::int8_t;

/// 16-bit signed integer
using i16 = std::int16_t;

/// 32-bit signed integer
using i32 = std::int32_t;

/// 64-bit signed integer
using i64 = std::int64_t;

/// نوع النصوص / String type
using String = std::string;

/// معرف فريد للكائنات / Unique object identifier
using Handle = UInt64;

// ============================================================================
// الثوابت العامة / Global Constants
// ============================================================================

/// معرف غير صالح / Invalid handle identifier
constexpr Handle INVALID_HANDLE = 0;

/// الحد الأقصى لطول النص / Maximum string length
constexpr size_t MAX_STRING_LENGTH = 1024;

// ============================================================================
// التعدادات / Enumerations
// ============================================================================

/// نتيجة العملية / Operation result
enum class Result {
    Success = 0,      ///< العملية نجحت / Operation succeeded
    Failure = 1,      ///< العملية فشلت / Operation failed
    InvalidHandle,    ///< معرف غير صالح / Invalid handle
    InvalidParameter, ///< معامل غير صالح / Invalid parameter
    OutOfMemory,      ///< نفاد الذاكرة / Out of memory
    NotImplemented,   ///< غير مطبق / Not implemented
    NotSupported      ///< غير مدعوم / Not supported
};

/// مستوى السجل / Log level
enum class LogLevel {
    Debug,    ///< رسائل التنقيح / Debug messages
    Info,     ///< رسائل معلوماتية / Information messages
    Warning,  ///< تحذيرات / Warnings
    Error,    ///< أخطاء / Errors
    Critical  ///< أخطاء حرجة / Critical errors
};

// ============================================================================
// الهياكل الأساسية / Basic Structures
// ============================================================================

/// نقطة ثنائية الأبعاد / 2D Point
struct Point2D {
    Float32 x; ///< الإحداثي الأفقي / Horizontal coordinate
    Float32 y; ///< الإحداثي العمودي / Vertical coordinate
    
    /// المنشئ الافتراضي / Default constructor
    Point2D() : x(0.0f), y(0.0f) {}
    
    /// منشئ بقيم / Constructor with values
    Point2D(Float32 x_, Float32 y_) : x(x_), y(y_) {}
};

/// حجم ثنائي الأبعاد / 2D Size
struct Size2D {
    Float32 width;  ///< العرض / Width
    Float32 height; ///< الارتفاع / Height
    
    /// المنشئ الافتراضي / Default constructor
    Size2D() : width(0.0f), height(0.0f) {}
    
    /// منشئ بقيم / Constructor with values
    Size2D(Float32 w, Float32 h) : width(w), height(h) {}
};

/// مستطيل / Rectangle
struct Rect {
    Float32 x;      ///< موقع X / X position
    Float32 y;      ///< موقع Y / Y position
    Float32 width;  ///< العرض / Width
    Float32 height; ///< الارتفاع / Height
    
    /// المنشئ الافتراضي / Default constructor
    Rect() : x(0.0f), y(0.0f), width(0.0f), height(0.0f) {}
    
    /// منشئ بقيم / Constructor with values
    Rect(Float32 x_, Float32 y_, Float32 w, Float32 h)
        : x(x_), y(y_), width(w), height(h) {}
};

// ============================================================================
// المتجهات / Vectors
// ============================================================================

/// متجه ثنائي الأبعاد / 2D Vector
struct Vec2 {
    Float32 x, y;
    
    Vec2() : x(0.0f), y(0.0f) {}
    Vec2(Float32 x_, Float32 y_) : x(x_), y(y_) {}
    Vec2(Float32 s) : x(s), y(s) {}              // منشئ قيمة واحدة / Scalar constructor
    
    // العمليات الحسابية / Arithmetic operators
    Vec2 operator+(const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
    Vec2 operator-(const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
    Vec2 operator*(const Vec2& v) const { return Vec2(x * v.x, y * v.y); }
    Vec2 operator/(const Vec2& v) const { return Vec2(x / v.x, y / v.y); }
    Vec2 operator*(Float32 s) const { return Vec2(x * s, y * s); }
    Vec2 operator/(Float32 s) const { Float32 inv = 1.0f / s; return Vec2(x * inv, y * inv); }
    Vec2 operator-() const { return Vec2(-x, -y); }
    
    // عمليات الإسناد المركبة / Compound assignment operators
    Vec2& operator+=(const Vec2& v) { x += v.x; y += v.y; return *this; }
    Vec2& operator-=(const Vec2& v) { x -= v.x; y -= v.y; return *this; }
    Vec2& operator*=(Float32 s) { x *= s; y *= s; return *this; }
    Vec2& operator/=(Float32 s) { Float32 inv = 1.0f / s; x *= inv; y *= inv; return *this; }
    
    // المقارنة / Comparison
    bool operator==(const Vec2& v) const { return x == v.x && y == v.y; }
    bool operator!=(const Vec2& v) const { return !(*this == v); }
    
    // الوصول بالفهرس / Index access
    Float32& operator[](int i) { return (&x)[i]; }
    Float32 operator[](int i) const { return (&x)[i]; }
    
    // الدوال الرياضية / Math functions
    Float32 Dot(const Vec2& v) const { return x * v.x + y * v.y; }
    Float32 Cross(const Vec2& v) const { return x * v.y - y * v.x; }
    Float32 LengthSq() const { return x * x + y * y; }
    Float32 Length() const;
    Vec2 Normalized() const;
    void Normalize();
    Float32 Distance(const Vec2& v) const;
    Vec2 Lerp(const Vec2& v, Float32 t) const { return *this + (v - *this) * t; }
    Vec2 Perpendicular() const { return Vec2(-y, x); }
    Float32 Angle() const;
    
    // ثوابت / Constants
    static Vec2 Zero() { return Vec2(0.0f, 0.0f); }
    static Vec2 One() { return Vec2(1.0f, 1.0f); }
    static Vec2 UnitX() { return Vec2(1.0f, 0.0f); }
    static Vec2 UnitY() { return Vec2(0.0f, 1.0f); }
};
inline Vec2 operator*(Float32 s, const Vec2& v) { return v * s; }

/// متجه ثلاثي الأبعاد / 3D Vector
struct Vec3 {
    Float32 x, y, z;
    
    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(Float32 x_, Float32 y_, Float32 z_) : x(x_), y(y_), z(z_) {}
    Vec3(Float32 s) : x(s), y(s), z(s) {}
    Vec3(const Vec2& v, Float32 z_) : x(v.x), y(v.y), z(z_) {}
    
    // العمليات الحسابية / Arithmetic operators
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
    Vec3 operator/(const Vec3& v) const { return Vec3(x / v.x, y / v.y, z / v.z); }
    Vec3 operator*(Float32 s) const { return Vec3(x * s, y * s, z * s); }
    Vec3 operator/(Float32 s) const { Float32 inv = 1.0f / s; return Vec3(x * inv, y * inv, z * inv); }
    Vec3 operator-() const { return Vec3(-x, -y, -z); }
    
    Vec3& operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vec3& operator-=(const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vec3& operator*=(Float32 s) { x *= s; y *= s; z *= s; return *this; }
    Vec3& operator/=(Float32 s) { Float32 inv = 1.0f / s; x *= inv; y *= inv; z *= inv; return *this; }
    
    bool operator==(const Vec3& v) const { return x == v.x && y == v.y && z == v.z; }
    bool operator!=(const Vec3& v) const { return !(*this == v); }
    
    Float32& operator[](int i) { return (&x)[i]; }
    Float32 operator[](int i) const { return (&x)[i]; }
    
    // الدوال الرياضية / Math functions
    Float32 Dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3 Cross(const Vec3& v) const {
        return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
    Float32 LengthSq() const { return x * x + y * y + z * z; }
    Float32 Length() const;
    Vec3 Normalized() const;
    void Normalize();
    Float32 Distance(const Vec3& v) const;
    Vec3 Lerp(const Vec3& v, Float32 t) const { return *this + (v - *this) * t; }
    Vec3 Reflect(const Vec3& normal) const { return *this - normal * (2.0f * Dot(normal)); }
    
    Vec2 XY() const { return Vec2(x, y); }
    
    static Vec3 Zero() { return Vec3(0.0f, 0.0f, 0.0f); }
    static Vec3 One() { return Vec3(1.0f, 1.0f, 1.0f); }
    static Vec3 UnitX() { return Vec3(1.0f, 0.0f, 0.0f); }
    static Vec3 UnitY() { return Vec3(0.0f, 1.0f, 0.0f); }
    static Vec3 UnitZ() { return Vec3(0.0f, 0.0f, 1.0f); }
    static Vec3 Up() { return Vec3(0.0f, 1.0f, 0.0f); }
    static Vec3 Forward() { return Vec3(0.0f, 0.0f, -1.0f); }
    static Vec3 Right() { return Vec3(1.0f, 0.0f, 0.0f); }
};
inline Vec3 operator*(Float32 s, const Vec3& v) { return v * s; }

/// متجه رباعي الأبعاد / 4D Vector
struct Vec4 {
    Float32 x, y, z, w;
    
    Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    Vec4(Float32 x_, Float32 y_, Float32 z_, Float32 w_) : x(x_), y(y_), z(z_), w(w_) {}
    Vec4(Float32 s) : x(s), y(s), z(s), w(s) {}
    Vec4(const Vec3& v, Float32 w_) : x(v.x), y(v.y), z(v.z), w(w_) {}
    Vec4(const Vec2& v, Float32 z_, Float32 w_) : x(v.x), y(v.y), z(z_), w(w_) {}
    
    Vec4 operator+(const Vec4& v) const { return Vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
    Vec4 operator-(const Vec4& v) const { return Vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
    Vec4 operator*(const Vec4& v) const { return Vec4(x * v.x, y * v.y, z * v.z, w * v.w); }
    Vec4 operator*(Float32 s) const { return Vec4(x * s, y * s, z * s, w * s); }
    Vec4 operator/(Float32 s) const { Float32 inv = 1.0f / s; return Vec4(x * inv, y * inv, z * inv, w * inv); }
    Vec4 operator-() const { return Vec4(-x, -y, -z, -w); }
    
    Vec4& operator+=(const Vec4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
    Vec4& operator-=(const Vec4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
    Vec4& operator*=(Float32 s) { x *= s; y *= s; z *= s; w *= s; return *this; }
    
    bool operator==(const Vec4& v) const { return x == v.x && y == v.y && z == v.z && w == v.w; }
    bool operator!=(const Vec4& v) const { return !(*this == v); }
    
    Float32& operator[](int i) { return (&x)[i]; }
    Float32 operator[](int i) const { return (&x)[i]; }
    
    Float32 Dot(const Vec4& v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }
    Float32 LengthSq() const { return x * x + y * y + z * z + w * w; }
    Float32 Length() const;
    Vec4 Normalized() const;
    
    Vec2 XY() const { return Vec2(x, y); }
    Vec3 XYZ() const { return Vec3(x, y, z); }
    
    static Vec4 Zero() { return Vec4(0.0f, 0.0f, 0.0f, 0.0f); }
    static Vec4 One() { return Vec4(1.0f, 1.0f, 1.0f, 1.0f); }
};
inline Vec4 operator*(Float32 s, const Vec4& v) { return v * s; }

// ============================================================================
// المصفوفات / Matrices
// ============================================================================

/// مصفوفة 3x3 / 3x3 Matrix
struct Mat3 {
    Float32 m[9];
    
    Mat3() {
        for (int i = 0; i < 9; i++) m[i] = 0.0f;
    }
    
    static Mat3 Identity() {
        Mat3 result;
        result.m[0] = result.m[4] = result.m[8] = 1.0f;
        return result;
    }
};

/// مصفوفة 4x4 / 4x4 Matrix (column-major like OpenGL)
struct Mat4 {
    Float32 m[16];
    
    Mat4() {
        for (int i = 0; i < 16; i++) m[i] = 0.0f;
    }
    
    // الوصول بالعمود والصف / Access by column and row
    Float32& At(int row, int col) { return m[col * 4 + row]; }
    Float32 At(int row, int col) const { return m[col * 4 + row]; }
    
    // ضرب المصفوفات / Matrix multiplication
    Mat4 operator*(const Mat4& other) const;
    Vec4 operator*(const Vec4& v) const;
    Vec3 TransformPoint(const Vec3& p) const;
    Vec3 TransformDirection(const Vec3& d) const;
    
    // إنشاء مصفوفات خاصة / Create special matrices
    static Mat4 Identity();
    static Mat4 Translate(const Vec3& t);
    static Mat4 Translate(Float32 x, Float32 y, Float32 z);
    static Mat4 Scale(const Vec3& s);
    static Mat4 Scale(Float32 x, Float32 y, Float32 z);
    static Mat4 Scale(Float32 s);
    static Mat4 RotateX(Float32 radians);
    static Mat4 RotateY(Float32 radians);
    static Mat4 RotateZ(Float32 radians);
    static Mat4 RotateAxis(const Vec3& axis, Float32 radians);
    
    // مصفوفات الإسقاط / Projection matrices
    static Mat4 Ortho(Float32 left, Float32 right, Float32 bottom, Float32 top,
                      Float32 near_ = -1.0f, Float32 far_ = 1.0f);
    static Mat4 Perspective(Float32 fovRadians, Float32 aspect, Float32 near_, Float32 far_);
    static Mat4 LookAt(const Vec3& eye, const Vec3& target, const Vec3& up);
    
    // عمليات المصفوفة / Matrix operations
    Mat4 Transposed() const;
    Mat4 Inverted() const;
    Float32 Determinant() const;
    
    // استخراج / Extract
    Vec3 GetTranslation() const { return Vec3(m[12], m[13], m[14]); }
    Vec3 GetScale() const;
    
    // الحصول على مؤشر البيانات (لـ OpenGL) / Get data pointer (for OpenGL)
    const Float32* Data() const { return m; }
    Float32* Data() { return m; }
};

/// لون RGBA / RGBA Color
struct Color {
    Float32 r; ///< الأحمر (0.0 - 1.0) / Red (0.0 - 1.0)
    Float32 g; ///< الأخضر (0.0 - 1.0) / Green (0.0 - 1.0)
    Float32 b; ///< الأزرق (0.0 - 1.0) / Blue (0.0 - 1.0)
    Float32 a; ///< الشفافية (0.0 - 1.0) / Alpha (0.0 - 1.0)
    
    /// المنشئ الافتراضي - أسود شفاف / Default - transparent black
    Color() : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}
    
    /// منشئ بقيم RGB (شفافية = 1.0) / Constructor with RGB (alpha = 1.0)
    Color(Float32 r_, Float32 g_, Float32 b_)
        : r(r_), g(g_), b(b_), a(1.0f) {}
    
    /// منشئ بقيم RGBA / Constructor with RGBA
    Color(Float32 r_, Float32 g_, Float32 b_, Float32 a_)
        : r(r_), g(g_), b(b_), a(a_) {}
    
    /// منشئ من قيم بايت (0-255) / Constructor from byte values (0-255)
    static Color FromBytes(int r_, int g_, int b_, int a_ = 255) {
        return Color(r_ / 255.0f, g_ / 255.0f, b_ / 255.0f, a_ / 255.0f);
    }
    
    /// منشئ من نص HEX / Constructor from HEX string
    static Color FromHex(const std::string& hex) {
        std::string h = hex;
        if (!h.empty() && h[0] == '#') h = h.substr(1);
        unsigned int val = 0;
        for (char c : h) {
            val <<= 4;
            if (c >= '0' && c <= '9') val |= (c - '0');
            else if (c >= 'a' && c <= 'f') val |= (c - 'a' + 10);
            else if (c >= 'A' && c <= 'F') val |= (c - 'A' + 10);
        }
        if (h.size() == 8) {
            return FromBytes((val >> 24) & 0xFF, (val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF);
        }
        return FromBytes((val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF);
    }
    
    // الألوان المعرفة مسبقاً / Predefined colors
    static const Color Black;       ///< أسود / Black
    static const Color White;       ///< أبيض / White
    static const Color Red;         ///< أحمر / Red
    static const Color Green;       ///< أخضر / Green
    static const Color Blue;        ///< أزرق / Blue
    static const Color Yellow;      ///< أصفر / Yellow
    static const Color Cyan;        ///< سماوي / Cyan
    static const Color Magenta;     ///< أرجواني / Magenta
    static const Color Transparent; ///< شفاف / Transparent
    static const Color Gray;        ///< رمادي / Gray
    static const Color LightGray;   ///< رمادي فاتح / Light Gray
    static const Color DarkGray;    ///< رمادي غامق / Dark Gray
};

// ============================================================================
// أنواع الدوال / Function Types
// ============================================================================

/// نوع دالة السجل / Logger function type
using LogFunction = std::function<void(LogLevel, const String&)>;

/// نوع دالة رد نداء للسجل / Log callback function type
using LogCallback = std::function<void(LogLevel, const String&)>;

/// نوع دالة رد النداء / Callback function type
using CallbackFunction = std::function<void()>;

/// نوع دالة رد نداء بمعامل / Callback with parameter
template<typename T>
using CallbackWithParam = std::function<void(T)>;

} // namespace graphics
} // namespace sad

// Namespace alias للتوافق / Namespace alias for compatibility
namespace SadGraphics {
    using namespace sad::graphics;
}

#endif // SAD_GRAPHICS_TYPES_H
