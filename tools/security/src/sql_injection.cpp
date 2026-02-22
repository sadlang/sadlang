/**
 * ==========================================================
 * ملف: sql_injection.cpp
 * الوصف: كاشف SQL Injection
 * المهمة: T346 - أمان ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * SQL Injection هجوم يحقن أوامر SQL خبيثة في الاستعلامات.
 * هذا الكاشف يحلل الكود ويحذر من أنماط خطيرة.
 * 
 * 🔴 أنماط خطيرة:
 * - دمج النصوص في استعلامات
 * - استخدام + أو format() في SQL
 * 
 * 🟢 أنماط آمنة:
 * - استعلامات معاملة (parameterized)
 * - استخدام ORM
 */

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_set>

namespace sad {
namespace security {

// ==========================================================
// 📌 أنواع الثغرات
// ==========================================================

/**
 * نوع ثغرة SQL
 */
enum class نوع_SQL_Injection {
    دمج_نص,           // String concatenation
    تنسيق_نص,         // String formatting
    إدخال_مباشر,      // Direct input
    UNION_attack,
    تعليق,            // Comment injection
    مكدس,            // Stacked queries
    غير_معروف
};

/**
 * ثغرة SQL مكتشفة
 */
struct ثغرة_SQL {
    نوع_SQL_Injection نوع;
    std::string وصف;
    std::string ملف;
    int سطر;
    int عمود;
    std::string كود;
    std::string إصلاح_مقترح;
    double خطورة = 9.0;  // SQL Injection عادة حرج
    
    std::string النوع_كنص() const {
        switch (نوع) {
            case نوع_SQL_Injection::دمج_نص: return "دمج نص";
            case نوع_SQL_Injection::تنسيق_نص: return "تنسيق نص";
            case نوع_SQL_Injection::إدخال_مباشر: return "إدخال مباشر";
            case نوع_SQL_Injection::UNION_attack: return "UNION Attack";
            default: return "غير معروف";
        }
    }
};

// ==========================================================
// 📌 أنماط الكشف
// ==========================================================

/**
 * نمط كشف SQL Injection
 */
struct نمط_SQL {
    std::string اسم;
    std::regex regex;
    نوع_SQL_Injection نوع;
    std::string وصف;
    std::string إصلاح;
};

// ==========================================================
// 📌 كاشف SQL Injection
// ==========================================================

/**
 * كاشف SQL Injection
 */
class كاشف_SQL_Injection {
private:
    std::vector<نمط_SQL> أنماط_;
    std::unordered_set<std::string> دوال_SQL_;
    
public:
    كاشف_SQL_Injection() {
        تهيئة_الأنماط();
        تهيئة_دوال_SQL();
    }
    
    /**
     * فحص ملف
     */
    std::vector<ثغرة_SQL> افحص_ملف(const std::string& مسار,
                                    const std::string& محتوى) {
        std::vector<ثغرة_SQL> ثغرات;
        
        std::istringstream stream(محتوى);
        std::string سطر;
        int رقم_السطر = 0;
        
        while (std::getline(stream, سطر)) {
            رقم_السطر++;
            
            auto ثغرات_السطر = افحص_سطر(سطر, رقم_السطر);
            for (auto& ث : ثغرات_السطر) {
                ث.ملف = مسار;
                ثغرات.push_back(ث);
            }
        }
        
        return ثغرات;
    }
    
    /**
     * فحص سطر واحد
     */
    std::vector<ثغرة_SQL> افحص_سطر(const std::string& سطر, int رقم) {
        std::vector<ثغرة_SQL> ثغرات;
        
        // تجاهل التعليقات
        if (سطر.find("//") == 0 || سطر.find("#") == 0) {
            return ثغرات;
        }
        
        // فحص كل نمط
        for (const auto& نمط : أنماط_) {
            std::smatch match;
            if (std::regex_search(سطر, match, نمط.regex)) {
                ثغرة_SQL ث;
                ث.نوع = نمط.نوع;
                ث.وصف = نمط.وصف;
                ث.سطر = رقم;
                ث.كود = سطر;
                ث.إصلاح_مقترح = نمط.إصلاح;
                ثغرات.push_back(ث);
            }
        }
        
        // فحص إضافي للدوال
        for (const auto& دالة : دوال_SQL_) {
            if (سطر.find(دالة) != std::string::npos) {
                // التحقق من وجود دمج نص
                if (سطر.find("+") != std::string::npos ||
                    سطر.find("نسّق") != std::string::npos ||
                    سطر.find("format") != std::string::npos) {
                    ثغرة_SQL ث;
                    ث.نوع = نوع_SQL_Injection::دمج_نص;
                    ث.وصف = "دمج نص في استعلام SQL";
                    ث.سطر = رقم;
                    ث.كود = سطر;
                    ث.إصلاح_مقترح = "استخدم استعلامات معاملة بدلاً من دمج النصوص";
                    ثغرات.push_back(ث);
                }
            }
        }
        
        return ثغرات;
    }
    
