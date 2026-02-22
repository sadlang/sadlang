/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: suggestions.cpp                                                      ║
 * ║   الوصف: نظام اقتراحات الإصلاح الذكي                                        ║
 * ║   المهمة: T161 - Phase 15 (US13)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                         دليل المبتدئين | Beginner's Guide
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 ما هي اقتراحات الإصلاح؟ | What are Fix Suggestions?
 * ─────────────────────────────────────────────────────────
 *
 * عندما تكتب برنامجاً وتخطئ في شيء ما، يقوم المترجم بإخبارك بالخطأ.
 * لكن معرفة الخطأ وحدها ليست كافية! نحتاج أيضاً لمعرفة كيف نصلحه.
 *
 * 📝 مثال بسيط | Simple Example:
 *
 *    أنت تكتب: اسم = 5 + "نص"
 *
 *    الخطأ: لا يمكن جمع عدد مع نص!
 *
 *    💡 اقتراحات الإصلاح:
 *       1. حوّل العدد إلى نص: اسم = نص(5) + "نص"
 *       2. أو حوّل النص إلى عدد: اسم = 5 + عدد("نص")
 *
 * 🎯 لماذا نحتاج هذا النظام؟ | Why Do We Need This System?
 * ─────────────────────────────────────────────────────────
 *
 * 1️⃣ التعليم: يساعد المبتدئين على فهم أخطائهم
 * 2️⃣ الإنتاجية: يوفر الوقت بدلاً من البحث عن الحل
 * 3️⃣ الجودة: يقترح أفضل الممارسات
 *
 * 🔧 كيف يعمل النظام؟ | How Does It Work?
 * ─────────────────────────────────────────
 *
 *    ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
 *    │   الكود     │ --> │   الخطأ     │ --> │  الاقتراح   │
 *    │  المكتوب   │     │   المكتشف   │     │  للإصلاح    │
 *    └─────────────┘     └─────────────┘     └─────────────┘
 *           ↓                   ↓                   ↓
 *       نص + عدد          نوع غير متوافق      استخدم نص()
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <algorithm>
#include <sstream>
#include <cmath>

namespace sad {
namespace errors {

// ═══════════════════════════════════════════════════════════════════════════════
//                    أنواع الاقتراحات | Suggestion Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📊 نوع الاقتراح | Suggestion Type
 *
 * يحدد نوع الإجراء المقترح:
 * - REPLACEMENT: استبدال الكود الخاطئ
 * - INSERTION: إضافة كود جديد
 * - DELETION: حذف كود زائد
 * - REFACTOR: إعادة هيكلة الكود
 */
enum class SuggestionType {
    REPLACEMENT,  // استبدال
    INSERTION,    // إضافة
    DELETION,     // حذف
    REFACTOR,     // إعادة هيكلة
    HINT          // تلميح فقط
};

/**
 * 📈 مستوى الثقة | Confidence Level
 *
 * كم نحن متأكدون من صحة الاقتراح:
 * - HIGH: متأكدون 90%+ - اقترح مباشرة
 * - MEDIUM: متأكدون 70%+ - اقترح مع تحذير
 * - LOW: أقل من 70% - اقترح كاحتمال
 */
enum class ConfidenceLevel {
    HIGH,    // ثقة عالية
    MEDIUM,  // ثقة متوسطة
    LOW      // ثقة منخفضة
};

/**
 * 🔧 فئة الخطأ | Error Category
 *
 * تصنيف الأخطاء لتحديد نوع الاقتراحات:
 */
enum class ErrorCategory {
    TYPE_MISMATCH,      // عدم تطابق الأنواع
    UNDEFINED_SYMBOL,   // رمز غير معرّف
    SYNTAX_ERROR,       // خطأ نحوي
    IMPORT_ERROR,       // خطأ استيراد
    SCOPE_ERROR,        // خطأ نطاق
    ARGUMENT_ERROR,     // خطأ معاملات
    ARABIC_ENCODING,    // خطأ ترميز عربي
    NAMING_CONVENTION,  // خطأ تسمية
    LOGIC_ERROR,        // خطأ منطقي
    PERFORMANCE_HINT    // تلميح أداء
};

// ═══════════════════════════════════════════════════════════════════════════════
//                      هيكل الاقتراح | Suggestion Structure
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 💾 موقع في الكود | Code Location
 */
struct CodeLocation {
    std::string filename;
    size_t line;
    size_t column;
    size_t endLine;
    size_t endColumn;
    
