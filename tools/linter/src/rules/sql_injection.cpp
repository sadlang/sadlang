/**
 * =============================================================================
 * ملف: sql_injection.cpp
 * الوصف: كاشف حقن SQL في لغة ص
 * المهمة: T220 - SQL injection detector
 * المرحلة: Phase 22 - User Story 19 (Security Linting ص حارس)
 * =============================================================================
 * 
 * 🛡️ دليل المبتدئ لفهم حقن SQL
 * ══════════════════════════════
 * 
 * ما هو SQL؟
 * ──────────
 * SQL هي لغة للتحدث مع قواعد البيانات.
 * 
 * مثال: "أعطني كل المستخدمين الذين اسمهم أحمد"
 * في SQL: SELECT * FROM users WHERE name = 'أحمد'
 * 
 * ما هو حقن SQL؟
 * ───────────────
 * عندما يُدخل مستخدم خبيث نصاً يُغير معنى الاستعلام!
 * 
 * 📖 قصة للتوضيح:
 * ─────────────────
 * تخيل موظفاً في بنك يسأل الزبون:
 * "ما اسمك؟" ← الزبون يقول: "أحمد"
 * الموظف يكتب: "اعرض حساب أحمد"
 * 
 * الآن تخيل زبوناً خبيثاً يقول:
 * "أحمد' أو اعرض كل الحسابات أو '"
 * الموظف يكتب: "اعرض حساب أحمد' أو اعرض كل الحسابات أو ''"
 * 
 * النتيجة: المهاجم يرى كل الحسابات! 😱
 * 
 * كيف نحمي أنفسنا؟
 * ─────────────────
 * 1. استخدام الاستعلامات المُعدّة (Prepared Statements)
 * 2. التحقق من المدخلات
 * 3. تشفير الأحرف الخاصة
 * 
 * هذا الملف يكشف الكود الخطير تلقائياً!
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
// أنماط خطيرة (Dangerous Patterns)
// =============================================================================

/**
 * نوع الثغرة
 */
enum class SQLInjectionType {
    StringConcatenation,    // دمج نصوص مع استعلام
    InterpolatedVariable,   // متغير مُدرج في استعلام
    DynamicTableName,       // اسم جدول ديناميكي
    DynamicColumnName,      // اسم عمود ديناميكي
    UnescapedInput,         // مدخل غير مُشفّر
    RawQueryExecution,      // تنفيذ استعلام خام
    ORMBypass              // تجاوز ORM بكود خام
};

inline std::string injectionTypeToArabic(SQLInjectionType type) {
    switch (type) {
        case SQLInjectionType::StringConcatenation:  return "دمج نصوص";
        case SQLInjectionType::InterpolatedVariable: return "متغير مُدرج";
        case SQLInjectionType::DynamicTableName:     return "اسم جدول ديناميكي";
        case SQLInjectionType::DynamicColumnName:    return "اسم عمود ديناميكي";
        case SQLInjectionType::UnescapedInput:       return "مدخل غير مُشفّر";
        case SQLInjectionType::RawQueryExecution:    return "استعلام خام";
        case SQLInjectionType::ORMBypass:            return "تجاوز ORM";
    }
    return "غير معروف";
}

// =============================================================================
// موقع الكود
// =============================================================================

struct SourceLocation {
    std::string file;
    int line = 0;
    int column = 0;
    int endColumn = 0;
    
    std::string toString() const {
        std::ostringstream ss;
        ss << file << ":" << line << ":" << column;
        return ss.str();
    }
};

// =============================================================================
// نتيجة الكشف (Detection Result)
// =============================================================================

/**
 * نتيجة كشف حقن SQL
 */
struct SQLInjectionFinding {
    SQLInjectionType type;          // نوع الثغرة
    SourceLocation location;        // الموقع
    std::string code;               // الكود المشكل
    std::string message;            // الرسالة
    std::string explanation;        // شرح للمبتدئين
    std::string unsafeCode;         // الكود الخطير
    std::string safeCode;           // الكود الآمن
    int severity = 9;               // الخطورة (1-10)
    std::vector<std::string> references; // مراجع
    
