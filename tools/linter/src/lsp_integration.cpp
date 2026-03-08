/**
 * =============================================================================
 * ملف: lsp_integration.cpp
 * الوصف: تكامل محلل Linter مع خادم LSP
 * (AR) @brief ربط قواعد Lint بالتشخيصات في LSP
 * (EN) @brief LSP integration — publish lint diagnostics
 * المهمة: T305 - Linter LSP integration
 * =============================================================================
 */

#include "linter_engine.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace sad::linter {

// =============================================================================
// واجهة تكامل LSP
// =============================================================================

/**
 * (AR) تحويل تشخيصات Lint إلى صيغة LSP JSON
 * (EN) Convert lint diagnostics to LSP JSON format
 */
class LspLinterBridge {
public:
    LspLinterBridge() {
        engine_ = rules::LinterEngine::createDefault();
    }
    
    /**
     * (AR) تحليل ملف وإرجاع تشخيصات LSP بصيغة JSON
     * (EN) Analyze file and return LSP diagnostics as JSON
     */
    std::string analyzefile(const std::string& filePath,
                           const std::string& content) {
        auto diagnostics = engine_->analyzeFile(filePath, content);
        return diagnosticsToLspJson(filePath, diagnostics);
    }
    
    /**
     * (AR) الحصول على تشخيصات لملف (بدون JSON)
     * (EN) Get diagnostics for a file (without JSON)
     */
    std::vector<LintDiagnostic> getDiagnostics(const std::string& filePath,
                                               const std::string& content) {
        return engine_->analyzeFile(filePath, content);
    }
    
    /**
     * (AR) تعطيل/تفعيل قاعدة
     * (EN) Enable/disable rule
     */
    void setRuleEnabled(const std::string& ruleId, bool enabled) {
        engine_->setRuleEnabled(ruleId, enabled);
    }
    
    /**
     * (AR) تحويل Severity إلى LSP DiagnosticSeverity
     * (EN) Convert Severity to LSP DiagnosticSeverity
     */
    static int severityToLsp(Severity s) {
        switch (s) {
            case Severity::Error:   return 1;  // DiagnosticSeverity.Error
            case Severity::Warning: return 2;  // DiagnosticSeverity.Warning
            case Severity::Info:    return 3;  // DiagnosticSeverity.Information
            case Severity::Hint:    return 4;  // DiagnosticSeverity.Hint
        }
        return 3;
    }

private:
    std::unique_ptr<rules::LinterEngine> engine_;
    
    /**
     * (AR) تحويل تشخيصات إلى JSON متوافق مع LSP
     * (EN) Convert diagnostics to LSP-compatible JSON
     */
    std::string diagnosticsToLspJson(const std::string& uri,
                                     const std::vector<LintDiagnostic>& diags) {
        std::ostringstream json;
        json << "{\n";
        json << "  \"jsonrpc\": \"2.0\",\n";
        json << "  \"method\": \"textDocument/publishDiagnostics\",\n";
        json << "  \"params\": {\n";
        json << "    \"uri\": \"" << escapeJson(uri) << "\",\n";
        json << "    \"diagnostics\": [\n";
        
        for (size_t i = 0; i < diags.size(); i++) {
            const auto& d = diags[i];
            if (i > 0) json << ",\n";
            
            int startLine = std::max(0, d.line - 1);
            int startCol = std::max(0, d.column - 1);
            int endLine = d.endLine > 0 ? d.endLine - 1 : startLine;
            int endCol = d.endColumn > 0 ? d.endColumn - 1 : startCol + 10;
            
            json << "      {\n";
            json << "        \"range\": {\n";
            json << "          \"start\": {\"line\": " << startLine 
                 << ", \"character\": " << startCol << "},\n";
            json << "          \"end\": {\"line\": " << endLine 
                 << ", \"character\": " << endCol << "}\n";
            json << "        },\n";
            json << "        \"severity\": " << severityToLsp(d.severity) << ",\n";
            json << "        \"code\": \"" << d.ruleId << "\",\n";
            json << "        \"source\": \"sad-lint\",\n";
            json << "        \"message\": \"" << escapeJson(d.message) << "\"";
            
            if (!d.suggestion.empty()) {
                json << ",\n        \"relatedInformation\": [{\n";
                json << "          \"location\": {\n";
                json << "            \"uri\": \"" << escapeJson(uri) << "\",\n";
                json << "            \"range\": {\n";
                json << "              \"start\": {\"line\": " << startLine 
                     << ", \"character\": " << startCol << "},\n";
                json << "              \"end\": {\"line\": " << endLine 
                     << ", \"character\": " << endCol << "}\n";
                json << "            }\n";
                json << "          },\n";
                json << "          \"message\": \"" << escapeJson(d.suggestion) << "\"\n";
                json << "        }]";
            }
            
            json << "\n      }";
        }
        
        json << "\n    ]\n";
        json << "  }\n";
        json << "}";
        
        return json.str();
    }
    
