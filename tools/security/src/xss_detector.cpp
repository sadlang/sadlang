/**
 * ==========================================================
 * ملف: xss_detector.cpp
 * الوصف: كاشف XSS (Cross-Site Scripting)
 * المهمة: T347 - أمان ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * XSS هجوم يحقن JavaScript خبيث في صفحات الويب.
 * يمكن أن يسرق ملفات تعريف الارتباط أو يعيد توجيه المستخدمين.
 * 
 * 🔴 أنواع XSS:
 * - Reflected: الإدخال يُرجع مباشرة
 * - Stored: الإدخال يُخزن ثم يُعرض
 * - DOM-based: التلاعب بـ DOM
 */

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <unordered_set>
#include <algorithm>
#include <sstream>

namespace sad {
namespace security {

// ==========================================================
// 📌 أنواع XSS
// ==========================================================

/**
 * نوع XSS
 */
enum class نوع_XSS {
    Reflected,
    Stored,
    DOM,
    غير_معروف
};

/**
 * سياق الإخراج
 */
enum class سياق_HTML {
    محتوى_نص,         // <div>HERE</div>
    صفة,              // <div attr="HERE">
    JavaScript,       // <script>HERE</script>
    URL,              // <a href="HERE">
    CSS,              // <style>HERE</style>
    تعليق             // <!-- HERE -->
};

/**
 * ثغرة XSS مكتشفة
 */
struct ثغرة_XSS {
    نوع_XSS نوع;
    سياق_HTML سياق;
    std::string وصف;
    std::string ملف;
    int سطر;
    std::string كود;
    std::string إصلاح_مقترح;
    double خطورة = 7.0;
    
    std::string النوع_كنص() const {
        switch (نوع) {
            case نوع_XSS::Reflected: return "Reflected XSS";
            case نوع_XSS::Stored: return "Stored XSS";
            case نوع_XSS::DOM: return "DOM XSS";
            default: return "XSS";
        }
    }
    
    std::string السياق_كنص() const {
        switch (سياق) {
            case سياق_HTML::محتوى_نص: return "محتوى نص";
            case سياق_HTML::صفة: return "صفة HTML";
            case سياق_HTML::JavaScript: return "كود JavaScript";
            case سياق_HTML::URL: return "رابط URL";
            default: return "غير معروف";
        }
    }
};

// ==========================================================
// 📌 كاشف XSS
// ==========================================================

/**
 * كاشف ثغرات XSS
 */
class كاشف_XSS {
private:
    std::vector<std::regex> أنماط_خطيرة_;
    std::unordered_set<std::string> مصارف_DOM_;
    std::unordered_set<std::string> دوال_عرض_;
    
public:
    كاشف_XSS() {
        تهيئة_الأنماط();
        تهيئة_مصارف_DOM();
        تهيئة_دوال_العرض();
    }
    
