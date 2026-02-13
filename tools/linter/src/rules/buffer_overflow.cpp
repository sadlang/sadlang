/**
 * =============================================================================
 * ملف: buffer_overflow.cpp
 * الوصف: كاشف طفح المخزن المؤقت في لغة ص
 * المهمة: T221 - Buffer overflow detector
 * المرحلة: Phase 22 - User Story 19 (Security Linting ص حارس)
 * =============================================================================
 * 
 * 📦 دليل المبتدئ لفهم طفح المخزن المؤقت
 * ═══════════════════════════════════════
 * 
 * ما هو المخزن المؤقت (Buffer)؟
 * ─────────────────────────────
 * تخيل صندوقاً به 10 خانات. كل خانة تسع حرفاً واحداً.
 * 
 * المخزن المؤقت مثل هذا الصندوق في الذاكرة:
 * ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
 * │ │ │ │ │ │ │ │ │ │ │  ← 10 خانات
 * └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘
 * 
 * ما هو طفح المخزن (Buffer Overflow)؟
 * ───────────────────────────────────
 * عندما تحاول وضع 15 حرفاً في صندوق يسع 10 فقط!
 * 
 * ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐ ─ ─ ─ ─ ─
 * │A│B│C│D│E│F│G│H│I│J│ K │ L │ M │ ← الأحرف الزائدة تتجاوز!
 * └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘ ─ ─ ─ ─ ─
 *                        ↑ ↑ ↑
 *                    هذه تكتب فوق ذاكرة أخرى!
 * 
 * لماذا هذا خطير؟
 * ────────────────
 * الذاكرة المجاورة قد تحتوي على:
 * 1. عنوان العودة من الدالة ← المهاجم يتحكم بتنفيذ الكود!
 * 2. متغيرات أمنية ← المهاجم يغير كلمات السر!
 * 3. مؤشرات ← المهاجم يوجه البرنامج لأي مكان!
 * 
 * مثال تاريخي:
 * ─────────────
 * Morris Worm (1988) - أول دودة إنترنت استخدمت buffer overflow
 * أصابت 10% من الإنترنت في ذلك الوقت!
 * 
 * كيف تحمي نفسك؟
 * ────────────────
 * 1. استخدم دوال آمنة (مثل نسخ_آمن بدلاً من نسخ)
 * 2. تحقق دائماً من حجم البيانات قبل النسخ
 * 3. استخدم أنواع لغة ص الآمنة (متجه، نص)
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <regex>
#include <map>
#include <sstream>
#include <iostream>
#include <memory>
#include <set>

namespace sad::security::rules {

// =============================================================================
// أنواع ثغرات طفح المخزن
// =============================================================================

/**
 * نوع ثغرة طفح المخزن
 */
enum class BufferOverflowType {
    StackOverflow,      // طفح المكدس
    HeapOverflow,       // طفح الكومة
    IntegerOverflow,    // طفح عددي يؤدي لطفح مخزن
    OffByOne,           // خطأ بواحد
    FormatString,       // ثغرة سلسلة التنسيق
    UnsafeCopy,         // نسخ غير آمن
    UnsafeRead,         // قراءة غير آمنة
    ArrayOutOfBounds,   // تجاوز حدود المصفوفة
    NullTerminator      // مشكلة في منهي السلسلة
};

inline std::string overflowTypeToArabic(BufferOverflowType type) {
    switch (type) {
        case BufferOverflowType::StackOverflow:    return "طفح المكدس";
        case BufferOverflowType::HeapOverflow:     return "طفح الكومة";
        case BufferOverflowType::IntegerOverflow:  return "طفح عددي";
        case BufferOverflowType::OffByOne:         return "خطأ بواحد";
        case BufferOverflowType::FormatString:     return "سلسلة تنسيق";
        case BufferOverflowType::UnsafeCopy:       return "نسخ غير آمن";
        case BufferOverflowType::UnsafeRead:       return "قراءة غير آمنة";
        case BufferOverflowType::ArrayOutOfBounds: return "تجاوز حدود المصفوفة";
        case BufferOverflowType::NullTerminator:   return "منهي سلسلة مفقود";
    }
    return "غير معروف";
}

// =============================================================================
// نتيجة الكشف
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

/**
 * نتيجة كشف طفح المخزن
 */
struct BufferOverflowFinding {
    BufferOverflowType type;
    SourceLocation location;
    std::string code;
    std::string message;
    std::string explanation;
    std::string unsafeCode;
    std::string safeCode;
    int severity = 8;
    std::string cweId;  // Common Weakness Enumeration
    
