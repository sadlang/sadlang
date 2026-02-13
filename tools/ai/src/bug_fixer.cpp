/**
 * ==========================================================
 * ملف: bug_fixer.cpp
 * الوصف: إصلاح الأخطاء تلقائياً
 * المهمة: T304 - نظام ذكاء ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🔧 ما هو مصلح الأخطاء؟
 * ----------------------
 * برنامج يكتشف الأخطاء في الكود ويصلحها تلقائياً!
 * مثل المدقق الإملائي في معالج النصوص.
 * 
 * 🐛 أنواع الأخطاء:
 * -----------------
 * 1. أخطاء نحوية: مثل نسيان "نهاية"
 * 2. أخطاء منطقية: مثل شرط دائماً صح
 * 3. أخطاء الملكية: مثل استخدام متغير منقول
 * 4. أخطاء الأداء: مثل حلقة غير فعالة
 * 
 * 📝 مثال:
 * --------
 * الكود الخاطئ:
 *   دالة مرحبا()
 *       اطبع("مرحبا")
 *   // نسي كلمة نهاية
 * 
 * بعد الإصلاح:
 *   دالة مرحبا()
 *       اطبع("مرحبا")
 *   نهاية  // ← تمت الإضافة تلقائياً
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <regex>

namespace sad {
namespace ai {

// ==========================================================
// 📌 أنواع الإصلاحات
// ==========================================================

/**
 * 🏷️ نوع الإصلاح
 */
enum class FixType {
    ADD,            // إضافة كود
    REMOVE,         // حذف كود
    REPLACE,        // استبدال كود
    MOVE,           // نقل كود
    SUGGEST         // اقتراح فقط
};

/**
 * 📝 أسماء أنواع الإصلاحات
 */
const char* fixTypeName(FixType type) {
    switch (type) {
        case FixType::ADD: return "إضافة";
        case FixType::REMOVE: return "حذف";
        case FixType::REPLACE: return "استبدال";
        case FixType::MOVE: return "نقل";
        case FixType::SUGGEST: return "اقتراح";
        default: return "غير معروف";
    }
}

// ==========================================================
// 📌 إصلاح مقترح
// ==========================================================

/**
 * 🔧 إصلاح مقترح
 */
struct Fix {
    FixType type;
    int startLine;          // بداية النطاق
    int endLine;            // نهاية النطاق
    int startCol;           // عمود البداية
    int endCol;             // عمود النهاية
    std::string oldCode;    // الكود القديم
    std::string newCode;    // الكود الجديد
    std::string explanation;// شرح الإصلاح
    int confidence;         // الثقة (0-100)
    
    /**
     * تحويل لـ string
     */
    std::string toString() const {
        std::ostringstream oss;
        oss << "🔧 " << fixTypeName(type) << " (ثقة: " << confidence << "%)\n";
        oss << "   السطر: " << startLine;
        if (endLine != startLine) {
            oss << "-" << endLine;
        }
        oss << "\n";
        if (!oldCode.empty()) {
            oss << "   القديم: " << oldCode << "\n";
        }
        oss << "   الجديد: " << newCode << "\n";
        oss << "   السبب: " << explanation << "\n";
        return oss.str();
    }
    
    /**
     * تحويل لـ JSON
     */
    std::string toJson() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"type\": \"" << fixTypeName(type) << "\",\n";
        oss << "  \"range\": {\n";
        oss << "    \"start\": {\"line\": " << startLine << ", \"col\": " << startCol << "},\n";
        oss << "    \"end\": {\"line\": " << endLine << ", \"col\": " << endCol << "}\n";
        oss << "  },\n";
        oss << "  \"oldCode\": \"" << escapeJson(oldCode) << "\",\n";
        oss << "  \"newCode\": \"" << escapeJson(newCode) << "\",\n";
        oss << "  \"explanation\": \"" << escapeJson(explanation) << "\",\n";
        oss << "  \"confidence\": " << confidence << "\n";
        oss << "}";
        return oss.str();
    }
    