    /**
     * تنسيق النتيجة
     */
    std::string format() const {
        std::ostringstream ss;
        
        ss << "\n";
        ss << "╔══════════════════════════════════════════════════════════════╗\n";
        ss << "║  🛡️ تحذير حقن SQL - " << injectionTypeToArabic(type) << "\n";
        ss << "╠══════════════════════════════════════════════════════════════╣\n";
        ss << "║  📍 الموقع: " << location.toString() << "\n";
        ss << "║  ⚠️ الخطورة: " << severity << "/10\n";
        ss << "╠──────────────────────────────────────────────────────────────╣\n";
        ss << "║  ❌ المشكلة:\n";
        ss << "║     " << message << "\n";
        ss << "╠──────────────────────────────────────────────────────────────╣\n";
        ss << "║  📖 الشرح للمبتدئين:\n";
        
        // تقسيم الشرح لأسطر
        std::istringstream explStream(explanation);
        std::string line;
        while (std::getline(explStream, line)) {
            ss << "║     " << line << "\n";
        }
        
        if (!unsafeCode.empty()) {
            ss << "╠──────────────────────────────────────────────────────────────╣\n";
            ss << "║  ❌ الكود الخطير:\n";
            ss << "║     " << unsafeCode << "\n";
        }
        
        if (!safeCode.empty()) {
            ss << "╠──────────────────────────────────────────────────────────────╣\n";
            ss << "║  ✅ الكود الآمن:\n";
            ss << "║     " << safeCode << "\n";
        }
        
        ss << "╚══════════════════════════════════════════════════════════════╝\n";
        
        return ss.str();
    }
};

// =============================================================================
// كاشف حقن SQL
// =============================================================================

/**
 * كاشف حقن SQL
 * 
 * للمبتدئين:
 * هذا الكاشف يقرأ كودك ويبحث عن طرق خطيرة لبناء استعلامات SQL
 */
class SQLInjectionDetector {
public:
    /**
     * فحص سطر من الكود
     */
    std::vector<SQLInjectionFinding> checkLine(
        const std::string& line,
        int lineNumber,
        const std::string& filePath
    ) {
        std::vector<SQLInjectionFinding> findings;
        
        // 1. كشف دمج النصوص مع استعلام SQL
        checkStringConcatenation(line, lineNumber, filePath, findings);
        
        // 2. كشف الاستعلام الخام
        checkRawQuery(line, lineNumber, filePath, findings);
        
        // 3. كشف اسم جدول/عمود ديناميكي
        checkDynamicNames(line, lineNumber, filePath, findings);
        
        // 4. كشف f-string مع SQL
        checkFormattedString(line, lineNumber, filePath, findings);
        
        return findings;
    }
    