    std::string format() const {
        std::ostringstream ss;
        
        ss << "\n";
        ss << "╔══════════════════════════════════════════════════════════════╗\n";
        ss << "║  📦 تحذير طفح المخزن - " << overflowTypeToArabic(type) << "\n";
        ss << "╠══════════════════════════════════════════════════════════════╣\n";
        ss << "║  📍 الموقع: " << location.toString() << "\n";
        ss << "║  ⚠️ الخطورة: " << severity << "/10\n";
        if (!cweId.empty()) {
            ss << "║  🔗 CWE: " << cweId << "\n";
        }
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
// كاشف طفح المخزن
// =============================================================================

/**
 * كاشف ثغرات طفح المخزن
 */
class BufferOverflowDetector {
public:
    /**
     * فحص سطر
     */
    std::vector<BufferOverflowFinding> checkLine(
        const std::string& line,
        int lineNumber,
        const std::string& filePath
    ) {
        std::vector<BufferOverflowFinding> findings;
        
        // 1. كشف دوال النسخ الخطيرة
        checkUnsafeCopyFunctions(line, lineNumber, filePath, findings);
        
        // 2. كشف دوال القراءة الخطيرة
        checkUnsafeReadFunctions(line, lineNumber, filePath, findings);
        
        // 3. كشف تخصيص مصفوفة بحجم متغير
        checkVariableSizeArray(line, lineNumber, filePath, findings);
        
        // 4. كشف الوصول للمصفوفات بدون تحقق
        checkUncheckedArrayAccess(line, lineNumber, filePath, findings);
        
        // 5. كشف سلاسل التنسيق الخطيرة
        checkFormatStrings(line, lineNumber, filePath, findings);
        
        // 6. كشف العمليات الحسابية الخطيرة على الأحجام
        checkSizeArithmetic(line, lineNumber, filePath, findings);
        
        return findings;
    }
    
    /**
     * فحص ملف كامل
     */
    std::vector<BufferOverflowFinding> checkFile(
        const std::string& content,
        const std::string& filePath
    ) {
        std::vector<BufferOverflowFinding> allFindings;
        std::istringstream stream(content);
        std::string line;
        int lineNumber = 0;
        
        // تتبع السياق
        Context ctx;
        
        while (std::getline(stream, line)) {
            lineNumber++;
            
            // تحديث السياق
            updateContext(line, ctx);
            
            // فحص السطر
            auto findings = checkLine(line, lineNumber, filePath);
            
            // فحص إضافي مع السياق
            auto contextFindings = checkWithContext(line, lineNumber, filePath, ctx);
            findings.insert(findings.end(), contextFindings.begin(), contextFindings.end());
            
            allFindings.insert(allFindings.end(), findings.begin(), findings.end());
        }
        
        return allFindings;
    }
    
private:
    /**
     * سياق التحليل
     */
    struct Context {
        std::map<std::string, int> bufferSizes;     // أحجام المخازن المعروفة
        std::set<std::string> checkedVariables;     // متغيرات تم التحقق منها
        bool inUnsafeBlock = false;                 // داخل كتلة غير آمنة
    };
    
    /**
     * كشف دوال النسخ الخطيرة
     */
    void checkUnsafeCopyFunctions(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<BufferOverflowFinding>& findings
    ) {
        // دوال خطيرة: strcpy, memcpy بدون حجم، sprintf
        std::vector<std::pair<std::regex, std::string>> patterns = {
            // نسخ بدون تحديد الحجم
            {std::regex(R"(\b(strcpy|انسخ_نص)\s*\()"), "strcpy/انسخ_نص"},
            {std::regex(R"(\b(strcat|الحق_نص)\s*\()"), "strcat/الحق_نص"},
            {std::regex(R"(\b(sprintf|نسق_نص)\s*\()"), "sprintf/نسق_نص"},
            {std::regex(R"(\b(gets|اقرأ_سطر)\s*\()"), "gets/اقرأ_سطر"},
        };
        
        for (const auto& [pattern, name] : patterns) {
            std::smatch match;
            if (std::regex_search(line, match, pattern)) {
                BufferOverflowFinding finding;
                finding.type = BufferOverflowType::UnsafeCopy;
                finding.location = {filePath, lineNumber, static_cast<int>(match.position()) + 1};
                finding.code = line;
                finding.severity = 9;
                finding.cweId = "CWE-120";
                finding.message = "استخدام دالة نسخ خطيرة: " + name;
                finding.explanation = 
                    "هذه الدالة لا تتحقق من حجم الوجهة.\n"
                    "إذا كان المصدر أكبر من الوجهة، سيحدث طفح.\n"
                    "استخدم البدائل الآمنة التي تحدد الحجم الأقصى.";
                
                if (name == "strcpy/انسخ_نص") {
                    finding.unsafeCode = "انسخ_نص(وجهة, مصدر)";
                    finding.safeCode = "انسخ_نص_آمن(وجهة, مصدر, حجم_الوجهة)";
                } else if (name == "gets/اقرأ_سطر") {
                    finding.unsafeCode = "اقرأ_سطر(مخزن)";
                    finding.safeCode = "اقرأ_سطر_آمن(مخزن, حجم_المخزن)";
                } else if (name == "sprintf/نسق_نص") {
                    finding.unsafeCode = "نسق_نص(مخزن, \"%s\", قيمة)";
                    finding.safeCode = "نسق_نص_آمن(مخزن, حجم_المخزن, \"%s\", قيمة)";
                }
                
                findings.push_back(finding);
            }
        }
    }
    
    /**
     * كشف دوال القراءة الخطيرة
     */
    void checkUnsafeReadFunctions(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<BufferOverflowFinding>& findings
    ) {
        // scanf بدون تحديد العرض
        std::regex scanfPattern(R"(\b(scanf|اقرأ)\s*\(\s*["'][^"']*%s[^"']*["'])");
        std::smatch match;
        
        if (std::regex_search(line, match, scanfPattern)) {
            // تحقق إذا كان هناك عرض محدد مثل %10s
            if (line.find("%10s") == std::string::npos &&
                line.find("%20s") == std::string::npos) {
                BufferOverflowFinding finding;
                finding.type = BufferOverflowType::UnsafeRead;
                finding.location = {filePath, lineNumber, static_cast<int>(match.position()) + 1};
                finding.code = line;
                finding.severity = 8;
                finding.cweId = "CWE-120";
                finding.message = "قراءة نص بدون تحديد الحجم الأقصى";
                finding.explanation = 
                    "استخدام %s في scanf بدون تحديد العرض يسمح\n"
                    "للمستخدم بإدخال نص أطول من المخزن.";
                finding.unsafeCode = R"(اقرأ("%s", مخزن))";
                finding.safeCode = R"(اقرأ("%99s", مخزن)  # تحديد الحد الأقصى 99 حرف)";
                
                findings.push_back(finding);
            }
        }
    }
    
    /**
     * كشف مصفوفات بحجم متغير
     */
    void checkVariableSizeArray(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<BufferOverflowFinding>& findings
    ) {
        // مصفوفة بحجم من المستخدم
        std::regex vlaPattern(R"(\[\s*(\w+)\s*\]\s*=|\bمصفوفة\s*\[\s*(\w+)\s*\])");
        std::smatch match;
        
        if (std::regex_search(line, match, vlaPattern)) {
            std::string sizeVar = match[1].matched ? match[1].str() : match[2].str();
            
            // إذا كان المتغير من إدخال المستخدم
            if (line.find("مدخل") != std::string::npos ||
                line.find("input") != std::string::npos ||
                line.find("اقرأ") != std::string::npos) {
                
                BufferOverflowFinding finding;
                finding.type = BufferOverflowType::StackOverflow;
                finding.location = {filePath, lineNumber, static_cast<int>(match.position()) + 1};
                finding.code = line;
                finding.severity = 7;
                finding.cweId = "CWE-121";
                finding.message = "مصفوفة بحجم من إدخال المستخدم على المكدس";
                finding.explanation = 
                    "تخصيص مصفوفة على المكدس بحجم يحدده المستخدم\n"
                    "يمكن أن يسبب طفح المكدس إذا كان الحجم كبيراً جداً.\n"
                    "استخدم تخصيصاً على الكومة مع تحقق من الحجم.";
                finding.unsafeCode = "متغير مخزن[حجم_من_المستخدم]";
                finding.safeCode = "متغير مخزن = خصص_آمن<نوع>(الحجم_الأقصى.حد_أدنى(حجم_المستخدم))";
                
                findings.push_back(finding);
            }
        }
    }
    
    /**
     * كشف الوصول للمصفوفات بدون تحقق
     */
    void checkUncheckedArrayAccess(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<BufferOverflowFinding>& findings
    ) {
        // وصول لمصفوفة بمتغير
        std::regex accessPattern(R"(\[\s*(\w+)\s*\])");
        std::smatch match;
        std::string::const_iterator searchStart(line.cbegin());
        
        while (std::regex_search(searchStart, line.cend(), match, accessPattern)) {
            std::string indexVar = match[1].str();
            
            // تجاهل الثوابت
            bool isConstant = true;
            for (char c : indexVar) {
                if (!std::isdigit(c)) {
                    isConstant = false;
                    break;
                }
            }
            
            if (!isConstant) {
                // تحقق إذا كان هناك فحص حدود قبل الوصول
                bool hasCheck = line.find("إذا") != std::string::npos ||
                               line.find("if") != std::string::npos ||
                               line.find("تحقق") != std::string::npos ||
                               line.find("<") != std::string::npos;
                
                if (!hasCheck) {
                    // Low severity لأنه قد يكون هناك تحقق في سطر سابق
                    BufferOverflowFinding finding;
                    finding.type = BufferOverflowType::ArrayOutOfBounds;
                    finding.location = {filePath, lineNumber, 
                                       static_cast<int>(match.position()) + 1};
                    finding.code = line;
                    finding.severity = 5;
                    finding.cweId = "CWE-125";
                    finding.message = "وصول لمصفوفة بفهرس متغير - تأكد من فحص الحدود";
                    finding.explanation = 
                        "الوصول لمصفوفة بفهرس متغير بدون التحقق من الحدود\n"
                        "قد يؤدي لقراءة/كتابة خارج حدود المصفوفة.";
                    finding.unsafeCode = "مصفوفة[فهرس]";
                    finding.safeCode = R"(إذا فهرس < طول(مصفوفة)
    مصفوفة[فهرس]
نهاية)";
                    
                    findings.push_back(finding);
                }
            }
            
            searchStart = match.suffix().first;
        }
    }
    
    /**
     * كشف ثغرات سلاسل التنسيق
     */
    void checkFormatStrings(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<BufferOverflowFinding>& findings
    ) {
        // printf مع متغير كسلسلة تنسيق
        std::regex fmtPattern(R"(\b(printf|اطبع_منسق)\s*\(\s*(\w+)\s*\))");
        std::smatch match;
        
        if (std::regex_search(line, match, fmtPattern)) {
            BufferOverflowFinding finding;
            finding.type = BufferOverflowType::FormatString;
            finding.location = {filePath, lineNumber, static_cast<int>(match.position()) + 1};
            finding.code = line;
            finding.severity = 9;
            finding.cweId = "CWE-134";
            finding.message = "سلسلة تنسيق من متغير - خطر أمني!";
            finding.explanation = 
                "إذا كانت سلسلة التنسيق من إدخال المستخدم،\n"
                "يمكنه استخدام %n لكتابة في الذاكرة\n"
                "أو %x لقراءة المكدس!";
            finding.unsafeCode = "اطبع_منسق(نص_من_المستخدم)";
            finding.safeCode = R"(اطبع_منسق("%s", نص_من_المستخدم))";
            
            findings.push_back(finding);
        }
    }
    
    /**
     * كشف عمليات حسابية خطيرة على الأحجام
     */
    void checkSizeArithmetic(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        std::vector<BufferOverflowFinding>& findings
    ) {
        // حجم * عدد بدون تحقق من الطفح
        std::regex sizeMultPattern(R"(\b(حجم|size|عدد|count|length)\s*\*\s*\d+)");
        std::smatch match;
        
        if (std::regex_search(line, match, sizeMultPattern)) {
            // تحقق إذا كان داخل دالة آمنة
            if (line.find("تحقق_طفح") == std::string::npos &&
                line.find("check_overflow") == std::string::npos) {
                
                BufferOverflowFinding finding;
                finding.type = BufferOverflowType::IntegerOverflow;
                finding.location = {filePath, lineNumber, static_cast<int>(match.position()) + 1};
                finding.code = line;
                finding.severity = 7;
                finding.cweId = "CWE-190";
                finding.message = "عملية ضرب على حجم بدون تحقق من الطفح العددي";
                finding.explanation = 
                    "إذا كان الحجم كبيراً، الضرب قد يتجاوز الحد الأقصى للعدد\n"
                    "ويصبح رقماً صغيراً، مما يؤدي لتخصيص مخزن أصغر من المطلوب.";
                finding.unsafeCode = "حجم * عدد_العناصر";
                finding.safeCode = R"(إذا حجم > الحد_الأقصى / عدد_العناصر
    خطأ("سيحدث طفح!")
نهاية
حجم * عدد_العناصر)";
                
                findings.push_back(finding);
            }
        }
    }
    
    /**
     * تحديث السياق
     */
    void updateContext(const std::string& line, Context& ctx) {
        // كشف تعريف مخزن بحجم معروف
        std::regex bufDeclPattern(R"(متغير\s+(\w+)\s*\[\s*(\d+)\s*\])");
        std::smatch match;
        
        if (std::regex_search(line, match, bufDeclPattern)) {
            ctx.bufferSizes[match[1].str()] = std::stoi(match[2].str());
        }
        
        // كشف فحص حدود
        std::regex checkPattern(R"(إذا\s+(\w+)\s*<\s*(\d+|\w+))");
        if (std::regex_search(line, match, checkPattern)) {
            ctx.checkedVariables.insert(match[1].str());
        }
        
        // كشف كتلة غير آمنة
        if (line.find("غير_آمن") != std::string::npos) {
            ctx.inUnsafeBlock = true;
        }
        if (line.find("نهاية") != std::string::npos && ctx.inUnsafeBlock) {
            ctx.inUnsafeBlock = false;
        }
    }
    
    /**
     * فحص مع السياق
     */
    std::vector<BufferOverflowFinding> checkWithContext(
        const std::string& line,
        int lineNumber,
        const std::string& filePath,
        const Context& ctx
    ) {
        std::vector<BufferOverflowFinding> findings;
        
        // فحص نسخ بين مخازن معروفة الحجم
        for (const auto& [bufName, bufSize] : ctx.bufferSizes) {
            // نسخ إلى مخزن معروف
            std::regex copyToPattern("انسخ.*" + bufName);
            if (std::regex_search(line, copyToPattern)) {
                // إذا كان المصدر أكبر
                for (const auto& [srcName, srcSize] : ctx.bufferSizes) {
                    if (srcSize > bufSize && line.find(srcName) != std::string::npos) {
                        BufferOverflowFinding finding;
                        finding.type = BufferOverflowType::UnsafeCopy;
                        finding.location = {filePath, lineNumber, 1};
                        finding.code = line;
                        finding.severity = 8;
                        finding.message = "نسخ من مخزن أكبر (" + std::to_string(srcSize) +
                                         ") إلى أصغر (" + std::to_string(bufSize) + ")";
                        finding.explanation = 
                            "المخزن المصدر أكبر من الوجهة.\n"
                            "سيحدث طفح إذا كان المصدر ممتلئاً.";
                        
                        findings.push_back(finding);
                    }
                }
            }
        }
        
        return findings;
    }
};

// =============================================================================
// تقرير طفح المخزن
// =============================================================================

class BufferOverflowReport {
public:
    void addFindings(const std::vector<BufferOverflowFinding>& findings) {
        findings_.insert(findings_.end(), findings.begin(), findings.end());
    }
    
