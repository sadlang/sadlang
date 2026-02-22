/**
 * ==========================================================
 * ملف: command_parser.cpp
 * الوصف: محلل أوامر البرمجة الصوتية
 * المهمة: T335 - صوت ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * هذا الملف يحول الكلام المعترف به إلى أوامر برمجية.
 * يفهم الأوامر بالعربية ويحولها لأكواد Sad.
 * 
 * 🗣️ أمثلة على الأوامر:
 * - "أنشئ دالة اسمها احسب" → دالة احسب()
 * - "أضف متغير اسمه العداد" → متغير العداد
 * - "أرجع القيمة صفر" → أرجع 0
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <regex>
#include <sstream>
#include <functional>

namespace sad {
namespace voice {

// ==========================================================
// 📌 أنواع الأوامر
// ==========================================================

/**
 * نوع الأمر الصوتي
 */
enum class نوع_الأمر {
    // إنشاء
    إنشاء_دالة,
    إنشاء_صنف,
    إنشاء_متغير,
    إنشاء_ثابت,
    إنشاء_هيكل,
    إنشاء_واجهة,
    
    // تحكم
    إضافة_شرط,
    إضافة_شرط_آخر,
    إضافة_حلقة,
    إضافة_حلقة_طالما,
    إغلاق_كتلة,
    
    // تعبيرات
    إسناد_قيمة,
    استدعاء_دالة,
    إرجاع_قيمة,
    
    // تنقل
    انتقال_سطر,
    انتقال_دالة,
    انتقال_بداية,
    انتقال_نهاية,
    
    // تحرير
    حذف_سطر,
    نسخ_سطر,
    لصق,
    تراجع,
    إعادة,
    
    // تنسيق
    سطر_جديد,
    مسافة,
    تبويب,
    
    // تعليق
    إضافة_تعليق,
    
    // استيراد
    استيراد_مكتبة,
    
    // غير معروف
    غير_معروف
};

// ==========================================================
// 📌 الأمر المُحلل
// ==========================================================

/**
 * أمر صوتي مُحلل
 */
struct أمر_صوتي {
    نوع_الأمر نوع = نوع_الأمر::غير_معروف;
    std::string اسم;
    std::vector<std::string> معاملات;
    std::string نوع_البيانات;
    std::string قيمة;
    float ثقة = 1.0f;
    std::string نص_أصلي;
    
    bool صالح() const { return نوع != نوع_الأمر::غير_معروف; }
};

// ==========================================================
// 📌 قواعد التحليل
// ==========================================================

/**
 * قاعدة تحليل واحدة
 */
struct قاعدة_تحليل {
    std::regex نمط;
    نوع_الأمر نوع;
    std::function<أمر_صوتي(const std::smatch&)> استخراج;
};

/**
 * بنّاء الأوامر
 * 
 * يبني أكواد Sad من الأوامر المُحللة
 */
class بنّاء_الأوامر {
private:
    int مستوى_المسافة_ = 0;
    
public:
    std::string ابنِ(const أمر_صوتي& أمر) {
        switch (أمر.نوع) {
            case نوع_الأمر::إنشاء_دالة:
                return بناء_دالة(أمر);
            case نوع_الأمر::إنشاء_صنف:
                return بناء_صنف(أمر);
            case نوع_الأمر::إنشاء_متغير:
                return بناء_متغير(أمر);
            case نوع_الأمر::إنشاء_ثابت:
                return بناء_ثابت(أمر);
            case نوع_الأمر::إضافة_شرط:
                return بناء_شرط(أمر);
            case نوع_الأمر::إضافة_شرط_آخر:
                return بناء_شرط_آخر(أمر);
            case نوع_الأمر::إضافة_حلقة:
                return بناء_حلقة(أمر);
            case نوع_الأمر::إغلاق_كتلة:
                return بناء_إغلاق();
            case نوع_الأمر::إرجاع_قيمة:
                return بناء_إرجاع(أمر);
            case نوع_الأمر::استدعاء_دالة:
                return بناء_استدعاء(أمر);
            case نوع_الأمر::إضافة_تعليق:
                return بناء_تعليق(أمر);
            case نوع_الأمر::سطر_جديد:
                return "\n";
            case نوع_الأمر::مسافة:
                return " ";
            case نوع_الأمر::تبويب:
                return "    ";
            default:
                return "";
        }
    }
    
