// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file ownership_suggester.cpp
 * @brief تنفيذ مُقترِح تحويل الملكية / Ownership Suggester Implementation
 * 
 * هذا الملف يوفر نظاماً تعليمياً ذكياً يساعد المبرمجين على:
 * 1. فهم كيفية تحويل كود GC إلى نظام ملكية
 * 2. اكتشاف الأنماط غير الآمنة
 * 3. تعلم أفضل الممارسات في إدارة الذاكرة
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#include "memory/ownership_suggester.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <iomanip>

namespace Sad {
namespace Memory {

// =============================================================================
//                    تنفيذ داخلي
// =============================================================================

struct OwnershipSuggester::Impl {
    // هنا سيتم إضافة AST والتحليل الفعلي لاحقاً
    // حالياً نوفر الهيكل الأساسي
};

// =============================================================================
//                    البناء والهدم
// =============================================================================

OwnershipSuggester::OwnershipSuggester() 
    : options_(SuggesterOptions{}), impl_(std::make_unique<Impl>()) {
}

OwnershipSuggester::OwnershipSuggester(const SuggesterOptions& options)
    : options_(options), impl_(std::make_unique<Impl>()) {
}

OwnershipSuggester::~OwnershipSuggester() = default;

// =============================================================================
//                    التحليل الرئيسي
// =============================================================================

std::vector<OwnershipSuggestion> OwnershipSuggester::analyzeFile(const std::string& filePath) {
    // قراءة الملف
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return {};
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return analyzeSource(buffer.str(), filePath);
}

std::vector<OwnershipSuggestion> OwnershipSuggester::analyzeSource(
    const std::string& sourceCode,
    const std::string& fileName
) {
    std::vector<OwnershipSuggestion> suggestions;
    
    // تحليل بسيط للأنماط الشائعة
    // (في الإصدار الكامل سيتم استخدام AST)
    
    std::istringstream stream(sourceCode);
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(stream, line)) {
        lineNumber++;
        
        // ======================================================================
        // اكتشاف استخدام جديد (new) بدون ملكية واضحة
        // ======================================================================
        if (line.find("جديد ") != std::string::npos || line.find("جديد(") != std::string::npos) {
            // التحقق من عدم وجود مؤشر ذكي
            if (line.find("عم<") == std::string::npos && 
                line.find("عم_ذري<") == std::string::npos &&
                line.find("Rc<") == std::string::npos &&
                line.find("Arc<") == std::string::npos) {
                
                SourceLocation loc{fileName, lineNumber, 0, lineNumber, line.length()};
                
                auto suggestion = createSuggestion(
                    SuggestionType::UseRc,
                    SuggestionSeverity::Suggestion,
                    loc,
                    line,
                    "// استبدل بـ: عم<نوع>::أنشئ(...)"
                );
                
                suggestion.explanationArabic = 
                    "استخدام 'جديد' مباشرة قد يسبب تسرب ذاكرة إذا نسيت تحرير الكائن.\n"
                    "بدلاً من ذلك، استخدم عم<نوع> (عدّاد مرجعي) لضمان تحرير الذاكرة تلقائياً.\n\n"
                    "مثال:\n"
                    "  // قبل:\n"
                    "  قائمة* ق = جديد قائمة()\n\n"
                    "  // بعد:\n"
                    "  عم<قائمة> ق = عم<قائمة>::أنشئ()";
                    
                suggestion.explanationEnglish = 
                    "Using 'new' directly may cause memory leaks if you forget to free the object.\n"
                    "Instead, use Rc<Type> (reference counted) to ensure automatic memory release.\n\n"
                    "Example:\n"
                    "  // Before:\n"
                    "  List* l = new List()\n\n"
                    "  // After:\n"
                    "  Rc<List> l = Rc<List>::create()";
                
                suggestion.relatedConcepts = {"عدّاد_مرجعي", "RAII", "إدارة_الذاكرة_التلقائية"};
                suggestion.confidence = 0.85f;
                suggestion.isAutoFixable = false;  // يحتاج مراجعة
                
                suggestions.push_back(suggestion);
            }
        }
        
        // ======================================================================
        // اكتشاف مراجع قد تصبح معلقة
        // ======================================================================
        std::regex refReturnPattern(R"(ارجع\s*&)");
        if (std::regex_search(line, refReturnPattern)) {
            SourceLocation loc{fileName, lineNumber, 0, lineNumber, line.length()};
            
            auto suggestion = createSuggestion(
                SuggestionType::AddLifetime,
                SuggestionSeverity::Warning,
                loc,
                line,
                "// تحقق من عمر المرجع المُرجَع"
            );
            
            suggestion.explanationArabic = 
                "⚠️ إرجاع مرجع من دالة قد يكون خطيراً!\n\n"
                "إذا كان المرجع يشير إلى متغير محلي، سيصبح معلقاً (dangling) بعد انتهاء الدالة.\n\n"
                "الحالات الآمنة:\n"
                "  ✓ إرجاع مرجع لحقل في self\n"
                "  ✓ إرجاع مرجع لمعامل مُمرَّر بالمرجع\n"
                "  ✓ إرجاع مرجع لمتغير عام\n\n"
                "الحالات الخطرة:\n"
                "  ✗ إرجاع مرجع لمتغير محلي\n"
                "  ✗ إرجاع مرجع لكائن مُنشأ في الدالة";
                
            suggestion.explanationEnglish = 
                "⚠️ Returning a reference from a function can be dangerous!\n\n"
                "If the reference points to a local variable, it becomes dangling after function returns.\n\n"
                "Safe cases:\n"
                "  ✓ Return reference to self's field\n"
                "  ✓ Return reference to a by-ref parameter\n"
                "  ✓ Return reference to global variable\n\n"
                "Dangerous cases:\n"
                "  ✗ Return reference to local variable\n"
                "  ✗ Return reference to object created in function";
            
            suggestion.relatedConcepts = {"مراجع_معلقة", "عمر_المرجع", "Dangling_Reference"};
            suggestion.confidence = 0.7f;
            suggestion.isAutoFixable = false;
            
            suggestions.push_back(suggestion);
        }
        
        // ======================================================================
        // اكتشاف أنماط الدورات المحتملة
        // ======================================================================
        if (options_.detectCycles) {
            // نمط بسيط: كائن يحتوي على مرجع لنفس النوع
            std::regex selfRefPattern(R"((\w+)\s*:\s*عم<\1>)");
            std::smatch match;
            if (std::regex_search(line, match, selfRefPattern)) {
                SourceLocation loc{fileName, lineNumber, 0, lineNumber, line.length()};
                
                auto suggestion = createSuggestion(
                    SuggestionType::PotentialCycle,
                    SuggestionSeverity::Warning,
                    loc,
                    line,
                    "// قد تحتاج استخدام مرجع_ضعيف<> لكسر الدورة"
                );
                
                suggestion.explanationArabic = 
                    "🔄 تحذير: احتمال وجود دورة مراجع!\n\n"
                    "عندما يحتوي كائن على عم<> لنفس نوعه (أو أنواع تشير لبعضها)،\n"
                    "قد تتشكل دورة تمنع جامع القمامة من تحرير الكائنات.\n\n"
                    "الحل:\n"
                    "  • استخدم مرجع_ضعيف<> (Weak) للمراجع 'العكسية'\n"
                    "  • مثال: الأب يملك الأبناء بـ عم<>، الأبناء يشيرون للأب بـ مرجع_ضعيف<>\n\n"
                    "مثال الشجرة:\n"
                    "  صنف عقدة\n"
                    "    أبناء: مصفوفة<عم<عقدة>>      // ملكية قوية\n"
                    "    أب: مرجع_ضعيف<عقدة>         // مرجع ضعيف (لا يملك)\n"
                    "  نهاية";
                    
                suggestion.explanationEnglish = 
                    "🔄 Warning: Potential reference cycle detected!\n\n"
                    "When an object contains Rc<> to its own type (or types that reference each other),\n"
                    "a cycle may form that prevents the GC from freeing the objects.\n\n"
                    "Solution:\n"
                    "  • Use Weak<> for 'back' references\n"
                    "  • Example: Parent owns children with Rc<>, children point to parent with Weak<>\n\n"
                    "Tree example:\n"
                    "  class Node\n"
                    "    children: Array<Rc<Node>>     // Strong ownership\n"
                    "    parent: Weak<Node>            // Weak reference (no ownership)\n"
                    "  end";
                
                suggestion.relatedConcepts = {"دورة_مراجع", "مرجع_ضعيف", "Reference_Cycle"};
                suggestion.confidence = 0.6f;
                suggestion.isAutoFixable = false;
                
                suggestions.push_back(suggestion);
            }
        }
        
        // ======================================================================
        // اكتراح استخدام اختياري<> بدلاً من null
        // ======================================================================
        if (line.find("== لاشيء") != std::string::npos ||
            line.find("!= لاشيء") != std::string::npos ||
            line.find("== null") != std::string::npos ||
            line.find("!= null") != std::string::npos) {
            
            SourceLocation loc{fileName, lineNumber, 0, lineNumber, line.length()};
            
            auto suggestion = createSuggestion(
                SuggestionType::UseOption,
                SuggestionSeverity::Hint,
                loc,
                line,
                "// فكّر باستخدام اختياري<نوع> بدلاً من المقارنة مع null"
            );
            
            suggestion.explanationArabic = 
                "💡 استخدام اختياري<> أكثر أماناً من null!\n\n"
                "المقارنة مع null/لاشيء قد تُنسى في بعض الأماكن، مما يسبب أخطاء.\n"
                "نوع اختياري<> يجبرك على التعامل مع حالة 'لا قيمة' صراحةً.\n\n"
                "قبل:\n"
                "  مستخدم* م = ابحث_مستخدم(123)\n"
                "  إذا (م != لاشيء)\n"
                "    اطبع(م.اسم)\n"
                "  نهاية\n\n"
                "بعد:\n"
                "  اختياري<مستخدم> م = ابحث_مستخدم(123)\n"
                "  إذا (م.موجود())\n"
                "    اطبع(م.قيمة().اسم)\n"
                "  نهاية\n\n"
                "أو بشكل أفضل:\n"
                "  ابحث_مستخدم(123).تطابق(\n"
                "    بعض: دالة(م) -> اطبع(م.اسم)،\n"
                "    لاشيء: دالة() -> اطبع(\"غير موجود\")\n"
                "  )";
            
            suggestion.relatedConcepts = {"اختياري", "Option", "Null_Safety"};
            suggestion.confidence = 0.75f;
            suggestion.isAutoFixable = false;
            
            suggestions.push_back(suggestion);
        }
        
        // ======================================================================
        // اقتراح نقل بدلاً من نسخ للكائنات الكبيرة
        // ======================================================================
        std::regex copyPattern(R"((\w+)\s*=\s*(\w+)\s*$)");
        std::smatch copyMatch;
        if (std::regex_search(line, copyMatch, copyPattern)) {
            // هذا تحليل مبسط - في الواقع نحتاج معرفة نوع المتغير
            // حالياً نعرض كمعلومة تعليمية عامة
        }
    }
    
