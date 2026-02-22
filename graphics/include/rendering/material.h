// ============================================================================
// material.h - نظام المواد والشيدرات
// Materials and Shaders System
// ============================================================================
// الوصف: يحتوي على فئات المواد والخصائص السطحية للرسم ثلاثي الأبعاد
// Description: Contains material classes and surface properties for 3D rendering
// ============================================================================

#ifndef SAD_GRAPHICS_MATERIAL_H
#define SAD_GRAPHICS_MATERIAL_H

#include "../core/types.h"
#include "../core/math3d.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>

namespace sad {
namespace graphics {

// Forward declarations
class Texture;
using TexturePtr = std::shared_ptr<Texture>;

// ============================================================================
// أنواع خصائص المادة / Material Property Types
// ============================================================================

/// قيمة خاصية المادة / Material property value
using MaterialPropertyValue = std::variant<
    Float32,
    Vec2,
    Vec3,
    Vec4,
    Color,
    Mat4,
    TexturePtr,
    int
>;

/// نوع الخاصية / Property type enum
enum class MaterialPropertyType {
    Float,
    Vec2,
    Vec3,
    Vec4,
    Color,
    Matrix,
    Texture,
    Int
};

// ============================================================================
// وضع المزج / Blend Mode
// ============================================================================

/// وضع المزج / Blend mode
enum class BlendMode {
    Opaque,         ///< معتم (بدون مزج) / Opaque (no blending)
    Transparent,    ///< شفاف (ألفا) / Transparent (alpha)
    Additive,       ///< إضافي / Additive
    Multiply,       ///< ضرب / Multiply
    Premultiplied,  ///< ألفا مضروب مسبقاً / Premultiplied alpha
    Custom          ///< مخصص / Custom
};

/// عوامل المزج / Blend factors
enum class BlendFactor {
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha
};

/// وظيفة المزج / Blend function
enum class BlendOp {
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max
};

/// حالة المزج الكاملة / Full blend state
struct BlendState {
    bool enabled;
    BlendFactor srcColor;
    BlendFactor dstColor;
    BlendOp colorOp;
    BlendFactor srcAlpha;
    BlendFactor dstAlpha;
    BlendOp alphaOp;
    
    BlendState()
        : enabled(false)
        , srcColor(BlendFactor::One)
        , dstColor(BlendFactor::Zero)
        , colorOp(BlendOp::Add)
        , srcAlpha(BlendFactor::One)
        , dstAlpha(BlendFactor::Zero)
        , alphaOp(BlendOp::Add)
    {}
    
    /// معتم / Opaque
    static BlendState Opaque() {
        return BlendState();
    }
    
    /// شفاف (ألفا) / Alpha blend
    static BlendState Alpha() {
        BlendState s;
        s.enabled = true;
        s.srcColor = BlendFactor::SrcAlpha;
        s.dstColor = BlendFactor::OneMinusSrcAlpha;
        s.srcAlpha = BlendFactor::One;
        s.dstAlpha = BlendFactor::OneMinusSrcAlpha;
        return s;
    }
    
    /// إضافي / Additive
    static BlendState Additive() {
        BlendState s;
        s.enabled = true;
        s.srcColor = BlendFactor::SrcAlpha;
        s.dstColor = BlendFactor::One;
        s.srcAlpha = BlendFactor::One;
        s.dstAlpha = BlendFactor::One;
        return s;
    }
    
    /// ألفا مضروب مسبقاً / Premultiplied alpha
    static BlendState Premultiplied() {
        BlendState s;
        s.enabled = true;
        s.srcColor = BlendFactor::One;
        s.dstColor = BlendFactor::OneMinusSrcAlpha;
        s.srcAlpha = BlendFactor::One;
        s.dstAlpha = BlendFactor::OneMinusSrcAlpha;
        return s;
    }
};

// ============================================================================
// وضع الوجه المقطوع / Cull Mode
// ============================================================================

/// وضع قطع الوجه / Cull mode
enum class CullMode {
    None,   ///< بدون قطع (ثنائي الجانب) / No culling (two-sided)
    Front,  ///< قطع الوجه الأمامي / Cull front faces
    Back    ///< قطع الوجه الخلفي / Cull back faces
};

// ============================================================================
// وضع العمق / Depth Mode
// ============================================================================

/// وظيفة مقارنة العمق / Depth comparison function
enum class DepthFunc {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always
};

/// حالة العمق / Depth state
struct DepthState {
    bool testEnabled;    ///< اختبار العمق مفعل / Depth test enabled
    bool writeEnabled;   ///< كتابة العمق مفعلة / Depth write enabled
    DepthFunc func;      ///< وظيفة المقارنة / Comparison function
    
