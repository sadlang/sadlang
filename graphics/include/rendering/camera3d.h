// ============================================================================
// camera3d.h - نظام الكاميرا ثلاثية الأبعاد
// 3D Camera System
// ============================================================================
// الوصف: يحتوي على أنواع مختلفة من الكاميرات للرؤية ثلاثية الأبعاد
// Description: Contains different camera types for 3D viewing
// ============================================================================

#ifndef SAD_GRAPHICS_CAMERA3D_H
#define SAD_GRAPHICS_CAMERA3D_H

#include "../core/types.h"
#include "../core/math3d.h"
#include <algorithm>

namespace sad {
namespace graphics {

// ============================================================================
// نوع الإسقاط / Projection Type
// ============================================================================

/// نوع إسقاط الكاميرا / Camera projection type
enum class ProjectionType {
    Perspective,   ///< منظور (للألعاب والتطبيقات ثلاثية الأبعاد) / Perspective (for games and 3D apps)
    Orthographic   ///< متعامد (للـ CAD و 2D) / Orthographic (for CAD and 2D)
};

// ============================================================================
// الكاميرا الأساسية / Base Camera
// ============================================================================

/// الكاميرا ثلاثية الأبعاد الأساسية / Base 3D Camera
class Camera3D {
public:
    // ============================================
    // المنشئات / Constructors
    // ============================================
    
    /// المنشئ الافتراضي / Default constructor
    Camera3D()
        : m_position(0, 0, 5)
        , m_rotation(Quat::Identity())
        , m_fov(Radians(60.0f))
        , m_aspectRatio(16.0f / 9.0f)
        , m_nearPlane(0.1f)
        , m_farPlane(1000.0f)
        , m_orthoSize(10.0f)
        , m_projectionType(ProjectionType::Perspective)
        , m_viewDirty(true)
        , m_projDirty(true)
    {}
    
    /// منشئ منظوري / Perspective constructor
    Camera3D(Float32 fovDegrees, Float32 aspect, Float32 nearPlane, Float32 farPlane)
        : m_position(0, 0, 5)
        , m_rotation(Quat::Identity())
        , m_fov(Radians(fovDegrees))
        , m_aspectRatio(aspect)
        , m_nearPlane(nearPlane)
        , m_farPlane(farPlane)
        , m_orthoSize(10.0f)
        , m_projectionType(ProjectionType::Perspective)
        , m_viewDirty(true)
        , m_projDirty(true)
    {}
    
    // ============================================
    // الموضع والاتجاه / Position and Orientation
    // ============================================
    
    /// الحصول على الموقع / Get position
    const Vec3& GetPosition() const { return m_position; }
    
    /// تعيين الموقع / Set position
    void SetPosition(const Vec3& pos) {
        m_position = pos;
        m_viewDirty = true;
    }
    
    /// تعيين الموقع / Set position (xyz)
    void SetPosition(Float32 x, Float32 y, Float32 z) {
        SetPosition(Vec3(x, y, z));
    }
    
    /// الحصول على الدوران / Get rotation
    const Quat& GetRotation() const { return m_rotation; }
    
    /// تعيين الدوران / Set rotation
    void SetRotation(const Quat& rot) {
        m_rotation = rot;
        m_viewDirty = true;
    }
    
    /// تعيين الدوران بزوايا أويلر / Set rotation from Euler angles
    void SetRotation(Float32 pitch, Float32 yaw, Float32 roll) {
        SetRotation(Quat::FromEuler(pitch, yaw, roll));
    }
    
    /// النظر إلى نقطة / Look at point
    void LookAt(const Vec3& target, const Vec3& up = Vec3::Up()) {
        Vec3 forward = (target - m_position).Normalized();
        m_rotation = Quat::LookRotation(forward, up);
        m_viewDirty = true;
    }
    
    // ============================================
    // الاتجاهات / Directions
    // ============================================
    
    /// متجه الأمام / Forward vector
    Vec3 Forward() const { return m_rotation * Vec3::Forward(); }
    
    /// متجه الخلف / Back vector
    Vec3 Back() const { return -Forward(); }
    
    /// متجه الأعلى / Up vector
    Vec3 Up() const { return m_rotation * Vec3::Up(); }
    
    /// متجه الأسفل / Down vector
    Vec3 Down() const { return -Up(); }
    
    /// متجه اليمين / Right vector
    Vec3 Right() const { return m_rotation * Vec3::Right(); }
    
    /// متجه اليسار / Left vector
    Vec3 Left() const { return -Right(); }
    
    // ============================================
    // التحريك / Movement
    // ============================================
    
    /// التحريك للأمام/الخلف / Move forward/backward
    void MoveForward(Float32 distance) {
        m_position = m_position + Forward() * distance;
        m_viewDirty = true;
    }
    
    /// التحريك لليمين/اليسار / Move right/left
    void MoveRight(Float32 distance) {
        m_position = m_position + Right() * distance;
        m_viewDirty = true;
    }
    