    // ترتيب بحسب الخطورة
    std::sort(suggestions.begin(), suggestions.end(),
        [](const OwnershipSuggestion& a, const OwnershipSuggestion& b) {
            return static_cast<int>(a.severity) > static_cast<int>(b.severity);
        }
    );
    
    // تطبيق حد الاقتراحات
    if (suggestions.size() > options_.maxSuggestionsPerFile) {
        suggestions.resize(options_.maxSuggestionsPerFile);
    }
    
    return suggestions;
}

OwnershipSuggestionReport OwnershipSuggester::analyzeProject(const std::string& projectPath) {
    OwnershipSuggestionReport report;
    report.projectName = projectPath;
    report.analyzedPath = projectPath;
    
    // TODO: تنفيذ تحليل المشروع الكامل
    // سيتم البحث عن ملفات .s و .sad وتحليلها
    
    return report;
}

// =============================================================================
//                    الفلترة والبحث
// =============================================================================

std::vector<OwnershipSuggestion> OwnershipSuggester::filterBySeverity(
    const std::vector<OwnershipSuggestion>& suggestions,
    SuggestionSeverity minSeverity
) {
    std::vector<OwnershipSuggestion> filtered;
    std::copy_if(suggestions.begin(), suggestions.end(),
        std::back_inserter(filtered),
        [minSeverity](const OwnershipSuggestion& s) {
            return static_cast<int>(s.severity) >= static_cast<int>(minSeverity);
        }
    );
    return filtered;
}

std::vector<OwnershipSuggestion> OwnershipSuggester::filterByType(
    const std::vector<OwnershipSuggestion>& suggestions,
    SuggestionType type
) {
    std::vector<OwnershipSuggestion> filtered;
    std::copy_if(suggestions.begin(), suggestions.end(),
        std::back_inserter(filtered),
        [type](const OwnershipSuggestion& s) {
            return s.type == type;
        }
    );
    return filtered;
}

std::vector<OwnershipSuggestion> OwnershipSuggester::getAutoFixable(
    const std::vector<OwnershipSuggestion>& suggestions
) {
    std::vector<OwnershipSuggestion> filtered;
    std::copy_if(suggestions.begin(), suggestions.end(),
        std::back_inserter(filtered),
        [](const OwnershipSuggestion& s) {
            return s.isAutoFixable;
        }
    );
    return filtered;
}

// =============================================================================
//                    التقارير
// =============================================================================

std::string OwnershipSuggester::generateTextReport(
    const std::vector<OwnershipSuggestion>& suggestions,
    bool arabic
) const {
    std::ostringstream report;
    
    if (arabic) {
        report << "\n";
        report << "╔═══════════════════════════════════════════════════════════════════════════════╗\n";
        report << "║                    تقرير اقتراحات تحويل الملكية                               ║\n";
        report << "╚═══════════════════════════════════════════════════════════════════════════════╝\n";
        report << "\n";
        report << "  إجمالي الاقتراحات: " << suggestions.size() << "\n";
        report << "\n";
        
        for (size_t i = 0; i < suggestions.size(); i++) {
            const auto& s = suggestions[i];
            
            report << "┌─────────────────────────────────────────────────────────────────────────────┐\n";
            report << "│ اقتراح #" << (i + 1) << " - " << severityToArabic(s.severity) << "\n";
            report << "├─────────────────────────────────────────────────────────────────────────────┤\n";
            report << "│ الموقع: " << s.location.toString() << "\n";
            report << "│ النوع: " << suggestionTypeToArabic(s.type) << "\n";
            report << "│ الثقة: " << std::fixed << std::setprecision(0) << (s.confidence * 100) << "%\n";
            report << "├─────────────────────────────────────────────────────────────────────────────┤\n";
            report << "│ العنوان: " << s.titleArabic << "\n";
            report << "│ الوصف: " << s.descriptionArabic << "\n";
            report << "├─────────────────────────────────────────────────────────────────────────────┤\n";
            report << "│ الكود الأصلي:\n";
            report << "│   " << s.originalCode << "\n";
            report << "│ الكود المقترح:\n";
            report << "│   " << s.suggestedCode << "\n";
            
            if (options_.teacherMode && !s.explanationArabic.empty()) {
                report << "├─────────────────────────────────────────────────────────────────────────────┤\n";
                report << "│ 📚 شرح تعليمي:\n";
                // تقسيم الشرح على أسطر
                std::istringstream explStream(s.explanationArabic);
                std::string explLine;
                while (std::getline(explStream, explLine)) {
                    report << "│   " << explLine << "\n";
                }
            }
            
            report << "└─────────────────────────────────────────────────────────────────────────────┘\n";
            report << "\n";
        }
    } else {
        // English version
        report << "\n";
        report << "╔═══════════════════════════════════════════════════════════════════════════════╗\n";
        report << "║                    Ownership Conversion Suggestions Report                    ║\n";
        report << "╚═══════════════════════════════════════════════════════════════════════════════╝\n";
        report << "\n";
        report << "  Total suggestions: " << suggestions.size() << "\n";
        report << "\n";
        
        for (size_t i = 0; i < suggestions.size(); i++) {
            const auto& s = suggestions[i];
            
            report << "┌─────────────────────────────────────────────────────────────────────────────┐\n";
            report << "│ Suggestion #" << (i + 1) << " - " << severityToEnglish(s.severity) << "\n";
            report << "├─────────────────────────────────────────────────────────────────────────────┤\n";
            report << "│ Location: " << s.location.toString() << "\n";
            report << "│ Type: " << suggestionTypeToEnglish(s.type) << "\n";
            report << "│ Confidence: " << std::fixed << std::setprecision(0) << (s.confidence * 100) << "%\n";
            report << "│ Title: " << s.titleEnglish << "\n";
            report << "│ Description: " << s.descriptionEnglish << "\n";
            report << "└─────────────────────────────────────────────────────────────────────────────┘\n";
            report << "\n";
        }
    }
    
    return report.str();
}

std::string OwnershipSuggester::generateJsonReport(const OwnershipSuggestionReport& report) const {
    std::ostringstream json;
    
    json << "{\n";
    json << "  \"projectName\": \"" << report.projectName << "\",\n";
    json << "  \"analyzedPath\": \"" << report.analyzedPath << "\",\n";
    json << "  \"filesAnalyzed\": " << report.filesAnalyzed << ",\n";
    json << "  \"linesAnalyzed\": " << report.linesAnalyzed << ",\n";
    json << "  \"summary\": {\n";
    json << "    \"total\": " << report.summary.totalSuggestions << ",\n";
    json << "    \"info\": " << report.summary.infoCount << ",\n";
    json << "    \"hints\": " << report.summary.hintCount << ",\n";
    json << "    \"suggestions\": " << report.summary.suggestionCount << ",\n";
    json << "    \"warnings\": " << report.summary.warningCount << ",\n";
    json << "    \"critical\": " << report.summary.criticalCount << ",\n";
    json << "    \"autoFixable\": " << report.summary.autoFixableCount << "\n";
    json << "  },\n";
    json << "  \"readyForProduction\": " << (report.readyForProduction ? "true" : "false") << ",\n";
    json << "  \"suggestions\": [\n";
    
    for (size_t i = 0; i < report.suggestions.size(); i++) {
        const auto& s = report.suggestions[i];
        json << "    {\n";
        json << "      \"id\": \"" << s.id << "\",\n";
        json << "      \"type\": \"" << suggestionTypeToEnglish(s.type) << "\",\n";
        json << "      \"severity\": \"" << severityToEnglish(s.severity) << "\",\n";
        json << "      \"location\": {\n";
        json << "        \"file\": \"" << s.location.filePath << "\",\n";
        json << "        \"line\": " << s.location.line << ",\n";
        json << "        \"column\": " << s.location.column << "\n";
        json << "      },\n";
        json << "      \"titleArabic\": \"" << s.titleArabic << "\",\n";
        json << "      \"titleEnglish\": \"" << s.titleEnglish << "\",\n";
        json << "      \"confidence\": " << s.confidence << ",\n";
        json << "      \"autoFixable\": " << (s.isAutoFixable ? "true" : "false") << "\n";
        json << "    }";
        if (i < report.suggestions.size() - 1) json << ",";
        json << "\n";
    }
    
    json << "  ]\n";
    json << "}\n";
    
    return json.str();
}

std::string OwnershipSuggester::generateHtmlReport(const OwnershipSuggestionReport& report) const {
    // TODO: تنفيذ تقرير HTML تفاعلي
    return "<html><body>TODO: HTML Report</body></html>";
}

// =============================================================================
//                    الإصلاح التلقائي
// =============================================================================

size_t OwnershipSuggester::applyAutoFixes(
    const std::string& filePath,
    const std::vector<OwnershipSuggestion>& suggestions
) {
    // TODO: تنفيذ الإصلاح التلقائي
    return 0;
}

std::string OwnershipSuggester::previewAutoFixes(
    const std::string& sourceCode,
    const std::vector<OwnershipSuggestion>& suggestions
) const {
    // TODO: تنفيذ معاينة الإصلاحات
    return sourceCode;
}

// =============================================================================
//                    الإعدادات
// =============================================================================

void OwnershipSuggester::setOptions(const SuggesterOptions& options) {
    options_ = options;
}

const SuggesterOptions& OwnershipSuggester::getOptions() const {
    return options_;
}

// =============================================================================
//                    إنشاء الاقتراحات
// =============================================================================

OwnershipSuggestion OwnershipSuggester::createSuggestion(
    SuggestionType type,
    SuggestionSeverity severity,
    const SourceLocation& location,
    const std::string& originalCode,
    const std::string& suggestedCode
) const {
    OwnershipSuggestion suggestion;
    
    // إنشاء معرف فريد
    static size_t counter = 0;
    suggestion.id = "SUGG_" + std::to_string(++counter);
    
    suggestion.type = type;
    suggestion.severity = severity;
    suggestion.location = location;
    suggestion.originalCode = originalCode;
    suggestion.suggestedCode = suggestedCode;
    
    // الحصول على العناوين والأوصاف
    auto titles = getTitleForType(type);
    suggestion.titleArabic = titles.first;
    suggestion.titleEnglish = titles.second;
    
    auto descriptions = getDescriptionForType(type);
    suggestion.descriptionArabic = descriptions.first;
    suggestion.descriptionEnglish = descriptions.second;
    
    return suggestion;
}

std::pair<std::string, std::string> OwnershipSuggester::getTitleForType(SuggestionType type) const {
    switch (type) {
        case SuggestionType::UseOwned:
            return {"استخدم قيمة مملوكة", "Use Owned Value"};
        case SuggestionType::UseBorrowed:
            return {"استخدم استعارة", "Use Borrowed Reference"};
        case SuggestionType::UseMove:
            return {"انقل بدلاً من النسخ", "Move Instead of Copy"};
        case SuggestionType::AddLifetime:
            return {"أضف تحديد عمر", "Add Lifetime Annotation"};
        case SuggestionType::UseRc:
            return {"استخدم عم<> (عدّاد مرجعي)", "Use Rc<> (Reference Counted)"};
        case SuggestionType::UseArc:
            return {"استخدم عم_ذري<>", "Use Arc<> (Atomic Ref Count)"};
        case SuggestionType::UseWeak:
            return {"استخدم مرجع_ضعيف<>", "Use Weak<>"};
        case SuggestionType::UseOption:
            return {"استخدم اختياري<>", "Use Option<>"};
        case SuggestionType::UseResult:
            return {"استخدم نتيجة<>", "Use Result<>"};
        case SuggestionType::UseGuard:
            return {"استخدم حارس RAII", "Use RAII Guard"};
        case SuggestionType::PotentialCycle:
            return {"دورة مراجع محتملة", "Potential Reference Cycle"};
        case SuggestionType::BreakCycleWithWeak:
            return {"اكسر الدورة بـ Weak", "Break Cycle with Weak"};
        case SuggestionType::AvoidAllocation:
            return {"تجنب التخصيص", "Avoid Allocation"};
        case SuggestionType::UseStackAllocation:
            return {"استخدم المكدس", "Use Stack Allocation"};
        case SuggestionType::PoolAllocation:
            return {"استخدم مجمع ذاكرة", "Use Memory Pool"};
    }
    return {"اقتراح", "Suggestion"};
}

std::pair<std::string, std::string> OwnershipSuggester::getDescriptionForType(SuggestionType type) const {
    switch (type) {
        case SuggestionType::UseRc:
            return {
                "استخدم عم<> للملكية المشتركة مع تحرير تلقائي",
                "Use Rc<> for shared ownership with automatic cleanup"
            };
        case SuggestionType::PotentialCycle:
            return {
                "قد تتشكل دورة مراجع تمنع تحرير الذاكرة",
                "A reference cycle may form preventing memory cleanup"
            };
        case SuggestionType::UseOption:
            return {
                "نوع اختياري<> أكثر أماناً من استخدام null",
                "Option<> type is safer than using null"
            };
        case SuggestionType::AddLifetime:
            return {
                "تحقق من أن المرجع المُرجَع لا يصبح معلقاً",
                "Ensure returned reference doesn't become dangling"
            };
        default:
            return {"", ""};
    }
}

std::pair<std::string, std::string> OwnershipSuggester::getExplanationForType(SuggestionType type) const {
    // يتم تعيين الشرح المفصل في مكان آخر بناءً على السياق
    return {"", ""};
}

// =============================================================================
//                    الدوال المساعدة
// =============================================================================

std::string severityToArabic(SuggestionSeverity severity) {
    switch (severity) {
        case SuggestionSeverity::Info:       return "معلومة";
        case SuggestionSeverity::Hint:       return "تلميح";
        case SuggestionSeverity::Suggestion: return "اقتراح";
        case SuggestionSeverity::Warning:    return "تحذير";
        case SuggestionSeverity::Critical:   return "حرج";
    }
    return "غير معروف";
}

std::string severityToEnglish(SuggestionSeverity severity) {
    switch (severity) {
        case SuggestionSeverity::Info:       return "Info";
        case SuggestionSeverity::Hint:       return "Hint";
        case SuggestionSeverity::Suggestion: return "Suggestion";
        case SuggestionSeverity::Warning:    return "Warning";
        case SuggestionSeverity::Critical:   return "Critical";
    }
    return "Unknown";
}

std::string suggestionTypeToArabic(SuggestionType type) {
    switch (type) {
        case SuggestionType::UseOwned:           return "قيمة_مملوكة";
        case SuggestionType::UseBorrowed:        return "استعارة";
        case SuggestionType::UseMove:            return "نقل";
        case SuggestionType::AddLifetime:        return "عمر_مرجع";
        case SuggestionType::UseRc:              return "عم";
        case SuggestionType::UseArc:             return "عم_ذري";
        case SuggestionType::UseWeak:            return "مرجع_ضعيف";
        case SuggestionType::UseOption:          return "اختياري";
        case SuggestionType::UseResult:          return "نتيجة";
        case SuggestionType::UseGuard:           return "حارس_RAII";
        case SuggestionType::PotentialCycle:     return "دورة_محتملة";
        case SuggestionType::BreakCycleWithWeak: return "كسر_دورة";
        case SuggestionType::AvoidAllocation:    return "تجنب_تخصيص";
        case SuggestionType::UseStackAllocation: return "تخصيص_مكدس";
        case SuggestionType::PoolAllocation:     return "مجمع_ذاكرة";
    }
    return "غير_معروف";
}

std::string suggestionTypeToEnglish(SuggestionType type) {
    switch (type) {
        case SuggestionType::UseOwned:           return "UseOwned";
        case SuggestionType::UseBorrowed:        return "UseBorrowed";
        case SuggestionType::UseMove:            return "UseMove";
        case SuggestionType::AddLifetime:        return "AddLifetime";
        case SuggestionType::UseRc:              return "UseRc";
        case SuggestionType::UseArc:             return "UseArc";
        case SuggestionType::UseWeak:            return "UseWeak";
        case SuggestionType::UseOption:          return "UseOption";
        case SuggestionType::UseResult:          return "UseResult";
        case SuggestionType::UseGuard:           return "UseGuard";
        case SuggestionType::PotentialCycle:     return "PotentialCycle";
        case SuggestionType::BreakCycleWithWeak: return "BreakCycleWithWeak";
        case SuggestionType::AvoidAllocation:    return "AvoidAllocation";
        case SuggestionType::UseStackAllocation: return "UseStackAllocation";
        case SuggestionType::PoolAllocation:     return "PoolAllocation";
    }
    return "Unknown";
}

std::string severityToAnsiColor(SuggestionSeverity severity) {
    switch (severity) {
        case SuggestionSeverity::Info:       return "\033[36m";  // Cyan
        case SuggestionSeverity::Hint:       return "\033[34m";  // Blue
        case SuggestionSeverity::Suggestion: return "\033[32m";  // Green
        case SuggestionSeverity::Warning:    return "\033[33m";  // Yellow
        case SuggestionSeverity::Critical:   return "\033[31m";  // Red
    }
    return "\033[0m";  // Reset
}

} // namespace Memory
} // namespace Sad
