// texture.h - نظام إدارة الـ Textures على الـ GPU
// Texture management system for GPU textures
// يوفر واجهة لإنشاء وإدارة textures ثنائية الأبعاد مع دعم لعدة صيغ
// Provides interface for creating and managing 2D textures with multiple format support

#pragma once

#include "../core/types.h"
#include <string>
#include <memory>

namespace sad {
namespace graphics {

// تعداد صيغ الـ texture المدعومة
// Enumeration of supported texture formats
enum class TextureFormat {
    UNKNOWN = 0,          // صيغة غير معروفة - Unknown format
    R8,                   // أحمر 8-bit - Red 8-bit
    RG8,                  // أحمر-أخضر 8-bit - Red-Green 8-bit
    RGB8,                 // RGB 8-bit لكل قناة - RGB 8-bit per channel
    RGBA8,                // RGBA 8-bit لكل قناة - RGBA 8-bit per channel
    R16F,                 // أحمر 16-bit float - Red 16-bit float
    RG16F,                // أحمر-أخضر 16-bit float - Red-Green 16-bit float
    RGB16F,               // RGB 16-bit float - RGB 16-bit float per channel
    RGBA16F,              // RGBA 16-bit float - RGBA 16-bit float per channel
    R32F,                 // أحمر 32-bit float - Red 32-bit float
    RG32F,                // أحمر-أخضر 32-bit float - Red-Green 32-bit float
    RGB32F,               // RGB 32-bit float - RGB 32-bit float per channel
    RGBA32F,              // RGBA 32-bit float - RGBA 32-bit float per channel
    DEPTH24_STENCIL8      // عمق 24-bit + stencil 8-bit - Depth 24-bit + stencil 8-bit
};

// تعداد أنواع الـ texture wrapping
// Enumeration of texture wrapping modes
enum class TextureWrap {
    REPEAT = 0,           // تكرار - Repeat texture
    MIRRORED_REPEAT,      // تكرار معكوس - Mirrored repeat
    CLAMP_TO_EDGE,        // تثبيت على الحافة - Clamp to edge
    CLAMP_TO_BORDER       // تثبيت على الحدود - Clamp to border
};

// تعداد أنواع الـ texture filtering
// Enumeration of texture filtering modes
enum class TextureFilter {
    NEAREST = 0,          // أقرب نقطة - Nearest neighbor
    LINEAR,               // خطي - Linear interpolation
    NEAREST_MIPMAP_NEAREST, // أقرب mipmap + أقرب نقطة - Nearest mipmap + nearest
    LINEAR_MIPMAP_NEAREST,  // خطي + أقرب mipmap - Linear + nearest mipmap
    NEAREST_MIPMAP_LINEAR,  // أقرب نقطة + mipmap خطي - Nearest + linear mipmap
    LINEAR_MIPMAP_LINEAR    // خطي بالكامل (trilinear) - Fully linear (trilinear)
};

// مواصفات إنشاء texture جديد
// Specifications for creating a new texture
struct TextureSpec {
    u32 width = 0;                          // عرض الـ texture بالبكسل - Texture width in pixels
    u32 height = 0;                         // ارتفاع الـ texture بالبكسل - Texture height in pixels
    TextureFormat format = TextureFormat::RGBA8; // صيغة البيانات - Data format
    TextureWrap wrapS = TextureWrap::REPEAT;     // wrapping على المحور S (X) - S-axis (X) wrapping
    TextureWrap wrapT = TextureWrap::REPEAT;     // wrapping على المحور T (Y) - T-axis (Y) wrapping
    TextureFilter minFilter = TextureFilter::LINEAR; // تصفية التصغير - Minification filter
    TextureFilter magFilter = TextureFilter::LINEAR; // تصفية التكبير - Magnification filter
    bool generateMipmaps = true;            // توليد mipmaps تلقائياً - Generate mipmaps automatically
    bool sRGB = false;                      // استخدام مساحة لون sRGB - Use sRGB color space
};

// فئة Texture - إدارة texture على الـ GPU
// Texture class - manages a GPU texture
class Texture {
public:
    // المُنشئ الافتراضي - Default constructor
    Texture();
    
    // المُنشئ بالمواصفات - Constructor with specifications
    // specs: مواصفات الـ texture - texture specifications
    // data: بيانات البكسلات (اختياري) - pixel data (optional)
    explicit Texture(const TextureSpec& specs, const void* data = nullptr);
    
    // المُدمر - Destructor
    ~Texture();
    
    // منع النسخ (استخدم shared_ptr للمشاركة)
    // Prevent copying (use shared_ptr for sharing)
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    
    // السماح بالنقل - Allow moving
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
    
    // إنشاء texture بمواصفات محددة
    // Create texture with specific specifications
    // specs: مواصفات الـ texture - texture specifications
    // data: بيانات البكسلات (اختياري) - pixel data (optional)
    // returns: نجح الإنشاء - creation succeeded
    bool Create(const TextureSpec& specs, const void* data = nullptr);
    
    // تحديث بيانات الـ texture
    // Update texture data
    // data: البيانات الجديدة - new data
    // x, y: موقع البداية - starting position
    // width, height: أبعاد المنطقة المراد تحديثها - dimensions of area to update
    void SetData(const void* data, u32 x = 0, u32 y = 0, u32 width = 0, u32 height = 0);
    
    // ربط الـ texture بوحدة texture محددة
    // Bind texture to specific texture unit
    // slot: رقم وحدة الـ texture (0-31) - texture unit number (0-31)
    void Bind(u32 slot = 0) const;
    
    // فك ربط الـ texture
    // Unbind texture
    void Unbind() const;
    
