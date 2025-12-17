// بسم الله الرحمن الرحيم
/**
 * @file image_module.h
 * @brief Image Processing Module - وحدة معالجة الصور
 * 
 * This module provides comprehensive image processing capabilities for Sad Language.
 * هذه الوحدة توفر قدرات معالجة صور شاملة للغة ص
 * 
 * Features / المميزات:
 * - Image loading/saving (PNG, JPEG, BMP) / تحميل/حفظ الصور
 * - Image manipulation (resize, crop, rotate, flip) / معالجة الصور
 * - Color operations (grayscale, invert, brightness, contrast) / عمليات الألوان
 * - Filters (blur, sharpen, edge detection) / الفلاتر
 * - Drawing operations (lines, rectangles, circles, text) / عمليات الرسم
 * - Image format conversion / تحويل صيغ الصور
 * - Histogram operations / عمليات المدرج التكراري
 * - Alpha blending / دمج الشفافية
 * 
 * @author Sad Language Team
 * @date December 2025
 * الحمد لله رب العالمين
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <cstdint>
#include <stdexcept>

namespace sad {
namespace stdlib {
namespace image {

// ============================================================================
// Type Definitions - تعريفات الأنواع
// ============================================================================

/**
 * @brief Image format types - أنواع صيغ الصور
 */
enum class ImageFormat {
    PNG,
    JPEG,
    BMP,
    UNKNOWN
};

/**
 * @brief Color space types - أنواع فضاءات الألوان
 */
enum class ColorSpace {
    RGB,        // Red, Green, Blue
    RGBA,       // RGB with Alpha
    GRAYSCALE,  // Single channel
    BGR,        // Blue, Green, Red (OpenCV format)
    BGRA        // BGR with Alpha
};

/**
 * @brief Interpolation methods - طرق الاستيفاء
 */
enum class InterpolationMethod {
    NEAREST,    // Nearest neighbor
    LINEAR,     // Bilinear
    CUBIC,      // Bicubic
    LANCZOS     // Lanczos resampling
};

/**
 * @brief Flip direction - اتجاه القلب
 */
enum class FlipDirection {
    HORIZONTAL,
    VERTICAL,
    BOTH
};

/**
 * @brief Filter types - أنواع الفلاتر
 */
enum class FilterType {
    BLUR,
    GAUSSIAN_BLUR,
    SHARPEN,
    EDGE_DETECT,
    EMBOSS,
    MEDIAN
};

// ============================================================================
// Color Structure - هيكل اللون
// ============================================================================

/**
 * @brief RGBA color structure
 * هيكل لون RGBA
 */
struct Color {
    uint8_t r{0};    // Red (0-255)
    uint8_t g{0};    // Green (0-255)
    uint8_t b{0};    // Blue (0-255)
    uint8_t a{255};  // Alpha (0-255, 255 = opaque)
    
    Color() = default;
    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}
    
    // Common colors
    static Color Black() { return Color(0, 0, 0); }
    static Color White() { return Color(255, 255, 255); }
    static Color Red() { return Color(255, 0, 0); }
    static Color Green() { return Color(0, 255, 0); }
    static Color Blue() { return Color(0, 0, 255); }
    static Color Yellow() { return Color(255, 255, 0); }
    static Color Cyan() { return Color(0, 255, 255); }
    static Color Magenta() { return Color(255, 0, 255); }
    static Color Transparent() { return Color(0, 0, 0, 0); }
    
    bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
};

/**
 * @brief Rectangle structure
 * هيكل المستطيل
 */
struct Rectangle {
    int x{0};
    int y{0};
    int width{0};
    int height{0};
    
    Rectangle() = default;
    Rectangle(int x_, int y_, int w, int h) : x(x_), y(y_), width(w), height(h) {}
    
    bool contains(int px, int py) const {
        return px >= x && px < x + width && py >= y && py < y + height;
    }
};

/**
 * @brief Point structure
 * هيكل النقطة
 */
struct Point {
    int x{0};
    int y{0};
    
    Point() = default;
    Point(int x_, int y_) : x(x_), y(y_) {}
};

// ============================================================================
// Exception Classes - فئات الاستثناءات
// ============================================================================

/**
 * @brief Image processing exception - استثناء معالجة الصور
 */
class ImageException : public std::runtime_error {
public:
    explicit ImageException(const std::string& message)
        : std::runtime_error(message) {}
};

// ============================================================================
// Image Class - فئة الصورة
// ============================================================================

/**
 * @brief Main image class for image processing
 * الفئة الرئيسية لمعالجة الصور
 */
class Image {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

public:
    // Construction / البناء
    Image();
    Image(int width, int height, ColorSpace colorSpace = ColorSpace::RGBA);
    Image(const Image& other);
    Image(Image&& other) noexcept;
    ~Image();
    
    Image& operator=(const Image& other);
    Image& operator=(Image&& other) noexcept;
    
    // Loading and Saving / التحميل والحفظ
    static Image load(const std::string& filename);
    static Image load(const std::string& filename, ColorSpace colorSpace);
    void save(const std::string& filename) const;
    void save(const std::string& filename, ImageFormat format) const;
    