    /// التحريك للأعلى/الأسفل / Move up/down
    void MoveUp(Float32 distance) {
        m_position = m_position + Up() * distance;
        m_viewDirty = true;
    }
    
    /// التحريك في الفضاء العالمي / Move in world space
    void Move(const Vec3& delta) {
        m_position = m_position + delta;
        m_viewDirty = true;
    }
    
    /// التحريك في الفضاء المحلي / Move in local space
    void MoveLocal(const Vec3& delta) {
        m_position = m_position + m_rotation * delta;
        m_viewDirty = true;
    }
    
    // ============================================
    // الدوران / Rotation
    // ============================================
    
    /// الدوران حول المحور X / Rotate around X axis (pitch)
    void RotatePitch(Float32 radians) {
        m_rotation = m_rotation * Quat::FromAxisAngle(Vec3::Right(), radians);
        m_viewDirty = true;
    }
    
    /// الدوران حول المحور Y / Rotate around Y axis (yaw)
    void RotateYaw(Float32 radians) {
        m_rotation = Quat::FromAxisAngle(Vec3::Up(), radians) * m_rotation;
        m_viewDirty = true;
    }
    
    /// الدوران حول المحور Z / Rotate around Z axis (roll)
    void RotateRoll(Float32 radians) {
        m_rotation = m_rotation * Quat::FromAxisAngle(Vec3::Forward(), radians);
        m_viewDirty = true;
    }
    
    /// الدوران حول محور معين / Rotate around arbitrary axis
    void Rotate(const Vec3& axis, Float32 radians) {
        m_rotation = Quat::FromAxisAngle(axis, radians) * m_rotation;
        m_viewDirty = true;
    }
    
    /// الدوران حول نقطة / Rotate around a point
    void RotateAround(const Vec3& point, const Vec3& axis, Float32 radians) {
        Quat rot = Quat::FromAxisAngle(axis, radians);
        m_position = point + rot * (m_position - point);
        m_rotation = rot * m_rotation;
        m_viewDirty = true;
    }
    
    // ============================================
    // خصائص الإسقاط / Projection Properties
    // ============================================
    
    /// الحصول على نوع الإسقاط / Get projection type
    ProjectionType GetProjectionType() const { return m_projectionType; }
    
    /// تعيين نوع الإسقاط / Set projection type
    void SetProjectionType(ProjectionType type) {
        m_projectionType = type;
        m_projDirty = true;
    }
    
    /// الحصول على زاوية الرؤية / Get field of view (radians)
    Float32 GetFOV() const { return m_fov; }
    
    /// تعيين زاوية الرؤية / Set field of view (radians)
    void SetFOV(Float32 radians) {
        m_fov = radians;
        m_projDirty = true;
    }
    
    /// تعيين زاوية الرؤية بالدرجات / Set field of view (degrees)
    void SetFOVDegrees(Float32 degrees) {
        SetFOV(Radians(degrees));
    }
    
    /// الحصول على نسبة العرض إلى الارتفاع / Get aspect ratio
    Float32 GetAspectRatio() const { return m_aspectRatio; }
    
    /// تعيين نسبة العرض إلى الارتفاع / Set aspect ratio
    void SetAspectRatio(Float32 aspect) {
        m_aspectRatio = aspect;
        m_projDirty = true;
    }
    
    /// تعيين نسبة العرض من أبعاد الشاشة / Set aspect ratio from screen dimensions
    void SetAspectRatio(Float32 width, Float32 height) {
        SetAspectRatio(width / height);
    }
    
    /// الحصول على المستوى القريب / Get near plane
    Float32 GetNearPlane() const { return m_nearPlane; }
    
    /// تعيين المستوى القريب / Set near plane
    void SetNearPlane(Float32 nearPlane) {
        m_nearPlane = nearPlane;
        m_projDirty = true;
    }
    
    /// الحصول على المستوى البعيد / Get far plane
    Float32 GetFarPlane() const { return m_farPlane; }
    
    /// تعيين المستوى البعيد / Set far plane
    void SetFarPlane(Float32 farPlane) {
        m_farPlane = farPlane;
        m_projDirty = true;
    }
    
    /// تعيين المستويات / Set near and far planes
    void SetClipPlanes(Float32 nearPlane, Float32 farPlane) {
        m_nearPlane = nearPlane;
        m_farPlane = farPlane;
        m_projDirty = true;
    }
    
    /// الحصول على حجم العرض المتعامد / Get orthographic size
    Float32 GetOrthoSize() const { return m_orthoSize; }
    
    /// تعيين حجم العرض المتعامد / Set orthographic size
    void SetOrthoSize(Float32 size) {
        m_orthoSize = size;
        m_projDirty = true;
    }
    
    // ============================================
    // المصفوفات / Matrices
    // ============================================
    
    /// الحصول على مصفوفة العرض / Get view matrix
    const Mat4& GetViewMatrix() const {
        if (m_viewDirty) {
            UpdateViewMatrix();
        }
        return m_viewMatrix;
    }
    