    void زد_المسافة() { مستوى_المسافة_++; }
    void أنقص_المسافة() { if (مستوى_المسافة_ > 0) مستوى_المسافة_--; }
    
private:
    std::string مسافة_بادئة() const {
        return std::string(مستوى_المسافة_ * 4, ' ');
    }
    
    std::string بناء_دالة(const أمر_صوتي& أمر) {
        std::ostringstream ss;
        ss << مسافة_بادئة() << "دالة " << أمر.اسم << "(";
        
        // المعاملات
        for (size_t i = 0; i < أمر.معاملات.size(); ++i) {
            if (i > 0) ss << "، ";
            ss << أمر.معاملات[i];
        }
        ss << ")";
        
        // نوع الإرجاع
        if (!أمر.نوع_البيانات.empty()) {
            ss << " -> " << أمر.نوع_البيانات;
        }
        
        ss << "\n";
        زد_المسافة();
        return ss.str();
    }
    
    std::string بناء_صنف(const أمر_صوتي& أمر) {
        std::ostringstream ss;
        ss << مسافة_بادئة() << "صنف " << أمر.اسم << "\n";
        زد_المسافة();
        return ss.str();
    }
    
    std::string بناء_متغير(const أمر_صوتي& أمر) {
        std::ostringstream ss;
        ss << مسافة_بادئة() << "متغير " << أمر.اسم;
        
        if (!أمر.نوع_البيانات.empty()) {
            ss << ": " << أمر.نوع_البيانات;
        }
        
        if (!أمر.قيمة.empty()) {
            ss << " = " << أمر.قيمة;
        }
        
        ss << "\n";
        return ss.str();
    }
    
    std::string بناء_ثابت(const أمر_صوتي& أمر) {
        std::ostringstream ss;
        ss << مسافة_بادئة() << "ثابت " << أمر.اسم;
        
        if (!أمر.قيمة.empty()) {
            ss << " = " << أمر.قيمة;
        }
        
        ss << "\n";
        return ss.str();
    }
    
    std::string بناء_شرط(const أمر_صوتي& أمر) {
        std::ostringstream ss;
        ss << مسافة_بادئة() << "لو " << أمر.قيمة << "\n";
        زد_المسافة();
        return ss.str();
    }
    
    std::string بناء_شرط_آخر(const أمر_صوتي& أمر) {
        أنقص_المسافة();
        std::ostringstream ss;
        
        if (أمر.قيمة.empty()) {
            ss << مسافة_بادئة() << "وإلا\n";
        } else {
            ss << مسافة_بادئة() << "وإلا لو " << أمر.قيمة << "\n";
        }
        
        زد_المسافة();
        return ss.str();
    }
    
    std::string بناء_حلقة(const أمر_صوتي& أمر) {
        std::ostringstream ss;
        
        if (أمر.معاملات.size() >= 2) {
            // حلقة لكل
            ss << مسافة_بادئة() << "لكل " << أمر.اسم 
               << " في " << أمر.قيمة << "\n";
        } else {
            // حلقة طالما
            ss << مسافة_بادئة() << "طالما " << أمر.قيمة << "\n";
        }
        
        زد_المسافة();
        return ss.str();
    }
    
    std::string بناء_إغلاق() {
        أنقص_المسافة();
        return مسافة_بادئة() + "نهاية\n";
    }
    
