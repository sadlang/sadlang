/*
 * ============================================================================
 * وحدة المخزن المؤقت للإطارات و GPU - لغة ص
 * Framebuffer & GPU Module - Sad Language
 * ============================================================================
 *
 * @brief (AR) إدارة المخزن المؤقت للإطارات والرسومات عالية الدقة
 *              دعم أوضاع VESA/VBE و GOP (UEFI) و GPU المباشر
 *              رسم البكسل والأشكال والخطوط والنصوص
 *              طبقة تجريد GPU مع دعم NVIDIA و AMD و Intel
 *
 * @brief (EN) Framebuffer management and HD graphics
 *              VESA/VBE and GOP (UEFI) and direct GPU support
 *              Pixel, shape, line, and text rendering
 *              GPU abstraction layer with NVIDIA, AMD, Intel support
 * ============================================================================
 */

#ifndef SAD_LOW_LEVEL_FRAMEBUFFER_H
#define SAD_LOW_LEVEL_FRAMEBUFFER_H

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <functional>

namespace Sad {
namespace LowLevel {

// ============================================================================
// هياكل الألوان / Color Structures
// ============================================================================

/// @brief (AR) لون RGBA (32-بت) / (EN) RGBA Color (32-bit)
#pragma pack(push, 1)
struct Color {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;

    Color() : blue(0), green(0), red(0), alpha(255) {}
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : blue(b), green(g), red(r), alpha(a) {}

    /// @brief (AR) تحويل إلى عدد صحيح 32-بت / (EN) Convert to 32-bit integer
    uint32_t toUint32() const {
        return (static_cast<uint32_t>(alpha) << 24) |
               (static_cast<uint32_t>(red) << 16) |
               (static_cast<uint32_t>(green) << 8) |
               static_cast<uint32_t>(blue);
    }

    /// @brief (AR) إنشاء من عدد صحيح / (EN) Create from integer
    static Color fromUint32(uint32_t c) {
        return Color((c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF, (c >> 24) & 0xFF);
    }

    /// @brief (AR) مزج ألفا / (EN) Alpha blend
    Color blend(const Color& over) const {
        if (over.alpha == 255) return over;
        if (over.alpha == 0) return *this;
        uint16_t a = over.alpha;
        uint16_t ia = 255 - a;
        return Color(
            static_cast<uint8_t>((over.red * a + red * ia) / 255),
            static_cast<uint8_t>((over.green * a + green * ia) / 255),
            static_cast<uint8_t>((over.blue * a + blue * ia) / 255),
            255
        );
    }
};
#pragma pack(pop)

/// @brief (AR) ألوان شائعة / (EN) Common colors
namespace Colors {
    static const Color Black(0, 0, 0);
    static const Color White(255, 255, 255);
    static const Color Red(255, 0, 0);
    static const Color Green(0, 255, 0);
    static const Color Blue(0, 0, 255);
    static const Color Yellow(255, 255, 0);
    static const Color Cyan(0, 255, 255);
    static const Color Magenta(255, 0, 255);
    static const Color Orange(255, 165, 0);
    static const Color Gray(128, 128, 128);
    static const Color DarkGray(64, 64, 64);
    static const Color LightGray(192, 192, 192);
    static const Color Transparent(0, 0, 0, 0);
}

// ============================================================================
// هياكل الأبعاد / Dimension Structures
// ============================================================================

/// @brief (AR) نقطة ثنائية الأبعاد / (EN) 2D Point
struct Point {
    int32_t x;
    int32_t y;
    Point() : x(0), y(0) {}
    Point(int32_t x_, int32_t y_) : x(x_), y(y_) {}
};

/// @brief (AR) مستطيل / (EN) Rectangle
struct Rect {
    int32_t x, y;
    uint32_t width, height;
    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(int32_t x_, int32_t y_, uint32_t w, uint32_t h)
        : x(x_), y(y_), width(w), height(h) {}