    DepthState()
        : testEnabled(true)
        , writeEnabled(true)
        , func(DepthFunc::Less)
    {}
    
    /// الافتراضي / Default (test and write)
    static DepthState Default() { return DepthState(); }
    
    /// قراءة فقط / Read only
    static DepthState ReadOnly() {
        DepthState s;
        s.writeEnabled = false;
        return s;
    }
    
    /// معطل / Disabled
    static DepthState Disabled() {
        DepthState s;
        s.testEnabled = false;
        s.writeEnabled = false;
        return s;
    }
};

// ============================================================================
// حالة الاستنسل / Stencil State
// ============================================================================

/// عملية الاستنسل / Stencil operation
enum class StencilOp {
    Keep,
    Zero,
    Replace,
    IncrementSaturate,
    DecrementSaturate,
    Invert,
    IncrementWrap,
    DecrementWrap
};

/// وظيفة الاستنسل / Stencil function
enum class StencilFunc {
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always
};

/// حالة الاستنسل / Stencil state
struct StencilState {
    bool enabled;
    StencilFunc func;
    UInt32 reference;
    UInt32 readMask;
    UInt32 writeMask;
    StencilOp stencilFail;
    StencilOp depthFail;
    StencilOp pass;
    
    StencilState()
        : enabled(false)
        , func(StencilFunc::Always)
        , reference(0)
        , readMask(0xFF)
        , writeMask(0xFF)
        , stencilFail(StencilOp::Keep)
        , depthFail(StencilOp::Keep)
        , pass(StencilOp::Keep)
    {}
};

// ============================================================================
// حالة الرسم / Render State
// ============================================================================

/// حالة الرسم الكاملة / Complete render state
struct RenderState {
    BlendState blend;       ///< حالة المزج / Blend state
    DepthState depth;       ///< حالة العمق / Depth state
    StencilState stencil;   ///< حالة الاستنسل / Stencil state
    CullMode cullMode;      ///< وضع القطع / Cull mode
    bool wireframe;         ///< وضع الأسلاك / Wireframe mode
    bool scissorTest;       ///< اختبار المقص / Scissor test
    
    RenderState()
        : cullMode(CullMode::Back)
        , wireframe(false)
        , scissorTest(false)
    {}
};

// ============================================================================
// خاصية المادة / Material Property
// ============================================================================

/// خاصية مادة مفردة / Single material property
struct MaterialProperty {
    std::string name;                ///< الاسم / Name
    MaterialPropertyType type;       ///< النوع / Type
    MaterialPropertyValue value;     ///< القيمة / Value
    int location;                    ///< موقع في الشيدر / Shader location
    
    MaterialProperty() : type(MaterialPropertyType::Float), location(-1) {}
    
    MaterialProperty(const std::string& n, Float32 v)
        : name(n), type(MaterialPropertyType::Float), value(v), location(-1) {}
    
    MaterialProperty(const std::string& n, const Vec2& v)
        : name(n), type(MaterialPropertyType::Vec2), value(v), location(-1) {}
    
    MaterialProperty(const std::string& n, const Vec3& v)
        : name(n), type(MaterialPropertyType::Vec3), value(v), location(-1) {}
    
    MaterialProperty(const std::string& n, const Vec4& v)
        : name(n), type(MaterialPropertyType::Vec4), value(v), location(-1) {}
    
    MaterialProperty(const std::string& n, const Color& v)
        : name(n), type(MaterialPropertyType::Color), value(v), location(-1) {}
    
    MaterialProperty(const std::string& n, const TexturePtr& v)
        : name(n), type(MaterialPropertyType::Texture), value(v), location(-1) {}
};

// ============================================================================
// المادة الأساسية / Base Material
// ============================================================================

/// فئة المادة الأساسية / Base material class
class Material {
public:
    /// المنشئ الافتراضي / Default constructor
    Material() : m_gpuHandle(0), m_dirty(true) {}
    