    /// الحصول على مصفوفة الإسقاط / Get projection matrix
    const Mat4& GetProjectionMatrix() const {
        if (m_projDirty) {
            UpdateProjectionMatrix();
        }
        return m_projMatrix;
    }
    
    /// الحصول على مصفوفة العرض-الإسقاط / Get view-projection matrix
    Mat4 GetViewProjectionMatrix() const {
        return GetProjectionMatrix() * GetViewMatrix();
    }
    
    /// الحصول على مصفوفة العرض المعكوسة / Get inverse view matrix
    Mat4 GetInverseViewMatrix() const {
        return GetViewMatrix().Inverted();
    }
    
    // ============================================
    // تحويل الإحداثيات / Coordinate Transformation
    // ============================================
    
    /// تحويل نقطة من فضاء العالم إلى فضاء الشاشة / World to screen space
    Vec3 WorldToScreen(const Vec3& worldPos, Float32 screenWidth, Float32 screenHeight) const {
        Vec4 clipPos = GetViewProjectionMatrix() * Vec4(worldPos, 1.0f);
        
        if (Abs(clipPos.w) < EPSILON) return Vec3::Zero();
        
        Vec3 ndc = clipPos.XYZ() / clipPos.w;
        
        return Vec3(
            (ndc.x + 1.0f) * 0.5f * screenWidth,
            (1.0f - ndc.y) * 0.5f * screenHeight,  // Y معكوس / Y inverted
            ndc.z
        );
    }
    
    /// تحويل نقطة من فضاء الشاشة إلى فضاء العالم / Screen to world space
    Vec3 ScreenToWorld(const Vec3& screenPos, Float32 screenWidth, Float32 screenHeight) const {
        Vec3 ndc(
            (screenPos.x / screenWidth) * 2.0f - 1.0f,
            1.0f - (screenPos.y / screenHeight) * 2.0f,  // Y معكوس / Y inverted
            screenPos.z
        );
        
        Mat4 invVP = GetViewProjectionMatrix().Inverted();
        Vec4 worldPos = invVP * Vec4(ndc, 1.0f);
        
        if (Abs(worldPos.w) < EPSILON) return Vec3::Zero();
        
        return worldPos.XYZ() / worldPos.w;
    }
    
    /// إنشاء شعاع من نقطة على الشاشة / Create ray from screen point
    Ray ScreenToRay(Float32 screenX, Float32 screenY, Float32 screenWidth, Float32 screenHeight) const {
        Vec3 nearPoint = ScreenToWorld(Vec3(screenX, screenY, 0.0f), screenWidth, screenHeight);
        Vec3 farPoint = ScreenToWorld(Vec3(screenX, screenY, 1.0f), screenWidth, screenHeight);
        
        return Ray(nearPoint, (farPoint - nearPoint).Normalized());
    }
    
    // ============================================
    // هرم الرؤية / View Frustum
    // ============================================
    
    /// الحصول على هرم الرؤية / Get view frustum
    Frustum GetFrustum() const {
        return Frustum::FromMatrix(GetViewProjectionMatrix());
    }
    
    /// أركان هرم الرؤية / Frustum corners
    std::array<Vec3, 8> GetFrustumCorners() const {
        Mat4 invVP = GetViewProjectionMatrix().Inverted();
        
        // NDC corners / أركان NDC
        Vec4 corners[8] = {
            Vec4(-1, -1, -1, 1), Vec4( 1, -1, -1, 1),
            Vec4(-1,  1, -1, 1), Vec4( 1,  1, -1, 1),
            Vec4(-1, -1,  1, 1), Vec4( 1, -1,  1, 1),
            Vec4(-1,  1,  1, 1), Vec4( 1,  1,  1, 1)
        };
        
        std::array<Vec3, 8> worldCorners;
        for (int i = 0; i < 8; i++) {
            Vec4 world = invVP * corners[i];
            worldCorners[i] = world.XYZ() / world.w;
        }
        
        return worldCorners;
    }
    
protected:
    /// تحديث مصفوفة العرض / Update view matrix
    void UpdateViewMatrix() const {
        // View matrix = inverse of camera transform
        Mat4 rotMatrix = m_rotation.ToMatrix();
        m_viewMatrix = rotMatrix.Transposed();
        
        Vec3 eye = Vec3(
            -m_position.Dot(Vec3(rotMatrix.m[0], rotMatrix.m[4], rotMatrix.m[8])),
            -m_position.Dot(Vec3(rotMatrix.m[1], rotMatrix.m[5], rotMatrix.m[9])),
            -m_position.Dot(Vec3(rotMatrix.m[2], rotMatrix.m[6], rotMatrix.m[10]))
        );
        
        m_viewMatrix.m[12] = eye.x;
        m_viewMatrix.m[13] = eye.y;
        m_viewMatrix.m[14] = eye.z;
        
        m_viewDirty = false;
    }
    
