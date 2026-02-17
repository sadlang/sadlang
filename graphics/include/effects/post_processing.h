// ==============================================================================
// post_processing.h - نظام المعالجة البعدية / Post-Processing System
// ==============================================================================
// الوصف: تأثيرات بصرية متقدمة للشاشة الكاملة
// Description: Advanced full-screen visual effects
// ==============================================================================
// المميزات / Features:
//   - توهج (Bloom) / Bloom
//   - ضبابية (Blur) / Blur
//   - انحراف لوني / Chromatic aberration
//   - فينييت / Vignette
//   - تصحيح الألوان / Color correction
//   - تأثيرات CRT / CRT effects
//   - سلاسل تأثيرات قابلة للتخصيص / Customizable effect chains
// ==============================================================================

#ifndef SAD_GRAPHICS_POST_PROCESSING_H
#define SAD_GRAPHICS_POST_PROCESSING_H

#include "../core/types.h"
#include "../rendering/framebuffer.h"
#include "../rendering/shader.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

namespace sad {
namespace graphics {

// ShaderRef alias (ShaderProgram is in SadGraphics namespace)
using ShaderRef = std::shared_ptr<SadGraphics::ShaderProgram>;

// ==============================================================================
// أنواع التأثيرات / Effect Types
// ==============================================================================

/// أنواع التأثيرات المدمجة / Built-in effect types
enum class PostEffectType {
    None,               // بدون تأثير / No effect
    Bloom,              // توهج / Bloom
    Blur,               // ضبابية / Blur
    GaussianBlur,       // ضبابية غاوسية / Gaussian blur
    MotionBlur,         // ضبابية حركية / Motion blur
    ChromaticAberration,// انحراف لوني / Chromatic aberration
    Vignette,           // فينييت / Vignette
    ColorGrading,       // تدرج الألوان / Color grading
    Grayscale,          // تدرج رمادي / Grayscale
    Sepia,              // سيبيا / Sepia
    Invert,             // عكس الألوان / Invert colors
    Pixelate,           // تقطيع البكسل / Pixelate
    CRT,                // شاشة CRT قديمة / Old CRT effect
    FilmGrain,          // حبيبات الفيلم / Film grain
    Scanlines,          // خطوط المسح / Scanlines
    Sharpen,            // شحذ / Sharpen
    EdgeDetection,      // كشف الحواف / Edge detection
    Custom              // تأثير مخصص / Custom effect
};

// ==============================================================================
// معلمات التأثيرات / Effect Parameters
// ==============================================================================

/// معلمات التوهج / Bloom parameters
struct BloomParams {
    Float32 threshold = 0.8f;       // عتبة السطوع / Brightness threshold
    Float32 intensity = 1.0f;       // شدة التوهج / Bloom intensity
    Float32 radius = 1.0f;          // نصف قطر الانتشار / Spread radius
    Int32 passes = 5;               // عدد مرات التمرير / Number of passes
};

/// معلمات الضبابية / Blur parameters
struct BlurParams {
    Float32 radius = 5.0f;          // نصف القطر / Radius
    Float32 sigma = 2.0f;           // سيغما (للغاوسي) / Sigma (for Gaussian)
    Int32 samples = 9;              // عدد العينات / Sample count
    Vec2 direction = Vec2(1, 0);    // اتجاه الضبابية / Blur direction
};

/// معلمات الانحراف اللوني / Chromatic aberration parameters
struct ChromaticAberrationParams {
    Float32 intensity = 1.0f;       // شدة التأثير / Effect intensity
    Float32 redOffset = 0.005f;     // إزاحة الأحمر / Red offset
    Float32 blueOffset = -0.005f;   // إزاحة الأزرق / Blue offset
};

/// معلمات الفينييت / Vignette parameters
struct VignetteParams {
    Float32 intensity = 0.5f;       // شدة التعتيم / Darkness intensity
    Float32 radius = 0.75f;         // نصف القطر / Radius
    Float32 smoothness = 0.3f;      // نعومة الحافة / Edge smoothness
    Color color = Color::Black;     // لون الفينييت / Vignette color
};

/// معلمات تدرج الألوان / Color grading parameters
struct ColorGradingParams {
    Float32 brightness = 0.0f;      // السطوع (-1 إلى 1) / Brightness (-1 to 1)
    Float32 contrast = 1.0f;        // التباين / Contrast
    Float32 saturation = 1.0f;      // التشبع / Saturation
    Float32 gamma = 1.0f;           // غاما / Gamma
    Vec3 colorBalance = Vec3(1, 1, 1); // توازن الألوان / Color balance (RGB)
    Float32 temperature = 0.0f;     // درجة الحرارة / Temperature (-1 warm, 1 cool)
    Float32 tint = 0.0f;            // الصبغة / Tint (-1 green, 1 magenta)
};

/// معلمات التقطيع / Pixelate parameters
struct PixelateParams {
    Float32 pixelSize = 4.0f;       // حجم البكسل / Pixel size
    bool preserveAspect = true;     // حفظ نسبة العرض / Preserve aspect ratio
};

/// معلمات شاشة CRT / CRT effect parameters
struct CRTParams {
    Float32 curvature = 0.1f;       // انحناء الشاشة / Screen curvature
    Float32 scanlineIntensity = 0.3f; // شدة خطوط المسح / Scanline intensity
    Float32 vignette = 0.2f;        // فينييت / Vignette
    Float32 noise = 0.05f;          // ضوضاء / Noise
    Float32 flickering = 0.03f;     // وميض / Flickering
    Color tint = Color(0.9f, 1.0f, 0.9f, 1.0f); // صبغة خضراء خفيفة / Slight green tint
};

/// معلمات حبيبات الفيلم / Film grain parameters
struct FilmGrainParams {
    Float32 intensity = 0.1f;       // شدة الحبيبات / Grain intensity
    Float32 speed = 1.0f;           // سرعة التغير / Animation speed
    bool colored = false;           // حبيبات ملونة / Colored grain
};

// ==============================================================================
// التأثير الأساسي / Base Effect
// ==============================================================================

/// التأثير الأساسي / Base post-processing effect
class PostEffect {
public:
    // ============================================================================
    // الإنشاء والتدمير / Construction and Destruction
    // ============================================================================
    
