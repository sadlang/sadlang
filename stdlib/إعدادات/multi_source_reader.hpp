/**
 * =============================================================================
 * ملف: multi_source_reader.hpp
 * الوصف: قارئ الإعدادات متعدد المصادر في لغة ص
 * المهمة: T1075 - Phase 101
 * =============================================================================
 * 
 * 📚 دليل المبتدئ للإعدادات متعددة المصادر
 * ═════════════════════════════════════════
 * 
 * المشكلة:
 * ───────
 * 
 * الإعدادات تأتي من أماكن كثيرة:
 * - ملف config.toml (إعدادات افتراضية)
 * - ملف config.production.toml (إعدادات بيئة الإنتاج)
 * - متغيرات البيئة (PASSWORD=secret)
 * - سطر الأوامر (--port=8080)
 * 
 * أي واحد يُستخدم؟
 * 
 *     ┌──────────────────────────────────────────────────────────────┐
 *     │                                                              │
 *     │   ترتيب الأولوية (الأحدث يتجاوز الأقدم):                     │
 *     │   ────────────────────────────────────────                  │
 *     │                                                              │
 *     │   5. سطر الأوامر    --port=9000     ◄── الأعلى أولوية       │
 *     │      ↑                                                       │
 *     │   4. متغيرات البيئة  APP_PORT=8080                          │
 *     │      ↑                                                       │
 *     │   3. ملف البيئة     config.prod.toml                        │
 *     │      ↑                                                       │
 *     │   2. ملف عام        config.toml                             │
 *     │      ↑                                                       │
 *     │   1. افتراضي         (في الكود)    ◄── الأدنى أولوية        │
 *     │                                                              │
 *     │   النتيجة: port = 9000 (من سطر الأوامر)                      │
 *     │                                                              │
 *     └──────────────────────────────────────────────────────────────┘
 * 
 * =============================================================================
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <variant>
#include <functional>
#include <sstream>
#include <fstream>

namespace sad {
namespace إعدادات {

// ═══════════════════════════════════════════════════════════════════════════════
//                              أنواع القيم
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * قيمة إعداد (يمكن أن تكون أي نوع)
 */
using قيمة_إعداد = std::variant<
    std::nullptr_t,             // لا قيمة
    bool,                       // منطقي
    int64_t,                    // عدد صحيح
    double,                     // عدد عشري
    std::string,                // نص
    std::vector<std::string>,   // قائمة نصوص
    std::unordered_map<std::string, std::string>  // قاموس
>;

/**
 * نوع القيمة
 */
enum class نوع_قيمة {
    فارغ,
    منطقي,
    عدد_صحيح,
    عدد_عشري,
    نص,
    قائمة,
    قاموس
};

// ═══════════════════════════════════════════════════════════════════════════════
//                              مصدر الإعدادات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * نوع المصدر
 */
enum class نوع_مصدر {
    افتراضي,          // قيم مُعرَّفة في الكود
    ملف,              // ملف TOML/JSON/YAML
    بيئة,             // متغيرات البيئة
    سطر_أوامر,        // arguments
    ذاكرة             // مُعيَّن برمجياً
};

/**
 * مصدر إعدادات واحد
 */
class مصدر_إعدادات {
public:
    virtual ~مصدر_إعدادات() = default;
    
    /**
     * نوع المصدر
     */
    virtual نوع_مصدر النوع() const = 0;
    
    /**
     * اسم المصدر (للتصحيح)
     */
    virtual std::string الاسم() const = 0;
    
    /**
     * قراءة قيمة
     */
    virtual std::optional<قيمة_إعداد> اقرأ(const std::string& مفتاح) const = 0;
    
    /**
     * هل المفتاح موجود؟
     */
    virtual bool يوجد(const std::string& مفتاح) const = 0;
    
    /**
     * كل المفاتيح
     */
    virtual std::vector<std::string> المفاتيح() const = 0;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                              مصدر الذاكرة (القيم الافتراضية)
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * مصدر من الذاكرة (للقيم الافتراضية)
 */
class مصدر_ذاكرة : public مصدر_إعدادات {
public:
    مصدر_ذاكرة(std::string اسم = "افتراضي") : الاسم_(std::move(اسم)) {}
    
    نوع_مصدر النوع() const override { return نوع_مصدر::افتراضي; }
    std::string الاسم() const override { return الاسم_; }
    
    std::optional<قيمة_إعداد> اقرأ(const std::string& مفتاح) const override {
        auto it = القيم_.find(مفتاح);
        return it != القيم_.end() ? std::make_optional(it->second) : std::nullopt;
    }
    
    bool يوجد(const std::string& مفتاح) const override {
        return القيم_.find(مفتاح) != القيم_.end();
    }
    