    /// تحديث مصفوفة الإسقاط / Update projection matrix
    void UpdateProjectionMatrix() const {
        if (m_projectionType == ProjectionType::Perspective) {
            m_projMatrix = Mat4::Perspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
        } else {
            Float32 halfHeight = m_orthoSize * 0.5f;
            Float32 halfWidth = halfHeight * m_aspectRatio;
            m_projMatrix = Mat4::Ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, m_nearPlane, m_farPlane);
        }
        m_projDirty = false;
    }
    
protected:
    Vec3 m_position;                    ///< الموقع / Position
    Quat m_rotation;                    ///< الدوران / Rotation
    
    Float32 m_fov;                      ///< زاوية الرؤية / Field of view
    Float32 m_aspectRatio;              ///< نسبة العرض / Aspect ratio
    Float32 m_nearPlane;                ///< المستوى القريب / Near plane
    Float32 m_farPlane;                 ///< المستوى البعيد / Far plane
    Float32 m_orthoSize;                ///< الحجم المتعامد / Orthographic size
    
    ProjectionType m_projectionType;    ///< نوع الإسقاط / Projection type
    
    mutable Mat4 m_viewMatrix;          ///< مصفوفة العرض / View matrix
    mutable Mat4 m_projMatrix;          ///< مصفوفة الإسقاط / Projection matrix
    mutable bool m_viewDirty;           ///< علامة تحديث العرض / View dirty flag
    mutable bool m_projDirty;           ///< علامة تحديث الإسقاط / Projection dirty flag
};

// ============================================================================
// كاميرا الطيران الحر / Fly Camera
// ============================================================================

/// كاميرا طيران حر للملاحة في البيئات ثلاثية الأبعاد / Free-fly camera for 3D navigation
class FlyCamera : public Camera3D {
public:
    /// المنشئ الافتراضي / Default constructor
    FlyCamera() 
        : m_pitch(0.0f)
        , m_yaw(0.0f)
        , m_sensitivity(0.005f)
        , m_moveSpeed(5.0f)
        , m_sprintMultiplier(2.0f) {}
    
    /// تحديث من إدخال الماوس / Update from mouse input
    void ProcessMouseMovement(Float32 deltaX, Float32 deltaY) {
        m_yaw -= deltaX * m_sensitivity;
        m_pitch -= deltaY * m_sensitivity;
        
        // تقييد الميلان / Clamp pitch
        m_pitch = Clamp(m_pitch, -HALF_PI + 0.1f, HALF_PI - 0.1f);
        
        UpdateRotationFromAngles();
    }
    
    /// تحديث من الإدخال / Update from input
    void Update(Float32 deltaTime, bool forward, bool backward, bool left, bool right, 
                bool up, bool down, bool sprint = false) {
        Float32 speed = m_moveSpeed * deltaTime;
        if (sprint) speed *= m_sprintMultiplier;
        
        Vec3 movement = Vec3::Zero();
        
        if (forward) movement = movement + Forward();
        if (backward) movement = movement - Forward();
        if (right) movement = movement + Right();
        if (left) movement = movement - Right();
        if (up) movement = movement + Vec3::Up();
        if (down) movement = movement - Vec3::Up();
        
        if (movement.LengthSq() > 0.0f) {
            movement.Normalize();
            Move(movement * speed);
        }
    }
    
    /// الحساسية / Sensitivity
    Float32 GetSensitivity() const { return m_sensitivity; }
    void SetSensitivity(Float32 sens) { m_sensitivity = sens; }
    
    /// سرعة الحركة / Move speed
    Float32 GetMoveSpeed() const { return m_moveSpeed; }
    void SetMoveSpeed(Float32 speed) { m_moveSpeed = speed; }
    
    /// مضاعف الركض / Sprint multiplier
    Float32 GetSprintMultiplier() const { return m_sprintMultiplier; }
    void SetSprintMultiplier(Float32 mult) { m_sprintMultiplier = mult; }
    
    /// زاوية الميلان / Pitch angle
    Float32 GetPitch() const { return m_pitch; }
    void SetPitch(Float32 pitch) { m_pitch = pitch; UpdateRotationFromAngles(); }
    
    /// زاوية الانعراج / Yaw angle
    Float32 GetYaw() const { return m_yaw; }
    void SetYaw(Float32 yaw) { m_yaw = yaw; UpdateRotationFromAngles(); }
    
private:
    void UpdateRotationFromAngles() {
        SetRotation(Quat::FromEuler(m_pitch, m_yaw, 0.0f));
    }
    