    std::string بناء_إرجاع(const أمر_صوتي& أمر) {
        std::ostringstream ss;
        ss << مسافة_بادئة() << "أرجع";
        
        if (!أمر.قيمة.empty()) {
            ss << " " << أمر.قيمة;
        }
        
        ss << "\n";
        return ss.str();
    }
    
    std::string بناء_استدعاء(const أمر_صوتي& أمر) {
        std::ostringstream ss;
        ss << مسافة_بادئة() << أمر.اسم << "(";
        
        for (size_t i = 0; i < أمر.معاملات.size(); ++i) {
            if (i > 0) ss << "، ";
            ss << أمر.معاملات[i];
        }
        
        ss << ")\n";
        return ss.str();
    }
    
    std::string بناء_تعليق(const أمر_صوتي& أمر) {
        return مسافة_بادئة() + "// " + أمر.قيمة + "\n";
    }
};

// ==========================================================
// 📌 محلل الأوامر الصوتية
// ==========================================================

/**
 * محلل الأوامر الصوتية
 * 
 * يحول النص المعترف به إلى أوامر برمجية
 */
class محلل_الأوامر {
private:
    std::vector<قاعدة_تحليل> قواعد_;
    بنّاء_الأوامر بنّاء_;
    
    // قاموس الأرقام بالعربي
    std::unordered_map<std::string, std::string> أرقام_عربية_;
    
public:
    محلل_الأوامر() {
        تهيئة_القواعد();
        تهيئة_الأرقام();
    }
    
    /**
     * تحليل نص إلى أمر
     */
    أمر_صوتي حلّل(const std::string& نص) {
        std::string نص_معالج = معالجة_أولية(نص);
        
        for (const auto& قاعدة : قواعد_) {
            std::smatch مطابقات;
            if (std::regex_search(نص_معالج, مطابقات, قاعدة.نمط)) {
                أمر_صوتي أمر = قاعدة.استخراج(مطابقات);
                أمر.نوع = قاعدة.نوع;
                أمر.نص_أصلي = نص;
                return أمر;
            }
        }
        
        // أمر غير معروف
        أمر_صوتي أمر;
        أمر.نوع = نوع_الأمر::غير_معروف;
        أمر.نص_أصلي = نص;
        return أمر;
    }
    
    /**
     * بناء كود من أمر
     */
    std::string ابنِ_كود(const أمر_صوتي& أمر) {
        return بنّاء_.ابنِ(أمر);
    }
    