    bool contains(int32_t px, int32_t py) const {
        return px >= x && px < x + static_cast<int32_t>(width) &&
               py >= y && py < y + static_cast<int32_t>(height);
    }

    Rect intersect(const Rect& other) const {
        int32_t x1 = (x > other.x) ? x : other.x;
        int32_t y1 = (y > other.y) ? y : other.y;
        int32_t x2 = ((x + static_cast<int32_t>(width)) < (other.x + static_cast<int32_t>(other.width)))
                          ? (x + static_cast<int32_t>(width))
                          : (other.x + static_cast<int32_t>(other.width));
        int32_t y2 = ((y + static_cast<int32_t>(height)) < (other.y + static_cast<int32_t>(other.height)))
                          ? (y + static_cast<int32_t>(height))
                          : (other.y + static_cast<int32_t>(other.height));
        if (x1 >= x2 || y1 >= y2) return Rect(0, 0, 0, 0);
        return Rect(x1, y1, static_cast<uint32_t>(x2 - x1), static_cast<uint32_t>(y2 - y1));
    }
};

// ============================================================================
// تنسيق البكسل / Pixel Format
// ============================================================================

/// @brief (AR) تنسيق البكسل / (EN) Pixel format
enum class PixelFormat : uint8_t {
    RGB_888,        // 24-بت / 24-bit
    RGBA_8888,      // 32-بت / 32-bit
    BGR_888,        // 24-بت BGR
    BGRA_8888,      // 32-بت BGR
    RGB_565,        // 16-بت / 16-bit
    INDEXED_8,      // 8-بت مفهرس / 8-bit indexed
    GRAYSCALE_8     // تدرج رمادي / Grayscale
};

// ============================================================================
// وضع الفيديو / Video Mode
// ============================================================================

/// @brief (AR) معلومات وضع الفيديو / (EN) Video mode information
struct VideoMode {
    uint32_t width;             // العرض بالبكسل / Width in pixels
    uint32_t height;            // الارتفاع بالبكسل / Height in pixels
    uint8_t  bitsPerPixel;      // عمق اللون / Color depth
    uint32_t pitch;             // بايت لكل سطر / Bytes per scanline
    uint64_t framebufferAddr;   // عنوان مادي / Physical address
    uint64_t framebufferSize;   // حجم بالبايت / Size in bytes
    PixelFormat format;         // تنسيق البكسل / Pixel format
    uint16_t modeNumber;        // رقم وضع VESA / VESA mode number
    uint32_t refreshRate;       // معدل التحديث / Refresh rate (Hz)