private:
    static std::string escapeJson(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

// ==========================================================
// 📌 نتيجة التحليل
// ==========================================================

/**
 * 📊 نتيجة تحليل الأخطاء
 */
struct AnalysisResult {
    std::vector<Fix> fixes;
    int totalErrors = 0;
    int fixableErrors = 0;
    
    /**
     * تطبيق الإصلاحات على الكود
     */
    std::string apply(const std::string& code) {
        if (fixes.empty()) return code;
        
        std::vector<std::string> lines = splitLines(code);
        
        // ترتيب الإصلاحات من الأسفل للأعلى (لتجنب تغير أرقام السطور)
        std::vector<Fix> sortedFixes = fixes;
        std::sort(sortedFixes.begin(), sortedFixes.end(),
                 [](const Fix& a, const Fix& b) {
                     return a.startLine > b.startLine;
                 });
        
        // تطبيق كل إصلاح
        for (const auto& fix : sortedFixes) {
            if (fix.confidence < 50) continue; // تخطي الإصلاحات غير الموثوقة
            
            int lineIdx = fix.startLine - 1;
            if (lineIdx < 0 || lineIdx >= static_cast<int>(lines.size())) continue;
            
            switch (fix.type) {
                case FixType::ADD:
                    // إضافة بعد السطر المحدد
                    lines.insert(lines.begin() + lineIdx + 1, fix.newCode);
                    break;
                case FixType::REMOVE:
                    lines.erase(lines.begin() + lineIdx);
                    break;
                case FixType::REPLACE:
                    lines[lineIdx] = fix.newCode;
                    break;
                default:
                    break;
            }
        }
        
        // دمج السطور
        std::ostringstream oss;
        for (size_t i = 0; i < lines.size(); ++i) {
            if (i > 0) oss << "\n";
            oss << lines[i];
        }
        return oss.str();
    }
    
    /**
     * تقرير الإصلاحات
     */
    std::string report() const {
        std::ostringstream oss;
        
        oss << "═══════════════════════════════════\n";
        oss << "      تقرير إصلاح الأخطاء\n";
        oss << "═══════════════════════════════════\n\n";
        
        oss << "📊 الإحصائيات:\n";
        oss << "   إجمالي الأخطاء: " << totalErrors << "\n";
        oss << "   قابلة للإصلاح: " << fixableErrors << "\n";
        oss << "   الإصلاحات المقترحة: " << fixes.size() << "\n\n";
        
        if (!fixes.empty()) {
            oss << "🔧 الإصلاحات:\n";
            oss << "───────────────────────────────────\n";
            for (const auto& fix : fixes) {
                oss << fix.toString() << "\n";
            }
        }
        
        return oss.str();
    }
    
private:
    static std::vector<std::string> splitLines(const std::string& str) {
        std::vector<std::string> lines;
        std::istringstream iss(str);
        std::string line;
        while (std::getline(iss, line)) {
            lines.push_back(line);
        }
        return lines;
    }
};

// ==========================================================
// 📌 مصلح الأخطاء
// ==========================================================

/**
 * 🔧 مصلح الأخطاء
 */
class BugFixer {
private:
    bool autoFix_ = false;      // تطبيق تلقائي
    int minConfidence_ = 80;    // أدنى ثقة للتطبيق
    
public:
    /**
     * تعيين الإصلاح التلقائي
     */
    void setAutoFix(bool autoFix) { autoFix_ = autoFix; }
    
    /**
     * تعيين أدنى ثقة
     */
    void setMinConfidence(int confidence) { minConfidence_ = confidence; }
    
    /**
     * تحليل وإصلاح الكود
     */
    AnalysisResult analyzeAndFix(const std::string& code) {
        AnalysisResult result;
        
        // 1. فحص "نهاية" المفقودة
        auto missingEnds = findMissingEnds(code);
        result.fixes.insert(result.fixes.end(), missingEnds.begin(), missingEnds.end());
        
        // 2. فحص المتغيرات غير المستخدمة
        auto unusedVars = findUnusedVariables(code);
        result.fixes.insert(result.fixes.end(), unusedVars.begin(), unusedVars.end());
        
        // 3. فحص أخطاء الملكية
        auto ownershipFixes = findOwnershipIssues(code);
        result.fixes.insert(result.fixes.end(), ownershipFixes.begin(), ownershipFixes.end());
        
        // 4. فحص الأخطاء النحوية الشائعة
        auto syntaxFixes = findSyntaxIssues(code);
        result.fixes.insert(result.fixes.end(), syntaxFixes.begin(), syntaxFixes.end());
        
        // 5. فحص أخطاء الأداء
        auto perfFixes = findPerformanceIssues(code);
        result.fixes.insert(result.fixes.end(), perfFixes.begin(), perfFixes.end());
        
        result.totalErrors = result.fixes.size();
        result.fixableErrors = 0;
        for (const auto& fix : result.fixes) {
            if (fix.confidence >= minConfidence_) {
                result.fixableErrors++;
            }
        }
        
        return result;
    }
    
    /**
     * إصلاح من رسالة خطأ
     */
    Fix fixFromError(const std::string& errorMessage, 
                     const std::string& code,
                     int line) {
        Fix fix;
        fix.startLine = line;
        fix.endLine = line;
        fix.confidence = 80;
        
        // تحليل رسالة الخطأ
        if (errorMessage.find("غير معرّف") != std::string::npos) {
            // استخراج اسم المتغير
            std::string varName = extractIdentifier(errorMessage);
            fix.type = FixType::ADD;
            fix.newCode = "متغير " + varName + " = 0 // TODO: تحديد القيمة";
            fix.explanation = "تعريف المتغير المفقود";
            fix.startLine = line - 1; // إضافة قبل السطر الحالي
        } else if (errorMessage.find("نهاية") != std::string::npos) {
            fix.type = FixType::ADD;
            fix.newCode = "نهاية";
            fix.explanation = "إضافة كلمة نهاية المفقودة";
            fix.confidence = 95;
        } else if (errorMessage.find("نوع") != std::string::npos) {
            fix.type = FixType::SUGGEST;
            fix.explanation = "تحقق من توافق الأنواع";
            fix.confidence = 50;
        } else if (errorMessage.find("ملكية") != std::string::npos) {
            fix.type = FixType::REPLACE;
            fix.explanation = "إضافة .استنسخ() لتجنب نقل الملكية";
            // يحتاج تحليل أعمق للكود
        } else {
            fix.type = FixType::SUGGEST;
            fix.explanation = "راجع الخطأ يدوياً";
            fix.confidence = 30;
        }
        
        return fix;
    }
    
private:
    /**
     * إيجاد "نهاية" المفقودة
     */
    std::vector<Fix> findMissingEnds(const std::string& code) {
        std::vector<Fix> fixes;
        
        // عد الكتل المفتوحة
        int openBlocks = 0;
        std::vector<std::pair<int, std::string>> blockStack; // (سطر، نوع)
        
        auto lines = splitLines(code);
        for (size_t i = 0; i < lines.size(); ++i) {
            std::string line = trim(lines[i]);
            
            // كتل تبدأ
            if (line.find("دالة ") == 0 || line.find("هيكل ") == 0 ||
                line.find("إذا ") == 0 || line.find("لكل ") == 0 ||
                line.find("بينما ") == 0 || line.find("اختبار ") == 0) {
                std::string blockType = line.substr(0, line.find(' '));
                blockStack.push_back({static_cast<int>(i + 1), blockType});
                openBlocks++;
            }
            
            // كتل تنتهي
            if (line == "نهاية") {
                if (!blockStack.empty()) {
                    blockStack.pop_back();
                    openBlocks--;
                }
            }
        }
        
        // إضافة إصلاحات للكتل غير المغلقة
        for (const auto& [lineNum, blockType] : blockStack) {
            Fix fix;
            fix.type = FixType::ADD;
            fix.startLine = static_cast<int>(lines.size());
            fix.endLine = fix.startLine;
            fix.newCode = "نهاية";
            fix.explanation = "إغلاق كتلة '" + blockType + "' في السطر " + 
                             std::to_string(lineNum);
            fix.confidence = 90;
            fixes.push_back(fix);
        }
        
        return fixes;
    }
    
    /**
     * إيجاد المتغيرات غير المستخدمة
     */
    std::vector<Fix> findUnusedVariables(const std::string& code) {
        std::vector<Fix> fixes;
        
        // تحليل مبسط
        auto lines = splitLines(code);
        
        for (size_t i = 0; i < lines.size(); ++i) {
            std::string line = trim(lines[i]);
            
            if (line.find("متغير ") == 0) {
                // استخراج اسم المتغير
                size_t start = 7; // بعد "متغير "
                size_t end = line.find_first_of(" :=", start);
                if (end == std::string::npos) end = line.length();
                
                std::string varName = line.substr(start, end - start);
                
                // البحث عن استخدام (مبسط)
                bool used = false;
                for (size_t j = i + 1; j < lines.size() && !used; ++j) {
                    if (lines[j].find(varName) != std::string::npos &&
                        lines[j].find("متغير " + varName) == std::string::npos) {
                        used = true;
                    }
                }
                
                if (!used) {
                    Fix fix;
                    fix.type = FixType::SUGGEST;
                    fix.startLine = static_cast<int>(i + 1);
                    fix.endLine = fix.startLine;
                    fix.oldCode = line;
                    fix.explanation = "المتغير '" + varName + "' غير مستخدم - فكر في حذفه";
                    fix.confidence = 60;
                    fixes.push_back(fix);
                }
            }
        }
        
        return fixes;
    }
    
    /**
     * إيجاد مشاكل الملكية
     */
    std::vector<Fix> findOwnershipIssues(const std::string& code) {
        std::vector<Fix> fixes;
        
        auto lines = splitLines(code);
        
        // تتبع المتغيرات المنقولة (مبسط)
        std::map<std::string, int> movedVars; // اسم → سطر النقل
        
        for (size_t i = 0; i < lines.size(); ++i) {
            std::string line = trim(lines[i]);
            
            // البحث عن تمرير لدالة (قد يكون نقلاً)
            size_t funcCall = line.find("(");
            if (funcCall != std::string::npos) {
                // تحليل مبسط
            }
        }
        
        return fixes;
    }
    
    /**
     * إيجاد أخطاء نحوية
     */
    std::vector<Fix> findSyntaxIssues(const std::string& code) {
        std::vector<Fix> fixes;
        
        auto lines = splitLines(code);
        
        for (size_t i = 0; i < lines.size(); ++i) {
            std::string line = trim(lines[i]);
            
            // التحقق من الأقواس
            int parenCount = 0;
            for (char c : line) {
                if (c == '(') parenCount++;
                else if (c == ')') parenCount--;
            }
            
            if (parenCount != 0) {
                Fix fix;
                fix.type = FixType::SUGGEST;
                fix.startLine = static_cast<int>(i + 1);
                fix.endLine = fix.startLine;
                fix.explanation = parenCount > 0 ? 
                    "قوس إغلاق مفقود" : "قوس فتح مفقود";
                fix.confidence = 85;
                fixes.push_back(fix);
            }
        }
        
        return fixes;
    }
    
    /**
     * إيجاد مشاكل الأداء
     */
    std::vector<Fix> findPerformanceIssues(const std::string& code) {
        std::vector<Fix> fixes;
        
        auto lines = splitLines(code);
        
        for (size_t i = 0; i < lines.size(); ++i) {
            std::string line = trim(lines[i]);
            
            // البحث عن استنساخ داخل حلقة
            if (line.find("لكل") != std::string::npos ||
                line.find("بينما") != std::string::npos) {
                // فحص الأسطر التالية
                for (size_t j = i + 1; j < lines.size(); ++j) {
                    std::string innerLine = trim(lines[j]);
                    if (innerLine == "نهاية") break;
                    
                    if (innerLine.find(".استنسخ()") != std::string::npos) {
                        Fix fix;
                        fix.type = FixType::SUGGEST;
                        fix.startLine = static_cast<int>(j + 1);
                        fix.explanation = "استنساخ داخل حلقة قد يؤثر على الأداء";
                        fix.confidence = 50;
                        fixes.push_back(fix);
                    }
                }
            }
        }
        
        return fixes;
    }
    
    // ═══════════════════════════════════════════════════════
    // دوال مساعدة
    // ═══════════════════════════════════════════════════════
    
    std::vector<std::string> splitLines(const std::string& str) {
        std::vector<std::string> lines;
        std::istringstream iss(str);
        std::string line;
        while (std::getline(iss, line)) {
            lines.push_back(line);
        }
        return lines;
    }
    
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    std::string extractIdentifier(const std::string& message) {
        // استخراج اسم من رسالة خطأ (مبسط)
        size_t pos = message.find("'");
        if (pos != std::string::npos) {
            size_t end = message.find("'", pos + 1);
            if (end != std::string::npos) {
                return message.substr(pos + 1, end - pos - 1);
            }
        }
        return "متغير";
    }
};

} // namespace ai
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء مصلح أخطاء
 */