    CodeLocation()
        : line(0), column(0), endLine(0), endColumn(0) {}
    
    CodeLocation(const std::string& file, size_t l, size_t c)
        : filename(file), line(l), column(c), endLine(l), endColumn(c) {}
    
    CodeLocation(const std::string& file, size_t l, size_t c, size_t el, size_t ec)
        : filename(file), line(l), column(c), endLine(el), endColumn(ec) {}
    
    std::string toString() const {
        std::ostringstream oss;
        oss << filename << ":" << line << ":" << column;
        return oss.str();
    }
};

/**
 * 🔄 تعديل مقترح | Suggested Edit
 *
 * يصف تعديلاً واحداً على الكود:
 * - أين يقع التعديل
 * - ماذا نحذف
 * - ماذا نضيف
 */
struct SuggestedEdit {
    CodeLocation location;
    std::string oldText;      // النص الحالي (للحذف/الاستبدال)
    std::string newText;      // النص الجديد (للإضافة/الاستبدال)
    
    SuggestedEdit() = default;
    
    SuggestedEdit(const CodeLocation& loc, const std::string& oldT, const std::string& newT)
        : location(loc), oldText(oldT), newText(newT) {}
};

/**
 * 💡 اقتراح إصلاح | Fix Suggestion
 *
 * يحتوي على:
 * - وصف الإصلاح بالعربية
 * - شرح لماذا هذا الإصلاح مناسب
 * - التعديلات المطلوبة
 * - مستوى الثقة
 */
struct FixSuggestion {
    std::string title;              // عنوان الاقتراح
    std::string titleEn;            // عنوان بالإنجليزية
    std::string description;        // شرح مفصل
    std::string descriptionEn;      // شرح بالإنجليزية
    SuggestionType type;            // نوع الاقتراح
    ConfidenceLevel confidence;     // مستوى الثقة
    std::vector<SuggestedEdit> edits;  // التعديلات
    std::string codeExample;        // مثال للكود الصحيح
    
    FixSuggestion()
        : type(SuggestionType::HINT)
        , confidence(ConfidenceLevel::MEDIUM)
    {}
    
