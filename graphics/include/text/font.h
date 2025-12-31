// font.h - نظام الخطوط
// Font system
// يوفر واجهة لتحميل وإدارة الخطوط TrueType
// Provides interface for loading and managing TrueType fonts

#pragma once

#include "../core/types.h"
#include "../texture/texture.h"
#include <string>
#include <memory>
#include <map>

namespace sad {
namespace graphics {

// معلومات حرف / Glyph information
struct GlyphInfo {
    u32 codepoint;              // رمز الحرف Unicode - Unicode codepoint
    float advanceX;             // المسافة للحرف التالي - Advance to next char
    float bearingX;             // إزاحة X من نقطة الرسم - X offset from draw point
    float bearingY;             // إزاحة Y من نقطة الرسم - Y offset from draw point
    float width;                // عرض الحرف - Glyph width
    float height;               // ارتفاع الحرف - Glyph height
    
    // إحداثيات UV في الـ texture atlas
    // UV coordinates in texture atlas
    float u0, v0;               // أعلى يسار - Top-left UV
    float u1, v1;               // أسفل يمين - Bottom-right UV
};

// فئة Font - إدارة الخطوط
// Font class - manages TrueType fonts
class Font {
public:
    // المُنشئ الافتراضي - Default constructor
    Font();
    
    // المُنشئ بمسار ملف - Constructor with file path
    // filepath: مسار ملف الخط TTF - TTF font file path
    // fontSize: حجم الخط بالبكسل - Font size in pixels
    explicit Font(const std::string& filepath, float fontSize = 24.0f);
    
    // المُدمر - Destructor
    ~Font();
    
    // منع النسخ - Prevent copying
    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;
    
    // السماح بالنقل - Allow moving
    Font(Font&& other) noexcept;
    Font& operator=(Font&& other) noexcept;
    
    // تحميل خط من ملف - Load font from file
    // filepath: مسار ملف الخط - Font file path
    // fontSize: حجم الخط - Font size
    // returns: نجح التحميل - Loading succeeded
    bool LoadFromFile(const std::string& filepath, float fontSize = 24.0f);
    
    // تحميل خط من الذاكرة - Load font from memory
    // data: بيانات الخط - Font data
    // size: حجم البيانات - Data size
    // fontSize: حجم الخط - Font size
    // returns: نجح التحميل - Loading succeeded
    bool LoadFromMemory(const u8* data, size_t size, float fontSize = 24.0f);
    
    // توليد texture atlas للخط - Generate texture atlas for font
    // firstChar: أول حرف - First character
    // numChars: عدد الأحرف - Number of characters
    // returns: نجح التوليد - Generation succeeded
    bool GenerateAtlas(u32 firstChar = 32, u32 numChars = 96);
    
    // الحصول على معلومات حرف - Get glyph information
    // codepoint: رمز الحرف - Character codepoint
    // returns: معلومات الحرف - Glyph info
    const GlyphInfo* GetGlyph(u32 codepoint) const;
    
    // حساب عرض نص - Calculate text width
    // text: النص المراد قياسه - Text to measure
    // returns: العرض بالبكسل - Width in pixels
    float MeasureText(const std::string& text) const;
    
    // حساب أبعاد نص - Calculate text dimensions
    // text: النص المراد قياسه - Text to measure
    // width: العرض (output) - Width (output)
    // height: الارتفاع (output) - Height (output)
    void MeasureText(const std::string& text, float& width, float& height) const;
    
    // التحقق من صحة الخط - Check if font is valid
    // returns: الخط صالح للاستخدام - Font is valid
    bool IsValid() const { return m_isValid && m_atlas != nullptr; }
    
    // الحصول على الـ texture atlas - Get texture atlas
    // returns: texture الأحرف - Glyph texture
    TextureRef GetAtlas() const { return m_atlas; }
    
    // الحصول على حجم الخط - Get font size
    // returns: حجم الخط بالبكسل - Font size in pixels
    float GetFontSize() const { return m_fontSize; }
    
    // الحصول على ارتفاع السطر - Get line height
    // returns: ارتفاع السطر - Line height
    float GetLineHeight() const { return m_lineHeight; }
    
    // الحصول على المسافة الأساسية - Get baseline
    // returns: المسافة من الأعلى للخط الأساسي - Distance from top to baseline
    float GetBaseline() const { return m_baseline; }
    
    // الحصول على ascent (المسافة فوق baseline)
    // Get ascent (distance above baseline)
    // returns: ascent بالبكسل - Ascent in pixels
    float GetAscent() const { return m_ascent; }
    
    // الحصول على descent (المسافة تحت baseline)
    // Get descent (distance below baseline)
    // returns: descent بالبكسل - Descent in pixels
    float GetDescent() const { return m_descent; }
    
    // تحرير موارد الخط - Release font resources
    void Free();
    
    // إنشاء خط من ملف - Create font from file
    // filepath: مسار الملف - File path
    // fontSize: حجم الخط - Font size
    // returns: shared pointer للخط - Shared pointer to font
    static std::shared_ptr<Font> CreateFromFile(const std::string& filepath, float fontSize = 24.0f);
    
    // إنشاء خط افتراضي - Create default font
    // returns: shared pointer لخط افتراضي - Shared pointer to default font
    static std::shared_ptr<Font> CreateDefault(float fontSize = 24.0f);

private:
    // بناء atlas من بيانات الخط - Build atlas from font data
    bool BuildAtlas(u32 firstChar, u32 numChars);
    
    // الحصول على معلومات kerning - Get kerning information
    // char1, char2: الحرفان - Two characters
    // returns: مسافة kerning - Kerning distance
    float GetKerning(u32 char1, u32 char2) const;

private:
    u8* m_fontData;                     // بيانات الخط - Font data
    size_t m_fontDataSize;              // حجم بيانات الخط - Font data size
    bool m_ownsData;                    // هل نمتلك البيانات - Do we own data
    
    float m_fontSize;                   // حجم الخط - Font size
    float m_lineHeight;                 // ارتفاع السطر - Line height
    float m_baseline;                   // الخط الأساسي - Baseline
    float m_ascent;                     // ascent (فوق baseline)
    float m_descent;                    // descent (تحت baseline)
    
    std::map<u32, GlyphInfo> m_glyphs;  // خريطة الأحرف - Glyph map
    TextureRef m_atlas;                 // texture الأحرف - Glyph atlas texture
    
    bool m_isValid;                     // صحة الخط - Font validity
    
    // معلومات stb_truetype الداخلية - Internal stb_truetype info
    void* m_stbFontInfo;                // معلومات الخط - Font info
};

// أنواع مستعارة - Type aliases
using FontRef = std::shared_ptr<Font>;

} // namespace graphics
} // namespace sad
