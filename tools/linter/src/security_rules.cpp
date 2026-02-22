/**
 * =============================================================================
 * ملف: security_rules.cpp
 * الوصف: إطار قواعد الأمان في لغة ص
 * المهمة: T218 - Security lint rules framework
 * المرحلة: Phase 22 - User Story 19 (Security Linting ص حارس)
 * =============================================================================
 * 
 * 🔒 دليل المبتدئ للأمان البرمجي
 * ══════════════════════════════
 * 
 * ما هو الأمان البرمجي؟
 * ─────────────────────
 * الأمان البرمجي يعني كتابة كود لا يمكن للمهاجمين استغلاله.
 * 
 * تخيل أنك تبني منزلاً:
 * - الباب الأمامي = واجهة البرنامج (API)
 * - النوافذ = المدخلات التي يقدمها المستخدم
 * - الأقفال = التحقق من صحة البيانات
 * 
 * إذا تركت نافذة مفتوحة (لم تتحقق من المدخلات)،
 * يمكن للسارق (المهاجم) الدخول!
 * 
 * أنواع الثغرات الأمنية الشائعة:
 * ───────────────────────────────
 * 
 * 1. SQL Injection (حقن SQL)
 *    ────────────────────────
 *    عندما يدخل المستخدم نصاً يحتوي على أوامر SQL
 *    
 *    مثال خطير:
 *    ```
 *    اسم_المستخدم = "أحمد' OR '1'='1"
 *    استعلام("SELECT * FROM users WHERE name='" + اسم_المستخدم + "'")
 *    ```
 *    هذا يُرجع كل المستخدمين بدلاً من أحمد فقط!
 * 
 * 2. Buffer Overflow (طفح المخزن المؤقت)
 *    ─────────────────────────────────────
 *    عندما تكتب بيانات أكثر من المساحة المخصصة
 *    
 *    مثل محاولة وضع 10 تفاحات في صندوق يسع 5 فقط!
 * 
 * 3. XSS (Cross-Site Scripting)
 *    ────────────────────────────
 *    عندما يدخل المستخدم كود JavaScript خبيث
 *    
 *    مثال: <script>سرقة_كلمة_السر()</script>
 * 
 * كيف يعمل حارس ص؟
 * ─────────────────
 * 1. يقرأ الكود المصدري
 * 2. يبحث عن أنماط خطيرة
 * 3. يُنبه المبرمج قبل أن تصبح مشكلة
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <regex>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <set>

namespace sad::security {

// =============================================================================
// مستويات الخطورة (Severity Levels)
// =============================================================================

/**
 * مستوى خطورة المشكلة الأمنية
 * 
 * للمبتدئين:
 * تخيل إشارات المرور:
 * - Critical = أحمر ساطع → توقف فوراً!
 * - High = أحمر → خطر كبير
 * - Medium = برتقالي → انتبه
 * - Low = أصفر → تحسين مستحب
 * - Info = أخضر → معلومة فقط
 */
enum class Severity {
    Critical,   // حرج - يجب إصلاحه فوراً (مثل تسريب كلمة سر)
    High,       // عالي - خطر أمني كبير
    Medium,     // متوسط - مشكلة يجب معالجتها
    Low,        // منخفض - تحسين مستحب
    Info        // معلومة - ليس خطراً لكن جيد معرفته
};

/**
 * تحويل مستوى الخطورة لنص عربي
 */
inline std::string severityToArabic(Severity s) {
    switch (s) {
        case Severity::Critical: return "🔴 حرج";
        case Severity::High:     return "🟠 عالي";
        case Severity::Medium:   return "🟡 متوسط";
        case Severity::Low:      return "🟢 منخفض";
        case Severity::Info:     return "ℹ️ معلومة";
    }
    return "غير معروف";
}

inline std::string severityToEnglish(Severity s) {
    switch (s) {
        case Severity::Critical: return "CRITICAL";
        case Severity::High:     return "HIGH";
        case Severity::Medium:   return "MEDIUM";
        case Severity::Low:      return "LOW";
        case Severity::Info:     return "INFO";
    }
    return "UNKNOWN";
}