    // Basic properties / الخصائص الأساسية
    int width() const;
    int height() const;
    ColorSpace color_space() const;
    int channels() const;
    size_t size() const; // Total bytes
    bool empty() const;
    
    // Pixel access / الوصول للبكسلات
    Color get_pixel(int x, int y) const;
    void set_pixel(int x, int y, const Color& color);
    uint8_t* data();
    const uint8_t* data() const;
    
    // Image manipulation / معالجة الصور
    Image resize(int new_width, int new_height, 
                 InterpolationMethod method = InterpolationMethod::LINEAR) const;
    Image crop(int x, int y, int width, int height) const;
    Image crop(const Rectangle& rect) const;
    Image rotate(double angle_degrees) const;
    Image flip(FlipDirection direction) const;
    
    // Color operations / عمليات الألوان
    Image grayscale() const;
    Image invert() const;
    Image adjust_brightness(int delta) const; // -255 to +255
    Image adjust_contrast(double factor) const; // 0.0 to 3.0
    Image adjust_saturation(double factor) const;
    Image adjust_hue(int degrees) const; // -180 to +180
    Image sepia() const;
    
    // Filters / الفلاتر
    Image blur(int radius) const;
    Image gaussian_blur(int radius, double sigma = 0) const;
    Image sharpen(double amount = 1.0) const;
    Image edge_detect() const;
    Image emboss() const;
    Image median_filter(int kernel_size) const;
    
    // Drawing operations / عمليات الرسم
    void draw_line(const Point& p1, const Point& p2, const Color& color, int thickness = 1);
    void draw_rectangle(const Rectangle& rect, const Color& color, int thickness = 1);
    void fill_rectangle(const Rectangle& rect, const Color& color);
    void draw_circle(const Point& center, int radius, const Color& color, int thickness = 1);
    void fill_circle(const Point& center, int radius, const Color& color);
    void draw_ellipse(const Point& center, int radius_x, int radius_y, 
                      const Color& color, int thickness = 1);
    void draw_text(const std::string& text, const Point& position, 
                   const Color& color, int font_size = 16);
    
    // Alpha blending / دمج الشفافية
    Image blend(const Image& other, double alpha = 0.5) const;
    Image overlay(const Image& other, int x, int y) const;
    
    // Format conversion / تحويل الصيغة
    Image convert_color_space(ColorSpace new_color_space) const;
    
    // Histogram / المدرج التكراري
    std::vector<int> histogram(int channel = 0) const; // Returns 256 values
    Image equalize_histogram() const;
    
    // Utility / أدوات مساعدة
    Image clone() const;
    void fill(const Color& color);
    Image extract_channel(int channel_index) const;
    static Image merge_channels(const std::vector<Image>& channels);
    
    // Statistics / إحصائيات
    Color average_color() const;
    double brightness() const;
    bool is_similar(const Image& other, double threshold = 0.95) const;
};

// ============================================================================
// Image Format Detection - اكتشاف صيغة الصورة
// ============================================================================

class ImageFormat {
public:
    static ImageFormat detect(const std::string& filename);
    static ImageFormat from_extension(const std::string& ext);
    static std::string to_extension(ImageFormat format);
    static bool is_supported(const std::string& filename);
};

// ============================================================================
// Image Utilities - أدوات الصور
// ============================================================================

/**
 * @brief Create thumbnail / إنشاء صورة مصغرة
 */
Image create_thumbnail(const Image& source, int max_width, int max_height,
                       bool maintain_aspect = true);

/**
 * @brief Stitch images horizontally / خياطة الصور أفقياً
 */
Image stitch_horizontal(const std::vector<Image>& images);

/**
 * @brief Stitch images vertically / خياطة الصور عمودياً
 */
Image stitch_vertical(const std::vector<Image>& images);

/**
 * @brief Create image grid / إنشاء شبكة صور
 */
Image create_grid(const std::vector<Image>& images, int columns);

/**
 * @brief Apply mask to image / تطبيق قناع على صورة
 */
Image apply_mask(const Image& source, const Image& mask);

/**
 * @brief Extract difference between images / استخراج الفرق بين الصور
 */
Image difference(const Image& img1, const Image& img2);

/**
 * @brief Color interpolation / استيفاء اللون
 */
Color interpolate_color(const Color& c1, const Color& c2, double t);

/**
 * @brief Calculate image similarity / حساب تشابه الصور
 */
double calculate_similarity(const Image& img1, const Image& img2);

/**
 * @brief Get image format name / الحصول على اسم صيغة الصورة
 */
std::string format_to_string(ImageFormat format);

/**
 * @brief Get color space name / الحصول على اسم فضاء اللون
 */
std::string color_space_to_string(ColorSpace colorSpace);

/**
 * @brief Get interpolation method name / الحصول على اسم طريقة الاستيفاء
 */
std::string interpolation_method_to_string(InterpolationMethod method);

} // namespace image
} // namespace stdlib
} // namespace sad

// الحمد لله رب العالمين
