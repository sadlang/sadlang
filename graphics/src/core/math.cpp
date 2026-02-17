// ============================================================================
// math.cpp - تنفيذ الدوال الرياضية / Math Functions Implementation
// ============================================================================
// الوصف: تنفيذ عمليات المتجهات والمصفوفات
// Description: Implementation of vector and matrix operations
// ============================================================================

#include "../../include/core/types.h"
#include <cmath>
#include <cstring>

namespace sad {
namespace graphics {

// ============================================================================
// Vec2 Implementation
// ============================================================================

Float32 Vec2::Length() const {
    return std::sqrt(x * x + y * y);
}

Vec2 Vec2::Normalized() const {
    Float32 len = Length();
    if (len < 1e-8f) return Vec2(0.0f, 0.0f);
    return *this / len;
}

void Vec2::Normalize() {
    Float32 len = Length();
    if (len < 1e-8f) return;
    *this /= len;
}

Float32 Vec2::Distance(const Vec2& v) const {
    return (*this - v).Length();
}

Float32 Vec2::Angle() const {
    return std::atan2(y, x);
}

// ============================================================================
// Vec3 Implementation
// ============================================================================

Float32 Vec3::Length() const {
    return std::sqrt(x * x + y * y + z * z);
}

Vec3 Vec3::Normalized() const {
    Float32 len = Length();
    if (len < 1e-8f) return Vec3(0.0f, 0.0f, 0.0f);
    return *this / len;
}

void Vec3::Normalize() {
    Float32 len = Length();
    if (len < 1e-8f) return;
    *this /= len;
}

Float32 Vec3::Distance(const Vec3& v) const {
    return (*this - v).Length();
}

// ============================================================================
// Vec4 Implementation
// ============================================================================

Float32 Vec4::Length() const {
    return std::sqrt(x * x + y * y + z * z + w * w);
}

Vec4 Vec4::Normalized() const {
    Float32 len = Length();
    if (len < 1e-8f) return Vec4(0.0f, 0.0f, 0.0f, 0.0f);
    return *this / len;
}

// ============================================================================
// Mat4 Implementation
// ============================================================================

Mat4 Mat4::Identity() {
    Mat4 result;
    result.m[0] = result.m[5] = result.m[10] = result.m[15] = 1.0f;
    return result;
}

Mat4 Mat4::operator*(const Mat4& other) const {
    Mat4 result;
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            result.m[col * 4 + row] =
                m[0 * 4 + row] * other.m[col * 4 + 0] +
                m[1 * 4 + row] * other.m[col * 4 + 1] +
                m[2 * 4 + row] * other.m[col * 4 + 2] +
                m[3 * 4 + row] * other.m[col * 4 + 3];
        }
    }
    return result;
}

Vec4 Mat4::operator*(const Vec4& v) const {
    return Vec4(
        m[0] * v.x + m[4] * v.y + m[8]  * v.z + m[12] * v.w,
        m[1] * v.x + m[5] * v.y + m[9]  * v.z + m[13] * v.w,
        m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14] * v.w,
        m[3] * v.x + m[7] * v.y + m[11] * v.z + m[15] * v.w
    );
}

Vec3 Mat4::TransformPoint(const Vec3& p) const {
    Vec4 result = *this * Vec4(p, 1.0f);
    if (std::abs(result.w) > 1e-8f) {
        return Vec3(result.x / result.w, result.y / result.w, result.z / result.w);
    }
    return result.XYZ();
}

Vec3 Mat4::TransformDirection(const Vec3& d) const {
    return Vec3(
        m[0] * d.x + m[4] * d.y + m[8]  * d.z,
        m[1] * d.x + m[5] * d.y + m[9]  * d.z,
        m[2] * d.x + m[6] * d.y + m[10] * d.z
    );
}

Mat4 Mat4::Translate(const Vec3& t) {
    Mat4 result = Identity();
    result.m[12] = t.x;
    result.m[13] = t.y;
    result.m[14] = t.z;
    return result;
}

Mat4 Mat4::Translate(Float32 x, Float32 y, Float32 z) {
    return Translate(Vec3(x, y, z));
}

Mat4 Mat4::Scale(const Vec3& s) {
    Mat4 result;
    result.m[0] = s.x;
    result.m[5] = s.y;
    result.m[10] = s.z;
    result.m[15] = 1.0f;
    return result;
}