// =============================================================================
// فئات الثغرات (Vulnerability Categories)
// =============================================================================

/**
 * فئة الثغرة الأمنية
 * 
 * للمبتدئين:
 * مثل أنواع الأمراض في الطب:
 * - كل نوع له أعراض مختلفة
 * - كل نوع له علاج مختلف
 */
enum class VulnerabilityCategory {
    Injection,          // حقن (SQL, Command, etc.)
    BufferOverflow,     // طفح المخزن المؤقت
    CryptoMisuse,       // سوء استخدام التشفير
    InputValidation,    // عدم التحقق من المدخلات
    AccessControl,      // مشاكل التحكم بالوصول
    InformationLeak,    // تسريب معلومات
    UnsafeCode,         // كود غير آمن
    MemorySafety,       // أمان الذاكرة
    Concurrency,        // مشاكل التزامن
    Configuration       // إعدادات خاطئة
};

inline std::string categoryToArabic(VulnerabilityCategory c) {
    switch (c) {
        case VulnerabilityCategory::Injection:       return "حقن";
        case VulnerabilityCategory::BufferOverflow:  return "طفح المخزن";
        case VulnerabilityCategory::CryptoMisuse:    return "سوء استخدام التشفير";
        case VulnerabilityCategory::InputValidation: return "التحقق من المدخلات";
        case VulnerabilityCategory::AccessControl:   return "التحكم بالوصول";
        case VulnerabilityCategory::InformationLeak: return "تسريب معلومات";
        case VulnerabilityCategory::UnsafeCode:      return "كود غير آمن";
        case VulnerabilityCategory::MemorySafety:    return "أمان الذاكرة";
        case VulnerabilityCategory::Concurrency:     return "مشاكل التزامن";
        case VulnerabilityCategory::Configuration:   return "إعدادات خاطئة";
    }
    return "غير معروف";
}

// =============================================================================
// موقع الكود (Source Location)
// =============================================================================

/**
 * موقع في الكود المصدري
 * 
 * للمبتدئين:
 * مثل عنوان في كتاب:
 * - الملف = اسم الكتاب
 * - السطر = رقم الصفحة
 * - العمود = الكلمة في السطر
 */
struct SourceLocation {
    std::string file;       // اسم الملف
    int line = 0;           // رقم السطر (يبدأ من 1)
    int column = 0;         // رقم العمود (يبدأ من 1)
    int endLine = 0;        // سطر النهاية
    int endColumn = 0;      // عمود النهاية
    
    std::string toString() const {
        std::ostringstream ss;
        ss << file << ":" << line << ":" << column;
        return ss.str();
    }
};

// =============================================================================
// نتيجة الفحص (Finding)
// =============================================================================

/**
 * نتيجة فحص أمني
 * 
 * للمبتدئين:
 * مثل تقرير طبي:
 * - اسم المرض (نوع الثغرة)
 * - مكانه (موقع الكود)
 * - خطورته (مستوى الخطر)
 * - العلاج (كيفية الإصلاح)
 */
struct SecurityFinding {
    std::string ruleId;             // معرف القاعدة (مثل: SEC001)
    std::string ruleName;           // اسم القاعدة
    VulnerabilityCategory category; // فئة الثغرة
    Severity severity;              // مستوى الخطورة
    SourceLocation location;        // موقع المشكلة
    std::string message;            // رسالة الخطأ
    std::string description;        // شرح تفصيلي
    std::string codeSnippet;        // مقتطف الكود المشكل
    std::string suggestion;         // اقتراح الإصلاح
    std::string fixedCode;          // الكود المُصحح
    std::vector<std::string> references; // مراجع إضافية
    