    PostEffect(PostEffectType type = PostEffectType::None);
    virtual ~PostEffect() = default;
    
    // ============================================================================
    // الخصائص / Properties
    // ============================================================================
    
    /// نوع التأثير / Effect type
    PostEffectType GetType() const { return m_type; }
    
    /// الاسم / Name
    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    
    /// هل التأثير مفعّل / Is effect enabled
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    /// شدة التأثير (0-1) / Effect intensity (0-1)
    Float32 GetIntensity() const { return m_intensity; }
    void SetIntensity(Float32 intensity) { m_intensity = std::max(0.0f, std::min(1.0f, intensity)); }
    
    // ============================================================================
    // التطبيق / Application
    // ============================================================================
    
    /// هل يحتاج FBO إضافي / Needs extra FBO
    virtual bool NeedsExtraPass() const { return false; }
    
    /// تهيئة الموارد / Initialize resources
    virtual bool Initialize(Int32 width, Int32 height) { return true; }
    
    /// تنظيف الموارد / Cleanup resources
    virtual void Cleanup() {}
    
    /// تحديث التأثير / Update effect  
    virtual void Update(Float32 deltaTime) { m_time += deltaTime; }
    
    /// تطبيق التأثير / Apply effect
    virtual void Apply(UInt32 inputTexture, UInt32 outputFBO, Int32 width, Int32 height) = 0;

protected:
    PostEffectType m_type;
    std::string m_name;
    bool m_enabled = true;
    Float32 m_intensity = 1.0f;
    Float32 m_time = 0.0f;
    