Mat4 Mat4::Scale(Float32 x, Float32 y, Float32 z) {
    return Scale(Vec3(x, y, z));
}

Mat4 Mat4::Scale(Float32 s) {
    return Scale(Vec3(s, s, s));
}

Mat4 Mat4::RotateX(Float32 radians) {
    Float32 c = std::cos(radians);
    Float32 s = std::sin(radians);
    Mat4 result = Identity();
    result.m[5] = c;
    result.m[6] = s;
    result.m[9] = -s;
    result.m[10] = c;
    return result;
}

Mat4 Mat4::RotateY(Float32 radians) {
    Float32 c = std::cos(radians);
    Float32 s = std::sin(radians);
    Mat4 result = Identity();
    result.m[0] = c;
    result.m[2] = -s;
    result.m[8] = s;
    result.m[10] = c;
    return result;
}

Mat4 Mat4::RotateZ(Float32 radians) {
    Float32 c = std::cos(radians);
    Float32 s = std::sin(radians);
    Mat4 result = Identity();
    result.m[0] = c;
    result.m[1] = s;
    result.m[4] = -s;
    result.m[5] = c;
    return result;
}

Mat4 Mat4::RotateAxis(const Vec3& axis, Float32 radians) {
    Vec3 n = axis.Normalized();
    Float32 c = std::cos(radians);
    Float32 s = std::sin(radians);
    Float32 t = 1.0f - c;
    
    Mat4 result = Identity();
    result.m[0]  = t * n.x * n.x + c;
    result.m[1]  = t * n.x * n.y + s * n.z;
    result.m[2]  = t * n.x * n.z - s * n.y;
    result.m[4]  = t * n.x * n.y - s * n.z;
    result.m[5]  = t * n.y * n.y + c;
    result.m[6]  = t * n.y * n.z + s * n.x;
    result.m[8]  = t * n.x * n.z + s * n.y;
    result.m[9]  = t * n.y * n.z - s * n.x;
    result.m[10] = t * n.z * n.z + c;
    return result;
}

Mat4 Mat4::Ortho(Float32 left, Float32 right, Float32 bottom, Float32 top,
                 Float32 near_, Float32 far_) {
    Mat4 result;
    result.m[0]  = 2.0f / (right - left);
    result.m[5]  = 2.0f / (top - bottom);
    result.m[10] = -2.0f / (far_ - near_);
    result.m[12] = -(right + left) / (right - left);
    result.m[13] = -(top + bottom) / (top - bottom);
    result.m[14] = -(far_ + near_) / (far_ - near_);
    result.m[15] = 1.0f;
    return result;
}

Mat4 Mat4::Perspective(Float32 fovRadians, Float32 aspect, Float32 near_, Float32 far_) {
    Float32 tanHalfFov = std::tan(fovRadians * 0.5f);
    
    Mat4 result;
    result.m[0]  = 1.0f / (aspect * tanHalfFov);
    result.m[5]  = 1.0f / tanHalfFov;
    result.m[10] = -(far_ + near_) / (far_ - near_);
    result.m[11] = -1.0f;
    result.m[14] = -(2.0f * far_ * near_) / (far_ - near_);
    return result;
}

Mat4 Mat4::LookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
    Vec3 f = (target - eye).Normalized();   // اتجاه الأمام / Forward
    Vec3 r = f.Cross(up).Normalized();       // اتجاه اليمين / Right
    Vec3 u = r.Cross(f);                     // اتجاه الأعلى / Up
    
    Mat4 result = Identity();
    result.m[0]  = r.x;
    result.m[4]  = r.y;
    result.m[8]  = r.z;
    result.m[1]  = u.x;
    result.m[5]  = u.y;
    result.m[9]  = u.z;
    result.m[2]  = -f.x;
    result.m[6]  = -f.y;
    result.m[10] = -f.z;
    result.m[12] = -r.Dot(eye);
    result.m[13] = -u.Dot(eye);
    result.m[14] = f.Dot(eye);
    return result;
}

Mat4 Mat4::Transposed() const {
    Mat4 result;
    for (int col = 0; col < 4; col++)
        for (int row = 0; row < 4; row++)
            result.m[col * 4 + row] = m[row * 4 + col];
    return result;
}