    /**
     * فحص استعلام SQL مباشرة
     */
    std::vector<ثغرة_SQL> افحص_استعلام(const std::string& استعلام,
                                        const std::vector<std::string>& معاملات) {
        std::vector<ثغرة_SQL> ثغرات;
        
        // التحقق من أنماط خطيرة في الاستعلام
        for (const auto& معامل : معاملات) {
            // فحص محتوى المعامل
            if (يحتوي_حقن_محتمل(معامل)) {
                ثغرة_SQL ث;
                ث.نوع = نوع_SQL_Injection::إدخال_مباشر;
                ث.وصف = "المعامل يحتوي على أحرف SQL خطيرة";
                ث.كود = معامل;
                ث.إصلاح_مقترح = "نقِّ المعامل أو استخدم استعلامات معاملة";
                ثغرات.push_back(ث);
            }
        }
        
        return ثغرات;
    }
    
    /**
     * فحص إذا كان النص يحتوي حقن محتمل
     */
    bool يحتوي_حقن_محتمل(const std::string& نص) const {
        // أنماط شائعة للحقن
        std::vector<std::string> أنماط_خطيرة = {
            "'--",
            "' OR ",
            "' AND ",
            "1=1",
            "1' OR '1'='1",
            "; DROP ",
            "; DELETE ",
            "; UPDATE ",
            "UNION SELECT",
            "/*",
            "*/"
        };
        
        std::string نص_علوي = نص;
        std::transform(نص_علوي.begin(), نص_علوي.end(), 
                      نص_علوي.begin(), ::toupper);
        
        for (const auto& نمط : أنماط_خطيرة) {
            std::string نمط_علوي = نمط;
            std::transform(نمط_علوي.begin(), نمط_علوي.end(),
                          نمط_علوي.begin(), ::toupper);
            
            if (نص_علوي.find(نمط_علوي) != std::string::npos) {
                return true;
            }
        }
        
        return false;
    }
    
    /**
     * تنقية معامل SQL
     */
    std::string نقِّ(const std::string& قيمة) const {
        std::string نتيجة;
        نتيجة.reserve(قيمة.size() * 2);
        
        for (char c : قيمة) {
            switch (c) {
                case '\'': نتيجة += "''"; break;
                case '\\': نتيجة += "\\\\"; break;
                case '\0': break;  // تجاهل
                case '\n': نتيجة += "\\n"; break;
                case '\r': نتيجة += "\\r"; break;
                case '\t': نتيجة += "\\t"; break;
                default: نتيجة += c;
            }
        }
        
        return نتيجة;
    }
    
private:
    void تهيئة_الأنماط() {
        // نمط: دمج نص مع SELECT/INSERT/UPDATE/DELETE
        أنماط_.push_back({
            "دمج_SELECT",
            std::regex(R"(SELECT.*\+.*[a-z_]+)", std::regex::icase),
            نوع_SQL_Injection::دمج_نص,
            "دمج متغير في استعلام SELECT",
            "استخدم: استعلم(\"SELECT * FROM t WHERE id = ?\", [id])"
        });
        
        أنماط_.push_back({
            "دمج_INSERT",
            std::regex(R"(INSERT.*\+.*[a-z_]+)", std::regex::icase),
            نوع_SQL_Injection::دمج_نص,
            "دمج متغير في استعلام INSERT",
            "استخدم استعلامات معاملة"
        });
        
        // نمط: تنسيق نص
        أنماط_.push_back({
            "format_SQL",
            std::regex(R"((نسّق|format)\s*\(.*SQL)", std::regex::icase),
            نوع_SQL_Injection::تنسيق_نص,
            "استخدام format() في بناء استعلام SQL",
            "استخدم استعلامات معاملة بدلاً من format()"
        });
        
        // نمط: f-string في SQL
        أنماط_.push_back({
            "fstring_SQL",
            std::regex(R"(\$\{.*\}.*SELECT|INSERT|UPDATE|DELETE)", std::regex::icase),
            نوع_SQL_Injection::دمج_نص,
            "استخدام متغيرات في نص SQL",
            "استخدم استعلامات معاملة"
        });
    }
    