    /**
     * فحص ملف
     */
    std::vector<ثغرة_XSS> افحص_ملف(const std::string& مسار,
                                    const std::string& محتوى) {
        std::vector<ثغرة_XSS> ثغرات;
        
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
     * فحص سطر
     */
    std::vector<ثغرة_XSS> افحص_سطر(const std::string& سطر, int رقم) {
        std::vector<ثغرة_XSS> ثغرات;
        
        // فحص أنماط خطيرة
        for (const auto& نمط : أنماط_خطيرة_) {
            std::smatch match;
            if (std::regex_search(سطر, match, نمط)) {
                ثغرة_XSS ث;
                ث.نوع = نوع_XSS::غير_معروف;
                ث.وصف = "نمط HTML/JS خطير";
                ث.سطر = رقم;
                ث.كود = سطر;
                ثغرات.push_back(ث);
            }
        }
        
        // فحص مصارف DOM
        for (const auto& مصرف : مصارف_DOM_) {
            if (سطر.find(مصرف) != std::string::npos) {
                // التحقق من وجود متغير
                if (يحتوي_متغير(سطر)) {
                    ثغرة_XSS ث;
                    ث.نوع = نوع_XSS::DOM;
                    ث.سياق = سياق_HTML::JavaScript;
                    ث.وصف = "استخدام " + مصرف + " مع متغير غير منقى";
                    ث.سطر = رقم;
                    ث.كود = سطر;
                    ث.إصلاح_مقترح = "استخدم textContent بدلاً من innerHTML";
                    ثغرات.push_back(ث);
                }
            }
        }
        
        // فحص دوال العرض
        for (const auto& دالة : دوال_عرض_) {
            if (سطر.find(دالة) != std::string::npos) {
                if (يحتوي_متغير(سطر) && !يحتوي_تنقية(سطر)) {
                    ثغرة_XSS ث;
                    ث.نوع = نوع_XSS::Reflected;
                    ث.سياق = سياق_HTML::محتوى_نص;
                    ث.وصف = "عرض متغير بدون تنقية HTML";
                    ث.سطر = رقم;
                    ث.كود = سطر;
                    ث.إصلاح_مقترح = "استخدم هروب_HTML() قبل العرض";
                    ثغرات.push_back(ث);
                }
            }
        }
        
        return ثغرات;
    }
    
    /**
     * التحقق من نص إدخال للـ XSS
     */
    bool يحتوي_XSS_محتمل(const std::string& نص) const {
        // أنماط XSS شائعة
        std::vector<std::string> أنماط = {
            "<script",
            "javascript:",
            "onerror=",
            "onload=",
            "onclick=",
            "onmouseover=",
            "<img",
            "<iframe",
            "<svg",
            "expression(",
            "eval(",
            "document.cookie"
        };
        
        std::string نص_صغير = نص;
        std::transform(نص_صغير.begin(), نص_صغير.end(),
                      نص_صغير.begin(), ::tolower);
        
        for (const auto& نمط : أنماط) {
            if (نص_صغير.find(نمط) != std::string::npos) {
                return true;
            }
        }
        
        return false;
    }
    
    /**
     * تنقية HTML
     */
    std::string هروب_HTML(const std::string& نص) const {
        std::string نتيجة;
        نتيجة.reserve(نص.size() * 2);
        
        for (char c : نص) {
            switch (c) {
                case '<': نتيجة += "&lt;"; break;
                case '>': نتيجة += "&gt;"; break;
                case '&': نتيجة += "&amp;"; break;
                case '"': نتيجة += "&quot;"; break;
                case '\'': نتيجة += "&#x27;"; break;
                case '/': نتيجة += "&#x2F;"; break;
                default: نتيجة += c;
            }
        }
        
        return نتيجة;
    }
    
    /**
     * تنقية للصفات
     */
    std::string هروب_صفة(const std::string& نص) const {
        std::string نتيجة;
        نتيجة.reserve(نص.size() * 2);
        
        for (char c : نص) {
            // السماح فقط بأحرف أبجدية ورقمية
            if (std::isalnum(c) || c == ' ' || c == '_' || c == '-') {
                نتيجة += c;
            } else {
                // ترميز hex
                char buf[8];
                snprintf(buf, sizeof(buf), "&#x%02X;", static_cast<unsigned char>(c));
                نتيجة += buf;
            }
        }
        
        return نتيجة;
    }
    
    /**
     * تنقية JavaScript
     */
    std::string هروب_JS(const std::string& نص) const {
        std::string نتيجة;
        نتيجة.reserve(نص.size() * 2);
        
        for (char c : نص) {
            switch (c) {
                case '\'': نتيجة += "\\'"; break;
                case '"': نتيجة += "\\\""; break;
                case '\\': نتيجة += "\\\\"; break;
                case '\n': نتيجة += "\\n"; break;
                case '\r': نتيجة += "\\r"; break;
                case '<': نتيجة += "\\x3C"; break;
                case '>': نتيجة += "\\x3E"; break;
                default: نتيجة += c;
            }
        }
        
        return نتيجة;
    }
    
    /**
     * تنقية URL
     */
    std::string هروب_URL(const std::string& نص) const {
        std::string نتيجة;
        نتيجة.reserve(نص.size() * 3);
        
        for (unsigned char c : نص) {
            if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                نتيجة += c;
            } else {
                char buf[4];
                snprintf(buf, sizeof(buf), "%%%02X", c);
                نتيجة += buf;
            }
        }
        
        return نتيجة;
    }
    
private:
    void تهيئة_الأنماط() {
        // أنماط script
        أنماط_خطيرة_.push_back(std::regex(R"(<script[^>]*>)", std::regex::icase));
        
        // أنماط event handlers
        أنماط_خطيرة_.push_back(std::regex(R"(on\w+\s*=)", std::regex::icase));
        
        // javascript: في الروابط
        أنماط_خطيرة_.push_back(std::regex(R"(javascript\s*:)", std::regex::icase));
        
        // data: URIs
        أنماط_خطيرة_.push_back(std::regex(R"(data\s*:\s*text/html)", std::regex::icase));
    }
    
    void تهيئة_مصارف_DOM() {
        مصارف_DOM_.insert("innerHTML");
        مصارف_DOM_.insert("outerHTML");
        مصارف_DOM_.insert("document.write");
        مصارف_DOM_.insert("document.writeln");
        مصارف_DOM_.insert("eval");
        مصارف_DOM_.insert("setTimeout");
        مصارف_DOM_.insert("setInterval");
        مصارف_DOM_.insert("execScript");
    }
    