    std::vector<std::string> المفاتيح() const override {
        std::vector<std::string> نتائج;
        for (const auto& [مفتاح, _] : القيم_) {
            نتائج.push_back(مفتاح);
        }
        return نتائج;
    }
    
    /**
     * تعيين قيمة
     */
    void عيِّن(const std::string& مفتاح, قيمة_إعداد قيمة) {
        القيم_[مفتاح] = std::move(قيمة);
    }
    
    /**
     * تعيين نص
     */
    void عيِّن_نص(const std::string& مفتاح, const std::string& قيمة) {
        القيم_[مفتاح] = قيمة;
    }
    
    /**
     * تعيين عدد
     */
    void عيِّن_عدد(const std::string& مفتاح, int64_t قيمة) {
        القيم_[مفتاح] = قيمة;
    }
    
    /**
     * تعيين منطقي
     */
    void عيِّن_منطقي(const std::string& مفتاح, bool قيمة) {
        القيم_[مفتاح] = قيمة;
    }

private:
    std::string الاسم_;
    std::unordered_map<std::string, قيمة_إعداد> القيم_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                              مصدر الملفات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * مصدر من ملف TOML
 */
class مصدر_ملف : public مصدر_إعدادات {
public:
    مصدر_ملف(const std::string& مسار) : المسار_(مسار) {
        حمِّل();
    }
    
    نوع_مصدر النوع() const override { return نوع_مصدر::ملف; }
    std::string الاسم() const override { return "ملف: " + المسار_; }
    
    std::optional<قيمة_إعداد> اقرأ(const std::string& مفتاح) const override {
        auto it = القيم_.find(مفتاح);
        return it != القيم_.end() ? std::make_optional(it->second) : std::nullopt;
    }
    
    bool يوجد(const std::string& مفتاح) const override {
        return القيم_.find(مفتاح) != القيم_.end();
    }
    
    std::vector<std::string> المفاتيح() const override {
        std::vector<std::string> نتائج;
        for (const auto& [مفتاح, _] : القيم_) {
            نتائج.push_back(مفتاح);
        }
        return نتائج;
    }
    
    /**
     * إعادة تحميل الملف
     */
    bool أعد_التحميل() {
        القيم_.clear();
        return حمِّل();
    }
    
    /**
     * هل الملف موجود؟
     */
    bool موجود() const { return موجود_; }

private:
    bool حمِّل() {
        std::ifstream ملف(المسار_);
        if (!ملف.is_open()) {
            موجود_ = false;
            return false;
        }
        
        موجود_ = true;
        std::string سطر;
        std::string قسم;
        
        while (std::getline(ملف, سطر)) {
            // تجاهل التعليقات والأسطر الفارغة
            if (سطر.empty() || سطر[0] == '#') continue;
            
            // اكتشاف القسم
            if (سطر[0] == '[') {
                size_t نهاية = سطر.find(']');
                if (نهاية != std::string::npos) {
                    قسم = سطر.substr(1, نهاية - 1);
                }
                continue;
            }
            
            // تحليل القيمة
            size_t يساوي = سطر.find('=');
            if (يساوي == std::string::npos) continue;
            
            std::string مفتاح = قص(سطر.substr(0, يساوي));
            std::string قيمة_نص = قص(سطر.substr(يساوي + 1));
            
            // بناء المفتاح الكامل
            std::string مفتاح_كامل = قسم.empty() ? مفتاح : قسم + "." + مفتاح;
            
            // تحليل القيمة
            قيمة_إعداد قيمة = حلل_قيمة(قيمة_نص);
            القيم_[مفتاح_كامل] = قيمة;
        }
        
        return true;
    }
    
    قيمة_إعداد حلل_قيمة(const std::string& نص) {
        // إزالة علامات الاقتباس
        if (نص.size() >= 2 && نص.front() == '"' && نص.back() == '"') {
            return نص.substr(1, نص.size() - 2);
        }
        
        // منطقي
        if (نص == "true" || نص == "صحيح") return true;
        if (نص == "false" || نص == "خاطئ") return false;
        
        // عدد صحيح
        try {
            return static_cast<int64_t>(std::stoll(نص));
        } catch (...) {}
        
        // عدد عشري
        try {
            return std::stod(نص);
        } catch (...) {}
        
        // نص
        return نص;
    }
    
    std::string قص(const std::string& نص) {
        size_t بداية = نص.find_first_not_of(" \t");
        size_t نهاية = نص.find_last_not_of(" \t");
        if (بداية == std::string::npos) return "";
        return نص.substr(بداية, نهاية - بداية + 1);
    }
    
    std::string المسار_;
    bool موجود_ = false;
    std::unordered_map<std::string, قيمة_إعداد> القيم_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                              مصدر البيئة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * مصدر من متغيرات البيئة
 */
class مصدر_بيئة : public مصدر_إعدادات {
public:
    مصدر_بيئة(std::string بادئة = "") : البادئة_(std::move(بادئة)) {}
    
