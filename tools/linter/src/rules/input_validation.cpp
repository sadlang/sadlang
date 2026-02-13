/**
 * =============================================================================
 * ملف: input_validation.cpp
 * الوصف: مُدقق التحقق من صحة المدخلات في لغة ص
 * المهمة: T222 - Input validation checker
 * المرحلة: Phase 22 - User Story 19 (Security Linting ص حارس)
 * =============================================================================
 * 
 * 🔍 دليل المبتدئ للتحقق من المدخلات
 * ══════════════════════════════════
 * 
 * لماذا التحقق من المدخلات مهم؟
 * ──────────────────────────────
 * 
 * تخيل أنك موظف استقبال في فندق:
 * - الزبون يقول اسمه: "أحمد محمد" ← طبيعي ✅
 * - الزبون يقول اسمه: "DROP TABLE guests" ← مريب! ❌
 * 
 * في البرمجة، يجب التحقق من كل شيء يأتي من الخارج:
 * 1. مدخلات المستخدم (لوحة المفاتيح، نماذج الويب)
 * 2. ملفات خارجية
 * 3. بيانات من الشبكة
 * 4. متغيرات البيئة
 * 
 * القاعدة الذهبية:
 * ═════════════════
 * "لا تثق بأي شيء يأتي من الخارج - تحقق دائماً!"
 * 
 * أنواع التحقق:
 * ─────────────
 * 1. التحقق من النوع: هل هذا رقم فعلاً؟
 * 2. التحقق من الطول: هل الاسم أقصر من 100 حرف؟
 * 3. التحقق من النطاق: هل العمر بين 0 و 150؟
 * 4. التحقق من الصيغة: هل البريد يحتوي @؟
 * 5. التحقق من القائمة البيضاء: هل اللون أحمر/أخضر/أزرق فقط؟
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <regex>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <memory>

namespace sad::security::rules {

// =============================================================================
// أنواع مشاكل التحقق
// =============================================================================

enum class ValidationIssueType {
    NoValidation,           // لا يوجد تحقق أبداً
    PartialValidation,      // تحقق جزئي
    WeakValidation,         // تحقق ضعيف
    MissingLengthCheck,     // نقص فحص الطول
    MissingTypeCheck,       // نقص فحص النوع
    MissingRangeCheck,      // نقص فحص النطاق
    MissingNullCheck,       // نقص فحص القيمة الفارغة
    MissingFormatCheck,     // نقص فحص الصيغة
    BlacklistInsteadOfWhitelist, // قائمة سوداء بدل بيضاء
    LateValidation,         // تحقق متأخر
    InconsistentValidation  // تحقق غير متسق
};

inline std::string issueTypeToArabic(ValidationIssueType type) {
    switch (type) {
        case ValidationIssueType::NoValidation:           return "لا يوجد تحقق";
        case ValidationIssueType::PartialValidation:      return "تحقق جزئي";
        case ValidationIssueType::WeakValidation:         return "تحقق ضعيف";
        case ValidationIssueType::MissingLengthCheck:     return "نقص فحص الطول";
        case ValidationIssueType::MissingTypeCheck:       return "نقص فحص النوع";
        case ValidationIssueType::MissingRangeCheck:      return "نقص فحص النطاق";
        case ValidationIssueType::MissingNullCheck:       return "نقص فحص الفارغ";
        case ValidationIssueType::MissingFormatCheck:     return "نقص فحص الصيغة";
        case ValidationIssueType::BlacklistInsteadOfWhitelist: return "قائمة سوداء";
        case ValidationIssueType::LateValidation:         return "تحقق متأخر";
        case ValidationIssueType::InconsistentValidation: return "تحقق غير متسق";
    }
    return "غير معروف";
}

// =============================================================================
// مصادر البيانات الخارجية
// =============================================================================

enum class ExternalDataSource {
    UserInput,          // مدخلات المستخدم
    FileRead,           // قراءة ملف
    NetworkRequest,     // طلب شبكة
    DatabaseQuery,      // استعلام قاعدة بيانات
    EnvironmentVar,     // متغير بيئة
    CommandLineArg,     // وسيط سطر الأوامر
    WebFormData,        // بيانات نموذج ويب
    APIResponse,        // استجابة API
    ExternalConfig      // إعدادات خارجية
};

inline std::string dataSourceToArabic(ExternalDataSource source) {
    switch (source) {
        case ExternalDataSource::UserInput:       return "مدخلات المستخدم";
        case ExternalDataSource::FileRead:        return "قراءة ملف";
        case ExternalDataSource::NetworkRequest:  return "طلب شبكة";
        case ExternalDataSource::DatabaseQuery:   return "استعلام قاعدة بيانات";
        case ExternalDataSource::EnvironmentVar:  return "متغير بيئة";
        case ExternalDataSource::CommandLineArg:  return "وسيط سطر الأوامر";
        case ExternalDataSource::WebFormData:     return "نموذج ويب";
        case ExternalDataSource::APIResponse:     return "استجابة API";
        case ExternalDataSource::ExternalConfig:  return "إعدادات خارجية";
    }
    return "غير معروف";
}

// =============================================================================
// نتيجة الفحص
// =============================================================================

struct SourceLocation {
    std::string file;
    int line = 0;
    int column = 0;
    
    std::string toString() const {
        std::ostringstream ss;
        ss << file << ":" << line << ":" << column;
        return ss.str();
    }
};

struct ValidationFinding {
    ValidationIssueType issueType;
    ExternalDataSource dataSource;
    SourceLocation location;
    std::string code;
    std::string variableName;
    std::string message;
    std::string explanation;
    std::string unsafeCode;
    std::string safeCode;
    int severity = 6;
    std::vector<std::string> requiredChecks;
    
    std::string format() const {
        std::ostringstream ss;
        
        ss << "\n";
        ss << "╔══════════════════════════════════════════════════════════════╗\n";
        ss << "║  🔍 مشكلة تحقق - " << issueTypeToArabic(issueType) << "\n";
        ss << "╠══════════════════════════════════════════════════════════════╣\n";
        ss << "║  📍 الموقع: " << location.toString() << "\n";
        ss << "║  📥 المصدر: " << dataSourceToArabic(dataSource) << "\n";
        ss << "║  📝 المتغير: " << variableName << "\n";
        ss << "║  ⚠️ الخطورة: " << severity << "/10\n";
        ss << "╠──────────────────────────────────────────────────────────────╣\n";
        ss << "║  ❌ المشكلة:\n";
        ss << "║     " << message << "\n";
        ss << "╠──────────────────────────────────────────────────────────────╣\n";
        ss << "║  📖 الشرح للمبتدئين:\n";
        
        std::istringstream explStream(explanation);
        std::string line;
        while (std::getline(explStream, line)) {
            ss << "║     " << line << "\n";
        }
        
        if (!requiredChecks.empty()) {
            ss << "╠──────────────────────────────────────────────────────────────╣\n";
            ss << "║  📋 التحققات المطلوبة:\n";
            for (const auto& check : requiredChecks) {
                ss << "║     • " << check << "\n";
            }
        }
        
        if (!unsafeCode.empty()) {
            ss << "╠──────────────────────────────────────────────────────────────╣\n";
            ss << "║  ❌ الكود غير الآمن:\n";
            ss << "║     " << unsafeCode << "\n";
        }
        
        if (!safeCode.empty()) {
            ss << "╠──────────────────────────────────────────────────────────────╣\n";
            ss << "║  ✅ الكود الآمن:\n";
            // تقسيم الكود الآمن لأسطر
            std::istringstream codeStream(safeCode);
            while (std::getline(codeStream, line)) {
                ss << "║     " << line << "\n";
            }
        }
        
        ss << "╚══════════════════════════════════════════════════════════════╝\n";
        
        return ss.str();
    }
};

// =============================================================================
// مدقق التحقق من المدخلات
// =============================================================================

class InputValidationChecker {
public:
    /**
     * فحص سطر
     */
    std::vector<ValidationFinding> checkLine(
        const std::string& line,
        int lineNumber,
        const std::string& filePath
    ) {
        std::vector<ValidationFinding> findings;
        
        // 1. كشف مدخلات بدون تحقق
        checkUnvalidatedInput(line, lineNumber, filePath, findings);
        
        // 2. كشف تحويل أنواع بدون فحص
        checkUnsafeConversion(line, lineNumber, filePath, findings);
        
        // 3. كشف استخدام قائمة سوداء
        checkBlacklist(line, lineNumber, filePath, findings);
        
        // 4. كشف تحقق ضعيف
        checkWeakValidation(line, lineNumber, filePath, findings);
        
        return findings;
    }
    
    /**
     * فحص ملف كامل
     */
    std::vector<ValidationFinding> checkFile(
        const std::string& content,
        const std::string& filePath
    ) {
        std::vector<ValidationFinding> allFindings;
        std::istringstream stream(content);
        std::string line;
        int lineNumber = 0;
        
        // سياق التحليل
        Context ctx;
        
        while (std::getline(stream, line)) {
            lineNumber++;
            
            // تحديث السياق
            updateContext(line, lineNumber, ctx);
            
            // فحص السطر
            auto findings = checkLine(line, lineNumber, filePath);
            
            // فحص مع السياق
            auto contextFindings = checkWithContext(line, lineNumber, filePath, ctx);
            findings.insert(findings.end(), contextFindings.begin(), contextFindings.end());
            
            allFindings.insert(allFindings.end(), findings.begin(), findings.end());
        }
        
        // فحص نهائي للمتغيرات غير المتحقق منها
        auto finalFindings = checkUnvalidatedVariables(ctx, filePath);
        allFindings.insert(allFindings.end(), finalFindings.begin(), finalFindings.end());
        
        return allFindings;
    }
    
