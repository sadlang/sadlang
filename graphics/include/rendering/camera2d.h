// ==============================================================================
// camera2d.h - نظام الكاميرا ثنائية الأبعاد
// 2D Camera System
// ==============================================================================
// الوصف: كاميرا متقدمة مع تكبير وتدوير ومتابعة سلسة وتأثير اهتزاز
// Description: Advanced camera with zoom, rotation, smooth follow, and shake effect
// ==============================================================================

#pragma once

#include "../core/types.h"
#include <memory>

namespace sad {
namespace graphics {

// ==============================================================================
// فئة الكاميرا ثنائية الأبعاد / 2D Camera Class
// ==============================================================================
class Camera2D {
public:
    // ==============================================================================
    // الإنشاء / Construction
    // ==============================================================================
    
    /// المُنشئ / Constructor
    /// viewportWidth, viewportHeight: أبعاد منطقة العرض / Viewport dimensions
    Camera2D(Float32 viewportWidth, Float32 viewportHeight);
    
    /// المدمر / Destructor
    ~Camera2D() = default;
    
    // ==============================================================================
    // الموقع والتحويل / Position and Transform
    // ==============================================================================
    
    /// تعيين موقع الكاميرا / Set camera position
    void SetPosition(Float32 x, Float32 y);
    void SetPosition(const Vec2& pos);
    
    /// الحصول على موقع الكاميرا / Get camera position
    Vec2 GetPosition() const { return m_position; }
    
    /// تعيين التكبير / Set zoom level
    /// zoom: مستوى التكبير (1.0 = طبيعي) / Zoom level (1.0 = normal)
    void SetZoom(Float32 zoom);
    
    /// الحصول على التكبير / Get zoom level
    Float32 GetZoom() const { return m_zoom; }
    
    /// تعيين الدوران / Set rotation
    /// angle: الزاوية بالراديان / Angle in radians
    void SetRotation(Float32 angle);
    
    /// الحصول على الدوران / Get rotation
    Float32 GetRotation() const { return m_rotation; }
    
    /// تعيين أبعاد منطقة العرض / Set viewport dimensions
    void SetViewport(Float32 width, Float32 height);
    
    /// الحصول على أبعاد العرض / Get viewport dimensions
    Vec2 GetViewportSize() const { return m_viewportSize; }
    
    // ==============================================================================
    // حدود الكاميرا / Camera Bounds
    // ==============================================================================
    
    /// تعيين حدود حركة الكاميرا / Set camera movement bounds
    /// minX, minY: الحد الأدنى / Minimum bounds
    /// maxX, maxY: الحد الأقصى / Maximum bounds
    void SetBounds(Float32 minX, Float32 minY, Float32 maxX, Float32 maxY);
    
    /// إزالة حدود الكاميرا / Remove camera bounds
    void ClearBounds();
    
    /// هل الحدود مفعلة / Are bounds enabled
    bool HasBounds() const { return m_hasBounds; }
    
    // ==============================================================================
    // المتابعة السلسة / Smooth Follow
    // ==============================================================================
    
    /// تعيين هدف المتابعة / Set follow target position
    /// target: موقع الهدف / Target position
    void SetFollowTarget(const Vec2& target);
    
    /// إيقاف المتابعة / Stop following
    void StopFollowing();
    
    /// هل الكاميرا تتابع هدفاً / Is camera following a target
    bool IsFollowing() const { return m_isFollowing; }
    
    /// تعيين سرعة المتابعة (التخميد) / Set follow speed (damping)
    /// speed: سرعة التخميد (0-1, أعلى = أسرع) / Damping speed (0-1, higher = faster)
    void SetFollowSpeed(Float32 speed);
    
    /// تعيين إزاحة المتابعة / Set follow offset
    /// offset: إزاحة عن الهدف / Offset from target
    void SetFollowOffset(const Vec2& offset);
    
    /// تعيين المنطقة الميتة (لا تتحرك الكاميرا إلا خارجها)
    /// Set dead zone (camera doesn't move unless target is outside)
    /// width, height: أبعاد المنطقة الميتة / Dead zone dimensions
    void SetDeadZone(Float32 width, Float32 height);
    
    /// إزالة المنطقة الميتة / Remove dead zone
    void ClearDeadZone();
    
    // ==============================================================================
    // تأثير الاهتزاز / Shake Effect
    // ==============================================================================
    
    /// بدء اهتزاز الكاميرا / Start camera shake
    /// intensity: شدة الاهتزاز بالبكسل / Shake intensity in pixels
    /// duration: مدة الاهتزاز بالثواني / Shake duration in seconds
    /// frequency: تردد الاهتزاز / Shake frequency
    void Shake(Float32 intensity, Float32 duration, Float32 frequency = 30.0f);
    
