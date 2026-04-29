#include "analyzer_engine.h"

#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>

#include "ast_analysis_visitor.h"
#include "error_manager.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "utf8_utils.h" // (AR) دعم أسماء الملفات العربية / (EN) Arabic filename support

namespace Sad::Analizer
{

    namespace
    {

        std::string readWholeFile(const std::string &filePath)
        {
            // (AR) استخدام utf8 open لدعم أسماء الملفات العربية على Windows
            // (EN) Use utf8 open for Arabic filename support on Windows
            auto input = sad::utf8::open_ifstream(filePath);
            if (!input)
            {
                throw std::runtime_error("تعذر فتح الملف: " + filePath);
            }

            std::ostringstream buffer;
            buffer << input.rdbuf();
            return buffer.str();
        }

    } // namespace

    AnalysisResult AnalyzerEngine::analyzeFile(const std::string &filePath,
                                               const AnalyzerOptions &options) const
    {
        return analyzeSource(readWholeFile(filePath), filePath, options);
    }

    AnalysisResult AnalyzerEngine::analyzeSource(const std::string &source,
                                                 const std::string &filePath,
                                                 const AnalyzerOptions &options) const
    {
        AnalysisResult result;
        result.filePath = filePath;

        try
        {
            Errors::ErrorManager::getInstance().clear();
            Errors::ErrorManager::getInstance().setSourceCode(source, filePath);

            Lexer::LexerCore lexer(source);
            Parser::ParserCore parser(lexer);

            AST::StmtList program = parser.parseProgram();

            if (parser.hasErrors())
            {
                result.parsedSuccessfully = false;

                if (options.includeParserDiagnostics)
                {
                    for (const std::string &errorMessage : parser.getErrors())
                    {
                        AnalysisIssue issue;
                        issue.ruleId = "SAD-SYNTAX-001";
                        issue.kind = IssueKind::Syntax;
                        issue.severity = Errors::DiagnosticSeverity::ERROR;
                        issue.location = Errors::SourceLocation(filePath, 1, 1, 0, 1);
                        issue.messageArabic = errorMessage;
                        issue.messageEnglish = errorMessage;
                        result.issues.push_back(std::move(issue));
                        ++result.summary.totalErrors;
                    }
                }

                if (result.issues.empty())
                {
                    AnalysisIssue issue;
                    issue.ruleId = "SAD-SYNTAX-000";
                    issue.kind = IssueKind::Syntax;
                    issue.severity = Errors::DiagnosticSeverity::ERROR;
                    issue.location = Errors::SourceLocation(filePath, 1, 1, 0, 1);
                    issue.messageArabic = "فشل التحليل النحوي لكن المحلل الأساسي لم يرجع تشخيصاً نصياً واضحاً. يلزم تتبع أعمق داخل ParserCore.";
                    issue.messageEnglish = "Parsing failed but no textual diagnostic was returned by ParserCore.";
                    result.issues.push_back(std::move(issue));
                    ++result.summary.totalErrors;
                }

                return result;
            }

            result.parsedSuccessfully = true;

            AstAnalysisVisitor visitor(filePath, options);
            visitor.predeclareProgram(program);
            visitor.analyzeProgram(program);
            result = visitor.takeResult();
            result.filePath = filePath;
            return result;
        }
        catch (const std::exception &exception)
        {
            result.parsedSuccessfully = false;

            AnalysisIssue issue;
            issue.ruleId = "SAD-ENGINE-001";
            issue.kind = IssueKind::Syntax;
            issue.severity = Errors::DiagnosticSeverity::ERROR;
            issue.location = Errors::SourceLocation(filePath, 1, 1, 0, 1);
            issue.messageArabic = exception.what();
            issue.messageEnglish = exception.what();
            result.issues.push_back(std::move(issue));
            result.summary.totalErrors = 1;
            return result;
        }
    }

} // namespace Sad::Analizer
