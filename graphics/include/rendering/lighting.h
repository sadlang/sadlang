// ============================================================================
// lighting.h - نظام الإضاءة ثلاثية الأبعاد
// 3D Lighting System
// ============================================================================
// الوصف: يحتوي على أنواع مختلفة من مصادر الضوء والإضاءة
// Description: Contains different types of lights and lighting
// ============================================================================

#ifndef SAD_GRAPHICS_LIGHTING_H
#define SAD_GRAPHICS_LIGHTING_H

#include "../core/types.h"
#include "../core/math3d.h"
#include <vector>
#include <memory>
#include <array>

namespace sad {
namespace graphics {

// ============================================================================
// أنواع الضوء / Light Types
// ============================================================================

/// نوع الضوء / Light type
enum class LightType {
    Directional,  ///< ضوء اتجاهي (مثل الشمس) / Directional (like sun)
    Point,        ///< ضوء نقطي (مثل المصباح) / Point (like lamp)
    Spot,         ///< ضوء بقعي (مثل الكشاف) / Spot (like flashlight)
    Area          ///< ضوء مساحي (للإضاءة الناعمة) / Area (for soft lighting)
};

// ============================================================================
// الضوء الأساسي / Base Light
// ============================================================================

/// فئة الضوء الأساسية / Base light class
class Light {
public:
    /// المنشئ الافتراضي / Default constructor
    Light(LightType type = LightType::Point)
        : m_type(type)
        , m_color(Color::White)
        , m_intensity(1.0f)
        , m_enabled(true)
        , m_castShadows(false)
        , m_shadowBias(0.005f)
        , m_shadowNormalBias(0.02f)
    {}
    
    /// المدمر الافتراضي / Virtual destructor
    virtual ~Light() = default;
    
    // ============================================
    // الخصائص الأساسية / Basic Properties
    // ============================================
    
    /// نوع الضوء / Light type
    LightType GetType() const { return m_type; }
    
    /// اللون / Color
    const Color& GetColor() const { return m_color; }
    void SetColor(const Color& color) { m_color = color; }
    void SetColor(Float32 r, Float32 g, Float32 b) { m_color = Color(r, g, b); }
    
    /// الشدة / Intensity
    Float32 GetIntensity() const { return m_intensity; }
    void SetIntensity(Float32 intensity) { m_intensity = intensity; }
    
    /// مفعل / Enabled
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    /// اللون الفعال / Effective color (color * intensity)
    Vec3 GetEffectiveColor() const {
        return Vec3(m_color.r * m_intensity, m_color.g * m_intensity, m_color.b * m_intensity);
    }
    
    // ============================================
    // الظلال / Shadows
    // ============================================
    
    /// هل يلقي ظلال / Casts shadows
    bool CastsShadows() const { return m_castShadows; }
    void SetCastsShadows(bool casts) { m_castShadows = casts; }
    
    /// انحياز الظل / Shadow bias
    Float32 GetShadowBias() const { return m_shadowBias; }
    void SetShadowBias(Float32 bias) { m_shadowBias = bias; }
    
    /// انحياز العمودي للظل / Shadow normal bias
    Float32 GetShadowNormalBias() const { return m_shadowNormalBias; }
    void SetShadowNormalBias(Float32 bias) { m_shadowNormalBias = bias; }
    
protected:
    LightType m_type;           ///< نوع الضوء / Light type
    Color m_color;              ///< اللون / Color
    Float32 m_intensity;        ///< الشدة / Intensity
    bool m_enabled;             ///< مفعل / Enabled
    bool m_castShadows;         ///< يلقي ظلال / Casts shadows
    Float32 m_shadowBias;       ///< انحياز الظل / Shadow bias
    Float32 m_shadowNormalBias; ///< انحياز العمودي / Normal bias
};

// ============================================================================
// الضوء الاتجاهي / Directional Light
// ============================================================================

/// ضوء اتجاهي (مثل الشمس) / Directional light (like sun)
class DirectionalLight : public Light {
public:
    /// المنشئ الافتراضي / Default constructor
    DirectionalLight()
        : Light(LightType::Directional)
        , m_direction(0, -1, 0)
        , m_shadowCascades(4)
        , m_shadowDistance(100.0f)
    {}
    
