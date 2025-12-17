// بسم الله الرحمن الرحيم
/**
 * @file image_module.cpp
 * @brief Image Processing Module Implementation - تنفيذ وحدة معالجة الصور
 * 
 * الحمد لله رب العالمين
 */

#include "stdlib/image/image_module.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <sstream>

// STB Image libraries (single-header, public domain)
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

namespace sad {
namespace stdlib {
namespace image {

// ============================================================================
// Image Implementation - تنفيذ الصورة
// ============================================================================

struct Image::Impl {
    std::vector<uint8_t> pixels;
    int width{0};
    int height{0};
    ColorSpace color_space{ColorSpace::RGBA};
    
    int get_channels() const {
        switch (color_space) {
            case ColorSpace::GRAYSCALE: return 1;
            case ColorSpace::RGB:
            case ColorSpace::BGR: return 3;
            case ColorSpace::RGBA:
            case ColorSpace::BGRA: return 4;
            default: return 4;
        }
    }
    
    size_t pixel_offset(int x, int y) const {
        return (y * width + x) * get_channels();
    }
};

// ============================================================================
// Construction - البناء
// ============================================================================

Image::Image() : pImpl(std::make_unique<Impl>()) {}

Image::Image(int width, int height, ColorSpace colorSpace)
    : pImpl(std::make_unique<Impl>()) {
    if (width <= 0 || height <= 0) {
        throw ImageException("Invalid image dimensions / أبعاد صورة غير صالحة");
    }
    
    pImpl->width = width;
    pImpl->height = height;
    pImpl->color_space = colorSpace;
    
    int channels = pImpl->get_channels();
    pImpl->pixels.resize(width * height * channels, 0);
}

Image::Image(const Image& other) : pImpl(std::make_unique<Impl>(*other.pImpl)) {}

Image::Image(Image&& other) noexcept = default;

Image::~Image() = default;

Image& Image::operator=(const Image& other) {
    if (this != &other) {
        pImpl = std::make_unique<Impl>(*other.pImpl);
    }
    return *this;
}

Image& Image::operator=(Image&& other) noexcept = default;

// ============================================================================
// Loading and Saving - التحميل والحفظ
// ============================================================================

Image Image::load(const std::string& filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    
    if (!data) {
        throw ImageException("Failed to load image: " + filename + " / فشل تحميل الصورة");
    }
    
    ColorSpace colorSpace;
    switch (channels) {
        case 1: colorSpace = ColorSpace::GRAYSCALE; break;
        case 3: colorSpace = ColorSpace::RGB; break;
        case 4: colorSpace = ColorSpace::RGBA; break;
        default:
            stbi_image_free(data);
            throw ImageException("Unsupported channel count / عدد قنوات غير مدعوم");
    }
    
    Image img(width, height, colorSpace);
    std::memcpy(img.pImpl->pixels.data(), data, width * height * channels);
    
    stbi_image_free(data);
    return img;
}

Image Image::load(const std::string& filename, ColorSpace colorSpace) {
    Image img = load(filename);
    if (img.color_space() != colorSpace) {
        img = img.convert_color_space(colorSpace);
    }
    return img;
}

void Image::save(const std::string& filename) const {
    // Detect format from extension
    std::string ext = filename.substr(filename.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    ImageFormat format = ImageFormat::PNG;
    if (ext == "jpg" || ext == "jpeg") format = ImageFormat::JPEG;
    else if (ext == "bmp") format = ImageFormat::BMP;
    
    save(filename, format);
}

void Image::save(const std::string& filename, ImageFormat format) const {
    if (empty()) {
        throw ImageException("Cannot save empty image / لا يمكن حفظ صورة فارغة");
    }
    
    int channels = pImpl->get_channels();
    const uint8_t* data = pImpl->pixels.data();
    
    int result = 0;
    switch (format) {
        case ImageFormat::PNG:
            result = stbi_write_png(filename.c_str(), pImpl->width, pImpl->height,
                                   channels, data, pImpl->width * channels);
            break;
        case ImageFormat::JPEG:
            result = stbi_write_jpg(filename.c_str(), pImpl->width, pImpl->height,
                                   channels, data, 90); // Quality: 90
            break;
        case ImageFormat::BMP:
            result = stbi_write_bmp(filename.c_str(), pImpl->width, pImpl->height,
                                   channels, data);
            break;
        default:
            throw ImageException("Unsupported format / صيغة غير مدعومة");
    }
    
    if (!result) {
        throw ImageException("Failed to save image / فشل حفظ الصورة");
    }
}

// ============================================================================
// Basic Properties - الخصائص الأساسية
// ============================================================================

int Image::width() const { return pImpl->width; }
int Image::height() const { return pImpl->height; }
ColorSpace Image::color_space() const { return pImpl->color_space; }
int Image::channels() const { return pImpl->get_channels(); }
size_t Image::size() const { return pImpl->pixels.size(); }
bool Image::empty() const { return pImpl->pixels.empty(); }

// ============================================================================
// Pixel Access - الوصول للبكسلات
// ============================================================================

Color Image::get_pixel(int x, int y) const {
    if (x < 0 || x >= pImpl->width || y < 0 || y >= pImpl->height) {
        throw ImageException("Pixel coordinates out of bounds / إحداثيات بكسل خارج الحدود");
    }
    
    size_t offset = pImpl->pixel_offset(x, y);
    const uint8_t* p = &pImpl->pixels[offset];
    
    Color color;
    switch (pImpl->color_space) {
        case ColorSpace::GRAYSCALE:
            color.r = color.g = color.b = p[0];
            color.a = 255;
            break;
        case ColorSpace::RGB:
            color.r = p[0]; color.g = p[1]; color.b = p[2]; color.a = 255;
            break;
        case ColorSpace::RGBA:
            color.r = p[0]; color.g = p[1]; color.b = p[2]; color.a = p[3];
            break;
        case ColorSpace::BGR:
            color.r = p[2]; color.g = p[1]; color.b = p[0]; color.a = 255;
            break;
        case ColorSpace::BGRA:
            color.r = p[2]; color.g = p[1]; color.b = p[0]; color.a = p[3];
            break;
    }
    
    return color;
}

void Image::set_pixel(int x, int y, const Color& color) {
    if (x < 0 || x >= pImpl->width || y < 0 || y >= pImpl->height) {
        return; // Silently ignore out-of-bounds
    }
    
    size_t offset = pImpl->pixel_offset(x, y);
    uint8_t* p = &pImpl->pixels[offset];
    
    switch (pImpl->color_space) {
        case ColorSpace::GRAYSCALE:
            p[0] = static_cast<uint8_t>(0.299 * color.r + 0.587 * color.g + 0.114 * color.b);
            break;
        case ColorSpace::RGB:
            p[0] = color.r; p[1] = color.g; p[2] = color.b;
            break;
        case ColorSpace::RGBA:
            p[0] = color.r; p[1] = color.g; p[2] = color.b; p[3] = color.a;
            break;
        case ColorSpace::BGR:
            p[0] = color.b; p[1] = color.g; p[2] = color.r;
            break;
        case ColorSpace::BGRA:
            p[0] = color.b; p[1] = color.g; p[2] = color.r; p[3] = color.a;
            break;
    }
}

uint8_t* Image::data() { return pImpl->pixels.data(); }
const uint8_t* Image::data() const { return pImpl->pixels.data(); }

// ============================================================================
// Image Manipulation - معالجة الصور
// ============================================================================

Image Image::resize(int new_width, int new_height, InterpolationMethod method) const {
    if (new_width <= 0 || new_height <= 0) {
        throw ImageException("Invalid resize dimensions / أبعاد تغيير حجم غير صالحة");
    }
    
    Image result(new_width, new_height, pImpl->color_space);
    
    double x_ratio = static_cast<double>(pImpl->width) / new_width;
    double y_ratio = static_cast<double>(pImpl->height) / new_height;
    
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            double src_x = x * x_ratio;
            double src_y = y * y_ratio;
            
            if (method == InterpolationMethod::NEAREST) {
                int ix = static_cast<int>(src_x);
                int iy = static_cast<int>(src_y);
                result.set_pixel(x, y, get_pixel(ix, iy));
            } else { // LINEAR (bilinear)
                int x1 = static_cast<int>(src_x);
                int y1 = static_cast<int>(src_y);
                int x2 = std::min(x1 + 1, pImpl->width - 1);
                int y2 = std::min(y1 + 1, pImpl->height - 1);
                
                double dx = src_x - x1;
                double dy = src_y - y1;
                
                Color c11 = get_pixel(x1, y1);
                Color c12 = get_pixel(x1, y2);
                Color c21 = get_pixel(x2, y1);
                Color c22 = get_pixel(x2, y2);
                
                Color c1 = interpolate_color(c11, c21, dx);
                Color c2 = interpolate_color(c12, c22, dx);
                Color final = interpolate_color(c1, c2, dy);
                
                result.set_pixel(x, y, final);
            }
        }
    }
    