    /**
     * تحليل وبناء كود مباشرة
     */
    std::string حلّل_وابنِ(const std::string& نص) {
        أمر_صوتي أمر = حلّل(نص);
        return ابنِ_كود(أمر);
    }
    
private:
    void تهيئة_القواعد() {
        // قاعدة: أنشئ دالة اسمها X
        قواعد_.push_back({
            std::regex("(?:أنشئ|اعمل|اصنع)\\s+دالة\\s+(?:اسمها\\s+)?([\\p{Arabic}\\w]+)(?:\\s+(?:تأخذ|بمعاملات)\\s+(.+))?", std::regex::icase),
            نوع_الأمر::إنشاء_دالة,
            [](const std::smatch& م) {
                أمر_صوتي أمر;
                أمر.اسم = م[1].str();
                if (م[2].matched) {
                    // تحليل المعاملات
                    std::string معاملات_نص = م[2].str();
                    // تقسيم بـ "و" أو ","
                }
                return أمر;
            }
        });
        
        // قاعدة: أنشئ صنف اسمه X
        قواعد_.push_back({
            std::regex("(?:أنشئ|اعمل)\\s+صنف\\s+(?:اسمه\\s+)?([\\p{Arabic}\\w]+)", std::regex::icase),
            نوع_الأمر::إنشاء_صنف,
            [](const std::smatch& م) {
                أمر_صوتي أمر;
                أمر.اسم = م[1].str();
                return أمر;
            }
        });
        
        // قاعدة: أنشئ متغير اسمه X يساوي Y
        قواعد_.push_back({
            std::regex("(?:أنشئ|اعمل|عرّف)\\s+متغير\\s+(?:اسمه\\s+)?([\\p{Arabic}\\w]+)(?:\\s+(?:يساوي|قيمته)\\s+(.+))?", std::regex::icase),
            نوع_الأمر::إنشاء_متغير,
            [this](const std::smatch& م) {
                أمر_صوتي أمر;
                أمر.اسم = م[1].str();
                if (م[2].matched) {
                    أمر.قيمة = حوّل_قيمة(م[2].str());
                }
                return أمر;
            }
        });
        
        // قاعدة: أنشئ ثابت
        قواعد_.push_back({
            std::regex("(?:أنشئ|عرّف)\\s+ثابت\\s+(?:اسمه\\s+)?([\\p{Arabic}\\w]+)(?:\\s+(?:يساوي|قيمته)\\s+(.+))?", std::regex::icase),
            نوع_الأمر::إنشاء_ثابت,
            [this](const std::smatch& م) {
                أمر_صوتي أمر;
                أمر.اسم = م[1].str();
                if (م[2].matched) {
                    أمر.قيمة = حوّل_قيمة(م[2].str());
                }
                return أمر;
            }
        });
        
        // قاعدة: لو / إذا
        قواعد_.push_back({
            std::regex("(?:لو|إذا|اذا)\\s+(.+)", std::regex::icase),
            نوع_الأمر::إضافة_شرط,
            [this](const std::smatch& م) {
                أمر_صوتي أمر;
                أمر.قيمة = حوّل_شرط(م[1].str());
                return أمر;
            }
        });
        
        // قاعدة: وإلا
        قواعد_.push_back({
            std::regex("(?:وإلا|والا|غير ذلك)(?:\\s+(?:لو|إذا)\\s+(.+))?", std::regex::icase),
            نوع_الأمر::إضافة_شرط_آخر,
            [this](const std::smatch& م) {
                أمر_صوتي أمر;
                if (م[1].matched) {
                    أمر.قيمة = حوّل_شرط(م[1].str());
                }
                return أمر;
            }
        });
        
        // قاعدة: كرر / لكل
        قواعد_.push_back({
            std::regex("(?:كرر|لكل)\\s+([\\p{Arabic}\\w]+)\\s+(?:من|في)\\s+(.+)", std::regex::icase),
            نوع_الأمر::إضافة_حلقة,
            [](const std::smatch& م) {
                أمر_صوتي أمر;
                أمر.اسم = م[1].str();
                أمر.قيمة = م[2].str();
                أمر.معاملات = {أمر.اسم, أمر.قيمة};
                return أمر;
            }
        });
        
        // قاعدة: طالما
        قواعد_.push_back({
            std::regex("طالما\\s+(.+)", std::regex::icase),
            نوع_الأمر::إضافة_حلقة_طالما,
            [this](const std::smatch& م) {
                أمر_صوتي أمر;
                أمر.قيمة = حوّل_شرط(م[1].str());
                return أمر;
            }
        });
        
        // قاعدة: أغلق / نهاية
        قواعد_.push_back({
            std::regex("(?:أغلق|نهاية|انهِ|أنهِ)", std::regex::icase),
            نوع_الأمر::إغلاق_كتلة,
            [](const std::smatch&) {
                return أمر_صوتي{};
            }
        });
        
        // قاعدة: أرجع
        قواعد_.push_back({
            std::regex("(?:أرجع|ارجع|رجّع)(?:\\s+(.+))?", std::regex::icase),
            نوع_الأمر::إرجاع_قيمة,
            [this](const std::smatch& م) {
                أمر_صوتي أمر;
                if (م[1].matched) {
                    أمر.قيمة = حوّل_قيمة(م[1].str());
                }
                return أمر;
            }
        });
        
        // قاعدة: استدعاء دالة
        قواعد_.push_back({
            std::regex("(?:نادِ|استدعِ|شغّل)\\s+(?:دالة\\s+)?([\\p{Arabic}\\w]+)(?:\\s+(?:بـ|مع)\\s+(.+))?", std::regex::icase),
            نوع_الأمر::استدعاء_دالة,
            [](const std::smatch& م) {
                أمر_صوتي أمر;
                أمر.اسم = م[1].str();
                if (م[2].matched) {
                    // تحليل المعاملات
                    أمر.معاملات.push_back(م[2].str());
                }
                return أمر;
            }
        });
        
        // قاعدة: تعليق
        قواعد_.push_back({
            std::regex("(?:أضف\\s+)?تعليق\\s+(.+)", std::regex::icase),
            نوع_الأمر::إضافة_تعليق,
            [](const std::smatch& م) {
                أمر_صوتي أمر;
                أمر.قيمة = م[1].str();
                return أمر;
            }
        });
        
        // قاعدة: سطر جديد
        قواعد_.push_back({
            std::regex("سطر\\s*جديد", std::regex::icase),
            نوع_الأمر::سطر_جديد,
            [](const std::smatch&) { return أمر_صوتي{}; }
        });
        
        // قاعدة: مسافة
        قواعد_.push_back({
            std::regex("مسافة", std::regex::icase),
            نوع_الأمر::مسافة,
            [](const std::smatch&) { return أمر_صوتي{}; }
        });
    }
    
