// ==============================================================================
// arabic_text.h - دعم النصوص العربية / Arabic Text Support
// ==============================================================================
// الوصف: فك ترميز UTF-8، تشكيل الحروف العربية، ودعم RTL
// Description: UTF-8 decoding, Arabic character shaping, and RTL support
// ==============================================================================

#ifndef SAD_GRAPHICS_ARABIC_TEXT_H
#define SAD_GRAPHICS_ARABIC_TEXT_H

#include "../core/types.h"
#include <string>
#include <vector>

namespace sad {
namespace graphics {

// ============================================================================
// اتجاه النص / Text Direction
// ============================================================================

/// اتجاه النص / Text direction
enum class TextDirection {
    LTR,        ///< من اليسار لليمين / Left-to-right
    RTL,        ///< من اليمين لليسار / Right-to-left
    Auto        ///< تحديد تلقائي / Auto-detect from first strong character
};

// ============================================================================
// سياق الحرف العربي / Arabic Character Context
// ============================================================================

/// موضع الحرف في الكلمة / Character position in word
enum class ArabicForm {
    Isolated,   ///< منفصل / Standalone
    Initial,    ///< بداية / Beginning of word
    Medial,     ///< وسط / Middle of word
    Final       ///< نهاية / End of word
};

// ============================================================================
// معلومات حرف Unicode / Unicode Character Info
// ============================================================================

/// نتيجة فك ترميز حرف / Decoded character result
struct DecodedChar {
    u32 codepoint;      ///< رمز Unicode / Unicode codepoint
    size_t bytesUsed;   ///< عدد البايتات المستخدمة / Number of bytes consumed
};

// ============================================================================
// أشكال الحروف العربية / Arabic Letter Forms
// ============================================================================

/// أشكال حرف عربي (منفصل، بداية، وسط، نهاية)
/// Arabic letter forms (isolated, initial, medial, final)
struct ArabicLetterForms {
    u32 base;       ///< الشكل الأساسي / Base form (e.g., U+0628 ب)
    u32 isolated;   ///< المنفصل / Isolated form
    u32 initial;    ///< البداية / Initial form
    u32 medial;     ///< الوسط / Medial form
    u32 final_;     ///< النهاية / Final form ('final' is reserved keyword)
    bool joins_right; ///< يتصل من اليمين / Joins from right
    bool joins_left;  ///< يتصل من اليسار / Joins from left
};

// ============================================================================
// فئة ArabicText - أدوات النصوص العربية
// ============================================================================

class ArabicText {
public:
    // ==================================================================
    // فك ترميز UTF-8 / UTF-8 Decoding
    // ==================================================================
    
    /// فك ترميز حرف UTF-8 واحد / Decode a single UTF-8 character
    /// data: مؤشر لبيانات UTF-8 / Pointer to UTF-8 data
    /// length: طول البيانات المتبقية / Remaining data length
    /// returns: حرف مفكوك / Decoded character
    static DecodedChar DecodeUTF8Char(const char* data, size_t length);
    
    /// فك ترميز نص UTF-8 كامل إلى رموز Unicode / Decode full UTF-8 string to codepoints
    /// text: النص UTF-8 / UTF-8 string
    /// returns: مصفوفة رموز Unicode / Array of Unicode codepoints
    static std::vector<u32> DecodeUTF8(const std::string& text);
    
    /// ترميز رموز Unicode إلى UTF-8 / Encode Unicode codepoints to UTF-8
    /// codepoints: رموز Unicode / Unicode codepoints
    /// returns: نص UTF-8 / UTF-8 string
    static std::string EncodeUTF8(const std::vector<u32>& codepoints);

    /// ترميز رمز Unicode واحد إلى UTF-8 / Encode single codepoint to UTF-8
    static std::string EncodeUTF8Char(u32 codepoint);
    
    // ==================================================================
    // تحديد نوع الحرف / Character Classification
    // ==================================================================
    
    /// هل الحرف عربي؟ / Is character Arabic?
    static bool IsArabic(u32 codepoint);
    
    /// هل الحرف عربي يتصل؟ / Is Arabic joining character?
    static bool IsArabicJoining(u32 codepoint);
    
    /// هل الحرف RTL؟ / Is character Right-to-Left?
    static bool IsRTL(u32 codepoint);

    /// هل الحرف تشكيل (حركة)؟ / Is character a diacritical mark?
    static bool IsDiacritic(u32 codepoint);
    
    // ==================================================================
    // تشكيل الحروف / Character Shaping
    // ==================================================================
    
    /// تشكيل نص عربي / Shape Arabic text
    /// codepoints: الأحرف الأصلية / Original characters
    /// returns: الأحرف بعد التشكيل / Shaped characters (presentation forms)
    static std::vector<u32> ShapeArabic(const std::vector<u32>& codepoints);
    
    /// الحصول على الشكل العربي لحرف / Get Arabic presentation form
    /// codepoint: الحرف الأساسي / Base character
    /// form: الشكل المطلوب / Desired form
    /// returns: رمز شكل العرض / Presentation form codepoint
    static u32 GetArabicForm(u32 codepoint, ArabicForm form);
    
    // ==================================================================
    // اتجاه النص / Text Direction
    // ==================================================================
    
    /// تحديد اتجاه النص / Detect text direction
    /// codepoints: رموز Unicode / Unicode codepoints
    /// returns: الاتجاه المحدد / Detected direction
    static TextDirection DetectDirection(const std::vector<u32>& codepoints);
    
    /// إعادة ترتيب النص ثنائي الاتجاه (BiDi بسيط)
    /// Simple BiDi reordering for mixed LTR/RTL text
    /// codepoints: الأحرف / Characters
    /// returns: الأحرف مُرتّبة بصرياً / Visually ordered characters
    static std::vector<u32> ReorderBiDi(const std::vector<u32>& codepoints);
    
    // ==================================================================
    // نطاقات Unicode العربية / Arabic Unicode Ranges
    // ==================================================================
    
    /// نطاقات Unicode للأحرف العربية / Unicode ranges for Arabic characters
    struct UnicodeRange {
        u32 first;   ///< أول رمز / First codepoint
        u32 last;    ///< آخر رمز / Last codepoint
    };
    
    /// الحصول على نطاقات Unicode العربية / Get Arabic Unicode ranges
    static std::vector<UnicodeRange> GetArabicRanges();
    
    /// الحصول على جميع النطاقات (ASCII + عربي) / Get all ranges (ASCII + Arabic)  
    static std::vector<UnicodeRange> GetFullRanges();

private:
    /// جدول أشكال الحروف العربية / Arabic letter forms table
    static const ArabicLetterForms s_arabicForms[];
    
    /// عدد الحروف في الجدول / Number of letters in table
    static const size_t s_arabicFormsCount;
    
    /// البحث عن أشكال حرف / Find forms for a character
    static const ArabicLetterForms* FindLetterForms(u32 codepoint);
};

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_ARABIC_TEXT_H
