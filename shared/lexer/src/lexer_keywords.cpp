// ======================================================================
// lexer_keywords.cpp - تنفيذ جدول الكلمات المفتاحية / Keywords Table Implementation
// ======================================================================
// الوصف بالعربية:
//   تنفيذ جميع وظائف جدول الكلمات المفتاحية مع 40+ كلمة عربية
//
// English Description:
//   Implementation of keywords table functions with 40+ Arabic words
//
// ملاحظات / Notes:
//   - يتضمن سجلات DEBUG_PRINT مفصلة
//     Includes detailed DEBUG_PRINT logs
//   - الكلمات مقسمة حسب الفئات لسهولة الصيانة
//     Words organized by category for easy maintenance
// ======================================================================

#include <string>
#include "lexer_keywords.h"
#include "string_utils.h"
#include <stdexcept>
#include <algorithm>

namespace Sad {
namespace Lexer {

// تهيئة المتغيرات الثابتة / Initialize static variables
std::unordered_map<std::string, TokenType> KeywordTable::keywords_;
bool KeywordTable::initialized_ = false;

// ======================================================================
// دالة التهيئة / Initialization Function
// ======================================================================

/**
 * @brief (AR) تهيئة جدول الكلمات المفتاحية بجميع الكلمات العربية
 * @brief (EN) Initialize keywords table with all Arabic words
 */
void KeywordTable::initialize() {
    // التحقق إذا تم التهيئة سابقاً
    // Check if already initialized
    if (initialized_) {
        DEBUG_PRINT("جدول الكلمات المفتاحية مُهيأ مسبقاً");
        return;
    }
    
    DEBUG_PRINT("بدء تهيئة جدول الكلمات المفتاحية");
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) الـ 40 كلمة محجوزة الرسمية فقط + أسماء بديلة بدون تشكيل
    //      جميع الكلمات الأخرى (سياقية، أنواع، BeeOS) أصبحت مُعرّفات
    //      عادية يتعرف عليها البارسر سياقياً
    // (EN) Only the official 40 reserved keywords + diacritics-free aliases
    //      All other words (contextual, types, BeeOS) are now regular
    //      identifiers recognized contextually by the parser
    // ═══════════════════════════════════════════════════════════════════

    // ── [1-11] الدوال والبنيات والأصناف ──
    keywords_["دالة"] = TokenType::KEYWORD_FUNCTION;    // 1. دالة
    keywords_["ارجع"] = TokenType::KEYWORD_RETURN;      // 2. ارجع
    keywords_["صنف"] = TokenType::KEYWORD_CLASS;         // 3. صنف
    keywords_["بنية"] = TokenType::KEYWORD_STRUCT;       // 4. بنية
    keywords_["تعداد"] = TokenType::KEYWORD_ENUM;        // 5. تعداد
    keywords_["يرث"] = TokenType::KEYWORD_INHERITS;      // 6. يرث
    keywords_["نهاية"] = TokenType::KEYWORD_END;         // 7. نهاية
    keywords_["جديد"] = TokenType::KEYWORD_NEW;          // 8. جديد
    keywords_["هذا"] = TokenType::KEYWORD_THIS;          // 9. هذا
    keywords_["باني"] = TokenType::KEYWORD_CONSTRUCTOR;  // 10. باني
    keywords_["الأساس"] = TokenType::KEYWORD_SUPER;      // 11. الأساس

    // ── [12-18] التحكم في التدفق ──
    keywords_["إذا"] = TokenType::KEYWORD_IF;            // 12. إذا
    keywords_["اذا"] = TokenType::KEYWORD_IF;            //     (بدون همزة)
    keywords_["وإلا"] = TokenType::KEYWORD_ELSE;         // 13. وإلا
    keywords_["والا"] = TokenType::KEYWORD_ELSE;         //     (بدون همزة)
    keywords_["بينما"] = TokenType::KEYWORD_WHILE;       // 14. بينما
    keywords_["لكل"] = TokenType::KEYWORD_FOR;           // 15. لكل
    keywords_["في"] = TokenType::KEYWORD_IN;             // 16. في
    keywords_["توقف"] = TokenType::KEYWORD_BREAK;        // 17. توقف
    keywords_["استمر"] = TokenType::KEYWORD_CONTINUE;    // 18. استمر