    /// منشئ باتجاه / Constructor with direction
    explicit DirectionalLight(const Vec3& direction)
        : Light(LightType::Directional)
        , m_direction(direction.Normalized())
        , m_shadowCascades(4)
        , m_shadowDistance(100.0f)
    {}
    
    // ============================================
    // الاتجاه / Direction
    // ============================================
    
    /// الحصول على الاتجاه / Get direction
    const Vec3& GetDirection() const { return m_direction; }
    
    /// تعيين الاتجاه / Set direction
    void SetDirection(const Vec3& dir) { m_direction = dir.Normalized(); }
    void SetDirection(Float32 x, Float32 y, Float32 z) { SetDirection(Vec3(x, y, z)); }
    
    /// تعيين الاتجاه من زاوية / Set direction from angles
    void SetDirectionFromAngles(Float32 azimuth, Float32 elevation) {
        m_direction = Vec3(
            std::cos(elevation) * std::sin(azimuth),
            -std::sin(elevation),
            std::cos(elevation) * std::cos(azimuth)
        );
    }
    
    // ============================================
    // الظلال المتدرجة / Cascaded Shadows
    // ============================================
    
    /// عدد الشرائح / Number of cascades
    int GetShadowCascades() const { return m_shadowCascades; }
    void SetShadowCascades(int cascades) { m_shadowCascades = Clamp(cascades, 1, 4); }
    
    /// مسافة الظل / Shadow distance
    Float32 GetShadowDistance() const { return m_shadowDistance; }
    void SetShadowDistance(Float32 distance) { m_shadowDistance = distance; }
    
    /// حساب مصفوفة الإسقاط للظل / Calculate shadow projection matrix
    Mat4 GetLightViewMatrix() const {
        Vec3 up = (Abs(m_direction.y) > 0.99f) ? Vec3::UnitX() : Vec3::UnitY();
        Vec3 right = up.Cross(m_direction).Normalized();
        up = m_direction.Cross(right);
        
        Mat4 view = Mat4::Identity();
        view.m[0] = right.x; view.m[4] = right.y; view.m[8] = right.z;
        view.m[1] = up.x;    view.m[5] = up.y;    view.m[9] = up.z;
        view.m[2] = m_direction.x; view.m[6] = m_direction.y; view.m[10] = m_direction.z;
        return view;
    }
    
private:
    Vec3 m_direction;           ///< الاتجاه / Direction
    int m_shadowCascades;       ///< عدد الشرائح / Cascade count
    Float32 m_shadowDistance;   ///< مسافة الظل / Shadow distance
};

// ============================================================================
// الضوء النقطي / Point Light
// ============================================================================

/// ضوء نقطي (مثل المصباح) / Point light (like lamp)
class PointLight : public Light {
public:
    /// المنشئ الافتراضي / Default constructor
    PointLight()
        : Light(LightType::Point)
        , m_position(Vec3::Zero())
        , m_range(10.0f)
        , m_constantAttenuation(1.0f)
        , m_linearAttenuation(0.09f)
        , m_quadraticAttenuation(0.032f)
    {}
    
    /// منشئ بموقع ونطاق / Constructor with position and range
    PointLight(const Vec3& position, Float32 range = 10.0f)
        : Light(LightType::Point)
        , m_position(position)
        , m_range(range)
        , m_constantAttenuation(1.0f)
        , m_linearAttenuation(0.09f)
        , m_quadraticAttenuation(0.032f)
    {}
    
    // ============================================
    // الموقع / Position
    // ============================================
    
    /// الحصول على الموقع / Get position
    const Vec3& GetPosition() const { return m_position; }
    
    /// تعيين الموقع / Set position
    void SetPosition(const Vec3& pos) { m_position = pos; }
    void SetPosition(Float32 x, Float32 y, Float32 z) { m_position = Vec3(x, y, z); }
    
    // ============================================
    // النطاق والتخميد / Range and Attenuation
    // ============================================
    
    /// النطاق / Range
    Float32 GetRange() const { return m_range; }
    void SetRange(Float32 range) { m_range = range; }
    
