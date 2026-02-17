// ==============================================================================
// arabic_text.cpp - تطبيق دعم النصوص العربية / Arabic Text Implementation
// ==============================================================================
// الوصف: تطبيق فك ترميز UTF-8، تشكيل الحروف العربية، ودعم RTL
// Description: UTF-8 decoding, Arabic shaping, and RTL support implementation
// ==============================================================================

#include "../../include/text/arabic_text.h"
#include <algorithm>
#include <cstring>

namespace sad {
namespace graphics {

// ==============================================================================
// جدول أشكال الحروف العربية / Arabic Letter Forms Table
// ==============================================================================
// كل حرف عربي له 4 أشكال: منفصل، بداية، وسط، نهاية
// Each Arabic letter has 4 forms: isolated, initial, medial, final
// القيم من Unicode Arabic Presentation Forms-B (FE70-FEFF)
// Values from Unicode Arabic Presentation Forms-B
// ==============================================================================

const ArabicLetterForms ArabicText::s_arabicForms[] = {
    // base, isolated, initial, medial, final, joins_right, joins_left
    // الهمزة / Hamza
    { 0x0621, 0xFE80, 0xFE80, 0xFE80, 0xFE80, false, false }, // ء
    // ألف بهمزة فوق / Alef with Hamza above
    { 0x0623, 0xFE83, 0xFE83, 0xFE84, 0xFE84, true,  false }, // أ
    // واو بهمزة / Waw with Hamza
    { 0x0624, 0xFE85, 0xFE85, 0xFE86, 0xFE86, true,  false }, // ؤ
    // ألف بهمزة تحت / Alef with Hamza below
    { 0x0625, 0xFE87, 0xFE87, 0xFE88, 0xFE88, true,  false }, // إ
    // ياء بهمزة / Yaa with Hamza
    { 0x0626, 0xFE89, 0xFE8B, 0xFE8C, 0xFE8A, true,  true  }, // ئ
    // ألف / Alef
    { 0x0627, 0xFE8D, 0xFE8D, 0xFE8E, 0xFE8E, true,  false }, // ا
    // باء / Baa
    { 0x0628, 0xFE8F, 0xFE91, 0xFE92, 0xFE90, true,  true  }, // ب
    // تاء مربوطة / Taa Marbuta
    { 0x0629, 0xFE93, 0xFE93, 0xFE94, 0xFE94, true,  false }, // ة
    // تاء / Taa
    { 0x062A, 0xFE95, 0xFE97, 0xFE98, 0xFE96, true,  true  }, // ت
    // ثاء / Thaa
    { 0x062B, 0xFE99, 0xFE9B, 0xFE9C, 0xFE9A, true,  true  }, // ث
    // جيم / Jeem
    { 0x062C, 0xFE9D, 0xFE9F, 0xFEA0, 0xFE9E, true,  true  }, // ج
    // حاء / Haa
    { 0x062D, 0xFEA1, 0xFEA3, 0xFEA4, 0xFEA2, true,  true  }, // ح
    // خاء / Khaa
    { 0x062E, 0xFEA5, 0xFEA7, 0xFEA8, 0xFEA6, true,  true  }, // خ
    // دال / Dal
    { 0x062F, 0xFEA9, 0xFEA9, 0xFEAA, 0xFEAA, true,  false }, // د
    // ذال / Thal
    { 0x0630, 0xFEAB, 0xFEAB, 0xFEAC, 0xFEAC, true,  false }, // ذ
    // راء / Raa
    { 0x0631, 0xFEAD, 0xFEAD, 0xFEAE, 0xFEAE, true,  false }, // ر
    // زاي / Zain
    { 0x0632, 0xFEAF, 0xFEAF, 0xFEB0, 0xFEB0, true,  false }, // ز
    // سين / Seen
    { 0x0633, 0xFEB1, 0xFEB3, 0xFEB4, 0xFEB2, true,  true  }, // س
    // شين / Sheen
    { 0x0634, 0xFEB5, 0xFEB7, 0xFEB8, 0xFEB6, true,  true  }, // ش
    // صاد / Sad
    { 0x0635, 0xFEB9, 0xFEBB, 0xFEBC, 0xFEBA, true,  true  }, // ص
    // ضاد / Dad
    { 0x0636, 0xFEBD, 0xFEBF, 0xFEC0, 0xFEBE, true,  true  }, // ض
    // طاء / Taa (emphatic)
    { 0x0637, 0xFEC1, 0xFEC3, 0xFEC4, 0xFEC2, true,  true  }, // ط
    // ظاء / Zaa (emphatic)
    { 0x0638, 0xFEC5, 0xFEC7, 0xFEC8, 0xFEC6, true,  true  }, // ظ
    // عين / Ain
    { 0x0639, 0xFEC9, 0xFECB, 0xFECC, 0xFECA, true,  true  }, // ع
    // غين / Ghain
    { 0x063A, 0xFECD, 0xFECF, 0xFED0, 0xFECE, true,  true  }, // غ
    // فاء / Faa
    { 0x0641, 0xFED1, 0xFED3, 0xFED4, 0xFED2, true,  true  }, // ف
    // قاف / Qaf
    { 0x0642, 0xFED5, 0xFED7, 0xFED8, 0xFED6, true,  true  }, // ق
    // كاف / Kaf
    { 0x0643, 0xFED9, 0xFEDB, 0xFEDC, 0xFEDA, true,  true  }, // ك
    // لام / Lam
    { 0x0644, 0xFEDD, 0xFEDF, 0xFEE0, 0xFEDE, true,  true  }, // ل
    // ميم / Meem
    { 0x0645, 0xFEE1, 0xFEE3, 0xFEE4, 0xFEE2, true,  true  }, // م
    // نون / Noon
    { 0x0646, 0xFEE5, 0xFEE7, 0xFEE8, 0xFEE6, true,  true  }, // ن
    // هاء / Haa
    { 0x0647, 0xFEE9, 0xFEEB, 0xFEEC, 0xFEEA, true,  true  }, // ه
    // واو / Waw
    { 0x0648, 0xFEED, 0xFEED, 0xFEEE, 0xFEEE, true,  false }, // و
    // ألف مقصورة / Alef Maqsura
    { 0x0649, 0xFEEF, 0xFEEF, 0xFEF0, 0xFEF0, true,  false }, // ى
    // ياء / Yaa
    { 0x064A, 0xFEF1, 0xFEF3, 0xFEF4, 0xFEF2, true,  true  }, // ي
    // لام ألف / Lam-Alef ligatures
    { 0x0622, 0xFE81, 0xFE81, 0xFE82, 0xFE82, true,  false }, // آ (Alef Madda)
};

const size_t ArabicText::s_arabicFormsCount = sizeof(s_arabicForms) / sizeof(s_arabicForms[0]);

// ==============================================================================
// فك ترميز UTF-8 / UTF-8 Decoding
// ==============================================================================

DecodedChar ArabicText::DecodeUTF8Char(const char* data, size_t length) {
    DecodedChar result = { 0xFFFD, 1 }; // حرف بديل افتراضي / Default replacement char
    
    if (!data || length == 0) {
        result.bytesUsed = 0;
        return result;
    }
    
    u8 byte = static_cast<u8>(data[0]);
    
    if (byte < 0x80) {
        // ASCII - بايت واحد / Single byte
        result.codepoint = byte;
        result.bytesUsed = 1;
    }
    else if ((byte & 0xE0) == 0xC0 && length >= 2) {
        // بايتان / Two bytes (U+0080 to U+07FF)
        u8 b1 = static_cast<u8>(data[1]);
        if ((b1 & 0xC0) == 0x80) {
            result.codepoint = ((byte & 0x1F) << 6) | (b1 & 0x3F);
            result.bytesUsed = 2;
            // رفض ترميز زائد / Reject overlong encoding
            if (result.codepoint < 0x80) {
                result.codepoint = 0xFFFD;
            }
        }
    }
    else if ((byte & 0xF0) == 0xE0 && length >= 3) {
        // ثلاثة بايتات / Three bytes (U+0800 to U+FFFF)
        u8 b1 = static_cast<u8>(data[1]);
        u8 b2 = static_cast<u8>(data[2]);
        if ((b1 & 0xC0) == 0x80 && (b2 & 0xC0) == 0x80) {
            result.codepoint = ((byte & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F);
            result.bytesUsed = 3;
            // رفض ترميز زائد / Reject overlong
            if (result.codepoint < 0x800) {
                result.codepoint = 0xFFFD;
            }
        }
    }
    else if ((byte & 0xF8) == 0xF0 && length >= 4) {
        // أربعة بايتات / Four bytes (U+10000 to U+10FFFF)
        u8 b1 = static_cast<u8>(data[1]);
        u8 b2 = static_cast<u8>(data[2]);
        u8 b3 = static_cast<u8>(data[3]);
        if ((b1 & 0xC0) == 0x80 && (b2 & 0xC0) == 0x80 && (b3 & 0xC0) == 0x80) {
            result.codepoint = ((byte & 0x07) << 18) | ((b1 & 0x3F) << 12) |
                              ((b2 & 0x3F) << 6) | (b3 & 0x3F);
            result.bytesUsed = 4;
            // رفض ترميز زائد وقيم خارج النطاق / Reject overlong and out of range
            if (result.codepoint < 0x10000 || result.codepoint > 0x10FFFF) {
                result.codepoint = 0xFFFD;
            }
        }
    }
    
    return result;
}

std::vector<u32> ArabicText::DecodeUTF8(const std::string& text) {
    std::vector<u32> codepoints;
    codepoints.reserve(text.size()); // حجز أقصى حجم ممكن / Reserve max possible size
    
    size_t i = 0;
    while (i < text.size()) {
        DecodedChar dc = DecodeUTF8Char(text.data() + i, text.size() - i);
        if (dc.bytesUsed == 0) break;
        codepoints.push_back(dc.codepoint);
        i += dc.bytesUsed;
    }
    
    return codepoints;
}

std::string ArabicText::EncodeUTF8Char(u32 cp) {
    std::string result;
    
    if (cp < 0x80) {
        result += static_cast<char>(cp);
    }
    else if (cp < 0x800) {
        result += static_cast<char>(0xC0 | (cp >> 6));
        result += static_cast<char>(0x80 | (cp & 0x3F));
    }
    else if (cp < 0x10000) {
        result += static_cast<char>(0xE0 | (cp >> 12));
        result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (cp & 0x3F));
    }
    else if (cp <= 0x10FFFF) {
        result += static_cast<char>(0xF0 | (cp >> 18));
        result += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (cp & 0x3F));
    }
    
