// بسم الله الرحمن الرحيم
/**
 * ══════════════════════════════════════════════════════════════════════════════
 * ملف: ai_copilot_provider.h
 * الوصف: نظام المساعد الذكي الثوري للغة ص - AI Copilot System
 * ══════════════════════════════════════════════════════════════════════════════
 *
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║                    🤖 نظام المساعد الذكي 🤖                              ║
 * ╠═══════════════════════════════════════════════════════════════════════════╣
 * ║                                                                           ║
 * ║   ┌─────────────────────────────────────────────────────────────────┐    ║
 * ║   │                   محرك الذكاء الاصطناعي                        │    ║
 * ║   │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐          │    ║
 * ║   │  │ تحليل   │ │ تنبؤ    │ │ توليد   │ │ تعلم    │          │    ║
 * ║   │  │ السياق  │→│ النوايا │→│ الكود   │→│ التفضيلات│          │    ║
 * ║   │  └──────────┘ └──────────┘ └──────────┘ └──────────┘          │    ║
 * ║   └─────────────────────────────────────────────────────────────────┘    ║
 * ║                                                                           ║
 * ║   الميزات الثورية:                                                       ║
 * ║   ① إكمال ذكي متعدد الأسطر (Multi-line intelligent completion)         ║
 * ║   ② توليد دوال كاملة من الوصف (Function generation from description)   ║
 * ║   ③ إصلاح تلقائي ذكي (Smart auto-fix)                                   ║
 * ║   ④ شرح الكود (Code explanation)                                        ║
 * ║   ⑤ تحويل بين اللغات (Code translation)                                 ║
 * ║   ⑥ اقتراحات أمثلة (Example suggestions)                                ║
 * ║   ⑦ تحسين الأداء (Performance optimization)                             ║
 * ║   ⑧ توثيق تلقائي (Auto documentation)                                   ║
 * ║                                                                           ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 *
 * @author فريق لغة ص
 * @date فبراير 2026
 * الحمد لله رب العالمين
 */

#pragma once

#include "../../include/lsp_protocol_types.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <map>
#include <optional>
#include <chrono>
#include <queue>
#include <mutex>

namespace sad {
namespace lsp {
namespace ai {

// ══════════════════════════════════════════════════════════════════════════════
//  سياق الذكاء الاصطناعي - AI Context
// ══════════════════════════════════════════════════════════════════════════════

/// نوع السياق
enum class ContextType {
    Function,       // داخل دالة
    Class,          // داخل صنف
    Loop,           // داخل حلقة
    Condition,      // داخل شرط
    Global,         // مستوى عام
    Import,         // بعد استيراد
    Comment,        // داخل تعليق
    String          // داخل نص
};

/// معلومات السياق الكامل
struct AIContext {
    ContextType type = ContextType::Global;
    std::string current_function;           // الدالة الحالية
    std::string current_class;              // الصنف الحالي
    std::vector<std::string> local_vars;    // المتغيرات المحلية
    std::vector<std::string> imports;       // الاستيرادات
    std::string preceding_code;             // الكود السابق (آخر 50 سطر)
    std::string following_code;             // الكود التالي (أول 10 أسطر)
    std::string current_line;               // السطر الحالي
    int cursor_offset;                      // موضع المؤشر في السطر
    std::string trigger_text;               // النص المحفز
    bool is_partial_word;                   // هل نكمل كلمة ناقصة؟
};

// ══════════════════════════════════════════════════════════════════════════════
//  اقتراحات الذكاء الاصطناعي - AI Suggestions
// ══════════════════════════════════════════════════════════════════════════════

/// نوع الاقتراح
enum class SuggestionType {
    Completion,         // إكمال كود
    FullFunction,       // دالة كاملة
    CodeBlock,          // كتلة كود
    Fix,                // إصلاح
    Refactor,           // إعادة هيكلة
    Documentation,      // توثيق
    Example,            // مثال
    Translation         // تحويل من لغة أخرى
};

/// اقتراح واحد
struct AISuggestion {
    std::string id;                     // معرف فريد
    SuggestionType type;                // النوع
    std::string title;                  // العنوان المعروض
    std::string description;            // وصف مختصر
    std::string code;                   // الكود المقترح
    std::string explanation;            // شرح الاقتراح
    float confidence;                   // درجة الثقة (0.0 - 1.0)
    Range replace_range;                // النطاق الذي سيُستبدل
    std::vector<std::string> tags;      // وسوم للتصنيف
    
