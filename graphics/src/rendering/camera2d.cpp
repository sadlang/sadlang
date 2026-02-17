// ==============================================================================
// camera2d.cpp - تنفيذ نظام الكاميرا ثنائية الأبعاد
// 2D Camera System Implementation
// ==============================================================================

#include "../../include/rendering/camera2d.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

namespace sad {
namespace graphics {

// ==============================================================================
// المُنشئ / Constructor
// ==============================================================================

Camera2D::Camera2D(Float32 viewportWidth, Float32 viewportHeight)
    : m_position(0.0f, 0.0f)
    , m_zoom(1.0f)
    , m_rotation(0.0f)
    , m_viewportSize(viewportWidth, viewportHeight)
    , m_hasBounds(false)
    , m_boundsMinX(0), m_boundsMinY(0), m_boundsMaxX(0), m_boundsMaxY(0)
    , m_isFollowing(false)
    , m_followTarget(0.0f, 0.0f)
    , m_followOffset(0.0f, 0.0f)
    , m_followSpeed(0.1f)
    , m_hasDeadZone(false)
    , m_deadZoneW(0), m_deadZoneH(0)
    , m_shakeIntensity(0.0f)
    , m_shakeDuration(0.0f)
    , m_shakeTimer(0.0f)
    , m_shakeFrequency(30.0f)
    , m_shakeOffset(0.0f, 0.0f)
{
}

// ==============================================================================
// الموقع والتحويل / Position and Transform
// ==============================================================================

void Camera2D::SetPosition(Float32 x, Float32 y) {
    m_position.x = x;
    m_position.y = y;
    ApplyBounds();
}

void Camera2D::SetPosition(const Vec2& pos) {
    m_position = pos;
    ApplyBounds();
}

void Camera2D::SetZoom(Float32 zoom) {
    m_zoom = (zoom < 0.01f) ? 0.01f : zoom; // حد أدنى / Minimum zoom
}

void Camera2D::SetRotation(Float32 angle) {
    m_rotation = angle;
}

void Camera2D::SetViewport(Float32 width, Float32 height) {
    m_viewportSize.x = width;
    m_viewportSize.y = height;
}

// ==============================================================================
// حدود الكاميرا / Camera Bounds
// ==============================================================================

void Camera2D::SetBounds(Float32 minX, Float32 minY, Float32 maxX, Float32 maxY) {
    m_boundsMinX = minX;
    m_boundsMinY = minY;
    m_boundsMaxX = maxX;
    m_boundsMaxY = maxY;
    m_hasBounds = true;
    ApplyBounds();
}

void Camera2D::ClearBounds() {
    m_hasBounds = false;
}

// ==============================================================================
// المتابعة السلسة / Smooth Follow
// ==============================================================================

void Camera2D::SetFollowTarget(const Vec2& target) {
    m_followTarget = target;
    m_isFollowing = true;
}

void Camera2D::StopFollowing() {
    m_isFollowing = false;
}

void Camera2D::SetFollowSpeed(Float32 speed) {
    m_followSpeed = (speed < 0.0f) ? 0.0f : ((speed > 1.0f) ? 1.0f : speed);
}

void Camera2D::SetFollowOffset(const Vec2& offset) {
    m_followOffset = offset;
}

void Camera2D::SetDeadZone(Float32 width, Float32 height) {
    m_deadZoneW = width;
    m_deadZoneH = height;
    m_hasDeadZone = true;
}

void Camera2D::ClearDeadZone() {
    m_hasDeadZone = false;
}

// ==============================================================================
// تأثير الاهتزاز / Shake Effect
// ==============================================================================

void Camera2D::Shake(Float32 intensity, Float32 duration, Float32 frequency) {
    m_shakeIntensity = intensity;
    m_shakeDuration = duration;
    m_shakeTimer = duration;
    m_shakeFrequency = frequency;
}

void Camera2D::StopShake() {
    m_shakeTimer = 0.0f;
    m_shakeOffset.x = 0.0f;
    m_shakeOffset.y = 0.0f;
}

// ==============================================================================
// التحديث / Update
// ==============================================================================

void Camera2D::Update(Float32 deltaTime) {
    // متابعة الهدف / Follow target
    if (m_isFollowing) {
        Vec2 target = m_followTarget + m_followOffset;
        
        if (m_hasDeadZone) {
            // المنطقة الميتة: لا تتحرك إلا إذا خرج الهدف منها
            // Dead zone: don't move unless target exits it
            Float32 halfW = m_deadZoneW * 0.5f;
            Float32 halfH = m_deadZoneH * 0.5f;
            
            Float32 dx = target.x - m_position.x;
            Float32 dy = target.y - m_position.y;
            
            Float32 targetX = m_position.x;
            Float32 targetY = m_position.y;
            
            if (dx > halfW) targetX = target.x - halfW;
            else if (dx < -halfW) targetX = target.x + halfW;
            
            if (dy > halfH) targetY = target.y - halfH;
            else if (dy < -halfH) targetY = target.y + halfH;
            
            target.x = targetX;
            target.y = targetY;
        }
        
        // استيفاء خطي مع تخميد / Linear interpolation with damping
        Float32 t = 1.0f - std::pow(1.0f - m_followSpeed, deltaTime * 60.0f);
        m_position.x += (target.x - m_position.x) * t;
        m_position.y += (target.y - m_position.y) * t;
    }
    
    // تحديث الاهتزاز / Update shake
    UpdateShake(deltaTime);
    
    // تطبيق الحدود / Apply bounds
    ApplyBounds();
}

// ==============================================================================
// المصفوفات / Matrices
// ==============================================================================

void Camera2D::GetViewMatrix(Float32* matrix) const {
    Mat4 view = GetViewMatrix();
    const Float32* data = view.Data();
    for (int i = 0; i < 16; ++i) {
        matrix[i] = data[i];
    }
}

Mat4 Camera2D::GetViewMatrix() const {
    // المصفوفة: Scale * Rotate * Translate
    // تحريك عكسي لموقع الكاميرا + إزاحة الاهتزاز
    Float32 px = -(m_position.x + m_shakeOffset.x);
    Float32 py = -(m_position.y + m_shakeOffset.y);
    
    Mat4 translate = Mat4::Translate(px, py, 0.0f);
    
    Mat4 result = translate;
    
    if (m_rotation != 0.0f) {
        Mat4 rotate = Mat4::RotateZ(-m_rotation);
        result = rotate * result;
    }
    
    if (m_zoom != 1.0f) {
        Mat4 scale = Mat4::Scale(m_zoom, m_zoom, 1.0f);
        result = scale * result;
    }
    
    return result;
}

Mat4 Camera2D::GetProjectionMatrix() const {
    Float32 halfW = m_viewportSize.x * 0.5f;
    Float32 halfH = m_viewportSize.y * 0.5f;
    return Mat4::Ortho(-halfW, halfW, halfH, -halfH);
}

Mat4 Camera2D::GetViewProjectionMatrix() const {
    return GetProjectionMatrix() * GetViewMatrix();
}

// ==============================================================================
// تحويل الإحداثيات / Coordinate Conversion
// ==============================================================================

Vec2 Camera2D::ScreenToWorld(Float32 screenX, Float32 screenY) const {
    // تحويل من إحداثيات الشاشة إلى NDC
    Float32 ndcX = (screenX / m_viewportSize.x) * 2.0f - 1.0f;
    Float32 ndcY = (screenY / m_viewportSize.y) * 2.0f - 1.0f;
    
    // عكس التكبير / Reverse zoom
    Float32 halfW = m_viewportSize.x * 0.5f / m_zoom;
    Float32 halfH = m_viewportSize.y * 0.5f / m_zoom;
    
    Float32 worldX = ndcX * halfW;
    Float32 worldY = ndcY * halfH;
    
    // عكس الدوران / Reverse rotation
    if (m_rotation != 0.0f) {
        Float32 cosA = std::cos(m_rotation);
        Float32 sinA = std::sin(m_rotation);
        Float32 rx = worldX * cosA - worldY * sinA;
        Float32 ry = worldX * sinA + worldY * cosA;
        worldX = rx;
        worldY = ry;
    }
    
    // إضافة موقع الكاميرا / Add camera position
    worldX += m_position.x + m_shakeOffset.x;
    worldY += m_position.y + m_shakeOffset.y;
    
    return Vec2(worldX, worldY);
}

Vec2 Camera2D::ScreenToWorld(const Vec2& screenPos) const {
    return ScreenToWorld(screenPos.x, screenPos.y);
}

Vec2 Camera2D::WorldToScreen(Float32 worldX, Float32 worldY) const {
    // طرح موقع الكاميرا / Subtract camera position
    Float32 relX = worldX - m_position.x - m_shakeOffset.x;
    Float32 relY = worldY - m_position.y - m_shakeOffset.y;
    
    // تطبيق الدوران / Apply rotation
    if (m_rotation != 0.0f) {
        Float32 cosA = std::cos(-m_rotation);
        Float32 sinA = std::sin(-m_rotation);
        Float32 rx = relX * cosA - relY * sinA;
        Float32 ry = relX * sinA + relY * cosA;
        relX = rx;
        relY = ry;
    }
    
    // تطبيق التكبير وتحويل إلى شاشة / Apply zoom and convert to screen
    Float32 screenX = (relX * m_zoom / m_viewportSize.x + 0.5f) * m_viewportSize.x;
    Float32 screenY = (relY * m_zoom / m_viewportSize.y + 0.5f) * m_viewportSize.y;
    
    return Vec2(screenX, screenY);
}

Vec2 Camera2D::WorldToScreen(const Vec2& worldPos) const {
    return WorldToScreen(worldPos.x, worldPos.y);
}

Rect Camera2D::GetVisibleRect() const {
    Float32 halfW = m_viewportSize.x * 0.5f / m_zoom;
    Float32 halfH = m_viewportSize.y * 0.5f / m_zoom;
    
    Float32 cx = m_position.x + m_shakeOffset.x;
    Float32 cy = m_position.y + m_shakeOffset.y;
    
    // إذا كان هناك دوران، نستخدم المستطيل المحيط بالمنطقة المدورة
    // If rotated, use bounding box of rotated area
    if (m_rotation != 0.0f) {
        Float32 cosA = std::abs(std::cos(m_rotation));
        Float32 sinA = std::abs(std::sin(m_rotation));
        Float32 newHalfW = halfW * cosA + halfH * sinA;
        Float32 newHalfH = halfW * sinA + halfH * cosA;
        halfW = newHalfW;
        halfH = newHalfH;
    }
    
    return Rect(cx - halfW, cy - halfH, halfW * 2.0f, halfH * 2.0f);
}

bool Camera2D::IsPointVisible(Float32 x, Float32 y) const {
    Rect r = GetVisibleRect();
    return x >= r.x && x <= r.x + r.width &&
           y >= r.y && y <= r.y + r.height;
}

bool Camera2D::IsPointVisible(const Vec2& point) const {
    return IsPointVisible(point.x, point.y);
}

bool Camera2D::IsRectVisible(Float32 x, Float32 y, Float32 w, Float32 h) const {
    Rect vis = GetVisibleRect();
    return !(x + w < vis.x || x > vis.x + vis.width ||
             y + h < vis.y || y > vis.y + vis.height);
}

// ==============================================================================
// إنشاء ثابت / Static Creation
// ==============================================================================

std::shared_ptr<Camera2D> Camera2D::Create(Float32 viewportWidth, Float32 viewportHeight) {
    return std::make_shared<Camera2D>(viewportWidth, viewportHeight);
}

// ==============================================================================
// دوال مساعدة خاصة / Private Helper Functions
// ==============================================================================

void Camera2D::ApplyBounds() {
    if (!m_hasBounds) return;
    
    Float32 halfW = m_viewportSize.x * 0.5f / m_zoom;
    Float32 halfH = m_viewportSize.y * 0.5f / m_zoom;
    
    // تقييد بحيث لا يخرج مجال الرؤية عن الحدود
    // Clamp so view doesn't exceed bounds
    Float32 minX = m_boundsMinX + halfW;
    Float32 maxX = m_boundsMaxX - halfW;
    Float32 minY = m_boundsMinY + halfH;
    Float32 maxY = m_boundsMaxY - halfH;
    
    if (minX > maxX) {
        // العالم أصغر من العرض: وسّط الكاميرا
        // World smaller than view: center camera
        m_position.x = (m_boundsMinX + m_boundsMaxX) * 0.5f;
    } else {
        if (m_position.x < minX) m_position.x = minX;
        if (m_position.x > maxX) m_position.x = maxX;
    }
    
    if (minY > maxY) {
        m_position.y = (m_boundsMinY + m_boundsMaxY) * 0.5f;
    } else {
        if (m_position.y < minY) m_position.y = minY;
        if (m_position.y > maxY) m_position.y = maxY;
    }
}

void Camera2D::UpdateShake(Float32 deltaTime) {
    if (m_shakeTimer <= 0.0f) {
        m_shakeOffset.x = 0.0f;
        m_shakeOffset.y = 0.0f;
        return;
    }
    
    m_shakeTimer -= deltaTime;
    
    // تخفيف الشدة مع مرور الوقت / Decay intensity over time
    Float32 progress = m_shakeTimer / m_shakeDuration;
    Float32 currentIntensity = m_shakeIntensity * progress;
    
    // اهتزاز شبه عشوائي / Pseudo-random shake
    Float32 time = (m_shakeDuration - m_shakeTimer) * m_shakeFrequency;
    m_shakeOffset.x = std::sin(time * 7.13f) * currentIntensity;
    m_shakeOffset.y = std::cos(time * 11.07f) * currentIntensity;
    
    if (m_shakeTimer <= 0.0f) {
        m_shakeTimer = 0.0f;
        m_shakeOffset.x = 0.0f;
        m_shakeOffset.y = 0.0f;
    }
}

Float32 Camera2D::RandomFloat(Float32 min, Float32 max) const {
    Float32 r = static_cast<Float32>(std::rand()) / RAND_MAX;
    return min + r * (max - min);
}

} // namespace graphics
} // namespace sad
