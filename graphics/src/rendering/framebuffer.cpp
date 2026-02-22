// ============================================================================
// framebuffer.cpp - تنفيذ نظام إطار التخزين المؤقت / Framebuffer Implementation
// ============================================================================

#include "../../include/rendering/framebuffer.h"
#include <glad/glad.h>
#include <iostream>
#include <algorithm>

namespace sad {
namespace graphics {

// ============================================================================
// دوال مساعدة داخلية / Internal Helper Functions
// ============================================================================

/// تحويل TextureFormat إلى صيغة OpenGL الداخلية / Convert TextureFormat to GL internal format
static GLenum TextureFormatToGLInternal(TextureFormat format) {
    switch (format) {
        case TextureFormat::R8:      return GL_R8;
        case TextureFormat::RG8:     return GL_RG8;
        case TextureFormat::RGB8:    return GL_RGB8;
        case TextureFormat::RGBA8:   return GL_RGBA8;
        case TextureFormat::R16F:    return GL_R16F;
        case TextureFormat::RG16F:   return GL_RG16F;
        case TextureFormat::RGB16F:  return GL_RGB16F;
        case TextureFormat::RGBA16F: return GL_RGBA16F;
        case TextureFormat::R32F:    return GL_R32F;
        case TextureFormat::RGB32F:  return GL_RGB32F;
        case TextureFormat::RGBA32F: return GL_RGBA32F;
        default:                     return GL_RGBA8;
    }
}

/// تحويل TextureFormat إلى صيغة OpenGL / Convert TextureFormat to GL format
static GLenum TextureFormatToGLFormat(TextureFormat format) {
    switch (format) {
        case TextureFormat::R8:
        case TextureFormat::R16F:
        case TextureFormat::R32F:    return GL_RED;
        case TextureFormat::RG8:
        case TextureFormat::RG16F:   return GL_RG;
        case TextureFormat::RGB8:
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F:  return GL_RGB;
        case TextureFormat::RGBA8:
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA32F: return GL_RGBA;
        default:                     return GL_RGBA;
    }
}

/// تحويل TextureFormat إلى نوع OpenGL / Convert TextureFormat to GL data type
static GLenum TextureFormatToGLType(TextureFormat format) {
    switch (format) {
        case TextureFormat::R8:
        case TextureFormat::RG8:
        case TextureFormat::RGB8:
        case TextureFormat::RGBA8:   return GL_UNSIGNED_BYTE;
        case TextureFormat::R16F:
        case TextureFormat::RG16F:
        case TextureFormat::RGB16F:
        case TextureFormat::RGBA16F: return GL_HALF_FLOAT;
        case TextureFormat::R32F:
        case TextureFormat::RGB32F:
        case TextureFormat::RGBA32F: return GL_FLOAT;
        default:                     return GL_UNSIGNED_BYTE;
    }
}

// ============================================================================
// الإنشاء والتدمير / Construction and Destruction
// ============================================================================

Framebuffer::Framebuffer(const FramebufferSpec& spec)
    : m_spec(spec)
    , m_fbo(0)
    , m_depthAttachment(0)
    , m_isValid(false)
{
    Create();
}

Framebuffer::~Framebuffer() {
    Destroy();
}

// ============================================================================
// الإنشاء الداخلي / Internal Creation
// ============================================================================

void Framebuffer::Create() {
    // حذف الموارد القديمة / Delete old resources
    if (m_fbo) {
        Destroy();
    }
    
    // إنشاء FBO / Create FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    
    // إنشاء مرفقات اللون / Create color attachments
    m_colorAttachments.resize(m_spec.colorAttachments.size());
    
    if (!m_spec.colorAttachments.empty()) {
        // إنشاء textures لمرفقات اللون / Create textures for color attachments
        glGenTextures(static_cast<GLsizei>(m_colorAttachments.size()), m_colorAttachments.data());
        
        for (size_t i = 0; i < m_colorAttachments.size(); i++) {
            glBindTexture(GL_TEXTURE_2D, m_colorAttachments[i]);
            
            GLenum internalFormat = TextureFormatToGLInternal(m_spec.colorAttachments[i].format);
            GLenum format = TextureFormatToGLFormat(m_spec.colorAttachments[i].format);
            GLenum type = TextureFormatToGLType(m_spec.colorAttachments[i].format);
            
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
                        m_spec.width, m_spec.height, 0,
                        format, type, nullptr);
            
            // إعدادات التصفية / Filter settings
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            // ربط المرفق / Attach
            glFramebufferTexture2D(GL_FRAMEBUFFER, 
                                   GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i),
                                   GL_TEXTURE_2D, m_colorAttachments[i], 0);
        }
        