    /**
     * فحص ملف كامل
     */
    std::vector<SQLInjectionFinding> checkFile(
        const std::string& content,
        const std::string& filePath
    ) {
        std::vector<SQLInjectionFinding> allFindings;
        std::istringstream stream(content);
        std::string line;
        int lineNumber = 0;
        
        // تتبع السياق
        bool inSQLBlock = false;
        std::string sqlVariable;
        
        while (std::getline(stream, line)) {
            lineNumber++;
            
            // تحديث السياق
            updateContext(line, inSQLBlock, sqlVariable);
            
            // فحص السطر
            auto findings = checkLine(line, lineNumber, filePath);
            
            // فحص إضافي مع السياق
            if (inSQLBlock || !sqlVariable.empty()) {
                auto contextFindings = checkWithContext(
                    line, lineNumber, filePath, inSQLBlock, sqlVariable
                );
                findings.insert(findings.end(), 
                               contextFindings.begin(), 
                               contextFindings.end());
            }
            
            allFindings.insert(allFindings.end(), findings.begin(), findings.end());
        }
        
        return allFindings;
    }
    
private:
    /**
     * كشف دمج النصوص مع استعلام SQL
     * 
     * خطير: "SELECT * FROM users WHERE name = '" + اسم + "'"
     * آمن: استعلام("SELECT * FROM users WHERE name = ?", اسم)
     */
    void checkStringConcatenation(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<SQLInjectionFinding>& findings
    ) {
        // أنماط SQL شائعة
        std::vector<std::string> sqlKeywords = {
            "SELECT", "INSERT", "UPDATE", "DELETE", "DROP", "CREATE",
            "اختر", "أدخل", "حدّث", "احذف", "أسقط", "أنشئ"
        };
        
        // بحث عن كلمة SQL + دمج نص
        std::regex concatPattern(R"((["']).*?(SELECT|INSERT|UPDATE|DELETE|اختر|أدخل|حدّث|احذف).*?\1\s*\+)");
        std::smatch match;
        
        if (std::regex_search(line, match, concatPattern)) {
            SQLInjectionFinding finding;
            finding.type = SQLInjectionType::StringConcatenation;
            finding.location = {filePath, lineNumber, 
                               static_cast<int>(match.position()) + 1,
                               static_cast<int>(match.position() + match.length())};
            finding.code = line;
            finding.severity = 9;
            finding.message = "دمج نصوص مع استعلام SQL - خطر حقن!";
            finding.explanation = 
                "عندما تدمج نصاً من المستخدم مع استعلام SQL،\n"
                "يمكن للمهاجم إدخال أوامر SQL خاصة به.\n"
                "مثلاً: إذا أدخل المستخدم: أحمد' OR '1'='1\n"
                "سيصبح الاستعلام: SELECT * FROM users WHERE name='أحمد' OR '1'='1'\n"
                "وهذا يُرجع كل المستخدمين!";
            finding.unsafeCode = "استعلام(\"SELECT * FROM users WHERE name='\" + اسم + \"'\")";
            finding.safeCode = "استعلام_آمن(\"SELECT * FROM users WHERE name=?\", اسم)";
            
            findings.push_back(finding);
        }
        
        // بحث بنمط آخر: variable + "SQL"
        std::regex concatPattern2(R"(\+\s*["'].*?(WHERE|AND|OR|VALUES|SET).*?["'])");
        if (std::regex_search(line, match, concatPattern2)) {
            // تحقق إذا لم نكتشفه بالفعل
            bool alreadyFound = false;
            for (const auto& f : findings) {
                if (f.location.line == lineNumber) {
                    alreadyFound = true;
                    break;
                }
            }
            
            if (!alreadyFound) {
                SQLInjectionFinding finding;
                finding.type = SQLInjectionType::StringConcatenation;
                finding.location = {filePath, lineNumber,
                                   static_cast<int>(match.position()) + 1,
                                   static_cast<int>(match.position() + match.length())};
                finding.code = line;
                finding.severity = 9;
                finding.message = "دمج متغير مع جزء من استعلام SQL";
                finding.explanation = 
                    "حتى لو كان جزء صغير من الاستعلام يأتي من متغير،\n"
                    "يمكن للمهاجم استغلاله.";
                finding.unsafeCode = line;
                finding.safeCode = "استخدم الاستعلامات المُعدّة (Prepared Statements)";
                
                findings.push_back(finding);
            }
        }
    }
    
    /**
     * كشف تنفيذ استعلام خام
     */
    void checkRawQuery(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<SQLInjectionFinding>& findings
    ) {
        // دوال خطيرة
        std::regex rawPattern(R"(استعلام_خام|raw_query|execute_raw|نفذ_خام|\.raw\()");
        std::smatch match;
        
        if (std::regex_search(line, match, rawPattern)) {
            SQLInjectionFinding finding;
            finding.type = SQLInjectionType::RawQueryExecution;
            finding.location = {filePath, lineNumber,
                               static_cast<int>(match.position()) + 1,
                               static_cast<int>(match.position() + match.length())};
            finding.code = line;
            finding.severity = 8;
            finding.message = "استخدام استعلام خام - تأكد من تنظيف المدخلات";
            finding.explanation = 
                "الاستعلامات الخام تنفذ SQL كما هو بدون حماية.\n"
                "إذا كانت هناك بيانات من المستخدم، قد تكون خطيرة.\n"
                "استخدم دائماً الاستعلامات المُعدّة إن أمكن.";
            finding.unsafeCode = "قب.استعلام_خام(\"SELECT * FROM users WHERE id=\" + معرف)";
            finding.safeCode = "قب.استعلام(\"SELECT * FROM users WHERE id=?\", معرف)";
            
            findings.push_back(finding);
        }
    }
    