    /// المدمر الافتراضي / Virtual destructor
    virtual ~Material() = default;
    
    // ============================================
    // الاسم / Name
    // ============================================
    
    /// الحصول على الاسم / Get name
    const std::string& GetName() const { return m_name; }
    
    /// تعيين الاسم / Set name
    void SetName(const std::string& name) { m_name = name; }
    
    // ============================================
    // حالة الرسم / Render State
    // ============================================
    
    /// الحصول على حالة الرسم / Get render state
    const RenderState& GetRenderState() const { return m_renderState; }
    RenderState& GetRenderState() { return m_renderState; }
    
    /// تعيين حالة الرسم / Set render state
    void SetRenderState(const RenderState& state) { m_renderState = state; }
    
    // ============================================
    // الخصائص / Properties
    // ============================================
    
    /// تعيين قيمة عائمة / Set float value
    void SetFloat(const std::string& name, Float32 value) {
        m_properties[name] = MaterialProperty(name, value);
        m_dirty = true;
    }
    
    /// الحصول على قيمة عائمة / Get float value
    Float32 GetFloat(const std::string& name, Float32 defaultValue = 0.0f) const {
        auto it = m_properties.find(name);
        if (it != m_properties.end() && it->second.type == MaterialPropertyType::Float) {
            return std::get<Float32>(it->second.value);
        }
        return defaultValue;
    }
    
    /// تعيين Vec2 / Set Vec2
    void SetVec2(const std::string& name, const Vec2& value) {
        m_properties[name] = MaterialProperty(name, value);
        m_dirty = true;
    }
    
    /// تعيين Vec3 / Set Vec3
    void SetVec3(const std::string& name, const Vec3& value) {
        m_properties[name] = MaterialProperty(name, value);
        m_dirty = true;
    }
    
    /// تعيين Vec4 / Set Vec4
    void SetVec4(const std::string& name, const Vec4& value) {
        m_properties[name] = MaterialProperty(name, value);
        m_dirty = true;
    }
    
    /// تعيين اللون / Set color
    void SetColor(const std::string& name, const Color& value) {
        m_properties[name] = MaterialProperty(name, value);
        m_dirty = true;
    }
    
    /// الحصول على اللون / Get color
    Color GetColor(const std::string& name, const Color& defaultValue = Color::White) const {
        auto it = m_properties.find(name);
        if (it != m_properties.end() && it->second.type == MaterialPropertyType::Color) {
            return std::get<Color>(it->second.value);
        }
        return defaultValue;
    }
    
    /// تعيين النسيج / Set texture
    void SetTexture(const std::string& name, const TexturePtr& texture) {
        m_properties[name] = MaterialProperty(name, texture);
        m_dirty = true;
    }
    
    /// الحصول على النسيج / Get texture
    TexturePtr GetTexture(const std::string& name) const {
        auto it = m_properties.find(name);
        if (it != m_properties.end() && it->second.type == MaterialPropertyType::Texture) {
            return std::get<TexturePtr>(it->second.value);
        }
        return nullptr;
    }
    
    /// هل توجد الخاصية / Has property
    bool HasProperty(const std::string& name) const {
        return m_properties.find(name) != m_properties.end();
    }
    
    /// إزالة خاصية / Remove property
    void RemoveProperty(const std::string& name) {
        m_properties.erase(name);
        m_dirty = true;
    }
    
    /// الحصول على جميع الخصائص / Get all properties
    const std::unordered_map<std::string, MaterialProperty>& GetProperties() const {
        return m_properties;
    }
    
    // ============================================
    // حالة المادة / Material State
    // ============================================
    
    /// هل تم التعديل / Is dirty
    bool IsDirty() const { return m_dirty; }
    
    /// تعيين الحالة / Set dirty
    void SetDirty(bool dirty = true) { m_dirty = dirty; }
    
