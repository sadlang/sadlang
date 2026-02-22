// ==============================================================================
// test_types.cpp - اختبارات الأنواع الأساسية / Core Types Tests
// ==============================================================================
#include "catch.hpp"
#include "core/types.h"
#include <cmath>

using namespace sad::graphics;

// ============================================================================
// اختبارات Point2D / Point2D Tests
// ============================================================================

TEST_CASE("Point2D default constructor", "[types][point]") {
    Point2D p;
    REQUIRE(p.x == 0.0f);
    REQUIRE(p.y == 0.0f);
}

TEST_CASE("Point2D parameterized constructor", "[types][point]") {
    Point2D p(3.5f, -7.2f);
    REQUIRE(p.x == Approx(3.5f));
    REQUIRE(p.y == Approx(-7.2f));
}

// ============================================================================
// اختبارات Size2D / Size2D Tests
// ============================================================================

TEST_CASE("Size2D default constructor", "[types][size]") {
    Size2D s;
    REQUIRE(s.width == 0.0f);
    REQUIRE(s.height == 0.0f);
}

TEST_CASE("Size2D parameterized constructor", "[types][size]") {
    Size2D s(1920.0f, 1080.0f);
    REQUIRE(s.width == Approx(1920.0f));
    REQUIRE(s.height == Approx(1080.0f));
}

// ============================================================================
// اختبارات Rect / Rect Tests
// ============================================================================

TEST_CASE("Rect default constructor", "[types][rect]") {
    Rect r;
    REQUIRE(r.x == 0.0f);
    REQUIRE(r.y == 0.0f);
    REQUIRE(r.width == 0.0f);
    REQUIRE(r.height == 0.0f);
}

TEST_CASE("Rect parameterized constructor", "[types][rect]") {
    Rect r(10.0f, 20.0f, 300.0f, 200.0f);
    REQUIRE(r.x == Approx(10.0f));
    REQUIRE(r.y == Approx(20.0f));
    REQUIRE(r.width == Approx(300.0f));
    REQUIRE(r.height == Approx(200.0f));
}

// ============================================================================
// اختبارات Vec2 / Vec2 Tests
// ============================================================================

TEST_CASE("Vec2 default constructor", "[types][vec2]") {
    Vec2 v;
    REQUIRE(v.x == 0.0f);
    REQUIRE(v.y == 0.0f);
}

TEST_CASE("Vec2 parameterized constructor", "[types][vec2]") {
    Vec2 v(1.0f, 2.0f);
    REQUIRE(v.x == Approx(1.0f));
    REQUIRE(v.y == Approx(2.0f));
}

// ============================================================================
// اختبارات Vec3 / Vec3 Tests
// ============================================================================

TEST_CASE("Vec3 default constructor", "[types][vec3]") {
    Vec3 v;
    REQUIRE(v.x == 0.0f);
    REQUIRE(v.y == 0.0f);
    REQUIRE(v.z == 0.0f);
}

TEST_CASE("Vec3 parameterized constructor", "[types][vec3]") {
    Vec3 v(1.0f, 2.0f, 3.0f);
    REQUIRE(v.x == Approx(1.0f));
    REQUIRE(v.y == Approx(2.0f));
    REQUIRE(v.z == Approx(3.0f));
}

// ============================================================================
// اختبارات Vec4 / Vec4 Tests
// ============================================================================

TEST_CASE("Vec4 default constructor", "[types][vec4]") {
    Vec4 v;
    REQUIRE(v.x == 0.0f);
    REQUIRE(v.y == 0.0f);
    REQUIRE(v.z == 0.0f);
    REQUIRE(v.w == 0.0f);
}

TEST_CASE("Vec4 parameterized constructor", "[types][vec4]") {
    Vec4 v(1.0f, 2.0f, 3.0f, 4.0f);
    REQUIRE(v.x == Approx(1.0f));
    REQUIRE(v.y == Approx(2.0f));
    REQUIRE(v.z == Approx(3.0f));
    REQUIRE(v.w == Approx(4.0f));
}

// ============================================================================
// اختبارات Mat3 / Mat3 Tests
// ============================================================================

TEST_CASE("Mat3 default constructor initializes to zero", "[types][mat3]") {
    Mat3 m;
    for (int i = 0; i < 9; i++) {
        REQUIRE(m.m[i] == 0.0f);
    }
}

// ============================================================================
// اختبارات Mat4 / Mat4 Tests
// ============================================================================

