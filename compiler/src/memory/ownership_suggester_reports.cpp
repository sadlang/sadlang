#ifdef _MSC_VER
#pragma warning(disable : 4819)
#endif
// (AR) تقارير وتصدير اقتراحات تحويل الملكية
// (EN) Ownership suggestion reports and export
#include "memory/ownership_suggester.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <iomanip>
#include <filesystem>

namespace Sad
{
    namespace Memory
    {
        // =============================================================================
        //                    التقارير
        // =============================================================================

        std::string OwnershipSuggester::generateTextReport(
            const std::vector<OwnershipSuggestion> &suggestions,
            bool arabic) const
        {
            std::ostringstream report;

            if (arabic)
            {
                report << "\n";
                report << "╔═══════════════════════════════════════════════════════════════════════════════╗\n";
                report << "║                    تقرير اقتراحات تحويل الملكية                               ║\n";
                report << "╚═══════════════════════════════════════════════════════════════════════════════╝\n";
                report << "\n";
                report << "  إجمالي الاقتراحات: " << suggestions.size() << "\n";
                report << "\n";

                for (size_t i = 0; i < suggestions.size(); i++)
                {
                    const auto &s = suggestions[i];

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

                    if (options_.teacherMode && !s.explanationArabic.empty())
                    {
                        report << "├─────────────────────────────────────────────────────────────────────────────┤\n";
                        report << "│ 📚 شرح تعليمي:\n";
                        // تقسيم الشرح على أسطر
                        std::istringstream explStream(s.explanationArabic);
                        std::string explLine;
                        while (std::getline(explStream, explLine))
                        {
                            report << "│   " << explLine << "\n";
                        }
                    }

                    report << "└─────────────────────────────────────────────────────────────────────────────┘\n";
                    report << "\n";
                }
            }
            else
            {
                // English version
                report << "\n";
                report << "╔═══════════════════════════════════════════════════════════════════════════════╗\n";
                report << "║                    Ownership Conversion Suggestions Report                    ║\n";
                report << "╚═══════════════════════════════════════════════════════════════════════════════╝\n";
                report << "\n";
                report << "  Total suggestions: " << suggestions.size() << "\n";
                report << "\n";

                for (size_t i = 0; i < suggestions.size(); i++)
                {
                    const auto &s = suggestions[i];

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

        std::string OwnershipSuggester::generateJsonReport(const OwnershipSuggestionReport &report) const
        {
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

            for (size_t i = 0; i < report.suggestions.size(); i++)
            {
                const auto &s = report.suggestions[i];
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
                if (i < report.suggestions.size() - 1)
                    json << ",";
                json << "\n";
            }

            json << "  ]\n";
            json << "}\n";

            return json.str();
        }

        std::string OwnershipSuggester::generateHtmlReport(const OwnershipSuggestionReport &report) const
        {
            // (AR) تقرير HTML تفاعلي — يعرض الاقتراحات مع تلوين الكود
            // (EN) Interactive HTML report — shows suggestions with code highlighting
            std::ostringstream html;

            html << "<!DOCTYPE html>\n<html dir='rtl' lang='ar'>\n<head>\n";
            html << "<meta charset='UTF-8'>\n";
            html << "<title>تقرير اقتراحات الملكية — " << report.projectName << "</title>\n";
            html << "<style>\n";
            html << "body { font-family: 'Segoe UI', Tahoma, sans-serif; background: #1e1e1e; color: #d4d4d4; padding: 20px; }\n";
            html << ".header { background: #0e639c; padding: 20px; border-radius: 8px; margin-bottom: 20px; }\n";
            html << ".header h1 { color: white; margin: 0; }\n";
            html << ".summary { display: flex; gap: 15px; margin-bottom: 20px; flex-wrap: wrap; }\n";
            html << ".stat { background: #2d2d2d; border-radius: 8px; padding: 15px; min-width: 120px; text-align: center; }\n";
            html << ".stat .number { font-size: 2em; font-weight: bold; color: #4ec9b0; }\n";
            html << ".stat .label { font-size: 0.9em; color: #808080; }\n";
            html << ".suggestion { background: #2d2d2d; border-radius: 8px; margin-bottom: 15px; overflow: hidden; }\n";
            html << ".suggestion-header { padding: 12px 15px; display: flex; justify-content: space-between; align-items: center; }\n";
            html << ".severity-critical { border-right: 4px solid #f44747; }\n";
            html << ".severity-warning { border-right: 4px solid #cca700; }\n";
            html << ".severity-suggestion { border-right: 4px solid #3794ff; }\n";
            html << ".severity-info { border-right: 4px solid #4ec9b0; }\n";
            html << ".code { background: #1e1e1e; padding: 10px 15px; font-family: 'Consolas', monospace; direction: ltr; text-align: left; }\n";
            html << ".code-old { color: #f44747; }\n";
            html << ".code-new { color: #4ec9b0; }\n";
            html << ".badge { padding: 2px 8px; border-radius: 4px; font-size: 0.8em; }\n";
            html << ".badge-fix { background: #4ec9b0; color: #1e1e1e; }\n";
            html << "</style>\n</head>\n<body>\n";

            html << "<div class='header'><h1>تقرير اقتراحات تحويل الملكية</h1>";
            html << "<p>المشروع: " << report.projectName << " | الملفات: " << report.filesAnalyzed;
            html << " | الأسطر: " << report.linesAnalyzed << "</p></div>\n";

            html << "<div class='summary'>\n";
            html << "<div class='stat'><div class='number'>" << report.summary.totalSuggestions << "</div><div class='label'>إجمالي</div></div>\n";
            html << "<div class='stat'><div class='number'>" << report.summary.criticalCount << "</div><div class='label'>حرجة</div></div>\n";
            html << "<div class='stat'><div class='number'>" << report.summary.warningCount << "</div><div class='label'>تحذيرات</div></div>\n";
            html << "<div class='stat'><div class='number'>" << report.summary.autoFixableCount << "</div><div class='label'>قابل للإصلاح</div></div>\n";
            html << "</div>\n";

            for (size_t i = 0; i < report.suggestions.size(); i++)
            {
                const auto &s = report.suggestions[i];
                std::string severityClass;
                if (s.severity == SuggestionSeverity::Critical)
                    severityClass = "severity-critical";
                else if (s.severity == SuggestionSeverity::Warning)
                    severityClass = "severity-warning";
                else if (s.severity == SuggestionSeverity::Suggestion)
                    severityClass = "severity-suggestion";
                else
                    severityClass = "severity-info";

                html << "<div class='suggestion " << severityClass << "'>\n";
                html << "<div class='suggestion-header'><span><strong>#" << (i + 1) << "</strong> " << s.titleArabic << "</span>";
                if (s.isAutoFixable)
                    html << "<span class='badge badge-fix'>إصلاح تلقائي</span>";
                html << "</div>\n";
                html << "<p style='padding: 0 15px;'>" << s.descriptionArabic << "</p>\n";
                html << "<div class='code'><div class='code-old'>- " << s.originalCode << "</div>";
                html << "<div class='code-new'>+ " << s.suggestedCode << "</div></div>\n";
                html << "</div>\n";
            }

            html << "</body>\n</html>\n";
            return html.str();
        }

        // =============================================================================
        //                    الإصلاح التلقائي
        // =============================================================================

        size_t OwnershipSuggester::applyAutoFixes(
            const std::string &filePath,
            const std::vector<OwnershipSuggestion> &suggestions)
        {
            // (AR) قراءة الملف، تطبيق الإصلاحات من الأسفل للأعلى (لتجنب تغيير أرقام الأسطر)
            // (EN) Read file, apply fixes bottom-to-top (to avoid line number shifts)
            std::ifstream infile(filePath);
            if (!infile.is_open())
                return 0;

            std::vector<std::string> lines;
            std::string line;
            while (std::getline(infile, line))
            {
                lines.push_back(line);
            }
            infile.close();

            // (AR) ترتيب الاقتراحات من آخر سطر لأول سطر
            // (EN) Sort suggestions from last line to first
            auto fixable = getAutoFixable(suggestions);
            std::sort(fixable.begin(), fixable.end(),
                      [](const OwnershipSuggestion &a, const OwnershipSuggestion &b)
                      {
                          return a.location.line > b.location.line;
                      });

            size_t appliedCount = 0;
            for (const auto &s : fixable)
            {
                int lineIdx = s.location.line - 1;
                if (lineIdx >= 0 && lineIdx < static_cast<int>(lines.size()))
                {
                    // (AR) استبدال الكود الأصلي بالمقترح
                    // (EN) Replace original code with suggested
                    size_t pos = lines[lineIdx].find(s.originalCode);
                    if (pos != std::string::npos)
                    {
                        lines[lineIdx].replace(pos, s.originalCode.length(), s.suggestedCode);
                        appliedCount++;
                    }
                }
            }

            // (AR) كتابة الملف المعدّل
            // (EN) Write modified file
            if (appliedCount > 0)
            {
                std::ofstream outfile(filePath);
                for (const auto &l : lines)
                {
                    outfile << l << "\n";
                }
            }

            return appliedCount;
        }

        std::string OwnershipSuggester::previewAutoFixes(
            const std::string &sourceCode,
            const std::vector<OwnershipSuggestion> &suggestions) const
        {
            // (AR) معاينة الإصلاحات بدون تعديل الملف
            // (EN) Preview fixes without modifying the file
            std::string result = sourceCode;

            auto fixable = getAutoFixable(suggestions);
            // (AR) تطبيق من آخر سطر لأول سطر
            // (EN) Apply from last line to first
            std::sort(fixable.begin(), fixable.end(),
                      [](const OwnershipSuggestion &a, const OwnershipSuggestion &b)
                      {
                          return a.location.line > b.location.line;
                      });

            for (const auto &s : fixable)
            {
                size_t pos = result.find(s.originalCode);
                if (pos != std::string::npos)
                {
                    result.replace(pos, s.originalCode.length(), s.suggestedCode);
                }
            }

            return result;
        }

        // =============================================================================
        //                    الإعدادات
        // =============================================================================

        void OwnershipSuggester::setOptions(const SuggesterOptions &options)
        {
            options_ = options;
        }

        const SuggesterOptions &OwnershipSuggester::getOptions() const
        {
            return options_;
        }

        // =============================================================================
        //                    إنشاء الاقتراحات
        // =============================================================================

        OwnershipSuggestion OwnershipSuggester::createSuggestion(
            SuggestionType type,
            SuggestionSeverity severity,
            const SourceLocation &location,
            const std::string &originalCode,
            const std::string &suggestedCode) const
        {
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

        std::pair<std::string, std::string> OwnershipSuggester::getTitleForType(SuggestionType type) const
        {
            switch (type)
            {
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

        std::pair<std::string, std::string> OwnershipSuggester::getDescriptionForType(SuggestionType type) const
        {
            switch (type)
            {
            case SuggestionType::UseRc:
                return {
                    "استخدم عم<> للملكية المشتركة مع تحرير تلقائي",
                    "Use Rc<> for shared ownership with automatic cleanup"};
            case SuggestionType::PotentialCycle:
                return {
                    "قد تتشكل دورة مراجع تمنع تحرير الذاكرة",
                    "A reference cycle may form preventing memory cleanup"};
            case SuggestionType::UseOption:
                return {
                    "نوع اختياري<> أكثر أماناً من استخدام null",
                    "Option<> type is safer than using null"};
            case SuggestionType::AddLifetime:
                return {
                    "تحقق من أن المرجع المُرجَع لا يصبح معلقاً",
                    "Ensure returned reference doesn't become dangling"};
            default:
                return {"", ""};
            }
        }

        std::pair<std::string, std::string> OwnershipSuggester::getExplanationForType(SuggestionType type) const
        {
            // يتم تعيين الشرح المفصل في مكان آخر بناءً على السياق
            return {"", ""};
        }

        // =============================================================================
        //                    الدوال المساعدة
        // =============================================================================

        std::string severityToArabic(SuggestionSeverity severity)
        {
            switch (severity)
            {
            case SuggestionSeverity::Info:
                return "معلومة";
            case SuggestionSeverity::Hint:
                return "تلميح";
            case SuggestionSeverity::Suggestion:
                return "اقتراح";
            case SuggestionSeverity::Warning:
                return "تحذير";
            case SuggestionSeverity::Critical:
                return "حرج";
            }
            return "غير معروف";
        }

        std::string severityToEnglish(SuggestionSeverity severity)
        {
            switch (severity)
            {
            case SuggestionSeverity::Info:
                return "Info";
            case SuggestionSeverity::Hint:
                return "Hint";
            case SuggestionSeverity::Suggestion:
                return "Suggestion";
            case SuggestionSeverity::Warning:
                return "Warning";
            case SuggestionSeverity::Critical:
                return "Critical";
            }
            return "Unknown";
        }

        std::string suggestionTypeToArabic(SuggestionType type)
        {
            switch (type)
            {
            case SuggestionType::UseOwned:
                return "قيمة_مملوكة";
            case SuggestionType::UseBorrowed:
                return "استعارة";
            case SuggestionType::UseMove:
                return "نقل";
            case SuggestionType::AddLifetime:
                return "عمر_مرجع";
            case SuggestionType::UseRc:
                return "عم";
            case SuggestionType::UseArc:
                return "عم_ذري";
            case SuggestionType::UseWeak:
                return "مرجع_ضعيف";
            case SuggestionType::UseOption:
                return "اختياري";
            case SuggestionType::UseResult:
                return "نتيجة";
            case SuggestionType::UseGuard:
                return "حارس_RAII";
            case SuggestionType::PotentialCycle:
                return "دورة_محتملة";
            case SuggestionType::BreakCycleWithWeak:
                return "كسر_دورة";
            case SuggestionType::AvoidAllocation:
                return "تجنب_تخصيص";
            case SuggestionType::UseStackAllocation:
                return "تخصيص_مكدس";
            case SuggestionType::PoolAllocation:
                return "مجمع_ذاكرة";
            }
            return "غير_معروف";
        }

        std::string suggestionTypeToEnglish(SuggestionType type)
        {
            switch (type)
            {
            case SuggestionType::UseOwned:
                return "UseOwned";
            case SuggestionType::UseBorrowed:
                return "UseBorrowed";
            case SuggestionType::UseMove:
                return "UseMove";
            case SuggestionType::AddLifetime:
                return "AddLifetime";
            case SuggestionType::UseRc:
                return "UseRc";
            case SuggestionType::UseArc:
                return "UseArc";
            case SuggestionType::UseWeak:
                return "UseWeak";
            case SuggestionType::UseOption:
                return "UseOption";
            case SuggestionType::UseResult:
                return "UseResult";
            case SuggestionType::UseGuard:
                return "UseGuard";
            case SuggestionType::PotentialCycle:
                return "PotentialCycle";
            case SuggestionType::BreakCycleWithWeak:
                return "BreakCycleWithWeak";
            case SuggestionType::AvoidAllocation:
                return "AvoidAllocation";
            case SuggestionType::UseStackAllocation:
                return "UseStackAllocation";
            case SuggestionType::PoolAllocation:
                return "PoolAllocation";
            }
            return "Unknown";
        }

        std::string severityToAnsiColor(SuggestionSeverity severity)
        {
            switch (severity)
            {
            case SuggestionSeverity::Info:
                return "\033[36m"; // Cyan
            case SuggestionSeverity::Hint:
                return "\033[34m"; // Blue
            case SuggestionSeverity::Suggestion:
                return "\033[32m"; // Green
            case SuggestionSeverity::Warning:
                return "\033[33m"; // Yellow
            case SuggestionSeverity::Critical:
                return "\033[31m"; // Red
            }
            return "\033[0m"; // Reset
        }

    } // namespace Memory
} // namespace Sad