    return result;
}

Image Image::crop(int x, int y, int width, int height) const {
    if (x < 0 || y < 0 || x + width > pImpl->width || y + height > pImpl->height) {
        throw ImageException("Crop rectangle out of bounds / مستطيل قص خارج الحدود");
    }
    
    Image result(width, height, pImpl->color_space);
    
    for (int cy = 0; cy < height; cy++) {
        for (int cx = 0; cx < width; cx++) {
            result.set_pixel(cx, cy, get_pixel(x + cx, y + cy));
        }
    }
    
    return result;
}

Image Image::crop(const Rectangle& rect) const {
    return crop(rect.x, rect.y, rect.width, rect.height);
}

Image Image::rotate(double angle_degrees) const {
    double angle = angle_degrees * M_PI / 180.0;
    double cos_a = std::cos(angle);
    double sin_a = std::sin(angle);
    
    int cx = pImpl->width / 2;
    int cy = pImpl->height / 2;
    
    // Calculate new dimensions
    int new_width = static_cast<int>(std::abs(pImpl->width * cos_a) + 
                                     std::abs(pImpl->height * sin_a));
    int new_height = static_cast<int>(std::abs(pImpl->width * sin_a) + 
                                      std::abs(pImpl->height * cos_a));
    
    Image result(new_width, new_height, pImpl->color_space);
    result.fill(Color::Transparent());
    
    int new_cx = new_width / 2;
    int new_cy = new_height / 2;
    
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            int dx = x - new_cx;
            int dy = y - new_cy;
            
            int src_x = static_cast<int>(dx * cos_a + dy * sin_a + cx);
            int src_y = static_cast<int>(-dx * sin_a + dy * cos_a + cy);
            
            if (src_x >= 0 && src_x < pImpl->width && src_y >= 0 && src_y < pImpl->height) {
                result.set_pixel(x, y, get_pixel(src_x, src_y));
            }
        }
    }
    