TEST_CASE("Mat4 default constructor initializes to zero", "[types][mat4]") {
    Mat4 m;
    for (int i = 0; i < 16; i++) {
        REQUIRE(m.m[i] == 0.0f);
    }
}

// ============================================================================
// اختبارات Result enum / Result enum Tests
// ============================================================================

TEST_CASE("Result enum values", "[types][result]") {
    REQUIRE(static_cast<int>(Result::Success) == 0);
    REQUIRE(static_cast<int>(Result::Failure) == 1);
    REQUIRE(Result::Success != Result::Failure);
    REQUIRE(Result::InvalidHandle != Result::InvalidParameter);
}

// ============================================================================
// اختبارات LogLevel enum / LogLevel enum Tests
// ============================================================================

TEST_CASE("LogLevel enum ordering", "[types][loglevel]") {
    // التأكد من أن الترتيب صحيح / Ensure correct ordering
    REQUIRE(static_cast<int>(LogLevel::Debug) < static_cast<int>(LogLevel::Info));
    REQUIRE(static_cast<int>(LogLevel::Info) < static_cast<int>(LogLevel::Warning));
    REQUIRE(static_cast<int>(LogLevel::Warning) < static_cast<int>(LogLevel::Error));
    REQUIRE(static_cast<int>(LogLevel::Error) < static_cast<int>(LogLevel::Critical));
}

// ============================================================================
// اختبارات الثوابت / Constants Tests
// ============================================================================

TEST_CASE("INVALID_HANDLE is zero", "[types][constants]") {
    REQUIRE(INVALID_HANDLE == 0);
}

TEST_CASE("MAX_STRING_LENGTH is 1024", "[types][constants]") {
    REQUIRE(MAX_STRING_LENGTH == 1024);
}

// ============================================================================
// اختبارات الأنواع المستعارة / Type Alias Tests
// ============================================================================

TEST_CASE("Type aliases have correct sizes", "[types][aliases]") {
    REQUIRE(sizeof(u8) == 1);
    REQUIRE(sizeof(u16) == 2);
    REQUIRE(sizeof(u32) == 4);
    REQUIRE(sizeof(u64) == 8);
    REQUIRE(sizeof(i8) == 1);
    REQUIRE(sizeof(i16) == 2);
    REQUIRE(sizeof(i32) == 4);
    REQUIRE(sizeof(i64) == 8);
    REQUIRE(sizeof(Float32) == 4);
    REQUIRE(sizeof(Float64) == 8);
    REQUIRE(sizeof(Int32) == 4);
    REQUIRE(sizeof(UInt32) == 4);
    REQUIRE(sizeof(Int64) == 8);
    REQUIRE(sizeof(UInt64) == 8);
}

// ============================================================================
// اختبارات SadGraphics namespace alias
// ============================================================================

TEST_CASE("SadGraphics namespace alias works", "[types][namespace]") {
    SadGraphics::Point2D p(1.0f, 2.0f);
    REQUIRE(p.x == Approx(1.0f));
    REQUIRE(p.y == Approx(2.0f));
    
    SadGraphics::Color c(0.5f, 0.6f, 0.7f);
    REQUIRE(c.r == Approx(0.5f));
}

// ============================================================================
// اختبارات عمليات المتجهات / Vector Operations Tests
// ============================================================================

TEST_CASE("Vec2 arithmetic operators", "[types][vec2][math]") {
    Vec2 a(3.0f, 4.0f);
    Vec2 b(1.0f, 2.0f);
    
    Vec2 sum = a + b;
    REQUIRE(sum.x == Approx(4.0f));
    REQUIRE(sum.y == Approx(6.0f));
    
    Vec2 diff = a - b;
    REQUIRE(diff.x == Approx(2.0f));
    REQUIRE(diff.y == Approx(2.0f));
    
    Vec2 scaled = a * 2.0f;
    REQUIRE(scaled.x == Approx(6.0f));
    REQUIRE(scaled.y == Approx(8.0f));
    
    Vec2 div = a / 2.0f;
    REQUIRE(div.x == Approx(1.5f));
    REQUIRE(div.y == Approx(2.0f));
    
    Vec2 neg = -a;
    REQUIRE(neg.x == Approx(-3.0f));
    REQUIRE(neg.y == Approx(-4.0f));
}