    void تهيئة_دوال_SQL() {
        دوال_SQL_.insert("نفّذ_SQL");
        دوال_SQL_.insert("استعلم");
        دوال_SQL_.insert("execute");
        دوال_SQL_.insert("query");
        دوال_SQL_.insert("raw_sql");
        دوال_SQL_.insert("exec_sql");
    }
};

// ==========================================================
// 📌 تقرير SQL Injection
// ==========================================================

/**
 * إنشاء تقرير SQL Injection
 */
class تقرير_SQL_Injection {
public:
    static std::string أنشئ(const std::vector<ثغرة_SQL>& ثغرات) {
        std::ostringstream ss;
        
        ss << "═══════════════════════════════════════════════════\n";
        ss << "🔍 تقرير فحص SQL Injection\n";
        ss << "═══════════════════════════════════════════════════\n\n";
        
        if (ثغرات.empty()) {
            ss << "✅ لم يتم العثور على ثغرات SQL Injection\n";
        } else {
            ss << "⚠️ تم العثور على " << ثغرات.size() << " ثغرة محتملة\n\n";
            
            int رقم = 1;
            for (const auto& ث : ثغرات) {
                ss << "───────────────────────────────────────────────────\n";
                ss << "🔴 ثغرة #" << رقم++ << ": " << ث.النوع_كنص() << "\n";
                ss << "───────────────────────────────────────────────────\n";
                ss << "📍 الموقع: " << ث.ملف << ":" << ث.سطر << "\n";
                ss << "📝 الوصف: " << ث.وصف << "\n";
                ss << "💻 الكود:\n   " << ث.كود << "\n";
                ss << "✏️ الإصلاح: " << ث.إصلاح_مقترح << "\n\n";
            }
            
            // نصائح
            ss << "═══════════════════════════════════════════════════\n";
            ss << "💡 نصائح للوقاية من SQL Injection:\n";
            ss << "═══════════════════════════════════════════════════\n";
            ss << "1. استخدم استعلامات معاملة (Parameterized Queries)\n";
            ss << "2. لا تدمج نصوص في استعلامات SQL\n";
            ss << "3. استخدم ORM عند الإمكان\n";
            ss << "4. تحقق من صحة الإدخال\n";
            ss << "5. استخدم حساب قاعدة بيانات بصلاحيات محدودة\n";
        }
        
        return ss.str();
    }
};

} // namespace security
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

using namespace sad::security;

typedef struct SadSQLScanner SadSQLScanner;

SadSQLScanner* sad_sql_scanner_create(void) {
    return reinterpret_cast<SadSQLScanner*>(new كاشف_SQL_Injection());
}

void sad_sql_scanner_destroy(SadSQLScanner* scanner) {
    delete reinterpret_cast<كاشف_SQL_Injection*>(scanner);
}

int sad_sql_scanner_scan_line(SadSQLScanner* scanner, 
                               const char* line, 
                               int line_number) {
    if (!scanner || !line) return 0;
    auto ثغرات = reinterpret_cast<كاشف_SQL_Injection*>(scanner)->افحص_سطر(line, line_number);
    return static_cast<int>(ثغرات.size());
}

int sad_sql_scanner_is_injection(SadSQLScanner* scanner, const char* input) {
    if (!scanner || !input) return 0;
    return reinterpret_cast<كاشف_SQL_Injection*>(scanner)->يحتوي_حقن_محتمل(input) ? 1 : 0;
}

const char* sad_sql_scanner_sanitize(SadSQLScanner* scanner, const char* value) {
    if (!scanner || !value) return nullptr;
    static std::string نتيجة;
    نتيجة = reinterpret_cast<كاشف_SQL_Injection*>(scanner)->نقِّ(value);
    return نتيجة.c_str();
}

} // extern "C"
