// ============================================================================
// security_scanner_analysis.cpp — فحص الأنماط وتحليل التلوث والتنسيق
// (AR) فحص الأنماط الأمنية، تحليل التلوث، تنسيق التقارير
// (EN) Pattern scanning, taint analysis, security report formatting
// تم استخراج هذا الملف من security_scanner.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include "security_scanner.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <cassert>

namespace fs = std::filesystem;

namespace Sad {
namespace Security {
void SecurityScanner::patternScan(const std::string& source, const std::string& file,
                                   std::vector<Finding>& findings) {
    std::istringstream stream(source);
    std::string line;
    int lineNum = 0;

    while (std::getline(stream, line)) {
        lineNum++;
        if (shouldExcludeLine(line)) continue;

        // ״×״®״· ״§„״×״¹„‚״§״×
        std::string trimmed = line;
        size_t firstNonSpace = trimmed.find_first_not_of(" \t");
        if (firstNonSpace != std::string::npos) trimmed = trimmed.substr(firstNonSpace);
        if (trimmed.empty() || trimmed[0] == '#') continue;

        checkSQLInjection(line, lineNum, file, findings);
        checkBufferOverflow(line, lineNum, file, findings);
        checkInputValidation(line, lineNum, file, findings);
        checkWeakCrypto(line, lineNum, file, findings);
        checkHardcodedSecrets(line, lineNum, file, findings);
        checkUnsafeBlocks(line, lineNum, file, findings);
        checkPermissions(line, lineNum, file, findings);
        checkMemorySafety(line, lineNum, file, findings);
    }
}

static bool containsPattern(const std::string& line, const std::string& pattern) {
    // (AR) …״­״§ˆ„״© regex ״¥† ƒ״§† ״­״×ˆ ״£״­״± ״®״§״µ״©
    // NOTE(#24): (AR) ״×… ״×״±״¬…״©/״¨†״§״¡ ״§„״×״¹״¨״± ״§„†…״·  ƒ„ ״§״³״×״¯״¹״§״¡ ״¨״¯ˆ† ״×״®״²† …״₪‚״× (caching).
    //            …ƒ† ״¥״¶״§״© LRU cache „„״×״¹״¨״±״§״× ״§„…״×ƒ״±״±״© „״×״­״³† ״§„״£״¯״§״¡.
    // NOTE(#24): (EN) Regex pattern is compiled on every call without caching.
    //            An LRU cache for frequently-used patterns would improve performance.
    // NOTE(#28): (AR) C++ std::regex „״§ ״¯״¹… …‡„״© ״²…†״© (timeout) ג€” ״×״¹״¨״±״§״× †…״·״© …״¹‚״¯״©
    //            ‚״¯ ״×״³״¨״¨ ״×״¬…״¯ (ReDoS). …ƒ† ״§״³״×״®״¯״§… RE2 ƒ״¨״¯„ ״¢…†.
    // NOTE(#28): (EN) C++ std::regex has no timeout support ג€” complex patterns may cause
    //            catastrophic backtracking (ReDoS). Consider RE2 as a safe alternative.
    if (pattern.find('\\') != std::string::npos ||
        pattern.find('[') != std::string::npos ||
        pattern.find('*') != std::string::npos ||
        pattern.find('+') != std::string::npos ||
        pattern.find('.') != std::string::npos) {
        try {
            std::regex re(pattern, std::regex::icase);
            return std::regex_search(line, re);
        } catch (...) {
            return line.find(pattern) != std::string::npos;
        }
    }
    return line.find(pattern) != std::string::npos;
}

static Finding makeFinding(const SecurityRule& rule, const std::string& line,
                           int lineNum, const std::string& file) {
    Finding f;
    f.ruleId = rule.id;
    f.severity = rule.defaultSeverity;
    f.category = rule.category;
    f.message = "[" + rule.id + "] " + rule.nameAr;
    f.file = file;
    f.line = lineNum;
    f.codeLine = line;
    f.suggestion = rule.fixSuggestionAr;
    f.confidence = 0.8;
    return f;
}

// ============================================================================
// ״¯״§„״© ״­״µ ״¹״§…״© ״¨״¯„״§‹ …† 8 ״¯ˆ״§„ …״×ƒ״±״±״©
// Generic check function replacing 8 duplicate methods
// ============================================================================

/// (AR) ״­״µ ״³״·״± ˆ״§״­״¯ ״¶״¯ ״¬…״¹ ״§„‚ˆ״§״¹״¯ ״§„…״¹‘„״© ג€” †״³״®״© …״­״³‘†״© …ˆ״­‘״¯״©
/// (EN) Check a single line against all enabled rules ג€” unified optimized version
static void checkLineAgainstRules(const SecurityScanner* scanner,
                                   const std::vector<SecurityRule>& rules,
                                   const std::string& line, int lineNum,
                                   const std::string& file,
                                   std::vector<Finding>& findings,
                                   const ScannerConfig& config) {
    for (const auto& rule : rules) {
        if (!rule.enabled) continue;
        // ״­״µ ״§„‚ˆ״§״¦… ״§„״¨״¶״§״¡/״§„״³ˆ״¯״§״¡
        if (!config.enabledRules.empty()) {
            bool found = false;
            for (const auto& r : config.enabledRules) {
                if (r == rule.id) { found = true; break; }
            }
            if (!found) continue;
        }
        bool disabled = false;
        for (const auto& r : config.disabledRules) {
            if (r == rule.id) { disabled = true; break; }
        }
        if (disabled) continue;

        for (const auto& pat : rule.patterns) {
            if (containsPattern(line, pat)) {
                auto f = makeFinding(rule, line, lineNum, file);
                // ״×״®״µ״µ ״­״³״¨ ״§„״¦״©
                if (rule.category == RuleCategory::Injection && rule.id.find("SQL") != std::string::npos) {
                    f.attackExample = "'; DROP TABLE users; --";
                }
                else if (rule.category == RuleCategory::Injection && rule.id.find("XSS") != std::string::npos) {
                    f.attackExample = "<script>alert('XSS')</script>";
                }
                else if (rule.category == RuleCategory::Injection && rule.id.find("CMD") != std::string::npos) {
                    f.attackExample = "; rm -rf / || del /f /s";
                    f.confidence = 0.85;
                }
                else if (rule.category == RuleCategory::Injection && rule.id.find("SSRF") != std::string::npos) {
                    f.attackExample = "http://169.254.169.254/latest/meta-data/";
                    f.confidence = 0.75;
                }
                else if (rule.category == RuleCategory::Injection && rule.id.find("PATH") != std::string::npos) {
                    f.attackExample = "../../../../etc/passwd";
                }
                else if (rule.category == RuleCategory::Injection && rule.id.find("DESER") != std::string::npos) {
                    f.attackExample = "״¨״§†״§״× …״³„״³„״© ״®״¨״«״© ״×†״° ƒˆ״¯״§‹";
                }
                else if (rule.category == RuleCategory::Secrets) {
                    f.confidence = 0.7; // ״£״³״±״§״± ״«״§״¨״×״© ‚״¯ ״×ƒˆ† false positive
                }
                findings.push_back(std::move(f));
                break;  // †…״· ˆ״§״­״¯ ƒ״§ „ƒ„ ‚״§״¹״¯״©
            }
        }
    }
}

// === ״§„״×ˆ״§‚״©: ״§„״¯ˆ״§„ ״§„‚״¯…״© ״×״³״×״¯״¹ ״§„״¯״§„״© ״§„״¹״§…״© ===
void SecurityScanner::checkSQLInjection(const std::string& line, int lineNum,
                                         const std::string& file, std::vector<Finding>& findings) {
    std::vector<SecurityRule> filtered;
    for (const auto& r : rules_) if (r.category == RuleCategory::Injection) filtered.push_back(r);
    checkLineAgainstRules(this, filtered, line, lineNum, file, findings, config_);
}

void SecurityScanner::checkBufferOverflow(const std::string& line, int lineNum,
                                           const std::string& file, std::vector<Finding>& findings) {
    std::vector<SecurityRule> filtered;
    for (const auto& r : rules_) if (r.category == RuleCategory::BufferOverflow) filtered.push_back(r);
    checkLineAgainstRules(this, filtered, line, lineNum, file, findings, config_);
}

void SecurityScanner::checkInputValidation(const std::string& line, int lineNum,
                                            const std::string& file, std::vector<Finding>& findings) {
    std::vector<SecurityRule> filtered;
    for (const auto& r : rules_) if (r.category == RuleCategory::InputValidation) filtered.push_back(r);
    checkLineAgainstRules(this, filtered, line, lineNum, file, findings, config_);
}

void SecurityScanner::checkWeakCrypto(const std::string& line, int lineNum,
                                       const std::string& file, std::vector<Finding>& findings) {
    std::vector<SecurityRule> filtered;
    for (const auto& r : rules_) if (r.category == RuleCategory::Cryptography) filtered.push_back(r);
    checkLineAgainstRules(this, filtered, line, lineNum, file, findings, config_);
}

void SecurityScanner::checkHardcodedSecrets(const std::string& line, int lineNum,
                                             const std::string& file, std::vector<Finding>& findings) {
    std::vector<SecurityRule> filtered;
    for (const auto& r : rules_) if (r.category == RuleCategory::Secrets) filtered.push_back(r);
    checkLineAgainstRules(this, filtered, line, lineNum, file, findings, config_);
}

void SecurityScanner::checkUnsafeBlocks(const std::string& line, int lineNum,
                                         const std::string& file, std::vector<Finding>& findings) {
    std::vector<SecurityRule> filtered;
    for (const auto& r : rules_) if (r.category == RuleCategory::UnsafeBlock) filtered.push_back(r);
    checkLineAgainstRules(this, filtered, line, lineNum, file, findings, config_);
}

void SecurityScanner::checkPermissions(const std::string& line, int lineNum,
                                        const std::string& file, std::vector<Finding>& findings) {
    std::vector<SecurityRule> filtered;
    for (const auto& r : rules_) if (r.category == RuleCategory::Permissions) filtered.push_back(r);
    checkLineAgainstRules(this, filtered, line, lineNum, file, findings, config_);
}

void SecurityScanner::checkMemorySafety(const std::string& line, int lineNum,
                                         const std::string& file, std::vector<Finding>& findings) {
    std::vector<SecurityRule> filtered;
    for (const auto& r : rules_) if (r.category == RuleCategory::Memory) filtered.push_back(r);
    checkLineAgainstRules(this, filtered, line, lineNum, file, findings, config_);
}

// ============================================================================
// Taint analysis
// ============================================================================

void SecurityScanner::taintAnalysis(const std::string& source, const std::string& file,
                                     std::vector<Finding>& findings) {
    // ״×״­„„ ״×„ˆ‘״« …״×‚״¯…: ״×״×״¨״¹ ״§„…״×״÷״±״§״× …† ״§„…״µ״§״¯״± ״¹״¨״± ״§„״×†‚״© ״¥„‰ ״§„…״µ״§״±
    static const std::vector<std::string> taintSources = {
        "״§‚״±״£(", "״§‚״±״£_״³״·״±(", "״§״­״µ„_״¹„‰_…״¹״§…„(",
        "״§״­״µ„_״¹„‰_״±״£״³(", "״§״­״µ„_״¹„‰_ƒˆƒ(", "input(", "readline(",
        "״§״­״µ„_״¹„‰_״¬״³…(", "request.body", "request.query",
        "fetch(", "״¬„״¨(", "״§‚״±״£_…„("
    };
    static const std::vector<std::string> taintSinks = {
        "†‘״°_SQL(", "״§״³״×״¹„…(", "״§ƒ״×״¨_HTML(",
        "†‘״°(", "shell(", "system(", "exec(",
        "״§״×״­_…„(", "״­ˆ‘„_״¥„‰(",
        "״£״±״³„_״¨״±״¯(", "redirect(", "״£״¹״¯_״×ˆ״¬‡(",
        "eval(", "‚‘…(", "innerHTML"
    };
    // ״¯ˆ״§„ ״§„״×†‚״© ג€” ״¥״°״§ …״±‘ ״§„…״×״÷״± ״¹״¨״±‡״§ ״²״§„ ״§„״×„ˆ‘״«
    static const std::vector<std::string> sanitizers = {
        "†‚‘(", "sanitize(", "״×״±…״²_HTML(", "escapeHtml(",
        "״×״­‚‚(", "validate(", "„״×״±(", "filter(",
        "escape(", "encode(", "״×״·״¨״¹_…״³״§״±(", "normalize_path(",
        "ƒ„…״©_…״±ˆ״±_‡״§״´(", "hash_password("
    };

    // ״§„…״±״­„״© 1: ״§ƒ״×״´״§ ״§„…״×״÷״±״§״× ״§„…„ˆ‘״«״©
    std::map<std::string, int> taintedVars; // ״§״³… -> ״±‚… ״³״·״± ״§„…״µ״¯״±
    std::map<std::string, bool> sanitizedVars; // ‡„ ״×… ״×†‚״© ״§„…״×״÷״±״
    std::istringstream stream(source);
    std::string line;
    int lineNum = 0;

    while (std::getline(stream, line)) {
        lineNum++;
        if (shouldExcludeLine(line)) continue;

        // ״§„״¨״­״« ״¹† ״×״¹†״§״× …† …״µ״§״¯״± …„ˆ‘״«״©
        for (const auto& src : taintSources) {
            if (line.find(src) != std::string::npos) {
                // ״§״³״×״®״±״§״¬ ״§״³… ״§„…״×״÷״±: …״×״÷״± ״§״³… = …״µ״¯״±()
                size_t eq = line.find('=');
                if (eq != std::string::npos && line.find(src) > eq) {
                    std::string leftSide = line.substr(0, eq);
                    // ״§„״×״®„״µ …† "…״×״÷״±" ˆ״§„…״³״§״§״×
                    size_t varStart = leftSide.find_first_not_of(" \t");
                    if (varStart != std::string::npos) {
                        std::string varDecl = leftSide.substr(varStart);
                        // ״¥״²״§„״© "…״×״÷״±" ״¥† ˆ״¬״¯
                        if (varDecl.find("…״×״÷״±") == 0) {
                            size_t ws = varDecl.find(' ');
                            if (ws != std::string::npos) varDecl = varDecl.substr(ws + 1);
                        }
                        // ״¥״²״§„״© ״§„…״³״§״§״×
                        size_t s = varDecl.find_first_not_of(" \t");
                        size_t e = varDecl.find_last_not_of(" \t");
                        if (s != std::string::npos) {
                            std::string varName = varDecl.substr(s, e - s + 1);
                            taintedVars[varName] = lineNum;
                            sanitizedVars[varName] = false;
                        }
                    }
                }
            }
        }

        // ״§„…״±״­„״© 1.5: ƒ״´ ״§„״×†‚״© ג€” ״¥״°״§ …״±‘ …״×״÷״± …„ˆ‘״« ״¹״¨״± ״¯״§„״© ״×†‚״© ״²״§„ ״×„ˆ‘״«‡
        for (const auto& san : sanitizers) {
            if (line.find(san) != std::string::npos) {
                for (auto& [varName, srcLine] : taintedVars) {
                    if (line.find(varName) != std::string::npos) {
                        sanitizedVars[varName] = true;
                    }
                }
                // ״£״¶״§‹ ״­״µ ״¥״°״§ ƒ״§†״× ״§„†״×״¬״© ״×״¹‘† „…״×״÷״± ״¬״¯״¯
                size_t eq = line.find('=');
                if (eq != std::string::npos) {
                    std::string leftSide = line.substr(0, eq);
                    size_t s = leftSide.find_first_not_of(" \t");
                    size_t e = leftSide.find_last_not_of(" \t");
                    if (s != std::string::npos) {
                        std::string newVar = leftSide.substr(s, e - s + 1);
                        if (newVar.find("…״×״÷״±") == 0) {
                            size_t ws = newVar.find(' ');
                            if (ws != std::string::npos) newVar = newVar.substr(ws + 1);
                            size_t ns = newVar.find_first_not_of(" \t");
                            size_t ne = newVar.find_last_not_of(" \t");
                            if (ns != std::string::npos) newVar = newVar.substr(ns, ne - ns + 1);
                        }
                        // ״§„…״×״÷״± ״§„״¬״¯״¯ †״§״×״¬ ״¹† ״×†‚״© ג€” „״§ ״¹״×״¨״± …„ˆ‘״«״§‹
                        sanitizedVars[newVar] = true;
                    }
                }
            }
        }
    }

    if (taintedVars.empty()) return;

    // ״§„…״±״­„״© 2: ״­״µ ״§״³״×״®״¯״§… ״§„…״×״÷״±״§״× ״§„…„ˆ‘״«״© (״÷״± ״§„…†‚‘״§״©)  ״§„…״µ״§״±
    stream.clear();
    stream.str(source);
    lineNum = 0;

    while (std::getline(stream, line)) {
        lineNum++;
        if (shouldExcludeLine(line)) continue;

        for (const auto& sink : taintSinks) {
            if (line.find(sink) != std::string::npos) {
                for (const auto& [varName, srcLine] : taintedVars) {
                    if (line.find(varName) != std::string::npos) {
                        // ״×״®״· ״§„…״×״÷״±״§״× ״§„…†‚‘״§״©
                        auto it = sanitizedVars.find(varName);
                        if (it != sanitizedVars.end() && it->second) continue;

                        Finding f;
                        f.ruleId = "TAINT001";
                        f.severity = Severity::Critical;
                        f.category = RuleCategory::DataFlow;
                        f.message = "״¨״§†״§״× …„ˆ‘״«״© …† ״³״·״± " + std::to_string(srcLine) +
                                    " ״×״³״×״®״¯…  …״µ״± ״®״·״± ״¨״¯ˆ† ״×†‚״©";
                        f.file = file;
                        f.line = lineNum;
                        f.codeLine = line;
                        f.suggestion = "†‚‘ ״§„…״×״÷״± '" + varName + "' ״¹״¨״± †‚‘() ״£ˆ validate() ‚״¨„ ״§״³״×״®״¯״§…‡ ‡†״§";
                        f.confidence = 0.9;
                        f.autoFixable = true;
                        f.autoFixCode = "†‚‘(" + varName + ")";
                        findings.push_back(std::move(f));
                    }
                }
            }
        }
    }
}

// ============================================================================
// ReportFormatter
// ============================================================================

std::string SecurityReportFormatter::formatText(const ScanResult& result, bool verbose) {
    std::ostringstream out;
    out << "\nג•”ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•—\n"
        << "ג•‘  נ›¡ן¸  ״×‚״±״± ״§„״­״§״±״³ ״§„״£…† ג€” „״÷״© ״µ                               ג•‘\n"
        << "ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•\n\n";

    out << "נ“ …„״®״µ: " << result.filesScanned << " …„ | "
        << result.totalLines << " ״³״·״± | "
        << result.findings.size() << " †״×״¬״© | "
        << std::fixed;
    out.precision(2);
    out << result.durationSeconds << " ״«״§†״©\n\n";

    if (result.findings.empty()) {
        out << "  ג… „… ״×ƒ״×״´ ״£ ״«״÷״±״§״× ״£…†״©!\n\n";
        return out.str();
    }

    out << "  נ”´ ״­״±״¬״©: " << result.criticalCount()
        << "  נ  ״¹״§„״©: " << result.highCount()
        << "  נ¡ …״×ˆ״³״·״©: " << result.mediumCount()
        << "  נ”µ …†״®״¶״©: " << result.lowCount() << "\n\n";

    out << "ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€\n";

    for (const auto& f : result.findings) {
        out << f.severityIcon() << " [" << f.ruleId << "] " << f.message << "\n"
            << "   נ“ " << f.file << ":" << f.line << "\n";
        if (verbose && !f.codeLine.empty()) {
            std::string trimmed = f.codeLine;
            size_t s = trimmed.find_first_not_of(" \t");
            if (s != std::string::npos) trimmed = trimmed.substr(s);
            out << "   נ“ " << trimmed << "\n";
        }
        if (!f.suggestion.empty())
            out << "   נ’¡ " << f.suggestion << "\n";
        if (verbose && !f.attackExample.empty())
            out << "   ג”ן¸  …״«״§„ ‡״¬ˆ…: " << f.attackExample << "\n";
        out << "\n";
    }

    out << "ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•\n";
    if (result.hasCritical())
        out << "  ג›” ״×… ״§ƒ״×״´״§ ״«״÷״±״§״× ״­״±״¬״© ג€” ״¬״¨ ״§„״¥״µ„״§״­ ‚״¨„ ״§„†״´״±!\n";
    else if (result.highCount() > 0)
        out << "  ג ן¸  ״×… ״§ƒ״×״´״§ ״«״÷״±״§״× ״¹״§„״© ג€” †״µ״­ ״¨״§„״¥״µ„״§״­.\n";
    else
        out << "  ג… „״§ ״×ˆ״¬״¯ ״«״÷״±״§״× ״­״±״¬״© ״£ˆ ״¹״§„״©.\n";

    return out.str();
}

static std::string escJson(const std::string& s) {
    std::string r; r.reserve(s.size());
    for (char c : s) {
        switch(c) { case '"':r+="\\\"";break; case '\\':r+="\\\\";break;
        case '\n':r+="\\n";break; case '\r':r+="\\r";break; case '\t':r+="\\t";break;
        default: r+=c; }
    }
    return r;
}

std::string SecurityReportFormatter::formatJSON(const ScanResult& result) {
    std::ostringstream out;
    out << "{\n  \"summary\": {\n"
        << "    \"filesScanned\": " << result.filesScanned << ",\n"
        << "    \"totalLines\": " << result.totalLines << ",\n"
        << "    \"totalFindings\": " << result.findings.size() << ",\n"
        << "    \"critical\": " << result.criticalCount() << ",\n"
        << "    \"high\": " << result.highCount() << ",\n"
        << "    \"medium\": " << result.mediumCount() << ",\n"
        << "    \"low\": " << result.lowCount() << ",\n"
        << "    \"durationSeconds\": " << result.durationSeconds << "\n"
        << "  },\n  \"findings\": [\n";

    for (size_t i = 0; i < result.findings.size(); ++i) {
        const auto& f = result.findings[i];
        out << "    {\n"
            << "      \"ruleId\": \"" << escJson(f.ruleId) << "\",\n"
            << "      \"severity\": \"" << escJson(f.severityNameAr()) << "\",\n"
            << "      \"category\": \"" << escJson(f.categoryNameAr()) << "\",\n"
            << "      \"message\": \"" << escJson(f.message) << "\",\n"
            << "      \"file\": \"" << escJson(f.file) << "\",\n"
            << "      \"line\": " << f.line << ",\n"
            << "      \"codeLine\": \"" << escJson(f.codeLine) << "\",\n"
            << "      \"suggestion\": \"" << escJson(f.suggestion) << "\",\n"
            << "      \"confidence\": " << f.confidence << "\n"
            << "    }";
        if (i < result.findings.size() - 1) out << ",";
        out << "\n";
    }
    out << "  ]\n}\n";
    return out.str();
}

std::string SecurityReportFormatter::formatHTML(const ScanResult& result, const std::string& title) {
    std::ostringstream out;
    std::string t = title.empty() ? "״×‚״±״± ״§„״­״§״±״³ ״§„״£…†" : title;

    out << "<!DOCTYPE html>\n<html dir=\"rtl\" lang=\"ar\"><head><meta charset=\"UTF-8\">\n"
        << "<title>" << t << "</title>\n<style>\n"
        << ":root{--bg:#0f0f23;--card:#1a1a3e;--text:#e8e8f0;--border:#2a2a4a;--accent:#4dabf7}"
        << "body{font-family:'Segoe UI',Tahoma,sans-serif;background:var(--bg);color:var(--text);padding:20px;max-width:1000px;margin:0 auto}"
        << "h1{color:var(--accent);text-align:center}"
        << ".summary{display:grid;grid-template-columns:repeat(5,1fr);gap:10px;margin:20px 0}"
        << ".stat{background:var(--card);padding:15px;border-radius:8px;text-align:center;border:1px solid var(--border)}"
        << ".stat .num{font-size:2em;font-weight:bold}.critical .num{color:#ff6b6b}.high .num{color:#ffa94d}.medium .num{color:#ffd43b}.low .num{color:#74c0fc}"
        << ".finding{background:var(--card);padding:15px;border-radius:8px;margin:10px 0;border-right:4px solid}"
        << ".finding.sev-critical{border-color:#ff6b6b}.finding.sev-high{border-color:#ffa94d}.finding.sev-medium{border-color:#ffd43b}.finding.sev-low{border-color:#74c0fc}"
        << ".finding .rule{font-weight:bold;color:var(--accent)}.finding .loc{color:#888;font-size:.85em}"
        << ".finding code{background:#0a0a1a;padding:4px 8px;border-radius:4px;display:block;margin:5px 0;font-family:monospace;overflow-x:auto}"
        << ".finding .fix{color:#51cf66;font-style:italic}\n</style></head><body>\n"
        << "<h1>נ›¡ן¸ " << t << "</h1>\n";

    out << "<div class=\"summary\">\n"
        << "<div class=\"stat\"><div class=\"num\">" << result.filesScanned << "</div>…„</div>\n"
        << "<div class=\"stat critical\"><div class=\"num\">" << result.criticalCount() << "</div>״­״±״¬״©</div>\n"
        << "<div class=\"stat high\"><div class=\"num\">" << result.highCount() << "</div>״¹״§„״©</div>\n"
        << "<div class=\"stat medium\"><div class=\"num\">" << result.mediumCount() << "</div>…״×ˆ״³״·״©</div>\n"
        << "<div class=\"stat low\"><div class=\"num\">" << result.lowCount() << "</div>…†״®״¶״©</div>\n"
        << "</div>\n\n";

    if (result.findings.empty()) {
        out << "<div style='text-align:center;padding:40px'><h2>ג… „״§ ״×ˆ״¬״¯ ״«״÷״±״§״× ״£…†״©</h2></div>\n";
    } else {
        for (const auto& f : result.findings) {
            std::string sevClass;
            switch(f.severity) {
                case Severity::Critical: sevClass = "sev-critical"; break;
                case Severity::High: sevClass = "sev-high"; break;
                case Severity::Medium: sevClass = "sev-medium"; break;
                default: sevClass = "sev-low";
            }
            out << "<div class=\"finding " << sevClass << "\">\n"
                << "  <span class=\"rule\">" << f.severityIcon() << " [" << f.ruleId << "]</span> "
                << f.message << "\n"
                << "  <div class=\"loc\">נ“ " << f.file << ":" << f.line << "</div>\n";
            if (!f.codeLine.empty()) out << "  <code>" << f.codeLine << "</code>\n";
            if (!f.suggestion.empty()) out << "  <div class=\"fix\">נ’¡ " << f.suggestion << "</div>\n";
            out << "</div>\n";
        }
    }

    out << "</body></html>\n";
    return out.str();
}

std::string SecurityReportFormatter::formatSARIF(const ScanResult& result) {
    std::ostringstream out;
    out << "{\n  \"$schema\": \"https://raw.githubusercontent.com/oasis-tcs/sarif-spec/master/Schemata/sarif-schema-2.1.0.json\",\n"
        << "  \"version\": \"2.1.0\",\n  \"runs\": [{\n"
        << "    \"tool\": {\"driver\": {\"name\": \"sad-guard\", \"version\": \"1.0.0\"}},\n"
        << "    \"results\": [\n";

    for (size_t i = 0; i < result.findings.size(); ++i) {
        const auto& f = result.findings[i];
        std::string level;
        switch(f.severity) {
            case Severity::Critical: case Severity::High: level = "error"; break;
            case Severity::Medium: level = "warning"; break;
            default: level = "note";
        }
        out << "      {\"ruleId\": \"" << f.ruleId << "\", \"level\": \"" << level
            << "\", \"message\": {\"text\": \"" << escJson(f.message) << "\"}"
            << ", \"locations\": [{\"physicalLocation\": {\"artifactLocation\": {\"uri\": \""
            << escJson(f.file) << "\"}, \"region\": {\"startLine\": " << f.line << "}}}]}";
        if (i < result.findings.size()-1) out << ",";
        out << "\n";
    }
    out << "    ]\n  }]\n}\n";
    return out.str();
}

bool SecurityReportFormatter::writeReport(const ScanResult& result, const std::string& path, ReportFormat format) {
    std::ofstream file(path);
    if (!file.is_open()) return false;
    switch (format) {
        case ReportFormat::Text: file << formatText(result, true); break;
        case ReportFormat::JSON: file << formatJSON(result); break;
        case ReportFormat::HTML: file << formatHTML(result); break;
        case ReportFormat::SARIF: file << formatSARIF(result); break;
    }
    return file.good();
}

// ============================================================================
// CLI
// ============================================================================

int guard_main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "ג•”ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•—\n"
                  << "ג•‘  נ›¡ן¸  ״­״§״±״³ ״µ ״§„״£…† ג€” sad-guard            ג•‘\n"
                  << "ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•\n\n"
                  << "״§„״§״³״×״®״¯״§…:\n"
                  << "  sad-guard <…„.״µ>            ״­״µ …„\n"
                  << "  sad-guard <…״¬„״¯>             ״­״µ …״¬„״¯\n"
                  << "  sad-guard --json <…„>       ״¥״®״±״§״¬ JSON\n"
                  << "  sad-guard --html <…„>       ״¥״®״±״§״¬ HTML\n"
                  << "  sad-guard --sarif <…„>      ״¥״®״±״§״¬ SARIF\n"
                  << "  sad-guard --ci <…״¬„״¯>        ˆ״¶״¹ CI/CD\n"
                  << "  sad-guard --critical <…„>   ״§„״­״±״¬״© ‚״·\n"
                  << "  sad-guard --report <…„> <‡״¯>  ״­״¸ ״§„״×‚״±״±\n"
                  << "  sad-guard --config <…„>     …„ ״¥״¹״¯״§״¯״§״×\n\n";
        return 1;
    }

