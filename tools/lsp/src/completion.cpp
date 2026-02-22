// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: completion.cpp
 * الوصف: مزود الإكمال التلقائي للغة ص - Auto-Completion Provider
 * المرحلة: Phase 8 - US6 - أدوات التطوير
 * المهمة: T093
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يوفر خدمة الإكمال التلقائي (Auto-completion) للغة ص.
 * 
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║  ما هو الإكمال التلقائي؟                                                 ║
 * ╠═══════════════════════════════════════════════════════════════════════════╣
 * ║  ميزة في المحررات تقترح كلمات وعبارات أثناء الكتابة.                     ║
 * ║  يساعد المطور على:                                                       ║
 * ║  - الكتابة بشكل أسرع                                                     ║
 * ║  - تجنب الأخطاء الإملائية                                                ║
 * ║  - اكتشاف الدوال والمتغيرات المتاحة                                      ║
 * ║  - فهم واجهات البرمجة                                                    ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * أنواع الإكمال المدعومة:
 * 
 * ┌─────────────────┬────────────────────────────────────────────────────────────┐
 * │ النوع           │ الوصف                                                      │
 * ├─────────────────┼────────────────────────────────────────────────────────────┤
 * │ الكلمات المفتاحية│ إذا، وإلا، دالة، صنف، إرجاع، نهاية، إلخ                   │
 * │ الأنواع         │ رقم، نص، منطقي، قائمة، خريطة                               │
 * │ الدوال المدمجة  │ اطبع، ادخل، طول، نوع، إلخ                                  │
 * │ المتغيرات       │ المتغيرات المعرفة في النطاق الحالي                         │
 * │ الدوال          │ الدوال المعرفة في المستند                                  │
 * │ الأصناف         │ الأصناف وأعضاؤها                                           │
 * │ الوحدات         │ الوحدات المستوردة                                          │
 * │ القصاصات       │ قوالب كود جاهزة                                            │
 * └─────────────────┴────────────────────────────────────────────────────────────┘
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * أمثلة على الإكمال:
 * 
 * ١. إكمال الكلمات المفتاحية:
 *    المستخدم يكتب: "دا"
 *    الاقتراحات: دالة
 * 
 * ٢. إكمال الدوال المدمجة:
 *    المستخدم يكتب: "اط"
 *    الاقتراحات: اطبع(), اطبع_سطر()
 * 
 * ٣. إكمال أعضاء الكائن:
 *    المستخدم يكتب: "قائمة."
 *    الاقتراحات: أضف(), احذف(), طول(), فارغة()
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0.0
 * 
 * الحمد لله رب العالمين
 */

#include "completion.hpp"
#include "../lsp_types.h"

#include <algorithm>
#include <regex>
#include <unordered_map>

