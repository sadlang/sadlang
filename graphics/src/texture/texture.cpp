// texture.cpp - تنفيذ نظام إدارة الـ Textures
// Implementation of texture management system

#include "../../include/texture/texture.h"
#include "../../include/texture/image.h"
#include <glad/glad.h>
#include <cstring>
#include <iostream>

namespace sad {
namespace graphics {

// المُنشئ الافتراضي - Default constructor
Texture::Texture()
    : m_rendererID(0)      // بدون معرّف - no ID yet
    , m_width(0)           // بدون عرض - no width
    , m_height(0)          // بدون ارتفاع - no height
    , m_format(TextureFormat::UNKNOWN) // صيغة غير معروفة - unknown format
{
}

// المُنشئ بالمواصفات - Constructor with specifications
Texture::Texture(const TextureSpec& specs, const void* data)
    : Texture() // استدعاء المُنشئ الافتراضي - call default constructor
{
    Create(specs, data); // إنشاء الـ texture - create texture
}

// المُدمر - Destructor
Texture::~Texture() {
    Destroy(); // تحرير الموارد - release resources
}

// مُنشئ النقل - Move constructor
Texture::Texture(Texture&& other) noexcept
    : m_rendererID(other.m_rendererID)  // نقل المعرّف - move ID
    , m_width(other.m_width)            // نقل العرض - move width
    , m_height(other.m_height)          // نقل الارتفاع - move height
    , m_format(other.m_format)          // نقل الصيغة - move format
    , m_spec(other.m_spec)              // نقل المواصفات - move specs
{
    // إبطال الكائن المصدر - invalidate source object
    other.m_rendererID = 0;
    other.m_width = 0;
    other.m_height = 0;
    other.m_format = TextureFormat::UNKNOWN;
}

// عامل إسناد النقل - Move assignment operator
Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) { // تجنب الإسناد الذاتي - avoid self-assignment
        Destroy(); // تحرير الموارد الحالية - release current resources
        
        // نقل البيانات - move data
        m_rendererID = other.m_rendererID;
        m_width = other.m_width;
        m_height = other.m_height;
        m_format = other.m_format;
        m_spec = other.m_spec;
        
        // إبطال الكائن المصدر - invalidate source object
        other.m_rendererID = 0;
        other.m_width = 0;
        other.m_height = 0;
        other.m_format = TextureFormat::UNKNOWN;
    }
    return *this;
}

// إنشاء texture بمواصفات محددة
// Create texture with specific specifications
bool Texture::Create(const TextureSpec& specs, const void* data) {
    // التحقق من صحة المواصفات - validate specifications
    if (specs.width == 0 || specs.height == 0) {
        std::cerr << "Texture::Create: Invalid dimensions" << std::endl;
        return false;
    }
    
    // تحرير الـ texture القديم إن وُجد - destroy old texture if exists
    if (m_rendererID != 0) {
        Destroy();
    }
    
    // حفظ المواصفات - save specifications
    m_spec = specs;
    m_width = specs.width;
    m_height = specs.height;
    m_format = specs.format;
    
    // توليد texture في OpenGL - generate texture in OpenGL
    glGenTextures(1, &m_rendererID);
    glBindTexture(GL_TEXTURE_2D, m_rendererID);
    
    // تحويل الصيغة إلى OpenGL formats
    // Convert format to OpenGL formats
    u32 internalFormat = TextureFormatToGLInternalFormat(m_format);
    u32 format = TextureFormatToGLFormat(m_format);
    u32 dataType = TextureFormatToGLDataType(m_format);
    
    // رفع البيانات إلى الـ GPU - upload data to GPU
    glTexImage2D(
        GL_TEXTURE_2D,      // نوع الـ texture - texture type
        0,                  // مستوى mipmap - mipmap level
        internalFormat,     // الصيغة الداخلية - internal format
        m_width,            // العرض - width
        m_height,           // الارتفاع - height
        0,                  // الحدود (دائماً 0) - border (always 0)
        format,             // صيغة البيانات - data format
        dataType,           // نوع البيانات - data type
        data                // مؤشر البيانات - data pointer
    );
    
    // تطبيق wrapping modes
    // Apply wrapping modes
    SetWrap(specs.wrapS, specs.wrapT);
    
    // تطبيق filtering modes
    // Apply filtering modes
    SetFilter(specs.minFilter, specs.magFilter);
    
    // توليد mipmaps إذا طُلب - generate mipmaps if requested
    if (specs.generateMipmaps && data != nullptr) {
        GenerateMipmaps();
    }
    
    // فك الربط - unbind
    glBindTexture(GL_TEXTURE_2D, 0);
    
    return true; // نجح الإنشاء - creation succeeded
}

