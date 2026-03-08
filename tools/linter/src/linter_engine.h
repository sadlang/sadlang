/**
 * =============================================================================
 * ملف: linter_engine.h
 * الوصف: محرك التحليل الثابت الأساسي للغة ص
 * (AR) @brief محرك Linter الرئيسي — تحميل القواعد وتشغيل التحليل
 * (EN) @brief Main Linter engine — rule loading and analysis execution
 * المهمة: T303 - Linter infrastructure
 * =============================================================================
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>

namespace sad::linter {

// =============================================================================
// مستوى خطورة التحذير
// =============================================================================

enum class Severity {
    Error,      // خطأ — يجب إصلاحه
    Warning,    // تحذير — يُفضل إصلاحه
    Info,       // معلومة — اقتراح تحسين
    Hint        // تلميح — أسلوب أفضل
};

/**
 * (AR) تحويل المستوى إلى نص
 * (EN) Convert severity to string
 */
inline std::string severityToString(Severity s) {
    switch (s) {
        case Severity::Error:   return "خطأ";
        case Severity::Warning: return "تحذير";
        case Severity::Info:    return "معلومة";
        case Severity::Hint:    return "تلميح";
    }
    return "مجهول";
}

inline std::string severityToEmoji(Severity s) {
    switch (s) {
        case Severity::Error:   return "❌";
        case Severity::Warning: return "⚠️";
        case Severity::Info:    return "ℹ️";
        case Severity::Hint:    return "💡";
    }
    return "?";
}

// =============================================================================
// فئة القاعدة
// =============================================================================

enum class RuleCategory {
    CodeQuality,    // جودة الكود
    Style,          // الأسلوب
    Performance,    // الأداء
    Security,       // الأمان
    BestPractice,   // أفضل ممارسات
    Naming,         // التسمية
    Complexity,     // التعقيد
    Unused,         // غير مستخدم
    ErrorProne,     // عرضة للأخطاء
    Maintainability // قابلية الصيانة
};

inline std::string categoryToString(RuleCategory c) {
    switch (c) {
        case RuleCategory::CodeQuality:    return "جودة_الكود";
        case RuleCategory::Style:          return "الأسلوب";
        case RuleCategory::Performance:    return "الأداء";
        case RuleCategory::Security:       return "الأمان";
        case RuleCategory::BestPractice:   return "أفضل_ممارسات";
        case RuleCategory::Naming:         return "التسمية";
        case RuleCategory::Complexity:     return "التعقيد";
        case RuleCategory::Unused:         return "غير_مستخدم";
        case RuleCategory::ErrorProne:     return "عرضة_للأخطاء";
        case RuleCategory::Maintainability:return "قابلية_الصيانة";
    }
    return "أخرى";
}

// =============================================================================
// نتيجة التحليل (Diagnostic)
// =============================================================================

struct LintDiagnostic {
    std::string ruleId;         // معرف القاعدة: "SAD001"
    std::string message;        // رسالة التحذير
    std::string suggestion;     // اقتراح الإصلاح
    Severity severity;
    RuleCategory category;
    std::string filePath;
    int line;                   // رقم السطر (1-based)
    int column;                 // رقم العمود (1-based)
    int endLine = -1;           // نهاية السطر
    int endColumn = -1;         // نهاية العمود
    
    std::string toString() const {
        return severityToEmoji(severity) + " " + filePath + ":" +
               std::to_string(line) + ":" + std::to_string(column) +
               " [" + ruleId + "] " + message;
    }
};

// =============================================================================
// سياق التحليل
// =============================================================================

/**
 * (AR) معلومات عن رمز في الكود المصدري
 * (EN) Information about a token in source code
 */
struct SourceToken {
    std::string text;
    std::string type;   // "IDENTIFIER", "KEYWORD", "STRING", etc.
    int line;
    int column;
};

/**
 * (AR) سياق الملف الذي يتم تحليله
 * (EN) Context of the file being analyzed
 */
struct LintContext {
    std::string filePath;
    std::vector<std::string> lines;     // أسطر الملف
    std::vector<SourceToken> tokens;    // الرموز (إن توفرت)
    
    // معلومات محسوبة
    std::unordered_set<std::string> declaredVars;    // متغيرات مُعلنة
    std::unordered_set<std::string> usedVars;        // متغيرات مُستخدمة
    std::unordered_set<std::string> declaredFuncs;   // دوال مُعلنة
    std::unordered_set<std::string> calledFuncs;     // دوال مُستدعاة
    std::unordered_set<std::string> importedModules; // وحدات مستوردة
    
    int totalLines() const { return static_cast<int>(lines.size()); }
};

// =============================================================================
// القاعدة الأساسية
// =============================================================================

/**
 * (AR) الصنف الأساسي لقواعد التحليل
 * (EN) Base class for lint rules
 */
class LintRule {
public:
    virtual ~LintRule() = default;
    
    virtual std::string getId() const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual Severity getDefaultSeverity() const = 0;
    virtual RuleCategory getCategory() const = 0;
    
    /**
     * (AR) تنفيذ القاعدة على سياق الملف
     * (EN) Execute rule on file context
     */
    virtual std::vector<LintDiagnostic> check(const LintContext& ctx) = 0;
    