    /// شيدر التأثير / Effect shader
    ShaderRef m_shader;
};

// ==============================================================================
// التأثيرات المدمجة / Built-in Effects
// ==============================================================================

/// تأثير التوهج / Bloom effect
class BloomEffect : public PostEffect {
public:
    BloomEffect();
    
    BloomParams& GetParams() { return m_params; }
    const BloomParams& GetParams() const { return m_params; }
    void SetParams(const BloomParams& params) { m_params = params; }
    
    bool NeedsExtraPass() const override { return true; }
    bool Initialize(Int32 width, Int32 height) override;
    void Cleanup() override;
    void Apply(UInt32 inputTexture, UInt32 outputFBO, Int32 width, Int32 height) override;

private:
    BloomParams m_params;
    std::vector<std::unique_ptr<Framebuffer>> m_blurFBOs;
    ShaderRef m_extractShader;
    ShaderRef m_blurShader;
    ShaderRef m_combineShader;
};

/// تأثير الضبابية / Blur effect
class BlurEffect : public PostEffect {
public:
    BlurEffect();
    
    BlurParams& GetParams() { return m_params; }
    const BlurParams& GetParams() const { return m_params; }
    void SetParams(const BlurParams& params) { m_params = params; }
    
    bool NeedsExtraPass() const override { return true; }
    bool Initialize(Int32 width, Int32 height) override;
    void Apply(UInt32 inputTexture, UInt32 outputFBO, Int32 width, Int32 height) override;

private:
    BlurParams m_params;
    std::unique_ptr<Framebuffer> m_tempFBO;
};

/// تأثير الانحراف اللوني / Chromatic aberration effect
class ChromaticAberrationEffect : public PostEffect {
public:
    ChromaticAberrationEffect();
    
    ChromaticAberrationParams& GetParams() { return m_params; }
    void Apply(UInt32 inputTexture, UInt32 outputFBO, Int32 width, Int32 height) override;

private:
    ChromaticAberrationParams m_params;
};

/// تأثير الفينييت / Vignette effect
class VignetteEffect : public PostEffect {
public:
    VignetteEffect();
    
    VignetteParams& GetParams() { return m_params; }
    void Apply(UInt32 inputTexture, UInt32 outputFBO, Int32 width, Int32 height) override;

private:
    VignetteParams m_params;
};

/// تأثير تدرج الألوان / Color grading effect
class ColorGradingEffect : public PostEffect {
public:
    ColorGradingEffect();
    
    ColorGradingParams& GetParams() { return m_params; }
    void Apply(UInt32 inputTexture, UInt32 outputFBO, Int32 width, Int32 height) override;

private:
    ColorGradingParams m_params;
};

/// تأثير التقطيع / Pixelate effect
class PixelateEffect : public PostEffect {
public:
    PixelateEffect();
    
    PixelateParams& GetParams() { return m_params; }
    void Apply(UInt32 inputTexture, UInt32 outputFBO, Int32 width, Int32 height) override;

private:
    PixelateParams m_params;
};

/// تأثير شاشة CRT / CRT effect
class CRTEffect : public PostEffect {
public:
    CRTEffect();
    
    CRTParams& GetParams() { return m_params; }
    void Apply(UInt32 inputTexture, UInt32 outputFBO, Int32 width, Int32 height) override;

private:
    CRTParams m_params;
};

/// تأثير حبيبات الفيلم / Film grain effect
class FilmGrainEffect : public PostEffect {
public:
    FilmGrainEffect();
    
    FilmGrainParams& GetParams() { return m_params; }
    void Apply(UInt32 inputTexture, UInt32 outputFBO, Int32 width, Int32 height) override;

private:
    FilmGrainParams m_params;
};

// ==============================================================================
// سلسلة المعالجة البعدية / Post-Processing Pipeline
// ==============================================================================

/// سلسلة المعالجة البعدية / Post-processing pipeline
class PostProcessingPipeline {
public:
    // ============================================================================
    // الإنشاء والتدمير / Construction and Destruction
    // ============================================================================
    
    PostProcessingPipeline();
    ~PostProcessingPipeline();
    