    /**
     * طباعة النتيجة بشكل منسق
     */
    std::string format(bool arabic = true) const {
        std::ostringstream ss;
        
        if (arabic) {
            ss << "\n╔══════════════════════════════════════════════════════════════╗\n";
            ss << "║  " << severityToArabic(severity) << " - " << ruleName << "\n";
            ss << "╠══════════════════════════════════════════════════════════════╣\n";
            ss << "║  📍 الموقع: " << location.toString() << "\n";
            ss << "║  📂 الفئة: " << categoryToArabic(category) << "\n";
            ss << "║  🔑 المعرف: " << ruleId << "\n";
            ss << "╠══════════════════════════════════════════════════════════════╣\n";
            ss << "║  ❌ المشكلة:\n";
            ss << "║     " << message << "\n";
            if (!codeSnippet.empty()) {
                ss << "╠──────────────────────────────────────────────────────────────╣\n";
                ss << "║  📝 الكود المشكل:\n";
                ss << "║     " << codeSnippet << "\n";
            }
            if (!suggestion.empty()) {
                ss << "╠──────────────────────────────────────────────────────────────╣\n";
                ss << "║  ✅ الحل المقترح:\n";
                ss << "║     " << suggestion << "\n";
            }
            if (!fixedCode.empty()) {
                ss << "║  📝 الكود الصحيح:\n";
                ss << "║     " << fixedCode << "\n";
            }
            ss << "╚══════════════════════════════════════════════════════════════╝\n";
        } else {
            ss << "\n[" << severityToEnglish(severity) << "] " << ruleName << "\n";
            ss << "  Location: " << location.toString() << "\n";
            ss << "  Rule ID: " << ruleId << "\n";
            ss << "  Issue: " << message << "\n";
            if (!suggestion.empty()) {
                ss << "  Fix: " << suggestion << "\n";
            }
        }
        
        return ss.str();
    }
};

// =============================================================================
// واجهة قاعدة الأمان (Security Rule Interface)
// =============================================================================

/**
 * واجهة قاعدة الأمان الأساسية
 * 
 * للمبتدئين:
 * كل قاعدة مثل مفتش:
 * - له تخصص (نوع الثغرة)
 * - يفحص الكود
 * - يُبلغ عن المشاكل
 */
class SecurityRule {
public:
    virtual ~SecurityRule() = default;
    
    /**
     * الحصول على معرف القاعدة
     * مثل: SEC001, SEC002
     */
    virtual std::string getId() const = 0;
    
    /**
     * الحصول على اسم القاعدة
     */
    virtual std::string getName() const = 0;
    
    /**
     * الحصول على وصف القاعدة
     */
    virtual std::string getDescription() const = 0;
    
    /**
     * الحصول على فئة الثغرة
     */
    virtual VulnerabilityCategory getCategory() const = 0;
    
    /**
     * الحصول على مستوى الخطورة الافتراضي
     */
    virtual Severity getDefaultSeverity() const = 0;
    
    /**
     * هل القاعدة مفعلة افتراضياً؟
     */
    virtual bool isEnabledByDefault() const { return true; }
    
    /**
     * فحص سطر من الكود
     * 
     * @param line محتوى السطر
     * @param lineNumber رقم السطر
     * @param filePath مسار الملف
     * @return قائمة بالنتائج (قد تكون فارغة)
     */
    virtual std::vector<SecurityFinding> checkLine(
        const std::string& line,
        int lineNumber,
        const std::string& filePath
    ) = 0;
    