void* sad_bug_fixer_new(void) {
    return new sad::ai::BugFixer();
}

/**
 * تحرير المصلح
 */
void sad_bug_fixer_free(void* fixer) {
    delete static_cast<sad::ai::BugFixer*>(fixer);
}

/**
 * تحليل الكود
 */
char* sad_bug_fixer_analyze(void* fixer, const char* code) {
    auto* f = static_cast<sad::ai::BugFixer*>(fixer);
    auto result = f->analyzeAndFix(code);
    
    std::string report = result.report();
    char* str = new char[report.size() + 1];
    std::strcpy(str, report.c_str());
    return str;
}

/**
 * إصلاح الكود
 */
char* sad_bug_fixer_fix(void* fixer, const char* code) {
    auto* f = static_cast<sad::ai::BugFixer*>(fixer);
    auto result = f->analyzeAndFix(code);
    
    std::string fixed = result.apply(code);
    char* str = new char[fixed.size() + 1];
    std::strcpy(str, fixed.c_str());
    return str;
}

/**
 * إصلاح من رسالة خطأ
 */
char* sad_bug_fixer_from_error(void* fixer, const char* error, 
                                const char* code, int line) {
    auto* f = static_cast<sad::ai::BugFixer*>(fixer);
    auto fix = f->fixFromError(error, code, line);
    
    std::string result = fix.toString();
    char* str = new char[result.size() + 1];
    std::strcpy(str, result.c_str());
    return str;
}

/**
 * تحرير نص
 */
void sad_bug_fixer_free_string(char* str) {
    delete[] str;
}

} // extern "C"
