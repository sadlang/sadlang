/**
 * ==========================================================
 * ملف: dictation.cpp
 * الوصف: إملاء الكود بالصوت
 * المهمة: T336 - صوت ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * هذا الملف يتيح إملاء الكود مباشرة بالصوت.
 * يحول الكلام إلى أكواد Sad مع تنسيق تلقائي.
 * 
 * 🎤 كيف يعمل الإملاء:
 * 1. تسجيل الصوت المستمر
 * 2. تحويل لنص في الوقت الفعلي
 * 3. تطبيق قواعد التنسيق
 * 4. إدراج في المحرر
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <queue>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <sstream>

namespace sad {
namespace voice {

// ==========================================================
// 📌 إعدادات الإملاء
// ==========================================================

/**
 * إعدادات الإملاء
 */
struct إعدادات_الإملاء {
    bool تنسيق_تلقائي = true;
    bool مسافات_بادئة_تلقائية = true;
    bool إكمال_تلقائي = true;
    bool علامات_ترقيم_صوتية = true;
    int مهلة_الصمت_ms = 500;
    std::string لغة = "ar";
};

/**
 * وضع الإملاء
 */
enum class وضع_الإملاء {
    كود,           // إملاء كود
    تعليق,         // إملاء تعليقات
    نص,            // إملاء نص عادي
    مختلط          // كود + تعليقات
};

// ==========================================================
// 📌 معالج النص
// ==========================================================

/**
 * معالج النص المُملى
 * 
 * يطبق قواعد التنسيق والتصحيح
 */
class معالج_النص {
private:
    إعدادات_الإملاء إعدادات_;
    
    // قاموس التصحيحات
    std::unordered_map<std::string, std::string> تصحيحات_;
    
    // قاموس علامات الترقيم
    std::unordered_map<std::string, std::string> ترقيم_;
    
public:
    معالج_النص() {
        تهيئة_التصحيحات();
        تهيئة_الترقيم();
    }
    
    /**
     * معالجة النص المُملى
     */
    std::string عالج(const std::string& نص, وضع_الإملاء وضع) {
        std::string نتيجة = نص;
        
        // تطبيق التصحيحات
        نتيجة = تطبيق_التصحيحات(نتيجة);
        
        // تحويل علامات الترقيم
        if (إعدادات_.علامات_ترقيم_صوتية) {
            نتيجة = تطبيق_الترقيم(نتيجة);
        }
        
        // معالجة حسب الوضع
        switch (وضع) {
            case وضع_الإملاء::كود:
                نتيجة = معالجة_كود(نتيجة);
                break;
            case وضع_الإملاء::تعليق:
                نتيجة = "// " + نتيجة;
                break;
            case وضع_الإملاء::نص:
                نتيجة = "\"" + نتيجة + "\"";
                break;
            default:
                break;
        }
        
        return نتيجة;
    }
    
    /**
     * تحديث الإعدادات
     */
    void حدّث_إعدادات(const إعدادات_الإملاء& إعدادات) {
        إعدادات_ = إعدادات;
    }
    
private:
    void تهيئة_التصحيحات() {
        // تصحيحات نطق شائعة
        تصحيحات_["دالا"] = "دالة";
        تصحيحات_["متقير"] = "متغير";
        تصحيحات_["صنيف"] = "صنف";
        تصحيحات_["ارجع"] = "أرجع";
        تصحيحات_["والا"] = "وإلا";
        تصحيحات_["نهايه"] = "نهاية";
        تصحيحات_["طالاما"] = "طالما";
        تصحيحات_["لاكل"] = "لكل";
    }
    
    void تهيئة_الترقيم() {
        // تحويل علامات الترقيم المنطوقة
        ترقيم_["نقطة"] = ".";
        ترقيم_["فاصلة"] = "،";
        ترقيم_["فاصلة منقوطة"] = "؛";
        ترقيم_["نقطتان"] = ":";
        ترقيم_["علامة استفهام"] = "؟";
        ترقيم_["علامة تعجب"] = "!";
        ترقيم_["فتح قوس"] = "(";
        ترقيم_["غلق قوس"] = ")";
        ترقيم_["فتح قوس مربع"] = "[";
        ترقيم_["غلق قوس مربع"] = "]";
        ترقيم_["فتح قوس معقوف"] = "{";
        ترقيم_["غلق قوس معقوف"] = "}";
        ترقيم_["علامة اقتباس"] = "\"";
        ترقيم_["شرطة"] = "-";
        ترقيم_["زائد"] = "+";
        ترقيم_["ضرب"] = "*";
        ترقيم_["قسمة"] = "/";
        ترقيم_["يساوي"] = "=";
    }
    
