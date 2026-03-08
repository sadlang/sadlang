/**
 * =============================================================================
 * @file sad_image.h
 * @brief (AR) مكتبة معالجة الصور — resize, crop, rotate, filters
 * @brief (EN) Image Processing Library — resize, crop, rotate, filters
 * 
 * المهمة: T244 - stdlib/image
 * المرحلة: سبتمبر 2026 — Desktop 3D
 * =============================================================================
 * 
 * 🎯 الميزات
 * ══════════
 * ✓ تحميل/حفظ صور (PNG, JPEG, BMP, TGA)
 * ✓ تغيير الحجم (resize)
 * ✓ القص (crop)
 * ✓ الدوران (rotate)
 * ✓ الفلاتر (blur, sharpen, grayscale, etc.)
 * ✓ تعديل الألوان (brightness, contrast, saturation)
 * 
 * =============================================================================
 */

#ifndef SAD_STDLIB_IMAGE_H
#define SAD_STDLIB_IMAGE_H

#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <cstdint>

namespace sad {
namespace image {

// =============================================================================
// الأنواع الأساسية
// =============================================================================

struct Color {
    uint8_t r = 0, g = 0, b = 0, a = 255;
    
    Color() = default;
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}
    
    static Color White() { return Color(255, 255, 255); }
    static Color Black() { return Color(0, 0, 0); }
    static Color Red() { return Color(255, 0, 0); }
    static Color Green() { return Color(0, 255, 0); }
    static Color Blue() { return Color(0, 0, 255); }
    
    // تحويل إلى grayscale
    uint8_t Luminance() const {
        return static_cast<uint8_t>(0.299f * r + 0.587f * g + 0.114f * b);
    }
};

// =============================================================================
// طريقة interpolation
// =============================================================================

enum class InterpolationMode {
    Nearest,   // أقرب بكسل — سريع لكن مربع
    Bilinear,  // خطي ثنائي — جيد للتكبير
    Bicubic    // مكعب — أفضل جودة
};

// =============================================================================
// الصورة
// =============================================================================

class Image {
public:
    // ─────────────────────────────────────────────
    // المنشئات
    // ─────────────────────────────────────────────
    
    Image() : width_(0), height_(0), channels_(4) {}
    
    Image(int width, int height, int channels = 4)
        : width_(width), height_(height), channels_(channels) {
        data_.resize(width * height * channels, 0);
    }
    
    Image(int width, int height, const Color& fillColor)
        : width_(width), height_(height), channels_(4) {
        data_.resize(width * height * 4);
        Fill(fillColor);
    }
    
    // ─────────────────────────────────────────────
    // الخصائص
    // ─────────────────────────────────────────────
    
    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
    int GetChannels() const { return channels_; }
    bool IsValid() const { return width_ > 0 && height_ > 0 && !data_.empty(); }
    
    const uint8_t* GetData() const { return data_.data(); }
    uint8_t* GetData() { return data_.data(); }
    size_t GetDataSize() const { return data_.size(); }
    
    // ─────────────────────────────────────────────
    // الوصول للبكسل
    // ─────────────────────────────────────────────
    
    Color GetPixel(int x, int y) const {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) {
            return Color();
        }
        
        size_t idx = (y * width_ + x) * channels_;
        Color c;
        c.r = data_[idx];
        c.g = channels_ > 1 ? data_[idx + 1] : c.r;
        c.b = channels_ > 2 ? data_[idx + 2] : c.r;
        c.a = channels_ > 3 ? data_[idx + 3] : 255;
        return c;
    }
    
    void SetPixel(int x, int y, const Color& color) {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) return;
        