    /// معاملات التخميد / Attenuation coefficients
    Float32 GetConstantAttenuation() const { return m_constantAttenuation; }
    Float32 GetLinearAttenuation() const { return m_linearAttenuation; }
    Float32 GetQuadraticAttenuation() const { return m_quadraticAttenuation; }
    
    void SetAttenuation(Float32 constant, Float32 linear, Float32 quadratic) {
        m_constantAttenuation = constant;
        m_linearAttenuation = linear;
        m_quadraticAttenuation = quadratic;
    }
    
    /// حساب التخميد / Calculate attenuation
    Float32 CalculateAttenuation(Float32 distance) const {
        return 1.0f / (m_constantAttenuation + m_linearAttenuation * distance +
                       m_quadraticAttenuation * distance * distance);
    }
    
    /// تعيين التخميد من النطاق / Set attenuation from range
    void SetAttenuationFromRange(Float32 range) {
        m_range = range;
        // معادلة تقريبية للتخميد / Approximate attenuation formula
        m_constantAttenuation = 1.0f;
        m_linearAttenuation = 4.5f / range;
        m_quadraticAttenuation = 75.0f / (range * range);
    }
    
    /// الحصول على كرة التأثير / Get influence sphere
    Sphere GetInfluenceSphere() const {
        return Sphere(m_position, m_range);
    }
    
private:
    Vec3 m_position;              ///< الموقع / Position
    Float32 m_range;              ///< النطاق / Range
    Float32 m_constantAttenuation;  ///< التخميد الثابت / Constant attenuation
    Float32 m_linearAttenuation;    ///< التخميد الخطي / Linear attenuation
    Float32 m_quadraticAttenuation; ///< التخميد التربيعي / Quadratic attenuation
};

// ============================================================================
// الضوء البقعي / Spot Light
// ============================================================================

/// ضوء بقعي (مثل الكشاف) / Spot light (like flashlight)
class SpotLight : public Light {
public:
    /// المنشئ الافتراضي / Default constructor
    SpotLight()
        : Light(LightType::Spot)
        , m_position(Vec3::Zero())
        , m_direction(Vec3::Forward())
        , m_range(20.0f)
        , m_innerConeAngle(Radians(25.0f))
        , m_outerConeAngle(Radians(35.0f))
        , m_constantAttenuation(1.0f)
        , m_linearAttenuation(0.09f)
        , m_quadraticAttenuation(0.032f)
    {}
    
    /// منشئ كامل / Full constructor
    SpotLight(const Vec3& position, const Vec3& direction, Float32 range,
              Float32 innerAngleDeg, Float32 outerAngleDeg)
        : Light(LightType::Spot)
        , m_position(position)
        , m_direction(direction.Normalized())
        , m_range(range)
        , m_innerConeAngle(Radians(innerAngleDeg))
        , m_outerConeAngle(Radians(outerAngleDeg))
        , m_constantAttenuation(1.0f)
        , m_linearAttenuation(0.09f)
        , m_quadraticAttenuation(0.032f)
    {}
    
    // ============================================
    // الموقع والاتجاه / Position and Direction
    // ============================================
    
    /// الموقع / Position
    const Vec3& GetPosition() const { return m_position; }
    void SetPosition(const Vec3& pos) { m_position = pos; }
    
    /// الاتجاه / Direction
    const Vec3& GetDirection() const { return m_direction; }
    void SetDirection(const Vec3& dir) { m_direction = dir.Normalized(); }
    
    /// توجيه نحو نقطة / Point at target
    void LookAt(const Vec3& target) {
        m_direction = (target - m_position).Normalized();
    }
    
    // ============================================
    // المخروط / Cone
    // ============================================
    
    /// زاوية المخروط الداخلي / Inner cone angle
    Float32 GetInnerConeAngle() const { return m_innerConeAngle; }
    void SetInnerConeAngle(Float32 radians) { m_innerConeAngle = radians; }
    void SetInnerConeAngleDegrees(Float32 degrees) { m_innerConeAngle = Radians(degrees); }
    