    /**
     * 📝 تنسيق الاقتراح للعرض
     */
    std::string format(bool useArabic = true) const {
        std::ostringstream oss;
        
        // عنوان
        oss << "💡 " << (useArabic ? title : titleEn) << "\n";
        
        // شرح
        if (!description.empty()) {
            oss << "   " << (useArabic ? description : descriptionEn) << "\n";
        }
        
        // مثال
        if (!codeExample.empty()) {
            oss << "\n   مثال | Example:\n";
            oss << "   ─────────────────\n";
            // تقسيم المثال إلى أسطر
            std::istringstream stream(codeExample);
            std::string line;
            while (std::getline(stream, line)) {
                oss << "   " << line << "\n";
            }
        }
        
        return oss.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                     مولد الاقتراحات | Suggestion Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 SuggestionGenerator
 *
 * الفئة الرئيسية لتوليد اقتراحات الإصلاح
 *
 * 📚 كيف تعمل:
 *    1. استقبال معلومات الخطأ
 *    2. تحليل السياق
 *    3. البحث عن أنماط مشابهة
 *    4. توليد اقتراحات مناسبة
 *    5. ترتيب الاقتراحات حسب الثقة
 */
class SuggestionGenerator {
public:
    SuggestionGenerator() {
        initializePatterns();
    }
    
    /**
     * 🔍 توليد اقتراحات لخطأ معين
     *
     * @param category فئة الخطأ
     * @param errorCode رمز الخطأ
     * @param context السياق (الكود المحيط)
     * @param details تفاصيل إضافية
     * @return قائمة بالاقتراحات
     */
    std::vector<FixSuggestion> generateSuggestions(
        ErrorCategory category,
        const std::string& errorCode,
        const std::string& context,
        const std::unordered_map<std::string, std::string>& details
    ) {
        std::vector<FixSuggestion> suggestions;
        
        switch (category) {
            case ErrorCategory::TYPE_MISMATCH:
                suggestions = generateTypeMismatchSuggestions(details);
                break;
                
            case ErrorCategory::UNDEFINED_SYMBOL:
                suggestions = generateUndefinedSymbolSuggestions(details, context);
                break;
                
            case ErrorCategory::SYNTAX_ERROR:
                suggestions = generateSyntaxSuggestions(details, context);
                break;
                
            case ErrorCategory::IMPORT_ERROR:
                suggestions = generateImportSuggestions(details);
                break;
                
            case ErrorCategory::ARGUMENT_ERROR:
                suggestions = generateArgumentSuggestions(details);
                break;
                
            case ErrorCategory::ARABIC_ENCODING:
                suggestions = generateArabicEncodingSuggestions(details);
                break;
                
            case ErrorCategory::NAMING_CONVENTION:
                suggestions = generateNamingSuggestions(details);
                break;
                
            default:
                break;
        }
        
        // ترتيب حسب الثقة
        std::sort(suggestions.begin(), suggestions.end(),
            [](const FixSuggestion& a, const FixSuggestion& b) {
                return static_cast<int>(a.confidence) < static_cast<int>(b.confidence);
            });
        
        return suggestions;
    }
    
private:
    // أنماط الأخطاء الشائعة
    std::unordered_map<std::string, std::vector<std::string>> commonPatterns_;
    
    /**
     * 🔧 تهيئة أنماط الأخطاء الشائعة
     */
    void initializePatterns() {
        // أخطاء التحويل الشائعة
        commonPatterns_["type_conversion"] = {
            "نص",    // String
            "عدد",    // Number
            "صحيح",   // Integer
            "عشري",   // Float
            "منطقي"   // Boolean
        };
        
        // كلمات محجوزة
        commonPatterns_["reserved_words"] = {
            "دالة", "إذا", "وإلا", "طالما", "لكل",
            "أرجع", "صنف", "جديد", "هذا", "عدم"
        };
        
        // عمليات شائعة
        commonPatterns_["common_ops"] = {
            "+", "-", "*", "/", "==", "!=", "<", ">", "&&", "||"
        };
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    اقتراحات عدم تطابق الأنواع
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * 🔄 اقتراحات لأخطاء عدم تطابق الأنواع
     */
    std::vector<FixSuggestion> generateTypeMismatchSuggestions(
        const std::unordered_map<std::string, std::string>& details
    ) {
        std::vector<FixSuggestion> suggestions;
        
        std::string expectedType = details.count("expected") ? details.at("expected") : "";
        std::string actualType = details.count("actual") ? details.at("actual") : "";
        std::string expression = details.count("expression") ? details.at("expression") : "";
        
        // اقتراح 1: التحويل الصريح
        if (!expectedType.empty() && !actualType.empty()) {
            FixSuggestion conversion;
            conversion.title = "حوّل القيمة إلى النوع المطلوب";
            conversion.titleEn = "Convert value to expected type";
            
            if (expectedType == "نص" || expectedType == "string") {
                conversion.description = "استخدم دالة نص() لتحويل القيمة إلى نص";
                conversion.descriptionEn = "Use نص() function to convert value to string";
                conversion.codeExample = "متغير نتيجة = نص(" + expression + ")";
                conversion.confidence = ConfidenceLevel::HIGH;
            }
            else if (expectedType == "عدد" || expectedType == "number") {
                conversion.description = "استخدم دالة عدد() لتحويل القيمة إلى عدد";
                conversion.descriptionEn = "Use عدد() function to convert value to number";
                conversion.codeExample = "متغير نتيجة = عدد(" + expression + ")";
                conversion.confidence = ConfidenceLevel::HIGH;
            }
            else if (expectedType == "منطقي" || expectedType == "boolean") {
                conversion.description = "استخدم دالة منطقي() للتحويل إلى قيمة منطقية";
                conversion.descriptionEn = "Use منطقي() function to convert to boolean";
                conversion.codeExample = "متغير نتيجة = منطقي(" + expression + ")";
                conversion.confidence = ConfidenceLevel::MEDIUM;
            }
            
            conversion.type = SuggestionType::REPLACEMENT;
            suggestions.push_back(conversion);
        }
        
        // اقتراح 2: تغيير نوع المتغير
        {
            FixSuggestion changeType;
            changeType.title = "غيّر نوع المتغير";
            changeType.titleEn = "Change variable type";
            changeType.description = "إذا كان المتغير يجب أن يحتوي على هذا النوع، غيّر تعريفه";
            changeType.descriptionEn = "If variable should hold this type, change its declaration";
            changeType.type = SuggestionType::HINT;
            changeType.confidence = ConfidenceLevel::LOW;
            suggestions.push_back(changeType);
        }
        
        // اقتراح 3: استخدام قالب (Template)
        if (actualType.find("قائمة") != std::string::npos || 
            actualType.find("مصفوفة") != std::string::npos) {
            FixSuggestion useTemplate;
            useTemplate.title = "حدد نوع عناصر المجموعة";
            useTemplate.titleEn = "Specify collection element type";
            useTemplate.description = "استخدم <نوع> لتحديد نوع العناصر";
            useTemplate.descriptionEn = "Use <type> to specify element type";
            useTemplate.codeExample = "متغير قائمتي: قائمة<" + expectedType + "> = []";
            useTemplate.type = SuggestionType::REPLACEMENT;
            useTemplate.confidence = ConfidenceLevel::MEDIUM;
            suggestions.push_back(useTemplate);
        }
        
        return suggestions;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    اقتراحات الرموز غير المعرّفة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * 🔍 اقتراحات للرموز غير المعرّفة
     */
    std::vector<FixSuggestion> generateUndefinedSymbolSuggestions(
        const std::unordered_map<std::string, std::string>& details,
        const std::string& context
    ) {
        std::vector<FixSuggestion> suggestions;
        
        std::string symbolName = details.count("symbol") ? details.at("symbol") : "";
        std::string availableSymbols = details.count("available") ? details.at("available") : "";
        
        // اقتراح 1: تصحيح إملائي
        std::vector<std::string> similar = findSimilarSymbols(symbolName, availableSymbols);
        if (!similar.empty()) {
            FixSuggestion typo;
            typo.title = "هل قصدت: " + similar[0] + "؟";
            typo.titleEn = "Did you mean: " + similar[0] + "?";
            typo.description = "يبدو أن هناك خطأ إملائي في اسم الرمز";
            typo.descriptionEn = "There seems to be a typo in the symbol name";
            typo.type = SuggestionType::REPLACEMENT;
            typo.confidence = ConfidenceLevel::HIGH;
            
            SuggestedEdit edit;
            edit.oldText = symbolName;
            edit.newText = similar[0];
            typo.edits.push_back(edit);
            
            suggestions.push_back(typo);
        }
        
        // اقتراح 2: تعريف المتغير
        {
            FixSuggestion define;
            define.title = "عرّف المتغير أولاً";
            define.titleEn = "Define the variable first";
            define.description = "المتغير غير معرّف. أضف تعريفاً له قبل استخدامه";
            define.descriptionEn = "Variable is undefined. Add a definition before using it";
            define.codeExample = "متغير " + symbolName + " = قيمة_ابتدائية";
            define.type = SuggestionType::INSERTION;
            define.confidence = ConfidenceLevel::MEDIUM;
            suggestions.push_back(define);
        }
        
        // اقتراح 3: استيراد وحدة
        {
            FixSuggestion import;
            import.title = "استورد الوحدة المطلوبة";
            import.titleEn = "Import required module";
            import.description = "قد يكون الرمز موجوداً في وحدة لم تستوردها";
            import.descriptionEn = "Symbol might exist in a module you haven't imported";
            import.codeExample = "استورد \"اسم_الوحدة\"";
            import.type = SuggestionType::INSERTION;
            import.confidence = ConfidenceLevel::LOW;
            suggestions.push_back(import);
        }
        
        return suggestions;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    اقتراحات الأخطاء النحوية
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * 📝 اقتراحات للأخطاء النحوية
     */
    std::vector<FixSuggestion> generateSyntaxSuggestions(
        const std::unordered_map<std::string, std::string>& details,
        const std::string& context
    ) {
        std::vector<FixSuggestion> suggestions;
        
        std::string expected = details.count("expected") ? details.at("expected") : "";
        std::string found = details.count("found") ? details.at("found") : "";
        
        // فحص الأقواس
        if (expected.find("(") != std::string::npos || expected.find(")") != std::string::npos ||
            expected.find("{") != std::string::npos || expected.find("}") != std::string::npos) {
            FixSuggestion brackets;
            brackets.title = "تحقق من الأقواس";
            brackets.titleEn = "Check brackets";
            brackets.description = "تأكد من إغلاق جميع الأقواس المفتوحة";
            brackets.descriptionEn = "Make sure all opened brackets are closed";
            brackets.type = SuggestionType::HINT;
            brackets.confidence = ConfidenceLevel::HIGH;
            
            brackets.codeExample = 
                "// صحيح:\n"
                "دالة مثال() {\n"
                "    أرجع 1\n"
                "}\n"
                "\n"
                "// خاطئ:\n"
                "دالة مثال() {\n"
                "    أرجع 1\n"
                "// قوس مفقود!";
            
            suggestions.push_back(brackets);
        }
        
        // فحص النقطتين
        if (expected == ":" || found == ":") {
            FixSuggestion colon;
            colon.title = "أضف نقطتين بعد الشرط";
            colon.titleEn = "Add colon after condition";
            colon.description = "في لغة سعد، نحتاج نقطتين (:) بعد الشروط والتعريفات";
            colon.descriptionEn = "In Sad, we need a colon (:) after conditions and definitions";
            colon.codeExample = 
                "// صحيح:\n"
                "إذا عدد > 0:\n"
                "    اطبع(\"موجب\")\n"
                "\n"
                "// خاطئ:\n"
                "إذا عدد > 0\n"
                "    اطبع(\"موجب\")";
            colon.type = SuggestionType::INSERTION;
            colon.confidence = ConfidenceLevel::HIGH;
            suggestions.push_back(colon);
        }
        
        // فحص الفاصلة المنقوطة (لا نحتاجها في سعد!)
        if (found == ";") {
            FixSuggestion semicolon;
            semicolon.title = "أزل الفاصلة المنقوطة";
            semicolon.titleEn = "Remove semicolon";
            semicolon.description = "لغة سعد لا تحتاج فاصلة منقوطة في نهاية السطر";
            semicolon.descriptionEn = "Sad language doesn't need semicolons at end of lines";
            semicolon.codeExample = 
                "// صحيح:\n"
                "متغير س = 5\n"
                "\n"
                "// خاطئ:\n"
                "متغير س = 5;";
            semicolon.type = SuggestionType::DELETION;
            semicolon.confidence = ConfidenceLevel::HIGH;
            suggestions.push_back(semicolon);
        }
        
        return suggestions;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    اقتراحات أخطاء الاستيراد
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * 📦 اقتراحات لأخطاء الاستيراد
     */
    std::vector<FixSuggestion> generateImportSuggestions(
        const std::unordered_map<std::string, std::string>& details
    ) {
        std::vector<FixSuggestion> suggestions;
        
        std::string moduleName = details.count("module") ? details.at("module") : "";
        
        // اقتراح 1: تصحيح اسم الوحدة
        {
            FixSuggestion checkName;
            checkName.title = "تحقق من اسم الوحدة";
            checkName.titleEn = "Check module name";
            checkName.description = "تأكد من كتابة اسم الوحدة بشكل صحيح";
            checkName.descriptionEn = "Make sure the module name is spelled correctly";
            checkName.type = SuggestionType::HINT;
            checkName.confidence = ConfidenceLevel::HIGH;
            suggestions.push_back(checkName);
        }
        
        // اقتراح 2: تثبيت الحزمة
        {
            FixSuggestion install;
            install.title = "ثبّت الحزمة المطلوبة";
            install.titleEn = "Install required package";
            install.description = "قد تحتاج لتثبيت الحزمة باستخدام مدير الحزم";
            install.descriptionEn = "You may need to install the package using package manager";
            install.codeExample = "ص ثبّت " + moduleName;
            install.type = SuggestionType::HINT;
            install.confidence = ConfidenceLevel::MEDIUM;
            suggestions.push_back(install);
        }
        
        // اقتراح 3: فحص المسار
        {
            FixSuggestion checkPath;
            checkPath.title = "تحقق من مسار الملف";
            checkPath.titleEn = "Check file path";
            checkPath.description = "إذا كانت وحدة محلية، تأكد من صحة المسار";
            checkPath.descriptionEn = "If it's a local module, verify the path is correct";
            checkPath.codeExample = 
                "// استيراد من نفس المجلد:\n"
                "استورد \"./وحدتي\"\n"
                "\n"
                "// استيراد من مجلد آخر:\n"
                "استورد \"../مجلد/وحدة\"";
            checkPath.type = SuggestionType::HINT;
            checkPath.confidence = ConfidenceLevel::LOW;
            suggestions.push_back(checkPath);
        }
        
        return suggestions;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    اقتراحات أخطاء المعاملات
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * 🔢 اقتراحات لأخطاء المعاملات
     */
    std::vector<FixSuggestion> generateArgumentSuggestions(
        const std::unordered_map<std::string, std::string>& details
    ) {
        std::vector<FixSuggestion> suggestions;
        
        std::string functionName = details.count("function") ? details.at("function") : "";
        std::string expected = details.count("expected_count") ? details.at("expected_count") : "";
        std::string actual = details.count("actual_count") ? details.at("actual_count") : "";
        
        // اقتراح 1: تصحيح عدد المعاملات
        {
            FixSuggestion fixCount;
            fixCount.title = "صحّح عدد المعاملات";
            fixCount.titleEn = "Fix argument count";
            fixCount.description = "الدالة " + functionName + " تتوقع " + expected + 
                                  " معاملات، لكنك أعطيتها " + actual;
            fixCount.descriptionEn = "Function " + functionName + " expects " + expected +
                                    " arguments, but got " + actual;
            fixCount.type = SuggestionType::REPLACEMENT;
            fixCount.confidence = ConfidenceLevel::HIGH;
            suggestions.push_back(fixCount);
        }
        
        // اقتراح 2: استخدام معاملات اختيارية
        {
            FixSuggestion optional;
            optional.title = "استخدم المعاملات الاختيارية";
            optional.titleEn = "Use optional arguments";
            optional.description = "بعض المعاملات قد تكون اختيارية ولها قيم افتراضية";
            optional.descriptionEn = "Some arguments may be optional with default values";
            optional.codeExample = 
                "// تعريف دالة بمعامل اختياري:\n"
                "دالة تحية(اسم، تحية = \"مرحباً\"):\n"
                "    أرجع تحية + \" \" + اسم\n"
                "\n"
                "// استدعاء بدون المعامل الاختياري:\n"
                "تحية(\"أحمد\")  // مرحباً أحمد";
            optional.type = SuggestionType::HINT;
            optional.confidence = ConfidenceLevel::MEDIUM;
            suggestions.push_back(optional);
        }
        
        return suggestions;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    اقتراحات أخطاء الترميز العربي
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * 🔤 اقتراحات لأخطاء الترميز العربي
     */
    std::vector<FixSuggestion> generateArabicEncodingSuggestions(
        const std::unordered_map<std::string, std::string>& details
    ) {
        std::vector<FixSuggestion> suggestions;
        
        // اقتراح 1: تحقق من ترميز الملف
        {
            FixSuggestion encoding;
            encoding.title = "استخدم ترميز UTF-8";
            encoding.titleEn = "Use UTF-8 encoding";
            encoding.description = "تأكد من حفظ الملف بترميز UTF-8 لدعم العربية";
            encoding.descriptionEn = "Make sure file is saved with UTF-8 encoding for Arabic support";
            encoding.type = SuggestionType::HINT;
            encoding.confidence = ConfidenceLevel::HIGH;
            suggestions.push_back(encoding);
        }
        
        // اقتراح 2: تحقق من التشكيل
        {
            FixSuggestion tashkeel;
            tashkeel.title = "انتبه للتشكيل";
            tashkeel.titleEn = "Watch for diacritics";
            tashkeel.description = "التشكيل قد يسبب مشاكل في المقارنة. استخدم جرّد_تشكيل()";
            tashkeel.descriptionEn = "Diacritics may cause comparison issues. Use جرّد_تشكيل()";
            tashkeel.codeExample = 
                "// مقارنة مع تجاهل التشكيل:\n"
                "إذا جرّد_تشكيل(نص1) == جرّد_تشكيل(نص2):\n"
                "    اطبع(\"متطابقان\")";
            tashkeel.type = SuggestionType::HINT;
            tashkeel.confidence = ConfidenceLevel::MEDIUM;
            suggestions.push_back(tashkeel);
        }
        
        // اقتراح 3: تطبيع Unicode
        {
            FixSuggestion normalize;
            normalize.title = "طبّع النص (NFC)";
            normalize.titleEn = "Normalize text (NFC)";
            normalize.description = "استخدم التطبيع لتوحيد تمثيل الحروف";
            normalize.descriptionEn = "Use normalization to unify character representation";
            normalize.codeExample = "متغير نص_مطبّع = طبّع(نص، \"NFC\")";
            normalize.type = SuggestionType::HINT;
            normalize.confidence = ConfidenceLevel::LOW;
            suggestions.push_back(normalize);
        }
        
        return suggestions;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    اقتراحات أخطاء التسمية
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * 📛 اقتراحات لأخطاء التسمية
     */
    std::vector<FixSuggestion> generateNamingSuggestions(
        const std::unordered_map<std::string, std::string>& details
    ) {
        std::vector<FixSuggestion> suggestions;
        
        std::string name = details.count("name") ? details.at("name") : "";
        std::string issue = details.count("issue") ? details.at("issue") : "";
        
        // اقتراح: اتباع أسلوب التسمية
        {
            FixSuggestion naming;
            naming.title = "اتبع أسلوب التسمية المعتمد";
            naming.titleEn = "Follow naming convention";
            naming.description = "استخدم أسماء واضحة وذات معنى";
            naming.descriptionEn = "Use clear and meaningful names";
            naming.codeExample = 
                "// أسماء جيدة:\n"
                "متغير عداد_الطلاب = 0\n"
                "دالة احسب_المجموع(أرقام):\n"
                "    ...\n"
                "\n"
                "// أسماء غير جيدة:\n"
                "متغير س = 0\n"
                "دالة د(ق):\n"
                "    ...";
            naming.type = SuggestionType::HINT;
            naming.confidence = ConfidenceLevel::LOW;
            suggestions.push_back(naming);
        }
        
        return suggestions;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                          أدوات مساعدة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * 🔍 البحث عن رموز مشابهة (للتصحيح الإملائي)
     *
     * يستخدم خوارزمية Levenshtein Distance
     */
    std::vector<std::string> findSimilarSymbols(
        const std::string& symbol,
        const std::string& availableSymbolsStr
    ) {
        std::vector<std::string> similar;
        
        // تقسيم الرموز المتاحة
        std::vector<std::string> available;
        std::istringstream stream(availableSymbolsStr);
        std::string item;
        while (std::getline(stream, item, ',')) {
            // إزالة المسافات
            item.erase(0, item.find_first_not_of(' '));
            item.erase(item.find_last_not_of(' ') + 1);
            if (!item.empty()) {
                available.push_back(item);
            }
        }
        
        // حساب المسافة لكل رمز
        for (const auto& candidate : available) {
            int distance = levenshteinDistance(symbol, candidate);
            // إذا كانت المسافة صغيرة (أقل من 3)، أضف للاقتراحات
            if (distance > 0 && distance <= 3) {
                similar.push_back(candidate);
            }
        }
        
        // ترتيب حسب التشابه
        std::sort(similar.begin(), similar.end(),
            [&symbol, this](const std::string& a, const std::string& b) {
                return levenshteinDistance(symbol, a) < levenshteinDistance(symbol, b);
            });
        
        // إرجاع أقرب 3 اقتراحات
        if (similar.size() > 3) {
            similar.resize(3);
        }
        
        return similar;
    }
    
    /**
     * 📏 حساب مسافة Levenshtein
     *
     * تقيس عدد التعديلات اللازمة لتحويل نص إلى آخر
     */
    int levenshteinDistance(const std::string& s1, const std::string& s2) {
        size_t len1 = s1.size();
        size_t len2 = s2.size();
        
        std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));
        
        for (size_t i = 0; i <= len1; i++) dp[i][0] = static_cast<int>(i);
        for (size_t j = 0; j <= len2; j++) dp[0][j] = static_cast<int>(j);
        
        for (size_t i = 1; i <= len1; i++) {
            for (size_t j = 1; j <= len2; j++) {
                int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
                dp[i][j] = std::min({
                    dp[i-1][j] + 1,      // حذف
                    dp[i][j-1] + 1,      // إضافة
                    dp[i-1][j-1] + cost  // استبدال
                });
            }
        }
        
        return dp[len1][len2];
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

// مولد عالمي
static std::unique_ptr<SuggestionGenerator> globalGenerator;

/**
 * 🔧 تهيئة النظام
 */
extern "C" void sad_suggestions_init() {
    if (!globalGenerator) {
        globalGenerator = std::make_unique<SuggestionGenerator>();
    }
}

/**
 * 💡 توليد اقتراحات لخطأ
 */
extern "C" const char* sad_generate_suggestions(
    int category,
    const char* errorCode,
    const char* context,
    const char* detailsJson
) {
    if (!globalGenerator) {
        sad_suggestions_init();
    }
    
    // تحليل التفاصيل (تبسيط)
    std::unordered_map<std::string, std::string> details;
    // في التطبيق الفعلي، نحلل JSON
    
    auto suggestions = globalGenerator->generateSuggestions(
        static_cast<ErrorCategory>(category),
        errorCode ? errorCode : "",
        context ? context : "",
        details
    );
    
    // تنسيق النتائج
    static std::string result;
    result.clear();
    
    for (const auto& suggestion : suggestions) {
        result += suggestion.format(true);
        result += "\n";
    }
    
    return result.c_str();
}

} // namespace errors
} // namespace sad