    std::string تطبيق_التصحيحات(const std::string& نص) {
        std::string نتيجة = نص;
        
        for (const auto& [خطأ, صحيح] : تصحيحات_) {
            size_t pos = 0;
            while ((pos = نتيجة.find(خطأ, pos)) != std::string::npos) {
                نتيجة.replace(pos, خطأ.length(), صحيح);
                pos += صحيح.length();
            }
        }
        
        return نتيجة;
    }
    
    std::string تطبيق_الترقيم(const std::string& نص) {
        std::string نتيجة = نص;
        
        for (const auto& [منطوق, رمز] : ترقيم_) {
            size_t pos = 0;
            while ((pos = نتيجة.find(منطوق, pos)) != std::string::npos) {
                نتيجة.replace(pos, منطوق.length(), رمز);
                pos += رمز.length();
            }
        }
        
        return نتيجة;
    }
    
    std::string معالجة_كود(const std::string& نص) {
        std::string نتيجة = نص;
        
        // تحويل الكلمات المفتاحية إلى أكواد
        std::unordered_map<std::string, std::string> كلمات_مفتاحية = {
            {"دالة", "دالة"},
            {"متغير", "متغير"},
            {"ثابت", "ثابت"},
            {"صنف", "صنف"},
            {"هيكل", "هيكل"},
            {"لو", "لو"},
            {"وإلا", "وإلا"},
            {"لكل", "لكل"},
            {"طالما", "طالما"},
            {"أرجع", "أرجع"},
            {"كسر", "كسر"},
            {"استمر", "استمر"},
            {"نهاية", "نهاية"}
        };
        
        return نتيجة;
    }
};

// ==========================================================
// 📌 مخزن الإملاء المؤقت
// ==========================================================

/**
 * جزء من الإملاء
 */
struct جزء_إملاء {
    std::string نص;
    double بداية;
    double نهاية;
    float ثقة;
    bool مؤكد;  // هل هو مؤكد أم مؤقت؟
};

/**
 * مخزن الإملاء
 * 
 * يدير الأجزاء المؤقتة والمؤكدة
 */
class مخزن_الإملاء {
private:
    std::vector<جزء_إملاء> أجزاء_مؤكدة_;
    جزء_إملاء جزء_مؤقت_;
    std::mutex قفل_;
    
public:
    /**
     * إضافة جزء مؤقت
     */
    void أضف_مؤقت(const جزء_إملاء& جزء) {
        std::lock_guard<std::mutex> حارس(قفل_);
        جزء_مؤقت_ = جزء;
        جزء_مؤقت_.مؤكد = false;
    }
    
    /**
     * تأكيد الجزء المؤقت
     */
    void أكّد() {
        std::lock_guard<std::mutex> حارس(قفل_);
        if (!جزء_مؤقت_.نص.empty()) {
            جزء_مؤقت_.مؤكد = true;
            أجزاء_مؤكدة_.push_back(جزء_مؤقت_);
            جزء_مؤقت_ = جزء_إملاء{};
        }
    }
    
    /**
     * إلغاء الجزء المؤقت
     */
    void ألغِ_المؤقت() {
        std::lock_guard<std::mutex> حارس(قفل_);
        جزء_مؤقت_ = جزء_إملاء{};
    }
    
    /**
     * الحصول على النص الكامل
     */
    std::string النص_الكامل() const {
        std::lock_guard<std::mutex> حارس(const_cast<std::mutex&>(قفل_));
        
        std::ostringstream ss;
        for (const auto& جزء : أجزاء_مؤكدة_) {
            ss << جزء.نص;
        }
        
        if (!جزء_مؤقت_.نص.empty()) {
            ss << جزء_مؤقت_.نص;
        }
        
        return ss.str();
    }
    
