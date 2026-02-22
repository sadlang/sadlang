// ============================================================================
// framebuffer.h - نظام إطار التخزين المؤقت / Framebuffer System
// ============================================================================
// الوصف: دعم الرسم على textures (render-to-texture) وتأثيرات ما بعد المعالجة
// Description: Support for render-to-texture and post-processing effects
// ============================================================================

#ifndef SAD_GRAPHICS_FRAMEBUFFER_H
#define SAD_GRAPHICS_FRAMEBUFFER_H

#include "../core/types.h"
#include "../texture/texture.h"
#include <memory>
#include <vector>

namespace sad {
namespace graphics {

// ============================================================================
// مواصفات إطار التخزين المؤقت / Framebuffer Specification
// ============================================================================

/// نوع المرفق / Attachment type
enum class FramebufferAttachmentType {
    Color,          // مرفق لون / Color attachment
    Depth,          // مرفق عمق / Depth attachment
    Stencil,        // مرفق استنسل / Stencil attachment
    DepthStencil    // مرفق عمق واستنسل / Depth-stencil attachment
};

/// مواصفات المرفق / Attachment specification
struct FramebufferAttachmentSpec {
    FramebufferAttachmentType type;   // نوع المرفق / Attachment type
    TextureFormat format;              // صيغة الـ texture / Texture format
    
    FramebufferAttachmentSpec() 
        : type(FramebufferAttachmentType::Color)
        , format(TextureFormat::RGBA8) {}
    
    FramebufferAttachmentSpec(FramebufferAttachmentType type_, TextureFormat format_)
        : type(type_), format(format_) {}
};

/// مواصفات إطار التخزين المؤقت / Framebuffer specification
struct FramebufferSpec {
    u32 width;          // العرض بالبكسل / Width in pixels
    u32 height;         // الارتفاع بالبكسل / Height in pixels
    u32 samples;        // عدد العينات (MSAA) / Number of samples (MSAA)
    bool swapChainTarget; // هل هو هدف سلسلة التبادل / Is swap chain target
    
    std::vector<FramebufferAttachmentSpec> colorAttachments;  // مرفقات اللون / Color attachments
    bool hasDepthAttachment;          // يحتوي على مرفق عمق / Has depth attachment
    bool hasStencilAttachment;        // يحتوي على مرفق استنسل / Has stencil attachment
    
    FramebufferSpec()
        : width(800), height(600), samples(1)
        , swapChainTarget(false)
        , hasDepthAttachment(true)
        , hasStencilAttachment(false) {
        // مرفق لون افتراضي / Default color attachment
        colorAttachments.push_back({FramebufferAttachmentType::Color, TextureFormat::RGBA8});
    }
};

// ============================================================================
// فئة إطار التخزين المؤقت / Framebuffer Class
// ============================================================================

class Framebuffer {
public:
    /// إنشاء framebuffer بمواصفات محددة / Create framebuffer with specification
    explicit Framebuffer(const FramebufferSpec& spec);
    
    /// المدمر / Destructor
    ~Framebuffer();
    
    // منع النسخ / Prevent copying
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    
    // ==============================================================================
    // الاستخدام / Usage
    // ==============================================================================
    
    /// بدء الرسم على الـ framebuffer / Begin rendering to framebuffer
    void Bind() const;
    
    /// التوقف عن الرسم على الـ framebuffer (العودة للشاشة) / Stop rendering to framebuffer
    static void Unbind();
    
    /// مسح الـ framebuffer / Clear framebuffer
    void Clear(const Color& color = Color::Black);
    
    // ==============================================================================
    // تغيير الحجم / Resizing
    // ==============================================================================
    
    /// تغيير حجم الـ framebuffer / Resize framebuffer
    void Resize(u32 width, u32 height);
    
    // ==============================================================================
    // الحصول على المعلومات / Getting Information
    // ==============================================================================
    
    /// الحصول على معرف الـ texture (لاستخدامه في الرسم) / Get texture ID
    u32 GetColorAttachmentID(u32 index = 0) const;
    
    /// الحصول على texture المرفق / Get color attachment texture
    TextureRef GetColorTexture(u32 index = 0) const;
    
    /// الحصول على معرف مرفق العمق / Get depth attachment ID
    u32 GetDepthAttachmentID() const { return m_depthAttachment; }
    
    /// الحصول على العرض / Get width
    u32 GetWidth() const { return m_spec.width; }
    
    /// الحصول على الارتفاع / Get height
    u32 GetHeight() const { return m_spec.height; }
    
    /// الحصول على المواصفات / Get specification
    const FramebufferSpec& GetSpec() const { return m_spec; }
    
    /// التحقق من الصلاحية / Check validity
    bool IsValid() const { return m_isValid; }

    /// الحصول على معرف FBO الخام / Get raw FBO ID
    u32 GetFBO() const { return m_fbo; }
    
    // ==============================================================================
    // قراءة البكسلات / Reading Pixels
    // ==============================================================================
    
    /// قراءة بكسل واحد من مرفق اللون / Read single pixel from color attachment
    Color ReadPixel(u32 x, u32 y, u32 attachmentIndex = 0) const;
    
    // ==============================================================================
    // دوال مساعدة / Helper Functions
    // ==============================================================================
    
    /// إنشاء framebuffer بسيط (لون + عمق) / Create simple framebuffer
    static std::shared_ptr<Framebuffer> Create(u32 width, u32 height);
    
    /// إنشاء framebuffer بمواصفات محددة / Create framebuffer with spec
    static std::shared_ptr<Framebuffer> Create(const FramebufferSpec& spec);
    
    /// نسخ framebuffer إلى آخر (blit) / Copy framebuffer to another (blit)
    static void Blit(const Framebuffer& src, const Framebuffer& dst);
    static void BlitToScreen(const Framebuffer& src, u32 screenWidth, u32 screenHeight);

private:
    /// إنشاء الموارد الداخلية / Create internal resources
    void Create();
    
    /// تحرير الموارد / Release resources
    void Destroy();
    
    FramebufferSpec m_spec;                      // المواصفات / Specification
    u32 m_fbo;                                   // معرف FBO / FBO ID
    std::vector<u32> m_colorAttachments;         // مرفقات اللون / Color attachments
    u32 m_depthAttachment;                       // مرفق العمق / Depth attachment
    bool m_isValid;                              // صالح / Valid
};

// أنواع مستعارة / Type aliases
using FramebufferRef = std::shared_ptr<Framebuffer>;

} // namespace graphics
} // namespace sad

// الوصول من SadGraphics / Access from SadGraphics namespace
namespace SadGraphics {
    using sad::graphics::Framebuffer;
    using sad::graphics::FramebufferRef;
    using sad::graphics::FramebufferSpec;
    using sad::graphics::FramebufferAttachmentType;
    using sad::graphics::FramebufferAttachmentSpec;
}

#endif // SAD_GRAPHICS_FRAMEBUFFER_H