    /**
     * فحص ملف كامل
     * 
     * @param content محتوى الملف
     * @param filePath مسار الملف
     * @return قائمة بالنتائج
     */
    virtual std::vector<SecurityFinding> checkFile(
        const std::string& content,
        const std::string& filePath
    ) {
        std::vector<SecurityFinding> results;
        std::istringstream stream(content);
        std::string line;
        int lineNumber = 0;
        
        while (std::getline(stream, line)) {
            lineNumber++;
            auto findings = checkLine(line, lineNumber, filePath);
            results.insert(results.end(), findings.begin(), findings.end());
        }
        
        return results;
    }
    
protected:
    /**
     * إنشاء نتيجة فحص
     */
    SecurityFinding createFinding(
        const SourceLocation& loc,
        const std::string& message,
        const std::string& codeSnippet = "",
        const std::string& suggestion = "",
        const std::string& fixedCode = ""
    ) {
        SecurityFinding f;
        f.ruleId = getId();
        f.ruleName = getName();
        f.category = getCategory();
        f.severity = getDefaultSeverity();
        f.location = loc;
        f.message = message;
        f.description = getDescription();
        f.codeSnippet = codeSnippet;
        f.suggestion = suggestion;
        f.fixedCode = fixedCode;
        return f;
    }
};

// =============================================================================
// مدير القواعد (Rule Manager)
// =============================================================================

/**
 * مدير قواعد الأمان
 * 
 * للمبتدئين:
 * مثل مدير فريق المفتشين:
 * - يُسجل المفتشين (القواعد)
 * - يُوزع العمل عليهم
 * - يجمع التقارير منهم
 */
class SecurityRuleManager {
public:
    /**
     * الحصول على النسخة الوحيدة (Singleton)
     */
    static SecurityRuleManager& getInstance() {
        static SecurityRuleManager instance;
        return instance;
    }
    
    /**
     * تسجيل قاعدة جديدة
     */
    void registerRule(std::unique_ptr<SecurityRule> rule) {
        std::string id = rule->getId();
        rules_[id] = std::move(rule);
        
        if (rules_[id]->isEnabledByDefault()) {
            enabledRules_.insert(id);
        }
    }
    
    /**
     * تفعيل قاعدة
     */
    void enableRule(const std::string& id) {
        if (rules_.count(id)) {
            enabledRules_.insert(id);
        }
    }
    
    /**
     * تعطيل قاعدة
     */
    void disableRule(const std::string& id) {
        enabledRules_.erase(id);
    }
    
    /**
     * تفعيل جميع القواعد
     */
    void enableAll() {
        for (const auto& [id, rule] : rules_) {
            enabledRules_.insert(id);
        }
    }
    
    /**
     * تعطيل جميع القواعد
     */
    void disableAll() {
        enabledRules_.clear();
    }
    
    /**
     * فحص ملف
     */
    std::vector<SecurityFinding> checkFile(
        const std::string& content,
        const std::string& filePath
    ) {
        std::vector<SecurityFinding> allFindings;
        
        for (const auto& id : enabledRules_) {
            auto it = rules_.find(id);
            if (it != rules_.end()) {
                auto findings = it->second->checkFile(content, filePath);
                allFindings.insert(allFindings.end(), 
                                  findings.begin(), findings.end());
            }
        }
        
        // ترتيب حسب الخطورة
        std::sort(allFindings.begin(), allFindings.end(),
            [](const SecurityFinding& a, const SecurityFinding& b) {
                return static_cast<int>(a.severity) < static_cast<int>(b.severity);
            });
        
        return allFindings;
    }
    
    /**
     * الحصول على قائمة القواعد المسجلة
     */
    std::vector<std::string> getRegisteredRules() const {
        std::vector<std::string> ids;
        for (const auto& [id, rule] : rules_) {
            ids.push_back(id);
        }
        return ids;
    }
    
    /**
     * الحصول على قائمة القواعد المفعلة
     */
    std::set<std::string> getEnabledRules() const {
        return enabledRules_;
    }
    
    /**
     * الحصول على قاعدة بمعرفها
     */
    SecurityRule* getRule(const std::string& id) {
        auto it = rules_.find(id);
        return it != rules_.end() ? it->second.get() : nullptr;
    }
    
private:
    SecurityRuleManager() = default;
    
    std::map<std::string, std::unique_ptr<SecurityRule>> rules_;
    std::set<std::string> enabledRules_;
};

// =============================================================================
// إعدادات الفحص (Scan Configuration)
// =============================================================================

/**
 * إعدادات فحص الأمان
 */