    Float32 m_pitch;            ///< الميلان / Pitch
    Float32 m_yaw;              ///< الانعراج / Yaw
    Float32 m_sensitivity;      ///< الحساسية / Sensitivity
    Float32 m_moveSpeed;        ///< سرعة الحركة / Move speed
    Float32 m_sprintMultiplier; ///< مضاعف الركض / Sprint multiplier
};

// ============================================================================
// كاميرا المدار / Orbit Camera
// ============================================================================

/// كاميرا مدارية تدور حول هدف / Orbital camera that rotates around a target
class OrbitCamera : public Camera3D {
public:
    /// المنشئ الافتراضي / Default constructor
    OrbitCamera()
        : m_target(Vec3::Zero())
        , m_distance(5.0f)
        , m_minDistance(1.0f)
        , m_maxDistance(100.0f)
        , m_azimuth(0.0f)
        , m_elevation(0.3f)
        , m_minElevation(-HALF_PI + 0.1f)
        , m_maxElevation(HALF_PI - 0.1f)
        , m_sensitivity(0.01f)
        , m_zoomSpeed(1.0f)
        , m_panSpeed(1.0f)
    {
        UpdatePosition();
    }
    
    /// الحصول على الهدف / Get target
    const Vec3& GetTarget() const { return m_target; }
    
    /// تعيين الهدف / Set target
    void SetTarget(const Vec3& target) {
        m_target = target;
        UpdatePosition();
    }
    
    /// الحصول على المسافة / Get distance
    Float32 GetDistance() const { return m_distance; }
    
    /// تعيين المسافة / Set distance
    void SetDistance(Float32 dist) {
        m_distance = Clamp(dist, m_minDistance, m_maxDistance);
        UpdatePosition();
    }
    
    /// تعيين حدود المسافة / Set distance limits
    void SetDistanceLimits(Float32 minDist, Float32 maxDist) {
        m_minDistance = minDist;
        m_maxDistance = maxDist;
        m_distance = Clamp(m_distance, m_minDistance, m_maxDistance);
    }
    
    /// الدوران بالماوس / Rotate with mouse
    void ProcessMouseMovement(Float32 deltaX, Float32 deltaY) {
        m_azimuth -= deltaX * m_sensitivity;
        m_elevation += deltaY * m_sensitivity;
        
        m_elevation = Clamp(m_elevation, m_minElevation, m_maxElevation);
        
        UpdatePosition();
    }
    
    /// التكبير/التصغير / Zoom
    void Zoom(Float32 delta) {
        m_distance -= delta * m_zoomSpeed;
        m_distance = Clamp(m_distance, m_minDistance, m_maxDistance);
        UpdatePosition();
    }
    
    /// التحريك الجانبي / Pan
    void Pan(Float32 deltaX, Float32 deltaY) {
        Vec3 right = Right() * deltaX * m_panSpeed;
        Vec3 up = Up() * deltaY * m_panSpeed;
        m_target = m_target + right + up;
        UpdatePosition();
    }
    
    /// إعادة التعيين / Reset to default position
    void Reset() {
        m_target = Vec3::Zero();
        m_distance = 5.0f;
        m_azimuth = 0.0f;
        m_elevation = 0.3f;
        UpdatePosition();
    }
    
    /// الحساسية / Sensitivity
    Float32 GetSensitivity() const { return m_sensitivity; }
    void SetSensitivity(Float32 sens) { m_sensitivity = sens; }
    
    /// سرعة التكبير / Zoom speed
    Float32 GetZoomSpeed() const { return m_zoomSpeed; }
    void SetZoomSpeed(Float32 speed) { m_zoomSpeed = speed; }
    
    /// سرعة التحريك / Pan speed
    Float32 GetPanSpeed() const { return m_panSpeed; }
    void SetPanSpeed(Float32 speed) { m_panSpeed = speed; }
    
private:
    void UpdatePosition() {
        Float32 x = m_distance * std::cos(m_elevation) * std::sin(m_azimuth);
        Float32 y = m_distance * std::sin(m_elevation);
        Float32 z = m_distance * std::cos(m_elevation) * std::cos(m_azimuth);
        
        SetPosition(m_target + Vec3(x, y, z));
        LookAt(m_target);
    }
    
    Vec3 m_target;          ///< الهدف / Target
    Float32 m_distance;     ///< المسافة / Distance
    Float32 m_minDistance;  ///< أقل مسافة / Min distance
    Float32 m_maxDistance;  ///< أقصى مسافة / Max distance
    Float32 m_azimuth;      ///< السمت / Azimuth
    Float32 m_elevation;    ///< الارتفاع / Elevation
    Float32 m_minElevation; ///< أقل ارتفاع / Min elevation
    Float32 m_maxElevation; ///< أقصى ارتفاع / Max elevation
    Float32 m_sensitivity;  ///< الحساسية / Sensitivity
    Float32 m_zoomSpeed;    ///< سرعة التكبير / Zoom speed
    Float32 m_panSpeed;     ///< سرعة التحريك / Pan speed
};

// ============================================================================
// كاميرا الشخص الأول / First Person Camera
// ============================================================================

/// كاميرا الشخص الأول للألعاب / First-person camera for games
class FirstPersonCamera : public Camera3D {
public:
    /// المنشئ الافتراضي / Default constructor
    FirstPersonCamera()
        : m_pitch(0.0f)
        , m_yaw(0.0f)
        , m_sensitivity(0.003f)
        , m_headBobEnabled(false)
        , m_headBobFrequency(10.0f)
        , m_headBobAmplitude(0.05f)
        , m_headBobTime(0.0f)
        , m_eyeHeight(1.7f)
    {}
    
