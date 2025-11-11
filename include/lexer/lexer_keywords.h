// ======================================================================
// lexer_keywords.h - جدول الكلمات المفتاحية / Keywords Table
// ======================================================================
// الوصف بالعربية:
//   جدول يربط الكلمات المفتاحية العربية بأنواع الرموز المقابلة
//   يُستخدم من قبل المحلل المعجمي للتعرف على الكلمات المفتاحية
//
// English Description:
//   Table mapping Arabic keywords to their corresponding token types
//   Used by the Lexer to recognize keywords
//
// ملاحظات / Notes:
//   - يحتوي على 40+ كلمة مفتاحية عربية
//     Contains 40+ Arabic keywords
//   - يستخدم unordered_map للبحث السريع O(1)
//     Uses unordered_map for fast O(1) lookup
//   - يجب استدعاء initialize() قبل الاستخدام
//     Must call initialize() before use
// ======================================================================

#pragma once

#include <unordered_map>
#include <string>
#include "token.h"

namespace Sad {
namespace Lexer {

// ======================================================================
// فئة جدول الكلمات المفتاحية / Keywords Table Class
// ======================================================================
/**
 * @class KeywordTable
 * @brief (AR) جدول الكلمات المفتاحية العربية ونظيراتها من TokenType
 * @brief (EN) Arabic keywords table and their TokenType counterparts
 * 
 * ملاحظات إضافية:
 * - AR: الفئة ثابتة (static) - لا تحتاج إنشاء كائن
 * - EN: Class is static - no need to instantiate
 * - AR: يجب تهيئة الجدول مرة واحدة عند بدء البرنامج
 * - EN: Table must be initialized once at program start
 * - AR: البحث سريع جداً O(1) باستخدام unordered_map
 * - EN: Lookup is very fast O(1) using unordered_map
 */
class KeywordTable {
public:
    // ==================================================================
    // دالة التهيئة / Initialization Function
    // ==================================================================
    
    /**
     * @brief (AR) تهيئة جدول الكلمات المفتاحية
     * @brief (EN) Initialize keywords table
     * 
     * @throws لا يرمي استثناءات / Does not throw exceptions
     * 
     * مثال:
     * KeywordTable::initialize();
     * 
     * ملاحظات إضافية:
     * - AR: يجب استدعاؤها مرة واحدة فقط عند بدء البرنامج
     * - EN: Should be called once only at program start
     * - AR: آمنة للاستدعاء المتعدد (تتحقق إذا تم التهيئة سابقاً)
     * - EN: Safe to call multiple times (checks if already initialized)
     * - AR: تملأ الجدول بـ 40+ كلمة مفتاحية
     * - EN: Fills table with 40+ keywords
     */
    static void initialize();
    
    // ==================================================================
    // دوال الاستعلام / Query Functions
    // ==================================================================
    
    /**
     * @brief (AR) التحقق من أن كلمة هي كلمة مفتاحية
     * @brief (EN) Check if word is a keyword
     * 
     * @param word (std::string) — (AR) الكلمة المراد فحصها / (EN) word to check
     * @return (bool) — (AR) true إذا كانت كلمة مفتاحية / (EN) true if keyword
     * 
     * مثال:
     * if (KeywordTable::isKeyword("دالة")) {
     *     // هذه كلمة مفتاحية
     * }
     * 
     * ملاحظات إضافية:
     * - AR: حساسة لحالة الأحرف (دالة ≠ دَالة)
     * - EN: Case-sensitive (دالة ≠ دَالة)
     * - AR: سريعة جداً O(1)
     * - EN: Very fast O(1)
     */
    static bool isKeyword(const std::string& word);
    
    /**
     * @brief (AR) الحصول على نوع الرمز للكلمة المفتاحية
     * @brief (EN) Get token type for keyword
     * 
     * @param word (std::string) — (AR) الكلمة المفتاحية / (EN) keyword
     * @return (TokenType) — (AR) نوع الرمز المقابل / (EN) corresponding token type
     * @throws (std::runtime_error) — (AR) إذا لم تكن كلمة مفتاحية / (EN) if not a keyword
     * 
     * مثال:
     * TokenType type = KeywordTable::getKeywordType("إذا");
     * // type = TokenType::KEYWORD_IF
     * 
     * ملاحظات إضافية:
     * - AR: يجب التحقق بـ isKeyword() أولاً لتجنب الاستثناءات
     * - EN: Should check with isKeyword() first to avoid exceptions
     * - AR: سريعة جداً O(1)
     * - EN: Very fast O(1)
     */
    static TokenType getKeywordType(const std::string& word);
    
    /**
     * @brief (AR) الحصول على الكلمة المفتاحية من نوع الرمز
     * @brief (EN) Get keyword from token type
     * 
     * @param type (TokenType) — (AR) نوع الرمز / (EN) token type
     * @return (std::string) — (AR) الكلمة المفتاحية / (EN) keyword
     * @throws (std::runtime_error) — (AR) إذا لم يكن نوع كلمة مفتاحية / (EN) if not a keyword type
     * 
     * مثال:
     * std::string keyword = KeywordTable::getKeyword(TokenType::KEYWORD_IF);
     * // keyword = "إذا"
     * 
     * ملاحظات إضافية:
     * - AR: عملية عكسية لـ getKeywordType
     * - EN: Reverse operation of getKeywordType
     * - AR: أبطأ قليلاً O(n) لكن n صغير (40 كلمة)
     * - EN: Slightly slower O(n) but n is small (40 words)
     */
    static std::string getKeyword(TokenType type);
    
    /**
     * @brief (AR) الحصول على جميع الكلمات المفتاحية
     * @brief (EN) Get all keywords
     * 
     * @return (std::vector<std::string>) — (AR) قائمة بجميع الكلمات المفتاحية / (EN) list of all keywords
     * 
     * مثال:
     * auto keywords = KeywordTable::getAllKeywords();
     * for (const auto& kw : keywords) {
     *     std::cout << kw << std::endl;
     * }
     * 
     * ملاحظات إضافية:
     * - AR: مفيدة للتوثيق والاختبار
     * - EN: Useful for documentation and testing
     * - AR: تُرجع نسخة من القائمة (ليست مرجع)
     * - EN: Returns copy of list (not reference)
     */
    static std::vector<std::string> getAllKeywords();
    
    /**
     * @brief (AR) التحقق من أن الجدول تم تهيئته
     * @brief (EN) Check if table is initialized
     * 
     * @return (bool) — (AR) true إذا تم التهيئة / (EN) true if initialized
     * 
     * ملاحظات إضافية:
     * - AR: مفيدة للتحقق قبل الاستخدام
     * - EN: Useful for checking before use
     */
    static bool isInitialized();

private:
    /**
     * @brief (AR) الجدول الرئيسي للكلمات المفتاحية
     * @brief (EN) Main keywords table
     * 
     * ملاحظات:
     * - يربط الكلمة العربية بنوع الرمز
     * - Maps Arabic word to token type
     */
    static std::unordered_map<std::string, TokenType> keywords_;
    
    /**
     * @brief (AR) علَم لتتبع حالة التهيئة
     * @brief (EN) Flag to track initialization status
     */
    static bool initialized_;
};

} // namespace Lexer
} // namespace Sad