    return result;
}

std::string ArabicText::EncodeUTF8(const std::vector<u32>& codepoints) {
    std::string result;
    result.reserve(codepoints.size() * 3); // تقدير حجم / Estimate size
    
    for (u32 cp : codepoints) {
        result += EncodeUTF8Char(cp);
    }
    
    return result;
}

// ==============================================================================
// تحديد نوع الحرف / Character Classification
// ==============================================================================

bool ArabicText::IsArabic(u32 cp) {
    return (cp >= 0x0600 && cp <= 0x06FF) ||  // Arabic block
           (cp >= 0x0750 && cp <= 0x077F) ||  // Arabic Supplement
           (cp >= 0x08A0 && cp <= 0x08FF) ||  // Arabic Extended-A
           (cp >= 0xFB50 && cp <= 0xFDFF) ||  // Arabic Presentation Forms-A
           (cp >= 0xFE70 && cp <= 0xFEFF);    // Arabic Presentation Forms-B
}

bool ArabicText::IsArabicJoining(u32 cp) {
    // الحروف العربية التي تتصل / Arabic joining characters
    return (cp >= 0x0621 && cp <= 0x064A) && // الأبجدية الأساسية / Basic alphabet
           cp != 0x0640;                      // تاتاكال (لا يتصل) / Tatweel excluded
}

bool ArabicText::IsRTL(u32 cp) {
    // الأحرف ذات الاتجاه الأيمن-أيسر
    // Right-to-left characters
    return (cp >= 0x0590 && cp <= 0x05FF) ||  // Hebrew
           (cp >= 0x0600 && cp <= 0x06FF) ||  // Arabic
           (cp >= 0x0700 && cp <= 0x074F) ||  // Syriac
           (cp >= 0x0750 && cp <= 0x077F) ||  // Arabic Supplement
           (cp >= 0x0780 && cp <= 0x07BF) ||  // Thaana
           (cp >= 0x08A0 && cp <= 0x08FF) ||  // Arabic Extended-A
           (cp >= 0xFB50 && cp <= 0xFDFF) ||  // Arabic Presentation Forms-A
           (cp >= 0xFE70 && cp <= 0xFEFF);    // Arabic Presentation Forms-B
}

bool ArabicText::IsDiacritic(u32 cp) {
    // التشكيل العربي / Arabic diacritical marks
    return (cp >= 0x064B && cp <= 0x065F) ||  // فتحة، ضمة، كسرة... / Fathah, Dammah, Kasrah...
           (cp >= 0x0610 && cp <= 0x061A) ||  // علامات فوق الحرف / Above marks
           (cp == 0x0670);                     // ألف خنجرية / Superscript Alef
}

// ==============================================================================
// تشكيل الحروف العربية / Arabic Shaping
// ==============================================================================

const ArabicLetterForms* ArabicText::FindLetterForms(u32 codepoint) {
    for (size_t i = 0; i < s_arabicFormsCount; ++i) {
        if (s_arabicForms[i].base == codepoint) {
            return &s_arabicForms[i];
        }
    }
    return nullptr;
}

u32 ArabicText::GetArabicForm(u32 codepoint, ArabicForm form) {
    const ArabicLetterForms* forms = FindLetterForms(codepoint);
    if (!forms) return codepoint; // إرجاع الشكل الأصلي إذا لم يُوجد / Return original if not found
    
    switch (form) {
        case ArabicForm::Isolated: return forms->isolated;
        case ArabicForm::Initial:  return forms->initial;
        case ArabicForm::Medial:   return forms->medial;
        case ArabicForm::Final:    return forms->final_;
    }
    return codepoint;
}

std::vector<u32> ArabicText::ShapeArabic(const std::vector<u32>& codepoints) {
    std::vector<u32> result;
    result.reserve(codepoints.size());
    
    for (size_t i = 0; i < codepoints.size(); ++i) {
        u32 cp = codepoints[i];
        
        // تخطي التشكيل / Skip diacritics (pass through)
        if (IsDiacritic(cp)) {
            result.push_back(cp);
            continue;
        }
        
        // إذا لم يكن حرفاً عربياً أساسياً / If not a base Arabic letter
        const ArabicLetterForms* forms = FindLetterForms(cp);
        if (!forms) {
            result.push_back(cp);
            continue;
        }
        
        // التحقق من الحرف السابق (تجاهل التشكيل) / Check previous (skip diacritics)
        bool prevJoins = false;
        for (int j = static_cast<int>(i) - 1; j >= 0; --j) {
            if (IsDiacritic(codepoints[j])) continue;
            const ArabicLetterForms* prevForms = FindLetterForms(codepoints[j]);
            if (prevForms && prevForms->joins_left) {
                prevJoins = true;
            }
            break;
        }
        
        // التحقق من الحرف التالي (تجاهل التشكيل) / Check next (skip diacritics)
        bool nextJoins = false;
        for (size_t j = i + 1; j < codepoints.size(); ++j) {
            if (IsDiacritic(codepoints[j])) continue;
            const ArabicLetterForms* nextForms = FindLetterForms(codepoints[j]);
            if (nextForms && nextForms->joins_right) {
                nextJoins = true;
            }
            break;
        }
        
        // تحديد الشكل بناءً على السياق / Determine form based on context
        ArabicForm form;
        if (prevJoins && nextJoins && forms->joins_right && forms->joins_left) {
            form = ArabicForm::Medial;    // وسط / Medial
        }
        else if (prevJoins && forms->joins_right) {
            form = ArabicForm::Final;     // نهاية / Final
        }
        else if (nextJoins && forms->joins_left) {
            form = ArabicForm::Initial;   // بداية / Initial
        }
        else {
            form = ArabicForm::Isolated;  // منفصل / Isolated
        }
        
        result.push_back(GetArabicForm(cp, form));
    }
    
    return result;
}

// ==============================================================================
// اكتشاف الاتجاه وBiDi / Direction Detection and BiDi
// ==============================================================================

TextDirection ArabicText::DetectDirection(const std::vector<u32>& codepoints) {
    // البحث عن أول حرف قوي الاتجاه / Find first strong directional character
    for (u32 cp : codepoints) {
        if (IsRTL(cp)) return TextDirection::RTL;
        if ((cp >= 0x0041 && cp <= 0x005A) || // A-Z
            (cp >= 0x0061 && cp <= 0x007A) || // a-z
            (cp >= 0x00C0 && cp <= 0x024F)) { // Latin Extended
            return TextDirection::LTR;
        }
    }
    return TextDirection::LTR; // افتراضي LTR / Default LTR
}

std::vector<u32> ArabicText::ReorderBiDi(const std::vector<u32>& codepoints) {
    // تطبيق BiDi مبسط: عكس المقاطع العربية
    // Simplified BiDi: reverse Arabic segments
    
    TextDirection overall = DetectDirection(codepoints);
    
    if (overall == TextDirection::LTR) {
        // النص الأساسي LTR - عكس المقاطع العربية فقط
        // Base is LTR - only reverse Arabic segments
        std::vector<u32> result;
        result.reserve(codepoints.size());
        
        size_t i = 0;
        while (i < codepoints.size()) {
            if (IsRTL(codepoints[i]) || IsDiacritic(codepoints[i])) {
                // جمع مقطع RTL / Collect RTL run
                std::vector<u32> rtlRun;
                while (i < codepoints.size() && 
                       (IsRTL(codepoints[i]) || IsDiacritic(codepoints[i]) || 
                        codepoints[i] == ' ')) {
                    // توقف عند المسافة إذا كان الحرف التالي LTR / Stop at space if next is LTR
                    if (codepoints[i] == ' ') {
                        size_t next = i + 1;
                        while (next < codepoints.size() && codepoints[next] == ' ') next++;
                        if (next >= codepoints.size() || !IsRTL(codepoints[next])) break;
                    }
                    rtlRun.push_back(codepoints[i]);
                    ++i;
                }
                // عكس المقطع / Reverse segment
                std::reverse(rtlRun.begin(), rtlRun.end());
                result.insert(result.end(), rtlRun.begin(), rtlRun.end());
            } else {
                result.push_back(codepoints[i]);
                ++i;
            }
        }
        return result;
    }
    else {
        // النص الأساسي RTL - عكس الكل ثم إعادة ترتيب مقاطع LTR
        // Base is RTL - reverse all then re-reverse LTR segments
        std::vector<u32> result = codepoints;
        std::reverse(result.begin(), result.end());
        
        // إعادة ترتيب مقاطع LTR / Re-reverse LTR segments
        size_t i = 0;
        while (i < result.size()) {
            if (!IsRTL(result[i]) && !IsDiacritic(result[i]) && result[i] != ' ' && result[i] > 0x20) {
                size_t start = i;
                while (i < result.size() && !IsRTL(result[i]) && !IsDiacritic(result[i])) {
                    ++i;
                }
                std::reverse(result.begin() + start, result.begin() + i);
            } else {
                ++i;
            }
        }
        return result;
    }
}

// ==============================================================================
// نطاقات Unicode / Unicode Ranges
// ==============================================================================

std::vector<ArabicText::UnicodeRange> ArabicText::GetArabicRanges() {
    return {
        { 0x0600, 0x06FF },   // Arabic (256 chars)
        { 0x0750, 0x077F },   // Arabic Supplement (48 chars)
        { 0xFB50, 0xFDFF },   // Arabic Presentation Forms-A (688 chars)
        { 0xFE70, 0xFEFF },   // Arabic Presentation Forms-B (144 chars)
    };
}

std::vector<ArabicText::UnicodeRange> ArabicText::GetFullRanges() {
    return {
        { 0x0020, 0x007E },   // ASCII printable (95 chars)
        { 0x0600, 0x06FF },   // Arabic (256 chars)
        { 0x0750, 0x077F },   // Arabic Supplement (48 chars)
        { 0xFB50, 0xFDFF },   // Arabic Presentation Forms-A (688 chars)
        { 0xFE70, 0xFEFF },   // Arabic Presentation Forms-B (144 chars)
    };
}

} // namespace graphics
} // namespace sad