    ScannerConfig config;
    config.excludeLines = {"// noguard", "# noguard", "״­״§״±״³:״×״¬״§‡„"};
    std::string path, reportPath;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--json") config.reportFormat = ReportFormat::JSON;
        else if (arg == "--html") config.reportFormat = ReportFormat::HTML;
        else if (arg == "--sarif") config.reportFormat = ReportFormat::SARIF;
        else if (arg == "--ci") config.ciMode = true;
        else if (arg == "--critical") config.criticalOnly = true;
        else if (arg == "--verbose" || arg == "-v") config.verbose = true;
        else if (arg == "--no-taint") config.taintAnalysis = false;
        else if (arg == "--no-pattern") config.patternScan = false;
        else if (arg == "--report" && i + 1 < argc) reportPath = argv[++i];
        else if (arg == "--config" && i + 1 < argc) config = ScannerConfig::fromFile(argv[++i]);
        else if (arg == "--disable" && i + 1 < argc) config.disabledRules.push_back(argv[++i]);
        else path = arg;
    }

    if (path.empty()) { std::cerr << "״®״·״£: ״¬״¨ ״×״­״¯״¯ …„ ״£ˆ …״¬„״¯\n"; return 1; }

    SecurityScanner scanner(config);
    ScanResult result;

    if (fs::is_directory(path)) {
        result = scanner.scanDirectory(path);
    } else if (fs::is_regular_file(path)) {
        result = scanner.scanFile(path);
    } else {
        std::cerr << "״®״·״£: ״§„‡״¯ ״÷״± …ˆ״¬ˆ״¯: " << path << "\n";
        return 1;
    }

    // ״¹״±״¶ ״§„†״×״§״¦״¬
    switch (config.reportFormat) {
        case ReportFormat::JSON: std::cout << SecurityReportFormatter::formatJSON(result); break;
        case ReportFormat::HTML: std::cout << SecurityReportFormatter::formatHTML(result); break;
        case ReportFormat::SARIF: std::cout << SecurityReportFormatter::formatSARIF(result); break;
        default: std::cout << SecurityReportFormatter::formatText(result, config.verbose);
    }

    // ״­״¸ ״§„״×‚״±״±
    if (!reportPath.empty()) {
        SecurityReportFormatter::writeReport(result, reportPath, config.reportFormat);
        std::cout << "  נ’¾ ״×… ״­״¸ ״§„״×‚״±״±: " << reportPath << "\n";
    }

    // ˆ״¶״¹ CI
    if (config.ciMode) {
        return result.hasCritical() ? 2 : (result.highCount() > 0 ? 1 : 0);
    }

    return result.hasCritical() ? 1 : 0;
}


} // namespace Security
} // namespace Sad