    VideoMode() : width(0), height(0), bitsPerPixel(0), pitch(0),
                  framebufferAddr(0), framebufferSize(0),
                  format(PixelFormat::BGRA_8888), modeNumber(0), refreshRate(60) {}
};

// ============================================================================
// معلومات VESA VBE / VESA VBE Information
// ============================================================================

/// @brief (AR) كتلة معلومات VESA VBE / (EN) VESA VBE Info Block
#pragma pack(push, 1)
struct VBEInfoBlock {
    char     signature[4];      // "VESA"
    uint16_t version;           // إصدار VBE / VBE version
    uint32_t oemStringPtr;      // مؤشر سلسلة OEM / OEM string pointer
    uint32_t capabilities;      // إمكانيات / Capabilities
    uint32_t videoModePtr;      // مؤشر أوضاع الفيديو / Video mode list pointer
    uint16_t totalMemory;       // بوحدات 64KB / In 64KB units
    uint16_t softwareRevision;
    uint32_t vendorNamePtr;
    uint32_t productNamePtr;
    uint32_t productRevPtr;
    uint8_t  reserved[222];
    uint8_t  oemData[256];
};

/// @brief (AR) معلومات وضع VESA / (EN) VESA Mode Info
struct VBEModeInfo {
    uint16_t attributes;
    uint8_t  windowA;
    uint8_t  windowB;
    uint16_t granularity;
    uint16_t windowSize;
    uint16_t segmentA;
    uint16_t segmentB;
    uint32_t winFuncPtr;
    uint16_t pitch;             // بايت لكل سطر / Bytes per scanline
    uint16_t width;
    uint16_t height;
    uint8_t  charWidth;
    uint8_t  charHeight;
    uint8_t  planes;
    uint8_t  bpp;               // بت لكل بكسل / Bits per pixel
    uint8_t  banks;
    uint8_t  memoryModel;
    uint8_t  bankSize;
    uint8_t  imagePages;
    uint8_t  reserved0;
    // حقول اللون المباشر / Direct color fields
    uint8_t  redMaskSize;
    uint8_t  redFieldPosition;
    uint8_t  greenMaskSize;
    uint8_t  greenFieldPosition;
    uint8_t  blueMaskSize;
    uint8_t  blueFieldPosition;
    uint8_t  rsvdMaskSize;
    uint8_t  rsvdFieldPosition;
    uint8_t  directColorAttributes;
    uint32_t framebuffer;       // عنوان المخزن المادي / Physical framebuffer address
    uint32_t offScreenMemOff;
    uint16_t offScreenMemSize;
    uint8_t  reserved1[206];
};
#pragma pack(pop)

// ============================================================================
// معلومات GOP (UEFI) / GOP (UEFI) Information
// ============================================================================

/// @brief (AR) تنسيق بكسل GOP / (EN) GOP Pixel Format
enum class GOPPixelFormat : uint32_t {
    PixelRedGreenBlueReserved8BitPerColor = 0,
    PixelBlueGreenRedReserved8BitPerColor = 1,
    PixelBitMask = 2,
    PixelBltOnly = 3,
    PixelFormatMax = 4
};

/// @brief (AR) معلومات وضع GOP / (EN) GOP Mode Info
#pragma pack(push, 1)
struct GOPModeInfo {
    uint32_t version;
    uint32_t horizontalResolution;
    uint32_t verticalResolution;
    GOPPixelFormat pixelFormat;
    struct {
        uint32_t redMask;
        uint32_t greenMask;
        uint32_t blueMask;
        uint32_t reservedMask;
    } pixelInformation;
    uint32_t pixelsPerScanLine;
};
#pragma pack(pop)

// ============================================================================
// معلومات GPU / GPU Information
// ============================================================================

/// @brief (AR) نوع GPU / (EN) GPU Type
enum class GPUType : uint8_t {
    UNKNOWN,
    NVIDIA,
    AMD,
    INTEL,
    VIRTUAL,        // GPU افتراضي (QEMU, VirtualBox) / Virtual GPU
    SOFTWARE        // عرض برمجي / Software rendering
};

/// @brief (AR) قدرات GPU / (EN) GPU Capabilities
struct GPUCapabilities {
    bool supports2D;            // رسومات ثنائية الأبعاد / 2D graphics
    bool supports3D;            // رسومات ثلاثية الأبعاد / 3D graphics
    bool supportsHardwareCursor; // مؤشر عتاد / Hardware cursor
    bool supportsVSync;         // مزامنة عمودية / V-Sync
    bool supportsDoubleBuffer;  // تخزين مزدوج / Double buffering
    bool supportsAlphaBlend;    // مزج ألفا / Alpha blending
    bool supportsDMA;           // نقل DMA / DMA transfer
    bool supportsMMIO;          // MMIO مباشر / Direct MMIO
    uint64_t videoMemorySize;   // حجم ذاكرة الفيديو / Video memory size
    uint32_t maxWidth;          // أقصى عرض / Maximum width
    uint32_t maxHeight;         // أقصى ارتفاع / Maximum height
    uint32_t maxBpp;            // أقصى عمق لون / Maximum color depth