    // معلومات إضافية
    std::string source;                 // مصدر الاقتراح (pattern, ml, template)
    int priority;                       // الأولوية للترتيب
};

// ══════════════════════════════════════════════════════════════════════════════
//  أنماط الكود - Code Patterns
// ══════════════════════════════════════════════════════════════════════════════

/// نمط كود قابل للتعلم
struct CodePattern {
    std::string id;
    std::string name;                   // اسم النمط
    std::string description;            // وصف النمط
    std::string trigger_regex;          // regex للتفعيل
    std::string template_code;          // قالب الكود
    std::vector<std::string> variables; // المتغيرات في القالب
    ContextType required_context;       // السياق المطلوب
    int usage_count;                    // عدد مرات الاستخدام
    float success_rate;                 // معدل النجاح
};

// ══════════════════════════════════════════════════════════════════════════════
//  تحليل النوايا - Intent Analysis
// ══════════════════════════════════════════════════════════════════════════════

/// نية المبرمج المتوقعة
enum class Intent {
    // نوايا البرمجة الأساسية
    DefineVariable,         // تعريف متغير
    DefineFunction,         // تعريف دالة
    DefineClass,            // تعريف صنف
    CallFunction,           // استدعاء دالة
    AccessMember,           // وصول لعضو
    
    // نوايا التحكم
    WriteCondition,         // كتابة شرط
    WriteLoop,              // كتابة حلقة
    HandleError,            // معالجة خطأ
    ReturnValue,            // إرجاع قيمة
    
    // نوايا متقدمة
    ImplementInterface,     // تنفيذ واجهة
    OverrideMethod,         // تجاوز دالة
    CreateConstructor,      // إنشاء منشئ
    AddValidation,          // إضافة تحقق
    AddLogging,             // إضافة سجل
    
    // نوايا غير برمجية
    WriteComment,           // كتابة تعليق
    WriteDocumentation,     // كتابة توثيق
    
    Unknown                 // غير معروف
};

/// نتيجة تحليل النية
struct IntentAnalysis {
    Intent primary_intent;              // النية الرئيسية
    float confidence;                   // الثقة
    std::vector<Intent> secondary;      // نوايا ثانوية محتملة
    std::string detected_pattern;       // النمط المكتشف
    std::map<std::string, std::string> extracted_entities; // الكيانات المستخرجة
};

// ══════════════════════════════════════════════════════════════════════════════
//  المزود الرئيسي - Main Provider
// ══════════════════════════════════════════════════════════════════════════════

/**
 * @brief مزود الذكاء الاصطناعي الثوري
 * 
 * يوفر:
 * - إكمال ذكي متعدد الأسطر
 * - توليد كود من الوصف
 * - إصلاح تلقائي
 * - شرح الكود
 * - تحسين الأداء
 */
class AICopilotProvider {
public:
    AICopilotProvider();
    ~AICopilotProvider();
    
    // ────────────────────────────────────────────────────────────────────────
    //  الإكمال الذكي
    // ────────────────────────────────────────────────────────────────────────
    
    /**
     * @brief الحصول على اقتراحات ذكية
     * @param document الملف الحالي
     * @param position موضع المؤشر
     * @return قائمة الاقتراحات
     */
    std::vector<AISuggestion> get_suggestions(
        const std::string& document,
        const Position& position,
        const std::string& trigger_character = ""
    );
    
    /**
     * @brief إكمال متعدد الأسطر
     * @param context سياق الكود
     * @return الكود المقترح
     */
    std::optional<std::string> get_multiline_completion(const AIContext& context);
    
    // ────────────────────────────────────────────────────────────────────────
    //  توليد الكود
    // ────────────────────────────────────────────────────────────────────────
    
    /**
     * @brief توليد دالة من وصف نصي
     * @param description وصف الدالة (مثل "دالة تحسب مجموع قائمة أرقام")
     * @param context السياق الحالي
     * @return الكود المولد
     */
    std::string generate_function_from_description(
        const std::string& description,
        const AIContext& context
    );
    
    /**
     * @brief توليد صنف من وصف
     * @param description وصف الصنف
     * @return الكود المولد
     */
    std::string generate_class_from_description(const std::string& description);
    
    /**
     * @brief توليد اختبارات للدالة
     * @param function_code كود الدالة
     * @return كود الاختبارات
     */
    std::string generate_tests(const std::string& function_code);
    
    // ────────────────────────────────────────────────────────────────────────
    //  الإصلاح الذكي
    // ────────────────────────────────────────────────────────────────────────
    
    /**
     * @brief اقتراح إصلاحات للأخطاء
     * @param diagnostic التشخيص (الخطأ)
     * @param document الملف
     * @return قائمة الإصلاحات المقترحة
     */
    std::vector<AISuggestion> suggest_fixes(
        const Diagnostic& diagnostic,
        const std::string& document
    );
    