TEST_CASE("Vec2 dot product and length", "[types][vec2][math]") {
    Vec2 a(3.0f, 4.0f);
    
    REQUIRE(a.LengthSq() == Approx(25.0f));
    REQUIRE(a.Length() == Approx(5.0f));
    
    Vec2 n = a.Normalized();
    REQUIRE(n.x == Approx(0.6f));
    REQUIRE(n.y == Approx(0.8f));
    REQUIRE(n.Length() == Approx(1.0f));
    
    Vec2 b(1.0f, 0.0f);
    REQUIRE(a.Dot(b) == Approx(3.0f));
    
    REQUIRE(a.Distance(Vec2(0.0f, 0.0f)) == Approx(5.0f));
}

TEST_CASE("Vec2 compound assignment", "[types][vec2][math]") {
    Vec2 a(1.0f, 2.0f);
    a += Vec2(3.0f, 4.0f);
    REQUIRE(a.x == Approx(4.0f));
    REQUIRE(a.y == Approx(6.0f));
    
    a *= 0.5f;
    REQUIRE(a.x == Approx(2.0f));
    REQUIRE(a.y == Approx(3.0f));
}

TEST_CASE("Vec3 arithmetic and cross product", "[types][vec3][math]") {
    Vec3 a(1.0f, 0.0f, 0.0f);
    Vec3 b(0.0f, 1.0f, 0.0f);
    
    Vec3 cross = a.Cross(b);
    REQUIRE(cross.x == Approx(0.0f));
    REQUIRE(cross.y == Approx(0.0f));
    REQUIRE(cross.z == Approx(1.0f));
    
    REQUIRE(a.Dot(b) == Approx(0.0f));
    REQUIRE(a.Length() == Approx(1.0f));
    
    Vec3 sum = a + b;
    REQUIRE(sum.x == Approx(1.0f));
    REQUIRE(sum.y == Approx(1.0f));
    REQUIRE(sum.z == Approx(0.0f));
    
    Vec3 reflect = Vec3(1.0f, -1.0f, 0.0f).Reflect(Vec3(0.0f, 1.0f, 0.0f));
    REQUIRE(reflect.x == Approx(1.0f));
    REQUIRE(reflect.y == Approx(1.0f));
}

TEST_CASE("Vec4 operations", "[types][vec4][math]") {
    Vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
    Vec4 b(4.0f, 3.0f, 2.0f, 1.0f);
    
    REQUIRE(a.Dot(b) == Approx(20.0f));
    REQUIRE(a.LengthSq() == Approx(30.0f));
    
    Vec3 xyz = a.XYZ();
    REQUIRE(xyz.x == Approx(1.0f));
    REQUIRE(xyz.y == Approx(2.0f));
    REQUIRE(xyz.z == Approx(3.0f));
}

// ============================================================================
// اختبارات المصفوفات / Matrix Tests
// ============================================================================

TEST_CASE("Mat4 identity", "[types][mat4][math]") {
    Mat4 id = Mat4::Identity();
    REQUIRE(id.m[0] == Approx(1.0f));
    REQUIRE(id.m[5] == Approx(1.0f));
    REQUIRE(id.m[10] == Approx(1.0f));
    REQUIRE(id.m[15] == Approx(1.0f));
    REQUIRE(id.m[1] == Approx(0.0f));
    REQUIRE(id.m[4] == Approx(0.0f));
}

TEST_CASE("Mat4 multiply identity", "[types][mat4][math]") {
    Mat4 id = Mat4::Identity();
    Mat4 t = Mat4::Translate(1.0f, 2.0f, 3.0f);
    Mat4 result = id * t;
    
    REQUIRE(result.m[12] == Approx(1.0f));
    REQUIRE(result.m[13] == Approx(2.0f));
    REQUIRE(result.m[14] == Approx(3.0f));
}

TEST_CASE("Mat4 translate and transform point", "[types][mat4][math]") {
    Mat4 t = Mat4::Translate(10.0f, 20.0f, 30.0f);
    Vec3 p = t.TransformPoint(Vec3(1.0f, 2.0f, 3.0f));
    
    REQUIRE(p.x == Approx(11.0f));
    REQUIRE(p.y == Approx(22.0f));
    REQUIRE(p.z == Approx(33.0f));
}

TEST_CASE("Mat4 scale", "[types][mat4][math]") {
    Mat4 s = Mat4::Scale(2.0f, 3.0f, 4.0f);
    Vec3 p = s.TransformPoint(Vec3(1.0f, 1.0f, 1.0f));
    
    REQUIRE(p.x == Approx(2.0f));
    REQUIRE(p.y == Approx(3.0f));
    REQUIRE(p.z == Approx(4.0f));
}