    /// زاوية المخروط الخارجي / Outer cone angle
    Float32 GetOuterConeAngle() const { return m_outerConeAngle; }
    void SetOuterConeAngle(Float32 radians) { m_outerConeAngle = radians; }
    void SetOuterConeAngleDegrees(Float32 degrees) { m_outerConeAngle = Radians(degrees); }
    
    /// تعيين زوايا المخروط / Set cone angles
    void SetConeAngles(Float32 innerDeg, Float32 outerDeg) {
        m_innerConeAngle = Radians(innerDeg);
        m_outerConeAngle = Radians(outerDeg);
    }
    
    /// cos الزوايا (للشيدر) / Cosines (for shader)
    Float32 GetInnerConeCos() const { return std::cos(m_innerConeAngle); }
    Float32 GetOuterConeCos() const { return std::cos(m_outerConeAngle); }
    
    // ============================================
    // النطاق والتخميد / Range and Attenuation
    // ============================================
    
    /// النطاق / Range
    Float32 GetRange() const { return m_range; }
    void SetRange(Float32 range) { m_range = range; }
    
    /// معاملات التخميد / Attenuation
    void SetAttenuation(Float32 constant, Float32 linear, Float32 quadratic) {
        m_constantAttenuation = constant;
        m_linearAttenuation = linear;
        m_quadraticAttenuation = quadratic;
    }
    
    Float32 GetConstantAttenuation() const { return m_constantAttenuation; }
    Float32 GetLinearAttenuation() const { return m_linearAttenuation; }
    Float32 GetQuadraticAttenuation() const { return m_quadraticAttenuation; }
    
    /// حساب التخميد الزاوي / Calculate angular attenuation
    Float32 CalculateSpotAttenuation(const Vec3& pointToLight) const {
        Float32 theta = std::acos(Clamp((-pointToLight).Normalized().Dot(m_direction), -1.0f, 1.0f));
        Float32 epsilon = m_innerConeAngle - m_outerConeAngle;
        return Clamp((theta - m_outerConeAngle) / epsilon, 0.0f, 1.0f);
    }
    
    /// مصفوفة العرض للظلال / View matrix for shadows
    Mat4 GetLightViewMatrix() const {
        return Mat4::LookAt(m_position, m_position + m_direction, Vec3::Up());
    }
    
    /// مصفوفة الإسقاط للظلال / Projection matrix for shadows
    Mat4 GetLightProjectionMatrix() const {
        return Mat4::Perspective(m_outerConeAngle * 2.0f, 1.0f, 0.1f, m_range);
    }
    
private:
    Vec3 m_position;              ///< الموقع / Position
    Vec3 m_direction;             ///< الاتجاه / Direction
    Float32 m_range;              ///< النطاق / Range
    Float32 m_innerConeAngle;     ///< زاوية المخروط الداخلي / Inner cone angle
    Float32 m_outerConeAngle;     ///< زاوية المخروط الخارجي / Outer cone angle
    Float32 m_constantAttenuation;
    Float32 m_linearAttenuation;
    Float32 m_quadraticAttenuation;
};

// ============================================================================
// الضوء المساحي / Area Light
// ============================================================================

/// شكل الضوء المساحي / Area light shape
enum class AreaLightShape {
    Rectangle,  ///< مستطيل / Rectangle
    Disc,       ///< قرص / Disc
    Sphere      ///< كرة / Sphere
};

/// ضوء مساحي (للإضاءة الناعمة) / Area light (for soft lighting)
class AreaLight : public Light {
public:
    /// المنشئ الافتراضي / Default constructor
    AreaLight()
        : Light(LightType::Area)
        , m_position(Vec3::Zero())
        , m_direction(Vec3::Forward())
        , m_shape(AreaLightShape::Rectangle)
        , m_width(1.0f)
        , m_height(1.0f)
        , m_range(10.0f)
        , m_twoSided(false)
    {}
    
    // ============================================
    // الموقع والاتجاه / Position and Direction
    // ============================================
    
    /// الموقع / Position
    const Vec3& GetPosition() const { return m_position; }
    void SetPosition(const Vec3& pos) { m_position = pos; }
    
