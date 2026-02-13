/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: fix_suggestions.cpp                                                  ║
 * ║   الوصف: اقتراحات الإصلاح للأخطاء الشائعة                                   ║
 * ║   المهمة: T166 - Phase 15 (US13)                                            ║
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
 * عندما يجد المترجم خطأً، لا يكتفي بإخبارك بالمشكلة!
 * بل يقترح عليك كيف تصلحها أيضاً.
 *
 * مثال:
 * ─────
 *    الكود: متغير رسالة = "مرحبا" + 5
 *
 *    ❌ الخطأ: لا يمكن جمع نص مع عدد
 *
 *    💡 الاقتراحات:
 *       1. حوّل العدد لنص: "مرحبا" + نص(5)
 *       2. أو استخدم التنسيق: صيغة("مرحبا {}", 5)
 *
 * 📋 أنواع الاقتراحات:
 * ─────────────────────────────────────────────────────────
 *
 *    🔧 QUICK_FIX: إصلاح سريع بنقرة واحدة
 *    📝 SUGGESTION: اقتراح للتحسين
 *    📚 LEARNING: شرح تعليمي
 *    ⚠️ WARNING_FIX: إصلاح تحذير
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <functional>
#include <algorithm>

namespace sad {
namespace errors {

// ═══════════════════════════════════════════════════════════════════════════════
//                         أنواع الاقتراحات | Suggestion Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📊 FixType - نوع الإصلاح
 */
enum class FixType {
    QUICK_FIX,      // إصلاح سريع - تطبيق مباشر
    SUGGESTION,     // اقتراح - يحتاج مراجعة
    REFACTOR,       // إعادة هيكلة - تغيير كبير
    LEARNING,       // تعليمي - للتعلم فقط
    WARNING_FIX     // إصلاح تحذير
};

/**
 * 📊 FixPriority - أولوية الإصلاح
 */
enum class FixPriority {
    CRITICAL,       // حرج - يجب تطبيقه
    HIGH,           // عالي - ينصح بتطبيقه
    MEDIUM,         // متوسط - اختياري
    LOW             // منخفض - للتحسين فقط
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        هيكل الاقتراح | Suggestion Structure
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📍 TextEdit - تعديل نصي
 */
struct TextEdit {
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    std::string newText;
    
    TextEdit()
        : startLine(0), startColumn(0), endLine(0), endColumn(0) {}
    
    TextEdit(size_t sl, size_t sc, size_t el, size_t ec, const std::string& text)
        : startLine(sl), startColumn(sc), endLine(el), endColumn(ec), newText(text) {}
};

/**
 * 💡 FixSuggestion - اقتراح إصلاح
 */
struct FixSuggestion {
    std::string title;          // عنوان قصير
    std::string titleEn;        // عنوان بالإنجليزية
    std::string description;    // شرح مفصل
    std::string descriptionEn;  // شرح بالإنجليزية
    FixType type;
    FixPriority priority;
    std::vector<TextEdit> edits;
    std::string previewCode;    // معاينة الكود بعد الإصلاح
    bool isPreferred;           // هل هو الاقتراح المفضل؟
    
    FixSuggestion()
        : type(FixType::SUGGESTION)
        , priority(FixPriority::MEDIUM)
        , isPreferred(false)
    {}
    