    GPUCapabilities()
        : supports2D(true), supports3D(false), supportsHardwareCursor(false),
          supportsVSync(false), supportsDoubleBuffer(true), supportsAlphaBlend(true),
          supportsDMA(false), supportsMMIO(false), videoMemorySize(0),
          maxWidth(1920), maxHeight(1080), maxBpp(32) {}
};

/// @brief (AR) معلومات GPU كاملة / (EN) Full GPU Info
struct GPUInfo {
    GPUType type;
    uint16_t vendorId;
    uint16_t deviceId;
    std::string vendorName;
    std::string deviceName;
    GPUCapabilities capabilities;
    uint64_t mmioBase;          // عنوان MMIO الأساسي / MMIO base address
    uint64_t mmioSize;          // حجم MMIO / MMIO size
    uint64_t vramBase;          // عنوان VRAM / VRAM address
    uint64_t vramSize;          // حجم VRAM / VRAM size

    GPUInfo() : type(GPUType::UNKNOWN), vendorId(0), deviceId(0),
                mmioBase(0), mmioSize(0), vramBase(0), vramSize(0) {}
};

// ============================================================================
// نوع خط بسيط (bitmap font) / Simple Bitmap Font
// ============================================================================

/// @brief (AR) خط نقطي بسيط 8×16 / (EN) Simple 8x16 bitmap font
struct BitmapFont {
    uint8_t width;              // عرض المحرف / Glyph width
    uint8_t height;             // ارتفاع المحرف / Glyph height
    uint16_t numGlyphs;         // عدد المحارف / Number of glyphs
    const uint8_t* data;        // بيانات المحارف / Glyph data
    uint32_t firstChar;         // أول محرف / First character

    BitmapFont() : width(8), height(16), numGlyphs(256),
                   data(nullptr), firstChar(0) {}
};

// ============================================================================
// مدير المخزن المؤقت / Framebuffer Manager
// ============================================================================

/**
 * @brief (AR) مدير المخزن المؤقت للإطارات - محرك الرسومات الرئيسي
 *              يدعم VESA VBE و UEFI GOP ورسومات GPU المباشرة
 *              يوفر تخزين مزدوج وعمليات رسم كاملة
 *
 * @brief (EN) Framebuffer Manager - Main graphics engine
 *              Supports VESA VBE, UEFI GOP, and direct GPU rendering
 *              Provides double buffering and complete drawing operations
 */
class FramebufferManager {
public:
    /// @brief (AR) الحصول على المثيل الوحيد / (EN) Get singleton instance
    static FramebufferManager& getInstance();

    // ========================================================================
    // تهيئة / Initialization
    // ========================================================================

    /// @brief (AR) تهيئة من معلومات VESA VBE / (EN) Initialize from VESA VBE info
    bool initFromVBE(const VBEModeInfo* modeInfo);

    /// @brief (AR) تهيئة من معلومات GOP / (EN) Initialize from GOP info
    bool initFromGOP(const GOPModeInfo* modeInfo, uint64_t framebufferBase, uint64_t framebufferSize);

    /// @brief (AR) تهيئة يدوية / (EN) Manual initialization
    bool initManual(uint64_t framebufferAddr, uint32_t width, uint32_t height,
                    uint32_t pitch, uint8_t bpp, PixelFormat format);

    /// @brief (AR) تهيئة GPU / (EN) Initialize GPU
    bool initGPU(const GPUInfo& gpu);

    /// @brief (AR) تعيين وضع الفيديو / (EN) Set video mode
    bool setVideoMode(uint32_t width, uint32_t height, uint8_t bpp);

    /// @brief (AR) الحصول على أوضاع الفيديو المتاحة / (EN) Get available video modes
    std::vector<VideoMode> getAvailableModes() const;

    /// @brief (AR) الحصول على الوضع الحالي / (EN) Get current mode
    const VideoMode& getCurrentMode() const { return currentMode_; }

    /// @brief (AR) الحصول على معلومات GPU / (EN) Get GPU info
    const GPUInfo& getGPUInfo() const { return gpuInfo_; }

    // ========================================================================
    // عمليات البكسل / Pixel Operations
    // ========================================================================

    /// @brief (AR) رسم بكسل / (EN) Draw pixel
    void putPixel(int32_t x, int32_t y, const Color& color);