    /// الاتجاه / Direction
    const Vec3& GetDirection() const { return m_direction; }
    void SetDirection(const Vec3& dir) { m_direction = dir.Normalized(); }
    
    // ============================================
    // الشكل والأبعاد / Shape and Size
    // ============================================
    
    /// الشكل / Shape
    AreaLightShape GetShape() const { return m_shape; }
    void SetShape(AreaLightShape shape) { m_shape = shape; }
    
    /// العرض / Width
    Float32 GetWidth() const { return m_width; }
    void SetWidth(Float32 width) { m_width = width; }
    
    /// الارتفاع / Height
    Float32 GetHeight() const { return m_height; }
    void SetHeight(Float32 height) { m_height = height; }
    
    /// تعيين الأبعاد / Set size
    void SetSize(Float32 width, Float32 height) {
        m_width = width;
        m_height = height;
    }
    
    /// النطاق / Range
    Float32 GetRange() const { return m_range; }
    void SetRange(Float32 range) { m_range = range; }
    
    /// ثنائي الجانب / Two sided
    bool IsTwoSided() const { return m_twoSided; }
    void SetTwoSided(bool twoSided) { m_twoSided = twoSided; }
    
    /// حساب المساحة / Calculate area
    Float32 GetArea() const {
        switch (m_shape) {
            case AreaLightShape::Rectangle: return m_width * m_height;
            case AreaLightShape::Disc: return PI * m_width * m_width * 0.25f;
            case AreaLightShape::Sphere: return 4.0f * PI * m_width * m_width * 0.25f;
            default: return 1.0f;
        }
    }
    
private:
    Vec3 m_position;        ///< الموقع / Position
    Vec3 m_direction;       ///< الاتجاه / Direction
    AreaLightShape m_shape; ///< الشكل / Shape
    Float32 m_width;        ///< العرض / Width
    Float32 m_height;       ///< الارتفاع / Height
    Float32 m_range;        ///< النطاق / Range
    bool m_twoSided;        ///< ثنائي الجانب / Two sided
};

// ============================================================================
// الإضاءة المحيطة / Ambient Lighting
// ============================================================================

/// الإضاءة المحيطة / Ambient lighting settings
struct AmbientLighting {
    Color skyColor;       ///< لون السماء / Sky color (from above)
    Color equatorColor;   ///< لون خط الاستواء / Equator color
    Color groundColor;    ///< لون الأرض / Ground color (from below)
    Float32 intensity;    ///< الشدة / Intensity
    
    /// المنشئ الافتراضي / Default constructor
    AmbientLighting()
        : skyColor(0.2f, 0.25f, 0.3f)
        , equatorColor(0.15f, 0.15f, 0.15f)
        , groundColor(0.1f, 0.08f, 0.06f)
        , intensity(1.0f)
    {}
    
    /// إضاءة محيطة ثابتة / Constant ambient
    static AmbientLighting Constant(const Color& color, Float32 intensity = 1.0f) {
        AmbientLighting amb;
        amb.skyColor = color;
        amb.equatorColor = color;
        amb.groundColor = color;
        amb.intensity = intensity;
        return amb;
    }
    
    /// إضاءة محيطة تدرجية / Gradient ambient
    static AmbientLighting Gradient(const Color& sky, const Color& ground) {
        AmbientLighting amb;
        amb.skyColor = sky;
        amb.groundColor = ground;
        amb.equatorColor = Color(
            (sky.r + ground.r) * 0.5f,
            (sky.g + ground.g) * 0.5f,
            (sky.b + ground.b) * 0.5f
        );
        return amb;
    }
    