    // ============================================================================
    // التهيئة / Initialization
    // ============================================================================
    
    /// تهيئة السلسلة / Initialize pipeline
    bool Initialize(Int32 width, Int32 height);
    
    /// تغيير الحجم / Resize
    void Resize(Int32 width, Int32 height);
    
    /// تنظيف / Cleanup
    void Cleanup();
    
    // ============================================================================
    // إدارة التأثيرات / Effect Management
    // ============================================================================
    
    /// إضافة تأثير / Add effect
    template<typename T>
    T* AddEffect() {
        auto effect = std::make_unique<T>();
        effect->Initialize(m_width, m_height);
        T* ptr = effect.get();
        m_effects.push_back(std::move(effect));
        return ptr;
    }
    
    /// إضافة تأثير بنوع / Add effect by type
    PostEffect* AddEffect(PostEffectType type);
    
    /// إزالة تأثير / Remove effect
    void RemoveEffect(PostEffect* effect);
    
    /// مسح جميع التأثيرات / Clear all effects
    void ClearEffects();
    
    /// الحصول على تأثير بالفهرس / Get effect by index
    PostEffect* GetEffect(Int32 index);
    
    /// الحصول على تأثير بالنوع / Get effect by type
    template<typename T>
    T* GetEffect() {
        for (auto& effect : m_effects) {
            T* ptr = dynamic_cast<T*>(effect.get());
            if (ptr) return ptr;
        }
        return nullptr;
    }
    
    /// عدد التأثيرات / Effect count
    Int32 GetEffectCount() const { return static_cast<Int32>(m_effects.size()); }
    
    /// إعادة ترتيب التأثيرات / Reorder effects
    void MoveEffect(Int32 from, Int32 to);
    
    // ============================================================================
    // التشغيل / Execution
    // ============================================================================
    
    /// بدء الإطار (الرسم إلى FBO الداخلي) / Begin frame (render to internal FBO)
    void Begin();
    
    /// إنهاء الإطار (تطبيق التأثيرات والرسم للشاشة) / End frame (apply effects and render to screen)
    void End();
    
    /// تحديث التأثيرات / Update effects
    void Update(Float32 deltaTime);
    
    /// هل السلسلة مفعّلة / Is pipeline enabled
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
    // ============================================================================
    // الوصول إلى FBO / FBO Access
    // ============================================================================
    
    /// FBO المصدر / Source FBO
    Framebuffer* GetSourceFBO() const { return m_sourceFBO.get(); }
    
    /// نسيج المصدر / Source texture
    UInt32 GetSourceTexture() const;

private:
    Int32 m_width = 0;
    Int32 m_height = 0;
    bool m_enabled = true;
    bool m_initialized = false;
    
    std::unique_ptr<Framebuffer> m_sourceFBO;
    std::unique_ptr<Framebuffer> m_pingFBO;
    std::unique_ptr<Framebuffer> m_pongFBO;
    
    std::vector<std::unique_ptr<PostEffect>> m_effects;
    
    ShaderRef m_copyShader;
    UInt32 m_quadVAO = 0;
    UInt32 m_quadVBO = 0;
    
    void RenderQuad();
    void InitQuad();
    void CopyTexture(UInt32 source, UInt32 destFBO);
};

// ==============================================================================
// إعدادات مسبقة / Presets
// ==============================================================================

namespace PostProcessingPresets {
    /// سينمائي / Cinematic
    void ApplyCinematic(PostProcessingPipeline& pipeline);
    
    /// رجعي / Retro
    void ApplyRetro(PostProcessingPipeline& pipeline);
    
    /// حلم / Dream
    void ApplyDream(PostProcessingPipeline& pipeline);
    
    /// رعب / Horror
    void ApplyHorror(PostProcessingPipeline& pipeline);
    
    /// أبيض وأسود / Black and white
    void ApplyBlackAndWhite(PostProcessingPipeline& pipeline);
}

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_POST_PROCESSING_H