    /**
     * @brief إصلاح تلقائي بناءً على السياق
     */
    std::optional<AISuggestion> auto_fix(
        const std::string& error_message,
        const std::string& code_snippet,
        const AIContext& context
    );
    
    // ────────────────────────────────────────────────────────────────────────
    //  الشرح والتوثيق
    // ────────────────────────────────────────────────────────────────────────
    
    /**
     * @brief شرح كود معين
     * @param code الكود
     * @param detail_level مستوى التفصيل (1-3)
     * @return الشرح
     */
    std::string explain_code(const std::string& code, int detail_level = 2);
    
    /**
     * @brief توليد توثيق للدالة
     * @param function_code كود الدالة
     * @return التوثيق
     */
    std::string generate_documentation(const std::string& function_code);
    
    /**
     * @brief اقتراح تحسينات على التوثيق
     */
    std::vector<std::string> suggest_documentation_improvements(
        const std::string& existing_doc,
        const std::string& code
    );
    
    // ────────────────────────────────────────────────────────────────────────
    //  التحويل والترجمة
    // ────────────────────────────────────────────────────────────────────────
    
    /**
     * @brief تحويل كود من لغة أخرى إلى ص
     * @param code الكود الأصلي
     * @param source_lang اللغة المصدر (python, javascript, etc)
     * @return الكود بلغة ص
     */
    std::string translate_to_sad(
        const std::string& code,
        const std::string& source_lang
    );
    
    /**
     * @brief تحويل كود ص إلى لغة أخرى
     */
    std::string translate_from_sad(
        const std::string& code,
        const std::string& target_lang
    );
    
    // ────────────────────────────────────────────────────────────────────────
    //  التحسينات
    // ────────────────────────────────────────────────────────────────────────
    
    /**
     * @brief اقتراح تحسينات الأداء
     */
    std::vector<AISuggestion> suggest_performance_improvements(
        const std::string& code
    );
    
    /**
     * @brief اقتراح إعادة هيكلة
     */
    std::vector<AISuggestion> suggest_refactoring(
        const std::string& code,
        const std::string& refactor_type = ""
    );
    
    // ────────────────────────────────────────────────────────────────────────
    //  التعلم والتكيف
    // ────────────────────────────────────────────────────────────────────────
    
    /**
     * @brief تسجيل قبول اقتراح (للتعلم)
     */
    void record_acceptance(const std::string& suggestion_id);
    
    /**
     * @brief تسجيل رفض اقتراح
     */
    void record_rejection(const std::string& suggestion_id);
    
    /**
     * @brief تعلم نمط جديد من كود المستخدم
     */
    void learn_pattern(const std::string& code, const std::string& context);
    
private:
    // ────────────────────────────────────────────────────────────────────────
    //  أدوات داخلية
    // ────────────────────────────────────────────────────────────────────────
    
    /// بناء السياق من الملف والموضع
    AIContext build_context(const std::string& document, const Position& pos);
    
    /// تحليل نية المبرمج
    IntentAnalysis analyze_intent(const AIContext& context);
    
    /// إيجاد أنماط مطابقة
    std::vector<CodePattern> find_matching_patterns(const AIContext& context);
    
    /// توليد كود من قالب
    std::string expand_template(
        const std::string& template_code,
        const std::map<std::string, std::string>& variables
    );
    
    /// تقييم جودة الاقتراح
    float evaluate_suggestion(const AISuggestion& suggestion, const AIContext& context);
    
    /// فلترة وترتيب الاقتراحات
    std::vector<AISuggestion> filter_and_rank(
        std::vector<AISuggestion>& suggestions,
        const AIContext& context
    );
    
    // ────────────────────────────────────────────────────────────────────────
    //  البيانات الداخلية
    // ────────────────────────────────────────────────────────────────────────
    
    std::vector<CodePattern> patterns_;          // أنماط الكود المتعلمة
    std::unordered_map<std::string, int> usage_stats_;  // إحصائيات الاستخدام
    std::mutex mutex_;                           // للتزامن
    
    // إعدادات
    bool learning_enabled_ = true;               // التعلم مفعل
    float min_confidence_ = 0.3f;                // الحد الأدنى للثقة
    int max_suggestions_ = 10;                   // أقصى عدد اقتراحات
};

// ══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ══════════════════════════════════════════════════════════════════════════════

/// تحويل نوع السياق لنص
std::string context_type_to_string(ContextType type);

/// تحويل نوع الاقتراح لنص
std::string suggestion_type_to_string(SuggestionType type);

/// تحويل النية لنص
std::string intent_to_string(Intent intent);

} // namespace ai
} // namespace lsp
} // namespace sad
