// ״¨״³… ״§„„‡ ״§„״±״­…† ״§„״±״­…
// ============================================================================
// security_scanner.cpp ג€” ״×†״° ״§„״­״§״±״³ ״§„״£…† ״§„״´״§…„
// Comprehensive Security Scanner Implementation
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

// ============================================================================
// Finding helpers
// ============================================================================

std::string Finding::severityIcon() const {
    switch (severity) {
        case Severity::Critical: return "נ”´";
        case Severity::High:     return "נ ";
        case Severity::Medium:   return "נ¡";
        case Severity::Low:      return "נ”µ";
        case Severity::Info:     return "ג×";
    }
    return "ג”";
}

std::string Finding::severityNameAr() const {
    switch (severity) {
        case Severity::Critical: return "״­״±״¬״©";
        case Severity::High:     return "״¹״§„״©";
        case Severity::Medium:   return "…״×ˆ״³״·״©";
        case Severity::Low:      return "…†״®״¶״©";
        case Severity::Info:     return "…״¹„ˆ…״§״×";
    }
    return "״÷״± …״­״¯״¯";
}

std::string Finding::categoryNameAr() const {
    switch (category) {
        case RuleCategory::Injection:       return "״­‚†";
        case RuleCategory::BufferOverflow:  return "״¶ ״§„…״®״²†";
        case RuleCategory::InputValidation: return "״§„״×״­‚‚ …† ״§„…״¯״®„״§״×";
        case RuleCategory::Cryptography:    return "״§„״×״´״±";
        case RuleCategory::Secrets:         return "״£״³״±״§״± ״«״§״¨״×״©";
        case RuleCategory::Permissions:     return "״§„״µ„״§״­״§״×";
        case RuleCategory::DataFlow:        return "״×״¯‚ ״§„״¨״§†״§״×";
        case RuleCategory::UnsafeBlock:     return "ƒ״×„״© ״÷״± ״¢…†״©";
        case RuleCategory::Memory:          return "״£…״§† ״§„״°״§ƒ״±״©";
        case RuleCategory::Configuration:   return "״§„״¥״¹״¯״§״¯״§״×";
    }
    return "״£״®״±‰";
}

// ============================================================================
// ScanResult
// ============================================================================

int ScanResult::criticalCount() const {
    return (int)std::count_if(findings.begin(), findings.end(),
        [](const Finding& f) { return f.severity == Severity::Critical; });
}
int ScanResult::highCount() const {
    return (int)std::count_if(findings.begin(), findings.end(),
        [](const Finding& f) { return f.severity == Severity::High; });
}
int ScanResult::mediumCount() const {
    return (int)std::count_if(findings.begin(), findings.end(),
        [](const Finding& f) { return f.severity == Severity::Medium; });
}
int ScanResult::lowCount() const {
    return (int)std::count_if(findings.begin(), findings.end(),
        [](const Finding& f) { return f.severity == Severity::Low; });
}
int ScanResult::infoCount() const {
    return (int)std::count_if(findings.begin(), findings.end(),
        [](const Finding& f) { return f.severity == Severity::Info; });
}

// ============================================================================
// ScannerConfig
// ============================================================================

ScannerConfig ScannerConfig::fromFile(const std::string& path) {
    ScannerConfig cfg;
    std::ifstream file(path);
    if (!file.is_open()) return cfg;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        auto trim = [](std::string& s) {
            size_t a = s.find_first_not_of(" \t");
            size_t b = s.find_last_not_of(" \t\r\n");
            s = (a != std::string::npos) ? s.substr(a, b - a + 1) : "";
        };
        trim(key); trim(val);

        if (key == "verbose") cfg.verbose = (val == "true");
        else if (key == "critical_only") cfg.criticalOnly = (val == "true");
        else if (key == "auto_fix") cfg.autoFix = (val == "true");
        else if (key == "ci_mode") cfg.ciMode = (val == "true");
        else if (key == "taint_analysis") cfg.taintAnalysis = (val == "true");
        else if (key == "pattern_scan") cfg.patternScan = (val == "true");
        else if (key == "report_format") {
            if (val == "json") cfg.reportFormat = ReportFormat::JSON;
            else if (val == "html") cfg.reportFormat = ReportFormat::HTML;
            else if (val == "sarif") cfg.reportFormat = ReportFormat::SARIF;
            else cfg.reportFormat = ReportFormat::Text;
        }
        else if (key == "report_file") cfg.reportFile = val;
        else if (key == "min_severity") {
            if (val == "critical") cfg.minSeverity = Severity::Critical;
            else if (val == "high") cfg.minSeverity = Severity::High;
            else if (val == "medium") cfg.minSeverity = Severity::Medium;
            else cfg.minSeverity = Severity::Low;
        }
        else if (key == "disable_rule") cfg.disabledRules.push_back(val);
        else if (key == "enable_rule") cfg.enabledRules.push_back(val);
        else if (key == "exclude") cfg.excludePatterns.push_back(val);
    }
    return cfg;
}