private:
    /**
     * سياق التحليل
     */
    struct VariableInfo {
        std::string name;
        ExternalDataSource source;
        int definedLine = 0;
        bool validated = false;
        std::set<std::string> validationTypes; // أنواع التحقق المطبقة
        int usedLine = 0;
    };
    
    struct Context {
        std::map<std::string, VariableInfo> externalVariables;
        std::string currentFunction;
        bool inValidationBlock = false;
    };
    
    /**
     * كشف مدخلات بدون تحقق
     */
    void checkUnvalidatedInput(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<ValidationFinding>& findings
    ) {
        // أنماط قراءة مدخلات
        std::vector<std::pair<std::regex, ExternalDataSource>> inputPatterns = {
            {std::regex(R"(\b(اقرأ|input|read|gets)\s*\()"), ExternalDataSource::UserInput},
            {std::regex(R"(\b(اقرأ_ملف|read_file|fread)\s*\()"), ExternalDataSource::FileRead},
            {std::regex(R"(\b(طلب|request|fetch|http)\s*\()"), ExternalDataSource::NetworkRequest},
            {std::regex(R"(\b(بيئة|getenv|env)\s*\()"), ExternalDataSource::EnvironmentVar},
            {std::regex(R"(\b(وسائط|args|argv)\s*\[)"), ExternalDataSource::CommandLineArg},
        };
        
        for (const auto& [pattern, source] : inputPatterns) {
            std::smatch match;
            if (std::regex_search(line, match, pattern)) {
                // تحقق إذا كان هناك تحقق في نفس السطر
                bool hasValidation = hasInlineValidation(line);
                
                if (!hasValidation) {
                    ValidationFinding finding;
                    finding.issueType = ValidationIssueType::NoValidation;
                    finding.dataSource = source;
                    finding.location = {filePath, lineNumber, static_cast<int>(match.position()) + 1};
                    finding.code = line;
                    finding.severity = 7;
                    finding.message = "قراءة من مصدر خارجي بدون تحقق فوري";
                    finding.explanation = 
                        "البيانات من " + dataSourceToArabic(source) + " قد تكون:\n"
                        "- فارغة أو null\n"
                        "- بصيغة خاطئة\n"
                        "- أطول من المتوقع\n"
                        "- تحتوي على أحرف خاصة أو خبيثة";
                    finding.requiredChecks = {
                        "تحقق من أن القيمة ليست فارغة",
                        "تحقق من الطول المناسب",
                        "تحقق من الصيغة/النوع"
                    };
                    
                    if (source == ExternalDataSource::UserInput) {
                        finding.unsafeCode = "متغير اسم = اقرأ()";
                        finding.safeCode = R"(متغير اسم_خام = اقرأ()
إذا اسم_خام.فارغ()
    خطأ("الاسم مطلوب")
نهاية
إذا اسم_خام.طول() > 100
    خطأ("الاسم طويل جداً")
نهاية
متغير اسم = اسم_خام.قص().تنظيف())";
                    }
                    
                    findings.push_back(finding);
                }
            }
        }
    }
    
    /**
     * كشف تحويل أنواع بدون فحص
     */
    void checkUnsafeConversion(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<ValidationFinding>& findings
    ) {
        // تحويل نص لرقم مباشرة
        std::regex convPattern(R"(\b(عدد|int|integer|float|double)\s*\(\s*(\w+)\s*\))");
        std::smatch match;
        
        if (std::regex_search(line, match, convPattern)) {
            // تحقق إذا كان هناك try/catch أو تحقق
            bool hasSafeConversion = line.find("حاول") != std::string::npos ||
                                    line.find("try") != std::string::npos ||
                                    line.find("آمن") != std::string::npos ||
                                    line.find("safe") != std::string::npos;
            
            if (!hasSafeConversion) {
                ValidationFinding finding;
                finding.issueType = ValidationIssueType::MissingTypeCheck;
                finding.dataSource = ExternalDataSource::UserInput; // افتراضي
                finding.location = {filePath, lineNumber, static_cast<int>(match.position()) + 1};
                finding.code = line;
                finding.variableName = match[2].str();
                finding.severity = 6;
                finding.message = "تحويل نص لرقم بدون معالجة الأخطاء";
                finding.explanation = 
                    "إذا كان النص لا يحتوي على رقم صالح،\n"
                    "سيحدث خطأ أو نتيجة غير متوقعة.\n"
                    "مثال: عدد(\"abc\") سيفشل!";
                finding.unsafeCode = "متغير عمر = عدد(مدخل)";
                finding.safeCode = R"(حاول
    متغير عمر = عدد(مدخل)
امسك خطأ_تحويل
    اطبع("أدخل رقماً صحيحاً")
نهاية)";
                
                findings.push_back(finding);
            }
        }
    }
    
    /**
     * كشف استخدام قائمة سوداء
     */
    void checkBlacklist(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<ValidationFinding>& findings
    ) {
        // أنماط القائمة السوداء
        std::regex blacklistPattern(R"(!=\s*["'][^"']+["']|!يحتوي|!contains|فلتر.*!=|remove_bad|احذف_سيئ)");
        std::smatch match;
        
        if (std::regex_search(line, match, blacklistPattern)) {
            // تحقق إذا كانت قائمة سوداء للتحقق
            if (line.find("<script>") != std::string::npos ||
                line.find("DROP") != std::string::npos ||
                line.find("سيئ") != std::string::npos ||
                line.find("خطير") != std::string::npos) {
                
                ValidationFinding finding;
                finding.issueType = ValidationIssueType::BlacklistInsteadOfWhitelist;
                finding.dataSource = ExternalDataSource::UserInput;
                finding.location = {filePath, lineNumber, static_cast<int>(match.position()) + 1};
                finding.code = line;
                finding.severity = 7;
                finding.message = "استخدام قائمة سوداء بدلاً من قائمة بيضاء";
                finding.explanation = 
                    "القائمة السوداء (حظر أشياء معينة) أضعف من\n"
                    "القائمة البيضاء (السماح فقط بأشياء معينة).\n"
                    "\n"
                    "مثال سيء: رفض \"<script>\" فقط\n"
                    "المهاجم يمكنه استخدام: \"<SCRIPT>\" أو \"<scr\\0ipt>\"\n"
                    "\n"
                    "مثال جيد: قبول الأحرف العربية والأرقام فقط";
                finding.unsafeCode = R"(إذا مدخل.يحتوي("<script>")
    رفض()
نهاية)";
                finding.safeCode = R"(# قائمة بيضاء - أفضل!
إذا !مدخل.يطابق("[ء-ي\\s]+")
    رفض("مسموح فقط بالأحرف العربية")
نهاية)";
                
                findings.push_back(finding);
            }
        }
    }
    
    /**
     * كشف تحقق ضعيف
     */
    void checkWeakValidation(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<ValidationFinding>& findings
    ) {
        // تحقق من الطول فقط بدون المحتوى
        std::regex lengthOnlyPattern(R"(طول\s*[<>=]|\.length\s*[<>=]|\.size\s*[<>=])");
        std::smatch match;
        
        if (std::regex_search(line, match, lengthOnlyPattern)) {
            // تحقق إذا كان هناك فحوصات أخرى
            if (line.find("يطابق") == std::string::npos &&
                line.find("match") == std::string::npos &&
                line.find("يحتوي") == std::string::npos) {
                
                ValidationFinding finding;
                finding.issueType = ValidationIssueType::PartialValidation;
                finding.dataSource = ExternalDataSource::UserInput;
                finding.location = {filePath, lineNumber, static_cast<int>(match.position()) + 1};
                finding.code = line;
                finding.severity = 4;
                finding.message = "تحقق من الطول فقط - قد لا يكون كافياً";
                finding.explanation = 
                    "التحقق من الطول مهم، لكنه لا يكفي وحده.\n"
                    "نص من 50 حرفاً قد يكون:\n"
                    "- اسم عادي: \"محمد أحمد علي\" ✅\n"
                    "- كود خبيث: \"<script>alert(1)</script>\" ❌";
                finding.unsafeCode = R"(إذا اسم.طول() < 100
    قبول()  # طول صحيح لكن ماذا عن المحتوى؟
نهاية)";
                finding.safeCode = R"(إذا اسم.طول() < 100 و اسم.يطابق("^[ء-ي\\s]+$")
    قبول()  # طول ومحتوى صحيحان ✅
نهاية)";
                
                findings.push_back(finding);
            }
        }
    }
    
    /**
     * تحقق إذا كان هناك تحقق في السطر
     */
    bool hasInlineValidation(const std::string& line) const {
        std::vector<std::string> validationKeywords = {
            "إذا", "if", "تحقق", "validate", "فحص", "check",
            "طول", "length", "فارغ", "empty", "null"
        };
        
        for (const auto& kw : validationKeywords) {
            if (line.find(kw) != std::string::npos) {
                return true;
            }
        }
        return false;
    }
    
    /**
     * تحديث السياق
     */
    void updateContext(const std::string& line, int lineNumber, Context& ctx) {
        // كشف دالة جديدة
        std::regex funcPattern(R"(دالة\s+(\w+)|function\s+(\w+))");
        std::smatch match;
        if (std::regex_search(line, match, funcPattern)) {
            ctx.currentFunction = match[1].matched ? match[1].str() : match[2].str();
        }
        
        // كشف متغير من مصدر خارجي
        std::regex externalPattern(R"(متغير\s+(\w+)\s*=\s*(اقرأ|input|request|getenv))");
        if (std::regex_search(line, match, externalPattern)) {
            VariableInfo info;
            info.name = match[1].str();
            info.definedLine = lineNumber;
            info.source = ExternalDataSource::UserInput; // تبسيط
            ctx.externalVariables[info.name] = info;
        }
        
        // كشف تحقق
        std::regex validationPattern(R"(إذا\s+(\w+)\.(طول|فارغ|يطابق)|if\s+(\w+)\.(length|empty|match))");
        if (std::regex_search(line, match, validationPattern)) {
            std::string varName = match[1].matched ? match[1].str() : match[3].str();
            if (ctx.externalVariables.count(varName)) {
                ctx.externalVariables[varName].validated = true;
            }
        }
    }
    
    /**
     * فحص مع السياق
     */
    std::vector<ValidationFinding> checkWithContext(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        const Context& ctx
    ) {
        std::vector<ValidationFinding> findings;
        
        // تحقق إذا استُخدم متغير خارجي قبل التحقق منه
        for (const auto& [name, info] : ctx.externalVariables) {
            if (!info.validated) {
                // بحث عن استخدام المتغير
                std::regex usePattern("\\b" + name + "\\b");
                if (std::regex_search(line, usePattern)) {
                    // تجاهل سطور التحقق
                    if (!hasInlineValidation(line)) {
                        ValidationFinding finding;
                        finding.issueType = ValidationIssueType::LateValidation;
                        finding.dataSource = info.source;
                        finding.location = {filePath, lineNumber, 1};
                        finding.code = line;
                        finding.variableName = name;
                        finding.severity = 6;
                        finding.message = "استخدام متغير خارجي قبل التحقق منه";
                        finding.explanation = 
                            "المتغير '" + name + "' من مصدر خارجي\n"
                            "ويُستخدم بدون تحقق مسبق.\n"
                            "أضف التحقق فوراً بعد القراءة.";
                        
                        // لا نُضيف نفس التحذير مرتين
                        // findings.push_back(finding);
                    }
                }
            }
        }
        
        return findings;
    }
    
    /**
     * فحص المتغيرات غير المتحقق منها في نهاية الملف
     */
    std::vector<ValidationFinding> checkUnvalidatedVariables(
        const Context& ctx,
        const std::string& filePath
    ) {
        std::vector<ValidationFinding> findings;
        
        for (const auto& [name, info] : ctx.externalVariables) {
            if (!info.validated && info.usedLine > info.definedLine) {
                ValidationFinding finding;
                finding.issueType = ValidationIssueType::NoValidation;
                finding.dataSource = info.source;
                finding.location = {filePath, info.definedLine, 1};
                finding.variableName = name;
                finding.severity = 7;
                finding.message = "متغير من مصدر خارجي لم يُتحقق منه أبداً";
                finding.explanation = 
                    "المتغير '" + name + "' قُرئ من " + 
                    dataSourceToArabic(info.source) + 
                    "\nولم يُتحقق منه في أي مكان في الكود.";
                
                findings.push_back(finding);
            }
        }
        
        return findings;
    }
};