    static std::string escapeJson(const std::string& s) {
        std::string result;
        result.reserve(s.size() + 10);
        for (char c : s) {
            switch (c) {
                case '"':  result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:   result += c; break;
            }
        }
        return result;
    }
};

// =============================================================================
// واجهة سطر الأوامر
// =============================================================================

/**
 * (AR) واجهة CLI للمحلل
 * (EN) Linter CLI interface
 */
class LinterCLI {
public:
    struct Options {
        std::vector<std::string> files;
        bool jsonOutput = false;
        bool quiet = false;
        std::unordered_set<std::string> disabledRules;
        Severity minSeverity = Severity::Hint;
    };
    
    /**
     * (AR) تشغيل المحلل على ملفات
     * (EN) Run linter on files
     */
    static int run(const Options& opts) {
        auto engine = rules::LinterEngine::createDefault();
        
        // تعطيل القواعد المحددة
        for (const auto& rule : opts.disabledRules) {
            engine->setRuleEnabled(rule, false);
        }
        
        int totalErrors = 0;
        int totalWarnings = 0;
        int totalInfos = 0;
        
        for (const auto& file : opts.files) {
            // قراءة الملف
            std::ifstream ifs(file);
            if (!ifs.is_open()) {
                std::cerr << "خطأ: لا يمكن فتح الملف " << file << std::endl;
                continue;
            }
            
            std::string content((std::istreambuf_iterator<char>(ifs)),
                               std::istreambuf_iterator<char>());
            
            auto diags = engine->analyzeFile(file, content);
            
            // تصفية حسب المستوى
            for (const auto& d : diags) {
                if (static_cast<int>(d.severity) > static_cast<int>(opts.minSeverity))
                    continue;
                
                if (!opts.quiet) {
                    if (opts.jsonOutput) {
                        // JSON output per diagnostic
                        std::cout << d.toString() << "\n";
                    } else {
                        std::cout << d.toString() << "\n";
                        if (!d.suggestion.empty()) {
                            std::cout << "  💡 " << d.suggestion << "\n";
                        }
                    }
                }
                
                switch (d.severity) {
                    case Severity::Error: totalErrors++; break;
                    case Severity::Warning: totalWarnings++; break;
                    default: totalInfos++; break;
                }
            }
        }
        
        if (!opts.quiet) {
            std::cout << "\n═══════════════════════════════\n";
            std::cout << "النتائج: " << totalErrors << " أخطاء، " 
                      << totalWarnings << " تحذيرات، "
                      << totalInfos << " معلومات\n";
            std::cout << "═══════════════════════════════\n";
        }
        
        return totalErrors > 0 ? 1 : 0;
    }
    
    /**
     * (AR) طباعة المساعدة
     * (EN) Print help
     */
    static void printHelp() {
        std::cout << "استخدام: sad-lint [خيارات] <ملفات...>\n\n";
        std::cout << "خيارات:\n";
        std::cout << "  --json          إخراج بصيغة JSON\n";
        std::cout << "  --quiet         عرض الملخص فقط\n";
        std::cout << "  --disable=RULE  تعطيل قاعدة محددة\n";
        std::cout << "  --min=LEVEL     الحد الأدنى للمستوى (error/warning/info/hint)\n";
        std::cout << "  --list          عرض جميع القواعد\n";
        std::cout << "  --help          عرض المساعدة\n\n";
        
        std::cout << "القواعد المتاحة:\n";
        auto engine = rules::LinterEngine::createDefault();
        for (const auto* rule : engine->getAllRules()) {
            std::cout << "  " << rule->getId() << " — " 
                      << rule->getName() << " [" 
                      << severityToString(rule->getDefaultSeverity()) << "]\n";
            std::cout << "         " << rule->getDescription() << "\n";
        }
    }
};

} // namespace sad::linter