    /// @brief (AR) قراءة بكسل / (EN) Read pixel
    Color getPixel(int32_t x, int32_t y) const;

    /// @brief (AR) رسم بكسل مع مزج ألفا / (EN) Draw pixel with alpha blending
    void putPixelAlpha(int32_t x, int32_t y, const Color& color);

    // ========================================================================
    // عمليات الأشكال / Shape Operations
    // ========================================================================

    /// @brief (AR) رسم خط / (EN) Draw line (Bresenham)
    void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const Color& color);

    /// @brief (AR) رسم مستطيل (إطار) / (EN) Draw rectangle outline
    void drawRect(const Rect& rect, const Color& color);

    /// @brief (AR) رسم مستطيل ممتلئ / (EN) Draw filled rectangle
    void fillRect(const Rect& rect, const Color& color);

    /// @brief (AR) رسم دائرة / (EN) Draw circle (Midpoint algorithm)
    void drawCircle(int32_t cx, int32_t cy, int32_t radius, const Color& color);

    /// @brief (AR) رسم دائرة ممتلئة / (EN) Draw filled circle
    void fillCircle(int32_t cx, int32_t cy, int32_t radius, const Color& color);

    /// @brief (AR) رسم مثلث / (EN) Draw triangle
    void drawTriangle(Point p1, Point p2, Point p3, const Color& color);

    /// @brief (AR) رسم مثلث ممتلئ / (EN) Draw filled triangle
    void fillTriangle(Point p1, Point p2, Point p3, const Color& color);

    /// @brief (AR) رسم قوس / (EN) Draw arc
    void drawArc(int32_t cx, int32_t cy, int32_t radius,
                 float startAngle, float endAngle, const Color& color);

    /// @brief (AR) رسم مستطيل مدور الزوايا / (EN) Draw rounded rectangle
    void drawRoundedRect(const Rect& rect, int32_t radius, const Color& color);

    /// @brief (AR) رسم مستطيل مدور ممتلئ / (EN) Draw filled rounded rectangle
    void fillRoundedRect(const Rect& rect, int32_t radius, const Color& color);

    /// @brief (AR) رسم تدرج لوني أفقي / (EN) Draw horizontal gradient
    void fillGradientH(const Rect& rect, const Color& startColor, const Color& endColor);

    /// @brief (AR) رسم تدرج لوني عمودي / (EN) Draw vertical gradient
    void fillGradientV(const Rect& rect, const Color& startColor, const Color& endColor);

    // ========================================================================
    // عمليات النص / Text Operations
    // ========================================================================

    /// @brief (AR) تعيين الخط / (EN) Set font
    void setFont(const BitmapFont& font);

    /// @brief (AR) رسم محرف / (EN) Draw character
    void drawChar(int32_t x, int32_t y, char c, const Color& fg, const Color& bg);

    /// @brief (AR) رسم نص / (EN) Draw string
    void drawString(int32_t x, int32_t y, const char* str,
                    const Color& fg, const Color& bg = Colors::Transparent);

    /// @brief (AR) رسم نص UTF-8 / (EN) Draw UTF-8 string
    void drawStringUTF8(int32_t x, int32_t y, const char* str,
                        const Color& fg, const Color& bg = Colors::Transparent);

    // ========================================================================
    // عمليات المخزن / Buffer Operations
    // ========================================================================

    /// @brief (AR) مسح الشاشة / (EN) Clear screen
    void clear(const Color& color = Colors::Black);

    /// @brief (AR) تبديل المخزن (double buffering) / (EN) Swap buffers
    void swapBuffers();

    /// @brief (AR) تفعيل/تعطيل التخزين المزدوج / (EN) Enable/disable double buffering
    void setDoubleBuffering(bool enabled);

    /// @brief (AR) هل التخزين المزدوج مفعل / (EN) Is double buffering enabled
    bool isDoubleBuffered() const { return doubleBuffered_; }