// ============================================================================
// SecurityScanner ג€” initialization
// ============================================================================

SecurityScanner::SecurityScanner(const ScannerConfig& config) : config_(config) {
    initializeRules();
}

void SecurityScanner::initializeRules() {
    rules_.clear();

    // === SQL001: ״­‚† SQL ===
    rules_.push_back({
        "SQL001", "״­‚† SQL", "SQL Injection",
        RuleCategory::Injection, Severity::Critical,
        "״¨״§†״§״× ״÷״± …†‚‘״§״© ״×״³״×״®״¯…  ״§״³״×״¹„״§… SQL …״¨״§״´״±״©",
        "Unsanitized data used directly in SQL query",
        "״§״³״×״®״¯… ״§״³״×״¹„״§…״§״× …״¹״§…„״© (parameterized queries) ״¨״¯„״§‹ …† ״¯…״¬ ״§„†״µˆ״µ",
        "Use parameterized queries instead of string concatenation",
        {"†‘״°_SQL", "״§״³״×״¹„…", "raw_query", "SELECT.*\\+", "INSERT.*\\+", "DELETE.*\\+", "UPDATE.*\\+"}
    });

    // === SQL002: ״×†״° SQL ״®״§… ===
    rules_.push_back({
        "SQL002", "״§״³״×״¹„״§… ״®״§…", "Raw SQL Query",
        RuleCategory::Injection, Severity::High,
        "״§״³״×״¹„״§… SQL ״®״§… ״¨״¯ˆ† ״­…״§״©",
        "Raw SQL query without protection",
        "״§״³״×״®״¯… ORM ״£ˆ builder ״¨״¯„״§‹ …† ״§„״§״³״×״¹„״§…״§״× ״§„״®״§…",
        "Use ORM or query builder instead of raw queries",
        {"raw_query", "†‘״°_״®״§…"}
    });

    // === BUF001: ״¶ ״§„…״®״²† ===
    rules_.push_back({
        "BUF001", "״¶ ״§„…״®״²† ״§„…״­״×…„", "Potential Buffer Overflow",
        RuleCategory::BufferOverflow, Severity::Critical,
        "״¹…„״§״× ״¹„‰ …״µˆ״§״× ״£ˆ …״®״§״²† ‚״¯ ״×״³״¨״¨ ״¶",
        "Operations on arrays/buffers that may cause overflow",
        "״×״­‚‚ …† ״­״¯ˆ״¯ ״§„…״µˆ״© ‚״¨„ ״§„ˆ״µˆ„",
        "Check array bounds before access",
        {"…״µˆ״©.*\\[.*\\]", "״§״­״µ„_״¹„‰_״¹†״µ״±", "buffer", "memcpy", "strcpy"}
    });

    // === BUF002: ˆ״µˆ„ ״®״§״±״¬ ״§„״­״¯ˆ״¯ ===
    rules_.push_back({
        "BUF002", "ˆ״µˆ„ ״®״§״±״¬ ״§„״­״¯ˆ״¯", "Out-of-bounds Access",
        RuleCategory::BufferOverflow, Severity::High,
        "ˆ״µˆ„ „…״₪״´״± ״«״§״¨״×  …״µˆ״© ״¨״¯ˆ† ״­״µ ״§„״­״¬…",
        "Fixed index access without size check",
        "״§״³״×״®״¯… ״§„״×״§״¨״¹ .״§״­״µ„_״¢…†() ״£ˆ ״×״­‚‚ …† ״§„״·ˆ„ ״£ˆ„״§‹",
        "Use .safe_get() or check length first",
        {"\\[\\d+\\]"}
    });

    // === INP001: ״¥״¯״®״§„ ״¨״¯ˆ† ״×״­‚‚ ===
    rules_.push_back({
        "INP001", "״¥״¯״®״§„ ״¨״¯ˆ† ״×״­‚‚", "Unvalidated Input",
        RuleCategory::InputValidation, Severity::High,
        "״¥״¯״®״§„ ״§„…״³״×״®״¯… ״³״×״®״¯… …״¨״§״´״±״© ״¨״¯ˆ† ״×״­‚‚",
        "User input used directly without validation",
        "״×״­‚‚ …† ״§„״¥״¯״®״§„ ˆ†‚‘‡ ‚״¨„ ״§„״§״³״×״®״¯״§…",
        "Validate and sanitize input before use",
        {"״§‚״±״£\\(", "״§‚״±״£_״³״·״±\\(", "״§״­״µ„_״¹„‰_…״¹״§…„", "input\\("}
    });

    // === INP002: ״¹״¯… ״×״­‚‚ …† ״§„†ˆ״¹ ===
    rules_.push_back({
        "INP002", "״¹״¯… ״§„״×״­‚‚ …† ״§„†ˆ״¹", "Missing Type Validation",
        RuleCategory::InputValidation, Severity::Medium,
        "״×״­ˆ„ †ˆ״¹ ״¨״§†״§״× ״¨״¯ˆ† ״§„״×״­‚‚ …† ״µ„״§״­״×‡",
        "Type conversion without validity check",
        "״§״³״×״®״¯… ״­״§ˆ„/״§…״³ƒ ״­ˆ„ ״×״­ˆ„״§״× ״§„״£†ˆ״§״¹",
        "Wrap type conversions in try/catch",
        {"״­ˆ‘„_״¥„‰_״±‚…", "״­ˆ‘„_״¥„‰_״¹״´״±", "parseInt", "parseFloat"}
    });

    // === CRY001: ״®ˆ״§״±״²…״© ״×״´״± ״¶״¹״© ===
    rules_.push_back({
        "CRY001", "״®ˆ״§״±״²…״© ״×״´״± ״¶״¹״©", "Weak Cryptographic Algorithm",
        RuleCategory::Cryptography, Severity::High,
        "״§״³״×״®״¯״§… ״®ˆ״§״±״²…״© ״×״´״± …״¹״±ˆ״© ״¨״§„״¶״¹",
        "Use of known-weak cryptographic algorithm",
        "״§״³״×״®״¯… SHA-256 ״£ˆ ״£״¹„‰ ״¨״¯„״§‹ …† MD5/SHA1״ ˆ״§״³״×״®״¯… AES ״¨״¯„״§‹ …† DES/RC4",
        "Use SHA-256+ instead of MD5/SHA1, use AES instead of DES/RC4",
        {"MD5", "SHA1", "SHA-1", "DES", "RC4", "md5", "sha1"}
    });

    // === CRY002: …״×״§״­ ״×״´״± ״«״§״¨״× ===
    rules_.push_back({
        "CRY002", "…״×״§״­ ״×״´״± ״«״§״¨״×", "Hardcoded Encryption Key",
        RuleCategory::Cryptography, Severity::Critical,
        "…״×״§״­ ״×״´״± …״¶…‘† …״¨״§״´״±״©  ״§„ƒˆ״¯",
        "Encryption key hardcoded directly in source",
        "״§״³״×״®״¯… …״×״÷״±״§״× ״§„״¨״¦״© ״£ˆ …״®״²† ״§„…״§״×״­",
        "Use environment variables or key vault",
        {"…״×״§״­_״×״´״±.*=.*\"", "encryption_key.*=.*\"", "secret_key.*=.*\""}
    });

    // === SEC001: ƒ״×„״© ״÷״± ״¢…†״© ===
    rules_.push_back({
        "SEC001", "ƒ״×„״© ״÷״± ״¢…†״©", "Unsafe Block",
        RuleCategory::UnsafeBlock, Severity::Medium,
        "״§״³״×״®״¯״§… ƒ״×„״© '״÷״±_״¢…†' ״×״×״¬״§ˆ״² ״­״µ ״§„״£…״§†",
        "Use of 'unsafe' block bypasses safety checks",
        "‚„‘„ ״§״³״×״®״¯״§… ״§„ƒ״×„ ״÷״± ״§„״¢…†״© ˆ״£״¶ ״×״¹„‚״§״× ״×ˆ״¶״­ ״§„״³״¨״¨",
        "Minimize unsafe blocks and document the reason",
        {"״÷״±_״¢…†", "unsafe"}
    });

    // === SEC002: ״×†״° ״£ˆ״§…״± †״¸״§… ===
    rules_.push_back({
        "SEC002", "״×†״° ״£ˆ״§…״± †״¸״§…", "System Command Execution",
        RuleCategory::Injection, Severity::Critical,
        "״×†״° ״£ˆ״§…״± †״¸״§… ‚״¯ ״³…״­ ״¨״­‚† ״§„״£ˆ״§…״±",
        "System command execution may allow command injection",
        "״×״¬†״¨ ״×†״° ״£ˆ״§…״± ״§„†״¸״§…״ ״£ˆ ״§״³״×״®״¯… ‚״§״¦…״© ״¨״¶״§״¡ ״µ״§״±…״© „„״£ˆ״§…״± ״§„…״³…ˆ״­״©",
        "Avoid system commands, or use strict whitelist",
        {"†‘״°\\(", "shell\\(", "system\\(", "exec\\(", "״£…״±_†״¸״§…"}
    });

    // === SEC003: ״£״³״±״§״± ״«״§״¨״×״© ===
    rules_.push_back({
        "SEC003", "״£״³״±״§״± ״«״§״¨״×״©  ״§„ƒˆ״¯", "Hardcoded Secrets",
        RuleCategory::Secrets, Severity::Critical,
        "ƒ„…״© …״±ˆ״± ״£ˆ …״×״§״­ API ״£ˆ ״±…״² ״³״± ״«״§״¨״×  ״§„ƒˆ״¯ ״§„…״µ״¯״±",
        "Password, API key, or secret token hardcoded in source",
        "״§״³״×״®״¯… …״×״÷״±״§״× ״§„״¨״¦״© ״£ˆ …„ .env ״£ˆ …״®״²† ״§„״£״³״±״§״±",
        "Use environment variables, .env file, or secret vault",
        {"password.*=.*\"", "ƒ„…״©_…״±ˆ״±.*=.*\"", "api_key.*=.*\"",
         "…״×״§״­_API.*=.*\"", "token.*=.*\"", "secret.*=.*\"",
         "״±…״².*=.*\"", "״³״±.*=.*\""}
    });

    // === SEC004: ˆ״µˆ„ …„ ״¨״¯ˆ† ״×״­‚‚ ===
    rules_.push_back({
        "SEC004", "ˆ״µˆ„ …„ ״÷״± ״¢…†", "Unsafe File Access",
        RuleCategory::Injection, Severity::High,
        "״×״­ …„ ״¨״§״³״×״®״¯״§… …״³״§״± …† …״¯״®„״§״× ״§„…״³״×״®״¯… ״¨״¯ˆ† ״×״·״¨״¹",
        "Opening file with user-provided path without normalization",
        "״§״³״×״®״¯… ״×״·״¨״¹_…״³״§״±() ˆ״×״­‚‚_…״³״§״±() ‚״¨„ ״×״­ ״§„…„״§״×",
        "Use normalize_path() and validate_path() before opening files",
        {"״§״×״­_…„.*״§‚״±״£", "״§״×״­.*input", "open.*path"}
    });

    // === MEM001: ״×״³״±‘״¨ ״°״§ƒ״±״© …״­״×…„ ===
    rules_.push_back({
        "MEM001", "״×״³״±‘״¨ ״°״§ƒ״±״© …״­״×…„", "Potential Memory Leak",
        RuleCategory::Memory, Severity::Medium,
        "״×״®״µ״µ ״°״§ƒ״±״© ״¨״¯ˆ† ״×״­״±״± ˆ״§״¶״­",
        "Memory allocation without clear deallocation",
        "״§״³״×״®״¯… RAII ״£ˆ ״§„…״₪״´״±״§״× ״§„״°ƒ״©",
        "Use RAII or smart pointers",
        {"״®״µ‘״µ\\(", "alloc\\(", "״¬״¯״¯\\s"}
    });

    // === MEM002: ״§״³״×״®״¯״§… ״¨״¹״¯ ״§„״×״­״±״± ===
    rules_.push_back({
        "MEM002", "״§״³״×״®״¯״§… ״¨״¹״¯ ״§„״×״­״±״±", "Use After Free",
        RuleCategory::Memory, Severity::Critical,
        "״§„ˆ״µˆ„ „״°״§ƒ״±״© ״¨״¹״¯ ״×״­״±״±‡״§",
        "Accessing memory after freeing it",
        "„״§ ״×״³״×״®״¯… …״₪״´״±״§״× ״¨״¹״¯ ״×״­״±״± ״§„״°״§ƒ״±״© ״§„״× ״×״´״± ״¥„‡״§",
        "Don't use pointers after freeing the memory they point to",
        {"״­״±‘״±.*\\n.*״§״³״×״®״¯…", "free.*use"}
    });

    // === PERM001: ״µ„״§״­״§״× ˆ״§״³״¹״© ===
    rules_.push_back({
        "PERM001", "״µ„״§״­״§״× ˆ״§״³״¹״©", "Overly Broad Permissions",
        RuleCategory::Permissions, Severity::Medium,
        "…†״­ ״µ„״§״­״§״× ״£ˆ״³״¹ …† ״§„„״§״²…",
        "Granting broader permissions than necessary",
        "״§״×״¨״¹ …״¨״¯״£ ״§„״­״¯ ״§„״£״¯†‰ …† ״§„״µ„״§״­״§״×",
        "Follow principle of least privilege",
        {"״§„ƒ„", "All", "777", "0777", "chmod.*777"}
    });

    // === XSS001: ״­‚† ״§„†״µˆ״µ ״¹״¨״± ״§„…ˆ״§‚״¹ ===
    rules_.push_back({
        "XSS001", "״­‚† ״¹״¨״± ״§„…ˆ״§‚״¹ (XSS)", "Cross-Site Scripting",
        RuleCategory::Injection, Severity::High,
        "״¥״®״±״§״¬ ״¨״§†״§״× ״§„…״³״×״®״¯…  HTML ״¨״¯ˆ† ״×״±…״²",
        "User data output in HTML without encoding",
        "״§״³״×״®״¯… ״×״±…״²_HTML() ״£ˆ escapeHtml() ‚״¨„ ״¹״±״¶ ״¨״§†״§״× ״§„…״³״×״®״¯…",
        "Use html_encode() or escapeHtml() before rendering user data",
        {"״§ƒ״×״¨_HTML.*\\+", "innerHTML.*=", "document\\.write", "render_template.*\\+"}
    });

    // === XSS002: ״×‚… ƒˆ״¯ ״¯†״§…ƒ ===
    rules_.push_back({
        "XSS002", "״×‚… ƒˆ״¯ ״¯†״§…ƒ", "Dynamic Code Evaluation",
        RuleCategory::Injection, Severity::Critical,
        "״×‚… ƒˆ״¯ …† …״µ״¯״± ״®״§״±״¬ ג€” ״®״·״± ״­‚† ״´״¯״¯",
        "Evaluating code from external source ג€” severe injection risk",
        "״×״¬†״¨ eval/‚‘… ג€” ״§״³״×״®״¯… …״­„„ ״¢…† ״¨״¯„״§‹ …† ״°„ƒ",
        "Avoid eval ג€” use a safe parser instead",
        {"eval\\(", "‚‘…\\(", "Function\\(.*\\+"}
    });

    // === SSRF001: ״×״²ˆ״± ״·„״¨״§״× ״¬״§†״¨ ״§„״®״§״¯… ===
    rules_.push_back({
        "SSRF001", "״×״²ˆ״± ״·„״¨״§״× ״§„״®״§״¯… (SSRF)", "Server-Side Request Forgery",
        RuleCategory::Injection, Severity::High,
        "״¥״±״³״§„ ״·„״¨ HTTP/״´״¨ƒ״© ״¨״§״³״×״®״¯״§… ״¹†ˆ״§† URL …† …״¯״®„״§״× ״§„…״³״×״®״¯…",
        "Sending HTTP/network request using user-provided URL",
        "״×״­‚‚ …† URL ˆ״§…†״¹ ״§„ˆ״µˆ„ „״¹†״§ˆ† ״¯״§״®„״© (127.0.0.1, 10.x, 192.168.x)",
        "Validate URL and block access to internal addresses",
        {"fetch\\(.*\\+", "״¬„״¨\\(.*\\+", "״£״±״³„_״·„״¨\\(.*\\+", "http_get\\(.*\\+",
         "״§״×״µ„\\(.*\\+", "connect\\(.*\\+"}
    });

    // === PATH001: ״§״®״×״±״§‚ ״§„…״³״§״± ===
    rules_.push_back({
        "PATH001", "״§״®״×״±״§‚ ״§„…״³״§״±", "Path Traversal",
        RuleCategory::Injection, Severity::High,
        "״§״³״×״®״¯״§… …״³״§״± …„ …† ״§„…״³״×״®״¯… ‚״¯ ״³…״­ ״¨״§„ˆ״µˆ„ „…„״§״× ״®״§״±״¬ ״§„†״·״§‚",
        "User-provided file path may allow access to files outside scope",
        "״§״³״×״®״¯… ״×״·״¨״¹_…״³״§״±() ˆ…†״¹ '../' ˆ״×״­‚‚ …† ״§„…״³״§״± ״§„״£״³״§״³",
        "Use normalize_path(), block '../', and validate base path",
        {"\\.\\./", "\\.\\.\\\\", "../", "..\\\\"}
    });

    // === DESER001: ״¥„״÷״§״¡ ״×״³„״³„ ״÷״± ״¢…† ===
    rules_.push_back({
        "DESER001", "״¥„״÷״§״¡ ״×״³„״³„ ״÷״± ״¢…†", "Unsafe Deserialization",
        RuleCategory::Injection, Severity::Critical,
        "״¥„״÷״§״¡ ״×״³„״³„ ״¨״§†״§״× …† …״µ״¯״± ״÷״± …ˆ״«ˆ‚ ‚״¯ ״³…״­ ״¨״×†״° ƒˆ״¯",
        "Deserializing untrusted data may allow code execution",
        "״×״­‚‚ …† ״§„״¨״§†״§״× ‚״¨„ ״¥„״÷״§״¡ ״§„״×״³„״³„ ˆ״§״³״×״®״¯… ״µ״÷ …״­״¯ˆ״¯״© (JSON)",
        "Validate data before deserialization and use limited formats (JSON)",
        {"״­„‘„_ƒ״§״¦†\\(", "deserialize\\(", "unpickle\\(", "fromBytes\\(.*input"}
    });

    // === RACE001: ״­״§„״© ״§„״³״¨״§‚ ===
    rules_.push_back({
        "RACE001", "״­״§„״© ״³״¨״§‚ …״­״×…„״©", "Potential Race Condition",
        RuleCategory::Memory, Severity::Medium,
        "ˆ״µˆ„ „…״×״÷״± …״´״×״±ƒ ״¨״¯ˆ† ‚„  ״³״§‚ …״×״¹״¯״¯ ״§„״®ˆ״·",
        "Shared variable access without lock in multi-threaded context",
        "״§״³״×״®״¯… ‚„() ״£ˆ mutex „״­…״§״© ״§„ˆ״µˆ„ ״§„…״´״×״±ƒ",
        "Use lock/mutex to protect shared access",
        {"…״´״×״±ƒ.*=", "shared.*=.*thread", "global.*״®״·", "״¨״¯ˆ†_‚„"}
    });
}