struct ScanConfig {
    std::vector<std::string> enabledRules;  // قواعد مفعلة (فارغ = الكل)
    std::vector<std::string> disabledRules; // قواعد معطلة
    Severity minSeverity = Severity::Info;  // الحد الأدنى للخطورة
    bool includeInfo = true;                // تضمين المعلومات
    bool arabicOutput = true;               // إخراج عربي
    bool showFixedCode = true;              // عرض الكود المُصحح
    std::vector<std::string> ignorePaths;   // مسارات مستثناة
    std::vector<std::string> ignorePatterns; // أنماط مستثناة
};

// =============================================================================
// قاعدة: كشف الكود غير الآمن (Unsafe Code Detector)
// =============================================================================

/**
 * قاعدة كشف استخدام غير_آمن
 */
class UnsafeBlockRule : public SecurityRule {
public:
    std::string getId() const override { return "SEC001"; }
    std::string getName() const override { return "كتلة غير آمنة"; }
    std::string getDescription() const override {
        return "يكشف استخدام كتل 'غير_آمن' التي تتجاوز فحوصات الأمان";
    }
    VulnerabilityCategory getCategory() const override {
        return VulnerabilityCategory::UnsafeCode;
    }
    Severity getDefaultSeverity() const override { return Severity::High; }
    
    std::vector<SecurityFinding> checkLine(
        const std::string& line,
        int lineNumber,
        const std::string& filePath
    ) override {
        std::vector<SecurityFinding> findings;
        
        // البحث عن غير_آمن (unsafe)
        std::regex unsafePattern(R"(\bغير_آمن\s*\{|\bunsafe\s*\{)");
        std::smatch match;
        
        if (std::regex_search(line, match, unsafePattern)) {
            SourceLocation loc{filePath, lineNumber, 
                              static_cast<int>(match.position()) + 1, 
                              lineNumber, 
                              static_cast<int>(match.position() + match.length())};
            
            auto finding = createFinding(
                loc,
                "استخدام كتلة 'غير_آمن' يتجاوز فحوصات الأمان",
                line,
                "تأكد من أن استخدام 'غير_آمن' ضروري فعلاً",
                "# أضف تعليقاً يشرح سبب الحاجة للكود غير الآمن"
            );
            findings.push_back(finding);
        }
        
        return findings;
    }
};

// =============================================================================
// قاعدة: كشف مؤشرات خام (Raw Pointer Detector)
// =============================================================================

/**
 * قاعدة كشف المؤشرات الخام
 */
class RawPointerRule : public SecurityRule {
public:
    std::string getId() const override { return "SEC002"; }
    std::string getName() const override { return "مؤشر خام"; }
    std::string getDescription() const override {
        return "يكشف استخدام المؤشرات الخام التي قد تسبب مشاكل ذاكرة";
    }
    VulnerabilityCategory getCategory() const override {
        return VulnerabilityCategory::MemorySafety;
    }
    Severity getDefaultSeverity() const override { return Severity::Medium; }
    
    std::vector<SecurityFinding> checkLine(
        const std::string& line,
        int lineNumber,
        const std::string& filePath
    ) override {
        std::vector<SecurityFinding> findings;
        
        // البحث عن *مؤشر (raw pointer patterns)
        std::regex ptrPattern(R"(\*\s*متغير|\*\s*ثابت|مؤشر_خام|raw_ptr)");
        std::smatch match;
        
        if (std::regex_search(line, match, ptrPattern)) {
            SourceLocation loc{filePath, lineNumber,
                              static_cast<int>(match.position()) + 1,
                              lineNumber,
                              static_cast<int>(match.position() + match.length())};
            
            auto finding = createFinding(
                loc,
                "استخدام مؤشر خام قد يسبب مشاكل في الذاكرة",
                line,
                "استخدم مؤشرات ذكية (ذكي<T>) أو مراجع (&T) بدلاً من المؤشرات الخام",
                "# مثال: بدلاً من *متغير استخدم &متغير أو ذكي<نوع>"
            );
            findings.push_back(finding);
        }
        
        return findings;
    }
};