    void تهيئة_الأرقام() {
        أرقام_عربية_["صفر"] = "0";
        أرقام_عربية_["واحد"] = "1";
        أرقام_عربية_["اثنان"] = "2";
        أرقام_عربية_["اثنين"] = "2";
        أرقام_عربية_["ثلاثة"] = "3";
        أرقام_عربية_["أربعة"] = "4";
        أرقام_عربية_["خمسة"] = "5";
        أرقام_عربية_["ستة"] = "6";
        أرقام_عربية_["سبعة"] = "7";
        أرقام_عربية_["ثمانية"] = "8";
        أرقام_عربية_["تسعة"] = "9";
        أرقام_عربية_["عشرة"] = "10";
        أرقام_عربية_["مئة"] = "100";
        أرقام_عربية_["ألف"] = "1000";
    }
    
    std::string معالجة_أولية(const std::string& نص) {
        std::string نتيجة = نص;
        
        // إزالة المسافات الزائدة
        // تحويل الأرقام المنطوقة
        
        return نتيجة;
    }
    
    std::string حوّل_قيمة(const std::string& نص) {
        // تحويل الأرقام بالعربي
        auto it = أرقام_عربية_.find(نص);
        if (it != أرقام_عربية_.end()) {
            return it->second;
        }
        
        // تحويل "صحيح" و "خطأ"
        if (نص == "صحيح" || نص == "نعم") return "صحيح";
        if (نص == "خطأ" || نص == "لا") return "خطأ";
        if (نص == "عدم" || نص == "فارغ") return "عدم";
        
        return نص;
    }
    
    std::string حوّل_شرط(const std::string& نص) {
        std::string نتيجة = نص;
        
        // تحويل عوامل المقارنة
        std::unordered_map<std::string, std::string> عوامل = {
            {"يساوي", "=="},
            {"لا يساوي", "!="},
            {"أكبر من", ">"},
            {"أصغر من", "<"},
            {"أكبر أو يساوي", ">="},
            {"أصغر أو يساوي", "<="},
            {"و", " و "},
            {"أو", " أو "},
            {"ليس", "ليس "}
        };
        
        for (const auto& [من, إلى] : عوامل) {
            size_t pos = 0;
            while ((pos = نتيجة.find(من, pos)) != std::string::npos) {
                نتيجة.replace(pos, من.length(), إلى);
                pos += إلى.length();
            }
        }
        
        return نتيجة;
    }
};