    // توليد mipmaps للـ texture
    // Generate mipmaps for texture
    void GenerateMipmaps();
    
    // تحرير موارد الـ texture
    // Release texture resources
    void Destroy();
    
    // التحقق من صحة الـ texture
    // Check if texture is valid
    // returns: الـ texture صالح للاستخدام - texture is valid for use
    bool IsValid() const { return m_rendererID != 0; }
    
    // الحصول على معرّف OpenGL
    // Get OpenGL ID
    // returns: معرّف الـ texture في OpenGL - OpenGL texture ID
    u32 GetRendererID() const { return m_rendererID; }
    
    // الحصول على العرض
    // Get width
    // returns: عرض الـ texture بالبكسل - texture width in pixels
    u32 GetWidth() const { return m_width; }
    
    // الحصول على الارتفاع
    // Get height
    // returns: ارتفاع الـ texture بالبكسل - texture height in pixels
    u32 GetHeight() const { return m_height; }
    
    // الحصول على الصيغة
    // Get format
    // returns: صيغة بيانات الـ texture - texture data format
    TextureFormat GetFormat() const { return m_format; }
    
    // الحصول على المواصفات الكاملة
    // Get complete specifications
    // returns: مواصفات الـ texture - texture specifications
    const TextureSpec& GetSpec() const { return m_spec; }
    
    // تعيين wrapping mode
    // Set wrapping mode
    // wrapS: wrapping على المحور S (X) - S-axis (X) wrapping
    // wrapT: wrapping على المحور T (Y) - T-axis (Y) wrapping
    void SetWrap(TextureWrap wrapS, TextureWrap wrapT);
    
    // تعيين filtering mode
    // Set filtering mode
    // minFilter: تصفية التصغير - minification filter
    // magFilter: تصفية التكبير - magnification filter
    void SetFilter(TextureFilter minFilter, TextureFilter magFilter);
    
    // تعيين لون الحدود (للـ CLAMP_TO_BORDER)
    // Set border color (for CLAMP_TO_BORDER)
    // color: لون الحدود RGBA - border color RGBA
    void SetBorderColor(const Vec4& color);
    
    // إنشاء texture من ملف صورة
    // Create texture from image file
    // filepath: مسار ملف الصورة - image file path
    // generateMipmaps: توليد mipmaps - generate mipmaps
    // sRGB: استخدام مساحة لون sRGB - use sRGB color space
    // returns: shared pointer للـ texture - shared pointer to texture
    static std::shared_ptr<Texture> CreateFromFile(
        const std::string& filepath,
        bool generateMipmaps = true,
        bool sRGB = false
    );
    
    // إنشاء texture أبيض 1x1 (للاستخدام الافتراضي)
    // Create 1x1 white texture (for default use)
    // returns: shared pointer للـ texture - shared pointer to texture
    static std::shared_ptr<Texture> CreateWhite();
    
    // إنشاء texture أسود 1x1
    // Create 1x1 black texture
    // returns: shared pointer للـ texture - shared pointer to texture
    static std::shared_ptr<Texture> CreateBlack();
    
    // إنشاء texture من بيانات في الذاكرة
    // Create texture from raw memory data
    // data: مؤشر للبيانات - pointer to pixel data
    // width, height: أبعاد الـ texture - texture dimensions
    // format: صيغة البيانات - data format
    // minFilter: تصفية التصغير - minification filter
    // wrap: نوع الـ wrapping - wrapping mode
    // returns: shared pointer للـ texture - shared pointer to texture
    static std::shared_ptr<Texture> CreateFromMemory(
        const void* data,
        u32 width, u32 height,
        TextureFormat format,
        TextureFilter minFilter = TextureFilter::LINEAR,
        TextureWrap wrap = TextureWrap::REPEAT
    );
    
    // الحصول على عدد القنوات من الصيغة
    // Get number of channels from format
    // format: صيغة الـ texture - texture format
    // returns: عدد القنوات (1-4) - number of channels (1-4)
    static u32 GetChannelCount(TextureFormat format);
    
    // الحصول على حجم البكسل بالبايتات من الصيغة
    // Get pixel size in bytes from format
    // format: صيغة الـ texture - texture format
    // returns: حجم البكسل بالبايتات - pixel size in bytes
    static u32 GetPixelSize(TextureFormat format);

private:
    // تحويل TextureFormat إلى OpenGL internal format
    // Convert TextureFormat to OpenGL internal format
    static u32 TextureFormatToGLInternalFormat(TextureFormat format);
    
    // تحويل TextureFormat إلى OpenGL format
    // Convert TextureFormat to OpenGL format
    static u32 TextureFormatToGLFormat(TextureFormat format);
    
    // تحويل TextureFormat إلى OpenGL data type
    // Convert TextureFormat to OpenGL data type
    static u32 TextureFormatToGLDataType(TextureFormat format);
    
    // تحويل TextureWrap إلى OpenGL wrap mode
    // Convert TextureWrap to OpenGL wrap mode
    static u32 TextureWrapToGL(TextureWrap wrap);
    
    // تحويل TextureFilter إلى OpenGL filter mode
    // Convert TextureFilter to OpenGL filter mode
    static u32 TextureFilterToGL(TextureFilter filter);

private:
    u32 m_rendererID;           // معرّف OpenGL للـ texture - OpenGL texture ID
    u32 m_width;                // عرض الـ texture - texture width
    u32 m_height;               // ارتفاع الـ texture - texture height
    TextureFormat m_format;     // صيغة البيانات - data format
    TextureSpec m_spec;         // المواصفات الكاملة - complete specifications
};

// أنواع مستعارة للراحة
// Type aliases for convenience
using TextureRef = std::shared_ptr<Texture>;

} // namespace graphics
} // namespace sad