    /// حساب اللون عند اتجاه معين / Calculate color at direction
    Vec3 GetColorAtDirection(const Vec3& normal) const {
        Float32 y = normal.y * 0.5f + 0.5f; // Map from [-1,1] to [0,1]
        
        Vec3 sky(skyColor.r, skyColor.g, skyColor.b);
        Vec3 equator(equatorColor.r, equatorColor.g, equatorColor.b);
        Vec3 ground(groundColor.r, groundColor.g, groundColor.b);
        
        Vec3 color;
        if (y > 0.5f) {
            Float32 t = (y - 0.5f) * 2.0f;
            color = equator.Lerp(sky, t);
        } else {
            Float32 t = y * 2.0f;
            color = ground.Lerp(equator, t);
        }
        
        return color * intensity;
    }
};

// ============================================================================
// الضباب / Fog
// ============================================================================

/// نوع الضباب / Fog type
enum class FogType {
    None,        ///< بدون ضباب / No fog
    Linear,      ///< ضباب خطي / Linear fog
    Exponential, ///< ضباب أسي / Exponential fog
    ExponentialSquared, ///< ضباب أسي مربع / Exponential squared fog
    Height       ///< ضباب ارتفاعي / Height-based fog
};

/// إعدادات الضباب / Fog settings
struct FogSettings {
    FogType type;       ///< نوع الضباب / Fog type
    Color color;        ///< لون الضباب / Fog color
    Float32 density;    ///< كثافة الضباب / Fog density
    Float32 startDist;  ///< بداية الضباب (للخطي) / Start distance (for linear)
    Float32 endDist;    ///< نهاية الضباب (للخطي) / End distance (for linear)
    Float32 heightStart; ///< ارتفاع البداية / Height start
    Float32 heightEnd;  ///< ارتفاع النهاية / Height end
    
    /// المنشئ الافتراضي / Default constructor
    FogSettings()
        : type(FogType::None)
        , color(0.5f, 0.5f, 0.5f)
        , density(0.01f)
        , startDist(10.0f)
        , endDist(100.0f)
        , heightStart(0.0f)
        , heightEnd(50.0f)
    {}
    
    /// حساب كثافة الضباب / Calculate fog factor
    Float32 CalculateFogFactor(Float32 distance, Float32 height = 0.0f) const {
        switch (type) {
            case FogType::None:
                return 0.0f;
            
            case FogType::Linear:
                return Clamp((endDist - distance) / (endDist - startDist), 0.0f, 1.0f);
            
            case FogType::Exponential:
                return 1.0f - std::exp(-density * distance);
            
            case FogType::ExponentialSquared:
                return 1.0f - std::exp(-density * density * distance * distance);
            
            case FogType::Height: {
                Float32 distFog = 1.0f - std::exp(-density * distance);
                Float32 heightFog = Clamp((heightEnd - height) / (heightEnd - heightStart), 0.0f, 1.0f);
                return distFog * heightFog;
            }
            
            default:
                return 0.0f;
        }
    }
    
    /// ضباب خطي / Linear fog preset
    static FogSettings Linear(Float32 start, Float32 end, const Color& color = Color(0.5f, 0.5f, 0.5f)) {
        FogSettings f;
        f.type = FogType::Linear;
        f.startDist = start;
        f.endDist = end;
        f.color = color;
        return f;
    }
    
    /// ضباب أسي / Exponential fog preset
    static FogSettings Exponential(Float32 density, const Color& color = Color(0.5f, 0.5f, 0.5f)) {
        FogSettings f;
        f.type = FogType::Exponential;
        f.density = density;
        f.color = color;
        return f;
    }
};

// ============================================================================
// بيئة الإضاءة / Light Environment
// ============================================================================

/// بيئة الإضاءة الكاملة / Complete lighting environment
class LightEnvironment {
public:
    /// الحد الأقصى للأضواء / Maximum lights
    static constexpr int MAX_DIRECTIONAL_LIGHTS = 2;
    static constexpr int MAX_POINT_LIGHTS = 64;
    static constexpr int MAX_SPOT_LIGHTS = 32;
    
    /// المنشئ الافتراضي / Default constructor
    LightEnvironment() {}
    
    // ============================================
    // الإضاءة المحيطة / Ambient Lighting
    // ============================================
    
    /// الحصول على الإضاءة المحيطة / Get ambient lighting
    const AmbientLighting& GetAmbient() const { return m_ambient; }
    AmbientLighting& GetAmbient() { return m_ambient; }
    
    /// تعيين الإضاءة المحيطة / Set ambient lighting
    void SetAmbient(const AmbientLighting& ambient) { m_ambient = ambient; }
    
    // ============================================
    // الضباب / Fog
    // ============================================
    