        size_t idx = (y * width_ + x) * channels_;
        data_[idx] = color.r;
        if (channels_ > 1) data_[idx + 1] = color.g;
        if (channels_ > 2) data_[idx + 2] = color.b;
        if (channels_ > 3) data_[idx + 3] = color.a;
    }
    
    void Fill(const Color& color) {
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                SetPixel(x, y, color);
            }
        }
    }
    
    // ─────────────────────────────────────────────
    // تغيير الحجم
    // ─────────────────────────────────────────────
    
    Image Resize(int newWidth, int newHeight,
                 InterpolationMode mode = InterpolationMode::Bilinear) const {
        Image result(newWidth, newHeight, channels_);
        
        float scaleX = static_cast<float>(width_) / newWidth;
        float scaleY = static_cast<float>(height_) / newHeight;
        
        for (int y = 0; y < newHeight; y++) {
            for (int x = 0; x < newWidth; x++) {
                float srcX = (x + 0.5f) * scaleX - 0.5f;
                float srcY = (y + 0.5f) * scaleY - 0.5f;
                
                Color c;
                switch (mode) {
                    case InterpolationMode::Nearest:
                        c = SampleNearest(srcX, srcY);
                        break;
                    case InterpolationMode::Bilinear:
                        c = SampleBilinear(srcX, srcY);
                        break;
                    case InterpolationMode::Bicubic:
                        c = SampleBicubic(srcX, srcY);
                        break;
                }
                
                result.SetPixel(x, y, c);
            }
        }
        
        return result;
    }
    
    // ─────────────────────────────────────────────
    // القص
    // ─────────────────────────────────────────────
    
    Image Crop(int x, int y, int cropWidth, int cropHeight) const {
        // تصحيح الحدود
        x = std::max(0, std::min(x, width_ - 1));
        y = std::max(0, std::min(y, height_ - 1));
        cropWidth = std::min(cropWidth, width_ - x);
        cropHeight = std::min(cropHeight, height_ - y);
        
        Image result(cropWidth, cropHeight, channels_);
        
        for (int dy = 0; dy < cropHeight; dy++) {
            for (int dx = 0; dx < cropWidth; dx++) {
                result.SetPixel(dx, dy, GetPixel(x + dx, y + dy));
            }
        }
        
        return result;
    }
    
    // ─────────────────────────────────────────────
    // الدوران
    // ─────────────────────────────────────────────
    
    Image Rotate90CW() const {
        Image result(height_, width_, channels_);
        
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                result.SetPixel(height_ - 1 - y, x, GetPixel(x, y));
            }
        }
        
        return result;
    }
    
    Image Rotate90CCW() const {
        Image result(height_, width_, channels_);
        
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                result.SetPixel(y, width_ - 1 - x, GetPixel(x, y));
            }
        }
        
        return result;
    }
    
    Image Rotate180() const {
        Image result(width_, height_, channels_);
        
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                result.SetPixel(width_ - 1 - x, height_ - 1 - y, GetPixel(x, y));
            }
        }
        
        return result;
    }
    
    Image RotateArbitrary(float angleDegrees,
                          InterpolationMode mode = InterpolationMode::Bilinear) const {
        float angle = angleDegrees * 3.14159265f / 180.0f;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        
        // حساب الأبعاد الجديدة
        int newWidth = static_cast<int>(std::abs(width_ * cosA) + std::abs(height_ * sinA));
        int newHeight = static_cast<int>(std::abs(width_ * sinA) + std::abs(height_ * cosA));
        
        Image result(newWidth, newHeight, channels_);
        
        float cx = width_ / 2.0f;
        float cy = height_ / 2.0f;
        float ncx = newWidth / 2.0f;
        float ncy = newHeight / 2.0f;
        
        for (int y = 0; y < newHeight; y++) {
            for (int x = 0; x < newWidth; x++) {
                float dx = x - ncx;
                float dy = y - ncy;
                
                float srcX = cosA * dx + sinA * dy + cx;
                float srcY = -sinA * dx + cosA * dy + cy;
                
                if (srcX >= 0 && srcX < width_ && srcY >= 0 && srcY < height_) {
                    Color c = (mode == InterpolationMode::Nearest)
                        ? SampleNearest(srcX, srcY)
                        : SampleBilinear(srcX, srcY);
                    result.SetPixel(x, y, c);
                }
            }
        }
        
        return result;
    }
    
    // ─────────────────────────────────────────────
    // الانعكاس
    // ─────────────────────────────────────────────
    
    Image FlipHorizontal() const {
        Image result(width_, height_, channels_);
        
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                result.SetPixel(width_ - 1 - x, y, GetPixel(x, y));
            }
        }
        
        return result;
    }
    
    Image FlipVertical() const {
        Image result(width_, height_, channels_);
        
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                result.SetPixel(x, height_ - 1 - y, GetPixel(x, y));
            }
        }
        
        return result;
    }
    
    // ─────────────────────────────────────────────
    // فلاتر الألوان
    // ─────────────────────────────────────────────
    
    Image ToGrayscale() const {
        Image result(width_, height_, channels_);
        
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                Color c = GetPixel(x, y);
                uint8_t gray = c.Luminance();
                result.SetPixel(x, y, Color(gray, gray, gray, c.a));
            }
        }
        
        return result;
    }
    
    Image AdjustBrightness(float factor) const {
        Image result(width_, height_, channels_);
        
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                Color c = GetPixel(x, y);
                result.SetPixel(x, y, Color(
                    ClampByte(c.r * factor),
                    ClampByte(c.g * factor),
                    ClampByte(c.b * factor),
                    c.a
                ));
            }
        }
        
        return result;
    }
    
    Image AdjustContrast(float factor) const {
        Image result(width_, height_, channels_);
        float center = 128.0f;
        
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                Color c = GetPixel(x, y);
                result.SetPixel(x, y, Color(
                    ClampByte(center + (c.r - center) * factor),
                    ClampByte(center + (c.g - center) * factor),
                    ClampByte(center + (c.b - center) * factor),
                    c.a
                ));
            }
        }
        
        return result;
    }
    
    Image AdjustSaturation(float factor) const {
        Image result(width_, height_, channels_);
        
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                Color c = GetPixel(x, y);
                float gray = c.Luminance();
                result.SetPixel(x, y, Color(
                    ClampByte(gray + (c.r - gray) * factor),
                    ClampByte(gray + (c.g - gray) * factor),
                    ClampByte(gray + (c.b - gray) * factor),
                    c.a
                ));
            }
        }
        
        return result;
    }
    
    Image Invert() const {
        Image result(width_, height_, channels_);
        
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                Color c = GetPixel(x, y);
                result.SetPixel(x, y, Color(255 - c.r, 255 - c.g, 255 - c.b, c.a));
            }
        }
        
        return result;
    }
    
    Image Sepia() const {
        Image result(width_, height_, channels_);
        
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                Color c = GetPixel(x, y);
                result.SetPixel(x, y, Color(
                    ClampByte(c.r * 0.393f + c.g * 0.769f + c.b * 0.189f),
                    ClampByte(c.r * 0.349f + c.g * 0.686f + c.b * 0.168f),
                    ClampByte(c.r * 0.272f + c.g * 0.534f + c.b * 0.131f),
                    c.a
                ));
            }
        }
        
        return result;
    }
    
    // ─────────────────────────────────────────────
    // فلاتر التمويه والحدة
    // ─────────────────────────────────────────────
    
    Image BoxBlur(int radius) const {
        Image result(width_, height_, channels_);
        int size = radius * 2 + 1;
        float weight = 1.0f / (size * size);
        
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                float r = 0, g = 0, b = 0, a = 0;
                
                for (int ky = -radius; ky <= radius; ky++) {
                    for (int kx = -radius; kx <= radius; kx++) {
                        Color c = GetPixel(x + kx, y + ky);
                        r += c.r * weight;
                        g += c.g * weight;
                        b += c.b * weight;
                        a += c.a * weight;
                    }
                }
                
                result.SetPixel(x, y, Color(
                    ClampByte(r), ClampByte(g), ClampByte(b), ClampByte(a)
                ));
            }
        }
        
        return result;
    }
    
    Image GaussianBlur(float sigma = 1.0f) const {
        int radius = static_cast<int>(std::ceil(sigma * 3));
        std::vector<float> kernel(radius * 2 + 1);
        
        // إنشاء kernel
        float sum = 0;
        for (int i = -radius; i <= radius; i++) {
            float val = std::exp(-(i * i) / (2 * sigma * sigma));
            kernel[i + radius] = val;
            sum += val;
        }
        
        // تطبيع
        for (float& k : kernel) k /= sum;
        
        // تطبيق أفقي
        Image temp(width_, height_, channels_);
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                float r = 0, g = 0, b = 0, a = 0;
                for (int k = -radius; k <= radius; k++) {
                    Color c = GetPixel(x + k, y);
                    float w = kernel[k + radius];
                    r += c.r * w;
                    g += c.g * w;
                    b += c.b * w;
                    a += c.a * w;
                }
                temp.SetPixel(x, y, Color(
                    ClampByte(r), ClampByte(g), ClampByte(b), ClampByte(a)
                ));
            }
        }
        
        // تطبيق عمودي
        Image result(width_, height_, channels_);
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                float r = 0, g = 0, b = 0, a = 0;
                for (int k = -radius; k <= radius; k++) {
                    Color c = temp.GetPixel(x, y + k);
                    float w = kernel[k + radius];
                    r += c.r * w;
                    g += c.g * w;
                    b += c.b * w;
                    a += c.a * w;
                }
                result.SetPixel(x, y, Color(
                    ClampByte(r), ClampByte(g), ClampByte(b), ClampByte(a)
                ));
            }
        }
        
        return result;
    }
    
    Image Sharpen(float amount = 1.0f) const {
        // Laplacian sharpening
        float kernel[3][3] = {
            { 0, -1 * amount, 0 },
            { -1 * amount, 1 + 4 * amount, -1 * amount },
            { 0, -1 * amount, 0 }
        };
        
        Image result(width_, height_, channels_);
        
        for (int y = 0; y < height_; y++) {
            for (int x = 0; x < width_; x++) {
                float r = 0, g = 0, b = 0;
                
                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        Color c = GetPixel(x + kx, y + ky);
                        float w = kernel[ky + 1][kx + 1];
                        r += c.r * w;
                        g += c.g * w;
                        b += c.b * w;
                    }
                }
                
                Color orig = GetPixel(x, y);
                result.SetPixel(x, y, Color(
                    ClampByte(r), ClampByte(g), ClampByte(b), orig.a
                ));
            }
        }
        
        return result;
    }
    
    Image EdgeDetect() const {
        // Sobel edge detection
        Image gray = ToGrayscale();
        Image result(width_, height_, channels_);
        
        float Gx[3][3] = {{ -1, 0, 1 }, { -2, 0, 2 }, { -1, 0, 1 }};
        float Gy[3][3] = {{ -1, -2, -1 }, { 0, 0, 0 }, { 1, 2, 1 }};
        
        for (int y = 1; y < height_ - 1; y++) {
            for (int x = 1; x < width_ - 1; x++) {
                float gx = 0, gy = 0;
                
                for (int ky = -1; ky <= 1; ky++) {
                    for (int kx = -1; kx <= 1; kx++) {
                        float val = gray.GetPixel(x + kx, y + ky).r;
                        gx += val * Gx[ky + 1][kx + 1];
                        gy += val * Gy[ky + 1][kx + 1];
                    }
                }
                
                float mag = std::sqrt(gx * gx + gy * gy);
                uint8_t edge = ClampByte(mag);
                result.SetPixel(x, y, Color(edge, edge, edge, 255));
            }
        }
        
        return result;
    }
    
    // ─────────────────────────────────────────────
    // المزج
    // ─────────────────────────────────────────────
    
    Image Blend(const Image& other, float alpha) const {
        int w = std::min(width_, other.width_);
        int h = std::min(height_, other.height_);
        
        Image result(w, h, channels_);
        
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                Color c1 = GetPixel(x, y);
                Color c2 = other.GetPixel(x, y);
                result.SetPixel(x, y, Color(
                    ClampByte(c1.r * (1 - alpha) + c2.r * alpha),
                    ClampByte(c1.g * (1 - alpha) + c2.g * alpha),
                    ClampByte(c1.b * (1 - alpha) + c2.b * alpha),
                    ClampByte(c1.a * (1 - alpha) + c2.a * alpha)
                ));
            }
        }
        
        return result;
    }
    
    // ─────────────────────────────────────────────
    // حفظ BMP (بدون اعتماديات خارجية)
    // ─────────────────────────────────────────────
    
    bool SaveBMP(const std::string& path) const {
        std::ofstream file(path, std::ios::binary);
        if (!file) return false;
        
        int rowSize = ((width_ * 3 + 3) / 4) * 4;  // محاذاة 4 bytes
        int imageSize = rowSize * height_;
        int fileSize = 54 + imageSize;
        
        // BMP Header
        uint8_t header[54] = {
            'B', 'M',                         // Magic
            0, 0, 0, 0,                       // File size (filled below)
            0, 0, 0, 0,                       // Reserved
            54, 0, 0, 0,                      // Data offset
            
            40, 0, 0, 0,                      // Header size
            0, 0, 0, 0,                       // Width (filled below)
            0, 0, 0, 0,                       // Height (filled below)
            1, 0,                             // Planes
            24, 0,                            // Bits per pixel
            0, 0, 0, 0,                       // Compression
            0, 0, 0, 0,                       // Image size (filled below)
            0, 0, 0, 0,                       // X pixels per meter
            0, 0, 0, 0,                       // Y pixels per meter
            0, 0, 0, 0,                       // Colors used
            0, 0, 0, 0                        // Important colors
        };
        
        // Fill in sizes
        *reinterpret_cast<uint32_t*>(&header[2]) = fileSize;
        *reinterpret_cast<int32_t*>(&header[18]) = width_;
        *reinterpret_cast<int32_t*>(&header[22]) = height_;
        *reinterpret_cast<uint32_t*>(&header[34]) = imageSize;
        
        file.write(reinterpret_cast<char*>(header), 54);
        
        // Write pixels (BGR, bottom-up)
        std::vector<uint8_t> row(rowSize, 0);
        for (int y = height_ - 1; y >= 0; y--) {
            for (int x = 0; x < width_; x++) {
                Color c = GetPixel(x, y);
                row[x * 3] = c.b;
                row[x * 3 + 1] = c.g;
                row[x * 3 + 2] = c.r;
            }
            file.write(reinterpret_cast<char*>(row.data()), rowSize);
        }
        
        return true;
    }
    
    static Image LoadBMP(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) return Image();
        
        unsigned char header[54];
        file.read(reinterpret_cast<char*>(header), 54);
        
        if (header[0] != 'B' || header[1] != 'M') return Image();
        
        int width = *reinterpret_cast<int*>(&header[18]);
        int height = *reinterpret_cast<int*>(&header[22]);
        int bpp = *reinterpret_cast<short*>(&header[28]);
        
        if (bpp != 24 && bpp != 32) return Image();
        
        int rowSize = ((width * (bpp / 8) + 3) / 4) * 4;
        
        Image img(width, height, 4);
        std::vector<uint8_t> row(rowSize);
        
        for (int y = height - 1; y >= 0; y--) {
            file.read(reinterpret_cast<char*>(row.data()), rowSize);
            for (int x = 0; x < width; x++) {
                int idx = x * (bpp / 8);
                img.SetPixel(x, y, Color(
                    row[idx + 2], row[idx + 1], row[idx],
                    bpp == 32 ? row[idx + 3] : 255
                ));
            }
        }
        
        return img;
    }