Float32 Mat4::Determinant() const {
    // Laplace expansion along first row
    Float32 a00 = m[0], a01 = m[4], a02 = m[8],  a03 = m[12];
    Float32 a10 = m[1], a11 = m[5], a12 = m[9],  a13 = m[13];
    Float32 a20 = m[2], a21 = m[6], a22 = m[10], a23 = m[14];
    Float32 a30 = m[3], a31 = m[7], a32 = m[11], a33 = m[15];
    
    Float32 det = 
        a00 * (a11 * (a22 * a33 - a23 * a32) - a12 * (a21 * a33 - a23 * a31) + a13 * (a21 * a32 - a22 * a31)) -
        a01 * (a10 * (a22 * a33 - a23 * a32) - a12 * (a20 * a33 - a23 * a30) + a13 * (a20 * a32 - a22 * a30)) +
        a02 * (a10 * (a21 * a33 - a23 * a31) - a11 * (a20 * a33 - a23 * a30) + a13 * (a20 * a31 - a21 * a30)) -
        a03 * (a10 * (a21 * a32 - a22 * a31) - a11 * (a20 * a32 - a22 * a30) + a12 * (a20 * a31 - a21 * a30));
    return det;
}

Mat4 Mat4::Inverted() const {
    // حساب المعكوس باستخدام المصاحب / Compute inverse using adjugate
    Float32 a00 = m[0], a01 = m[4], a02 = m[8],  a03 = m[12];
    Float32 a10 = m[1], a11 = m[5], a12 = m[9],  a13 = m[13];
    Float32 a20 = m[2], a21 = m[6], a22 = m[10], a23 = m[14];
    Float32 a30 = m[3], a31 = m[7], a32 = m[11], a33 = m[15];
    
    Float32 b00 = a00 * a11 - a01 * a10;
    Float32 b01 = a00 * a12 - a02 * a10;
    Float32 b02 = a00 * a13 - a03 * a10;
    Float32 b03 = a01 * a12 - a02 * a11;
    Float32 b04 = a01 * a13 - a03 * a11;
    Float32 b05 = a02 * a13 - a03 * a12;
    Float32 b06 = a20 * a31 - a21 * a30;
    Float32 b07 = a20 * a32 - a22 * a30;
    Float32 b08 = a20 * a33 - a23 * a30;
    Float32 b09 = a21 * a32 - a22 * a31;
    Float32 b10 = a21 * a33 - a23 * a31;
    Float32 b11 = a22 * a33 - a23 * a32;
    
    Float32 det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
    if (std::abs(det) < 1e-8f) return Identity(); // المصفوفة شاذة / Singular matrix
    
    Float32 invDet = 1.0f / det;
    
    Mat4 result;
    result.m[0]  = ( a11 * b11 - a12 * b10 + a13 * b09) * invDet;
    result.m[1]  = (-a10 * b11 + a12 * b08 - a13 * b07) * invDet;
    result.m[2]  = ( a10 * b10 - a11 * b08 + a13 * b06) * invDet;
    result.m[3]  = (-a10 * b09 + a11 * b07 - a12 * b06) * invDet;
    result.m[4]  = (-a01 * b11 + a02 * b10 - a03 * b09) * invDet;
    result.m[5]  = ( a00 * b11 - a02 * b08 + a03 * b07) * invDet;
    result.m[6]  = (-a00 * b10 + a01 * b08 - a03 * b06) * invDet;
    result.m[7]  = ( a00 * b09 - a01 * b07 + a02 * b06) * invDet;
    result.m[8]  = ( a31 * b05 - a32 * b04 + a33 * b03) * invDet;
    result.m[9]  = (-a30 * b05 + a32 * b02 - a33 * b01) * invDet;
    result.m[10] = ( a30 * b04 - a31 * b02 + a33 * b00) * invDet;
    result.m[11] = (-a30 * b03 + a31 * b01 - a32 * b00) * invDet;
    result.m[12] = (-a21 * b05 + a22 * b04 - a23 * b03) * invDet;
    result.m[13] = ( a20 * b05 - a22 * b02 + a23 * b01) * invDet;
    result.m[14] = (-a20 * b04 + a21 * b02 - a23 * b00) * invDet;
    result.m[15] = ( a20 * b03 - a21 * b01 + a22 * b00) * invDet;
    return result;
}

Vec3 Mat4::GetScale() const {
    Float32 sx = Vec3(m[0], m[1], m[2]).Length();
    Float32 sy = Vec3(m[4], m[5], m[6]).Length();
    Float32 sz = Vec3(m[8], m[9], m[10]).Length();
    return Vec3(sx, sy, sz);
}

} // namespace graphics
} // namespace sad