    /**
     * الحصول على النص المؤكد فقط
     */
    std::string النص_المؤكد() const {
        std::lock_guard<std::mutex> حارس(const_cast<std::mutex&>(قفل_));
        
        std::ostringstream ss;
        for (const auto& جزء : أجزاء_مؤكدة_) {
            ss << جزء.نص;
        }
        
        return ss.str();
    }
    
    /**
     * مسح الكل
     */
    void امسح() {
        std::lock_guard<std::mutex> حارس(قفل_);
        أجزاء_مؤكدة_.clear();
        جزء_مؤقت_ = جزء_إملاء{};
    }
    
    /**
     * تراجع عن آخر جزء مؤكد
     */
    bool تراجع() {
        std::lock_guard<std::mutex> حارس(قفل_);
        if (أجزاء_مؤكدة_.empty()) return false;
        أجزاء_مؤكدة_.pop_back();
        return true;
    }
};

// ==========================================================
// 📌 محرك الإملاء
// ==========================================================

/**
 * أحداث الإملاء
 */
using مستمع_إملاء = std::function<void(const std::string& نص, bool مؤقت)>;

/**
 * محرك الإملاء
 * 
 * يدير عملية الإملاء الصوتي
 */
class محرك_الإملاء {
private:
    إعدادات_الإملاء إعدادات_;
    معالج_النص معالج_;
    مخزن_الإملاء مخزن_;
    
    وضع_الإملاء وضع_ = وضع_الإملاء::كود;
    std::atomic<bool> نشط_{false};
    
    std::vector<مستمع_إملاء> مستمعون_;
    
    // مستوى المسافات البادئة الحالي
    int مستوى_المسافة_ = 0;
    
public:
    محرك_الإملاء() = default;
    
    explicit محرك_الإملاء(const إعدادات_الإملاء& إعدادات) 
        : إعدادات_(إعدادات) {
        معالج_.حدّث_إعدادات(إعدادات_);
    }
    
    /**
     * بدء الإملاء
     */
    bool ابدأ() {
        if (نشط_) return false;
        نشط_ = true;
        مخزن_.امسح();
        return true;
    }
    
    /**
     * إيقاف الإملاء
     */
    std::string أوقف() {
        نشط_ = false;
        مخزن_.أكّد();  // تأكيد أي جزء مؤقت
        return مخزن_.النص_المؤكد();
    }
    
    /**
     * معالجة نص مُعترف به
     */
    void عالج_نص(const std::string& نص, bool مؤقت) {
        if (!نشط_) return;
        
        // معالجة النص
        std::string نص_معالج = معالج_.عالج(نص, وضع_);
        
        // إضافة مسافات بادئة إذا لزم
        if (إعدادات_.مسافات_بادئة_تلقائية) {
            نص_معالج = إضافة_مسافات_بادئة(نص_معالج);
        }
        
        // تحديث المخزن
        جزء_إملاء جزء;
        جزء.نص = نص_معالج;
        جزء.ثقة = 1.0f;
        
        if (مؤقت) {
            مخزن_.أضف_مؤقت(جزء);
        } else {
            مخزن_.أضف_مؤقت(جزء);
            مخزن_.أكّد();
            
            // تحديث مستوى المسافات
            تحديث_مستوى_المسافات(نص_معالج);
        }
        
        // إشعار المستمعين
        for (const auto& مستمع : مستمعون_) {
            مستمع(نص_معالج, مؤقت);
        }
    }
    
    /**
     * تغيير وضع الإملاء
     */
    void عيّن_الوضع(وضع_الإملاء وضع) {
        وضع_ = وضع;
    }
    
    /**
     * الحصول على الوضع الحالي
     */
    وضع_الإملاء الوضع() const { return وضع_; }
    
    /**
     * هل نشط؟
     */
    bool نشط() const { return نشط_; }
    
    /**
     * الحصول على النص الحالي
     */
    std::string النص() const {
        return مخزن_.النص_الكامل();
    }
    