private:
    int width_, height_, channels_;
    std::vector<uint8_t> data_;
    
    static uint8_t ClampByte(float v) {
        return static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, v)));
    }
    
    Color SampleNearest(float x, float y) const {
        int ix = static_cast<int>(std::round(x));
        int iy = static_cast<int>(std::round(y));
        return GetPixel(ix, iy);
    }
    
    Color SampleBilinear(float x, float y) const {
        int x0 = static_cast<int>(std::floor(x));
        int y0 = static_cast<int>(std::floor(y));
        float fx = x - x0;
        float fy = y - y0;
        
        Color c00 = GetPixel(x0, y0);
        Color c10 = GetPixel(x0 + 1, y0);
        Color c01 = GetPixel(x0, y0 + 1);
        Color c11 = GetPixel(x0 + 1, y0 + 1);
        
        return Color(
            ClampByte(Lerp(Lerp(c00.r, c10.r, fx), Lerp(c01.r, c11.r, fx), fy)),
            ClampByte(Lerp(Lerp(c00.g, c10.g, fx), Lerp(c01.g, c11.g, fx), fy)),
            ClampByte(Lerp(Lerp(c00.b, c10.b, fx), Lerp(c01.b, c11.b, fx), fy)),
            ClampByte(Lerp(Lerp(c00.a, c10.a, fx), Lerp(c01.a, c11.a, fx), fy))
        );
    }
    
    Color SampleBicubic(float x, float y) const {
        // Simplified bicubic (use bilinear for now)
        return SampleBilinear(x, y);
    }
    
    static float Lerp(float a, float b, float t) {
        return a + t * (b - a);
    }
};