    /// @brief (AR) نسخ منطقة / (EN) Copy region (blit)
    void blit(const uint32_t* src, int32_t srcX, int32_t srcY,
              uint32_t srcWidth, uint32_t srcHeight,
              int32_t destX, int32_t destY);

    /// @brief (AR) نسخ منطقة مع قناع شفافية / (EN) Blit with alpha mask
    void blitAlpha(const uint32_t* src, int32_t srcX, int32_t srcY,
                   uint32_t srcWidth, uint32_t srcHeight,
                   int32_t destX, int32_t destY);

    /// @brief (AR) تمرير الشاشة / (EN) Scroll screen
    void scroll(int32_t dx, int32_t dy, const Color& fillColor = Colors::Black);

    /// @brief (AR) الحصول على مؤشر المخزن الخلفي / (EN) Get back buffer pointer
    uint32_t* getBackBuffer() { return backBuffer_; }

    /// @brief (AR) الحصول على مؤشر المخزن الأمامي / (EN) Get front buffer pointer
    volatile uint32_t* getFrontBuffer() { return frontBuffer_; }

    // ========================================================================
    // عمليات الصورة / Image Operations
    // ========================================================================

    /// @brief (AR) رسم صورة BMP خام / (EN) Draw raw BMP image
    void drawBitmap(int32_t x, int32_t y, const uint32_t* pixels,
                    uint32_t width, uint32_t height);

    /// @brief (AR) رسم صورة مع شفافية / (EN) Draw image with transparency
    void drawBitmapAlpha(int32_t x, int32_t y, const uint32_t* pixels,
                         uint32_t width, uint32_t height);

    /// @brief (AR) تغيير حجم صورة (أقرب جار) / (EN) Scale image (nearest neighbor)
    void drawBitmapScaled(int32_t x, int32_t y, const uint32_t* pixels,
                          uint32_t srcW, uint32_t srcH,
                          uint32_t destW, uint32_t destH);

    // ========================================================================
    // المؤشر / Cursor
    // ========================================================================

    /// @brief (AR) تعيين صورة المؤشر / (EN) Set cursor image
    void setCursorImage(const uint32_t* pixels, uint32_t width, uint32_t height,
                        int32_t hotspotX, int32_t hotspotY);

    /// @brief (AR) تحريك المؤشر / (EN) Move cursor
    void moveCursor(int32_t x, int32_t y);

    /// @brief (AR) إظهار/إخفاء المؤشر / (EN) Show/hide cursor
    void showCursor(bool visible);

    // ========================================================================
    // Clipping / القص
    // ========================================================================

    /// @brief (AR) تعيين منطقة القص / (EN) Set clipping rectangle
    void setClipRect(const Rect& rect);

    /// @brief (AR) إزالة منطقة القص / (EN) Remove clipping
    void resetClipRect();

    /// @brief (AR) الحصول على منطقة القص / (EN) Get clip rect
    const Rect& getClipRect() const { return clipRect_; }

    // ========================================================================
    // VSync / المزامنة العمودية
    // ========================================================================

    /// @brief (AR) انتظار المزامنة العمودية / (EN) Wait for VSync
    void waitVSync();

    /// @brief (AR) تفعيل/تعطيل VSync / (EN) Enable/disable VSync
    void setVSync(bool enabled);

    // ========================================================================
    // التقرير / Report
    // ========================================================================

    /// @brief (AR) توليد تقرير / (EN) Generate report
    std::string generateReport() const;

    /// @brief (AR) هل المدير مهيأ / (EN) Is initialized
    bool isInitialized() const { return isInitialized_; }

private:
    FramebufferManager();
    FramebufferManager(const FramebufferManager&) = delete;
    FramebufferManager& operator=(const FramebufferManager&) = delete;

    /// @brief (AR) الحصول على مؤشر البكسل / (EN) Get pixel pointer in back buffer
    uint32_t* pixelAt(int32_t x, int32_t y);

