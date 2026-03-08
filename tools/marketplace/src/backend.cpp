/**
 * ===================================================================================
 *  خلفية السوق - Marketplace Backend (T404)
 * ===================================================================================
 * 
 *  الوصف: نظام خلفية لسوق الحزم مع دعم البيع والشراء
 *  Description: Backend system for package marketplace with buy/sell support
 *
 *  المميزات:
 *  - تسجيل الحزم المدفوعة والمجانية
 *  - نظام تصنيفات وفئات
 *  - إحصائيات التحميل والاستخدام
 *  - API للتكامل
 * 
 * ===================================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <functional>
#include <optional>
#include <algorithm>

namespace ص::سوق {

// ============== الأنواع ==============

/**
 * نوع الترخيص
 */
enum class نوع_ترخيص {
    مجاني,              // Free
    مجاني_مفتوح,        // Free & Open Source (MIT, Apache, etc.)
    مدفوع,              // Paid (one-time)
    اشتراك_شهري,        // Monthly subscription
    اشتراك_سنوي,        // Yearly subscription
    مؤسسي,              // Enterprise
    تجريبي              // Trial
};

/**
 * فئة الحزمة
 */
enum class فئة_حزمة {
    مكتبات_أساسية,      // Core libraries
    واجهات_مستخدم,      // UI frameworks
    قواعد_بيانات,       // Database
    شبكات,              // Networking
    أمان,               // Security
    ذكاء_اصطناعي,       // AI/ML
    ألعاب,              // Games
    إنترنت_الأشياء,     // IoT
    محمول,              // Mobile
    ويب,                // Web
    DevOps,             // DevOps
    أدوات,              // Tools
    قوالب,              // Templates
    أخرى                // Other
};

/**
 * حالة الحزمة
 */
enum class حالة_حزمة {
    مسودة,              // Draft
    قيد_المراجعة,       // Under review
    منشور,              // Published
    موقوف,              // Suspended
    محذوف               // Deleted
};

/**
 * معلومات السعر
 */
struct معلومات_سعر {
    double المبلغ = 0.0;
    std::string العملة = "USD";
    نوع_ترخيص الترخيص = نوع_ترخيص::مجاني;
    std::optional<int> أيام_تجريبية;
    std::optional<double> خصم_نسبة;
};

/**
 * إحصائيات الحزمة
 */
struct إحصائيات_حزمة {
    uint64_t تحميلات_كلية = 0;
    uint64_t تحميلات_أسبوعية = 0;
    uint64_t تحميلات_شهرية = 0;
    double تقييم_متوسط = 0.0;
    uint32_t عدد_التقييمات = 0;
    uint32_t عدد_النجوم = 0;
    uint32_t عدد_المساهمين = 0;
    std::chrono::system_clock::time_point آخر_تحديث;
};

/**
 * معلومات الناشر
 */
struct ناشر {
    std::string معرف;
    std::string اسم;
    std::string بريد;
    std::string موقع;
    std::string صورة;
    bool موثق = false;           // Verified badge
    bool مطور_نشط = false;       // Active developer badge
    std::chrono::system_clock::time_point تاريخ_التسجيل;
    std::vector<std::string> حزم;
    double إيرادات_كلية = 0.0;
};

/**
 * حزمة في السوق
 */
struct حزمة_سوق {
    // معلومات أساسية
    std::string معرف;
    std::string اسم;
    std::string وصف;
    std::string وصف_قصير;
    std::string إصدار;
    
    // التصنيف
    فئة_حزمة الفئة;
    std::vector<std::string> كلمات_مفتاحية;
    
    // الملكية
    std::string معرف_الناشر;
    معلومات_سعر السعر;
    
    // الحالة
    حالة_حزمة الحالة = حالة_حزمة::مسودة;
    إحصائيات_حزمة الإحصائيات;
    
    // الروابط
    std::string مستودع;
    std::string توثيق;
    std::string صفحة_رئيسية;
    std::string ملف_الحزمة;
    
    // البيانات الوصفية
    std::vector<std::string> متطلبات;
    std::string رخصة;
    std::string changelog;
    
    // التواريخ
    std::chrono::system_clock::time_point تاريخ_الإنشاء;
    std::chrono::system_clock::time_point تاريخ_التحديث;
    
    // الأصول
    std::vector<std::string> صور_شاشة;
    std::string أيقونة;
    std::string README;
};

/**
 * نتيجة البحث
 */