    /// تحديث من إدخال الماوس / Update from mouse input
    void ProcessMouseMovement(Float32 deltaX, Float32 deltaY) {
        m_yaw -= deltaX * m_sensitivity;
        m_pitch -= deltaY * m_sensitivity;
        
        m_pitch = Clamp(m_pitch, -HALF_PI + 0.1f, HALF_PI - 0.1f);
        
        SetRotation(Quat::FromEuler(m_pitch, m_yaw, 0.0f));
    }
    
    /// تحديث موقع الجسم / Update body position (camera follows)
    void SetBodyPosition(const Vec3& bodyPos, Float32 deltaTime, bool isMoving) {
        Vec3 eyePos = bodyPos + Vec3(0, m_eyeHeight, 0);
        
        // Head bob effect
        if (m_headBobEnabled && isMoving) {
            m_headBobTime += deltaTime * m_headBobFrequency;
            Float32 bobOffset = std::sin(m_headBobTime) * m_headBobAmplitude;
            eyePos.y += bobOffset;
        } else {
            m_headBobTime = 0.0f;
        }
        
        SetPosition(eyePos);
    }
    
    /// تفعيل/تعطيل اهتزاز الرأس / Enable/disable head bob
    void SetHeadBobEnabled(bool enabled) { m_headBobEnabled = enabled; }
    bool IsHeadBobEnabled() const { return m_headBobEnabled; }
    
    /// خصائص اهتزاز الرأس / Head bob properties
    void SetHeadBobFrequency(Float32 freq) { m_headBobFrequency = freq; }
    void SetHeadBobAmplitude(Float32 amp) { m_headBobAmplitude = amp; }
    
    /// ارتفاع العين / Eye height
    Float32 GetEyeHeight() const { return m_eyeHeight; }
    void SetEyeHeight(Float32 height) { m_eyeHeight = height; }
    
    /// الحساسية / Sensitivity
    Float32 GetSensitivity() const { return m_sensitivity; }
    void SetSensitivity(Float32 sens) { m_sensitivity = sens; }
    
private:
    Float32 m_pitch;            ///< الميلان / Pitch
    Float32 m_yaw;              ///< الانعراج / Yaw
    Float32 m_sensitivity;      ///< الحساسية / Sensitivity
    bool m_headBobEnabled;      ///< اهتزاز الرأس مفعل / Head bob enabled
    Float32 m_headBobFrequency; ///< تردد الاهتزاز / Bob frequency
    Float32 m_headBobAmplitude; ///< سعة الاهتزاز / Bob amplitude
    Float32 m_headBobTime;      ///< وقت الاهتزاز / Bob time
    Float32 m_eyeHeight;        ///< ارتفاع العين / Eye height
};

// ============================================================================
// كاميرا الشخص الثالث / Third Person Camera
// ============================================================================

/// كاميرا الشخص الثالث للألعاب / Third-person camera for games
class ThirdPersonCamera : public Camera3D {
public:
    /// المنشئ الافتراضي / Default constructor
    ThirdPersonCamera()
        : m_targetPosition(Vec3::Zero())
        , m_offset(0.0f, 2.0f, -5.0f)
        , m_pitch(0.0f)
        , m_yaw(0.0f)
        , m_distance(5.0f)
        , m_minDistance(2.0f)
        , m_maxDistance(15.0f)
        , m_sensitivity(0.003f)
        , m_followSpeed(10.0f)
        , m_collisionEnabled(true)
    {
        UpdateCameraPosition();
    }
    
    /// تحديث / Update
    void Update(Float32 deltaTime) {
        Vec3 desiredPos = CalculateDesiredPosition();
        
        // تتبع سلس / Smooth follow
        Vec3 currentPos = GetPosition();
        Vec3 newPos = currentPos.Lerp(desiredPos, 1.0f - std::exp(-m_followSpeed * deltaTime));
        
        SetPosition(newPos);
        LookAt(m_targetPosition + Vec3(0, m_offset.y * 0.5f, 0));
    }
    
    /// تحديث من إدخال الماوس / Update from mouse input
    void ProcessMouseMovement(Float32 deltaX, Float32 deltaY) {
        m_yaw -= deltaX * m_sensitivity;
        m_pitch -= deltaY * m_sensitivity;
        
        m_pitch = Clamp(m_pitch, -0.8f, 0.8f);
    }
    
    /// التكبير/التصغير / Zoom
    void Zoom(Float32 delta) {
        m_distance -= delta;
        m_distance = Clamp(m_distance, m_minDistance, m_maxDistance);
    }
    
    /// تعيين موقع الهدف / Set target position
    void SetTargetPosition(const Vec3& pos) {
        m_targetPosition = pos;
    }
    
