// image.cpp - تنفيذ نظام تحميل الصور
// Implementation of image loading system

#include "../../include/texture/image.h"

// تضمين stb_image - include stb_image
#define STB_IMAGE_IMPLEMENTATION
#include "../../third_party/stb_image.h"

#include <iostream>
#include <algorithm>

namespace sad {
namespace graphics {

// المُنشئ الافتراضي - Default constructor
Image::Image()
    : m_data(nullptr)   // بدون بيانات - no data
    , m_width(0)        // بدون عرض - no width
    , m_height(0)       // بدون ارتفاع - no height
    , m_channels(0)     // بدون قنوات - no channels
    , m_ownedByStb(false) // ذاكرة غير مخصصة - no allocated memory
{
}

// المُنشئ بأبعاد محددة - Constructor with dimensions
Image::Image(u32 width, u32 height, u32 channels)
    : m_width(width)
    , m_height(height)
    , m_channels(channels)
    , m_ownedByStb(false) // ذاكرة مخصصة ب new[] - memory allocated with new[]
{
    // تخصيص الذاكرة للبيانات - allocate memory for data
    size_t size = width * height * channels;
    m_data = new u8[size];
    
    // تصفير البيانات - zero out data
    std::memset(m_data, 0, size);
}

// المُدمر - Destructor
Image::~Image() {
    Free(); // تحرير البيانات - release data
}

// مُنشئ النقل - Move constructor
Image::Image(Image&& other) noexcept
    : m_data(other.m_data)          // نقل المؤشر - move pointer
    , m_width(other.m_width)        // نقل العرض - move width
    , m_height(other.m_height)      // نقل الارتفاع - move height
    , m_channels(other.m_channels)  // نقل عدد القنوات - move channels
    , m_ownedByStb(other.m_ownedByStb) // نقل علامة الملكية - move ownership flag
{
    // إبطال الكائن المصدر - invalidate source object
    other.m_data = nullptr;
    other.m_width = 0;
    other.m_height = 0;
    other.m_channels = 0;
    other.m_ownedByStb = false;
}

// عامل إسناد النقل - Move assignment operator
Image& Image::operator=(Image&& other) noexcept {
    if (this != &other) { // تجنب الإسناد الذاتي - avoid self-assignment
        Free(); // تحرير البيانات الحالية - release current data
        
        // نقل البيانات - move data
        m_data = other.m_data;
        m_width = other.m_width;
        m_height = other.m_height;
        m_channels = other.m_channels;
        m_ownedByStb = other.m_ownedByStb;
        
        // إبطال الكائن المصدر - invalidate source object
        other.m_data = nullptr;
        other.m_width = 0;
        other.m_height = 0;
        other.m_channels = 0;
        other.m_ownedByStb = false;
    }
    return *this;
}

// تحميل صورة من ملف
// Load image from file
bool Image::LoadFromFile(const std::string& filepath, u32 desiredChannels, bool flipVertically) {
    // تحرير البيانات القديمة إن وُجدت - free old data if exists
    Free();
    
    // تعيين اتجاه القلب - set flip direction
    stbi_set_flip_vertically_on_load(flipVertically ? 1 : 0);
    
    // تحميل الصورة باستخدام stb_image - load image using stb_image
    int width, height, channels;
    m_data = stbi_load(
        filepath.c_str(),               // مسار الملف - file path
        &width,                         // العرض - width
        &height,                        // الارتفاع - height
        &channels,                      // عدد القنوات - channels
        desiredChannels                 // القنوات المطلوبة (0 = تلقائي) - desired channels (0 = auto)
    );
    
    // التحقق من نجاح التحميل - check if loading succeeded
    if (!m_data) {
        std::cerr << "Image::LoadFromFile: Failed to load image: " << filepath << std::endl;
        std::cerr << "  Reason: " << stbi_failure_reason() << std::endl;
        return false;
    }
    
    // حفظ المعلومات - save information
    m_width = static_cast<u32>(width);
    m_height = static_cast<u32>(height);
    m_channels = desiredChannels > 0 ? desiredChannels : static_cast<u32>(channels);
    m_ownedByStb = true; // stb_image خصصت الذاكرة - stb_image allocated memory
    
    std::cout << "Image::LoadFromFile: Successfully loaded " << filepath << std::endl;
    std::cout << "  Dimensions: " << m_width << "x" << m_height << std::endl;
    std::cout << "  Channels: " << m_channels << std::endl;
    
    return true; // نجح التحميل - loading succeeded
}

// تحميل صورة من الذاكرة
// Load image from memory
bool Image::LoadFromMemory(const u8* data, size_t size, u32 desiredChannels, bool flipVertically) {
    // التحقق من صحة البيانات - validate data
    if (!data || size == 0) {
        std::cerr << "Image::LoadFromMemory: Invalid data or size" << std::endl;
        return false;
    }
    
    // تحرير البيانات القديمة - free old data
    Free();
    
    // تعيين اتجاه القلب - set flip direction
    stbi_set_flip_vertically_on_load(flipVertically ? 1 : 0);
    
    // تحميل الصورة من الذاكرة - load image from memory
    int width, height, channels;
    m_data = stbi_load_from_memory(
        data,                           // مؤشر البيانات - data pointer
        static_cast<int>(size),         // حجم البيانات - data size
        &width,                         // العرض - width
        &height,                        // الارتفاع - height
        &channels,                      // عدد القنوات - channels
        desiredChannels                 // القنوات المطلوبة - desired channels
    );
    
    // التحقق من نجاح التحميل - check if loading succeeded
    if (!m_data) {
        std::cerr << "Image::LoadFromMemory: Failed to load image from memory" << std::endl;
        std::cerr << "  Reason: " << stbi_failure_reason() << std::endl;
        return false;
    }
    
    // حفظ المعلومات - save information
    m_width = static_cast<u32>(width);
    m_height = static_cast<u32>(height);
    m_channels = desiredChannels > 0 ? desiredChannels : static_cast<u32>(channels);
    m_ownedByStb = true; // stb_image خصصت الذاكرة - stb_image allocated memory
    
    return true; // نجح التحميل - loading succeeded
}

// إنشاء texture من الصورة
// Create texture from image
TextureRef Image::CreateTexture(bool generateMipmaps, bool sRGB) const {
    if (!IsValid()) { // التحقق من صحة الصورة - validate image
        std::cerr << "Image::CreateTexture: Invalid image data" << std::endl;
        return nullptr;
    }
    
    // إنشاء مواصفات الـ texture - create texture specifications
    TextureSpec spec;
    spec.width = m_width;
    spec.height = m_height;
    spec.format = GetTextureFormat();
    spec.generateMipmaps = generateMipmaps;
    spec.sRGB = sRGB;
    
    // إنشاء الـ texture - create texture
    auto texture = std::make_shared<Texture>(spec, m_data);
    
    return texture;
}

// تحرير بيانات الصورة
// Release image data
void Image::Free() {
    if (m_data) { // إذا كانت البيانات موجودة - if data exists
        if (m_ownedByStb) {
            stbi_image_free(m_data); // تحرير بواسطة stb - free using stb
        } else {
            delete[] m_data; // تحرير بواسطة delete[] - free using delete[]
        }
        m_data = nullptr;
        m_width = 0;
        m_height = 0;
        m_channels = 0;
        m_ownedByStb = false;
    }
}

// الحصول على صيغة الـ texture المناسبة
// Get appropriate texture format
TextureFormat Image::GetTextureFormat() const {
    switch (m_channels) {
        case 1: return TextureFormat::R8;      // قناة واحدة - one channel
        case 2: return TextureFormat::RG8;     // قناتان - two channels
        case 3: return TextureFormat::RGB8;    // ثلاث قنوات - three channels
        case 4: return TextureFormat::RGBA8;   // أربع قنوات - four channels
        default: return TextureFormat::UNKNOWN; // غير معروف - unknown
    }
}

// قلب الصورة أفقياً
// Flip image horizontally
void Image::FlipHorizontal() {
    if (!IsValid()) return; // التحقق من الصحة - validate
    
    // قلب كل صف - flip each row
    for (u32 y = 0; y < m_height; ++y) {
        u8* row = m_data + (y * m_width * m_channels);
        
        // تبديل البكسلات من البداية والنهاية - swap pixels from start and end
        for (u32 x = 0; x < m_width / 2; ++x) {
            u32 left = x * m_channels;
            u32 right = (m_width - 1 - x) * m_channels;
            
            // تبديل كل قناة - swap each channel
            for (u32 c = 0; c < m_channels; ++c) {
                std::swap(row[left + c], row[right + c]);
            }
        }
    }
}

// قلب الصورة عمودياً
// Flip image vertically
void Image::FlipVertical() {
    if (!IsValid()) return; // التحقق من الصحة - validate
    
    size_t rowSize = m_width * m_channels; // حجم الصف - row size
    u8* tempRow = new u8[rowSize]; // صف مؤقت - temporary row
    
    // تبديل الصفوف من الأعلى والأسفل - swap rows from top and bottom
    for (u32 y = 0; y < m_height / 2; ++y) {
        u8* topRow = m_data + (y * rowSize);
        u8* bottomRow = m_data + ((m_height - 1 - y) * rowSize);
        
        // نسخ الصف العلوي للمؤقت - copy top row to temp
        std::memcpy(tempRow, topRow, rowSize);
        
        // نسخ الصف السفلي للعلوي - copy bottom row to top
        std::memcpy(topRow, bottomRow, rowSize);
        
        // نسخ المؤقت للسفلي - copy temp to bottom
        std::memcpy(bottomRow, tempRow, rowSize);
    }
    
    delete[] tempRow; // تحرير الصف المؤقت - free temporary row
}

// تدوير الصورة 90 درجة باتجاه عقارب الساعة
// Rotate image 90 degrees clockwise
void Image::Rotate90CW() {
    if (!IsValid()) return; // التحقق من الصحة - validate
    
    // إنشاء بيانات جديدة بأبعاد معكوسة - create new data with swapped dimensions
    u32 newWidth = m_height;
    u32 newHeight = m_width;
    u8* newData = new u8[newWidth * newHeight * m_channels];
    
    // نسخ البيانات مع التدوير - copy data with rotation
    for (u32 y = 0; y < m_height; ++y) {
        for (u32 x = 0; x < m_width; ++x) {
            u32 srcIndex = (y * m_width + x) * m_channels;
            u32 dstIndex = ((newHeight - 1 - x) * newWidth + y) * m_channels;
            
            // نسخ كل قناة - copy each channel
            for (u32 c = 0; c < m_channels; ++c) {
                newData[dstIndex + c] = m_data[srcIndex + c];
            }
        }
    }
    
    // استبدال البيانات القديمة - replace old data
    Free(); // تحرير آمن باستخدام العلامة الصحيحة - safe free using correct flag
    m_data = newData;
    m_width = newWidth;
    m_height = newHeight;
    m_ownedByStb = false; // new[] خصصت الذاكرة - new[] allocated memory
}

// تدوير الصورة 90 درجة عكس عقارب الساعة
// Rotate image 90 degrees counter-clockwise
void Image::Rotate90CCW() {
    if (!IsValid()) return; // التحقق من الصحة - validate
    
    // إنشاء بيانات جديدة - create new data
    u32 newWidth = m_height;
    u32 newHeight = m_width;
    u8* newData = new u8[newWidth * newHeight * m_channels];
    
    // نسخ البيانات مع التدوير - copy data with rotation
    for (u32 y = 0; y < m_height; ++y) {
        for (u32 x = 0; x < m_width; ++x) {
            u32 srcIndex = (y * m_width + x) * m_channels;
            u32 dstIndex = (x * newWidth + (newWidth - 1 - y)) * m_channels;
            
            // نسخ كل قناة - copy each channel
            for (u32 c = 0; c < m_channels; ++c) {
                newData[dstIndex + c] = m_data[srcIndex + c];
            }
        }
    }
    
    // استبدال البيانات - replace data
    Free(); // تحرير آمن - safe free
    m_data = newData;
    m_width = newWidth;
    m_height = newHeight;
    m_ownedByStb = false; // new[] خصصت الذاكرة - new[] allocated memory
}

// تحويل الصورة إلى تدرجات الرمادي
// Convert image to grayscale
void Image::ConvertToGrayscale() {
    if (!IsValid() || m_channels < 3) return; // التحقق - validate
    
    // إنشاء بيانات جديدة بقناة واحدة - create new data with single channel
    u8* newData = new u8[m_width * m_height];
    
    // تحويل كل بكسل - convert each pixel
    for (u32 i = 0; i < m_width * m_height; ++i) {
        u32 srcIndex = i * m_channels;
        
        // معادلة تدرج الرمادي: 0.299*R + 0.587*G + 0.114*B
        // Grayscale formula: 0.299*R + 0.587*G + 0.114*B
        float r = m_data[srcIndex] / 255.0f;
        float g = m_data[srcIndex + 1] / 255.0f;
        float b = m_data[srcIndex + 2] / 255.0f;
        
        float gray = 0.299f * r + 0.587f * g + 0.114f * b;
        newData[i] = static_cast<u8>(gray * 255.0f);
    }
    
    // استبدال البيانات - replace data
    Free(); // تحرير آمن - safe free
    m_data = newData;
    m_channels = 1;
    m_ownedByStb = false; // new[] خصصت الذاكرة - new[] allocated memory
}

// الحصول على لون بكسل محدد
// Get specific pixel color
Vec4 Image::GetPixel(u32 x, u32 y) const {
    // التحقق من الحدود - validate bounds
    if (!IsValid() || x >= m_width || y >= m_height) {
        return Vec4(0.0f, 0.0f, 0.0f, 1.0f); // أسود افتراضي - default black
    }
    
    // حساب الفهرس - calculate index
    u32 index = (y * m_width + x) * m_channels;
    
    // قراءة القنوات - read channels
    Vec4 color(0.0f, 0.0f, 0.0f, 1.0f);
    
    if (m_channels >= 1) color.x = m_data[index] / 255.0f;         // أحمر - red
    if (m_channels >= 2) color.y = m_data[index + 1] / 255.0f;     // أخضر - green
    if (m_channels >= 3) color.z = m_data[index + 2] / 255.0f;     // أزرق - blue
    if (m_channels >= 4) color.w = m_data[index + 3] / 255.0f;     // شفافية - alpha
    
    return color;
}

// تعيين لون بكسل محدد
// Set specific pixel color
void Image::SetPixel(u32 x, u32 y, const Vec4& color) {
    // التحقق من الحدود - validate bounds
    if (!IsValid() || x >= m_width || y >= m_height) {
        return;
    }
    
    // حساب الفهرس - calculate index
    u32 index = (y * m_width + x) * m_channels;
    
    // كتابة القنوات - write channels
    if (m_channels >= 1) m_data[index] = static_cast<u8>(color.x * 255.0f);
    if (m_channels >= 2) m_data[index + 1] = static_cast<u8>(color.y * 255.0f);
    if (m_channels >= 3) m_data[index + 2] = static_cast<u8>(color.z * 255.0f);
    if (m_channels >= 4) m_data[index + 3] = static_cast<u8>(color.w * 255.0f);
}

// استخراج صيغة الصورة من اسم الملف
// Extract image format from filename
ImageFormat Image::GetFormatFromPath(const std::string& filepath) {
    // البحث عن آخر نقطة - find last dot
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos == std::string::npos) {
        return ImageFormat::UNKNOWN; // لا توجد نقطة - no dot found
    }
    