    void تهيئة_دوال_العرض() {
        دوال_عرض_.insert("اكتب_HTML");
        دوال_عرض_.insert("عرض_قالب");
        دوال_عرض_.insert("echo");
        دوال_عرض_.insert("print");
        دوال_عرض_.insert("render");
    }
    
    bool يحتوي_متغير(const std::string& سطر) const {
        // التحقق من وجود متغير عربي أو إنجليزي
        std::regex نمط_متغير(R"(\$\{[^}]+\}|[\u0621-\u064A_][a-zA-Z0-9_\u0621-\u064A]*)");
        return std::regex_search(سطر, نمط_متغير);
    }
    
    bool يحتوي_تنقية(const std::string& سطر) const {
        std::vector<std::string> دوال_تنقية = {
            "هروب_HTML", "هروب_صفة", "هروب_JS",
            "escape", "sanitize", "encode"
        };
        
        for (const auto& دالة : دوال_تنقية) {
            if (سطر.find(دالة) != std::string::npos) {
                return true;
            }
        }
        
        return false;
    }
};

// ==========================================================
// 📌 تقرير XSS
// ==========================================================

/**
 * إنشاء تقرير XSS
 */
class تقرير_XSS {
public:
    static std::string أنشئ(const std::vector<ثغرة_XSS>& ثغرات) {
        std::ostringstream ss;
        
        ss << "═══════════════════════════════════════════════════\n";
        ss << "🔍 تقرير فحص XSS\n";
        ss << "═══════════════════════════════════════════════════\n\n";
        
        if (ثغرات.empty()) {
            ss << "✅ لم يتم العثور على ثغرات XSS\n";
        } else {
            ss << "⚠️ تم العثور على " << ثغرات.size() << " ثغرة محتملة\n\n";
            
            int رقم = 1;
            for (const auto& ث : ثغرات) {
                ss << "───────────────────────────────────────────────────\n";
                ss << "🔴 ثغرة #" << رقم++ << ": " << ث.النوع_كنص() << "\n";
                ss << "───────────────────────────────────────────────────\n";
                ss << "📍 الموقع: " << ث.ملف << ":" << ث.سطر << "\n";
                ss << "📝 السياق: " << ث.السياق_كنص() << "\n";
                ss << "📝 الوصف: " << ث.وصف << "\n";
                ss << "💻 الكود:\n   " << ث.كود << "\n";
                if (!ث.إصلاح_مقترح.empty()) {
                    ss << "✏️ الإصلاح: " << ث.إصلاح_مقترح << "\n";
                }
                ss << "\n";
            }
            
            // نصائح
            ss << "═══════════════════════════════════════════════════\n";
            ss << "💡 نصائح للوقاية من XSS:\n";
            ss << "═══════════════════════════════════════════════════\n";
            ss << "1. استخدم هروب_HTML() للمحتوى النصي\n";
            ss << "2. استخدم هروب_صفة() للصفات\n";
            ss << "3. استخدم هروب_JS() لكود JavaScript\n";
            ss << "4. استخدم هروب_URL() للروابط\n";
            ss << "5. استخدم Content-Security-Policy\n";
            ss << "6. لا تستخدم innerHTML مع بيانات المستخدم\n";
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

typedef struct SadXSSScanner SadXSSScanner;

SadXSSScanner* sad_xss_scanner_create(void) {
    return reinterpret_cast<SadXSSScanner*>(new كاشف_XSS());
}

void sad_xss_scanner_destroy(SadXSSScanner* scanner) {
    delete reinterpret_cast<كاشف_XSS*>(scanner);
}

int sad_xss_scanner_is_xss(SadXSSScanner* scanner, const char* input) {
    if (!scanner || !input) return 0;
    return reinterpret_cast<كاشف_XSS*>(scanner)->يحتوي_XSS_محتمل(input) ? 1 : 0;
}

const char* sad_xss_escape_html(SadXSSScanner* scanner, const char* text) {
    if (!scanner || !text) return nullptr;
    static std::string نتيجة;
    نتيجة = reinterpret_cast<كاشف_XSS*>(scanner)->هروب_HTML(text);
    return نتيجة.c_str();
}

const char* sad_xss_escape_js(SadXSSScanner* scanner, const char* text) {
    if (!scanner || !text) return nullptr;
    static std::string نتيجة;
    نتيجة = reinterpret_cast<كاشف_XSS*>(scanner)->هروب_JS(text);
    return نتيجة.c_str();
}

const char* sad_xss_escape_url(SadXSSScanner* scanner, const char* text) {
    if (!scanner || !text) return nullptr;
    static std::string نتيجة;
    نتيجة = reinterpret_cast<كاشف_XSS*>(scanner)->هروب_URL(text);
    return نتيجة.c_str();
}

} // extern "C"