    // ── [19-21] مطابقة الأنماط ──
    keywords_["طابق"] = TokenType::KEYWORD_MATCH;        // 19. طابق
    keywords_["عندما"] = TokenType::KEYWORD_WHEN;        // 20. عندما
    keywords_["افتراضي"] = TokenType::KEYWORD_DEFAULT;   // 21. افتراضي

    // ── [22-25] معالجة الأخطاء ──
    keywords_["حاول"] = TokenType::KEYWORD_TRY;          // 22. حاول
    keywords_["امسك"] = TokenType::KEYWORD_CATCH;        // 23. امسك
    keywords_["ارمي"] = TokenType::KEYWORD_THROW;        // 24. ارمي
    keywords_["أخيراً"] = TokenType::KEYWORD_FINALLY;    // 25. أخيراً
    keywords_["أخيرا"] = TokenType::KEYWORD_FINALLY;     //     (بدون تنوين)

    // ── [26-29] التحكم بالوصول ──
    keywords_["عام"] = TokenType::KEYWORD_PUBLIC;        // 26. عام
    keywords_["خاص"] = TokenType::KEYWORD_PRIVATE;       // 27. خاص
    keywords_["محمي"] = TokenType::KEYWORD_PROTECTED;    // 28. محمي
    keywords_["مجرد"] = TokenType::KEYWORD_ABSTRACT;     // 29. مجرد
    keywords_["\xD9\x85\xD8\xAC\xD8\xB1\xD8\xAF"] = TokenType::KEYWORD_ABSTRACT; // (UTF-8 explicit)

    // ── [30-33] الوحدات ──
    keywords_["استورد"] = TokenType::KEYWORD_IMPORT;     // 30. استورد
    keywords_["من"] = TokenType::KEYWORD_FROM;           // 31. من
    keywords_["كـ"] = TokenType::KEYWORD_AS;             // 32. كـ
    keywords_["صدّر"] = TokenType::KEYWORD_EXPORT;       // 33. صدّر
    keywords_["صدر"] = TokenType::KEYWORD_EXPORT;        //     (بدون شدة)

    // ── [34-36] المتغيرات ──
    keywords_["متغير"] = TokenType::KEYWORD_VAR;         // 34. متغير
    keywords_["ثابت"] = TokenType::KEYWORD_CONST;        // 35. ثابت
    keywords_["ساكن"] = TokenType::KEYWORD_STATIC;       // 36. ساكن
    keywords_["خارجي"] = TokenType::KEYWORD_EXTERN;   // 37. خارجي

    // ── [38-40] القيم الحرفية ──
    keywords_["صحيح"] = TokenType::LITERAL_TRUE;         // 38. صحيح
    keywords_["خطأ"] = TokenType::LITERAL_FALSE;         // 39. خطأ
    keywords_["لاشيء"] = TokenType::LITERAL_NULL;        // 40. لاشيء

    // ── العوامل المنطقية (3) ──
    keywords_["و"] = TokenType::OP_AND;                  // و (AND)
    keywords_["أو"] = TokenType::OP_OR;                  // أو (OR)
    keywords_["ليس"] = TokenType::OP_NOT;                // ليس (NOT)

    // ═══════════════════════════════════════════════════════════════════
    // (AR) الكلمات التالية أصبحت سياقية — يتعرف عليها البارسر عبر
    //      نمط التحقق المزدوج:
    //      if (check(TT::KEYWORD_X) || (check(TT::IDENTIFIER) && val == "كلمة"))
    //
    // الكلمات السياقية (20):
    //   غير_متزامن، انتظر، لامدا، أنتج، مولد، باستخدام، نهاية_استخدام
    //   سمة/واجهة، نفّذ/نفذ، قالب، فضاء، نهاية_فضاء
    //   اختبر، خاصية، احصل، عيّن، هدم، عامل، رئيسية، حالة
    //
    // أسماء الأنواع المدمجة (9) — مُعرّفات عادية:
    //   رقم، عشري، نص، منطقي، فراغ، عدم، مصفوفة، خريطة، أي
    //   يتعرف عليها البارسر عبر isTypeToken() مع lookahead
    //
    // أنواع BeeOS المنخفضة المستوى — مُعرّفات عادية:
    //   ط8، ط16، ط32، ط64، ص8، ص16، ص32، ص64، مؤشر، بايت، كلمة، إلخ
    // ═══════════════════════════════════════════════════════════════════
    
    // تعيين علَم التهيئة
    // Set initialization flag
    initialized_ = true;
    
