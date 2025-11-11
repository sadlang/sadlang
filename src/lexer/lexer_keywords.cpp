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

#include "../../include/lexer/lexer_keywords.h"
#include "../../include/utils/string_utils.h"
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
    
    // ========== الكلمات المفتاحية - الدوال والبنيات ==========
    // Keywords - Functions & Structures
    keywords_["دالة"] = TokenType::KEYWORD_FUNCTION;
    keywords_["إرجاع"] = TokenType::KEYWORD_RETURN;
    keywords_["صنف"] = TokenType::KEYWORD_CLASS;
    keywords_["جديد"] = TokenType::KEYWORD_NEW;
    keywords_["هذا"] = TokenType::KEYWORD_THIS;
    keywords_["منشئ"] = TokenType::KEYWORD_CONSTRUCTOR;
    
    DEBUG_PRINT("تمت إضافة 6 كلمات: الدوال والبنيات");
    
    // ========== الكلمات المفتاحية - التحكم في التدفق ==========
    // Keywords - Control Flow
    keywords_["إذا"] = TokenType::KEYWORD_IF;
    keywords_["وإلا"] = TokenType::KEYWORD_ELSE;
    keywords_["وإلا_إذا"] = TokenType::KEYWORD_ELSE_IF;
    keywords_["بينما"] = TokenType::KEYWORD_WHILE;
    keywords_["لكل"] = TokenType::KEYWORD_FOR;
    keywords_["اخرج"] = TokenType::KEYWORD_BREAK;
    keywords_["استمر"] = TokenType::KEYWORD_CONTINUE;
    
    DEBUG_PRINT("تمت إضافة 7 كلمات: التحكم في التدفق");
    
    // ========== الكلمات المفتاحية - معالجة الأخطاء ==========
    // Keywords - Error Handling
    keywords_["حاول"] = TokenType::KEYWORD_TRY;
    keywords_["اصطد"] = TokenType::KEYWORD_CATCH;
    keywords_["ارمِ"] = TokenType::KEYWORD_THROW;
    keywords_["أخيراً"] = TokenType::KEYWORD_FINALLY;
    
    DEBUG_PRINT("تمت إضافة 4 كلمات: معالجة الأخطاء");
    
    // ========== الكلمات المفتاحية - التحكم بالوصول ==========
    // Keywords - Access Control
    keywords_["عام"] = TokenType::KEYWORD_PUBLIC;
    keywords_["خاص"] = TokenType::KEYWORD_PRIVATE;
    keywords_["محمي"] = TokenType::KEYWORD_PROTECTED;
    
    DEBUG_PRINT("تمت إضافة 3 كلمات: التحكم بالوصول");
    
    // ========== الكلمات المفتاحية - الوراثة ==========
    // Keywords - Inheritance
    keywords_["يرث"] = TokenType::KEYWORD_EXTENDS;
    keywords_["ينفذ"] = TokenType::KEYWORD_IMPLEMENTS;
    keywords_["واجهة"] = TokenType::KEYWORD_INTERFACE;
    keywords_["مجرد"] = TokenType::KEYWORD_ABSTRACT;
    keywords_["افتراضي"] = TokenType::KEYWORD_VIRTUAL;
    keywords_["تجاوز"] = TokenType::KEYWORD_OVERRIDE;
    
    DEBUG_PRINT("تمت إضافة 6 كلمات: الوراثة");
    
    // ========== الكلمات المفتاحية - الوحدات ==========
    // Keywords - Modules
    keywords_["استورد"] = TokenType::KEYWORD_IMPORT;
    keywords_["صدّر"] = TokenType::KEYWORD_EXPORT;
    keywords_["وحدة"] = TokenType::KEYWORD_MODULE;
    keywords_["حزمة"] = TokenType::KEYWORD_PACKAGE;
    
    DEBUG_PRINT("تمت إضافة 4 كلمات: الوحدات");
    
    // ========== الكلمات المفتاحية - المتغيرات ==========
    // Keywords - Variables
    keywords_["متغير"] = TokenType::KEYWORD_VAR;
    keywords_["ثابت"] = TokenType::KEYWORD_CONST;
    keywords_["اجعل"] = TokenType::KEYWORD_LET;
    keywords_["ساكن"] = TokenType::KEYWORD_STATIC;
    
    DEBUG_PRINT("تمت إضافة 4 كلمات: المتغيرات");
    
    // ========== أنواع البيانات الأساسية ==========
    // Basic Data Types
    keywords_["رقم"] = TokenType::TYPE_INTEGER;
    keywords_["عشري"] = TokenType::TYPE_DOUBLE;
    keywords_["نص"] = TokenType::TYPE_STRING;
    keywords_["منطقي"] = TokenType::TYPE_BOOLEAN;
    keywords_["فراغ"] = TokenType::TYPE_VOID;
    keywords_["عدم"] = TokenType::TYPE_NULL;
    keywords_["مصفوفة"] = TokenType::TYPE_ARRAY;
    keywords_["خريطة"] = TokenType::TYPE_MAP;
    
    DEBUG_PRINT("تمت إضافة 8 كلمات: أنواع البيانات");
    
    // ========== القيم الحرفية ==========
    // Literal Values
    keywords_["صحيح"] = TokenType::LITERAL_TRUE;
    keywords_["خطأ"] = TokenType::LITERAL_FALSE;
    keywords_["لاشيء"] = TokenType::LITERAL_NULL;
    
    DEBUG_PRINT("تمت إضافة 3 كلمات: القيم الحرفية");
    
    // تعيين علَم التهيئة
    // Set initialization flag
    initialized_ = true;
    
    DEBUG_PRINT("اكتمال تهيئة جدول الكلمات المفتاحية - المجموع: " + 
                std::to_string(keywords_.size()) + " كلمة");
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