struct نتيجة_بحث_سوق {
    std::vector<حزمة_سوق> حزم;
    uint64_t إجمالي;
    int صفحة;
    int حجم_صفحة;
};

/**
 * فلتر البحث
 */
struct فلتر_بحث {
    std::string كلمة_بحث;
    std::optional<فئة_حزمة> فئة;
    std::optional<نوع_ترخيص> ترخيص;
    bool مجاني_فقط = false;
    bool موثق_فقط = false;
    std::string ترتيب = "تحميلات";  // تحميلات, تقييم, جديد, اسم
    bool تنازلي = true;
    int صفحة = 1;
    int حجم_صفحة = 20;
};

// ============== خدمة السوق ==============

/**
 * خدمة خلفية السوق
 */
class خدمة_سوق {
private:
    // تخزين محلي (في الإنتاج: قاعدة بيانات)
    std::map<std::string, حزمة_سوق> الحزم_;
    std::map<std::string, ناشر> الناشرون_;
    
    // إعدادات
    std::string قاعدة_URL_ = "https://market.sad-lang.org";
    std::string مفتاح_API_;
    
    // معالجات الأحداث
    std::function<void(const حزمة_سوق&)> عند_نشر_;
    std::function<void(const std::string&, const std::string&)> عند_شراء_;
    
public:
    خدمة_سوق() = default;
    
    // ============== إدارة الحزم ==============
    
    /**
     * تسجيل حزمة جديدة
     */
    std::string تسجيل_حزمة(حزمة_سوق& حزمة) {
        // توليد معرف فريد
        حزمة.معرف = توليد_معرف();
        حزمة.تاريخ_الإنشاء = std::chrono::system_clock::now();
        حزمة.تاريخ_التحديث = حزمة.تاريخ_الإنشاء;
        حزمة.الحالة = حالة_حزمة::مسودة;
        
        الحزم_[حزمة.معرف] = حزمة;
        
        // إضافة للناشر
        if (الناشرون_.count(حزمة.معرف_الناشر)) {
            الناشرون_[حزمة.معرف_الناشر].حزم.push_back(حزمة.معرف);
        }
        
        return حزمة.معرف;
    }
    
    /**
     * تحديث حزمة
     */
    bool تحديث_حزمة(const std::string& معرف, const حزمة_سوق& تحديثات) {
        auto it = الحزم_.find(معرف);
        if (it == الحزم_.end()) return false;
        
        auto& حزمة = it->second;
        
        // تحديث الحقول
        if (!تحديثات.اسم.empty()) حزمة.اسم = تحديثات.اسم;
        if (!تحديثات.وصف.empty()) حزمة.وصف = تحديثات.وصف;
        if (!تحديثات.إصدار.empty()) حزمة.إصدار = تحديثات.إصدار;
        
        حزمة.السعر = تحديثات.السعر;
        حزمة.تاريخ_التحديث = std::chrono::system_clock::now();
        
        return true;
    }
    
    /**
     * نشر حزمة
     */
    bool نشر_حزمة(const std::string& معرف) {
        auto it = الحزم_.find(معرف);
        if (it == الحزم_.end()) return false;
        
        auto& حزمة = it->second;
        
        // التحقق من الجاهزية
        if (!التحقق_من_الحزمة(حزمة)) {
            return false;
        }
        
        حزمة.الحالة = حالة_حزمة::منشور;
        حزمة.تاريخ_التحديث = std::chrono::system_clock::now();
        
        if (عند_نشر_) {
            عند_نشر_(حزمة);
        }
        
        return true;
    }
    
    /**
     * حذف حزمة
     */
    bool حذف_حزمة(const std::string& معرف) {
        auto it = الحزم_.find(معرف);
        if (it == الحزم_.end()) return false;
        
        it->second.الحالة = حالة_حزمة::محذوف;
        return true;
    }
    