    // استخراج الامتداد - extract extension
    std::string ext = filepath.substr(dotPos + 1);
    
    // تحويل إلى أحرف صغيرة - convert to lowercase
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    // المطابقة - match
    if (ext == "png") return ImageFormat::PNG;
    if (ext == "jpg" || ext == "jpeg") return ImageFormat::JPG;
    if (ext == "bmp") return ImageFormat::BMP;
    if (ext == "tga") return ImageFormat::TGA;
    if (ext == "hdr") return ImageFormat::HDR;
    if (ext == "psd") return ImageFormat::PSD;
    if (ext == "gif") return ImageFormat::GIF;
    
    return ImageFormat::UNKNOWN; // غير معروف - unknown
}

// التحقق من دعم صيغة محددة
// Check if specific format is supported
bool Image::IsFormatSupported(ImageFormat format) {
    switch (format) {
        case ImageFormat::PNG:
        case ImageFormat::JPG:
        case ImageFormat::BMP:
        case ImageFormat::TGA:
        case ImageFormat::HDR:
        case ImageFormat::PSD:
        case ImageFormat::GIF:
            return true; // مدعوم - supported
        default:
            return false; // غير مدعوم - not supported
    }
}

// إنشاء صورة من ملف
// Create image from file
std::shared_ptr<Image> Image::CreateFromFile(
    const std::string& filepath,
    u32 desiredChannels,
    bool flipVertically)
{
    auto image = std::make_shared<Image>(); // إنشاء صورة جديدة - create new image
    
    // محاولة تحميل الصورة - attempt to load image
    if (!image->LoadFromFile(filepath, desiredChannels, flipVertically)) {
        return nullptr; // فشل التحميل - loading failed
    }
    
    return image; // إرجاع الصورة - return image
}

// إنشاء صورة من الذاكرة
// Create image from memory
std::shared_ptr<Image> Image::CreateFromMemory(
    const u8* data,
    size_t size,
    u32 desiredChannels,
    bool flipVertically)
{
    auto image = std::make_shared<Image>(); // إنشاء صورة جديدة - create new image
    
    // محاولة تحميل الصورة - attempt to load image
    if (!image->LoadFromMemory(data, size, desiredChannels, flipVertically)) {
        return nullptr; // فشل التحميل - loading failed
    }
    
    return image; // إرجاع الصورة - return image
}

// دالة مساعدة: تحميل texture مباشرة من ملف
// Helper function: load texture directly from file
TextureRef LoadTextureFromFile(
    const std::string& filepath,
    bool generateMipmaps,
    bool sRGB)
{
    // تحميل الصورة - load image
    auto image = Image::CreateFromFile(filepath, 0, true);
    if (!image) {
        std::cerr << "LoadTextureFromFile: Failed to load image: " << filepath << std::endl;
        return nullptr;
    }
    
    // إنشاء texture من الصورة - create texture from image
    auto texture = image->CreateTexture(generateMipmaps, sRGB);
    
    // لا حاجة لتحرير الصورة يدوياً - Image destructor will handle cleanup
    
    return texture;
}

} // namespace graphics
} // namespace sad
