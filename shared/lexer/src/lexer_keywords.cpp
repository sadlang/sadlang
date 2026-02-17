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
    
    // ========== الكلمات المفتاحية - الدوال والبنيات ==========
    // Keywords - Functions & Structures (Arabic + English)
    
    // (AR) كلمة 'دالة' لتعريف الدوال - الأساس في اللغة
    // (EN) 'function' keyword for function definitions - language foundation
    keywords_["دالة"] = TokenType::KEYWORD_FUNCTION;
    keywords_["function"] = TokenType::KEYWORD_FUNCTION;
    keywords_["func"] = TokenType::KEYWORD_FUNCTION;
    
    // (AR) كلمة 'رئيسية' للدالة الرئيسية - نقطة الدخول للبرنامج
    // (EN) 'main' keyword for main function - program entry point
    keywords_["رئيسية"] = TokenType::KEYWORD_MAIN;
    
    
    // (AR) كلمة 'إرجاع' لإرجاع القيم من الدوال
    // (EN) 'return' keyword for returning values from functions
    
    keywords_["ارجع"] = TokenType::KEYWORD_RETURN;
    keywords_["\xd8\xa5\xd8\xb1\xd8\xac\xd8\xa7\xd8\xb9"] = TokenType::KEYWORD_RETURN;  // إرجاع (with hamza under alef)
    
    
    // (AR) كلمة 'ترجع' لتحديد نوع الإرجاع (في القوالب والدوال)
    // (EN) 'returns' keyword for return type specification (templates & functions)
    keywords_["ترجع"] = TokenType::KEYWORD_RETURNS;
    
    
    keywords_["صنف"] = TokenType::KEYWORD_CLASS;
    
    // (AR) كلمة 'يرث' للوراثة - المواصفة 03_oop.md القسم 1 و 2
    // (EN) 'inherits' keyword for inheritance - spec 03_oop.md §1,2
    keywords_["يرث"] = TokenType::KEYWORD_INHERITS;
    
    // (AR) كلمة 'نهاية' لإنهاء الصنف/الكتلة - المواصفة 03_oop.md القسم 1
    // (EN) 'end' keyword for class/block termination - spec 03_oop.md §1
    keywords_["نهاية"] = TokenType::KEYWORD_END;
    
    keywords_["جديد"] = TokenType::KEYWORD_NEW;
   
    keywords_["هذا"] = TokenType::KEYWORD_THIS;
   
    keywords_["باني"] = TokenType::KEYWORD_CONSTRUCTOR;
    keywords_["constructor"] = TokenType::KEYWORD_CONSTRUCTOR;
    keywords_["باني"] = TokenType::KEYWORD_CONSTRUCTOR_ALT;   // Alternative constructor keyword
    keywords_["مدمر"] = TokenType::KEYWORD_DESTRUCTOR;         // Destructor
   
    keywords_["الأساس"] = TokenType::KEYWORD_SUPER;           // Super/base class
  
 
    
    DEBUG_PRINT("تمت إضافة 16 كلمة: الدوال والبنيات (عربي + إنجليزي)");
    
    // ========== الكلمات المفتاحية - التحكم في التدفق ==========
    // Keywords - Control Flow (Arabic + English)
    keywords_["إذا"] = TokenType::KEYWORD_IF;
     keywords_["اذا"] = TokenType::KEYWORD_IF;
    
    keywords_["وإلا"] = TokenType::KEYWORD_ELSE;
     keywords_["والا"] = TokenType::KEYWORD_ELSE;
    
    keywords_["وإلا_إذا"] = TokenType::KEYWORD_ELSE_IF;
     keywords_["والا_اذا"] = TokenType::KEYWORD_ELSE_IF;
   
    keywords_["بينما"] = TokenType::KEYWORD_WHILE;
    keywords_["طالما"] = TokenType::KEYWORD_WHILE;
   
    keywords_["لكل"] = TokenType::KEYWORD_FOR;

    keywords_["توقف"] = TokenType::KEYWORD_BREAK;
    
    keywords_["استمر"] = TokenType::KEYWORD_CONTINUE;
   
    
    DEBUG_PRINT("تمت إضافة 14 كلمة: التحكم في التدفق (عربي + إنجليزي)");
    
    // ========== الكلمات المفتاحية - Switch/Case ==========
    // Keywords - Switch/Case (spec 04_syntax.md)
    keywords_["حالة"] = TokenType::KEYWORD_CASE;
   
    keywords_["عندما"] = TokenType::KEYWORD_WHEN;

    keywords_["افتراضي"] = TokenType::KEYWORD_DEFAULT;  // For switch default

    
    DEBUG_PRINT("تمت إضافة 3 كلمات: Switch/Case");
    
    // ========== الكلمات المفتاحية - Pattern Matching (Phase 2) ==========
    // Keywords - Pattern Matching
    keywords_["طابق"] = TokenType::KEYWORD_MATCH;  // (AR) مطابقة الأنماط / (EN) Pattern matching

    
    DEBUG_PRINT("تمت إضافة 1 كلمة: Pattern Matching");
    
    // ========== الكلمات المفتاحية - Async/Await (Phase 2) ==========
    // Keywords - Async/Await
    keywords_["غير_متزامن"] = TokenType::KEYWORD_ASYNC;  // (AR) دالة غير متزامنة / (EN) Async function

    keywords_["انتظر"] = TokenType::KEYWORD_AWAIT;        // (AR) انتظار نتيجة / (EN) Await result

    
    DEBUG_PRINT("تمت إضافة 2 كلمات: Async/Await");
    
    // ========== الكلمات المفتاحية الإضافية ==========
    // Additional Keywords
    keywords_["في"] = TokenType::KEYWORD_IN;
   
    
    DEBUG_PRINT("تمت إضافة 1 كلمة: كلمات إضافية");
    
    // ========== الكلمات المفتاحية - معالجة الأخطاء ==========
    // Keywords - Error Handling (spec 05_python_features.md)
    keywords_["حاول"] = TokenType::KEYWORD_TRY;

    keywords_["امسك"] = TokenType::KEYWORD_CATCH;  // Changed from اصطد to امسك per spec

    keywords_["ارمي"] = TokenType::KEYWORD_THROW;
 
    keywords_["أخيراً"] = TokenType::KEYWORD_FINALLY;
 
    
    DEBUG_PRINT("تمت إضافة 2 كلمات: معالجة الأخطاء");
    
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

    keywords_["من"] = TokenType::KEYWORD_FROM;  // for "from X import Y"

    keywords_["كـ"] = TokenType::KEYWORD_AS;    // for "import X as Y"

    keywords_["صدّر"] = TokenType::KEYWORD_EXPORT;
  
    // NOTE: module and package removed - not in spec
    
    DEBUG_PRINT("تمت إضافة 8 كلمات: الوحدات");
    
    // ========== الكلمات المفتاحية - المتغيرات ==========
    // Keywords - Variables (spec 04_syntax.md)
    keywords_["متغير"] = TokenType::KEYWORD_VAR;
 
    keywords_["ثابت"] = TokenType::KEYWORD_CONST;
    
    keywords_["ساكن"] = TokenType::KEYWORD_STATIC;
  
    
    // Properties (Phase 6.3)
    keywords_["خاصية"] = TokenType::KEYWORD_PROPERTY;

    keywords_["احصل"] = TokenType::KEYWORD_GET;
    keywords_["عيّن"] = TokenType::KEYWORD_SET;
   
    
    DEBUG_PRINT("تمت إضافة 10 كلمات: المتغيرات والخصائص");
    
    // ========== أنواع البيانات الأساسية ==========
    // Basic Data Types (Arabic + English)
    
    // Integer Types (Arabic + English)
    keywords_["رقم"] = TokenType::TYPE_INTEGER;
   
   
    
    // Float/Double Types (Arabic + English)
    keywords_["عشري"] = TokenType::TYPE_DOUBLE;

    
    // String Types (Arabic + English)
    keywords_["نص"] = TokenType::TYPE_STRING;

    
    // Boolean Types (Arabic + English)
    keywords_["منطقي"] = TokenType::TYPE_BOOLEAN;

    
    // Void/None Types (Arabic + English)
    keywords_["فراغ"] = TokenType::TYPE_VOID;
  
    keywords_["عدم"] = TokenType::TYPE_NULL;
   
    
    // Composite Types (Arabic + English)
    keywords_["مصفوفة"] = TokenType::TYPE_ARRAY;

    keywords_["خريطة"] = TokenType::TYPE_MAP;
   
  
    
    // Any Type (spec 01_types.md, 04_syntax.md)
    keywords_["أي"] = TokenType::TYPE_ANY;

    
    DEBUG_PRINT("تمت إضافة 24 كلمة: أنواع البيانات (عربي + إنجليزي)");
    
    // ========== ميزات Python المدعومة ==========
    // Supported Python Features (spec 05_python_features.md)
    keywords_["لامدا"] = TokenType::KEYWORD_LAMBDA;


    keywords_["أنتج"] = TokenType::KEYWORD_YIELD;  // Alternative Arabic yield keyword
  
    keywords_["مولد"] = TokenType::KEYWORD_GENERATOR;  // Generator function keyword (Phase 7)

    keywords_["باستخدام"] = TokenType::KEYWORD_WITH;  // Context manager keyword (Phase 7)

    keywords_["نهاية_استخدام"] = TokenType::KEYWORD_END_WITH;  // End context manager (Phase 7)

    
    DEBUG_PRINT("تمت إضافة 11 كلمات: ميزات Python");
    
    // ========== ميزات C++ المدعومة ==========
    // Supported C++ Features (Phase 7B)
    
    // Templates (Phase 7B.1)
    keywords_["قالب"] = TokenType::KEYWORD_TEMPLATE;      // (AR) لتعريف القوالب / (EN) template keyword

    keywords_["نوع"] = TokenType::KEYWORD_TYPENAME;       // (AR) معامل النوع / (EN) typename

    
    // Namespaces (Phase 7B.5)
    keywords_["فضاء"] = TokenType::KEYWORD_NAMESPACE;     // (AR) فضاء الأسماء / (EN) namespace

    keywords_["نهاية_فضاء"] = TokenType::KEYWORD_END_NAMESPACE;  // (AR) نهاية فضاء الأسماء
 
    // Operator Overloading (Phase 7B.4)
    keywords_["عامل"] = TokenType::KEYWORD_OPERATOR;      // (AR) تحميل العوامل / (EN) operator overloading
 
    
    DEBUG_PRINT("تمت إضافة 10 كلمات: ميزات C++");
    
    // ================================================================
    // (AR) ميزات برمجة أنظمة التشغيل — OS Development Features
    // (EN) OS Development Features (Phase 8)
    // ================================================================
    // (AR) هذا القسم يضيف الكلمات المفتاحية اللازمة لبرمجة أنظمة التشغيل
    //      والتعامل المباشر مع العتاد (Hardware) والذاكرة
    // (EN) This section adds keywords needed for OS development
    //      and direct hardware/memory interaction
    
    // (AR) كلمة 'تجميع' للتجميع المضمّن — يسمح بكتابة أوامر المعالج مباشرة
    // (EN) 'asm' keyword for inline assembly — allows writing CPU instructions directly
    keywords_["تجميع"] = TokenType::KEYWORD_ASM;

    
    // (AR) كلمة 'متطاير' — تمنع المترجم من تحسين عمليات القراءة/الكتابة
    //      ضرورية عند التعامل مع سجلات العتاد (hardware registers)
    // (EN) 'volatile' — prevents compiler from optimizing read/write operations
    keywords_["متطاير"] = TokenType::KEYWORD_VOLATILE;
   
    
    // (AR) كلمة 'خارجي' — تصريح بوجود رمز معرّف في مكان آخر (ربط خارجي)
    // (EN) 'extern' — declares symbol defined elsewhere (external linkage)
    keywords_["خارجي"] = TokenType::KEYWORD_EXTERN;

    
    // (AR) كلمة 'حجم' — تعيد حجم النوع بالبايتات (مثل sizeof في C)
    // (EN) 'sizeof' — returns type size in bytes
    keywords_["حجم"] = TokenType::KEYWORD_SIZEOF;

    
    // (AR) كلمة 'غير_آمن' — كتلة تسمح بالعمليات الخطرة (وصول مباشر للذاكرة)
    // (EN) 'unsafe' — block that allows dangerous operations (raw memory access)
    keywords_["غير_آمن"] = TokenType::KEYWORD_UNSAFE;

    
    // (AR) كلمة 'محزوم' — بنية بلا حشو بين الحقول (للتوافق مع بنيات العتاد)
    // (EN) 'packed' — struct without padding (for hardware structure compatibility)
    keywords_["محزوم"] = TokenType::KEYWORD_PACKED;

    
    // (AR) كلمة 'بلا_رجوع' — دالة لا ترجع أبداً (مثل حلقة النواة الرئيسية)
    // (EN) 'noreturn' — function that never returns (like kernel main loop)
    keywords_["بلا_رجوع"] = TokenType::KEYWORD_NORETURN;
 
    
    // (AR) كلمة 'عارية' — دالة بدون مقدمة/خاتمة (لمعالجات المقاطعات)
    // (EN) 'naked' — function without prologue/epilogue (for interrupt handlers)
    keywords_["عارية"] = TokenType::KEYWORD_NAKED;
    
    
    // (AR) كلمة 'سجل' — تلميح لتخزين المتغير في سجل المعالج
    // (EN) 'register' — hint to store variable in CPU register
    keywords_["سجل"] = TokenType::KEYWORD_REGISTER;
 
    
    // (AR) كلمة 'قسم' — تحديد قسم في ملف الكائن (مثل .text, .data, .bss)
    // (EN) 'section' — specify section in object file
    keywords_["قسم"] = TokenType::KEYWORD_SECTION;
  
    
    DEBUG_PRINT("تمت إضافة 20 كلمة: ميزات برمجة أنظمة التشغيل");
    
    // ================================================================
    // (AR) كلمات مفتاحية نظام النحلة — BeeOS Keywords (Phase 9)
    // (EN) BeeOS development keywords for building an Arabic OS
    // ================================================================
    // (AR) هذا القسم يضيف الكلمات المفتاحية اللازمة لبناء نظام النحلة
    //      تشمل: الملكية، البنى، السمات، الاختبار، الذريات، الوحدات
    
    // (AR) كلمة 'دع' — تعريف متغير مع نظام الملكية (بديل لـ 'متغير' مع دلالة Rust-like)
    // (EN) 'let' — variable declaration with ownership semantics
    keywords_["دع"] = TokenType::KEYWORD_LET;

    
    // (AR) كلمة 'بنية' — تعريف بنية بيانات بسبطة بدون وراثة (مثل struct في C/Rust)
    // (EN) 'struct' — plain data structure without inheritance
    keywords_["بنية"] = TokenType::KEYWORD_STRUCT;
 
    
    // (AR) كلمة 'سمة' — تعريف واجهة/سمة (مثل trait في Rust)
    // (EN) 'trait' — trait/interface definition
    keywords_["سمة"] = TokenType::KEYWORD_TRAIT;

    
    // (AR) كلمة 'نفّذ' — تنفيذ سمة لنوع معين (مثل impl في Rust)
    // (EN) 'impl' — implement trait for a type
    keywords_["نفّذ"] = TokenType::KEYWORD_IMPL;
    keywords_["نفذ"] = TokenType::KEYWORD_IMPL;

    
    // (AR) كلمة 'اختبر' — تعريف دالة اختبار (مثل #[test] في Rust)
    // (EN) 'test' — define a test function
    keywords_["اختبر"] = TokenType::KEYWORD_TEST;

    
    // (AR) كلمة 'ذرّي' — عملية ذرية للمزامنة بين الخيوط (Atomic)
    // (EN) 'atomic' — atomic operation for thread synchronization
    keywords_["ذرّي"] = TokenType::KEYWORD_ATOMIC;
    keywords_["ذري"] = TokenType::KEYWORD_ATOMIC;

    
    // (AR) كلمة 'وحدة' — تعريف وحدة برمجية (مثل mod في Rust)
    // (EN) 'module' — module definition
    keywords_["وحدة"] = TokenType::KEYWORD_MODULE;

    
    // (AR) كلمة 'استخدم' — استيراد مختصر من وحدة (مثل use في Rust)
    // (EN) 'use' — short import from module
    keywords_["استخدم"] = TokenType::KEYWORD_USE;

    
    // (AR) كلمة 'متغيّر' — تعديل مؤشر/مرجع لجعله قابل للتعديل
    // (EN) 'mut' — make pointer/reference mutable
    keywords_["متغيّر"] = TokenType::KEYWORD_MUT;

    
    // (AR) كلمة 'وقت_ترجمة' — تقييم التعبير في وقت الترجمة (مثل comptime في Zig)
    // (EN) 'comptime' — compile-time evaluation
    keywords_["وقت_ترجمة"] = TokenType::KEYWORD_COMPTIME;

    
    DEBUG_PRINT("تمت إضافة 22 كلمة: ميزات نظام النحلة (BeeOS)");
    
    // ================================================================
    // (AR) أنواع بيانات إضافية لنظام النحلة
    // (EN) Additional data types for BeeOS
    // ================================================================
    keywords_["ص64"] = TokenType::TYPE_I64;         // (AR) عدد صحيح 64 بت بإشارة

    keywords_["ط_حجم"] = TokenType::TYPE_USIZE;    // (AR) حجم عنوان بلا إشارة
   
    keywords_["ص_حجم"] = TokenType::TYPE_ISIZE;    // (AR) حجم عنوان بإشارة
   
    
    DEBUG_PRINT("تمت إضافة 6 كلمات: أنواع بيانات إضافية");
    
    // ================================================================
    // (AR) أنواع بيانات منخفضة المستوى — Low-Level Data Types
    // (EN) Fixed-size types for hardware programming
    // ================================================================
    // (AR) هذه الأنواع ضرورية لبرمجة أنظمة التشغيل لأنها تحدد الحجم الدقيق
    //      بالبايتات، على عكس 'رقم' الذي يعتمد على بنية المعالج
    
    keywords_["ط8"] = TokenType::TYPE_U8;          // (AR) عدد صحيح 8 بت بلا إشارة (0-255)
   
    keywords_["بايت"] = TokenType::TYPE_U8;         // (AR) اسم بديل — بايت واحد
  
    keywords_["ط16"] = TokenType::TYPE_U16;         // (AR) عدد صحيح 16 بت بلا إشارة (0-65535)
   
    keywords_["كلمة"] = TokenType::TYPE_U16;        // (AR) اسم بديل — كلمة (word)
 
    keywords_["ط32"] = TokenType::TYPE_U32;         // (AR) عدد صحيح 32 بت بلا إشارة

    keywords_["ط64"] = TokenType::TYPE_U64;         // (AR) عدد صحيح 64 بت بلا إشارة

    keywords_["ص8"] = TokenType::TYPE_I8;           // (AR) عدد صحيح 8 بت بإشارة (-128 إلى 127)

    keywords_["ص16"] = TokenType::TYPE_I16;          // (AR) عدد صحيح 16 بت بإشارة

    keywords_["ص32"] = TokenType::TYPE_I32;          // (AR) عدد صحيح 32 بت بإشارة
  
    keywords_["مؤشر"] = TokenType::TYPE_PTR;        // (AR) مؤشر خام — عنوان ذاكرة

    
    DEBUG_PRINT("تمت إضافة 18 كلمة: أنواع بيانات منخفضة المستوى");
    
    // ========== القيم الحرفية ==========
    // Literal Values (Arabic + English)
    keywords_["صحيح"] = TokenType::LITERAL_TRUE;

    keywords_["خطأ"] = TokenType::LITERAL_FALSE;

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