TEST_CASE("Mat4 rotation Z 90 degrees", "[types][mat4][math]") {
    Float32 angle = 3.14159265359f / 2.0f; // 90 degrees
    Mat4 r = Mat4::RotateZ(angle);
    Vec3 p = r.TransformPoint(Vec3(1.0f, 0.0f, 0.0f));
    
    REQUIRE(p.x == Approx(0.0f).margin(1e-5f));
    REQUIRE(p.y == Approx(1.0f).margin(1e-5f));
    REQUIRE(p.z == Approx(0.0f).margin(1e-5f));
}

TEST_CASE("Mat4 ortho projection", "[types][mat4][math]") {
    Mat4 ortho = Mat4::Ortho(0.0f, 800.0f, 600.0f, 0.0f);
    
    // Center of screen should map to (0, 0) in NDC
    Vec3 center = ortho.TransformPoint(Vec3(400.0f, 300.0f, 0.0f));
    REQUIRE(center.x == Approx(0.0f));
    REQUIRE(center.y == Approx(0.0f));
}

TEST_CASE("Mat4 inverse", "[types][mat4][math]") {
    Mat4 t = Mat4::Translate(5.0f, 10.0f, 15.0f);
    Mat4 inv = t.Inverted();
    Mat4 result = t * inv;
    
    // Should be approximately identity
    REQUIRE(result.m[0] == Approx(1.0f).margin(1e-5f));
    REQUIRE(result.m[5] == Approx(1.0f).margin(1e-5f));
    REQUIRE(result.m[10] == Approx(1.0f).margin(1e-5f));
    REQUIRE(result.m[15] == Approx(1.0f).margin(1e-5f));
    REQUIRE(result.m[12] == Approx(0.0f).margin(1e-5f));
    REQUIRE(result.m[13] == Approx(0.0f).margin(1e-5f));
    REQUIRE(result.m[14] == Approx(0.0f).margin(1e-5f));
}

TEST_CASE("Mat4 determinant", "[types][mat4][math]") {
    Mat4 id = Mat4::Identity();
    REQUIRE(id.Determinant() == Approx(1.0f));
    
    Mat4 s = Mat4::Scale(2.0f);
    REQUIRE(s.Determinant() == Approx(8.0f));
}

TEST_CASE("Mat4 transpose", "[types][mat4][math]") {
    Mat4 t = Mat4::Translate(1.0f, 2.0f, 3.0f);
    Mat4 tt = t.Transposed();
    
    // Translation in column-major m[12,13,14] becomes row [3,7,11]
    REQUIRE(tt.m[3] == Approx(1.0f));
    REQUIRE(tt.m[7] == Approx(2.0f));
    REQUIRE(tt.m[11] == Approx(3.0f));
}

TEST_CASE("Mat4 LookAt", "[types][mat4][math]") {
    Mat4 view = Mat4::LookAt(
        Vec3(0.0f, 0.0f, 5.0f),   // eye
        Vec3(0.0f, 0.0f, 0.0f),   // target
        Vec3(0.0f, 1.0f, 0.0f)    // up
    );
    
    // Eye is at +5 on Z, looking at origin
    // Origin should transform to (0, 0, -5) in view space
    Vec3 origin = view.TransformPoint(Vec3(0.0f, 0.0f, 0.0f));
    REQUIRE(origin.x == Approx(0.0f).margin(1e-5f));
    REQUIRE(origin.y == Approx(0.0f).margin(1e-5f));
    REQUIRE(origin.z == Approx(-5.0f).margin(1e-5f));
}

TEST_CASE("Vec2 perpendicular", "[types][vec2][math]") {
    Vec2 v(1.0f, 0.0f);
    Vec2 perp = v.Perpendicular();
    REQUIRE(perp.x == Approx(0.0f));
    REQUIRE(perp.y == Approx(1.0f));
    REQUIRE(v.Dot(perp) == Approx(0.0f));
}

TEST_CASE("Vec2 cross product (2D)", "[types][vec2][math]") {
    Vec2 a(1.0f, 0.0f);
    Vec2 b(0.0f, 1.0f);
    REQUIRE(a.Cross(b) == Approx(1.0f));
    REQUIRE(b.Cross(a) == Approx(-1.0f));
}

TEST_CASE("Vec constants", "[types][vec][math]") {
    REQUIRE(Vec2::Zero().x == Approx(0.0f));
    REQUIRE(Vec3::Up().y == Approx(1.0f));
    REQUIRE(Vec3::Forward().z == Approx(-1.0f));
    REQUIRE(Vec3::Right().x == Approx(1.0f));
}