    /// مؤشر GPU / GPU handle
    Handle GetGPUHandle() const { return m_gpuHandle; }
    void SetGPUHandle(Handle handle) { m_gpuHandle = handle; }
    
protected:
    std::string m_name;              ///< الاسم / Name
    RenderState m_renderState;       ///< حالة الرسم / Render state
    std::unordered_map<std::string, MaterialProperty> m_properties; ///< الخصائص / Properties
    Handle m_gpuHandle;              ///< مؤشر GPU / GPU handle
    bool m_dirty;                    ///< علامة التعديل / Dirty flag
};

// ============================================================================
// مادة PBR القياسية / Standard PBR Material
// ============================================================================

/// مادة PBR قياسية (Physically Based Rendering) / Standard PBR Material
class PBRMaterial : public Material {
public:
    /// المنشئ الافتراضي / Default constructor
    PBRMaterial() {
        // القيم الافتراضية / Default values
        SetColor("albedo", Color::White);
        SetFloat("metallic", 0.0f);
        SetFloat("roughness", 0.5f);
        SetFloat("ao", 1.0f);
        SetVec3("emissive", Vec3::Zero());
        SetFloat("emissiveIntensity", 1.0f);
        SetFloat("normalStrength", 1.0f);
        SetVec2("tiling", Vec2::One());
        SetVec2("offset", Vec2::Zero());
    }
    
    // ============================================
    // خصائص PBR الأساسية / Basic PBR Properties
    // ============================================
    
    /// لون الألبيدو / Albedo color
    Color GetAlbedo() const { return GetColor("albedo"); }
    void SetAlbedo(const Color& color) { SetColor("albedo", color); }
    
    /// خريطة الألبيدو / Albedo map
    TexturePtr GetAlbedoMap() const { return GetTexture("albedoMap"); }
    void SetAlbedoMap(const TexturePtr& tex) { SetTexture("albedoMap", tex); }
    
    /// المعدنية / Metallic
    Float32 GetMetallic() const { return GetFloat("metallic"); }
    void SetMetallic(Float32 value) { SetFloat("metallic", Clamp(value, 0.0f, 1.0f)); }
    
    /// خريطة المعدنية / Metallic map
    TexturePtr GetMetallicMap() const { return GetTexture("metallicMap"); }
    void SetMetallicMap(const TexturePtr& tex) { SetTexture("metallicMap", tex); }
    
    /// الخشونة / Roughness
    Float32 GetRoughness() const { return GetFloat("roughness"); }
    void SetRoughness(Float32 value) { SetFloat("roughness", Clamp(value, 0.0f, 1.0f)); }
    
    /// خريطة الخشونة / Roughness map
    TexturePtr GetRoughnessMap() const { return GetTexture("roughnessMap"); }
    void SetRoughnessMap(const TexturePtr& tex) { SetTexture("roughnessMap", tex); }
    
    /// الانسداد المحيط / Ambient occlusion
    Float32 GetAO() const { return GetFloat("ao"); }
    void SetAO(Float32 value) { SetFloat("ao", Clamp(value, 0.0f, 1.0f)); }
    
    /// خريطة الانسداد المحيط / AO map
    TexturePtr GetAOMap() const { return GetTexture("aoMap"); }
    void SetAOMap(const TexturePtr& tex) { SetTexture("aoMap", tex); }
    
    // ============================================
    // الانبعاث / Emission
    // ============================================
    
    /// لون الانبعاث / Emissive color
    Vec3 GetEmissive() const {
        auto it = m_properties.find("emissive");
        if (it != m_properties.end()) {
            return std::get<Vec3>(it->second.value);
        }
        return Vec3::Zero();
    }
    void SetEmissive(const Vec3& color) { SetVec3("emissive", color); }
    
    /// شدة الانبعاث / Emissive intensity
    Float32 GetEmissiveIntensity() const { return GetFloat("emissiveIntensity", 1.0f); }
    void SetEmissiveIntensity(Float32 intensity) { SetFloat("emissiveIntensity", intensity); }
    
    /// خريطة الانبعاث / Emissive map
    TexturePtr GetEmissiveMap() const { return GetTexture("emissiveMap"); }
    void SetEmissiveMap(const TexturePtr& tex) { SetTexture("emissiveMap", tex); }
    
    // ============================================
    // العموديات / Normals
    // ============================================
    