    return result;
}

Image Image::flip(FlipDirection direction) const {
    Image result(pImpl->width, pImpl->height, pImpl->color_space);
    
    for (int y = 0; y < pImpl->height; y++) {
        for (int x = 0; x < pImpl->width; x++) {
            int src_x = x;
            int src_y = y;
            
            if (direction == FlipDirection::HORIZONTAL || direction == FlipDirection::BOTH) {
                src_x = pImpl->width - 1 - x;
            }
            if (direction == FlipDirection::VERTICAL || direction == FlipDirection::BOTH) {
                src_y = pImpl->height - 1 - y;
            }
            
            result.set_pixel(x, y, get_pixel(src_x, src_y));
        }
    }
    
    return result;
}

// ============================================================================
// Color Operations - عمليات الألوان
// ============================================================================

Image Image::grayscale() const {
    Image result(pImpl->width, pImpl->height, ColorSpace::GRAYSCALE);
    
    for (int y = 0; y < pImpl->height; y++) {
        for (int x = 0; x < pImpl->width; x++) {
            Color c = get_pixel(x, y);
            uint8_t gray = static_cast<uint8_t>(0.299 * c.r + 0.587 * c.g + 0.114 * c.b);
            result.set_pixel(x, y, Color(gray, gray, gray));
        }
    }
    
    return result;
}

Image Image::invert() const {
    Image result = clone();
    
    for (int y = 0; y < pImpl->height; y++) {
        for (int x = 0; x < pImpl->width; x++) {
            Color c = get_pixel(x, y);
            result.set_pixel(x, y, Color(255 - c.r, 255 - c.g, 255 - c.b, c.a));
        }
    }
    
    return result;
}

Image Image::adjust_brightness(int delta) const {
    delta = std::clamp(delta, -255, 255);
    Image result = clone();
    
    for (int y = 0; y < pImpl->height; y++) {
        for (int x = 0; x < pImpl->width; x++) {
            Color c = get_pixel(x, y);
            c.r = std::clamp(c.r + delta, 0, 255);
            c.g = std::clamp(c.g + delta, 0, 255);
            c.b = std::clamp(c.b + delta, 0, 255);
            result.set_pixel(x, y, c);
        }
    }
    
    return result;
}