    /// الحصول على الضباب / Get fog
    const FogSettings& GetFog() const { return m_fog; }
    FogSettings& GetFog() { return m_fog; }
    
    /// تعيين الضباب / Set fog
    void SetFog(const FogSettings& fog) { m_fog = fog; }
    
    // ============================================
    // الأضواء الاتجاهية / Directional Lights
    // ============================================
    
    /// إضافة ضوء اتجاهي / Add directional light
    DirectionalLight* AddDirectionalLight() {
        if (m_directionalLights.size() < MAX_DIRECTIONAL_LIGHTS) {
            m_directionalLights.push_back(std::make_unique<DirectionalLight>());
            return m_directionalLights.back().get();
        }
        return nullptr;
    }
    
    /// الحصول على الأضواء الاتجاهية / Get directional lights
    const std::vector<std::unique_ptr<DirectionalLight>>& GetDirectionalLights() const {
        return m_directionalLights;
    }
    
    /// الحصول على الضوء الاتجاهي الرئيسي / Get main directional light
    DirectionalLight* GetMainDirectionalLight() {
        return m_directionalLights.empty() ? nullptr : m_directionalLights[0].get();
    }
    
    // ============================================
    // الأضواء النقطية / Point Lights
    // ============================================
    
    /// إضافة ضوء نقطي / Add point light
    PointLight* AddPointLight() {
        if (m_pointLights.size() < MAX_POINT_LIGHTS) {
            m_pointLights.push_back(std::make_unique<PointLight>());
            return m_pointLights.back().get();
        }
        return nullptr;
    }
    
    /// الحصول على الأضواء النقطية / Get point lights
    const std::vector<std::unique_ptr<PointLight>>& GetPointLights() const {
        return m_pointLights;
    }
    
    // ============================================
    // الأضواء البقعية / Spot Lights
    // ============================================
    
    /// إضافة ضوء بقعي / Add spot light
    SpotLight* AddSpotLight() {
        if (m_spotLights.size() < MAX_SPOT_LIGHTS) {
            m_spotLights.push_back(std::make_unique<SpotLight>());
            return m_spotLights.back().get();
        }
        return nullptr;
    }
    
    /// الحصول على الأضواء البقعية / Get spot lights
    const std::vector<std::unique_ptr<SpotLight>>& GetSpotLights() const {
        return m_spotLights;
    }
    
    // ============================================
    // عمليات عامة / General Operations
    // ============================================
    
    /// إزالة جميع الأضواء / Remove all lights
    void Clear() {
        m_directionalLights.clear();
        m_pointLights.clear();
        m_spotLights.clear();
    }
    
    /// العدد الإجمالي للأضواء / Total light count
    size_t GetLightCount() const {
        return m_directionalLights.size() + m_pointLights.size() + m_spotLights.size();
    }
    
    /// جمع الأضواء المؤثرة على نقطة / Cull lights affecting a point
    std::vector<PointLight*> GetPointLightsAffecting(const Vec3& position, Float32 radius) const {
        std::vector<PointLight*> result;
        Sphere bounds(position, radius);
        
        for (const auto& light : m_pointLights) {
            if (light->IsEnabled() && light->GetInfluenceSphere().Intersects(bounds)) {
                result.push_back(light.get());
            }
        }
        
        return result;
    }
    
    /// جمع الأضواء المؤثرة في صندوق / Get lights in bounds
    std::vector<Light*> GetLightsInBounds(const AABB& bounds) const {
        std::vector<Light*> result;
        
        // الأضواء الاتجاهية تؤثر دائماً / Directional lights always affect
        for (const auto& light : m_directionalLights) {
            if (light->IsEnabled()) {
                result.push_back(light.get());
            }
        }
        
        // الأضواء النقطية / Point lights
        for (const auto& light : m_pointLights) {
            if (light->IsEnabled()) {
                if (bounds.Intersects(light->GetInfluenceSphere())) {
                    result.push_back(light.get());
                }
            }
        }
        
        // الأضواء البقعية / Spot lights
        for (const auto& light : m_spotLights) {
            if (light->IsEnabled()) {
                // Approximate with sphere for now
                Sphere influence(light->GetPosition(), light->GetRange());
                if (bounds.Intersects(influence)) {
                    result.push_back(light.get());
                }
            }
        }
        
        return result;
    }
    
private:
    AmbientLighting m_ambient;  ///< الإضاءة المحيطة / Ambient lighting
    FogSettings m_fog;          ///< الضباب / Fog
    
