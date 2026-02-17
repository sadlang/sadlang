// ==============================================================================
// font.cpp - تطبيق نظام الخطوط / Font System Implementation
// ==============================================================================
// الوصف: تطبيق كامل لتحميل وإدارة خطوط TrueType باستخدام stb_truetype
// Description: Complete implementation for loading and managing TrueType fonts using stb_truetype
// ==============================================================================

#include "../../include/text/font.h"          // ملف الرأس / Header file
#include "../../include/text/arabic_text.h"   // دعم العربية / Arabic support

// تضمين stb_truetype - مكتبة تحميل الخطوط
// Include stb_truetype - font loading library
#define STB_TRUETYPE_IMPLEMENTATION          // تفعيل التطبيق / Enable implementation
#include <stb_truetype.h>                    // مكتبة stb_truetype / stb_truetype library

#include <fstream>                           // لقراءة الملفات / For file reading
#include <vector>                            // للمصفوفات / For vectors
#include <cstring>                           // لعمليات الذاكرة / For memory operations
#include <algorithm>                         // للخوارزميات / For algorithms
#include <cmath>                             // للدوال الرياضية / For math functions
#include <iostream>                          // للطباعة / For printing

namespace sad {
namespace graphics {

// ==============================================================================
// المُنشئات والمُدمر / Constructors and Destructor
// ==============================================================================

/// المُنشئ الافتراضي / Default constructor
Font::Font()
    : m_fontData(nullptr)                    // لا توجد بيانات / No data
    , m_fontDataSize(0)                      // حجم صفر / Zero size
    , m_ownsData(false)                      // لا نمتلك البيانات / Don't own data
    , m_fontSize(0.0f)                       // حجم صفر / Zero size
    , m_lineHeight(0.0f)                     // ارتفاع صفر / Zero height
    , m_baseline(0.0f)                       // baseline صفر / Zero baseline
    , m_ascent(0.0f)                         // ascent صفر / Zero ascent
    , m_descent(0.0f)                        // descent صفر / Zero descent
    , m_atlas(nullptr)                       // لا يوجد atlas / No atlas
    , m_isValid(false)                       // غير صالح / Invalid
    , m_stbFontInfo(nullptr)                 // لا توجد معلومات / No info
{
}

/// المُنشئ بمسار ملف / Constructor with file path
Font::Font(const std::string& filepath, float fontSize)
    : Font()                                 // استدعاء المُنشئ الافتراضي / Call default constructor
{
    LoadFromFile(filepath, fontSize);        // تحميل من الملف / Load from file
}

/// المُدمر / Destructor
Font::~Font() {
    Free();                                  // تحرير الموارد / Free resources
}

/// مُنشئ النقل / Move constructor
Font::Font(Font&& other) noexcept
    : m_fontData(other.m_fontData)           // نقل البيانات / Move data
    , m_fontDataSize(other.m_fontDataSize)   // نقل الحجم / Move size
    , m_ownsData(other.m_ownsData)           // نقل الملكية / Move ownership
    , m_fontSize(other.m_fontSize)           // نقل حجم الخط / Move font size
    , m_lineHeight(other.m_lineHeight)       // نقل ارتفاع السطر / Move line height
    , m_baseline(other.m_baseline)           // نقل baseline / Move baseline
    , m_ascent(other.m_ascent)               // نقل ascent / Move ascent
    , m_descent(other.m_descent)             // نقل descent / Move descent
    , m_glyphs(std::move(other.m_glyphs))    // نقل الأحرف / Move glyphs
    , m_atlas(std::move(other.m_atlas))      // نقل atlas / Move atlas
    , m_isValid(other.m_isValid)             // نقل الصحة / Move validity
    , m_stbFontInfo(other.m_stbFontInfo)     // نقل معلومات stb / Move stb info
{
    // إعادة تعيين الكائن الآخر / Reset other object
    other.m_fontData = nullptr;              // مسح البيانات / Clear data
    other.m_fontDataSize = 0;                // مسح الحجم / Clear size
    other.m_ownsData = false;                // لا يمتلك البيانات / Doesn't own data
    other.m_isValid = false;                 // غير صالح / Invalid
    other.m_stbFontInfo = nullptr;           // مسح المعلومات / Clear info
}

/// مُعامل النقل / Move assignment operator
Font& Font::operator=(Font&& other) noexcept {
    if (this != &other) {                    // التحقق من عدم النقل لنفسه / Check not self-assignment
        Free();                              // تحرير الموارد الحالية / Free current resources
        
        // نقل البيانات / Move data
        m_fontData = other.m_fontData;
        m_fontDataSize = other.m_fontDataSize;
        m_ownsData = other.m_ownsData;
        m_fontSize = other.m_fontSize;
        m_lineHeight = other.m_lineHeight;
        m_baseline = other.m_baseline;
        m_ascent = other.m_ascent;
        m_descent = other.m_descent;
        m_glyphs = std::move(other.m_glyphs);
        m_atlas = std::move(other.m_atlas);
        m_isValid = other.m_isValid;
        m_stbFontInfo = other.m_stbFontInfo;
        
        // إعادة تعيين الآخر / Reset other
        other.m_fontData = nullptr;
        other.m_fontDataSize = 0;
        other.m_ownsData = false;
        other.m_isValid = false;
        other.m_stbFontInfo = nullptr;
    }
    return *this;                            // إرجاع المرجع / Return reference
}

// ==============================================================================
// التحميل / Loading
// ==============================================================================

/// تحميل خط من ملف / Load font from file
bool Font::LoadFromFile(const std::string& filepath, float fontSize) {
    // فتح الملف / Open file
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    
    if (!file.is_open()) {                   // التحقق من الفتح / Check if opened
        std::cerr << "(AR) فشل فتح ملف الخط: " << filepath << std::endl;
        std::cerr << "(EN) Failed to open font file: " << filepath << std::endl;
        return false;                        // فشل / Failure
    }
    
    // الحصول على حجم الملف / Get file size
    std::streamsize fileSize = file.tellg(); // حجم الملف / File size
    file.seekg(0, std::ios::beg);            // العودة للبداية / Return to beginning
    
    // تخصيص ذاكرة للبيانات / Allocate memory for data
    u8* data = new u8[fileSize];             // تخصيص ذاكرة / Allocate memory
    
    // قراءة البيانات / Read data
    if (!file.read(reinterpret_cast<char*>(data), fileSize)) {
        std::cerr << "(AR) فشل قراءة ملف الخط / (EN) Failed to read font file" << std::endl;
        delete[] data;                       // تحرير الذاكرة / Free memory
        return false;                        // فشل / Failure
    }
    
    file.close();                            // إغلاق الملف / Close file
    
    // تحميل من الذاكرة / Load from memory
    bool success = LoadFromMemory(data, static_cast<size_t>(fileSize), fontSize);
    
    if (success) {
        m_ownsData = true;                   // نمتلك البيانات / We own the data
    } else {
        delete[] data;                       // تحرير الذاكرة عند الفشل / Free memory on failure
    }
    
    return success;                          // إرجاع النتيجة / Return result
}

/// تحميل خط من الذاكرة / Load font from memory
bool Font::LoadFromMemory(const u8* data, size_t size, float fontSize) {
    // التحقق من البيانات / Validate data
    if (!data || size == 0) {                // التحقق من البيانات / Check data
        std::cerr << "(AR) بيانات خط غير صالحة / (EN) Invalid font data" << std::endl;
        return false;                        // فشل / Failure
    }
    
    // تحرير الموارد السابقة / Free previous resources
    Free();                                  // تحرير الموارد / Free resources
    
    // حفظ البيانات / Store data
    m_fontData = const_cast<u8*>(data);      // حفظ مؤشر البيانات / Store data pointer
    m_fontDataSize = size;                   // حفظ الحجم / Store size
    m_fontSize = fontSize;                   // حفظ حجم الخط / Store font size
    
    // تخصيص ذاكرة لمعلومات stb / Allocate memory for stb info
    m_stbFontInfo = new stbtt_fontinfo();    // تخصيص ذاكرة / Allocate memory
    
    // تهيئة stb_truetype / Initialize stb_truetype
    if (!stbtt_InitFont(static_cast<stbtt_fontinfo*>(m_stbFontInfo), 
                        m_fontData, 0)) {    // محاولة التهيئة / Try initialization
        std::cerr << "(AR) فشل تهيئة الخط / (EN) Failed to initialize font" << std::endl;
        delete static_cast<stbtt_fontinfo*>(m_stbFontInfo);
        m_stbFontInfo = nullptr;
        return false;                        // فشل / Failure
    }
    
    // حساب scale للحجم المطلوب / Calculate scale for desired size
    float scale = stbtt_ScaleForPixelHeight(
        static_cast<stbtt_fontinfo*>(m_stbFontInfo), 
        fontSize                             // حجم الخط المطلوب / Desired font size
    );
    
    // الحصول على معلومات الخط / Get font metrics
    int ascent, descent, lineGap;            // معلومات الخط / Font metrics
    stbtt_GetFontVMetrics(
        static_cast<stbtt_fontinfo*>(m_stbFontInfo),
        &ascent, &descent, &lineGap          // الحصول على القيم / Get values
    );
    
    // حساب القيم بمقياس البكسل / Calculate scaled values
    m_ascent = ascent * scale;               // مسافة فوق baseline / Distance above baseline
    m_descent = -descent * scale;            // مسافة تحت baseline / Distance below baseline
    m_lineHeight = (ascent - descent + lineGap) * scale;  // ارتفاع السطر / Line height
    m_baseline = m_ascent;                   // الخط الأساسي / Baseline
    
    // توليد texture atlas / Generate texture atlas
    if (!GenerateAtlas()) {                  // محاولة التوليد / Try generation
        std::cerr << "(AR) فشل توليد atlas / (EN) Failed to generate atlas" << std::endl;
        return false;                        // فشل / Failure
    }
    
    m_isValid = true;                        // الخط صالح الآن / Font is now valid
    return true;                             // نجح / Success
}

// ==============================================================================
// توليد Texture Atlas / Generate Texture Atlas
// ==============================================================================

/// توليد texture atlas للخط / Generate texture atlas for font
bool Font::GenerateAtlas(u32 firstChar, u32 numChars) {
    if (!m_stbFontInfo) {                    // التحقق من التهيئة / Check initialization
        return false;                        // فشل / Failure
    }
    
    // حساب scale / Calculate scale
    float scale = stbtt_ScaleForPixelHeight(
        static_cast<stbtt_fontinfo*>(m_stbFontInfo),
        m_fontSize                           // حجم الخط / Font size
    );
    
    // حساب حجم الـ atlas (نبدأ بـ 512x512) / Calculate atlas size (start with 512x512)
    int atlasWidth = 512;                    // عرض atlas / Atlas width
    int atlasHeight = 512;                   // ارتفاع atlas / Atlas height
    
    // تخصيص ذاكرة للـ atlas / Allocate atlas memory
    std::vector<u8> atlasData(atlasWidth * atlasHeight, 0);  // بيانات atlas / Atlas data
    
    // متغيرات للتخطيط / Variables for packing
    int currentX = 1;                        // الموقع الحالي X / Current X position
    int currentY = 1;                        // الموقع الحالي Y / Current Y position
    int maxRowHeight = 0;                    // أقصى ارتفاع للصف / Max row height
    
    // توليد الأحرف / Generate glyphs
    for (u32 codepoint = firstChar; codepoint < firstChar + numChars; ++codepoint) {
        // الحصول على فهرس الحرف / Get glyph index
        int glyphIndex = stbtt_FindGlyphIndex(
            static_cast<stbtt_fontinfo*>(m_stbFontInfo),
            codepoint                        // رمز الحرف / Character codepoint
        );
        
        if (glyphIndex == 0 && codepoint != ' ') {  // تخطي الأحرف غير الموجودة / Skip missing glyphs
            continue;                        // الانتقال للتالي / Move to next
        }
        
        // الحصول على معلومات الحرف / Get glyph metrics
        int advanceWidth, leftSideBearing;   // معلومات التقدم / Advance metrics
        stbtt_GetGlyphHMetrics(
            static_cast<stbtt_fontinfo*>(m_stbFontInfo),
            glyphIndex,
            &advanceWidth, &leftSideBearing  // الحصول على القيم / Get values
        );
        
        // الحصول على صندوق الحرف / Get glyph bounding box
        int x0, y0, x1, y1;                  // صندوق الحرف / Glyph box
        stbtt_GetGlyphBitmapBox(
            static_cast<stbtt_fontinfo*>(m_stbFontInfo),
            glyphIndex, scale, scale,
            &x0, &y0, &x1, &y1               // الحصول على الصندوق / Get box
        );
        
        int glyphWidth = x1 - x0;            // عرض الحرف / Glyph width
        int glyphHeight = y1 - y0;           // ارتفاع الحرف / Glyph height
        
        // التحقق من المساحة في السطر الحالي / Check space in current row
        if (currentX + glyphWidth + 1 > atlasWidth) {
            currentX = 1;                    // العودة لبداية سطر جديد / Return to new row start
            currentY += maxRowHeight + 1;    // الانتقال للسطر التالي / Move to next row
            maxRowHeight = 0;                // إعادة تعيين أقصى ارتفاع / Reset max height
        }
        
        // التحقق من المساحة العمودية / Check vertical space
        if (currentY + glyphHeight + 1 > atlasHeight) {
            std::cerr << "(AR) تحذير: atlas ممتلئ / (EN) Warning: Atlas full" << std::endl;
            break;                           // التوقف / Stop
        }
        
        // رسم الحرف في الـ atlas / Render glyph to atlas
        if (glyphWidth > 0 && glyphHeight > 0) {
            stbtt_MakeGlyphBitmap(
                static_cast<stbtt_fontinfo*>(m_stbFontInfo),
                atlasData.data() + currentY * atlasWidth + currentX,  // الوجهة / Destination
                glyphWidth, glyphHeight,     // الأبعاد / Dimensions
                atlasWidth,                  // stride
                scale, scale,                // المقياس / Scale
                glyphIndex                   // فهرس الحرف / Glyph index
            );
        }
        
        // حفظ معلومات الحرف / Store glyph info
        GlyphInfo glyph;                     // معلومات الحرف / Glyph info
        glyph.codepoint = codepoint;         // رمز الحرف / Codepoint
        glyph.advanceX = advanceWidth * scale;  // المسافة للتالي / Advance
        glyph.bearingX = leftSideBearing * scale;  // إزاحة X / X bearing
        glyph.bearingY = -y0;                // إزاحة Y / Y bearing
        glyph.width = static_cast<float>(glyphWidth);   // العرض / Width
        glyph.height = static_cast<float>(glyphHeight); // الارتفاع / Height
        
        // حساب إحداثيات UV / Calculate UV coordinates
        glyph.u0 = static_cast<float>(currentX) / atlasWidth;           // U0
        glyph.v0 = static_cast<float>(currentY) / atlasHeight;          // V0
        glyph.u1 = static_cast<float>(currentX + glyphWidth) / atlasWidth;   // U1
        glyph.v1 = static_cast<float>(currentY + glyphHeight) / atlasHeight; // V1
        
        m_glyphs[codepoint] = glyph;         // حفظ الحرف / Store glyph
        
        // تحديث الموقع / Update position
        currentX += glyphWidth + 1;          // الانتقال لليمين / Move right
        maxRowHeight = std::max(maxRowHeight, glyphHeight);  // تحديث أقصى ارتفاع / Update max height
    }
    
    // إنشاء texture من البيانات / Create texture from data
    m_atlas = Texture::CreateFromMemory(
        atlasData.data(),                    // بيانات atlas / Atlas data
        atlasWidth, atlasHeight,             // الأبعاد / Dimensions
        TextureFormat::R8,                   // صيغة قناة واحدة / Single channel format
        TextureFilter::LINEAR,               // فلتر خطي / Linear filter
        TextureWrap::CLAMP_TO_EDGE             // تثبيت الحواف / Clamp edges
    );
    
    if (!m_atlas || !m_atlas->IsValid()) {   // التحقق من النجاح / Check success
        std::cerr << "(AR) فشل إنشاء texture atlas / (EN) Failed to create atlas texture" << std::endl;
        return false;                        // فشل / Failure
    }
    
    return true;                             // نجح / Success
}

// ==============================================================================
// قياس النصوص / Text Measurement
// ==============================================================================

/// حساب عرض نص / Calculate text width
float Font::MeasureText(const std::string& text) const {
    if (!m_isValid) {                        // التحقق من الصحة / Check validity
        return 0.0f;                         // إرجاع صفر / Return zero
    }
    
    float width = 0.0f;                      // العرض الإجمالي / Total width
    u32 prevCodepoint = 0;                   // الحرف السابق / Previous codepoint
    
    // المرور على كل حرف / Iterate through characters
    for (size_t i = 0; i < text.length(); ++i) {
        u32 codepoint = static_cast<u32>(static_cast<u8>(text[i]));  // الحرف الحالي / Current char
        
        // الحصول على معلومات الحرف / Get glyph info
        const GlyphInfo* glyph = GetGlyph(codepoint);
        
        if (glyph) {                         // إذا وُجد الحرف / If glyph exists
            width += glyph->advanceX;        // إضافة عرض الحرف / Add glyph width
            
            // إضافة kerning إذا كان متاحاً / Add kerning if available
            if (prevCodepoint != 0) {
                width += GetKerning(prevCodepoint, codepoint);
            }
        }
        
        prevCodepoint = codepoint;           // تحديث الحرف السابق / Update previous
    }
    
    return width;                            // إرجاع العرض / Return width
}

/// حساب أبعاد نص / Calculate text dimensions
void Font::MeasureText(const std::string& text, float& width, float& height) const {
    width = MeasureText(text);               // حساب العرض / Calculate width
    height = m_lineHeight;                   // ارتفاع سطر واحد / Single line height
    
    // التحقق من وجود أسطر متعددة / Check for multiple lines
    size_t lineCount = 1;                    // عدد الأسطر / Line count
    for (char c : text) {
        if (c == '\n') {                     // إذا كان سطر جديد / If newline
            lineCount++;                     // زيادة العدد / Increment count
        }
    }
    
    height = lineCount * m_lineHeight;       // حساب الارتفاع الكلي / Calculate total height
}

// ==============================================================================
// الوصول للمعلومات / Information Access
// ==============================================================================

/// الحصول على معلومات حرف / Get glyph information
const GlyphInfo* Font::GetGlyph(u32 codepoint) const {
    auto it = m_glyphs.find(codepoint);      // البحث عن الحرف / Search for glyph
    
    if (it != m_glyphs.end()) {              // إذا وُجد / If found
        return &it->second;                  // إرجاع المعلومات / Return info
    }
    
    // إرجاع null إذا لم يُوجد / Return null if not found
    return nullptr;                          // لم يُوجد / Not found
}

/// الحصول على معلومات kerning / Get kerning information
float Font::GetKerning(u32 char1, u32 char2) const {
    if (!m_stbFontInfo) {                    // التحقق من التهيئة / Check initialization
        return 0.0f;                         // إرجاع صفر / Return zero
    }
    
    // الحصول على فهارس الأحرف / Get glyph indices
    int glyph1 = stbtt_FindGlyphIndex(
        static_cast<stbtt_fontinfo*>(m_stbFontInfo), 
        char1                                // الحرف الأول / First char
    );
    
    int glyph2 = stbtt_FindGlyphIndex(
        static_cast<stbtt_fontinfo*>(m_stbFontInfo), 
        char2                                // الحرف الثاني / Second char
    );
    
    // الحصول على kerning / Get kerning
    int kern = stbtt_GetGlyphKernAdvance(
        static_cast<stbtt_fontinfo*>(m_stbFontInfo),
        glyph1, glyph2                       // الحرفان / Two glyphs
    );
    
    // حساب scale / Calculate scale
    float scale = stbtt_ScaleForPixelHeight(
        static_cast<stbtt_fontinfo*>(m_stbFontInfo),
        m_fontSize                           // حجم الخط / Font size
    );
    
    return kern * scale;                     // إرجاع kerning مقاساً / Return scaled kerning
}

// ==============================================================================
// تحرير الموارد / Resource Management
// ==============================================================================

/// تحرير موارد الخط / Release font resources
void Font::Free() {
    // تحرير بيانات الخط / Free font data
    if (m_ownsData && m_fontData) {          // إذا كنا نمتلك البيانات / If we own data
        delete[] m_fontData;                 // تحرير الذاكرة / Free memory
        m_fontData = nullptr;                // مسح المؤشر / Clear pointer
    }
    
    // تحرير معلومات stb / Free stb info
    if (m_stbFontInfo) {                     // إذا كانت موجودة / If exists
        delete static_cast<stbtt_fontinfo*>(m_stbFontInfo);  // تحرير / Free
        m_stbFontInfo = nullptr;             // مسح / Clear
    }
    
    // مسح البيانات / Clear data
    m_glyphs.clear();                        // مسح الأحرف / Clear glyphs
    m_atlas.reset();                         // إعادة تعيين atlas / Reset atlas
    
    // إعادة تعيين القيم / Reset values
    m_fontDataSize = 0;                      // حجم صفر / Zero size
    m_ownsData = false;                      // لا نمتلك البيانات / Don't own data
    m_fontSize = 0.0f;                       // حجم صفر / Zero size
    m_lineHeight = 0.0f;                     // ارتفاع صفر / Zero height
    m_baseline = 0.0f;                       // baseline صفر / Zero baseline
    m_ascent = 0.0f;                         // ascent صفر / Zero ascent
    m_descent = 0.0f;                        // descent صفر / Zero descent
    m_isValid = false;                       // غير صالح / Invalid
}

// ==============================================================================
// توليد Atlas موسع / Extended Atlas Generation  
// ==============================================================================

/// توليد atlas موسع مع نطاقات Unicode متعددة
/// Generate extended atlas with multiple Unicode ranges
bool Font::GenerateExtendedAtlas(const std::vector<ArabicText::UnicodeRange>& ranges) {
    if (!m_stbFontInfo) return false;
    
    float scale = stbtt_ScaleForPixelHeight(
        static_cast<stbtt_fontinfo*>(m_stbFontInfo), m_fontSize);
    
    // حساب حجم أكبر للـ atlas (1024x1024 للحروف العربية)
    // Larger atlas for Arabic character set
    int atlasWidth = 1024;
    int atlasHeight = 1024;
    
    std::vector<u8> atlasData(atlasWidth * atlasHeight, 0);
    
    int currentX = 1;
    int currentY = 1;
    int maxRowHeight = 0;
    
    // المرور على كل نطاق / Iterate through each range
    for (const auto& range : ranges) {
        for (u32 codepoint = range.first; codepoint <= range.last; ++codepoint) {
            int glyphIndex = stbtt_FindGlyphIndex(
                static_cast<stbtt_fontinfo*>(m_stbFontInfo), codepoint);
            
            if (glyphIndex == 0 && codepoint != ' ') continue;
            
            int advanceWidth, leftSideBearing;
            stbtt_GetGlyphHMetrics(
                static_cast<stbtt_fontinfo*>(m_stbFontInfo),
                glyphIndex, &advanceWidth, &leftSideBearing);
            
            int x0, y0, x1, y1;
            stbtt_GetGlyphBitmapBox(
                static_cast<stbtt_fontinfo*>(m_stbFontInfo),
                glyphIndex, scale, scale, &x0, &y0, &x1, &y1);
            
            int glyphWidth = x1 - x0;
            int glyphHeight = y1 - y0;
            
            if (currentX + glyphWidth + 1 > atlasWidth) {
                currentX = 1;
                currentY += maxRowHeight + 1;
                maxRowHeight = 0;
            }
            
            if (currentY + glyphHeight + 1 > atlasHeight) {
                // النص العربي يحتاج atlas أكبر / Need bigger atlas
                // إعادة المحاولة مع atlas أكبر / Retry with larger atlas
                if (atlasWidth < 4096) {
                    atlasWidth *= 2;
                    atlasHeight *= 2;
                    atlasData.assign(atlasWidth * atlasHeight, 0);
                    currentX = 1;
                    currentY = 1;
                    maxRowHeight = 0;
                    m_glyphs.clear();
                    // إعادة توليد من البداية / Regenerate from start
                    return GenerateExtendedAtlas(ranges);
                }
                std::cerr << "(AR) تحذير: atlas ممتلئ / (EN) Warning: Atlas full at " 
                          << atlasWidth << "x" << atlasHeight << std::endl;
                break;
            }
            
            if (glyphWidth > 0 && glyphHeight > 0) {
                stbtt_MakeGlyphBitmap(
                    static_cast<stbtt_fontinfo*>(m_stbFontInfo),
                    atlasData.data() + currentY * atlasWidth + currentX,
                    glyphWidth, glyphHeight, atlasWidth,
                    scale, scale, glyphIndex);
            }
            
            GlyphInfo glyph;
            glyph.codepoint = codepoint;
            glyph.advanceX = advanceWidth * scale;
            glyph.bearingX = leftSideBearing * scale;
            glyph.bearingY = -y0;
            glyph.width = static_cast<float>(glyphWidth);
            glyph.height = static_cast<float>(glyphHeight);
            glyph.u0 = static_cast<float>(currentX) / atlasWidth;
            glyph.v0 = static_cast<float>(currentY) / atlasHeight;
            glyph.u1 = static_cast<float>(currentX + glyphWidth) / atlasWidth;
            glyph.v1 = static_cast<float>(currentY + glyphHeight) / atlasHeight;
            
            m_glyphs[codepoint] = glyph;
            
            currentX += glyphWidth + 1;
            maxRowHeight = std::max(maxRowHeight, glyphHeight);
        }
    }
    
    m_atlas = Texture::CreateFromMemory(
        atlasData.data(), atlasWidth, atlasHeight,
        TextureFormat::R8, TextureFilter::LINEAR, TextureWrap::CLAMP_TO_EDGE);
    
    if (!m_atlas || !m_atlas->IsValid()) {
        std::cerr << "(AR) فشل إنشاء atlas موسع / (EN) Failed to create extended atlas" << std::endl;
        return false;
    }
    
    return true;
}

/// توليد atlas عربي / Generate Arabic atlas
bool Font::GenerateArabicAtlas() {
    return GenerateExtendedAtlas(ArabicText::GetFullRanges());
}

// ==============================================================================
// قياس النصوص مع UTF-8 / Text Measurement with UTF-8
// ==============================================================================

float Font::MeasureTextUTF8(const std::string& text) const {
    if (!m_isValid) return 0.0f;
    
    auto codepoints = ArabicText::DecodeUTF8(text);
    
    float width = 0.0f;
    u32 prevCodepoint = 0;
    
    for (u32 cp : codepoints) {
        if (ArabicText::IsDiacritic(cp)) continue; // التشكيل لا يأخذ عرض / Diacritics don't advance
        
        const GlyphInfo* glyph = GetGlyph(cp);
        if (glyph) {
            width += glyph->advanceX;
            if (prevCodepoint != 0) {
                width += GetKerning(prevCodepoint, cp);
            }
        }
        prevCodepoint = cp;
    }
    
    return width;
}

void Font::MeasureTextUTF8(const std::string& text, float& width, float& height) const {
    width = MeasureTextUTF8(text);
    height = m_lineHeight;
    
    auto codepoints = ArabicText::DecodeUTF8(text);
    size_t lineCount = 1;
    for (u32 cp : codepoints) {
        if (cp == '\n') lineCount++;
    }
    height = lineCount * m_lineHeight;
}

// ==============================================================================
// دوال إنشاء ساكنة / Static Creation Functions
// ==============================================================================

/// إنشاء خط من ملف / Create font from file
std::shared_ptr<Font> Font::CreateFromFile(const std::string& filepath, float fontSize) {
    auto font = std::make_shared<Font>();    // إنشاء خط جديد / Create new font
    
    if (!font->LoadFromFile(filepath, fontSize)) {  // محاولة التحميل / Try loading
        std::cerr << "(AR) فشل إنشاء خط من: " << filepath << std::endl;
        std::cerr << "(EN) Failed to create font from: " << filepath << std::endl;
        return nullptr;                      // فشل / Failure
    }
    
    return font;                             // إرجاع الخط / Return font
}

/// إنشاء خط افتراضي / Create default font
std::shared_ptr<Font> Font::CreateDefault(float fontSize) {
    // ملاحظة: في التطبيق الحقيقي، يجب تضمين خط افتراضي
    // Note: In real implementation, should embed a default font
    
    // محاولة تحميل خط من النظام / Try loading system font
    std::vector<std::string> defaultFonts = {
        "C:/Windows/Fonts/arial.ttf",       // Windows
        "/System/Library/Fonts/Helvetica.ttc",  // macOS
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",  // Linux
        "fonts/default.ttf"                  // خط مضمن / Embedded font
    };
    
    for (const auto& path : defaultFonts) {  // المحاولة مع كل مسار / Try each path
        auto font = CreateFromFile(path, fontSize);
        if (font) {                          // إذا نجح / If succeeded
            return font;                     // إرجاع الخط / Return font
        }
    }
    
    std::cerr << "(AR) فشل إنشاء خط افتراضي! / (EN) Failed to create default font!" << std::endl;
    return nullptr;                          // فشل / Failure
}

} // namespace graphics
} // namespace sad