    نوع_مصدر النوع() const override { return نوع_مصدر::بيئة; }
    std::string الاسم() const override { return "متغيرات البيئة"; }
    
    std::optional<قيمة_إعداد> اقرأ(const std::string& مفتاح) const override {
        std::string اسم_بيئة = حوِّل_للبيئة(مفتاح);
        
        #ifdef _WIN32
        char* قيمة = nullptr;
        size_t حجم = 0;
        if (_dupenv_s(&قيمة, &حجم, اسم_بيئة.c_str()) == 0 && قيمة) {
            std::string نتيجة(قيمة);
            free(قيمة);
            return نتيجة;
        }
        #else
        const char* قيمة = std::getenv(اسم_بيئة.c_str());
        if (قيمة) {
            return std::string(قيمة);
        }
        #endif
        
        return std::nullopt;
    }
    
    bool يوجد(const std::string& مفتاح) const override {
        return اقرأ(مفتاح).has_value();
    }
    
    std::vector<std::string> المفاتيح() const override {
        // لا يمكن سرد كل متغيرات البيئة بسهولة
        return {};
    }

private:
    std::string حوِّل_للبيئة(const std::string& مفتاح) const {
        // config.database.host -> APP_CONFIG_DATABASE_HOST
        std::string نتيجة = البادئة_.empty() ? "" : البادئة_ + "_";
        
        for (char c : مفتاح) {
            if (c == '.') {
                نتيجة += '_';
            } else {
                نتيجة += std::toupper(c);
            }
        }
        
        return نتيجة;
    }
    
    std::string البادئة_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                              مصدر سطر الأوامر
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * مصدر من سطر الأوامر
 */
class مصدر_سطر_أوامر : public مصدر_إعدادات {
public:
    مصدر_سطر_أوامر(int argc, char** argv) {
        حلل(argc, argv);
    }
    
    نوع_مصدر النوع() const override { return نوع_مصدر::سطر_أوامر; }
    std::string الاسم() const override { return "سطر الأوامر"; }
    
    std::optional<قيمة_إعداد> اقرأ(const std::string& مفتاح) const override {
        auto it = القيم_.find(مفتاح);
        return it != القيم_.end() ? std::make_optional(it->second) : std::nullopt;
    }
    
    bool يوجد(const std::string& مفتاح) const override {
        return القيم_.find(مفتاح) != القيم_.end();
    }
    
    std::vector<std::string> المفاتيح() const override {
        std::vector<std::string> نتائج;
        for (const auto& [مفتاح, _] : القيم_) {
            نتائج.push_back(مفتاح);
        }
        return نتائج;
    }

private:
    void حلل(int argc, char** argv) {
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            
            // --key=value أو --key value
            if (arg.substr(0, 2) == "--") {
                std::string rest = arg.substr(2);
                size_t eq = rest.find('=');
                
                if (eq != std::string::npos) {
                    std::string مفتاح = rest.substr(0, eq);
                    std::string قيمة = rest.substr(eq + 1);
                    القيم_[مفتاح] = قيمة;
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    القيم_[rest] = std::string(argv[++i]);
                } else {
                    القيم_[rest] = true;
                }
            }
            // -k value
            else if (arg[0] == '-' && arg.size() == 2) {
                std::string مفتاح(1, arg[1]);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    القيم_[مفتاح] = std::string(argv[++i]);
                } else {
                    القيم_[مفتاح] = true;
                }
            }
        }
    }
    
    std::unordered_map<std::string, قيمة_إعداد> القيم_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                              قارئ الإعدادات الموحد
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * قارئ الإعدادات متعدد المصادر
 * 
 * يجمع كل المصادر ويُعطي القيمة الصحيحة حسب الأولوية
 */
class قارئ_إعدادات {
public:
    /**
     * إضافة مصدر (الأحدث له أولوية أعلى)
     */
    void أضف_مصدر(std::shared_ptr<مصدر_إعدادات> مصدر) {
        المصادر_.push_back(مصدر);
    }
    
    /**
     * قراءة قيمة (يبحث من الأحدث للأقدم)
     */
    std::optional<قيمة_إعداد> اقرأ(const std::string& مفتاح) const {
        // البحث من الأحدث (نهاية القائمة) للأقدم
        for (auto it = المصادر_.rbegin(); it != المصادر_.rend(); ++it) {
            auto قيمة = (*it)->اقرأ(مفتاح);
            if (قيمة.has_value()) {
                return قيمة;
            }
        }
        return std::nullopt;
    }
    