        // تحديد مخرجات الرسم / Specify draw outputs
        if (m_colorAttachments.size() > 1) {
            std::vector<GLenum> drawBuffers(m_colorAttachments.size());
            for (size_t i = 0; i < drawBuffers.size(); i++) {
                drawBuffers[i] = GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i);
            }
            glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
        }
    }
    
    // إنشاء مرفق العمق/الاستنسل / Create depth/stencil attachment
    if (m_spec.hasDepthAttachment || m_spec.hasStencilAttachment) {
        glGenTextures(1, &m_depthAttachment);
        glBindTexture(GL_TEXTURE_2D, m_depthAttachment);
        
        if (m_spec.hasDepthAttachment && m_spec.hasStencilAttachment) {
            // عمق + استنسل / Depth + Stencil
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,
                        m_spec.width, m_spec.height, 0,
                        GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                   GL_TEXTURE_2D, m_depthAttachment, 0);
        } else if (m_spec.hasDepthAttachment) {
            // عمق فقط / Depth only
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,
                        m_spec.width, m_spec.height, 0,
                        GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                   GL_TEXTURE_2D, m_depthAttachment, 0);
        }
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    
    // التحقق من اكتمال الـ framebuffer / Check framebuffer completeness
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer::Create: Framebuffer is not complete! Status: 0x" 
                  << std::hex << status << std::dec << std::endl;
        m_isValid = false;
    } else {
        m_isValid = true;
    }
    
    // فك الربط / Unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::Destroy() {
    if (!m_colorAttachments.empty()) {
        glDeleteTextures(static_cast<GLsizei>(m_colorAttachments.size()), m_colorAttachments.data());
        m_colorAttachments.clear();
    }
    
    if (m_depthAttachment) {
        glDeleteTextures(1, &m_depthAttachment);
        m_depthAttachment = 0;
    }
    
    if (m_fbo) {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }
    
    m_isValid = false;
}

// ============================================================================
// الاستخدام / Usage
// ============================================================================

void Framebuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_spec.width, m_spec.height);
}

void Framebuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Clear(const Color& color) {
    Bind();
    glClearColor(color.r, color.g, color.b, color.a);
    GLbitfield clearBits = GL_COLOR_BUFFER_BIT;
    if (m_spec.hasDepthAttachment) clearBits |= GL_DEPTH_BUFFER_BIT;
    if (m_spec.hasStencilAttachment) clearBits |= GL_STENCIL_BUFFER_BIT;
    glClear(clearBits);
}

// ============================================================================
// تغيير الحجم / Resizing
// ============================================================================

void Framebuffer::Resize(u32 width, u32 height) {
    if (width == 0 || height == 0 || width > 16384 || height > 16384) {
        std::cerr << "Framebuffer::Resize: Invalid dimensions: " 
                  << width << "x" << height << std::endl;
        return;
    }
    
    m_spec.width = width;
    m_spec.height = height;
    
    Create(); // إعادة إنشاء / Recreate
}

// ============================================================================
// الحصول على المعلومات / Getting Information
// ============================================================================

u32 Framebuffer::GetColorAttachmentID(u32 index) const {
    if (index < m_colorAttachments.size()) {
        return m_colorAttachments[index];
    }
    return 0;
}

TextureRef Framebuffer::GetColorTexture(u32 index) const {
    u32 id = GetColorAttachmentID(index);
    if (id == 0) return nullptr;
    
    // إنشاء texture wrapper / Create texture wrapper
    TextureSpec spec;
    spec.width = m_spec.width;
    spec.height = m_spec.height;
    if (index < m_spec.colorAttachments.size()) {
        spec.format = m_spec.colorAttachments[index].format;
    }
    
    // نستخدم Texture wrapper مع المعرف الموجود / Use existing texture ID wrapper
    auto texture = std::make_shared<Texture>(spec, nullptr);
    return texture;
}

// ============================================================================
// قراءة البكسلات / Reading Pixels
// ============================================================================

Color Framebuffer::ReadPixel(u32 x, u32 y, u32 attachmentIndex) const {
    if (x >= m_spec.width || y >= m_spec.height) {
        return Color::Black;
    }
    
    Bind();
    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
    
    Float32 pixel[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, pixel);
    
    return Color(pixel[0], pixel[1], pixel[2], pixel[3]);
}

// ============================================================================
// دوال مساعدة ساكنة / Static Helper Functions
// ============================================================================

std::shared_ptr<Framebuffer> Framebuffer::Create(u32 width, u32 height) {
    FramebufferSpec spec;
    spec.width = width;
    spec.height = height;
    return std::make_shared<Framebuffer>(spec);
}

std::shared_ptr<Framebuffer> Framebuffer::Create(const FramebufferSpec& spec) {
    return std::make_shared<Framebuffer>(spec);
}

void Framebuffer::Blit(const Framebuffer& src, const Framebuffer& dst) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, src.m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.m_fbo);
    glBlitFramebuffer(
        0, 0, src.m_spec.width, src.m_spec.height,
        0, 0, dst.m_spec.width, dst.m_spec.height,
        GL_COLOR_BUFFER_BIT, GL_LINEAR
    );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::BlitToScreen(const Framebuffer& src, u32 screenWidth, u32 screenHeight) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, src.m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(
        0, 0, src.m_spec.width, src.m_spec.height,
        0, 0, screenWidth, screenHeight,
        GL_COLOR_BUFFER_BIT, GL_LINEAR
    );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

} // namespace graphics
} // namespace sad
