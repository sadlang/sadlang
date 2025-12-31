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
};

/// متجه ثلاثي الأبعاد / 3D Vector
struct Vec3 {
    Float32 x, y, z;
    
    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(Float32 x_, Float32 y_, Float32 z_) : x(x_), y(y_), z(z_) {}
};

/// متجه رباعي الأبعاد / 4D Vector
struct Vec4 {
    Float32 x, y, z, w;
    
    Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    Vec4(Float32 x_, Float32 y_, Float32 z_, Float32 w_) : x(x_), y(y_), z(z_), w(w_) {}
};

// ============================================================================
// المصفوفات / Matrices
// ============================================================================

/// مصفوفة 3x3 / 3x3 Matrix
struct Mat3 {
    Float32 m[9];
    
    Mat3() {
        for (int i = 0; i < 9; i++) m[i] = 0.0f;
    }
};

/// مصفوفة 4x4 / 4x4 Matrix
struct Mat4 {
    Float32 m[16];
    
    Mat4() {
        for (int i = 0; i < 16; i++) m[i] = 0.0f;
    }
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