    std::vector<std::unique_ptr<DirectionalLight>> m_directionalLights; ///< الأضواء الاتجاهية
    std::vector<std::unique_ptr<PointLight>> m_pointLights;             ///< الأضواء النقطية
    std::vector<std::unique_ptr<SpotLight>> m_spotLights;               ///< الأضواء البقعية
};

// ============================================================================
// إعدادات مسبقة للإضاءة / Lighting Presets
// ============================================================================

/// إعدادات إضاءة مسبقة / Lighting presets
struct LightingPresets {
    /// إضاءة نهارية / Daylight
    static void ApplyDaylight(LightEnvironment& env) {
        env.Clear();
        
        // الشمس / Sun
        auto sun = env.AddDirectionalLight();
        sun->SetDirection(Vec3(-0.5f, -1.0f, -0.3f));
        sun->SetColor(Color(1.0f, 0.95f, 0.85f));
        sun->SetIntensity(1.2f);
        sun->SetCastsShadows(true);
        
        // السماء / Ambient
        env.GetAmbient() = AmbientLighting::Gradient(
            Color(0.4f, 0.5f, 0.7f),  // Sky
            Color(0.2f, 0.15f, 0.1f)  // Ground
        );
    }
    
    /// إضاءة غروب / Sunset
    static void ApplySunset(LightEnvironment& env) {
        env.Clear();
        
        auto sun = env.AddDirectionalLight();
        sun->SetDirection(Vec3(-1.0f, -0.3f, -0.2f));
        sun->SetColor(Color(1.0f, 0.6f, 0.3f));
        sun->SetIntensity(1.0f);
        sun->SetCastsShadows(true);
        
        env.GetAmbient() = AmbientLighting::Gradient(
            Color(0.6f, 0.4f, 0.3f),
            Color(0.15f, 0.1f, 0.1f)
        );
    }
    
    /// إضاءة ليلية / Night
    static void ApplyNight(LightEnvironment& env) {
        env.Clear();
        
        auto moon = env.AddDirectionalLight();
        moon->SetDirection(Vec3(0.3f, -1.0f, 0.2f));
        moon->SetColor(Color(0.3f, 0.35f, 0.5f));
        moon->SetIntensity(0.3f);
        moon->SetCastsShadows(true);
        
        env.GetAmbient() = AmbientLighting::Gradient(
            Color(0.05f, 0.05f, 0.1f),
            Color(0.02f, 0.02f, 0.03f)
        );
    }
    
    /// إضاءة استوديو / Studio
    static void ApplyStudio(LightEnvironment& env) {
        env.Clear();
        
        // Key light
        auto key = env.AddDirectionalLight();
        key->SetDirection(Vec3(-1.0f, -1.0f, 0.5f));
        key->SetColor(Color::White);
        key->SetIntensity(1.0f);
        
        // Fill light
        auto fill = env.AddDirectionalLight();
        fill->SetDirection(Vec3(1.0f, -0.5f, 0.5f));
        fill->SetColor(Color(0.8f, 0.85f, 0.9f));
        fill->SetIntensity(0.5f);
        
        env.GetAmbient() = AmbientLighting::Constant(Color(0.2f, 0.2f, 0.2f));
    }
    
    /// إضاءة داخلية / Indoor
    static void ApplyIndoor(LightEnvironment& env) {
        env.Clear();
        
        env.GetAmbient() = AmbientLighting::Constant(Color(0.15f, 0.15f, 0.15f));
        
        // إضافة بعض الأضواء النقطية / Add some point lights
        auto light1 = env.AddPointLight();
        light1->SetPosition(Vec3(0, 3, 0));
        light1->SetColor(Color(1.0f, 0.95f, 0.85f));
        light1->SetRange(10.0f);
    }
};

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_LIGHTING_H