// تحديث بيانات الـ texture
// Update texture data
void Texture::SetData(const void* data, u32 x, u32 y, u32 width, u32 height) {
    if (!IsValid()) { // التحقق من صحة الـ texture - check texture validity
        std::cerr << "Texture::SetData: Invalid texture" << std::endl;
        return;
    }
    
    // استخدام الأبعاد الكاملة إذا لم تُحدد - use full dimensions if not specified
    if (width == 0) width = m_width;
    if (height == 0) height = m_height;
    
    // التحقق من حدود المنطقة - validate region bounds
    if (x + width > m_width || y + height > m_height) {
        std::cerr << "Texture::SetData: Region out of bounds" << std::endl;
        return;
    }
    
    // ربط الـ texture - bind texture
    glBindTexture(GL_TEXTURE_2D, m_rendererID);
    
    // تحويل الصيغة - convert format
    u32 format = TextureFormatToGLFormat(m_format);
    u32 dataType = TextureFormatToGLDataType(m_format);
    
    // تحديث المنطقة المحددة - update specified region
    glTexSubImage2D(
        GL_TEXTURE_2D,      // نوع الـ texture - texture type
        0,                  // مستوى mipmap - mipmap level
        x, y,               // الإزاحة - offset
        width, height,      // الأبعاد - dimensions
        format,             // صيغة البيانات - data format
        dataType,           // نوع البيانات - data type
        data                // مؤشر البيانات - data pointer
    );
    
    // إعادة توليد mipmaps إذا كانت مفعّلة - regenerate mipmaps if enabled
    if (m_spec.generateMipmaps) {
        GenerateMipmaps();
    }
    
    // فك الربط - unbind
    glBindTexture(GL_TEXTURE_2D, 0);
}

// ربط الـ texture بوحدة texture محددة
// Bind texture to specific texture unit
void Texture::Bind(u32 slot) const {
    if (!IsValid()) { // التحقق من صحة الـ texture - check texture validity
        return;
    }
    
    // تفعيل وحدة الـ texture - activate texture unit
    glActiveTexture(GL_TEXTURE0 + slot);
    
    // ربط الـ texture - bind texture
    glBindTexture(GL_TEXTURE_2D, m_rendererID);
}

// فك ربط الـ texture
// Unbind texture
void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0); // فك الربط - unbind
}

// توليد mipmaps للـ texture
// Generate mipmaps for texture
void Texture::GenerateMipmaps() {
    if (!IsValid()) { // التحقق من صحة الـ texture - check texture validity
        return;
    }
    
    // ربط الـ texture - bind texture
    glBindTexture(GL_TEXTURE_2D, m_rendererID);
    
    // توليد mipmaps - generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // فك الربط - unbind
    glBindTexture(GL_TEXTURE_2D, 0);
}

// تحرير موارد الـ texture
// Release texture resources
void Texture::Destroy() {
    if (m_rendererID != 0) { // إذا كان الـ texture موجوداً - if texture exists
        glDeleteTextures(1, &m_rendererID); // حذف من OpenGL - delete from OpenGL
        m_rendererID = 0;
        m_width = 0;
        m_height = 0;
        m_format = TextureFormat::UNKNOWN;
    }
}

// تعيين wrapping mode
// Set wrapping mode
void Texture::SetWrap(TextureWrap wrapS, TextureWrap wrapT) {
    if (!IsValid()) return; // التحقق من الصحة - validate
    
    glBindTexture(GL_TEXTURE_2D, m_rendererID); // ربط - bind
    
    // تطبيق wrapping على المحور S (X) - apply S-axis (X) wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapToGL(wrapS));
    
    // تطبيق wrapping على المحور T (Y) - apply T-axis (Y) wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapToGL(wrapT));
    
    glBindTexture(GL_TEXTURE_2D, 0); // فك الربط - unbind
    
    // تحديث المواصفات - update specifications
    m_spec.wrapS = wrapS;
    m_spec.wrapT = wrapT;
}

// تعيين filtering mode
// Set filtering mode
void Texture::SetFilter(TextureFilter minFilter, TextureFilter magFilter) {
    if (!IsValid()) return; // التحقق من الصحة - validate
    
    glBindTexture(GL_TEXTURE_2D, m_rendererID); // ربط - bind
    
    // تطبيق تصفية التصغير - apply minification filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilterToGL(minFilter));
    
    // تطبيق تصفية التكبير - apply magnification filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilterToGL(magFilter));
    
    glBindTexture(GL_TEXTURE_2D, 0); // فك الربط - unbind
    
    // تحديث المواصفات - update specifications
    m_spec.minFilter = minFilter;
    m_spec.magFilter = magFilter;
}