// =============================================================================
// قاعدة: كشف تسريب معلومات حساسة
// =============================================================================

/**
 * قاعدة كشف تسريب المعلومات الحساسة
 */
class SensitiveDataLeakRule : public SecurityRule {
public:
    std::string getId() const override { return "SEC003"; }
    std::string getName() const override { return "تسريب معلومات حساسة"; }
    std::string getDescription() const override {
        return "يكشف طباعة أو تسجيل معلومات حساسة مثل كلمات السر";
    }
    VulnerabilityCategory getCategory() const override {
        return VulnerabilityCategory::InformationLeak;
    }
    Severity getDefaultSeverity() const override { return Severity::Critical; }
    
    std::vector<SecurityFinding> checkLine(
        const std::string& line,
        int lineNumber,
        const std::string& filePath
    ) override {
        std::vector<SecurityFinding> findings;
        
        // كلمات حساسة
        std::vector<std::string> sensitiveWords = {
            "كلمة_السر", "كلمة_المرور", "password", "passwd",
            "سر", "secret", "مفتاح_سري", "api_key", "token",
            "رمز_الوصول", "access_token", "private_key"
        };
        
        // أنماط الطباعة
        std::regex printPattern(R"(اطبع|طباعة|print|log|سجل|console)");
        
        if (std::regex_search(line, printPattern)) {
            for (const auto& word : sensitiveWords) {
                if (line.find(word) != std::string::npos) {
                    SourceLocation loc{filePath, lineNumber, 1, lineNumber, 
                                      static_cast<int>(line.length())};
                    
                    auto finding = createFinding(
                        loc,
                        "طباعة معلومات حساسة (" + word + ") قد تُسرب بيانات مهمة",
                        line,
                        "لا تطبع كلمات السر أو المفاتيح السرية. استخدم [مخفي] بدلاً منها",
                        "اطبع(\"كلمة السر: [مخفي]\")"
                    );
                    findings.push_back(finding);
                    break; // واحدة تكفي لكل سطر
                }
            }
        }
        
        return findings;
    }
};

// =============================================================================
// قاعدة: كشف أرقام سحرية (Magic Numbers)
// =============================================================================

/**
 * قاعدة كشف الأرقام السحرية
 * 
 * للمبتدئين:
 * الأرقام السحرية هي أرقام في الكود بدون شرح
 * مثل: إذا x > 42  ← ماذا يعني 42؟
 * الأفضل: إذا x > الحد_الأقصى  ← واضح!
 */
class MagicNumberRule : public SecurityRule {
public:
    std::string getId() const override { return "SEC004"; }
    std::string getName() const override { return "رقم سحري"; }
    std::string getDescription() const override {
        return "يكشف استخدام أرقام بدون تسمية واضحة";
    }
    VulnerabilityCategory getCategory() const override {
        return VulnerabilityCategory::Configuration;
    }
    Severity getDefaultSeverity() const override { return Severity::Low; }
    
    std::vector<SecurityFinding> checkLine(
        const std::string& line,
        int lineNumber,
        const std::string& filePath
    ) override {
        std::vector<SecurityFinding> findings;
        
        // أرقام قد تكون حدود أمنية
        std::regex securityNumbers(R"(\b(1024|2048|4096|8192|65535|256|512|128|64)\b)");
        std::smatch match;
        
        // تجاهل تعريفات الثوابت
        if (line.find("ثابت") != std::string::npos || 
            line.find("const") != std::string::npos) {
            return findings;
        }
        
        if (std::regex_search(line, match, securityNumbers)) {
            SourceLocation loc{filePath, lineNumber,
                              static_cast<int>(match.position()) + 1,
                              lineNumber,
                              static_cast<int>(match.position() + match.length())};
            
            auto finding = createFinding(
                loc,
                "رقم قد يكون حداً أمنياً (" + match.str() + ") بدون تسمية",
                line,
                "عرّف ثابتاً باسم واضح لهذا الرقم",
                "ثابت حجم_المخزن = " + match.str()
            );
            findings.push_back(finding);
        }
        
        return findings;
    }
};