    /**
     * إضافة مستمع
     */
    void أضف_مستمع(مستمع_إملاء مستمع) {
        مستمعون_.push_back(std::move(مستمع));
    }
    
    /**
     * تراجع
     */
    bool تراجع() {
        return مخزن_.تراجع();
    }
    
    /**
     * مسح
     */
    void امسح() {
        مخزن_.امسح();
        مستوى_المسافة_ = 0;
    }
    
private:
    std::string إضافة_مسافات_بادئة(const std::string& نص) {
        if (مستوى_المسافة_ <= 0) return نص;
        
        std::string مسافات(مستوى_المسافة_ * 4, ' ');
        return مسافات + نص;
    }
    
    void تحديث_مستوى_المسافات(const std::string& نص) {
        // زيادة المستوى بعد كلمات مفتاحية معينة
        if (نص.find("دالة") != std::string::npos ||
            نص.find("صنف") != std::string::npos ||
            نص.find("لو") != std::string::npos ||
            نص.find("لكل") != std::string::npos ||
            نص.find("طالما") != std::string::npos) {
            مستوى_المسافة_++;
        }
        
        // إنقاص المستوى عند "نهاية"
        if (نص.find("نهاية") != std::string::npos) {
            if (مستوى_المسافة_ > 0) مستوى_المسافة_--;
        }
    }
};

// ==========================================================
// 📌 مساعد الإملاء
// ==========================================================

/**
 * مساعد الإملاء
 * 
 * يقدم اقتراحات وتصحيحات أثناء الإملاء
 */
class مساعد_الإملاء {
private:
    std::vector<std::string> كلمات_مفتاحية_;
    std::vector<std::string> دوال_شائعة_;
    
public:
    مساعد_الإملاء() {
        تهيئة_الكلمات_المفتاحية();
        تهيئة_الدوال_الشائعة();
    }
    
    /**
     * اقتراحات الإكمال
     */
    std::vector<std::string> اقتراحات(const std::string& جزء) {
        std::vector<std::string> نتائج;
        
        // بحث في الكلمات المفتاحية
        for (const auto& كلمة : كلمات_مفتاحية_) {
            if (كلمة.find(جزء) == 0) {
                نتائج.push_back(كلمة);
            }
        }
        
        // بحث في الدوال الشائعة
        for (const auto& دالة : دوال_شائعة_) {
            if (دالة.find(جزء) == 0) {
                نتائج.push_back(دالة);
            }
        }
        
        return نتائج;
    }
    
    /**
     * التحقق من صحة البنية
     */
    bool تحقق_من_البنية(const std::string& كود) {
        int أقواس = 0;
        int كتل = 0;
        
        // حساب الأقواس
        for (char c : كود) {
            if (c == '(') أقواس++;
            else if (c == ')') أقواس--;
        }
        
        // حساب الكتل (دالة/صنف/لو/لكل vs نهاية)
        // مُبسّط
        
        return أقواس == 0;
    }
    
private:
    void تهيئة_الكلمات_المفتاحية() {
        كلمات_مفتاحية_ = {
            "دالة", "متغير", "ثابت", "صنف", "هيكل", "واجهة",
            "لو", "وإلا", "لكل", "طالما", "أرجع", "كسر", "استمر",
            "نهاية", "استورد", "من", "في", "صحيح", "خطأ", "عدم",
            "عدد", "نص", "منطقي", "قائمة", "قاموس"
        };
    }
    
    void تهيئة_الدوال_الشائعة() {
        دوال_شائعة_ = {
            "اطبع", "اقرأ", "طول", "نوع", "مدى",
            "أضف", "احذف", "ابحث", "رتّب", "اعكس"
        };
    }
};

// ==========================================================
// 📌 جلسة الإملاء
// ==========================================================

/**
 * جلسة إملاء كاملة
 * 
 * تدير جلسة إملاء من البداية للنهاية
 */
class جلسة_الإملاء {
private:
    محرك_الإملاء محرك_;
    مساعد_الإملاء مساعد_;
    
    std::string اسم_الملف_;
    std::vector<std::string> سجل_;
    
public:
    جلسة_الإملاء(const std::string& ملف = "") : اسم_الملف_(ملف) {}
    