    std::string generate() const {
        std::ostringstream ss;
        
        ss << "\n";
        ss << "╔══════════════════════════════════════════════════════════════════════╗\n";
        ss << "║                  📦 تقرير كشف طفح المخزن المؤقت                       ║\n";
        ss << "║                    Buffer Overflow Detection Report                   ║\n";
        ss << "╚══════════════════════════════════════════════════════════════════════╝\n";
        
        if (findings_.empty()) {
            ss << "\n✅ لم يُكتشف أي خطر طفح مخزن - الكود آمن!\n";
            return ss.str();
        }
        
        // إحصائيات
        std::map<BufferOverflowType, int> typeCounts;
        int critical = 0, high = 0, medium = 0;
        
        for (const auto& f : findings_) {
            typeCounts[f.type]++;
            if (f.severity >= 9) critical++;
            else if (f.severity >= 7) high++;
            else medium++;
        }
        
        ss << "\n📊 الإحصائيات:\n";
        ss << "  • إجمالي الثغرات: " << findings_.size() << "\n";
        ss << "  • حرج (9-10): " << critical << "\n";
        ss << "  • عالي (7-8): " << high << "\n";
        ss << "  • متوسط (5-6): " << medium << "\n";
        ss << "\n  حسب النوع:\n";
        for (const auto& [type, count] : typeCounts) {
            ss << "  • " << overflowTypeToArabic(type) << ": " << count << "\n";
        }
        
        // التفاصيل
        for (const auto& finding : findings_) {
            ss << finding.format();
        }
        
        // النصائح
        ss << "\n";
        ss << "╔══════════════════════════════════════════════════════════════════════╗\n";
        ss << "║                       💡 نصائح للحماية                                ║\n";
        ss << "╠══════════════════════════════════════════════════════════════════════╣\n";
        ss << "║  1. استخدم دائماً الدوال الآمنة (نسخ_آمن، قراءة_آمن)                   ║\n";
        ss << "║  2. تحقق من الحجم قبل كل عملية نسخ                                    ║\n";
        ss << "║  3. استخدم أنواع لغة ص الآمنة (متجه<T>، نص)                           ║\n";
        ss << "║  4. فعّل حماية المكدس (Stack Protection)                              ║\n";
        ss << "║  5. استخدم ASLR و DEP                                                ║\n";
        ss << "╚══════════════════════════════════════════════════════════════════════╝\n";
        
        return ss.str();
    }
    