Image Image::adjust_contrast(double factor) const {
    factor = std::clamp(factor, 0.0, 3.0);
    Image result = clone();
    
    for (int y = 0; y < pImpl->height; y++) {
        for (int x = 0; x < pImpl->width; x++) {
            Color c = get_pixel(x, y);
            c.r = std::clamp(static_cast<int>((c.r - 128) * factor + 128), 0, 255);
            c.g = std::clamp(static_cast<int>((c.g - 128) * factor + 128), 0, 255);
            c.b = std::clamp(static_cast<int>((c.b - 128) * factor + 128), 0, 255);
            result.set_pixel(x, y, c);
        }
    }
    
    return result;
}

Image Image::sepia() const {
    Image result = clone();
    
    for (int y = 0; y < pImpl->height; y++) {
        for (int x = 0; x < pImpl->width; x++) {
            Color c = get_pixel(x, y);
            
            int tr = static_cast<int>(0.393 * c.r + 0.769 * c.g + 0.189 * c.b);
            int tg = static_cast<int>(0.349 * c.r + 0.686 * c.g + 0.168 * c.b);
            int tb = static_cast<int>(0.272 * c.r + 0.534 * c.g + 0.131 * c.b);
            
            c.r = std::clamp(tr, 0, 255);
            c.g = std::clamp(tg, 0, 255);
            c.b = std::clamp(tb, 0, 255);
            
            result.set_pixel(x, y, c);
        }
    }
    
    return result;
}

// ============================================================================
// Filters - الفلاتر
// ============================================================================

Image Image::blur(int radius) const {
    return gaussian_blur(radius, 0);
}

Image Image::gaussian_blur(int radius, double sigma) const {
    if (sigma <= 0) sigma = radius / 3.0;
    
    Image result = clone();
    
    // Simple box blur approximation
    for (int y = 0; y < pImpl->height; y++) {
        for (int x = 0; x < pImpl->width; x++) {
            int r_sum = 0, g_sum = 0, b_sum = 0, count = 0;
            
            for (int ky = -radius; ky <= radius; ky++) {
                for (int kx = -radius; kx <= radius; kx++) {
                    int nx = x + kx;
                    int ny = y + ky;
                    
                    if (nx >= 0 && nx < pImpl->width && ny >= 0 && ny < pImpl->height) {
                        Color c = get_pixel(nx, ny);
                        r_sum += c.r;
                        g_sum += c.g;
                        b_sum += c.b;
                        count++;
                    }
                }
            }
            
            if (count > 0) {
                Color avg(r_sum / count, g_sum / count, b_sum / count);
                result.set_pixel(x, y, avg);
            }
        }
    }
    
    return result;
}

Image Image::sharpen(double amount) const {
    Image result = clone();
    
    // Sharpen kernel
    const int kernel[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };
    
    for (int y = 1; y < pImpl->height - 1; y++) {
        for (int x = 1; x < pImpl->width - 1; x++) {
            int r_sum = 0, g_sum = 0, b_sum = 0;
            
            for (int ky = 0; ky < 3; ky++) {
                for (int kx = 0; kx < 3; kx++) {
                    Color c = get_pixel(x + kx - 1, y + ky - 1);
                    int weight = kernel[ky][kx];
                    r_sum += c.r * weight;
                    g_sum += c.g * weight;
                    b_sum += c.b * weight;
                }
            }
            
            Color sharp(
                std::clamp(r_sum, 0, 255),
                std::clamp(g_sum, 0, 255),
                std::clamp(b_sum, 0, 255)
            );
            
            result.set_pixel(x, y, sharp);
        }
    }
    
    return result;
}

Image Image::edge_detect() const {
    Image result = clone();
    
    // Sobel operator
    const int sobel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    const int sobel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    
    for (int y = 1; y < pImpl->height - 1; y++) {
        for (int x = 1; x < pImpl->width - 1; x++) {
            int gx = 0, gy = 0;
            
            for (int ky = 0; ky < 3; ky++) {
                for (int kx = 0; kx < 3; kx++) {
                    Color c = get_pixel(x + kx - 1, y + ky - 1);
                    int gray = static_cast<int>(0.299 * c.r + 0.587 * c.g + 0.114 * c.b);
                    gx += gray * sobel_x[ky][kx];
                    gy += gray * sobel_y[ky][kx];
                }
            }
            
            int magnitude = static_cast<int>(std::sqrt(gx * gx + gy * gy));
            magnitude = std::clamp(magnitude, 0, 255);
            
            result.set_pixel(x, y, Color(magnitude, magnitude, magnitude));
        }
    }
    
    return result;
}

// ============================================================================
// Drawing Operations - عمليات الرسم
// ============================================================================