    bool isEnabled() const { return enabled_; }
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    Severity getSeverity() const { return overrideSeverity_.value_or(getDefaultSeverity()); }
    void setSeverity(Severity s) { overrideSeverity_ = s; }

protected:
    LintDiagnostic makeDiag(const LintContext& ctx, int line, int col,
                           const std::string& msg,
                           const std::string& suggestion = "") {
        LintDiagnostic d;
        d.ruleId = getId();
        d.message = msg;
        d.suggestion = suggestion;
        d.severity = getSeverity();
        d.category = getCategory();
        d.filePath = ctx.filePath;
        d.line = line;
        d.column = col;
        return d;
    }
    
private:
    bool enabled_ = true;
    std::optional<Severity> overrideSeverity_;
};

// =============================================================================
// محرك التحليل
// =============================================================================

/**
 * (AR) محرك التحليل الثابت
 * (EN) Static analysis engine
 */
class LinterEngine {
public:
    LinterEngine() = default;
    
    /**
     * (AR) تسجيل قاعدة
     * (EN) Register a rule
     */
    void registerRule(std::unique_ptr<LintRule> rule) {
        auto id = rule->getId();
        rules_[id] = std::move(rule);
    }
    
    /**
     * (AR) تحليل ملف واحد
     * (EN) Analyze a single file
     */
    std::vector<LintDiagnostic> analyzeFile(const std::string& filePath,
                                           const std::string& content) {
        LintContext ctx;
        ctx.filePath = filePath;
        
        // تقسيم إلى أسطر
        std::string line;
        for (char c : content) {
            if (c == '\n') {
                ctx.lines.push_back(line);
                line.clear();
            } else {
                line += c;
            }
        }
        if (!line.empty()) ctx.lines.push_back(line);
        
        // تحليل أولي (اكتشاف المتغيرات والدوال)
        preAnalyze(ctx);
        
        // تشغيل جميع القواعد
        std::vector<LintDiagnostic> results;
        for (auto& [id, rule] : rules_) {
            if (!rule->isEnabled()) continue;
            auto diags = rule->check(ctx);
            results.insert(results.end(), diags.begin(), diags.end());
        }
        
        // ترتيب حسب السطر
        std::sort(results.begin(), results.end(),
            [](const LintDiagnostic& a, const LintDiagnostic& b) {
                return a.line < b.line || (a.line == b.line && a.column < b.column);
            });
        
        return results;
    }
    
    /**
     * (AR) تعطيل/تفعيل قاعدة
     * (EN) Enable/disable a rule
     */
    void setRuleEnabled(const std::string& ruleId, bool enabled) {
        auto it = rules_.find(ruleId);
        if (it != rules_.end()) {
            it->second->setEnabled(enabled);
        }
    }
    
    /**
     * (AR) الحصول على جميع القواعد المسجلة
     * (EN) Get all registered rules
     */
    std::vector<const LintRule*> getAllRules() const {
        std::vector<const LintRule*> result;
        for (const auto& [id, rule] : rules_) {
            result.push_back(rule.get());
        }
        return result;
    }
    
    /**
     * (AR) إنشاء المحرك مع جميع القواعد الافتراضية
     * (EN) Create engine with all default rules
     */
    static std::unique_ptr<LinterEngine> createDefault();

private:
    std::unordered_map<std::string, std::unique_ptr<LintRule>> rules_;
    
    /**
     * (AR) تحليل أولي — اكتشاف المتغيرات والدوال
     * (EN) Pre-analysis — discover variables and functions
     */
    void preAnalyze(LintContext& ctx) {
        for (int i = 0; i < static_cast<int>(ctx.lines.size()); i++) {
            const auto& line = ctx.lines[i];
            
            // كشف تعريف متغير: "متغير اسم ="
            auto varPos = line.find("متغير ");
            if (varPos != std::string::npos) {
                auto rest = line.substr(varPos + std::string("متغير ").size());
                auto eqPos = rest.find('=');
                auto spPos = rest.find(' ');
                auto nameEnd = std::min(eqPos, spPos);
                if (nameEnd != std::string::npos) {
                    auto varName = rest.substr(0, nameEnd);
                    // تقليم المسافات
                    while (!varName.empty() && varName.back() == ' ') varName.pop_back();
                    if (!varName.empty()) ctx.declaredVars.insert(varName);
                }
            }
            
            // كشف ثابت
            auto constPos = line.find("ثابت ");
            if (constPos != std::string::npos) {
                auto rest = line.substr(constPos + std::string("ثابت ").size());
                auto eqPos = rest.find('=');
                if (eqPos != std::string::npos) {
                    auto name = rest.substr(0, eqPos);
                    while (!name.empty() && name.back() == ' ') name.pop_back();
                    if (!name.empty()) ctx.declaredVars.insert(name);
                }
            }
            
            // كشف دالة
            auto funcPos = line.find("دالة ");
            if (funcPos != std::string::npos) {
                auto rest = line.substr(funcPos + std::string("دالة ").size());
                auto parenPos = rest.find('(');
                if (parenPos != std::string::npos) {
                    auto funcName = rest.substr(0, parenPos);
                    while (!funcName.empty() && funcName.back() == ' ') funcName.pop_back();
                    if (!funcName.empty()) ctx.declaredFuncs.insert(funcName);
                }
            }
            
            // كشف استيراد
            auto importPos = line.find("استورد ");
            if (importPos != std::string::npos) {
                ctx.importedModules.insert(line);
            }
        }
    }
};

} // namespace sad::linter