    /**
     * كشف أسماء جداول/أعمدة ديناميكية
     */
    void checkDynamicNames(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<SQLInjectionFinding>& findings
    ) {
        // FROM + متغير
        std::regex fromPattern(R"(FROM\s*["']\s*\+|\+\s*["']\s*WHERE)");
        std::smatch match;
        
        if (std::regex_search(line, match, fromPattern)) {
            SQLInjectionFinding finding;
            finding.type = SQLInjectionType::DynamicTableName;
            finding.location = {filePath, lineNumber,
                               static_cast<int>(match.position()) + 1,
                               static_cast<int>(match.position() + match.length())};
            finding.code = line;
            finding.severity = 10;
            finding.message = "اسم جدول ديناميكي - خطر حقن شديد!";
            finding.explanation = 
                "إذا جاء اسم الجدول من المستخدم، يمكنه:\n"
                "1. الوصول لجداول محظورة\n"
                "2. حذف بيانات\n"
                "3. استخراج معلومات حساسة\n"
                "لا تستخدم أسماء جداول ديناميكية أبداً!";
            finding.unsafeCode = "\"SELECT * FROM \" + اسم_الجدول";
            finding.safeCode = "استخدم أسماء جداول ثابتة أو قائمة بيضاء محددة";
            
            findings.push_back(finding);
        }
    }
    
    /**
     * كشف f-string مع SQL
     */
    void checkFormattedString(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<SQLInjectionFinding>& findings
    ) {
        // نص{"..."} مع SQL (format string في ص)
        std::regex fstringPattern(R"(نص\s*\{.*?(SELECT|INSERT|UPDATE|DELETE|اختر|أدخل).*?\})");
        std::smatch match;
        
        if (std::regex_search(line, match, fstringPattern)) {
            SQLInjectionFinding finding;
            finding.type = SQLInjectionType::InterpolatedVariable;
            finding.location = {filePath, lineNumber,
                               static_cast<int>(match.position()) + 1,
                               static_cast<int>(match.position() + match.length())};
            finding.code = line;
            finding.severity = 9;
            finding.message = "استخدام نص منسق مع SQL - خطر حقن!";
            finding.explanation = 
                "نص{...} يدرج المتغيرات مباشرة في النص.\n"
                "هذا خطير جداً مع SQL لأنه لا يُشفّر الأحرف الخاصة.";
            finding.unsafeCode = "نص{\"SELECT * FROM users WHERE name='{اسم}'\"}";
            finding.safeCode = "استعلام_آمن(\"SELECT * FROM users WHERE name=?\", اسم)";
            
            findings.push_back(finding);
        }
        
        // $ interpolation
        std::regex dollarPattern(R"(\$\{.*?\}.*?(SELECT|INSERT|UPDATE|DELETE)|["'].*?(SELECT|INSERT|UPDATE|DELETE).*?\$\{)");
        if (std::regex_search(line, match, dollarPattern)) {
            SQLInjectionFinding finding;
            finding.type = SQLInjectionType::InterpolatedVariable;
            finding.location = {filePath, lineNumber,
                               static_cast<int>(match.position()) + 1,
                               static_cast<int>(match.position() + match.length())};
            finding.code = line;
            finding.severity = 9;
            finding.message = "استخدام ${} مع SQL - خطر حقن!";
            finding.explanation = 
                "إدراج المتغيرات بـ ${} في استعلام SQL خطير.";
            finding.safeCode = "استخدم الاستعلامات المُعدّة";
            
            findings.push_back(finding);
        }
    }
    
    /**
     * تحديث سياق التحليل
     */
    void updateContext(
        const std::string& line,
        bool& inSQLBlock,
        std::string& sqlVariable
    ) {
        // كشف بداية بناء استعلام
        std::regex sqlAssignPattern(R"((\w+)\s*=\s*["'].*?(SELECT|INSERT|UPDATE|DELETE))");
        std::smatch match;
        
        if (std::regex_search(line, match, sqlAssignPattern)) {
            sqlVariable = match[1].str();
        }
        
        // كشف استمرار البناء
        if (!sqlVariable.empty()) {
            if (line.find(sqlVariable) != std::string::npos &&
                line.find("+=") != std::string::npos) {
                inSQLBlock = true;
            }
            
            // كشف نهاية (تنفيذ الاستعلام)
            std::regex execPattern(R"(نفذ|execute|استعلام|query)");
            if (std::regex_search(line, execPattern)) {
                inSQLBlock = false;
                sqlVariable.clear();
            }
        }
    }
    
    /**
     * فحص مع السياق
     */
    std::vector<SQLInjectionFinding> checkWithContext(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        bool inSQLBlock,
        const std::string& sqlVariable
    ) {
        std::vector<SQLInjectionFinding> findings;
        
        // إذا كنا في كتلة بناء SQL وهناك دمج
        if (inSQLBlock && line.find("+=") != std::string::npos) {
            SQLInjectionFinding finding;
            finding.type = SQLInjectionType::StringConcatenation;
            finding.location = {filePath, lineNumber, 1, static_cast<int>(line.length())};
            finding.code = line;
            finding.severity = 8;
            finding.message = "استمرار بناء استعلام SQL بالدمج";
            finding.explanation = 
                "بناء استعلام SQL على عدة أسطر بالدمج خطير\n"
                "لأنه من السهل نسيان تنظيف أحد المتغيرات.";
            finding.safeCode = "استخدم استعلام مُعد واحد مع كل المعاملات";
            
            findings.push_back(finding);
        }
        
        return findings;
    }
};

// =============================================================================
// مولد التقرير
// =============================================================================

/**
 * تقرير كشف حقن SQL
 */
class SQLInjectionReport {
public:
    void addFindings(const std::vector<SQLInjectionFinding>& findings) {
        findings_.insert(findings_.end(), findings.begin(), findings.end());
    }
    
    /**
     * توليد التقرير
     */
    std::string generate() const {
        std::ostringstream ss;
        
        ss << "\n";
        ss << "╔══════════════════════════════════════════════════════════════════════╗\n";
        ss << "║                    🛡️ تقرير كشف حقن SQL                              ║\n";
        ss << "║                     SQL Injection Detection Report                    ║\n";
        ss << "╚══════════════════════════════════════════════════════════════════════╝\n";
        
        if (findings_.empty()) {
            ss << "\n✅ لم يُكتشف أي خطر حقن SQL - الكود آمن!\n";
            return ss.str();
        }
        
        // إحصائيات
        int critical = 0, high = 0, medium = 0;
        for (const auto& f : findings_) {
            if (f.severity >= 9) critical++;
            else if (f.severity >= 7) high++;
            else medium++;
        }
        
        ss << "\n📊 الإحصائيات:\n";
        ss << "  • إجمالي الثغرات: " << findings_.size() << "\n";
        ss << "  • حرج (9-10): " << critical << "\n";
        ss << "  • عالي (7-8): " << high << "\n";
        ss << "  • متوسط (5-6): " << medium << "\n";
        
        // التفاصيل
        ss << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        
        for (const auto& finding : findings_) {
            ss << finding.format();
        }
        
        // نصائح عامة
        ss << "\n";
        ss << "╔══════════════════════════════════════════════════════════════════════╗\n";
        ss << "║                         💡 نصائح للحماية                              ║\n";
        ss << "╠══════════════════════════════════════════════════════════════════════╣\n";
        ss << "║  1. استخدم دائماً الاستعلامات المُعدّة (Prepared Statements)          ║\n";
        ss << "║  2. لا تدمج نصوص المستخدم مع استعلامات SQL أبداً                      ║\n";
        ss << "║  3. تحقق من نوع وطول المدخلات                                         ║\n";
        ss << "║  4. استخدم ORM بدلاً من SQL الخام                                     ║\n";
        ss << "║  5. طبّق مبدأ الحد الأدنى من الصلاحيات                                 ║\n";
        ss << "╚══════════════════════════════════════════════════════════════════════╝\n";
        
        return ss.str();
    }
    
    int getCount() const { return static_cast<int>(findings_.size()); }
    bool hasCritical() const {
        for (const auto& f : findings_) {
            if (f.severity >= 9) return true;
        }
        return false;
    }
    
private:
    std::vector<SQLInjectionFinding> findings_;
};

} // namespace sad::security::rules

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadSQLDetector {
    sad::security::rules::SQLInjectionDetector* detector;
    sad::security::rules::SQLInjectionReport* report;
};