// تعيين لون الحدود
// Set border color
void Texture::SetBorderColor(const Vec4& color) {
    if (!IsValid()) return; // التحقق من الصحة - validate
    
    glBindTexture(GL_TEXTURE_2D, m_rendererID); // ربط - bind
    
    // تطبيق لون الحدود - apply border color
    float borderColor[4] = { color.x, color.y, color.z, color.w };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    glBindTexture(GL_TEXTURE_2D, 0); // فك الربط - unbind
}

// إنشاء texture أبيض 1x1
// Create 1x1 white texture
std::shared_ptr<Texture> Texture::CreateWhite() {
    // بيانات بكسل أبيض - white pixel data
    u32 whitePixel = 0xFFFFFFFF; // RGBA: كل القنوات 255 - all channels 255
    
    // مواصفات texture 1x1 - 1x1 texture specifications
    TextureSpec spec;
    spec.width = 1;
    spec.height = 1;
    spec.format = TextureFormat::RGBA8;
    spec.generateMipmaps = false; // لا حاجة لـ mipmaps لـ texture 1x1 - no mipmaps needed for 1x1
    
    // إنشاء وإرجاع الـ texture - create and return texture
    auto texture = std::make_shared<Texture>(spec, &whitePixel);
    return texture;
}

// إنشاء texture أسود 1x1
// Create 1x1 black texture
std::shared_ptr<Texture> Texture::CreateBlack() {
    // بيانات بكسل أسود - black pixel data
    u32 blackPixel = 0xFF000000; // RGBA: RGB=0, A=255 - RGB=0, A=255
    
    // مواصفات texture 1x1 - 1x1 texture specifications
    TextureSpec spec;
    spec.width = 1;
    spec.height = 1;
    spec.format = TextureFormat::RGBA8;
    spec.generateMipmaps = false; // لا حاجة لـ mipmaps - no mipmaps needed
    
    // إنشاء وإرجاع الـ texture - create and return texture
    auto texture = std::make_shared<Texture>(spec, &blackPixel);
    return texture;
}

// إنشاء texture من ملف صورة
// Create texture from image file
std::shared_ptr<Texture> Texture::CreateFromFile(
    const std::string& filepath,
    bool generateMipmaps,
    bool sRGB)
{
    // استخدام دالة LoadTextureFromFile من image.h
    // Use LoadTextureFromFile function from image.h
    return LoadTextureFromFile(filepath, generateMipmaps, sRGB);
}

// الحصول على عدد القنوات من الصيغة
// Get number of channels from format
u32 Texture::GetChannelCount(TextureFormat format) {
    switch (format) {
        case TextureFormat::R8:
        case TextureFormat::R16F:
        case TextureFormat::R32F:
            return 1; // قناة واحدة - one channel
            
        case TextureFormat::RG8:
        case TextureFormat::RG16F:
        case TextureFormat::RG32F:
            return 2; // قناتان - two channels
            
        case TextureFormat::RGB8:
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F:
            return 3; // ثلاث قنوات - three channels
            
        case TextureFormat::RGBA8:
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA32F:
            return 4; // أربع قنوات - four channels
            
        default:
            return 0; // غير معروف - unknown
    }
}

// الحصول على حجم البكسل بالبايتات
// Get pixel size in bytes
u32 Texture::GetPixelSize(TextureFormat format) {
    switch (format) {
        case TextureFormat::R8: return 1;      // 1 بايت - 1 byte
        case TextureFormat::RG8: return 2;     // 2 بايت - 2 bytes
        case TextureFormat::RGB8: return 3;    // 3 بايت - 3 bytes
        case TextureFormat::RGBA8: return 4;   // 4 بايت - 4 bytes
        
        case TextureFormat::R16F: return 2;    // 2 بايت - 2 bytes
        case TextureFormat::RG16F: return 4;   // 4 بايت - 4 bytes
        case TextureFormat::RGB16F: return 6;  // 6 بايت - 6 bytes
        case TextureFormat::RGBA16F: return 8; // 8 بايت - 8 bytes
        
        case TextureFormat::R32F: return 4;    // 4 بايت - 4 bytes
        case TextureFormat::RG32F: return 8;   // 8 بايت - 8 bytes
        case TextureFormat::RGB32F: return 12; // 12 بايت - 12 bytes
        case TextureFormat::RGBA32F: return 16; // 16 بايت - 16 bytes
        
        case TextureFormat::DEPTH24_STENCIL8: return 4; // 4 بايت - 4 bytes
        
        default: return 0; // غير معروف - unknown
    }
}