    /// إيقاف الاهتزاز / Stop shake
    void StopShake();
    
    /// هل الكاميرا تهتز / Is camera shaking
    bool IsShaking() const { return m_shakeTimer > 0.0f; }
    
    // ==============================================================================
    // التحديث والمصفوفات / Update and Matrices
    // ==============================================================================
    
    /// تحديث الكاميرا / Update camera
    /// deltaTime: الوقت المنقضي بالثواني / Elapsed time in seconds
    void Update(Float32 deltaTime);
    
    /// الحصول على مصفوفة العرض / Get view matrix
    /// matrix: مصفوفة 4x4 للإخراج / Output 4x4 matrix
    void GetViewMatrix(Float32* matrix) const;
    
    /// الحصول على مصفوفة العرض كـ Mat4 / Get view matrix as Mat4
    Mat4 GetViewMatrix() const;
    
    /// الحصول على مصفوفة الإسقاط / Get projection matrix
    Mat4 GetProjectionMatrix() const;
    
    /// الحصول على المصفوفة المجمعة / Get combined view-projection matrix
    Mat4 GetViewProjectionMatrix() const;
    
    // ==============================================================================
    // تحويل الإحداثيات / Coordinate Conversion
    // ==============================================================================
    
    /// تحويل من شاشة إلى عالم / Convert screen to world coordinates
    /// screenX, screenY: إحداثيات الشاشة / Screen coordinates
    /// returns: إحداثيات العالم / World coordinates
    Vec2 ScreenToWorld(Float32 screenX, Float32 screenY) const;
    Vec2 ScreenToWorld(const Vec2& screenPos) const;
    
    /// تحويل من عالم إلى شاشة / Convert world to screen coordinates
    /// worldX, worldY: إحداثيات العالم / World coordinates
    /// returns: إحداثيات الشاشة / Screen coordinates
    Vec2 WorldToScreen(Float32 worldX, Float32 worldY) const;
    Vec2 WorldToScreen(const Vec2& worldPos) const;
    
    /// الحصول على مستطيل الرؤية في العالم / Get visible world rectangle
    /// returns: المستطيل (x, y, width, height) / Rectangle (x, y, width, height)
    Rect GetVisibleRect() const;
    
    /// التحقق من أن نقطة مرئية / Check if point is visible
    bool IsPointVisible(Float32 x, Float32 y) const;
    bool IsPointVisible(const Vec2& point) const;
    
    /// التحقق من أن مستطيل مرئي / Check if rectangle is visible
    bool IsRectVisible(Float32 x, Float32 y, Float32 w, Float32 h) const;
    
    // ==============================================================================
    // إنشاء ثابت / Static Creation
    // ==============================================================================
    
    static std::shared_ptr<Camera2D> Create(Float32 viewportWidth, Float32 viewportHeight);

private:
    // الموقع والتحويل / Position and transform
    Vec2 m_position;                 // الموقع / Position
    Float32 m_zoom;                  // مستوى التكبير / Zoom level
    Float32 m_rotation;              // الدوران / Rotation
    Vec2 m_viewportSize;             // أبعاد العرض / Viewport size
    
    // حدود الكاميرا / Camera bounds
    bool m_hasBounds;
    Float32 m_boundsMinX, m_boundsMinY;
    Float32 m_boundsMaxX, m_boundsMaxY;
    
    // المتابعة السلسة / Smooth follow
    bool m_isFollowing;
    Vec2 m_followTarget;
    Vec2 m_followOffset;
    Float32 m_followSpeed;
    bool m_hasDeadZone;
    Float32 m_deadZoneW, m_deadZoneH;
    
    // تأثير الاهتزاز / Shake effect
    Float32 m_shakeIntensity;
    Float32 m_shakeDuration;
    Float32 m_shakeTimer;
    Float32 m_shakeFrequency;
    Vec2 m_shakeOffset;
    
    // تطبيق الحدود / Apply bounds
    void ApplyBounds();
    
    // تحديث الاهتزاز / Update shake
    void UpdateShake(Float32 deltaTime);
    
    // مولد عشوائي بسيط / Simple random generator
    Float32 RandomFloat(Float32 min, Float32 max) const;
};

// أنواع مستعارة / Type aliases
using Camera2DRef = std::shared_ptr<Camera2D>;

} // namespace graphics
} // namespace sad

// اختصار / Shortcut
namespace SadGraphics {
    using Camera2D = sad::graphics::Camera2D;
    using Camera2DRef = sad::graphics::Camera2DRef;
}
