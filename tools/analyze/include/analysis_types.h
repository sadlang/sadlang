#pragma once

/**
 * ═══════════════════════════════════════════════════════════════════════════
 * ملف: analysis_types.h
 * الوصف: الأنواع الأساسية المشتركة داخل المحلل المتقدم للغة ص
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * هذا الملف هو العقدة المركزية التي تتبادل عبرها بقية أجزاء المحلل
 * المعلومات النهائية الناتجة عن التحليل. تم عزله في ملف مستقل لسببين:
 *
 * 1. تقليل الترابط بين محرك التحليل وبين الزائر الذي يجتاز AST.
 * 2. السماح بإعادة استخدام نفس البنى لاحقاً من خادم اللغة LSP أو من
 *    أدوات أخرى مثل الفاحص التزايدي أو واجهة رسومية للتشخيصات.
 *
 * الفكرة العامة هنا أن المحلل لا يجب أن يكون مجرد طابعة أخطاء، بل طبقة
 * تحليل منهجية تنتج:
 *
 * - مشكلات دقيقة مرتبطة بمواضع واضحة داخل الملف
 * - إحصاءات عن عدد النطاقات والرموز والجمل التي تم تحليلها
 * - خيارات تشغيل تضبط مدى الصرامة والضوضاء في النتائج
 *
 * تم تصميم هذه البنى لتكون بسيطة في النسخة الأولى، ولكن قابلة للتوسع
 * لاحقاً بإضافة إصلاحات تلقائية، علاقات بين الرموز، وروابط مع LSP.
 */

#include <string>
#include <vector>
#include <cstddef>

#include "diagnostic.h"
#include "source_location.h"

namespace Sad::Analizer {

/**
 * @brief نوع منطقي للمشكلة المكتشفة داخل المحلل.
 *
 * استخدام نوع مستقل هنا أفضل من حصر كل شيء في رسالة نصية؛ لأن ذلك يسمح
 * لاحقاً ببناء تقارير وتصفية حسب الصنف وإجراء إحصاءات أكثر دقة.
 */
enum class IssueKind {
    Syntax,
    Semantic,
    Flow,
    Style,
    Performance,
    Safety
};

/**
 * @brief إعدادات تشغيل المحلل.
 *
 * هذه الخيارات تضبط سلوك النسخة الحالية من المحرك، وتم اختيارها لتغطي
 * الاستخدامين الأكثر شيوعاً:
 *
 * - تحليل سريع أثناء التطوير مع تحذيرات مفيدة.
 * - تحليل أكثر صرامة في CI أو قبل الدمج.
 */
struct AnalyzerOptions {
    bool strictMode = false;
    bool includeParserDiagnostics = true;
    bool enableShadowingWarnings = true;
    bool enableUnusedSymbolWarnings = true;
    bool enableConstantConditionWarnings = true;
    bool enableUnreachableCodeWarnings = true;
    bool verboseSummary = true;
    
    // قواعد التعقيد والأنماط السيئة
    bool enableComplexityWarnings = true;
    bool enableCodeSmellWarnings = true;
    bool enablePerformanceWarnings = true;
    bool enableMagicNumberWarnings = true;
    
    // حدود التعقيد
    std::size_t maxFunctionStatements = 30;     // أقصى عدد جمل في دالة
    std::size_t maxClassMethods = 10;           // أقصى عدد طرق في صنف
    std::size_t maxNestingDepth = 4;            // أقصى عمق تداخل
    std::size_t maxFunctionParameters = 6;      // أقصى عدد معاملات
    std::size_t maxExpressionComplexity = 5;    // أقصى تعقيد تعبير
};

/**
 * @brief تمثيل مشكلة واحدة ناتجة عن التحليل.
 *
 * نحتفظ بالرسائل بالعربية والإنجليزية من البداية حتى لا يفرض المحرك لغة
 * واحدة على المستهلك النهائي. الواجهة الحالية CLI ستطبع العربية افتراضياً،
 * لكن وجود الصياغة الإنجليزية الآن يسهل دمج الأداة لاحقاً مع أنظمة خارجية.
 */
struct AnalysisIssue {
    std::string ruleId;
    IssueKind kind = IssueKind::Semantic;
    Errors::DiagnosticSeverity severity = Errors::DiagnosticSeverity::WARNING;
    Errors::SourceLocation location;
    std::string messageArabic;
    std::string messageEnglish;
    std::string symbolName;

    bool isError() const {
        return severity == Errors::DiagnosticSeverity::ERROR;
    }

    bool isWarning() const {
        return severity == Errors::DiagnosticSeverity::WARNING;
    }
};

/**
 * @brief ملخص عددي لما حدث أثناء التحليل.
 */
struct AnalysisSummary {
    std::size_t totalStatements = 0;
    std::size_t totalExpressions = 0;
    std::size_t totalScopes = 0;
    std::size_t totalSymbols = 0;
    std::size_t totalErrors = 0;
    std::size_t totalWarnings = 0;
    std::size_t totalHints = 0;
    
    // إحصاءات التعقيد
    std::size_t maxNestingDepth = 0;
    std::size_t totalFunctions = 0;
    std::size_t totalClasses = 0;
    std::size_t totalMagicNumbers = 0;
};

/**
 * @brief النتيجة النهائية لتحليل ملف واحد.
 */
struct AnalysisResult {
    std::string filePath;
    bool parsedSuccessfully = false;
    AnalysisSummary summary;
    std::vector<AnalysisIssue> issues;

    bool hasErrors() const {
        return summary.totalErrors > 0;
    }

    bool isClean() const {
        return issues.empty();
    }
};

} // namespace Sad::Analizer