bool SecurityScanner::isRuleEnabled(const std::string& id) const {
    // ״¥״°״§ ˆ״¬״¯״× ‚״§״¦…״© ״¨״¶״§״¡״ ״¬״¨ ״£† ƒˆ† ‡״§
    if (!config_.enabledRules.empty()) {
        bool found = false;
        for (const auto& r : config_.enabledRules) {
            if (r == id) { found = true; break; }
        }
        if (!found) return false;
    }
    // ״§„‚״§״¦…״© ״§„״³ˆ״¯״§״¡
    for (const auto& r : config_.disabledRules) {
        if (r == id) return false;
    }
    return true;
}

bool SecurityScanner::shouldExcludeLine(const std::string& line) const {
    // ״×״¹„‚״§״× ״§„״×״¬״§‡„: // noguard, // ״­״§״±״³:״×״¬״§‡„
    for (const auto& ex : config_.excludeLines) {
        if (line.find(ex) != std::string::npos) return true;
    }
    if (line.find("// noguard") != std::string::npos) return true;
    if (line.find("# noguard") != std::string::npos) return true;
    if (line.find("״­״§״±״³:״×״¬״§‡„") != std::string::npos) return true;
    return false;
}

void SecurityScanner::addCustomRule(const SecurityRule& rule) {
    rules_.push_back(rule);
}