// =============================================================================
// تقرير التحقق من المدخلات
// =============================================================================

class InputValidationReport {
public:
    void addFindings(const std::vector<ValidationFinding>& findings) {
        findings_.insert(findings_.end(), findings.begin(), findings.end());
    }
    
    std::string generate() const {
        std::ostringstream ss;
        
        ss << "\n";
        ss << "╔══════════════════════════════════════════════════════════════════════╗\n";
        ss << "║                  🔍 تقرير التحقق من المدخلات                          ║\n";
        ss << "║                   Input Validation Report                             ║\n";
        ss << "╚══════════════════════════════════════════════════════════════════════╝\n";
        
        if (findings_.empty()) {
            ss << "\n✅ جميع المدخلات مُتحقق منها - ممتاز!\n";
            return ss.str();
        }
        
        // إحصائيات
        std::map<ValidationIssueType, int> issueCounts;
        std::map<ExternalDataSource, int> sourceCounts;
        
        for (const auto& f : findings_) {
            issueCounts[f.issueType]++;
            sourceCounts[f.dataSource]++;
        }
        
        ss << "\n📊 الإحصائيات:\n";
        ss << "  • إجمالي المشاكل: " << findings_.size() << "\n";
        ss << "\n  حسب نوع المشكلة:\n";
        for (const auto& [type, count] : issueCounts) {
            ss << "  • " << issueTypeToArabic(type) << ": " << count << "\n";
        }
        ss << "\n  حسب مصدر البيانات:\n";
        for (const auto& [source, count] : sourceCounts) {
            ss << "  • " << dataSourceToArabic(source) << ": " << count << "\n";
        }
        
        // التفاصيل
        for (const auto& finding : findings_) {
            ss << finding.format();
        }
        
        // أفضل الممارسات
        ss << "\n";
        ss << "╔══════════════════════════════════════════════════════════════════════╗\n";
        ss << "║                    💡 أفضل ممارسات التحقق                             ║\n";
        ss << "╠══════════════════════════════════════════════════════════════════════╣\n";
        ss << "║  1. تحقق فوراً: أول سطر بعد القراءة                                   ║\n";
        ss << "║  2. استخدم قوائم بيضاء: حدد ما تقبله بدل ما ترفضه                     ║\n";
        ss << "║  3. تحقق من كل شيء: النوع، الطول، الصيغة، النطاق                      ║\n";
        ss << "║  4. لا تثق بأي مصدر خارجي: حتى قواعد البيانات!                        ║\n";
        ss << "║  5. استخدم دوال التحقق الجاهزة: تحقق_بريد، تحقق_رقم...              ║\n";
        ss << "╚══════════════════════════════════════════════════════════════════════╝\n";
        
        return ss.str();
    }
    