    /// خريطة العموديات / Normal map
    TexturePtr GetNormalMap() const { return GetTexture("normalMap"); }
    void SetNormalMap(const TexturePtr& tex) { SetTexture("normalMap", tex); }
    
    /// قوة العموديات / Normal strength
    Float32 GetNormalStrength() const { return GetFloat("normalStrength", 1.0f); }
    void SetNormalStrength(Float32 strength) { SetFloat("normalStrength", strength); }
    
    // ============================================
    // الإزاحة / Height/Parallax
    // ============================================
    
    /// خريطة الارتفاع / Height map
    TexturePtr GetHeightMap() const { return GetTexture("heightMap"); }
    void SetHeightMap(const TexturePtr& tex) { SetTexture("heightMap", tex); }
    
    /// مقياس الارتفاع / Height scale
    Float32 GetHeightScale() const { return GetFloat("heightScale", 0.05f); }
    void SetHeightScale(Float32 scale) { SetFloat("heightScale", scale); }
    
    // ============================================
    // التكرار والإزاحة / Tiling and Offset
    // ============================================
    
    /// تكرار النسيج / Texture tiling
    Vec2 GetTiling() const {
        auto it = m_properties.find("tiling");
        if (it != m_properties.end()) {
            return std::get<Vec2>(it->second.value);
        }
        return Vec2::One();
    }
    void SetTiling(const Vec2& tiling) { SetVec2("tiling", tiling); }
    void SetTiling(Float32 x, Float32 y) { SetTiling(Vec2(x, y)); }
    
    /// إزاحة النسيج / Texture offset
    Vec2 GetOffset() const {
        auto it = m_properties.find("offset");
        if (it != m_properties.end()) {
            return std::get<Vec2>(it->second.value);
        }
        return Vec2::Zero();
    }
    void SetOffset(const Vec2& offset) { SetVec2("offset", offset); }
    void SetOffset(Float32 x, Float32 y) { SetOffset(Vec2(x, y)); }
    
    // ============================================
    // إعدادات مسبقة / Presets
    // ============================================
    
    /// معدن / Metal preset
    static std::shared_ptr<PBRMaterial> Metal(const Color& color = Color(0.9f, 0.9f, 0.9f)) {
        auto mat = std::make_shared<PBRMaterial>();
        mat->SetAlbedo(color);
        mat->SetMetallic(1.0f);
        mat->SetRoughness(0.3f);
        return mat;
    }
    
    /// بلاستيك / Plastic preset
    static std::shared_ptr<PBRMaterial> Plastic(const Color& color = Color::White) {
        auto mat = std::make_shared<PBRMaterial>();
        mat->SetAlbedo(color);
        mat->SetMetallic(0.0f);
        mat->SetRoughness(0.4f);
        return mat;
    }
    
    /// خشب / Wood preset
    static std::shared_ptr<PBRMaterial> Wood() {
        auto mat = std::make_shared<PBRMaterial>();
        mat->SetAlbedo(Color(0.6f, 0.4f, 0.2f));
        mat->SetMetallic(0.0f);
        mat->SetRoughness(0.7f);
        return mat;
    }
    
    /// حجر / Stone preset
    static std::shared_ptr<PBRMaterial> Stone() {
        auto mat = std::make_shared<PBRMaterial>();
        mat->SetAlbedo(Color(0.5f, 0.5f, 0.5f));
        mat->SetMetallic(0.0f);
        mat->SetRoughness(0.8f);
        return mat;
    }
    
    /// زجاج / Glass preset
    static std::shared_ptr<PBRMaterial> Glass() {
        auto mat = std::make_shared<PBRMaterial>();
        mat->SetAlbedo(Color(0.95f, 0.95f, 0.95f, 0.3f));
        mat->SetMetallic(0.0f);
        mat->SetRoughness(0.1f);
        mat->GetRenderState().blend = BlendState::Alpha();
        mat->GetRenderState().cullMode = CullMode::None;
        return mat;
    }
    
    /// ذهب / Gold preset
    static std::shared_ptr<PBRMaterial> Gold() {
        auto mat = std::make_shared<PBRMaterial>();
        mat->SetAlbedo(Color(1.0f, 0.85f, 0.4f));
        mat->SetMetallic(1.0f);
        mat->SetRoughness(0.3f);
        return mat;
    }
    