void Image::draw_line(const Point& p1, const Point& p2, const Color& color, int thickness) {
    // Bresenham's line algorithm
    int x0 = p1.x, y0 = p1.y;
    int x1 = p2.x, y1 = p2.y;
    
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        set_pixel(x0, y0, color);
        
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
}

void Image::draw_rectangle(const Rectangle& rect, const Color& color, int thickness) {
    // Draw four lines
    draw_line({rect.x, rect.y}, {rect.x + rect.width, rect.y}, color, thickness);
    draw_line({rect.x + rect.width, rect.y}, {rect.x + rect.width, rect.y + rect.height}, color, thickness);
    draw_line({rect.x + rect.width, rect.y + rect.height}, {rect.x, rect.y + rect.height}, color, thickness);
    draw_line({rect.x, rect.y + rect.height}, {rect.x, rect.y}, color, thickness);
}

void Image::fill_rectangle(const Rectangle& rect, const Color& color) {
    for (int y = rect.y; y < rect.y + rect.height; y++) {
        for (int x = rect.x; x < rect.x + rect.width; x++) {
            set_pixel(x, y, color);
        }
    }
}

void Image::draw_circle(const Point& center, int radius, const Color& color, int thickness) {
    // Midpoint circle algorithm
    int x = radius;
    int y = 0;
    int err = 0;
    
    while (x >= y) {
        set_pixel(center.x + x, center.y + y, color);
        set_pixel(center.x + y, center.y + x, color);
        set_pixel(center.x - y, center.y + x, color);
        set_pixel(center.x - x, center.y + y, color);
        set_pixel(center.x - x, center.y - y, color);
        set_pixel(center.x - y, center.y - x, color);
        set_pixel(center.x + y, center.y - x, color);
        set_pixel(center.x + x, center.y - y, color);
        
        if (err <= 0) {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void Image::fill_circle(const Point& center, int radius, const Color& color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                set_pixel(center.x + x, center.y + y, color);
            }
        }
    }
}

// ============================================================================
// Utility Functions - الدوال المساعدة
// ============================================================================

Image Image::clone() const {
    return Image(*this);
}

void Image::fill(const Color& color) {
    for (int y = 0; y < pImpl->height; y++) {
        for (int x = 0; x < pImpl->width; x++) {
            set_pixel(x, y, color);
        }
    }
}

Color Image::average_color() const {
    uint64_t r_sum = 0, g_sum = 0, b_sum = 0;
    int count = pImpl->width * pImpl->height;
    
    for (int y = 0; y < pImpl->height; y++) {
        for (int x = 0; x < pImpl->width; x++) {
            Color c = get_pixel(x, y);
            r_sum += c.r;
            g_sum += c.g;
            b_sum += c.b;
        }
    }
    
    return Color(r_sum / count, g_sum / count, b_sum / count);
}

// ============================================================================
// Helper Functions - دوال مساعدة
// ============================================================================

Color interpolate_color(const Color& c1, const Color& c2, double t) {
    t = std::clamp(t, 0.0, 1.0);
    return Color(
        static_cast<uint8_t>(c1.r + (c2.r - c1.r) * t),
        static_cast<uint8_t>(c1.g + (c2.g - c1.g) * t),
        static_cast<uint8_t>(c1.b + (c2.b - c1.b) * t),
        static_cast<uint8_t>(c1.a + (c2.a - c1.a) * t)
    );
}

Image create_thumbnail(const Image& source, int max_width, int max_height, bool maintain_aspect) {
    int width = source.width();
    int height = source.height();
    
    if (maintain_aspect) {
        double ratio = std::min(
            static_cast<double>(max_width) / width,
            static_cast<double>(max_height) / height
        );
        width = static_cast<int>(width * ratio);
        height = static_cast<int>(height * ratio);
    } else {
        width = max_width;
        height = max_height;
    }
    
    return source.resize(width, height);
}

std::string format_to_string(ImageFormat format) {
    switch (format) {
        case ImageFormat::PNG: return "PNG";
        case ImageFormat::JPEG: return "JPEG";
        case ImageFormat::BMP: return "BMP";
        default: return "Unknown";
    }
}

std::string color_space_to_string(ColorSpace colorSpace) {
    switch (colorSpace) {
        case ColorSpace::GRAYSCALE: return "Grayscale";
        case ColorSpace::RGB: return "RGB";
        case ColorSpace::RGBA: return "RGBA";
        case ColorSpace::BGR: return "BGR";
        case ColorSpace::BGRA: return "BGRA";
        default: return "Unknown";
    }
}

} // namespace image
} // namespace stdlib
} // namespace sad

// الحمد لله رب العالمين