// =============================================================================
// تسجيل القواعد الافتراضية
// =============================================================================

/**
 * تسجيل جميع القواعد الافتراضية
 */
inline void registerDefaultRules() {
    auto& manager = SecurityRuleManager::getInstance();
    
    manager.registerRule(std::make_unique<UnsafeBlockRule>());
    manager.registerRule(std::make_unique<RawPointerRule>());
    manager.registerRule(std::make_unique<SensitiveDataLeakRule>());
    manager.registerRule(std::make_unique<MagicNumberRule>());
}

// =============================================================================
// تهيئة تلقائية
// =============================================================================

namespace {
    struct RuleInitializer {
        RuleInitializer() {
            registerDefaultRules();
        }
    };
    // التهيئة عند تحميل المكتبة
    // static RuleInitializer _ruleInit;
}

} // namespace sad::security

// =============================================================================
// واجهة C للتكامل
// =============================================================================

extern "C" {

/**
 * تهيئة نظام الأمان
 */
void sad_security_init() {
    sad::security::registerDefaultRules();
}

/**
 * فحص ملف
 * @return عدد المشاكل المكتشفة
 */
int sad_security_check_file(const char* content, const char* filePath) {
    auto& manager = sad::security::SecurityRuleManager::getInstance();
    auto findings = manager.checkFile(content, filePath);
    
    for (const auto& f : findings) {
        std::cout << f.format(true);
    }
    
    return static_cast<int>(findings.size());
}

/**
 * تفعيل قاعدة
 */
void sad_security_enable_rule(const char* ruleId) {
    sad::security::SecurityRuleManager::getInstance().enableRule(ruleId);
}

/**
 * تعطيل قاعدة
 */
void sad_security_disable_rule(const char* ruleId) {
    sad::security::SecurityRuleManager::getInstance().disableRule(ruleId);
}

} // extern "C"

// =============================================================================
// اختبارات (Tests)
// =============================================================================

#ifdef SECURITY_RULES_TEST

#include <cassert>

void testUnsafeBlockRule() {
    sad::security::UnsafeBlockRule rule;
    
    // اختبار كشف غير_آمن
    auto findings = rule.checkLine("غير_آمن {", 1, "test.ص");
    assert(findings.size() == 1);
    assert(findings[0].ruleId == "SEC001");
    
    // اختبار كود آمن
    findings = rule.checkLine("متغير x = 10", 1, "test.ص");
    assert(findings.empty());
    
    std::cout << "✅ اختبار UnsafeBlockRule نجح\n";
}

void testSensitiveDataLeakRule() {
    sad::security::SensitiveDataLeakRule rule;
    
    // اختبار كشف طباعة كلمة سر
    auto findings = rule.checkLine("اطبع(كلمة_السر)", 1, "test.ص");
    assert(findings.size() == 1);
    assert(findings[0].severity == sad::security::Severity::Critical);
    
    // اختبار كود آمن
    findings = rule.checkLine("اطبع(\"مرحبا\")", 1, "test.ص");
    assert(findings.empty());
    
    std::cout << "✅ اختبار SensitiveDataLeakRule نجح\n";
}

void testRuleManager() {
    sad::security::registerDefaultRules();
    auto& manager = sad::security::SecurityRuleManager::getInstance();
    
    std::string code = R"(
غير_آمن {
    متغير x = 10
}
اطبع(كلمة_السر)
)";
    
    auto findings = manager.checkFile(code, "test.ص");
    assert(findings.size() >= 2);
    
    std::cout << "✅ اختبار RuleManager نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات قواعد الأمان\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testUnsafeBlockRule();
    testSensitiveDataLeakRule();
    testRuleManager();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // SECURITY_RULES_TEST