    /// فضة / Silver preset
    static std::shared_ptr<PBRMaterial> Silver() {
        auto mat = std::make_shared<PBRMaterial>();
        mat->SetAlbedo(Color(0.95f, 0.95f, 0.95f));
        mat->SetMetallic(1.0f);
        mat->SetRoughness(0.25f);
        return mat;
    }
};

// ============================================================================
// مادة Unlit / Unlit Material
// ============================================================================

/// مادة بدون إضاءة / Unlit material (no lighting)
class UnlitMaterial : public Material {
public:
    /// المنشئ الافتراضي / Default constructor
    UnlitMaterial() {
        SetColor("color", Color::White);
        SetVec2("tiling", Vec2::One());
        SetVec2("offset", Vec2::Zero());
    }
    
    /// منشئ بلون / Constructor with color
    explicit UnlitMaterial(const Color& color) : UnlitMaterial() {
        SetMainColor(color);
    }
    
    /// اللون الرئيسي / Main color
    Color GetMainColor() const { return GetColor("color"); }
    void SetMainColor(const Color& color) { SetColor("color", color); }
    
    /// النسيج الرئيسي / Main texture
    TexturePtr GetMainTexture() const { return GetTexture("mainTexture"); }
    void SetMainTexture(const TexturePtr& tex) { SetTexture("mainTexture", tex); }
    
    /// التكرار / Tiling
    Vec2 GetTiling() const {
        auto it = m_properties.find("tiling");
        return it != m_properties.end() ? std::get<Vec2>(it->second.value) : Vec2::One();
    }
    void SetTiling(const Vec2& tiling) { SetVec2("tiling", tiling); }
    
    /// الإزاحة / Offset
    Vec2 GetOffset() const {
        auto it = m_properties.find("offset");
        return it != m_properties.end() ? std::get<Vec2>(it->second.value) : Vec2::Zero();
    }
    void SetOffset(const Vec2& offset) { SetVec2("offset", offset); }
};

// ============================================================================
// مادة Sprite / Sprite Material
// ============================================================================

/// مادة للصور الطيفية / Sprite material for 2D in 3D
class SpriteMaterial : public UnlitMaterial {
public:
    /// المنشئ الافتراضي / Default constructor
    SpriteMaterial() : UnlitMaterial() {
        GetRenderState().blend = BlendState::Alpha();
        GetRenderState().depth.writeEnabled = false;
        GetRenderState().cullMode = CullMode::None;
    }
    
    /// منشئ بنسيج / Constructor with texture
    explicit SpriteMaterial(const TexturePtr& texture) : SpriteMaterial() {
        SetMainTexture(texture);
    }
};

// ============================================================================
// مادة Sky / Sky Material
// ============================================================================

/// نوع السماء / Sky type
enum class SkyType {
    Color,      ///< لون واحد / Solid color
    Gradient,   ///< تدرج / Gradient
    Skybox,     ///< صندوق السماء / Cubemap skybox
    Procedural  ///< إجرائي / Procedural
};

/// مادة السماء / Sky material
class SkyMaterial : public Material {
public:
    /// المنشئ الافتراضي / Default constructor
    SkyMaterial() : m_skyType(SkyType::Gradient) {
        SetColor("topColor", Color(0.4f, 0.6f, 1.0f));
        SetColor("bottomColor", Color(0.8f, 0.9f, 1.0f));
        SetColor("horizonColor", Color(0.7f, 0.8f, 0.95f));
        SetFloat("gradientSharpness", 0.5f);
        SetFloat("sunSize", 0.05f);
        SetFloat("sunIntensity", 10.0f);
    }
    
    /// نوع السماء / Sky type
    SkyType GetSkyType() const { return m_skyType; }
    void SetSkyType(SkyType type) { m_skyType = type; }
    
    /// ألوان التدرج / Gradient colors
    Color GetTopColor() const { return GetColor("topColor"); }
    void SetTopColor(const Color& color) { SetColor("topColor", color); }
    
    Color GetBottomColor() const { return GetColor("bottomColor"); }
    void SetBottomColor(const Color& color) { SetColor("bottomColor", color); }
    