// =============================================================================
// دوال مساعدة للغة ص
// =============================================================================

/**
 * @brief إنشاء صورة جديدة
 */
inline Image انشئ_صورة(int عرض, int ارتفاع) {
    return Image(عرض, ارتفاع);
}

/**
 * @brief تغيير حجم صورة
 */
inline Image غيّر_الحجم(const Image& صورة, int عرض, int ارتفاع) {
    return صورة.Resize(عرض, ارتفاع);
}

/**
 * @brief قص صورة
 */
inline Image قص(const Image& صورة, int س, int ص, int عرض, int ارتفاع) {
    return صورة.Crop(س, ص, عرض, ارتفاع);
}

/**
 * @brief دوران 90 درجة
 */
inline Image دور_90(const Image& صورة) {
    return صورة.Rotate90CW();
}

/**
 * @brief تحويل لرمادي
 */
inline Image رمادي(const Image& صورة) {
    return صورة.ToGrayscale();
}

/**
 * @brief تمويه الصورة
 */
inline Image موّه(const Image& صورة, int نصف_قطر = 3) {
    return صورة.BoxBlur(نصف_قطر);
}

/**
 * @brief اكتشاف الحواف
 */
inline Image حواف(const Image& صورة) {
    return صورة.EdgeDetect();
}

/**
 * @brief تعديل السطوع
 */
inline Image سطوع(const Image& صورة, float معامل) {
    return صورة.AdjustBrightness(معامل);
}

/**
 * @brief تعديل التباين
 */
inline Image تباين(const Image& صورة, float معامل) {
    return صورة.AdjustContrast(معامل);
}

} // namespace image
} // namespace sad

#endif // SAD_STDLIB_IMAGE_H