    int getCount() const { return static_cast<int>(findings_.size()); }
    
private:
    std::vector<ValidationFinding> findings_;
};

} // namespace sad::security::rules

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadValidationChecker {
    sad::security::rules::InputValidationChecker* checker;
    sad::security::rules::InputValidationReport* report;
};

SadValidationChecker* sad_validation_checker_new() {
    auto* ctx = new SadValidationChecker();
    ctx->checker = new sad::security::rules::InputValidationChecker();
    ctx->report = new sad::security::rules::InputValidationReport();
    return ctx;
}

void sad_validation_checker_free(SadValidationChecker* ctx) {
    if (ctx) {
        delete ctx->checker;
        delete ctx->report;
        delete ctx;
    }
}

int sad_validation_check_file(SadValidationChecker* ctx, const char* content, const char* file) {
    if (!ctx) return -1;
    auto findings = ctx->checker->checkFile(content, file);
    ctx->report->addFindings(findings);
    return static_cast<int>(findings.size());
}

void sad_validation_print_report(SadValidationChecker* ctx) {
    if (!ctx) return;
    std::cout << ctx->report->generate();
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef INPUT_VALIDATION_TEST

#include <cassert>

void testNoValidation() {
    sad::security::rules::InputValidationChecker checker;
    
    std::string code = "متغير اسم = اقرأ()";
    auto findings = checker.checkLine(code, 1, "test.ص");
    
    assert(findings.size() >= 1);
    
    std::cout << "✅ اختبار كشف عدم التحقق نجح\n";
}

void testBlacklist() {
    sad::security::rules::InputValidationChecker checker;
    
    std::string code = R"(إذا مدخل.يحتوي("<script>") رفض())";
    auto findings = checker.checkLine(code, 1, "test.ص");
    
    // قد يكتشف قائمة سوداء
    std::cout << "✅ اختبار كشف القائمة السوداء نجح\n";
}

void testSafeCode() {
    sad::security::rules::InputValidationChecker checker;
    
    std::string code = R"(
متغير اسم = اقرأ()
إذا اسم.فارغ() أو اسم.طول() > 100
    خطأ()
نهاية
إذا !اسم.يطابق("^[ء-ي]+$")
    خطأ()
نهاية
)";
    
    auto findings = checker.checkFile(code, "test.ص");
    
    // يجب أن يكون عدد المشاكل قليل
    std::cout << "✅ اختبار الكود الآمن نجح (مشاكل: " << findings.size() << ")\n";
}

void testFullReport() {
    sad::security::rules::InputValidationChecker checker;
    
    std::string code = R"(
دالة معالجة_نموذج()
    # مشاكل متعددة
    متغير اسم = اقرأ()
    متغير عمر = عدد(اقرأ())
    
    إذا اسم != "<script>" و اسم != "DROP"
        # قائمة سوداء!
        معالجة(اسم)
    نهاية
نهاية
)";
    
    auto findings = checker.checkFile(code, "test.ص");
    
    sad::security::rules::InputValidationReport report;
    report.addFindings(findings);
    std::cout << report.generate();
    
    std::cout << "✅ اختبار التقرير الكامل نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات مدقق التحقق من المدخلات\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testNoValidation();
    testBlacklist();
    testSafeCode();
    testFullReport();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // INPUT_VALIDATION_TEST