    Color GetHorizonColor() const { return GetColor("horizonColor"); }
    void SetHorizonColor(const Color& color) { SetColor("horizonColor", color); }
    
    /// خريطة صندوق السماء / Skybox cubemap
    TexturePtr GetSkyboxTexture() const { return GetTexture("skybox"); }
    void SetSkyboxTexture(const TexturePtr& tex) { SetTexture("skybox", tex); m_skyType = SkyType::Skybox; }
    
    /// خصائص الشمس / Sun properties
    Float32 GetSunSize() const { return GetFloat("sunSize"); }
    void SetSunSize(Float32 size) { SetFloat("sunSize", size); }
    
    Float32 GetSunIntensity() const { return GetFloat("sunIntensity"); }
    void SetSunIntensity(Float32 intensity) { SetFloat("sunIntensity", intensity); }
    
private:
    SkyType m_skyType;
};

// ============================================================================
// مادة الجسيمات / Particle Material
// ============================================================================

/// مادة الجسيمات / Particle material
class ParticleMaterial : public Material {
public:
    /// المنشئ الافتراضي / Default constructor
    ParticleMaterial() {
        SetColor("color", Color::White);
        SetFloat("softness", 0.5f);
        
        GetRenderState().blend = BlendState::Additive();
        GetRenderState().depth.writeEnabled = false;
        GetRenderState().cullMode = CullMode::None;
    }
    
    /// اللون / Color
    Color GetParticleColor() const { return GetColor("color"); }
    void SetParticleColor(const Color& color) { SetColor("color", color); }
    
    /// النسيج / Texture
    TexturePtr GetParticleTexture() const { return GetTexture("particleTexture"); }
    void SetParticleTexture(const TexturePtr& tex) { SetTexture("particleTexture", tex); }
    
    /// نعومة الجسيمات / Softness
    Float32 GetSoftness() const { return GetFloat("softness"); }
    void SetSoftness(Float32 softness) { SetFloat("softness", softness); }
    
    /// تعيين وضع المزج / Set blend mode
    void SetAdditiveBlend() { GetRenderState().blend = BlendState::Additive(); }
    void SetAlphaBlend() { GetRenderState().blend = BlendState::Alpha(); }
};

// ============================================================================
// مؤشرات ذكية / Smart Pointers
// ============================================================================

using MaterialPtr = std::shared_ptr<Material>;
using PBRMaterialPtr = std::shared_ptr<PBRMaterial>;
using UnlitMaterialPtr = std::shared_ptr<UnlitMaterial>;

/// إنشاء مادة PBR / Create PBR material
inline PBRMaterialPtr CreatePBRMaterial() {
    return std::make_shared<PBRMaterial>();
}

/// إنشاء مادة Unlit / Create unlit material
inline UnlitMaterialPtr CreateUnlitMaterial(const Color& color = Color::White) {
    return std::make_shared<UnlitMaterial>(color);
}

// ============================================================================
// مكتبة المواد / Material Library
// ============================================================================

/// مكتبة لإدارة المواد / Library for managing materials
class MaterialLibrary {
public:
    /// الحصول على النسخة الوحيدة / Get singleton instance
    static MaterialLibrary& Instance() {
        static MaterialLibrary instance;
        return instance;
    }
    
    /// إضافة مادة / Add material
    void Add(const std::string& name, const MaterialPtr& material) {
        m_materials[name] = material;
    }
    
    /// الحصول على مادة / Get material
    MaterialPtr Get(const std::string& name) {
        auto it = m_materials.find(name);
        return (it != m_materials.end()) ? it->second : nullptr;
    }
    
    /// إزالة مادة / Remove material
    void Remove(const std::string& name) {
        m_materials.erase(name);
    }
    
    /// مسح الكل / Clear all
    void Clear() {
        m_materials.clear();
    }
    
    /// هل توجد / Has material
    bool Has(const std::string& name) const {
        return m_materials.find(name) != m_materials.end();
    }
    
    /// الحصول على الكل / Get all materials
    const std::unordered_map<std::string, MaterialPtr>& GetAll() const {
        return m_materials;
    }
    
private:
    MaterialLibrary() = default;
    std::unordered_map<std::string, MaterialPtr> m_materials;
};

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_MATERIAL_H