    /**
     * قراءة نص
     */
    std::string نص(const std::string& مفتاح, const std::string& افتراضي = "") const {
        auto قيمة = اقرأ(مفتاح);
        if (!قيمة) return افتراضي;
        
        if (auto* ن = std::get_if<std::string>(&*قيمة)) {
            return *ن;
        }
        return افتراضي;
    }
    
    /**
     * قراءة عدد صحيح
     */
    int64_t عدد(const std::string& مفتاح, int64_t افتراضي = 0) const {
        auto قيمة = اقرأ(مفتاح);
        if (!قيمة) return افتراضي;
        
        if (auto* ع = std::get_if<int64_t>(&*قيمة)) {
            return *ع;
        }
        if (auto* ن = std::get_if<std::string>(&*قيمة)) {
            try { return std::stoll(*ن); } catch (...) {}
        }
        return افتراضي;
    }
    
    /**
     * قراءة منطقي
     */
    bool منطقي(const std::string& مفتاح, bool افتراضي = false) const {
        auto قيمة = اقرأ(مفتاح);
        if (!قيمة) return افتراضي;
        
        if (auto* م = std::get_if<bool>(&*قيمة)) {
            return *م;
        }
        if (auto* ن = std::get_if<std::string>(&*قيمة)) {
            return *ن == "true" || *ن == "صحيح" || *ن == "1";
        }
        return افتراضي;
    }
    
    /**
     * قراءة عشري
     */
    double عشري(const std::string& مفتاح, double افتراضي = 0.0) const {
        auto قيمة = اقرأ(مفتاح);
        if (!قيمة) return افتراضي;
        
        if (auto* ع = std::get_if<double>(&*قيمة)) {
            return *ع;
        }
        if (auto* ن = std::get_if<std::string>(&*قيمة)) {
            try { return std::stod(*ن); } catch (...) {}
        }
        return افتراضي;
    }
    
    /**
     * هل المفتاح موجود في أي مصدر؟
     */
    bool يوجد(const std::string& مفتاح) const {
        return اقرأ(مفتاح).has_value();
    }
    
    /**
     * معرفة من أين جاءت القيمة
     */
    std::string مصدر_القيمة(const std::string& مفتاح) const {
        for (auto it = المصادر_.rbegin(); it != المصادر_.rend(); ++it) {
            if ((*it)->يوجد(مفتاح)) {
                return (*it)->الاسم();
            }
        }
        return "غير موجود";
    }
    
    /**
     * كل المفاتيح من كل المصادر
     */
    std::vector<std::string> كل_المفاتيح() const {
        std::unordered_set<std::string> مجموعة;
        for (const auto& مصدر : المصادر_) {
            for (const auto& مفتاح : مصدر->المفاتيح()) {
                مجموعة.insert(مفتاح);
            }
        }
        return std::vector<std::string>(مجموعة.begin(), مجموعة.end());
    }

private:
    std::vector<std::shared_ptr<مصدر_إعدادات>> المصادر_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                              إنشاء قارئ كامل
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * إنشاء قارئ بالترتيب الافتراضي
 */
inline قارئ_إعدادات أنشئ_قارئ(
    int argc, 
    char** argv,
    const std::string& ملف_إعدادات = "config.toml",
    const std::string& بيئة = ""
) {
    قارئ_إعدادات قارئ;
    
    // 1. القيم الافتراضية (يمكن إضافتها لاحقاً)
    auto افتراضيات = std::make_shared<مصدر_ذاكرة>("افتراضي");
    قارئ.أضف_مصدر(افتراضيات);
    
    // 2. ملف الإعدادات العام
    auto ملف_عام = std::make_shared<مصدر_ملف>(ملف_إعدادات);
    if (ملف_عام->موجود()) {
        قارئ.أضف_مصدر(ملف_عام);
    }
    
    // 3. ملف البيئة (مثل config.production.toml)
    if (!بيئة.empty()) {
        std::string مسار_بيئة = ملف_إعدادات;
        size_t نقطة = مسار_بيئة.rfind('.');
        if (نقطة != std::string::npos) {
            مسار_بيئة = مسار_بيئة.substr(0, نقطة) + "." + بيئة + 
                        مسار_بيئة.substr(نقطة);
        }
        
        auto ملف_بيئة = std::make_shared<مصدر_ملف>(مسار_بيئة);
        if (ملف_بيئة->موجود()) {
            قارئ.أضف_مصدر(ملف_بيئة);
        }
    }
    
    // 4. متغيرات البيئة
    قارئ.أضف_مصدر(std::make_shared<مصدر_بيئة>("APP"));
    
    // 5. سطر الأوامر (الأعلى أولوية)
    قارئ.أضف_مصدر(std::make_shared<مصدر_سطر_أوامر>(argc, argv));
    
    return قارئ;
}

} // namespace إعدادات
} // namespace sad