    /// الحصول على موقع الهدف / Get target position
    const Vec3& GetTargetPosition() const { return m_targetPosition; }
    
    /// الإزاحة / Offset
    const Vec3& GetOffset() const { return m_offset; }
    void SetOffset(const Vec3& offset) { m_offset = offset; }
    
    /// المسافة / Distance
    Float32 GetDistance() const { return m_distance; }
    void SetDistance(Float32 dist) { m_distance = Clamp(dist, m_minDistance, m_maxDistance); }
    
    /// حدود المسافة / Distance limits
    void SetDistanceLimits(Float32 minDist, Float32 maxDist) {
        m_minDistance = minDist;
        m_maxDistance = maxDist;
    }
    
    /// الحساسية / Sensitivity
    Float32 GetSensitivity() const { return m_sensitivity; }
    void SetSensitivity(Float32 sens) { m_sensitivity = sens; }
    
    /// سرعة التتبع / Follow speed
    Float32 GetFollowSpeed() const { return m_followSpeed; }
    void SetFollowSpeed(Float32 speed) { m_followSpeed = speed; }
    
private:
    Vec3 CalculateDesiredPosition() const {
        Float32 horizontalDist = m_distance * std::cos(m_pitch);
        Float32 verticalDist = m_distance * std::sin(m_pitch);
        
        Vec3 offset(
            horizontalDist * std::sin(m_yaw),
            verticalDist + m_offset.y,
            horizontalDist * std::cos(m_yaw)
        );
        
        return m_targetPosition + offset;
    }
    
    void UpdateCameraPosition() {
        SetPosition(CalculateDesiredPosition());
        LookAt(m_targetPosition + Vec3(0, m_offset.y * 0.5f, 0));
    }
    
    Vec3 m_targetPosition;  ///< موقع الهدف / Target position
    Vec3 m_offset;          ///< الإزاحة / Offset
    Float32 m_pitch;        ///< الميلان / Pitch
    Float32 m_yaw;          ///< الانعراج / Yaw
    Float32 m_distance;     ///< المسافة / Distance
    Float32 m_minDistance;  ///< أقل مسافة / Min distance
    Float32 m_maxDistance;  ///< أقصى مسافة / Max distance
    Float32 m_sensitivity;  ///< الحساسية / Sensitivity
    Float32 m_followSpeed;  ///< سرعة التتبع / Follow speed
    bool m_collisionEnabled; ///< اصطدام مفعل / Collision enabled
};

// ============================================================================
// كاميرا متابعة ناعمة / Smooth Follow Camera
// ============================================================================

/// كاميرا متابعة ناعمة / Smooth following camera
class SmoothFollowCamera : public Camera3D {
public:
    /// المنشئ الافتراضي / Default constructor
    SmoothFollowCamera()
        : m_target(nullptr)
        , m_offset(0, 3, -8)
        , m_lookOffset(0, 1, 0)
        , m_positionDamping(3.0f)
        , m_rotationDamping(5.0f)
        , m_currentVelocity(Vec3::Zero())
    {}
    
    /// تحديث / Update
    void Update(Float32 deltaTime) {
        if (!m_target) return;
        
        // الموقع المطلوب / Desired position
        Vec3 targetPos = *m_target;
        Vec3 desiredPos = targetPos + m_offset;
        
        // تتبع ناعم بـ spring damping / Smooth follow with spring damping
        Vec3 currentPos = GetPosition();
        
        // Spring damping formula
        Float32 omega = 2.0f * m_positionDamping;
        Float32 zeta = 1.0f; // Critically damped
        Float32 expTerm = std::exp(-omega * zeta * deltaTime);
        
        Vec3 delta = currentPos - desiredPos;
        Vec3 vel = m_currentVelocity;
        
        Vec3 newPos = desiredPos + (delta * (1 + omega * deltaTime) + vel * deltaTime) * expTerm;
        m_currentVelocity = (vel * (1 - omega * deltaTime) - delta * omega * omega * deltaTime) * expTerm;
        
        SetPosition(newPos);
        
        // النظر نحو الهدف بتأخر / Look at target with lag
        Vec3 lookTarget = targetPos + m_lookOffset;
        Vec3 currentLook = GetPosition() + Forward() * 10.0f;
        Vec3 newLook = currentLook.Lerp(lookTarget, 1.0f - std::exp(-m_rotationDamping * deltaTime));
        LookAt(newLook);
    }
    
    /// تعيين الهدف / Set target
    void SetTarget(const Vec3* target) { m_target = target; }
    
    /// الإزاحة / Offset
    const Vec3& GetOffset() const { return m_offset; }
    void SetOffset(const Vec3& offset) { m_offset = offset; }
    
    /// إزاحة النظر / Look offset
    const Vec3& GetLookOffset() const { return m_lookOffset; }
    void SetLookOffset(const Vec3& offset) { m_lookOffset = offset; }
    
