// image.h - نظام تحميل الصور
// Image loading system
// يوفر واجهة لتحميل الصور من ملفات بصيغ مختلفة (PNG, JPG, BMP, TGA)
// Provides interface for loading images from files in various formats

#pragma once

#include "../core/types.h"
#include "../texture/texture.h"
#include <string>
#include <memory>
#include <vector>

namespace sad {
namespace graphics {

// تعداد صيغ الصور المدعومة
// Enumeration of supported image formats
enum class ImageFormat {
    UNKNOWN = 0,    // صيغة غير معروفة - Unknown format
    PNG,            // PNG format
    JPG,            // JPEG format
    BMP,            // BMP format
    TGA,            // TGA format
    HDR,            // HDR format (high dynamic range)
    PSD,            // Photoshop format
    GIF             // GIF format
};

// فئة Image - تمثيل صورة في الذاكرة
// Image class - represents an image in memory
class Image {
public:
    // المُنشئ الافتراضي - Default constructor
    Image();
    
    // المُنشئ بأبعاد وعدد قنوات محدد
    // Constructor with specific dimensions and channels
    // width: عرض الصورة - image width
    // height: ارتفاع الصورة - image height
    // channels: عدد القنوات (1-4) - number of channels (1-4)
    Image(u32 width, u32 height, u32 channels);
    
    // المُدمر - Destructor
    ~Image();
    
    // منع النسخ (استخدم shared_ptr للمشاركة)
    // Prevent copying (use shared_ptr for sharing)
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    
    // السماح بالنقل - Allow moving
    Image(Image&& other) noexcept;
    Image& operator=(Image&& other) noexcept;
    
    // تحميل صورة من ملف
    // Load image from file
    // filepath: مسار ملف الصورة - image file path
    // desiredChannels: عدد القنوات المطلوبة (0 = تلقائي) - desired channels (0 = automatic)
    // flipVertically: قلب الصورة عمودياً - flip image vertically
    // returns: نجح التحميل - loading succeeded
    bool LoadFromFile(const std::string& filepath, u32 desiredChannels = 0, bool flipVertically = true);
    
    // تحميل صورة من الذاكرة
    // Load image from memory
    // data: مؤشر البيانات - data pointer
    // size: حجم البيانات بالبايتات - data size in bytes
    // desiredChannels: عدد القنوات المطلوبة - desired channels
    // flipVertically: قلب الصورة عمودياً - flip image vertically
    // returns: نجح التحميل - loading succeeded
    bool LoadFromMemory(const u8* data, size_t size, u32 desiredChannels = 0, bool flipVertically = true);
    
    // إنشاء texture من الصورة
    // Create texture from image
    // generateMipmaps: توليد mipmaps - generate mipmaps
    // sRGB: استخدام مساحة لون sRGB - use sRGB color space
    // returns: shared pointer للـ texture - shared pointer to texture
    TextureRef CreateTexture(bool generateMipmaps = true, bool sRGB = false) const;
    
    // تحرير بيانات الصورة
    // Release image data
    void Free();
    
    // التحقق من صحة الصورة
    // Check if image is valid
    // returns: الصورة صالحة - image is valid
    bool IsValid() const { return m_data != nullptr; }
    
    // الحصول على البيانات
    // Get data
    // returns: مؤشر بيانات البكسلات - pixel data pointer
    u8* GetData() { return m_data; }
    const u8* GetData() const { return m_data; }
    
    // الحصول على العرض
    // Get width
    // returns: عرض الصورة بالبكسل - image width in pixels
    u32 GetWidth() const { return m_width; }
    
    // الحصول على الارتفاع
    // Get height
    // returns: ارتفاع الصورة بالبكسل - image height in pixels
    u32 GetHeight() const { return m_height; }
    
    // الحصول على عدد القنوات
    // Get number of channels
    // returns: عدد القنوات (1-4) - number of channels (1-4)
    u32 GetChannels() const { return m_channels; }
    
    // الحصول على حجم البيانات بالبايتات
    // Get data size in bytes
    // returns: حجم بيانات الصورة - image data size
    size_t GetDataSize() const { return m_width * m_height * m_channels; }
    
    // الحصول على صيغة الـ texture المناسبة
    // Get appropriate texture format
    // returns: صيغة الـ texture - texture format
    TextureFormat GetTextureFormat() const;
    
    // قلب الصورة أفقياً
    // Flip image horizontally
    void FlipHorizontal();
    
    // قلب الصورة عمودياً
    // Flip image vertically
    void FlipVertical();
    
    // تدوير الصورة 90 درجة باتجاه عقارب الساعة
    // Rotate image 90 degrees clockwise
    void Rotate90CW();
    
    // تدوير الصورة 90 درجة عكس عقارب الساعة
    // Rotate image 90 degrees counter-clockwise
    void Rotate90CCW();
    
    // تحويل الصورة إلى تدرجات الرمادي
    // Convert image to grayscale
    void ConvertToGrayscale();
    
    // الحصول على لون بكسل محدد
    // Get specific pixel color
    // x, y: إحداثيات البكسل - pixel coordinates
    // returns: لون البكسل (RGBA) - pixel color (RGBA)
    Vec4 GetPixel(u32 x, u32 y) const;
    
    // تعيين لون بكسل محدد
    // Set specific pixel color
    // x, y: إحداثيات البكسل - pixel coordinates
    // color: اللون الجديد (RGBA) - new color (RGBA)
    void SetPixel(u32 x, u32 y, const Vec4& color);
    
    // استخراج صيغة الصورة من اسم الملف
    // Extract image format from filename
    // filepath: مسار الملف - file path
    // returns: صيغة الصورة - image format
    static ImageFormat GetFormatFromPath(const std::string& filepath);
    
    // التحقق من دعم صيغة محددة
    // Check if specific format is supported
    // format: صيغة الصورة - image format
    // returns: الصيغة مدعومة - format is supported
    static bool IsFormatSupported(ImageFormat format);
    
    // إنشاء صورة من ملف وإرجاع shared pointer
    // Create image from file and return shared pointer
    // filepath: مسار ملف الصورة - image file path
    // desiredChannels: عدد القنوات المطلوبة - desired channels
    // flipVertically: قلب الصورة عمودياً - flip image vertically
    // returns: shared pointer للصورة - shared pointer to image
    static std::shared_ptr<Image> CreateFromFile(
        const std::string& filepath,
        u32 desiredChannels = 0,
        bool flipVertically = true
    );
    
    // إنشاء صورة من الذاكرة وإرجاع shared pointer
    // Create image from memory and return shared pointer
    static std::shared_ptr<Image> CreateFromMemory(
        const u8* data,
        size_t size,
        u32 desiredChannels = 0,
        bool flipVertically = true
    );

private:
    u8* m_data;         // بيانات البكسلات - pixel data
    u32 m_width;        // عرض الصورة - image width
    u32 m_height;       // ارتفاع الصورة - image height
    u32 m_channels;     // عدد القنوات - number of channels
};

// أنواع مستعارة للراحة
// Type aliases for convenience
using ImageRef = std::shared_ptr<Image>;

// دالة مساعدة: تحميل texture مباشرة من ملف
// Helper function: load texture directly from file
// filepath: مسار ملف الصورة - image file path
// generateMipmaps: توليد mipmaps - generate mipmaps
// sRGB: استخدام مساحة لون sRGB - use sRGB color space
// returns: shared pointer للـ texture - shared pointer to texture
TextureRef LoadTextureFromFile(
    const std::string& filepath,
    bool generateMipmaps = true,
    bool sRGB = false
);

} // namespace graphics
} // namespace sad