    /**
     * الحصول على حزمة
     */
    std::optional<حزمة_سوق> الحصول_على_حزمة(const std::string& معرف) const {
        auto it = الحزم_.find(معرف);
        if (it != الحزم_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    // ============== البحث ==============
    
    /**
     * البحث في السوق
     */
    نتيجة_بحث_سوق بحث(const فلتر_بحث& فلتر) {
        نتيجة_بحث_سوق نتيجة;
        نتيجة.صفحة = فلتر.صفحة;
        نتيجة.حجم_صفحة = فلتر.حجم_صفحة;
        
        std::vector<حزمة_سوق> متطابقة;
        
        for (const auto& [_, حزمة] : الحزم_) {
            // فقط المنشورة
            if (حزمة.الحالة != حالة_حزمة::منشور) continue;
            
            // فلتر الكلمات المفتاحية
            if (!فلتر.كلمة_بحث.empty()) {
                bool يطابق = false;
                if (حزمة.اسم.find(فلتر.كلمة_بحث) != std::string::npos ||
                    حزمة.وصف.find(فلتر.كلمة_بحث) != std::string::npos) {
                    يطابق = true;
                }
                for (const auto& كلمة : حزمة.كلمات_مفتاحية) {
                    if (كلمة.find(فلتر.كلمة_بحث) != std::string::npos) {
                        يطابق = true;
                        break;
                    }
                }
                if (!يطابق) continue;
            }
            
            // فلتر الفئة
            if (فلتر.فئة && حزمة.الفئة != *فلتر.فئة) continue;
            
            // فلتر المجاني
            if (فلتر.مجاني_فقط && 
                حزمة.السعر.الترخيص != نوع_ترخيص::مجاني &&
                حزمة.السعر.الترخيص != نوع_ترخيص::مجاني_مفتوح) {
                continue;
            }
            
            // فلتر الموثق
            if (فلتر.موثق_فقط) {
                auto ناشر_it = الناشرون_.find(حزمة.معرف_الناشر);
                if (ناشر_it == الناشرون_.end() || !ناشر_it->second.موثق) {
                    continue;
                }
            }
            
            متطابقة.push_back(حزمة);
        }
        
        // الترتيب
        if (فلتر.ترتيب == "تحميلات") {
            std::sort(متطابقة.begin(), متطابقة.end(),
                [&](const حزمة_سوق& أ, const حزمة_سوق& ب) {
                    return فلتر.تنازلي ? 
                        أ.الإحصائيات.تحميلات_كلية > ب.الإحصائيات.تحميلات_كلية :
                        أ.الإحصائيات.تحميلات_كلية < ب.الإحصائيات.تحميلات_كلية;
                });
        } else if (فلتر.ترتيب == "تقييم") {
            std::sort(متطابقة.begin(), متطابقة.end(),
                [&](const حزمة_سوق& أ, const حزمة_سوق& ب) {
                    return فلتر.تنازلي ?
                        أ.الإحصائيات.تقييم_متوسط > ب.الإحصائيات.تقييم_متوسط :
                        أ.الإحصائيات.تقييم_متوسط < ب.الإحصائيات.تقييم_متوسط;
                });
        }
        
        // التقسيم للصفحات
        نتيجة.إجمالي = متطابقة.size();
        int بداية = (فلتر.صفحة - 1) * فلتر.حجم_صفحة;
        int نهاية = std::min(بداية + فلتر.حجم_صفحة, (int)متطابقة.size());
        
        if (بداية < متطابقة.size()) {
            نتيجة.حزم = std::vector<حزمة_سوق>(
                متطابقة.begin() + بداية,
                متطابقة.begin() + نهاية
            );
        }
        
        return نتيجة;
    }
    
    /**
     * الحزم الشائعة
     */
    std::vector<حزمة_سوق> الحزم_الشائعة(int حد = 10) {
        فلتر_بحث فلتر;
        فلتر.ترتيب = "تحميلات";
        فلتر.حجم_صفحة = حد;
        return بحث(فلتر).حزم;
    }
    
    /**
     * الحزم الجديدة
     */
    std::vector<حزمة_سوق> الحزم_الجديدة(int حد = 10) {
        فلتر_بحث فلتر;
        فلتر.ترتيب = "جديد";
        فلتر.حجم_صفحة = حد;
        return بحث(فلتر).حزم;
    }
    
    // ============== إدارة الناشرين ==============
    
    /**
     * تسجيل ناشر جديد
     */
    std::string تسجيل_ناشر(ناشر& ن) {
        ن.معرف = توليد_معرف();
        ن.تاريخ_التسجيل = std::chrono::system_clock::now();
        الناشرون_[ن.معرف] = ن;
        return ن.معرف;
    }
    
    /**
     * التحقق من الناشر
     */
    bool توثيق_ناشر(const std::string& معرف) {
        auto it = الناشرون_.find(معرف);
        if (it == الناشرون_.end()) return false;
        it->second.موثق = true;
        return true;
    }
    
    /**
     * الحصول على ناشر
     */
    std::optional<ناشر> الحصول_على_ناشر(const std::string& معرف) const {
        auto it = الناشرون_.find(معرف);
        if (it != الناشرون_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    // ============== الإحصائيات ==============
    
    /**
     * تسجيل تحميل
     */
    void تسجيل_تحميل(const std::string& معرف_حزمة) {
        auto it = الحزم_.find(معرف_حزمة);
        if (it != الحزم_.end()) {
            it->second.الإحصائيات.تحميلات_كلية++;
            it->second.الإحصائيات.تحميلات_أسبوعية++;
            it->second.الإحصائيات.تحميلات_شهرية++;
        }
    }
    
    /**
     * إحصائيات السوق
     */
    struct إحصائيات_سوق {
        uint64_t إجمالي_الحزم;
        uint64_t حزم_مجانية;
        uint64_t حزم_مدفوعة;
        uint64_t إجمالي_التحميلات;
        uint64_t عدد_الناشرين;
        uint64_t ناشرين_موثقين;
    };
    
    إحصائيات_سوق إحصائيات() const {
        إحصائيات_سوق stats;
        stats.إجمالي_الحزم = 0;
        stats.حزم_مجانية = 0;
        stats.حزم_مدفوعة = 0;
        stats.إجمالي_التحميلات = 0;
        
        for (const auto& [_, حزمة] : الحزم_) {
            if (حزمة.الحالة != حالة_حزمة::منشور) continue;
            
            stats.إجمالي_الحزم++;
            stats.إجمالي_التحميلات += حزمة.الإحصائيات.تحميلات_كلية;
            
            if (حزمة.السعر.الترخيص == نوع_ترخيص::مجاني ||
                حزمة.السعر.الترخيص == نوع_ترخيص::مجاني_مفتوح) {
                stats.حزم_مجانية++;
            } else {
                stats.حزم_مدفوعة++;
            }
        }
        
        stats.عدد_الناشرين = الناشرون_.size();
        stats.ناشرين_موثقين = 0;
        for (const auto& [_, ناشر] : الناشرون_) {
            if (ناشر.موثق) stats.ناشرين_موثقين++;
        }
        
        return stats;
    }
    
    // ============== معالجات الأحداث ==============
    
    void عند_نشر_حزمة(std::function<void(const حزمة_سوق&)> معالج) {
        عند_نشر_ = معالج;
    }
    
    void عند_شراء(std::function<void(const std::string&, const std::string&)> معالج) {
        عند_شراء_ = معالج;
    }
    
private:
    /**
     * توليد معرف فريد
     */
    std::string توليد_معرف() {
        static int عداد = 0;
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        return "pkg_" + std::to_string(ms) + "_" + std::to_string(++عداد);
    }
    
    /**
     * التحقق من صحة الحزمة
     */
    bool التحقق_من_الحزمة(const حزمة_سوق& حزمة) {
        if (حزمة.اسم.empty()) return false;
        if (حزمة.وصف.empty()) return false;
        if (حزمة.إصدار.empty()) return false;
        if (حزمة.معرف_الناشر.empty()) return false;
        return true;
    }
};

}  // namespace ص::سوق

// ============== C API ==============

extern "C" {

using namespace ص::سوق;

typedef struct خدمة_سوق_مقبض {
    std::unique_ptr<خدمة_سوق> خدمة;
} خدمة_سوق_مقبض;

خدمة_سوق_مقبض* sad_market_create() {
    auto مقبض = new خدمة_سوق_مقبض();
    مقبض->خدمة = std::make_unique<خدمة_سوق>();
    return مقبض;
}

void sad_market_destroy(خدمة_سوق_مقبض* مقبض) {
    delete مقبض;
}

const char* sad_market_search(خدمة_سوق_مقبض* مقبض, const char* query, int page) {
    static std::string result;
    if (مقبض && مقبض->خدمة) {
        فلتر_بحث فلتر;
        فلتر.كلمة_بحث = query ? query : "";
        فلتر.صفحة = page;
        
        auto نتائج = مقبض->خدمة->بحث(فلتر);
        result = "{\"total\": " + std::to_string(نتائج.إجمالي) + 
                 ", \"page\": " + std::to_string(نتائج.صفحة) + 
                 ", \"packages\": []}";
        return result.c_str();
    }
    return "{}";
}

}  // extern "C"