// ==========================================================
// 📌 مفسر الأوامر التفاعلي
// ==========================================================

/**
 * مفسر الأوامر التفاعلي
 * 
 * يسمح بإدخال أوامر متتالية وبناء برنامج كامل
 */
class مفسر_تفاعلي {
private:
    محلل_الأوامر محلل_;
    std::vector<std::string> سجل_الأوامر_;
    std::string الكود_المبني_;
    
public:
    /**
     * معالجة أمر واحد
     */
    std::string عالج(const std::string& نص) {
        أمر_صوتي أمر = محلل_.حلّل(نص);
        
        if (!أمر.صالح()) {
            return "❓ لم أفهم الأمر: " + نص;
        }
        
        std::string كود = محلل_.ابنِ_كود(أمر);
        الكود_المبني_ += كود;
        سجل_الأوامر_.push_back(نص);
        
        return كود;
    }
    
    /**
     * الحصول على الكود الكامل المبني
     */
    const std::string& الكود() const { return الكود_المبني_; }
    
    /**
     * مسح الكود
     */
    void امسح() {
        الكود_المبني_.clear();
        سجل_الأوامر_.clear();
    }
    
    /**
     * تراجع عن آخر أمر
     */
    bool تراجع() {
        if (سجل_الأوامر_.empty()) return false;
        
        سجل_الأوامر_.pop_back();
        
        // إعادة بناء الكود
        الكود_المبني_.clear();
        محلل_الأوامر محلل_جديد;
        for (const auto& أمر : سجل_الأوامر_) {
            الكود_المبني_ += محلل_جديد.حلّل_وابنِ(أمر);
        }
        
        return true;
    }
    
    /**
     * سجل الأوامر
     */
    const std::vector<std::string>& السجل() const { return سجل_الأوامر_; }
};

} // namespace voice
} // namespace sad

// ==========================================================
// 📌 C API لمحلل الأوامر
// ==========================================================

extern "C" {

using namespace sad::voice;

typedef struct SadCommandParser SadCommandParser;

SadCommandParser* sad_command_parser_create(void) {
    return reinterpret_cast<SadCommandParser*>(new محلل_الأوامر());
}

void sad_command_parser_destroy(SadCommandParser* parser) {
    delete reinterpret_cast<محلل_الأوامر*>(parser);
}

const char* sad_command_parser_parse(SadCommandParser* parser, const char* text) {
    if (!parser || !text) return nullptr;
    
    static std::string result;
    result = reinterpret_cast<محلل_الأوامر*>(parser)->حلّل_وابنِ(text);
    return result.c_str();
}

// مفسر تفاعلي
typedef struct SadInteractiveParser SadInteractiveParser;

SadInteractiveParser* sad_interactive_parser_create(void) {
    return reinterpret_cast<SadInteractiveParser*>(new مفسر_تفاعلي());
}

void sad_interactive_parser_destroy(SadInteractiveParser* parser) {
    delete reinterpret_cast<مفسر_تفاعلي*>(parser);
}

const char* sad_interactive_parser_process(SadInteractiveParser* parser, const char* text) {
    if (!parser || !text) return nullptr;
    
    static std::string result;
    result = reinterpret_cast<مفسر_تفاعلي*>(parser)->عالج(text);
    return result.c_str();
}

const char* sad_interactive_parser_get_code(SadInteractiveParser* parser) {
    if (!parser) return nullptr;
    return reinterpret_cast<مفسر_تفاعلي*>(parser)->الكود().c_str();
}

void sad_interactive_parser_clear(SadInteractiveParser* parser) {
    if (parser) {
        reinterpret_cast<مفسر_تفاعلي*>(parser)->امسح();
    }
}

int sad_interactive_parser_undo(SadInteractiveParser* parser) {
    if (!parser) return 0;
    return reinterpret_cast<مفسر_تفاعلي*>(parser)->تراجع() ? 1 : 0;
}

} // extern "C"
