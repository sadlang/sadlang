#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "analyzer_engine.h"
#include "utf8_utils.h" // (AR) دعم أسماء الملفات العربية / (EN) Arabic filename support

#ifdef _WIN32
#include <windows.h>
#endif

namespace
{

    using Sad::Analizer::AnalysisIssue;
    using Sad::Analizer::AnalysisResult;
    using Sad::Analizer::AnalyzerEngine;
    using Sad::Analizer::AnalyzerOptions;
    using Sad::Errors::DiagnosticSeverity;

    std::string severityToArabic(DiagnosticSeverity severity)
    {
        switch (severity)
        {
        case DiagnosticSeverity::ERROR:
            return "خطأ";
        case DiagnosticSeverity::WARNING:
            return "تحذير";
        case DiagnosticSeverity::INFO:
            return "معلومة";
        case DiagnosticSeverity::NOTE:
            return "ملاحظة";
        case DiagnosticSeverity::HINT:
            return "تلميح";
        }

        return "تشخيص";
    }

    void printUsage()
    {
        std::cout
            << "الاستخدام:\n"
            << "  sad-analizer <ملف1.ص> [ملف2.ص ...] [--strict] [--quiet-summary] [--json]\n\n"
            << "الخيارات:\n"
            << "  --strict         تفعيل حدود أكثر صرامة لقواعد التعقيد والأنماط\n"
            << "  --quiet-summary  إخفاء الملخص العددي في نهاية كل ملف\n"
            << "  --json           إخراج النتيجة بصيغة JSON\n";
    }

    void printIssue(const AnalysisIssue &issue)
    {
        std::cout
            << "[" << severityToArabic(issue.severity) << "] "
            << issue.ruleId;

        if (issue.location.isValid())
        {
            std::cout << " @ " << issue.location.toString();
        }

        std::cout << "\n"
                  << "  " << issue.messageArabic << "\n";
    }

    void printSummary(const AnalysisResult &result)
    {
        std::cout
            << "الملخص:\n"
            << "  الجمل: " << result.summary.totalStatements << "\n"
            << "  التعابير: " << result.summary.totalExpressions << "\n"
            << "  النطاقات: " << result.summary.totalScopes << "\n"
            << "  الرموز: " << result.summary.totalSymbols << "\n"
            << "  الدوال: " << result.summary.totalFunctions << "\n"
            << "  الأصناف: " << result.summary.totalClasses << "\n"
            << "  أقصى عمق تداخل: " << result.summary.maxNestingDepth << "\n"
            << "  الأرقام السحرية: " << result.summary.totalMagicNumbers << "\n"
            << "  الأخطاء: " << result.summary.totalErrors << "\n"
            << "  التحذيرات: " << result.summary.totalWarnings << "\n"
            << "  التلميحات: " << result.summary.totalHints << "\n";
    }

    std::string jsonEscape(const std::string &input)
    {
        std::ostringstream out;
        for (const char ch : input)
        {
            switch (ch)
            {
            case '\\':
                out << "\\\\";
                break;
            case '"':
                out << "\\\"";
                break;
            case '\n':
                out << "\\n";
                break;
            case '\r':
                out << "\\r";
                break;
            case '\t':
                out << "\\t";
                break;
            default:
                out << ch;
                break;
            }
        }
        return out.str();
    }

    void printJsonResult(const AnalysisResult &result)
    {
        std::cout << "{";
        std::cout << "\"file\":\"" << jsonEscape(result.filePath) << "\",";
        std::cout << "\"parsedSuccessfully\":" << (result.parsedSuccessfully ? "true" : "false") << ",";

        std::cout << "\"summary\":{";
        std::cout << "\"totalStatements\":" << result.summary.totalStatements << ",";
        std::cout << "\"totalExpressions\":" << result.summary.totalExpressions << ",";
        std::cout << "\"totalScopes\":" << result.summary.totalScopes << ",";
        std::cout << "\"totalSymbols\":" << result.summary.totalSymbols << ",";
        std::cout << "\"totalFunctions\":" << result.summary.totalFunctions << ",";
        std::cout << "\"totalClasses\":" << result.summary.totalClasses << ",";
        std::cout << "\"maxNestingDepth\":" << result.summary.maxNestingDepth << ",";
        std::cout << "\"totalMagicNumbers\":" << result.summary.totalMagicNumbers << ",";
        std::cout << "\"totalErrors\":" << result.summary.totalErrors << ",";
        std::cout << "\"totalWarnings\":" << result.summary.totalWarnings << ",";
        std::cout << "\"totalHints\":" << result.summary.totalHints;
        std::cout << "},";

        std::cout << "\"issues\":[";
        for (std::size_t i = 0; i < result.issues.size(); ++i)
        {
            const auto &issue = result.issues[i];
            if (i > 0)
            {
                std::cout << ",";
            }
            std::cout << "{";
            std::cout << "\"ruleId\":\"" << jsonEscape(issue.ruleId) << "\",";
            std::cout << "\"severity\":\"" << jsonEscape(severityToArabic(issue.severity)) << "\",";
            std::cout << "\"location\":\"" << jsonEscape(issue.location.toString()) << "\",";
            std::cout << "\"messageArabic\":\"" << jsonEscape(issue.messageArabic) << "\"";
            std::cout << "}";
        }
        std::cout << "]}";
    }

} // namespace

int main(int argc, char *argv[])
{
    // (AR) إعداد UTF-8 لدعم أسماء الملفات العربية
    // (EN) Setup UTF-8 for Arabic filename support
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF-8");
    auto utf8_args = sad::utf8::get_utf8_args();
    std::vector<char *> new_argv;
    for (auto &s : utf8_args)
        new_argv.push_back(s.data());
    argc = static_cast<int>(new_argv.size());
    argv = new_argv.data();
#endif

    if (argc < 2)
    {
        printUsage();
        return 2;
    }

    AnalyzerOptions options;
    std::vector<std::string> files;
    bool jsonOutput = false;

    for (int index = 1; index < argc; ++index)
    {
        const std::string argument = argv[index];
        if (argument == "--strict")
        {
            options.strictMode = true;
            options.maxFunctionStatements = 20;
            options.maxClassMethods = 8;
            options.maxNestingDepth = 3;
            options.maxExpressionComplexity = 4;
            continue;
        }
        if (argument == "--quiet-summary")
        {
            options.verboseSummary = false;
            continue;
        }
        if (argument == "--json")
        {
            jsonOutput = true;
            continue;
        }

        files.push_back(argument);
    }

    if (files.empty())
    {
        printUsage();
        return 2;
    }

    AnalyzerEngine engine;
    bool hasErrors = false;

    for (std::size_t i = 0; i < files.size(); ++i)
    {
        const std::string &file = files[i];
        const AnalysisResult result = engine.analyzeFile(file, options);

        if (jsonOutput)
        {
            if (i > 0)
            {
                std::cout << "\n";
            }
            printJsonResult(result);
            hasErrors = hasErrors || result.hasErrors();
            continue;
        }

        std::cout << "══════════════════════════════════════════════════\n";
        std::cout << "تحليل الملف: " << file << "\n";
        std::cout << "══════════════════════════════════════════════════\n";

        if (result.issues.empty())
        {
            std::cout << "لا توجد مشكلات مكتشفة في هذا الملف وفق القواعد الحالية للمحلل.\n";
        }
        else
        {
            for (const auto &issue : result.issues)
            {
                printIssue(issue);
            }
        }

        if (options.verboseSummary)
        {
            printSummary(result);
        }

        hasErrors = hasErrors || result.hasErrors();
        std::cout << "\n";
    }

    return hasErrors ? 1 : 0;
}