    /// تخميد الموقع / Position damping
    Float32 GetPositionDamping() const { return m_positionDamping; }
    void SetPositionDamping(Float32 damping) { m_positionDamping = damping; }
    
    /// تخميد الدوران / Rotation damping
    Float32 GetRotationDamping() const { return m_rotationDamping; }
    void SetRotationDamping(Float32 damping) { m_rotationDamping = damping; }
    
private:
    const Vec3* m_target;       ///< الهدف / Target
    Vec3 m_offset;              ///< الإزاحة / Offset
    Vec3 m_lookOffset;          ///< إزاحة النظر / Look offset
    Float32 m_positionDamping;  ///< تخميد الموقع / Position damping
    Float32 m_rotationDamping;  ///< تخميد الدوران / Rotation damping
    Vec3 m_currentVelocity;     ///< السرعة الحالية / Current velocity
};

// ============================================================================
// مدير الكاميرا / Camera Manager
// ============================================================================

/// مدير للتبديل بين كاميرات متعددة / Manager for switching between cameras
class CameraManager {
public:
    /// المنشئ الافتراضي / Default constructor
    CameraManager() : m_activeCamera(nullptr), m_transitionTime(0.0f), m_transitionDuration(0.0f) {}
    
    /// إضافة كاميرا / Add camera
    void AddCamera(const std::string& name, Camera3D* camera) {
        m_cameras[name] = camera;
        if (!m_activeCamera) {
            m_activeCamera = camera;
        }
    }
    
    /// تعيين الكاميرا النشطة / Set active camera
    void SetActiveCamera(const std::string& name) {
        auto it = m_cameras.find(name);
        if (it != m_cameras.end()) {
            m_activeCamera = it->second;
        }
    }
    
    /// الانتقال إلى كاميرا / Transition to camera
    void TransitionTo(const std::string& name, Float32 duration) {
        auto it = m_cameras.find(name);
        if (it != m_cameras.end() && m_activeCamera) {
            m_previousCamera = m_activeCamera;
            m_activeCamera = it->second;
            m_transitionDuration = duration;
            m_transitionTime = 0.0f;
            
            m_startPosition = m_previousCamera->GetPosition();
            m_startRotation = m_previousCamera->GetRotation();
        }
    }
    
    /// تحديث / Update
    void Update(Float32 deltaTime) {
        if (m_transitionTime < m_transitionDuration && m_previousCamera) {
            m_transitionTime += deltaTime;
            Float32 t = Clamp(m_transitionTime / m_transitionDuration, 0.0f, 1.0f);
            t = SmoothStep(0.0f, 1.0f, t);
            
            Vec3 pos = m_startPosition.Lerp(m_activeCamera->GetPosition(), t);
            Quat rot = m_startRotation.Slerp(m_activeCamera->GetRotation(), t);
            
            m_blendedPosition = pos;
            m_blendedRotation = rot;
        }
    }
    
    /// الحصول على الكاميرا النشطة / Get active camera
    Camera3D* GetActiveCamera() { return m_activeCamera; }
    const Camera3D* GetActiveCamera() const { return m_activeCamera; }
    
    /// هل في حالة انتقال / Is transitioning
    bool IsTransitioning() const { return m_transitionTime < m_transitionDuration; }
    
    /// مصفوفة العرض المدمجة / Blended view matrix
    Mat4 GetViewMatrix() const {
        if (IsTransitioning()) {
            // إنشاء مصفوفة من الموقع والدوران المدمجين / Create matrix from blended pos/rot
            Mat4 rot = m_blendedRotation.ToMatrix().Transposed();
            rot.m[12] = -m_blendedPosition.Dot(Vec3(rot.m[0], rot.m[4], rot.m[8]));
            rot.m[13] = -m_blendedPosition.Dot(Vec3(rot.m[1], rot.m[5], rot.m[9]));
            rot.m[14] = -m_blendedPosition.Dot(Vec3(rot.m[2], rot.m[6], rot.m[10]));
            return rot;
        }
        return m_activeCamera ? m_activeCamera->GetViewMatrix() : Mat4::Identity();
    }
    
private:
    std::unordered_map<std::string, Camera3D*> m_cameras; ///< الكاميرات / Cameras
    Camera3D* m_activeCamera;   ///< الكاميرا النشطة / Active camera
    Camera3D* m_previousCamera = nullptr; ///< الكاميرا السابقة / Previous camera
    
    Float32 m_transitionTime;     ///< وقت الانتقال / Transition time
    Float32 m_transitionDuration; ///< مدة الانتقال / Transition duration
    
    Vec3 m_startPosition;  ///< موقع البداية / Start position
    Quat m_startRotation;  ///< دوران البداية / Start rotation
    Vec3 m_blendedPosition; ///< الموقع المدمج / Blended position
    Quat m_blendedRotation; ///< الدوران المدمج / Blended rotation
};

/// مؤشر ذكي للكاميرا / Smart pointer to Camera3D
using Camera3DPtr = std::shared_ptr<Camera3D>;

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_CAMERA3D_H