// ============================================================================
// Scanning
// ============================================================================

ScanResult SecurityScanner::scanFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        ScanResult r;
        r.findings.push_back({"ERR", Severity::Info, RuleCategory::Configuration,
            "„״§ …ƒ† ״×״­ ״§„…„: " + path, path, 0, 0, "", "", "", 1.0, false, ""});
        return r;
    }
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return scanSource(source, path);
}

ScanResult SecurityScanner::scanDirectory(const std::string& dir, bool recursive) {
    ScanResult total;
    auto startTime = std::chrono::high_resolution_clock::now();

    try {
        auto scan = [&](const fs::path& p) {
            std::string ext = p.extension().string();
            if (ext != ".״µ" && ext != ".sad") return;
            for (const auto& pat : config_.excludePatterns) {
                if (p.string().find(pat) != std::string::npos) return;
            }
            auto r = scanFile(p.string());
            total.filesScanned += r.filesScanned;
            total.totalLines += r.totalLines;
            for (auto& f : r.findings) total.findings.push_back(std::move(f));
        };

        if (recursive) {
            for (const auto& entry : fs::recursive_directory_iterator(dir)) {
                if (entry.is_regular_file()) scan(entry.path());
            }
        } else {
            for (const auto& entry : fs::directory_iterator(dir)) {
                if (entry.is_regular_file()) scan(entry.path());
            }
        }
    } catch (const fs::filesystem_error& e) {
        total.findings.push_back({"ERR", Severity::Info, RuleCategory::Configuration,
            std::string("״®״·״£  ‚״±״§״¡״© ״§„…״¬„״¯: ") + e.what(), dir, 0, 0, "", "", "", 1.0, false, ""});
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    total.durationSeconds = std::chrono::duration<double>(endTime - startTime).count();
    return total;
}

ScanResult SecurityScanner::scanSource(const std::string& source, const std::string& filename) {
    ScanResult result;
    auto startTime = std::chrono::high_resolution_clock::now();

    result.filesScanned = 1;

    // ״¹״¯ ״§„״£״³״·״±
    int lineCount = 1;
    for (char c : source) if (c == '\n') lineCount++;
    result.totalLines = lineCount;

    if (config_.patternScan) {
        patternScan(source, filename, result.findings);
    }

    if (config_.taintAnalysis) {
        taintAnalysis(source, filename, result.findings);
    }

    // ״×״±״×״¨ ״§„†״×״§״¦״¬ ״­״³״¨ ״§„״®״·ˆ״±״© ״«… ״§„״³״·״±
    std::sort(result.findings.begin(), result.findings.end(),
        [](const Finding& a, const Finding& b) {
            if (a.severity != b.severity) return (int)a.severity < (int)b.severity;
            return a.line < b.line;
        });

    // „״×״±״© ״­״³״¨ ״§„״­״¯ ״§„״£״¯†‰
    if (config_.criticalOnly) {
        result.findings.erase(
            std::remove_if(result.findings.begin(), result.findings.end(),
                [](const Finding& f) { return f.severity != Severity::Critical; }),
            result.findings.end());
    } else {
        result.findings.erase(
            std::remove_if(result.findings.begin(), result.findings.end(),
                [this](const Finding& f) { return (int)f.severity > (int)config_.minSeverity; }),
            result.findings.end());
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    result.durationSeconds = std::chrono::duration<double>(endTime - startTime).count();
    return result;
}

// ============================================================================
// Pattern-based scanning
// ============================================================================


} // namespace Security
} // namespace Sad