    /// @brief (AR) التحقق من أن الإحداثيات ضمن الحدود / (EN) Check bounds
    bool inBounds(int32_t x, int32_t y) const;

    /// @brief (AR) التحقق من القص / (EN) Check clipping
    bool isClipped(int32_t x, int32_t y) const;

    /// @brief (AR) رسم خط أفقي سريع / (EN) Fast horizontal line
    void hLine(int32_t x1, int32_t x2, int32_t y, const Color& color);

    /// @brief (AR) رسم خط عمودي سريع / (EN) Fast vertical line
    void vLine(int32_t x, int32_t y1, int32_t y2, const Color& color);

    // بيانات المخزن / Buffer data
    volatile uint32_t* frontBuffer_;    // المخزن الأمامي (VRAM) / Front buffer (VRAM)
    uint32_t* backBuffer_;              // المخزن الخلفي / Back buffer
    uint32_t  bufferSize_;              // حجم المخزن / Buffer size

    // وضع الفيديو / Video mode
    VideoMode currentMode_;

    // GPU
    GPUInfo gpuInfo_;

    // أوضاع متاحة / Available modes
    std::vector<VideoMode> availableModes_;

    // الخط الحالي / Current font
    BitmapFont currentFont_;

    // التخزين المزدوج / Double buffering
    bool doubleBuffered_;

    // القص / Clipping
    Rect clipRect_;
    bool clipEnabled_;

    // المؤشر / Cursor
    std::vector<uint32_t> cursorImage_;
    std::vector<uint32_t> cursorBackground_;
    uint32_t cursorWidth_, cursorHeight_;
    int32_t  cursorX_, cursorY_;
    int32_t  cursorHotX_, cursorHotY_;
    bool     cursorVisible_;

    // VSync
    bool vsyncEnabled_;

    // الحالة / State
    bool isInitialized_;
};

// ============================================================================
// ثوابت / Constants
// ============================================================================

namespace FramebufferConstants {
    // أوضاع VESA VBE شائعة / Common VESA VBE modes
    static constexpr uint16_t VBE_MODE_640x480x8    = 0x0101;
    static constexpr uint16_t VBE_MODE_800x600x8    = 0x0103;
    static constexpr uint16_t VBE_MODE_1024x768x8   = 0x0105;
    static constexpr uint16_t VBE_MODE_640x480x16   = 0x0111;
    static constexpr uint16_t VBE_MODE_800x600x16   = 0x0114;
    static constexpr uint16_t VBE_MODE_1024x768x16  = 0x0117;
    static constexpr uint16_t VBE_MODE_640x480x32   = 0x0112;
    static constexpr uint16_t VBE_MODE_800x600x32   = 0x0115;
    static constexpr uint16_t VBE_MODE_1024x768x32  = 0x0118;
    static constexpr uint16_t VBE_MODE_1280x1024x32 = 0x011B;
    static constexpr uint16_t VBE_LINEAR_FRAMEBUFFER = 0x4000;

    // أبعاد HD / HD resolutions
    static constexpr uint32_t HD_WIDTH   = 1280;
    static constexpr uint32_t HD_HEIGHT  = 720;
    static constexpr uint32_t FHD_WIDTH  = 1920;
    static constexpr uint32_t FHD_HEIGHT = 1080;
    static constexpr uint32_t QHD_WIDTH  = 2560;
    static constexpr uint32_t QHD_HEIGHT = 1440;
    static constexpr uint32_t UHD_WIDTH  = 3840;
    static constexpr uint32_t UHD_HEIGHT = 2160;

    // VGA CRT Controller / متحكم CRT
    static constexpr uint16_t VGA_CRTC_INDEX = 0x3D4;
    static constexpr uint16_t VGA_CRTC_DATA  = 0x3D5;
    static constexpr uint16_t VGA_INPUT_STATUS = 0x3DA;
}

} // namespace LowLevel
} // namespace Sad

#endif // SAD_LOW_LEVEL_FRAMEBUFFER_H
