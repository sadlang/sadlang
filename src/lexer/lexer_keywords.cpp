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
    // Keywords - Functions & Structures (Arabic + English)
    keywords_["دالة"] = TokenType::KEYWORD_FUNCTION;
    keywords_["function"] = TokenType::KEYWORD_FUNCTION;
    keywords_["func"] = TokenType::KEYWORD_FUNCTION;
    keywords_["إرجاع"] = TokenType::KEYWORD_RETURN;
    keywords_["ارجع"] = TokenType::KEYWORD_RETURN;
    keywords_["return"] = TokenType::KEYWORD_RETURN;
    keywords_["صنف"] = TokenType::KEYWORD_CLASS;
    keywords_["class"] = TokenType::KEYWORD_CLASS;
    // (AR) كلمة 'يرث' للوراثة - المواصفة 03_oop.md القسم 1 و 2
    // (EN) 'inherits' keyword for inheritance - spec 03_oop.md §1,2
    keywords_["يرث"] = TokenType::KEYWORD_INHERITS;
    keywords_["inherits"] = TokenType::KEYWORD_INHERITS;
    // (AR) كلمة 'نهاية' لإنهاء الصنف/الكتلة - المواصفة 03_oop.md القسم 1
    // (EN) 'end' keyword for class/block termination - spec 03_oop.md §1
    keywords_["نهاية"] = TokenType::KEYWORD_END;
    keywords_["end"] = TokenType::KEYWORD_END;
    keywords_["جديد"] = TokenType::KEYWORD_NEW;
    keywords_["new"] = TokenType::KEYWORD_NEW;
    keywords_["هذا"] = TokenType::KEYWORD_THIS;
    keywords_["this"] = TokenType::KEYWORD_THIS;
    keywords_["منشئ"] = TokenType::KEYWORD_CONSTRUCTOR;
    keywords_["constructor"] = TokenType::KEYWORD_CONSTRUCTOR;
    keywords_["باني"] = TokenType::KEYWORD_CONSTRUCTOR_ALT;   // Alternative constructor keyword
    keywords_["مدمر"] = TokenType::KEYWORD_DESTRUCTOR;         // Destructor
    keywords_["destructor"] = TokenType::KEYWORD_DESTRUCTOR;
    keywords_["الأساس"] = TokenType::KEYWORD_SUPER;           // Super/base class
    keywords_["super"] = TokenType::KEYWORD_SUPER;
    
    DEBUG_PRINT("تمت إضافة 16 كلمة: الدوال والبنيات (عربي + إنجليزي)");
    
    // ========== الكلمات المفتاحية - التحكم في التدفق ==========
    // Keywords - Control Flow (Arabic + English)
    keywords_["إذا"] = TokenType::KEYWORD_IF;
    keywords_["if"] = TokenType::KEYWORD_IF;
    keywords_["وإلا"] = TokenType::KEYWORD_ELSE;
    keywords_["else"] = TokenType::KEYWORD_ELSE;
    keywords_["وإلا_إذا"] = TokenType::KEYWORD_ELSE_IF;
    keywords_["elif"] = TokenType::KEYWORD_ELSE_IF;
    keywords_["بينما"] = TokenType::KEYWORD_WHILE;
    keywords_["while"] = TokenType::KEYWORD_WHILE;
    keywords_["لكل"] = TokenType::KEYWORD_FOR;
    keywords_["for"] = TokenType::KEYWORD_FOR;
    keywords_["اخرج"] = TokenType::KEYWORD_BREAK;
    keywords_["break"] = TokenType::KEYWORD_BREAK;
    keywords_["استمر"] = TokenType::KEYWORD_CONTINUE;
    keywords_["continue"] = TokenType::KEYWORD_CONTINUE;
    
    DEBUG_PRINT("تمت إضافة 14 كلمة: التحكم في التدفق (عربي + إنجليزي)");
    
    // ========== الكلمات المفتاحية - Switch/Case ==========
    // Keywords - Switch/Case (spec 04_syntax.md)
    keywords_["حالة"] = TokenType::KEYWORD_CASE;
    keywords_["case"] = TokenType::KEYWORD_CASE;
    keywords_["عندما"] = TokenType::KEYWORD_WHEN;
    keywords_["when"] = TokenType::KEYWORD_WHEN;
    keywords_["افتراضي"] = TokenType::KEYWORD_DEFAULT;  // For switch default
    keywords_["default"] = TokenType::KEYWORD_DEFAULT;
    
    DEBUG_PRINT("تمت إضافة 6 كلمات: Switch/Case");
    
    // ========== الكلمات المفتاحية - Pattern Matching (Phase 2) ==========
    // Keywords - Pattern Matching
    keywords_["طابق"] = TokenType::KEYWORD_MATCH;  // (AR) مطابقة الأنماط / (EN) Pattern matching
    keywords_["match"] = TokenType::KEYWORD_MATCH;
    
    DEBUG_PRINT("تمت إضافة 2 كلمة: Pattern Matching");
    
    // ========== الكلمات المفتاحية - Async/Await (Phase 2) ==========
    // Keywords - Async/Await
    keywords_["غير_متزامن"] = TokenType::KEYWORD_ASYNC;  // (AR) دالة غير متزامنة / (EN) Async function
    keywords_["async"] = TokenType::KEYWORD_ASYNC;
    keywords_["انتظر"] = TokenType::KEYWORD_AWAIT;        // (AR) انتظار نتيجة / (EN) Await result
    keywords_["await"] = TokenType::KEYWORD_AWAIT;
    
    DEBUG_PRINT("تمت إضافة 4 كلمات: Async/Await");
    
    // ========== الكلمات المفتاحية الإضافية ==========
    // Additional Keywords
    keywords_["في"] = TokenType::KEYWORD_IN;
    keywords_["in"] = TokenType::KEYWORD_IN;
    
    DEBUG_PRINT("تمت إضافة 2 كلمة: كلمات إضافية");
    
    // ========== الكلمات المفتاحية - معالجة الأخطاء ==========
    // Keywords - Error Handling (spec 05_python_features.md)
    keywords_["حاول"] = TokenType::KEYWORD_TRY;
    keywords_["try"] = TokenType::KEYWORD_TRY;
    keywords_["امسك"] = TokenType::KEYWORD_CATCH;  // Changed from اصطد to امسك per spec
    keywords_["catch"] = TokenType::KEYWORD_CATCH;
    keywords_["ارمِ"] = TokenType::KEYWORD_THROW;
    keywords_["throw"] = TokenType::KEYWORD_THROW;
    keywords_["أخيراً"] = TokenType::KEYWORD_FINALLY;
    keywords_["finally"] = TokenType::KEYWORD_FINALLY;
    
    DEBUG_PRINT("تمت إضافة 4 كلمات: معالجة الأخطاء");
    
    // ========== الكلمات المفتاحية - التحكم بالوصول ==========
    // Keywords - Access Control
    keywords_["عام"] = TokenType::KEYWORD_PUBLIC;
    keywords_["خاص"] = TokenType::KEYWORD_PRIVATE;
    keywords_["محمي"] = TokenType::KEYWORD_PROTECTED;
    
    DEBUG_PRINT("تمت إضافة 3 كلمات: التحكم بالوصول");
    
    // ========== الكلمات المفتاحية - الوراثة ==========
    // Keywords - Inheritance (spec 04_syntax.md)
    // NOTE: 'يرث' already registered as KEYWORD_INHERITS at line 59
    // NOTE: Advanced OOP (implements, interface, abstract, override) removed - Phase 2
    // NOTE: virtual removed - conflicts with DEFAULT (افتراضي)
    
    DEBUG_PRINT("تمت إضافة 0 كلمة: الوراثة (مسجلة بالفعل في القسم OOP)");
    
    // ========== الكلمات المفتاحية - الوحدات ==========
    // Keywords - Modules (spec 08_modules_and_ffi.md)
    keywords_["استورد"] = TokenType::KEYWORD_IMPORT;
    keywords_["import"] = TokenType::KEYWORD_IMPORT;
    keywords_["من"] = TokenType::KEYWORD_FROM;  // for "from X import Y"
    keywords_["from"] = TokenType::KEYWORD_FROM;
    keywords_["كـ"] = TokenType::KEYWORD_AS;    // for "import X as Y"
    keywords_["as"] = TokenType::KEYWORD_AS;
    keywords_["صدّر"] = TokenType::KEYWORD_EXPORT;
    keywords_["export"] = TokenType::KEYWORD_EXPORT;
    // NOTE: module and package removed - not in spec
    
    DEBUG_PRINT("تمت إضافة 8 كلمات: الوحدات");
    
    // ========== الكلمات المفتاحية - المتغيرات ==========
    // Keywords - Variables (spec 04_syntax.md)
    // NOTE: var and let removed - not in spec (type inference works)
    keywords_["ثابت"] = TokenType::KEYWORD_CONST;
    keywords_["const"] = TokenType::KEYWORD_CONST;
    keywords_["ساكن"] = TokenType::KEYWORD_STATIC;
    keywords_["static"] = TokenType::KEYWORD_STATIC;
    
    // Properties (Phase 6.3)
    keywords_["خاصية"] = TokenType::KEYWORD_PROPERTY;
    keywords_["property"] = TokenType::KEYWORD_PROPERTY;
    keywords_["احصل"] = TokenType::KEYWORD_GET;
    keywords_["get"] = TokenType::KEYWORD_GET;
    keywords_["عيّن"] = TokenType::KEYWORD_SET;
    keywords_["set"] = TokenType::KEYWORD_SET;
    
    DEBUG_PRINT("تمت إضافة 10 كلمات: المتغيرات والخصائص");
    
    // ========== أنواع البيانات الأساسية ==========
    // Basic Data Types (Arabic + English)
    
    // Integer Types (Arabic + English)
    keywords_["رقم"] = TokenType::TYPE_INTEGER;
    keywords_["int"] = TokenType::TYPE_INTEGER;
    keywords_["integer"] = TokenType::TYPE_INTEGER;
    keywords_["number"] = TokenType::TYPE_INTEGER;  // 'number' as alias for integer
    
    // Float/Double Types (Arabic + English)
    keywords_["عشري"] = TokenType::TYPE_DOUBLE;
    keywords_["float"] = TokenType::TYPE_DOUBLE;
    keywords_["double"] = TokenType::TYPE_DOUBLE;
    
    // String Types (Arabic + English)
    keywords_["نص"] = TokenType::TYPE_STRING;
    keywords_["string"] = TokenType::TYPE_STRING;
    keywords_["str"] = TokenType::TYPE_STRING;
    
    // Boolean Types (Arabic + English)
    keywords_["منطقي"] = TokenType::TYPE_BOOLEAN;
    keywords_["bool"] = TokenType::TYPE_BOOLEAN;
    keywords_["boolean"] = TokenType::TYPE_BOOLEAN;
    
    // Void/None Types (Arabic + English)
    keywords_["فراغ"] = TokenType::TYPE_VOID;
    keywords_["void"] = TokenType::TYPE_VOID;
    keywords_["عدم"] = TokenType::TYPE_NULL;
    keywords_["null"] = TokenType::TYPE_NULL;
    keywords_["none"] = TokenType::TYPE_NULL;
    
    // Composite Types (Arabic + English)
    keywords_["مصفوفة"] = TokenType::TYPE_ARRAY;
    keywords_["array"] = TokenType::TYPE_ARRAY;
    keywords_["خريطة"] = TokenType::TYPE_MAP;
    keywords_["map"] = TokenType::TYPE_MAP;
    keywords_["dict"] = TokenType::TYPE_MAP;
    
    // Any Type (spec 01_types.md, 04_syntax.md)
    keywords_["أي"] = TokenType::TYPE_ANY;
    keywords_["any"] = TokenType::TYPE_ANY;
    
    DEBUG_PRINT("تمت إضافة 24 كلمة: أنواع البيانات (عربي + إنجليزي)");
    
    // ========== ميزات Python المدعومة ==========
    // Supported Python Features (spec 05_python_features.md)
    keywords_["لامدا"] = TokenType::KEYWORD_LAMBDA;
    keywords_["lambda"] = TokenType::KEYWORD_LAMBDA;
    keywords_["اعطِ"] = TokenType::KEYWORD_YIELD;
    keywords_["yield"] = TokenType::KEYWORD_YIELD;
    
    DEBUG_PRINT("تمت إضافة 4 كلمات: ميزات Python");
    
    // ========== القيم الحرفية ==========
    // Literal Values (Arabic + English)
    keywords_["صحيح"] = TokenType::LITERAL_TRUE;
    keywords_["true"] = TokenType::LITERAL_TRUE;
    keywords_["خطأ"] = TokenType::LITERAL_FALSE;
    keywords_["false"] = TokenType::LITERAL_FALSE;
    keywords_["لاشيء"] = TokenType::LITERAL_NULL;
    
    DEBUG_PRINT("تمت إضافة 5 كلمات: القيم الحرفية (عربي + إنجليزي)");
    
    // ========== العوامل المنطقية ==========
    // Logical Operators (Arabic)
    keywords_["و"] = TokenType::OP_AND;
    keywords_["أو"] = TokenType::OP_OR;
    keywords_["ليس"] = TokenType::OP_NOT;
    
    DEBUG_PRINT("تمت إضافة 3 كلمات: العوامل المنطقية العربية");
    
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