    /**
     * بدء الجلسة
     */
    bool ابدأ() {
        سجل_.clear();
        return محرك_.ابدأ();
    }
    
    /**
     * إنهاء الجلسة
     */
    std::string أنهِ() {
        return محرك_.أوقف();
    }
    
    /**
     * إضافة نص من STT
     */
    void أضف_نص(const std::string& نص, bool مؤقت = false) {
        محرك_.عالج_نص(نص, مؤقت);
        if (!مؤقت) {
            سجل_.push_back(نص);
        }
    }
    
    /**
     * الحصول على الكود الحالي
     */
    std::string الكود() const {
        return محرك_.النص();
    }
    
    /**
     * الحصول على اقتراحات
     */
    std::vector<std::string> اقتراحات(const std::string& جزء) {
        return مساعد_.اقتراحات(جزء);
    }
    
    /**
     * تغيير الوضع
     */
    void عيّن_الوضع(وضع_الإملاء وضع) {
        محرك_.عيّن_الوضع(وضع);
    }
    
    /**
     * تراجع
     */
    bool تراجع() {
        if (محرك_.تراجع()) {
            if (!سجل_.empty()) سجل_.pop_back();
            return true;
        }
        return false;
    }
};

} // namespace voice
} // namespace sad

// ==========================================================
// 📌 C API للإملاء
// ==========================================================

extern "C" {

using namespace sad::voice;

typedef struct SadDictation SadDictation;

SadDictation* sad_dictation_create(void) {
    return reinterpret_cast<SadDictation*>(new محرك_الإملاء());
}

void sad_dictation_destroy(SadDictation* dictation) {
    delete reinterpret_cast<محرك_الإملاء*>(dictation);
}

int sad_dictation_start(SadDictation* dictation) {
    if (!dictation) return 0;
    return reinterpret_cast<محرك_الإملاء*>(dictation)->ابدأ() ? 1 : 0;
}

const char* sad_dictation_stop(SadDictation* dictation) {
    if (!dictation) return nullptr;
    
    static std::string result;
    result = reinterpret_cast<محرك_الإملاء*>(dictation)->أوقف();
    return result.c_str();
}

void sad_dictation_process(SadDictation* dictation, const char* text, int is_interim) {
    if (dictation && text) {
        reinterpret_cast<محرك_الإملاء*>(dictation)->عالج_نص(text, is_interim != 0);
    }
}

void sad_dictation_set_mode(SadDictation* dictation, int mode) {
    if (dictation) {
        reinterpret_cast<محرك_الإملاء*>(dictation)->عيّن_الوضع(
            static_cast<وضع_الإملاء>(mode)
        );
    }
}

const char* sad_dictation_get_text(SadDictation* dictation) {
    if (!dictation) return nullptr;
    
    static std::string result;
    result = reinterpret_cast<محرك_الإملاء*>(dictation)->النص();
    return result.c_str();
}

int sad_dictation_undo(SadDictation* dictation) {
    if (!dictation) return 0;
    return reinterpret_cast<محرك_الإملاء*>(dictation)->تراجع() ? 1 : 0;
}

void sad_dictation_clear(SadDictation* dictation) {
    if (dictation) {
        reinterpret_cast<محرك_الإملاء*>(dictation)->امسح();
    }
}

// جلسة الإملاء
typedef struct SadDictationSession SadDictationSession;

SadDictationSession* sad_dictation_session_create(const char* filename) {
    return reinterpret_cast<SadDictationSession*>(
        new جلسة_الإملاء(filename ? filename : "")
    );
}

void sad_dictation_session_destroy(SadDictationSession* session) {
    delete reinterpret_cast<جلسة_الإملاء*>(session);
}

int sad_dictation_session_start(SadDictationSession* session) {
    if (!session) return 0;
    return reinterpret_cast<جلسة_الإملاء*>(session)->ابدأ() ? 1 : 0;
}

const char* sad_dictation_session_end(SadDictationSession* session) {
    if (!session) return nullptr;
    
    static std::string result;
    result = reinterpret_cast<جلسة_الإملاء*>(session)->أنهِ();
    return result.c_str();
}

} // extern "C"