SadSQLDetector* sad_sql_detector_new() {
    auto* ctx = new SadSQLDetector();
    ctx->detector = new sad::security::rules::SQLInjectionDetector();
    ctx->report = new sad::security::rules::SQLInjectionReport();
    return ctx;
}

void sad_sql_detector_free(SadSQLDetector* ctx) {
    if (ctx) {
        delete ctx->detector;
        delete ctx->report;
        delete ctx;
    }
}

int sad_sql_check_file(SadSQLDetector* ctx, const char* content, const char* file) {
    if (!ctx) return -1;
    auto findings = ctx->detector->checkFile(content, file);
    ctx->report->addFindings(findings);
    return static_cast<int>(findings.size());
}

void sad_sql_print_report(SadSQLDetector* ctx) {
    if (!ctx) return;
    std::cout << ctx->report->generate();
}

int sad_sql_has_critical(SadSQLDetector* ctx) {
    if (!ctx) return 0;
    return ctx->report->hasCritical() ? 1 : 0;
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef SQL_INJECTION_TEST

#include <cassert>

void testStringConcatenation() {
    sad::security::rules::SQLInjectionDetector detector;
    
    std::string dangerous = R"(استعلام("SELECT * FROM users WHERE name='" + اسم + "'"))";
    auto findings = detector.checkLine(dangerous, 1, "test.ص");
    
    assert(findings.size() >= 1);
    assert(findings[0].severity >= 8);
    
    std::cout << "✅ اختبار كشف دمج النصوص نجح\n";
}

void testSafeCode() {
    sad::security::rules::SQLInjectionDetector detector;
    
    std::string safe = R"(استعلام_آمن("SELECT * FROM users WHERE name=?", اسم))";
    auto findings = detector.checkLine(safe, 1, "test.ص");
    
    assert(findings.empty());
    
    std::cout << "✅ اختبار الكود الآمن نجح\n";
}

void testRawQuery() {
    sad::security::rules::SQLInjectionDetector detector;
    
    std::string raw = R"(قب.استعلام_خام("DROP TABLE users"))";
    auto findings = detector.checkLine(raw, 1, "test.ص");
    
    assert(findings.size() >= 1);
    
    std::cout << "✅ اختبار كشف الاستعلام الخام نجح\n";
}

void testFullFile() {
    sad::security::rules::SQLInjectionDetector detector;
    
    std::string code = R"(
دالة بحث_مستخدم(اسم: نص) -> مستخدم
    # خطير!
    استعلام = "SELECT * FROM users WHERE name='" + اسم + "'"
    إرجاع قب.نفذ(استعلام)
نهاية

دالة بحث_آمن(اسم: نص) -> مستخدم
    # آمن
    إرجاع قب.استعلام_آمن("SELECT * FROM users WHERE name=?", اسم)
نهاية
)";
    
    auto findings = detector.checkFile(code, "test.ص");
    
    // يجب أن يكتشف الدالة الأولى فقط
    assert(findings.size() >= 1);
    
    std::cout << "✅ اختبار فحص الملف الكامل نجح\n";
    
    // طباعة التقرير
    sad::security::rules::SQLInjectionReport report;
    report.addFindings(findings);
    std::cout << report.generate();
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات كاشف حقن SQL\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testStringConcatenation();
    testSafeCode();
    testRawQuery();
    testFullFile();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // SQL_INJECTION_TEST