namespace sad {
namespace lsp {

// ═══════════════════════════════════════════════════════════════════════════════
// الكلمات المفتاحية والدوال المدمجة - Keywords and Built-in Functions
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief الكلمات المفتاحية للغة ص
 * قائمة بجميع الكلمات المحجوزة في اللغة
 */
const std::vector<CompletionItem> KEYWORDS = {
    // التحكم في التدفق
    {"إذا", CompletionItemKind::KEYWORD, "شرط - إذا كان الشرط صحيحاً", "إذا (${1:شرط})\n    ${2}\nنهاية"},
    {"وإلا", CompletionItemKind::KEYWORD, "شرط - وإلا (else)", "وإلا\n    ${1}"},
    {"وإلا_إذا", CompletionItemKind::KEYWORD, "شرط - وإلا إذا (else if)", "وإلا_إذا (${1:شرط})\n    ${2}"},
    {"طالما", CompletionItemKind::KEYWORD, "حلقة - طالما الشرط صحيح", "طالما (${1:شرط})\n    ${2}\nنهاية"},
    {"لكل", CompletionItemKind::KEYWORD, "حلقة - لكل عنصر في القائمة", "لكل ${1:عنصر} في ${2:قائمة}\n    ${3}\nنهاية"},
    {"كرر", CompletionItemKind::KEYWORD, "حلقة - كرر عدد مرات", "كرر ${1:عدد}\n    ${2}\nنهاية"},
    
    // التعريفات
    {"دالة", CompletionItemKind::KEYWORD, "تعريف دالة", "دالة ${1:اسم}(${2:معلمات})\n    ${3}\nنهاية"},
    {"صنف", CompletionItemKind::KEYWORD, "تعريف صنف", "صنف ${1:اسم}\n    ${2}\nنهاية"},
    {"واجهة", CompletionItemKind::KEYWORD, "تعريف واجهة", "واجهة ${1:اسم}\n    ${2}\nنهاية"},
    {"وحدة", CompletionItemKind::KEYWORD, "تعريف وحدة", "وحدة ${1:اسم}"},
    {"ثابت", CompletionItemKind::KEYWORD, "تعريف ثابت", "ثابت ${1:اسم} = ${2:قيمة}"},
    
    // التحكم
    {"إرجاع", CompletionItemKind::KEYWORD, "إرجاع قيمة من الدالة", "إرجاع ${1:قيمة}"},
    {"توقف", CompletionItemKind::KEYWORD, "الخروج من الحلقة", "توقف"},
    {"استمر", CompletionItemKind::KEYWORD, "الانتقال للتكرار التالي", "استمر"},
    {"نهاية", CompletionItemKind::KEYWORD, "نهاية الكتلة", "نهاية"},
    
    // الاستيراد
    {"استورد", CompletionItemKind::KEYWORD, "استيراد وحدة", "استورد ${1:وحدة}"},
    {"من", CompletionItemKind::KEYWORD, "استيراد من وحدة", "من ${1:وحدة} استورد ${2:عنصر}"},
    
    // الاستثناءات
    {"حاول", CompletionItemKind::KEYWORD, "بداية كتلة المحاولة", "حاول\n    ${1}\nامسك (${2:خطأ})\n    ${3}\nنهاية"},
    {"امسك", CompletionItemKind::KEYWORD, "معالجة الاستثناء", "امسك (${1:خطأ})\n    ${2}"},
    {"ارمِ", CompletionItemKind::KEYWORD, "رمي استثناء", "ارمِ ${1:خطأ}"},
    
    // أخرى
    {"جديد", CompletionItemKind::KEYWORD, "إنشاء كائن جديد", "جديد ${1:صنف}(${2})"},
    {"ذاتي", CompletionItemKind::KEYWORD, "الإشارة للكائن الحالي", "ذاتي"},
    {"فارغ", CompletionItemKind::KEYWORD, "قيمة فارغة", "فارغ"},
    {"صحيح", CompletionItemKind::KEYWORD, "قيمة منطقية صحيحة", "صحيح"},
    {"خطأ", CompletionItemKind::KEYWORD, "قيمة منطقية خاطئة", "خطأ"},
};

/**
 * @brief أنواع البيانات
 */
const std::vector<CompletionItem> TYPES = {
    {"رقم", CompletionItemKind::TYPE, "نوع رقمي (عدد صحيح أو عشري)", "رقم"},
    {"نص", CompletionItemKind::TYPE, "نوع نصي (سلسلة نصية)", "نص"},
    {"منطقي", CompletionItemKind::TYPE, "نوع منطقي (صحيح/خطأ)", "منطقي"},
    {"قائمة", CompletionItemKind::TYPE, "نوع قائمة (مصفوفة)", "قائمة[${1:نوع}]"},
    {"خريطة", CompletionItemKind::TYPE, "نوع خريطة (قاموس)", "خريطة[${1:مفتاح}، ${2:قيمة}]"},
    {"مجموعة", CompletionItemKind::TYPE, "نوع مجموعة (بدون تكرار)", "مجموعة[${1:نوع}]"},
    {"كائن", CompletionItemKind::TYPE, "نوع كائن عام", "كائن"},
    {"دالة", CompletionItemKind::TYPE, "نوع دالة", "دالة(${1:معلمات}) -> ${2:نوع_الإرجاع}"},
};

/**
 * @brief الدوال المدمجة
 */
const std::vector<CompletionItem> BUILT_IN_FUNCTIONS = {
    // الإدخال والإخراج
    {"اطبع", CompletionItemKind::FUNCTION, "طباعة قيمة على الشاشة\n\nمثال:\naطبع(\"مرحباً\")", "اطبع(${1:قيمة})"},
    {"اطبع_سطر", CompletionItemKind::FUNCTION, "طباعة قيمة مع سطر جديد", "اطبع_سطر(${1:قيمة})"},
    {"ادخل", CompletionItemKind::FUNCTION, "قراءة مدخل من المستخدم\n\nمثال:\nنص اسم = ادخل(\"أدخل اسمك: \")", "ادخل(${1:رسالة})"},
    
    // التحويلات
    {"إلى_رقم", CompletionItemKind::FUNCTION, "تحويل قيمة إلى رقم", "إلى_رقم(${1:قيمة})"},
    {"إلى_نص", CompletionItemKind::FUNCTION, "تحويل قيمة إلى نص", "إلى_نص(${1:قيمة})"},
    {"إلى_منطقي", CompletionItemKind::FUNCTION, "تحويل قيمة إلى منطقي", "إلى_منطقي(${1:قيمة})"},
    
    // العمليات على القوائم
    {"طول", CompletionItemKind::FUNCTION, "الحصول على طول قائمة أو نص", "طول(${1:قائمة_أو_نص})"},
    {"فارغة", CompletionItemKind::FUNCTION, "التحقق من أن القائمة فارغة", "فارغة(${1:قائمة})"},
    {"عكس", CompletionItemKind::FUNCTION, "عكس قائمة أو نص", "عكس(${1:قائمة_أو_نص})"},
    {"رتب", CompletionItemKind::FUNCTION, "ترتيب قائمة", "رتب(${1:قائمة})"},
    {"أضف", CompletionItemKind::FUNCTION, "إضافة عنصر للقائمة", "أضف(${1:قائمة}، ${2:عنصر})"},
    {"احذف", CompletionItemKind::FUNCTION, "حذف عنصر من القائمة", "احذف(${1:قائمة}، ${2:فهرس})"},
    
    // الرياضيات
    {"جذر", CompletionItemKind::FUNCTION, "الجذر التربيعي", "جذر(${1:رقم})"},
    {"قوة", CompletionItemKind::FUNCTION, "رفع رقم لقوة", "قوة(${1:أساس}، ${2:أس})"},
    {"مطلق", CompletionItemKind::FUNCTION, "القيمة المطلقة", "مطلق(${1:رقم})"},
    {"أقصى", CompletionItemKind::FUNCTION, "القيمة الأكبر", "أقصى(${1:أ}، ${2:ب})"},
    {"أدنى", CompletionItemKind::FUNCTION, "القيمة الأصغر", "أدنى(${1:أ}، ${2:ب})"},
    {"عشوائي", CompletionItemKind::FUNCTION, "رقم عشوائي", "عشوائي(${1:من}، ${2:إلى})"},
    
    // النصوص
    {"قسّم", CompletionItemKind::FUNCTION, "تقسيم نص إلى قائمة", "قسّم(${1:نص}، ${2:فاصل})"},
    {"اربط", CompletionItemKind::FUNCTION, "ربط قائمة إلى نص", "اربط(${1:قائمة}، ${2:فاصل})"},
    {"استبدل", CompletionItemKind::FUNCTION, "استبدال نص فرعي", "استبدل(${1:نص}، ${2:قديم}، ${3:جديد})"},
    {"يحتوي", CompletionItemKind::FUNCTION, "التحقق من احتواء نص", "يحتوي(${1:نص}، ${2:نص_فرعي})"},
    {"يبدأ_بـ", CompletionItemKind::FUNCTION, "التحقق من بداية النص", "يبدأ_بـ(${1:نص}، ${2:بداية})"},
    {"ينتهي_بـ", CompletionItemKind::FUNCTION, "التحقق من نهاية النص", "ينتهي_بـ(${1:نص}، ${2:نهاية})"},
    {"أزل_الفراغات", CompletionItemKind::FUNCTION, "إزالة الفراغات من الطرفين", "أزل_الفراغات(${1:نص})"},
    {"إلى_كبير", CompletionItemKind::FUNCTION, "تحويل إلى أحرف كبيرة", "إلى_كبير(${1:نص})"},
    {"إلى_صغير", CompletionItemKind::FUNCTION, "تحويل إلى أحرف صغيرة", "إلى_صغير(${1:نص})"},
    
    // النوع
    {"نوع", CompletionItemKind::FUNCTION, "الحصول على نوع القيمة", "نوع(${1:قيمة})"},
    {"هو_رقم", CompletionItemKind::FUNCTION, "التحقق من أن القيمة رقم", "هو_رقم(${1:قيمة})"},
    {"هو_نص", CompletionItemKind::FUNCTION, "التحقق من أن القيمة نص", "هو_نص(${1:قيمة})"},
    
    // الملفات
    {"اقرأ_ملف", CompletionItemKind::FUNCTION, "قراءة محتوى ملف", "اقرأ_ملف(${1:مسار})"},
    {"اكتب_ملف", CompletionItemKind::FUNCTION, "كتابة محتوى لملف", "اكتب_ملف(${1:مسار}، ${2:محتوى})"},
    {"ملف_موجود", CompletionItemKind::FUNCTION, "التحقق من وجود ملف", "ملف_موجود(${1:مسار})"},
};

/**
 * @brief القصاصات (Code Snippets)
 * قوالب كود جاهزة للاستخدام
 */
const std::vector<CompletionItem> SNIPPETS = {
    // دالة رئيسية
    {"دالة_رئيسية", CompletionItemKind::SNIPPET, "الدالة الرئيسية للبرنامج",
R"(دالة رئيسية()
    ${1:// الكود هنا}
    إرجاع 0
نهاية)"},
    
    // حلقة لكل
    {"لكل_حلقة", CompletionItemKind::SNIPPET, "حلقة لكل عنصر",
R"(لكل ${1:عنصر} في ${2:قائمة}
    ${3:// معالجة العنصر}
نهاية)"},
    
    // حلقة طالما
    {"طالما_حلقة", CompletionItemKind::SNIPPET, "حلقة طالما",
R"(طالما (${1:شرط})
    ${2:// الكود}
نهاية)"},
    
    // شرط كامل
    {"إذا_كامل", CompletionItemKind::SNIPPET, "شرط مع وإلا",
R"(إذا (${1:شرط})
    ${2:// إذا صحيح}
وإلا
    ${3:// إذا خطأ}
نهاية)"},
    
    // صنف كامل
    {"صنف_كامل", CompletionItemKind::SNIPPET, "تعريف صنف كامل",
R"(صنف ${1:اسم_الصنف}
    // الخصائص
    رقم ${2:خاصية1}
    نص ${3:خاصية2}
    
    // المنشئ
    دالة إنشاء(${4:معلمات})
        ${5:// تهيئة}
    نهاية
    
    // الدوال
    دالة ${6:اسم_دالة}()
        ${7:// الكود}
    نهاية
نهاية)"},
    
    // معالجة الأخطاء
    {"حاول_امسك", CompletionItemKind::SNIPPET, "معالجة الاستثناءات",
R"(حاول
    ${1:// الكود الذي قد يسبب خطأ}
امسك (خطأ)
    اطبع("حدث خطأ: " + خطأ)
نهاية)"},
    
    // قراءة ملف
    {"قراءة_ملف", CompletionItemKind::SNIPPET, "قراءة محتوى ملف",
R"(إذا (ملف_موجود("${1:مسار}"))
    نص محتوى = اقرأ_ملف("${1:مسار}")
    ${2:// معالجة المحتوى}
وإلا
    اطبع("الملف غير موجود")
نهاية)"},
    
    // HTTP طلب
    {"طلب_http", CompletionItemKind::SNIPPET, "إرسال طلب HTTP",
R"(استورد شبكة

نص رد = شبكة.طلب("${1:رابط}")
إذا (رد.نجح)
    ${2:// معالجة الرد}
نهاية)"},
};


// ═══════════════════════════════════════════════════════════════════════════════
// صنف مزود الإكمال - Completion Provider Class
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class CompletionProvider
 * @brief يوفر اقتراحات الإكمال التلقائي
 */
class CompletionProvider {
public:
    /**
     * @brief الحصول على الإكمالات
     * @param doc المستند
     * @param pos الموقع
     * @return قائمة الإكمالات
     */
    CompletionList get_completions(const DocumentInfo& doc, const Position& pos) {
        CompletionList list;
        list.is_incomplete = false;
        
        // الحصول على السطر الحالي
        if (pos.line < 0 || pos.line >= static_cast<int>(doc.lines.size())) {
            return list;
        }
        
        const std::string& line = doc.lines[pos.line];
        
        // الحصول على البادئة (ما يكتبه المستخدم)
        std::string prefix = get_prefix(line, pos.character);
        
        // التحقق من السياق
        CompletionContext context = analyze_context(doc, pos);
        
        // جمع الإكمالات حسب السياق
        switch (context.type) {
            case ContextType::MEMBER_ACCESS:
                // إكمال أعضاء الكائن
                add_member_completions(list, context.object_type, prefix);
                break;
                
            case ContextType::IMPORT:
                // إكمال الوحدات
                add_module_completions(list, prefix);
                break;
                
            case ContextType::TYPE_ANNOTATION:
                // إكمال الأنواع
                add_type_completions(list, prefix);
                break;
                
            case ContextType::GENERAL:
            default:
                // إكمال عام
                add_keyword_completions(list, prefix);
                add_type_completions(list, prefix);
                add_builtin_completions(list, prefix);
                add_snippet_completions(list, prefix);
                add_local_completions(list, doc, pos, prefix);
                break;
        }
        
        return list;
    }

private:
    /**
     * @brief أنواع السياق
     */
    enum class ContextType {
        GENERAL,           // سياق عام
        MEMBER_ACCESS,     // وصول لأعضاء (بعد نقطة)
        IMPORT,            // استيراد وحدة
        TYPE_ANNOTATION,   // تحديد نوع
        STRING,            // داخل نص
        COMMENT            // داخل تعليق
    };
    
    /**
     * @brief معلومات السياق
     */
    struct CompletionContext {
        ContextType type = ContextType::GENERAL;
        std::string object_type;  // نوع الكائن (لوصول الأعضاء)
        std::string module_name;  // اسم الوحدة (للاستيراد)
    };
    
    /**
     * @brief الحصول على البادئة
     */
    std::string get_prefix(const std::string& line, int character) {
        if (character <= 0 || character > static_cast<int>(line.length())) {
            return "";
        }
        
        int start = character - 1;
        
        // الرجوع للخلف حتى بداية الكلمة
        while (start >= 0 && is_identifier_char(line[start])) {
            start--;
        }
        
        return line.substr(start + 1, character - start - 1);
    }
    
    /**
     * @brief التحقق من حرف المعرف
     */
    bool is_identifier_char(char c) {
        // دعم الأحرف العربية والإنجليزية
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
               (c >= '0' && c <= '9') ||
               c == '_' ||
               (static_cast<unsigned char>(c) >= 0x80); // UTF-8 للعربية
    }
    
    /**
     * @brief تحليل السياق
     */
    CompletionContext analyze_context(const DocumentInfo& doc, const Position& pos) {
        CompletionContext ctx;
        
        const std::string& line = doc.lines[pos.line];
        
        // التحقق من التعليق
        if (line.find("//") != std::string::npos && 
            static_cast<int>(line.find("//")) < pos.character) {
            ctx.type = ContextType::COMMENT;
            return ctx;
        }
        
        // التحقق من النص
        int quote_count = 0;
        for (int i = 0; i < pos.character && i < static_cast<int>(line.length()); ++i) {
            if (line[i] == '"' || line[i] == '\'') quote_count++;
        }
        if (quote_count % 2 == 1) {
            ctx.type = ContextType::STRING;
            return ctx;
        }
        
        // التحقق من وصول الأعضاء
        if (pos.character > 0 && line[pos.character - 1] == '.') {
            ctx.type = ContextType::MEMBER_ACCESS;
            // TODO: تحديد نوع الكائن
            return ctx;
        }
        
        // التحقق من الاستيراد
        if (line.find("استورد") != std::string::npos ||
            line.find("من") != std::string::npos) {
            ctx.type = ContextType::IMPORT;
            return ctx;
        }
        
        // التحقق من تحديد النوع
        // بعد : أو قبل اسم متغير
        if (line.find(":") != std::string::npos) {
            ctx.type = ContextType::TYPE_ANNOTATION;
            return ctx;
        }
        
        return ctx;
    }
    
    /**
     * @brief إضافة إكمالات الكلمات المفتاحية
     */
    void add_keyword_completions(CompletionList& list, const std::string& prefix) {
        for (const auto& kw : KEYWORDS) {
            if (matches_prefix(kw.label, prefix)) {
                list.items.push_back(kw);
            }
        }
    }
    
    /**
     * @brief إضافة إكمالات الأنواع
     */
    void add_type_completions(CompletionList& list, const std::string& prefix) {
        for (const auto& type : TYPES) {
            if (matches_prefix(type.label, prefix)) {
                list.items.push_back(type);
            }
        }
    }
    
    /**
     * @brief إضافة إكمالات الدوال المدمجة
     */
    void add_builtin_completions(CompletionList& list, const std::string& prefix) {
        for (const auto& func : BUILT_IN_FUNCTIONS) {
            if (matches_prefix(func.label, prefix)) {
                list.items.push_back(func);
            }
        }
    }
    
    /**
     * @brief إضافة إكمالات القصاصات
     */
    void add_snippet_completions(CompletionList& list, const std::string& prefix) {
        for (const auto& snippet : SNIPPETS) {
            if (matches_prefix(snippet.label, prefix)) {
                list.items.push_back(snippet);
            }
        }
    }
    
    /**
     * @brief إضافة إكمالات محلية (من المستند)
     */
    void add_local_completions(CompletionList& list, const DocumentInfo& doc, 
                              const Position& pos, const std::string& prefix) {
        // إضافة الرموز المعرفة في المستند
        for (const auto& sym : doc.symbols) {
            if (matches_prefix(sym.name, prefix)) {
                CompletionItem item;
                item.label = sym.name;
                item.kind = symbol_kind_to_completion_kind(sym.kind);
                item.detail = sym.detail;
                item.documentation = sym.documentation;
                list.items.push_back(item);
            }
        }
    }
    
    /**
     * @brief إضافة إكمالات أعضاء الكائن
     */
    void add_member_completions(CompletionList& list, const std::string& type, 
                               const std::string& prefix) {
        // أعضاء القائمة
        if (type == "قائمة" || type.empty()) {
            std::vector<CompletionItem> list_members = {
                {"أضف", CompletionItemKind::METHOD, "إضافة عنصر للقائمة", "أضف(${1:عنصر})"},
                {"احذف", CompletionItemKind::METHOD, "حذف عنصر بالفهرس", "احذف(${1:فهرس})"},
                {"طول", CompletionItemKind::PROPERTY, "عدد العناصر", "طول"},
                {"فارغة", CompletionItemKind::METHOD, "هل القائمة فارغة؟", "فارغة()"},
                {"أول", CompletionItemKind::PROPERTY, "أول عنصر", "أول"},
                {"آخر", CompletionItemKind::PROPERTY, "آخر عنصر", "آخر"},
                {"عكس", CompletionItemKind::METHOD, "عكس القائمة", "عكس()"},
                {"رتب", CompletionItemKind::METHOD, "ترتيب القائمة", "رتب()"},
                {"ابحث", CompletionItemKind::METHOD, "البحث عن عنصر", "ابحث(${1:عنصر})"},
                {"يحتوي", CompletionItemKind::METHOD, "هل يحتوي على عنصر؟", "يحتوي(${1:عنصر})"},
            };
            
            for (const auto& member : list_members) {
                if (matches_prefix(member.label, prefix)) {
                    list.items.push_back(member);
                }
            }
        }
        
        // أعضاء النص
        if (type == "نص" || type.empty()) {
            std::vector<CompletionItem> string_members = {
                {"طول", CompletionItemKind::PROPERTY, "طول النص", "طول"},
                {"قسّم", CompletionItemKind::METHOD, "تقسيم النص", "قسّم(${1:فاصل})"},
                {"استبدل", CompletionItemKind::METHOD, "استبدال نص فرعي", "استبدل(${1:قديم}، ${2:جديد})"},
                {"يحتوي", CompletionItemKind::METHOD, "هل يحتوي على نص فرعي؟", "يحتوي(${1:نص})"},
                {"يبدأ_بـ", CompletionItemKind::METHOD, "هل يبدأ بـ؟", "يبدأ_بـ(${1:بداية})"},
                {"ينتهي_بـ", CompletionItemKind::METHOD, "هل ينتهي بـ؟", "ينتهي_بـ(${1:نهاية})"},
                {"إلى_كبير", CompletionItemKind::METHOD, "تحويل للحروف الكبيرة", "إلى_كبير()"},
                {"إلى_صغير", CompletionItemKind::METHOD, "تحويل للحروف الصغيرة", "إلى_صغير()"},
                {"أزل_الفراغات", CompletionItemKind::METHOD, "إزالة الفراغات", "أزل_الفراغات()"},
            };
            
            for (const auto& member : string_members) {
                if (matches_prefix(member.label, prefix)) {
                    list.items.push_back(member);
                }
            }
        }
        
        // أعضاء الخريطة
        if (type == "خريطة" || type.empty()) {
            std::vector<CompletionItem> map_members = {
                {"أضف", CompletionItemKind::METHOD, "إضافة زوج مفتاح-قيمة", "أضف(${1:مفتاح}، ${2:قيمة})"},
                {"احذف", CompletionItemKind::METHOD, "حذف بالمفتاح", "احذف(${1:مفتاح})"},
                {"طول", CompletionItemKind::PROPERTY, "عدد العناصر", "طول"},
                {"مفاتيح", CompletionItemKind::METHOD, "قائمة المفاتيح", "مفاتيح()"},
                {"قيم", CompletionItemKind::METHOD, "قائمة القيم", "قيم()"},
                {"يحتوي_مفتاح", CompletionItemKind::METHOD, "هل يحتوي على مفتاح؟", "يحتوي_مفتاح(${1:مفتاح})"},
            };
            
            for (const auto& member : map_members) {
                if (matches_prefix(member.label, prefix)) {
                    list.items.push_back(member);
                }
            }
        }
    }
    
    /**
     * @brief إضافة إكمالات الوحدات
     */
    void add_module_completions(CompletionList& list, const std::string& prefix) {
        std::vector<CompletionItem> modules = {
            {"رياضيات", CompletionItemKind::MODULE, "وحدة العمليات الرياضية", "رياضيات"},
            {"نص", CompletionItemKind::MODULE, "وحدة معالجة النصوص", "نص"},
            {"ملفات", CompletionItemKind::MODULE, "وحدة التعامل مع الملفات", "ملفات"},
            {"شبكة", CompletionItemKind::MODULE, "وحدة الشبكات والإنترنت", "شبكة"},
            {"json", CompletionItemKind::MODULE, "وحدة معالجة JSON", "json"},
            {"وقت", CompletionItemKind::MODULE, "وحدة التاريخ والوقت", "وقت"},
            {"نظام", CompletionItemKind::MODULE, "وحدة النظام", "نظام"},
            {"عشوائي", CompletionItemKind::MODULE, "وحدة الأرقام العشوائية", "عشوائي"},
            {"قاعدة_بيانات", CompletionItemKind::MODULE, "وحدة قواعد البيانات", "قاعدة_بيانات"},
            {"واجهة", CompletionItemKind::MODULE, "وحدة واجهة المستخدم", "واجهة"},
        };
        
        for (const auto& mod : modules) {
            if (matches_prefix(mod.label, prefix)) {
                list.items.push_back(mod);
            }
        }
    }
    
    /**
     * @brief التحقق من مطابقة البادئة
     */
    bool matches_prefix(const std::string& text, const std::string& prefix) {
        if (prefix.empty()) return true;
        if (text.length() < prefix.length()) return false;
        
        // مقارنة غير حساسة لحالة الأحرف
        for (size_t i = 0; i < prefix.length(); ++i) {
            if (std::tolower(text[i]) != std::tolower(prefix[i])) {
                return false;
            }
        }
        
        return true;
    }
    
    /**
     * @brief تحويل نوع الرمز إلى نوع الإكمال
     */
    CompletionItemKind symbol_kind_to_completion_kind(SymbolKind kind) {
        switch (kind) {
            case SymbolKind::FUNCTION: return CompletionItemKind::FUNCTION;
            case SymbolKind::METHOD: return CompletionItemKind::METHOD;
            case SymbolKind::CLASS: return CompletionItemKind::CLASS;
            case SymbolKind::VARIABLE: return CompletionItemKind::VARIABLE;
            case SymbolKind::CONSTANT: return CompletionItemKind::CONSTANT;
            case SymbolKind::PROPERTY: return CompletionItemKind::PROPERTY;
            case SymbolKind::MODULE: return CompletionItemKind::MODULE;
            default: return CompletionItemKind::TEXT;
        }
    }
};

} // namespace lsp
} // namespace sad