// تحويل TextureFormat إلى OpenGL internal format
// Convert TextureFormat to OpenGL internal format
u32 Texture::TextureFormatToGLInternalFormat(TextureFormat format) {
    switch (format) {
        case TextureFormat::R8: return GL_R8;
        case TextureFormat::RG8: return GL_RG8;
        case TextureFormat::RGB8: return GL_RGB8;
        case TextureFormat::RGBA8: return GL_RGBA8;
        
        case TextureFormat::R16F: return GL_R16F;
        case TextureFormat::RG16F: return GL_RG16F;
        case TextureFormat::RGB16F: return GL_RGB16F;
        case TextureFormat::RGBA16F: return GL_RGBA16F;
        
        case TextureFormat::R32F: return GL_R32F;
        case TextureFormat::RG32F: return GL_RG32F;
        case TextureFormat::RGB32F: return GL_RGB32F;
        case TextureFormat::RGBA32F: return GL_RGBA32F;
        
        case TextureFormat::DEPTH24_STENCIL8: return GL_DEPTH24_STENCIL8;
        
        default: return GL_RGBA8; // افتراضي - default
    }
}

// تحويل TextureFormat إلى OpenGL format
// Convert TextureFormat to OpenGL format
u32 Texture::TextureFormatToGLFormat(TextureFormat format) {
    switch (format) {
        case TextureFormat::R8:
        case TextureFormat::R16F:
        case TextureFormat::R32F:
            return GL_RED; // قناة حمراء فقط - red channel only
            
        case TextureFormat::RG8:
        case TextureFormat::RG16F:
        case TextureFormat::RG32F:
            return GL_RG; // قناتا أحمر وأخضر - red and green channels
            
        case TextureFormat::RGB8:
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F:
            return GL_RGB; // قنوات RGB - RGB channels
            
        case TextureFormat::RGBA8:
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA32F:
            return GL_RGBA; // قنوات RGBA - RGBA channels
            
        case TextureFormat::DEPTH24_STENCIL8:
            return GL_DEPTH_STENCIL; // عمق + stencil - depth + stencil
            
        default:
            return GL_RGBA; // افتراضي - default
    }
}

// تحويل TextureFormat إلى OpenGL data type
// Convert TextureFormat to OpenGL data type
u32 Texture::TextureFormatToGLDataType(TextureFormat format) {
    switch (format) {
        case TextureFormat::R8:
        case TextureFormat::RG8:
        case TextureFormat::RGB8:
        case TextureFormat::RGBA8:
            return GL_UNSIGNED_BYTE; // بايتات بدون إشارة - unsigned bytes
            
        case TextureFormat::R16F:
        case TextureFormat::RG16F:
        case TextureFormat::RGB16F:
        case TextureFormat::RGBA16F:
            return GL_HALF_FLOAT; // float نصفي - half float
            
        case TextureFormat::R32F:
        case TextureFormat::RG32F:
        case TextureFormat::RGB32F:
        case TextureFormat::RGBA32F:
            return GL_FLOAT; // float كامل - full float
            
        case TextureFormat::DEPTH24_STENCIL8:
            return GL_UNSIGNED_INT_24_8; // عمق 24-bit + stencil 8-bit
            
        default:
            return GL_UNSIGNED_BYTE; // افتراضي - default
    }
}

// تحويل TextureWrap إلى OpenGL wrap mode
// Convert TextureWrap to OpenGL wrap mode
u32 Texture::TextureWrapToGL(TextureWrap wrap) {
    switch (wrap) {
        case TextureWrap::REPEAT: return GL_REPEAT;
        case TextureWrap::MIRRORED_REPEAT: return GL_MIRRORED_REPEAT;
        case TextureWrap::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
        case TextureWrap::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
        default: return GL_REPEAT; // افتراضي - default
    }
}

// تحويل TextureFilter إلى OpenGL filter mode
// Convert TextureFilter to OpenGL filter mode
u32 Texture::TextureFilterToGL(TextureFilter filter) {
    switch (filter) {
        case TextureFilter::NEAREST: return GL_NEAREST;
        case TextureFilter::LINEAR: return GL_LINEAR;
        case TextureFilter::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
        case TextureFilter::LINEAR_MIPMAP_NEAREST: return GL_LINEAR_MIPMAP_NEAREST;
        case TextureFilter::NEAREST_MIPMAP_LINEAR: return GL_NEAREST_MIPMAP_LINEAR;
        case TextureFilter::LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
        default: return GL_LINEAR; // افتراضي - default
    }
}

} // namespace graphics
} // namespace sad