    int getCount() const { return static_cast<int>(findings_.size()); }
    
private:
    std::vector<BufferOverflowFinding> findings_;
};

} // namespace sad::security::rules

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadBufferDetector {
    sad::security::rules::BufferOverflowDetector* detector;
    sad::security::rules::BufferOverflowReport* report;
};

SadBufferDetector* sad_buffer_detector_new() {
    auto* ctx = new SadBufferDetector();
    ctx->detector = new sad::security::rules::BufferOverflowDetector();
    ctx->report = new sad::security::rules::BufferOverflowReport();
    return ctx;
}

void sad_buffer_detector_free(SadBufferDetector* ctx) {
    if (ctx) {
        delete ctx->detector;
        delete ctx->report;
        delete ctx;
    }
}

int sad_buffer_check_file(SadBufferDetector* ctx, const char* content, const char* file) {
    if (!ctx) return -1;
    auto findings = ctx->detector->checkFile(content, file);
    ctx->report->addFindings(findings);
    return static_cast<int>(findings.size());
}

void sad_buffer_print_report(SadBufferDetector* ctx) {
    if (!ctx) return;
    std::cout << ctx->report->generate();
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef BUFFER_OVERFLOW_TEST

#include <cassert>

void testUnsafeCopy() {
    sad::security::rules::BufferOverflowDetector detector;
    
    std::string dangerous = "انسخ_نص(وجهة, مصدر)";
    auto findings = detector.checkLine(dangerous, 1, "test.ص");
    
    assert(findings.size() >= 1);
    assert(findings[0].type == sad::security::rules::BufferOverflowType::UnsafeCopy);
    
    std::cout << "✅ اختبار كشف النسخ الخطير نجح\n";
}

void testFormatString() {
    sad::security::rules::BufferOverflowDetector detector;
    
    std::string dangerous = "اطبع_منسق(مدخل_المستخدم)";
    auto findings = detector.checkLine(dangerous, 1, "test.ص");
    
    assert(findings.size() >= 1);
    assert(findings[0].type == sad::security::rules::BufferOverflowType::FormatString);
    
    std::cout << "✅ اختبار كشف سلسلة التنسيق نجح\n";
}

void testSafeCode() {
    sad::security::rules::BufferOverflowDetector detector;
    
    std::string safe = "انسخ_نص_آمن(وجهة, مصدر, حجم_الوجهة)";
    auto findings = detector.checkLine(safe, 1, "test.ص");
    
    assert(findings.empty());
    
    std::cout << "✅ اختبار الكود الآمن نجح\n";
}

void testFullFile() {
    sad::security::rules::BufferOverflowDetector detector;
    
    std::string code = R"(
دالة معالجة_مدخل(نص: مصفوفة[256])
    متغير مخزن[64]
    
    # خطير!
    انسخ_نص(مخزن, نص)
    
    # خطير أيضاً!
    اطبع_منسق(نص)
    
    # آمن
    انسخ_نص_آمن(مخزن, نص, 64)
نهاية
)";
    
    auto findings = detector.checkFile(code, "test.ص");
    
    assert(findings.size() >= 2);
    
    std::cout << "✅ اختبار فحص الملف نجح\n";
    
    sad::security::rules::BufferOverflowReport report;
    report.addFindings(findings);
    std::cout << report.generate();
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات كاشف طفح المخزن المؤقت\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testUnsafeCopy();
    testFormatString();
    testSafeCode();
    testFullFile();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // BUFFER_OVERFLOW_TEST