    DEBUG_PRINT("اكتمال تهيئة جدول الكلمات المفتاحية - المجموع: " + 
                std::to_string(keywords_.size()) + " كلمة (40 محجوزة + أسماء بديلة)");
}

// ======================================================================
// دوال الاستعلام / Query Functions
// ======================================================================

/**
 * @brief (AR) التحقق من أن كلمة هي كلمة مفتاحية
 * @brief (EN) Check if word is a keyword
 */
bool KeywordTable::isKeyword(const std::string& word) {
    DEBUG_PRINT("فحص إذا كانت الكلمة مفتاحية: \"" + word + "\"");
    
    // التحقق من التهيئة
    // Check initialization
    if (!initialized_) {
        DEBUG_PRINT("تحذير: الجدول غير مُهيأ - سيتم التهيئة الآن");
        initialize();
    }
    
    // البحث في الجدول
    // Search in table
    bool found = keywords_.find(word) != keywords_.end();
    
    DEBUG_PRINT("النتيجة: " + std::string(found ? "نعم - كلمة مفتاحية" : "لا - معرف عادي"));
    return found;
}

/**
 * @brief (AR) الحصول على نوع الرمز للكلمة المفتاحية
 * @brief (EN) Get token type for keyword
 */
TokenType KeywordTable::getKeywordType(const std::string& word) {
    DEBUG_PRINT("الحصول على نوع الرمز للكلمة: \"" + word + "\"");
    
    // التحقق من التهيئة
    // Check initialization
    if (!initialized_) {
        DEBUG_PRINT("تحذير: الجدول غير مُهيأ - سيتم التهيئة الآن");
        initialize();
    }
    
    // البحث في الجدول
    // Search in table
    auto it = keywords_.find(word);
    
    if (it == keywords_.end()) {
        DEBUG_PRINT("خطأ: الكلمة ليست كلمة مفتاحية");
        throw std::runtime_error("الكلمة '" + word + "' ليست كلمة مفتاحية");
    }
    
    DEBUG_PRINT("تم العثور على النوع: " + Token::typeToString(it->second));
    return it->second;
}

/**
 * @brief (AR) الحصول على الكلمة المفتاحية من نوع الرمز
 * @brief (EN) Get keyword from token type
 */
std::string KeywordTable::getKeyword(TokenType type) {
    DEBUG_PRINT("البحث عن الكلمة المفتاحية للنوع: " + Token::typeToString(type));
    
    // التحقق من التهيئة
    // Check initialization
    if (!initialized_) {
        DEBUG_PRINT("تحذير: الجدول غير مُهيأ - سيتم التهيئة الآن");
        initialize();
    }
    
    // البحث العكسي في الجدول
    // Reverse search in table
    for (const auto& pair : keywords_) {
        if (pair.second == type) {
            DEBUG_PRINT("تم العثور على الكلمة: \"" + pair.first + "\"");
            return pair.first;
        }
    }
    
    DEBUG_PRINT("خطأ: النوع ليس نوع كلمة مفتاحية");
    throw std::runtime_error("النوع '" + Token::typeToString(type) + "' ليس نوع كلمة مفتاحية");
}

/**
 * @brief (AR) الحصول على جميع الكلمات المفتاحية
 * @brief (EN) Get all keywords
 */
std::vector<std::string> KeywordTable::getAllKeywords() {
    DEBUG_PRINT("جمع جميع الكلمات المفتاحية");
    
    // التحقق من التهيئة
    // Check initialization
    if (!initialized_) {
        DEBUG_PRINT("تحذير: الجدول غير مُهيأ - سيتم التهيئة الآن");
        initialize();
    }
    
    std::vector<std::string> result;
    result.reserve(keywords_.size());
    
    // جمع جميع الكلمات
    // Collect all words
    for (const auto& pair : keywords_) {
        result.push_back(pair.first);
    }
    
    // ترتيب القائمة أبجدياً
    // Sort alphabetically
    std::sort(result.begin(), result.end());
    
    DEBUG_PRINT("تم جمع " + std::to_string(result.size()) + " كلمة مفتاحية");
    return result;
}

/**
 * @brief (AR) التحقق من أن الجدول تم تهيئته
 * @brief (EN) Check if table is initialized
 */
bool KeywordTable::isInitialized() {
    return initialized_;
}

} // namespace Lexer
} // namespace Sad