    /**
     * 📝 تنسيق الاقتراح
     */
    std::string format(bool useArabic = true) const {
        std::ostringstream oss;
        
        // الرمز حسب النوع
        std::string icon;
        switch (type) {
            case FixType::QUICK_FIX: icon = "🔧"; break;
            case FixType::SUGGESTION: icon = "💡"; break;
            case FixType::REFACTOR: icon = "🔄"; break;
            case FixType::LEARNING: icon = "📚"; break;
            case FixType::WARNING_FIX: icon = "⚠️"; break;
        }
        
        oss << icon << " " << (useArabic ? title : titleEn);
        if (isPreferred) {
            oss << " ⭐";
        }
        oss << "\n";
        
        if (!description.empty()) {
            oss << "   " << (useArabic ? description : descriptionEn) << "\n";
        }
        
        if (!previewCode.empty()) {
            oss << "\n   المعاينة:\n";
            oss << "   ─────────\n";
            std::istringstream stream(previewCode);
            std::string line;
            while (std::getline(stream, line)) {
                oss << "   " << line << "\n";
            }
        }
        
        return oss.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                   قاعدة بيانات الإصلاحات | Fix Database
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📚 CommonFixPatterns - أنماط الإصلاح الشائعة
 */
class CommonFixPatterns {
public:
    // ═══════════════════════════════════════════════════════════════════════
    //                    إصلاحات الأنواع | Type Fixes
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * 🔧 إصلاح: نص + عدد
     */
    static std::vector<FixSuggestion> fixStringPlusNumber(
        const std::string& stringExpr,
        const std::string& numberExpr,
        size_t line,
        size_t column
    ) {
        std::vector<FixSuggestion> fixes;
        
        // الإصلاح 1: تحويل العدد لنص
        {
            FixSuggestion fix;
            fix.title = "حوّل العدد إلى نص";
            fix.titleEn = "Convert number to string";
            fix.description = "استخدم دالة نص() لتحويل العدد";
            fix.descriptionEn = "Use نص() function to convert number";
            fix.type = FixType::QUICK_FIX;
            fix.priority = FixPriority::HIGH;
            fix.isPreferred = true;
            fix.previewCode = stringExpr + " + نص(" + numberExpr + ")";
            
            TextEdit edit;
            edit.startLine = line;
            edit.startColumn = column;
            edit.newText = stringExpr + " + نص(" + numberExpr + ")";
            fix.edits.push_back(edit);
            
            fixes.push_back(fix);
        }
        
        // الإصلاح 2: استخدام التنسيق
        {
            FixSuggestion fix;
            fix.title = "استخدم تنسيق النصوص";
            fix.titleEn = "Use string formatting";
            fix.description = "استخدم دالة صيغة() للتنسيق الأفضل";
            fix.descriptionEn = "Use صيغة() function for better formatting";
            fix.type = FixType::SUGGESTION;
            fix.priority = FixPriority::MEDIUM;
            fix.previewCode = "صيغة(\"{}\" + \"{}\", " + stringExpr + ", " + numberExpr + ")";
            
            fixes.push_back(fix);
        }
        
        return fixes;
    }
    
    /**
     * 🔧 إصلاح: قسمة على صفر محتملة
     */
    static std::vector<FixSuggestion> fixPotentialDivisionByZero(
        const std::string& divisor,
        size_t line,
        size_t column
    ) {
        std::vector<FixSuggestion> fixes;
        
        // الإصلاح: إضافة فحص
        {
            FixSuggestion fix;
            fix.title = "أضف فحصاً للصفر";
            fix.titleEn = "Add zero check";
            fix.description = "تحقق من القيمة قبل القسمة";
            fix.descriptionEn = "Check value before division";
            fix.type = FixType::QUICK_FIX;
            fix.priority = FixPriority::HIGH;
            fix.isPreferred = true;
            fix.previewCode = 
                "إذا " + divisor + " != 0:\n"
                "    نتيجة = البسط / " + divisor + "\n"
                "وإلا:\n"
                "    // تعامل مع القسمة على صفر";
            
            fixes.push_back(fix);
        }
        
        return fixes;
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    //                إصلاحات الرموز غير المعرّفة | Undefined Symbol Fixes
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * 🔧 إصلاح: متغير غير معرّف
     */
    static std::vector<FixSuggestion> fixUndefinedVariable(
        const std::string& varName,
        const std::vector<std::string>& similarNames,
        size_t line,
        size_t column
    ) {
        std::vector<FixSuggestion> fixes;
        
        // الإصلاح 1: تصحيح إملائي
        for (const auto& similar : similarNames) {
            FixSuggestion fix;
            fix.title = "هل قصدت: " + similar + "؟";
            fix.titleEn = "Did you mean: " + similar + "?";
            fix.description = "تصحيح الاسم إلى " + similar;
            fix.descriptionEn = "Correct name to " + similar;
            fix.type = FixType::QUICK_FIX;
            fix.priority = FixPriority::HIGH;
            fix.isPreferred = true;
            
            TextEdit edit;
            edit.startLine = line;
            edit.startColumn = column;
            edit.endColumn = column + varName.length();
            edit.newText = similar;
            fix.edits.push_back(edit);
            
            fixes.push_back(fix);
            break;  // أول اقتراح فقط مفضل
        }
        
        // الإصلاح 2: تعريف المتغير
        {
            FixSuggestion fix;
            fix.title = "عرّف المتغير";
            fix.titleEn = "Define the variable";
            fix.description = "أضف تعريفاً للمتغير قبل استخدامه";
            fix.descriptionEn = "Add variable definition before use";
            fix.type = FixType::SUGGESTION;
            fix.priority = FixPriority::MEDIUM;
            fix.previewCode = "متغير " + varName + " = قيمة_ابتدائية";
            
            fixes.push_back(fix);
        }
        
        return fixes;
    }
    
    /**
     * 🔧 إصلاح: دالة غير معرّفة
     */
    static std::vector<FixSuggestion> fixUndefinedFunction(
        const std::string& funcName,
        const std::vector<std::string>& similarNames,
        const std::vector<std::string>& importSuggestions
    ) {
        std::vector<FixSuggestion> fixes;
        
        // الإصلاح 1: تصحيح إملائي
        for (const auto& similar : similarNames) {
            FixSuggestion fix;
            fix.title = "هل قصدت: " + similar + "()؟";
            fix.titleEn = "Did you mean: " + similar + "()?";
            fix.type = FixType::QUICK_FIX;
            fix.priority = FixPriority::HIGH;
            fix.isPreferred = true;
            fixes.push_back(fix);
            break;
        }
        
        // الإصلاح 2: استيراد وحدة
        for (const auto& import : importSuggestions) {
            FixSuggestion fix;
            fix.title = "استورد: " + import;
            fix.titleEn = "Import: " + import;
            fix.description = "الدالة موجودة في وحدة " + import;
            fix.descriptionEn = "Function exists in module " + import;
            fix.type = FixType::QUICK_FIX;
            fix.priority = FixPriority::HIGH;
            fix.previewCode = "استورد \"" + import + "\"";
            fixes.push_back(fix);
        }
        
        // الإصلاح 3: تعريف الدالة
        {
            FixSuggestion fix;
            fix.title = "عرّف الدالة";
            fix.titleEn = "Define the function";
            fix.type = FixType::SUGGESTION;
            fix.priority = FixPriority::LOW;
            fix.previewCode = 
                "دالة " + funcName + "(معاملات):\n"
                "    // جسم الدالة\n"
                "    أرجع نتيجة";
            fixes.push_back(fix);
        }
        
        return fixes;
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    //                إصلاحات نحوية | Syntax Fixes
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * 🔧 إصلاح: قوس مفقود
     */
    static std::vector<FixSuggestion> fixMissingBracket(
        char expectedBracket,
        size_t line,
        size_t column
    ) {
        std::vector<FixSuggestion> fixes;
        
        std::string bracketName;
        switch (expectedBracket) {
            case ')': bracketName = "قوس إغلاق )"; break;
            case ']': bracketName = "قوس مربع ]"; break;
            case '}': bracketName = "قوس معقوف }"; break;
            default: bracketName = "قوس"; break;
        }
        
        FixSuggestion fix;
        fix.title = "أضف " + bracketName;
        fix.titleEn = "Add closing bracket";
        fix.description = "يبدو أنك نسيت إغلاق القوس";
        fix.descriptionEn = "You seem to have forgotten to close the bracket";
        fix.type = FixType::QUICK_FIX;
        fix.priority = FixPriority::CRITICAL;
        fix.isPreferred = true;
        
        TextEdit edit;
        edit.startLine = line;
        edit.startColumn = column;
        edit.newText = std::string(1, expectedBracket);
        fix.edits.push_back(edit);
        
        fixes.push_back(fix);
        
        return fixes;
    }
    
    /**
     * 🔧 إصلاح: فاصلة منقوطة زائدة
     */
    static std::vector<FixSuggestion> fixExtraSemicolon(
        size_t line,
        size_t column
    ) {
        std::vector<FixSuggestion> fixes;
        
        FixSuggestion fix;
        fix.title = "أزل الفاصلة المنقوطة";
        fix.titleEn = "Remove semicolon";
        fix.description = "لغة سعد لا تحتاج فاصلة منقوطة في نهاية الأسطر";
        fix.descriptionEn = "Sad language doesn't need semicolons at line ends";
        fix.type = FixType::QUICK_FIX;
        fix.priority = FixPriority::HIGH;
        fix.isPreferred = true;
        
        TextEdit edit;
        edit.startLine = line;
        edit.startColumn = column;
        edit.endColumn = column + 1;
        edit.newText = "";
        fix.edits.push_back(edit);
        
        fixes.push_back(fix);
        
        // إضافة شرح تعليمي
        {
            FixSuggestion learning;
            learning.title = "لماذا لا نحتاج ؛؟";
            learning.titleEn = "Why no semicolons?";
            learning.description = 
                "في لغة سعد، نهاية السطر تكفي لإنهاء الجملة.\n"
                "هذا يجعل الكود أنظف وأسهل للقراءة.";
            learning.descriptionEn = 
                "In Sad, line endings are sufficient to end statements.\n"
                "This makes code cleaner and easier to read.";
            learning.type = FixType::LEARNING;
            learning.priority = FixPriority::LOW;
            fixes.push_back(learning);
        }
        
        return fixes;
    }
    
    /**
     * 🔧 إصلاح: نقطتان مفقودة
     */
    static std::vector<FixSuggestion> fixMissingColon(
        const std::string& keyword,
        size_t line,
        size_t column
    ) {
        std::vector<FixSuggestion> fixes;
        
        FixSuggestion fix;
        fix.title = "أضف نقطتين (:)";
        fix.titleEn = "Add colon (:)";
        fix.description = "بعد " + keyword + " يجب وضع نقطتين";
        fix.descriptionEn = "A colon is required after " + keyword;
        fix.type = FixType::QUICK_FIX;
        fix.priority = FixPriority::CRITICAL;
        fix.isPreferred = true;
        fix.previewCode = keyword + " شرط:";
        
        TextEdit edit;
        edit.startLine = line;
        edit.startColumn = column;
        edit.newText = ":";
        fix.edits.push_back(edit);
        
        fixes.push_back(fix);
        
        return fixes;
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    //                إصلاحات التنسيق | Style Fixes
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * 🔧 إصلاح: تسمية غير معيارية
     */
    static std::vector<FixSuggestion> fixNamingConvention(
        const std::string& currentName,
        const std::string& suggestedName,
        const std::string& reason
    ) {
        std::vector<FixSuggestion> fixes;
        
        FixSuggestion fix;
        fix.title = "غيّر الاسم إلى: " + suggestedName;
        fix.titleEn = "Rename to: " + suggestedName;
        fix.description = reason;
        fix.type = FixType::SUGGESTION;
        fix.priority = FixPriority::LOW;
        
        fixes.push_back(fix);
        
        return fixes;
    }
    
    /**
     * 🔧 إصلاح: متغير غير مستخدم
     */
    static std::vector<FixSuggestion> fixUnusedVariable(
        const std::string& varName,
        size_t line
    ) {
        std::vector<FixSuggestion> fixes;
        
        // الإصلاح 1: إزالة
        {
            FixSuggestion fix;
            fix.title = "أزل المتغير";
            fix.titleEn = "Remove variable";
            fix.description = "المتغير غير مستخدم ويمكن حذفه";
            fix.descriptionEn = "Variable is unused and can be removed";
            fix.type = FixType::QUICK_FIX;
            fix.priority = FixPriority::MEDIUM;
            
            TextEdit edit;
            edit.startLine = line;
            edit.startColumn = 1;
            edit.endLine = line + 1;
            edit.endColumn = 1;
            edit.newText = "";
            fix.edits.push_back(edit);
            
            fixes.push_back(fix);
        }
        
        // الإصلاح 2: إضافة _ للتجاهل
        {
            FixSuggestion fix;
            fix.title = "أضف _ لتجاهل التحذير";
            fix.titleEn = "Add _ to ignore warning";
            fix.description = "سمّ المتغير _" + varName + " للإشارة أنه متعمد";
            fix.descriptionEn = "Name it _" + varName + " to indicate intentional";
            fix.type = FixType::SUGGESTION;
            fix.priority = FixPriority::LOW;
            
            fixes.push_back(fix);
        }
        
        return fixes;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    مدير الاقتراحات | Suggestion Manager
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📦 FixSuggestionManager - مدير اقتراحات الإصلاح
 */
class FixSuggestionManager {
public:
    /**
     * 🔍 توليد اقتراحات لخطأ
     */
    std::vector<FixSuggestion> getSuggestionsForError(
        const std::string& errorCode,
        const std::unordered_map<std::string, std::string>& context
    ) {
        std::vector<FixSuggestion> suggestions;
        
        // تحديد نوع الخطأ من الرمز
        if (errorCode.starts_with("T")) {
            // أخطاء الأنواع
            suggestions = getTypeSuggestions(errorCode, context);
        }
        else if (errorCode.starts_with("S")) {
            // أخطاء دلالية
            suggestions = getSemanticSuggestions(errorCode, context);
        }
        else if (errorCode.starts_with("P")) {
            // أخطاء نحوية
            suggestions = getSyntaxSuggestions(errorCode, context);
        }
        
        // ترتيب حسب الأولوية
        std::sort(suggestions.begin(), suggestions.end(),
            [](const FixSuggestion& a, const FixSuggestion& b) {
                if (a.isPreferred != b.isPreferred) return a.isPreferred;
                return static_cast<int>(a.priority) < static_cast<int>(b.priority);
            });
        
        return suggestions;
    }
    
    /**
     * 📝 تنسيق الاقتراحات
     */
    std::string formatSuggestions(
        const std::vector<FixSuggestion>& suggestions,
        bool useArabic = true
    ) {
        if (suggestions.empty()) {
            return "";
        }
        
        std::ostringstream oss;
        
        oss << "\n💡 اقتراحات الإصلاح | Fix Suggestions:\n";
        oss << "   ─────────────────────────────────────────────────────────\n";
        
        for (size_t i = 0; i < suggestions.size(); i++) {
            oss << "\n   " << (i + 1) << ". " << suggestions[i].format(useArabic);
        }
        
        return oss.str();
    }
    
private:
    std::vector<FixSuggestion> getTypeSuggestions(
        const std::string& errorCode,
        const std::unordered_map<std::string, std::string>& context
    ) {
        std::vector<FixSuggestion> suggestions;
        
        // T001: نص + عدد
        if (errorCode == "T001") {
            std::string strExpr = context.count("string_expr") ? context.at("string_expr") : "نص";
            std::string numExpr = context.count("number_expr") ? context.at("number_expr") : "عدد";
            size_t line = context.count("line") ? std::stoul(context.at("line")) : 1;
            size_t col = context.count("column") ? std::stoul(context.at("column")) : 1;
            
            suggestions = CommonFixPatterns::fixStringPlusNumber(strExpr, numExpr, line, col);
        }
        
        return suggestions;
    }
    
    std::vector<FixSuggestion> getSemanticSuggestions(
        const std::string& errorCode,
        const std::unordered_map<std::string, std::string>& context
    ) {
        std::vector<FixSuggestion> suggestions;
        
        // S001: متغير غير معرّف
        if (errorCode == "S001") {
            std::string varName = context.count("variable") ? context.at("variable") : "";
            std::vector<std::string> similar;
            if (context.count("similar")) {
                // تقسيم الرموز المشابهة
                std::string similarStr = context.at("similar");
                // ... تقسيم
            }
            size_t line = context.count("line") ? std::stoul(context.at("line")) : 1;
            size_t col = context.count("column") ? std::stoul(context.at("column")) : 1;
            
            suggestions = CommonFixPatterns::fixUndefinedVariable(varName, similar, line, col);
        }
        
        return suggestions;
    }
    
    std::vector<FixSuggestion> getSyntaxSuggestions(
        const std::string& errorCode,
        const std::unordered_map<std::string, std::string>& context
    ) {
        std::vector<FixSuggestion> suggestions;
        
        // P001: قوس مفقود
        if (errorCode == "P001") {
            char bracket = context.count("bracket") ? context.at("bracket")[0] : ')';
            size_t line = context.count("line") ? std::stoul(context.at("line")) : 1;
            size_t col = context.count("column") ? std::stoul(context.at("column")) : 1;
            
            suggestions = CommonFixPatterns::fixMissingBracket(bracket, line, col);
        }
        // P002: فاصلة منقوطة زائدة
        else if (errorCode == "P002") {
            size_t line = context.count("line") ? std::stoul(context.at("line")) : 1;
            size_t col = context.count("column") ? std::stoul(context.at("column")) : 1;
            
            suggestions = CommonFixPatterns::fixExtraSemicolon(line, col);
        }
        
        return suggestions;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

static std::unique_ptr<FixSuggestionManager> globalManager;

extern "C" {
    void sad_fix_suggestions_init() {
        if (!globalManager) {
            globalManager = std::make_unique<FixSuggestionManager>();
        }
    }
    
    const char* sad_get_fix_suggestions(
        const char* errorCode,
        const char* contextJson
    ) {
        if (!globalManager) {
            sad_fix_suggestions_init();
        }
        
        // تحليل السياق (تبسيط)
        std::unordered_map<std::string, std::string> context;
        
        auto suggestions = globalManager->getSuggestionsForError(
            errorCode ? errorCode : "",
            context
        );
        
        static std::string result;
        result = globalManager->formatSuggestions(suggestions, true);
        return result.c_str();
    }
}

} // namespace errors
} // namespace sad
