/**
 * ===================================================================================
 *  نظام التراخيص - Package Licensing System (T406)
 * ===================================================================================
 * 
 *  الوصف: نظام تراخيص للحزم يدعم أنواع مختلفة من التراخيص
 *  Description: Package licensing system supporting various license types
 *
 *  المميزات:
 *  - تراخيص فردية وجماعية ومؤسسية
 *  - مفاتيح ترخيص مشفرة
 *  - التحقق من الترخيص
 *  - إدارة الاشتراكات
 * 
 * ===================================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <optional>
#include <random>
#include <sstream>
#include <iomanip>

namespace ص::سوق {

// ============== الأنواع ==============

/**
 * نوع الترخيص
 */
enum class نوع_رخصة {
    شخصي,              // Personal (1 device)
    احترافي,           // Professional (5 devices)
    فريق,              // Team (unlimited, 1 org)
    مؤسسي,             // Enterprise (unlimited, multi-org)
    تعليمي,            // Educational
    مجاني,             // Free/Open Source
    تجريبي             // Trial
};

/**
 * حالة الترخيص
 */
enum class حالة_رخصة {
    نشطة,
    منتهية,
    ملغية,
    معلقة
};

/**
 * قيود الترخيص
 */
struct قيود_رخصة {
    int أقصى_أجهزة = 1;
    int أقصى_مستخدمين = 1;
    bool استخدام_تجاري = false;
    bool توزيع = false;
    bool تعديل = false;
    bool دعم = false;
    bool تحديثات = true;
    std::vector<std::string> ميزات_إضافية;
};

/**
 * معلومات الترخيص
 */
struct رخصة {
    std::string معرف;
    std::string مفتاح;
    
    // الحزمة والمالك
    std::string معرف_حزمة;
    std::string معرف_مالك;
    std::string اسم_مالك;
    std::string بريد_مالك;
    
    // النوع والحالة
    نوع_رخصة النوع;
    حالة_رخصة الحالة = حالة_رخصة::نشطة;
    
    // القيود
    قيود_رخصة القيود;
    
    // التواريخ
    std::chrono::system_clock::time_point تاريخ_إصدار;
    std::chrono::system_clock::time_point تاريخ_انتهاء;
    
    // الاستخدام
    std::vector<std::string> أجهزة_مسجلة;
    int عدد_التفعيلات = 0;
};

/**
 * طلب تفعيل
 */
struct طلب_تفعيل {
    std::string مفتاح_رخصة;
    std::string معرف_جهاز;
    std::string اسم_جهاز;
    std::string نظام_تشغيل;
    std::string إصدار_حزمة;
};

/**
 * نتيجة تفعيل
 */
struct نتيجة_تفعيل {
    bool نجح;
    std::string رسالة;
    std::optional<رخصة> الرخصة;
    std::chrono::system_clock::time_point انتهاء_صلاحية;
};

// ============== مولد المفاتيح ==============

/**
 * مولد مفاتيح الترخيص
 */
class مولد_مفاتيح {
private:
    std::string مفتاح_تشفير_;
    
public:
    مولد_مفاتيح(const std::string& مفتاح = "sad-license-key-2024")
        : مفتاح_تشفير_(مفتاح) {}
    
    /**
     * توليد مفتاح ترخيص
     */
    std::string توليد_مفتاح(const std::string& معرف_حزمة, 
                            نوع_رخصة نوع) {
        // Format: XXXX-XXXX-XXXX-XXXX
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 35);
        
        const char chars[] = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";  // بدون I,O,0,1
        
        std::string مفتاح;
        for (int g = 0; g < 4; ++g) {
            if (g > 0) مفتاح += "-";
            for (int i = 0; i < 4; ++i) {
                مفتاح += chars[dis(gen)];
            }
        }
        
        // إضافة بادئة حسب النوع
        std::string بادئة;
        switch (نوع) {
            case نوع_رخصة::شخصي: بادئة = "P"; break;
            case نوع_رخصة::احترافي: بادئة = "R"; break;
            case نوع_رخصة::فريق: بادئة = "T"; break;
            case نوع_رخصة::مؤسسي: بادئة = "E"; break;
            case نوع_رخصة::تعليمي: بادئة = "D"; break;
            case نوع_رخصة::تجريبي: بادئة = "X"; break;
            default: بادئة = "F"; break;
        }
        
        return بادئة + "-" + مفتاح;
    }
    
    /**
     * التحقق من صيغة المفتاح
     */
    bool التحقق_من_الصيغة(const std::string& مفتاح) {
        // P-XXXX-XXXX-XXXX-XXXX
        if (مفتاح.length() != 21) return false;
        if (مفتاح[1] != '-') return false;
        
        // التحقق من الأجزاء
        for (int i = 0; i < 4; ++i) {
            int start = 2 + i * 5;
            if (i > 0 && مفتاح[start - 1] != '-') return false;
            for (int j = 0; j < 4; ++j) {
                char c = مفتاح[start + j];
                if (!std::isalnum(c)) return false;
            }
        }
        
        return true;
    }
    
    /**
     * استخراج نوع الترخيص من المفتاح
     */
    نوع_رخصة استخراج_النوع(const std::string& مفتاح) {
        if (مفتاح.empty()) return نوع_رخصة::مجاني;
        
        switch (مفتاح[0]) {
            case 'P': return نوع_رخصة::شخصي;
            case 'R': return نوع_رخصة::احترافي;
            case 'T': return نوع_رخصة::فريق;
            case 'E': return نوع_رخصة::مؤسسي;
            case 'D': return نوع_رخصة::تعليمي;
            case 'X': return نوع_رخصة::تجريبي;
            default: return نوع_رخصة::مجاني;
        }
    }
};

// ============== مدير التراخيص ==============

/**
 * مدير التراخيص
 */
class مدير_تراخيص {
private:
    std::map<std::string, رخصة> التراخيص_;  // بالمفتاح
    مولد_مفاتيح مولد_;
    
    // إعدادات
    std::string خادم_تحقق_ = "https://license.sad-lang.org";
    bool وضع_اختبار_ = true;
    
public:
    مدير_تراخيص() = default;
    
    // ============== إدارة التراخيص ==============
    
    /**
     * إنشاء ترخيص جديد
     */
    رخصة إنشاء_رخصة(const std::string& معرف_حزمة,
                     const std::string& معرف_مالك,
                     const std::string& اسم_مالك,
                     const std::string& بريد_مالك,
                     نوع_رخصة النوع,
                     int مدة_بالأشهر = 12) {
        رخصة ر;
        
        // معلومات أساسية
        ر.معرف = توليد_معرف();
        ر.مفتاح = مولد_.توليد_مفتاح(معرف_حزمة, النوع);
        ر.معرف_حزمة = معرف_حزمة;
        ر.معرف_مالك = معرف_مالك;
        ر.اسم_مالك = اسم_مالك;
        ر.بريد_مالك = بريد_مالك;
        ر.النوع = النوع;
        
        // التواريخ
        ر.تاريخ_إصدار = std::chrono::system_clock::now();
        ر.تاريخ_انتهاء = ر.تاريخ_إصدار + 
            std::chrono::hours(24 * 30 * مدة_بالأشهر);
        
        // القيود حسب النوع
        ر.القيود = القيود_حسب_النوع(النوع);
        
        // حفظ
        التراخيص_[ر.مفتاح] = ر;
        
        return ر;
    }
    
    /**
     * تفعيل ترخيص
     */
    نتيجة_تفعيل تفعيل(const طلب_تفعيل& طلب) {
        نتيجة_تفعيل نتيجة;
        
        // التحقق من صيغة المفتاح
        if (!مولد_.التحقق_من_الصيغة(طلب.مفتاح_رخصة)) {
            نتيجة.نجح = false;
            نتيجة.رسالة = "صيغة مفتاح غير صالحة";
            return نتيجة;
        }
        
        // البحث عن الترخيص
        auto it = التراخيص_.find(طلب.مفتاح_رخصة);
        if (it == التراخيص_.end()) {
            نتيجة.نجح = false;
            نتيجة.رسالة = "مفتاح ترخيص غير موجود";
            return نتيجة;
        }
        
        auto& رخصة = it->second;
        
        // التحقق من الحالة
        if (رخصة.الحالة != حالة_رخصة::نشطة) {
            نتيجة.نجح = false;
            نتيجة.رسالة = "الترخيص غير نشط";
            return نتيجة;
        }
        
        // التحقق من الانتهاء
        auto now = std::chrono::system_clock::now();
        if (now > رخصة.تاريخ_انتهاء) {
            رخصة.الحالة = حالة_رخصة::منتهية;
            نتيجة.نجح = false;
            نتيجة.رسالة = "الترخيص منتهي الصلاحية";
            return نتيجة;
        }
        
        // التحقق من عدد الأجهزة
        if (رخصة.أجهزة_مسجلة.size() >= رخصة.القيود.أقصى_أجهزة) {
            // هل الجهاز مسجل مسبقاً؟
            bool مسجل = false;
            for (const auto& جهاز : رخصة.أجهزة_مسجلة) {
                if (جهاز == طلب.معرف_جهاز) {
                    مسجل = true;
                    break;
                }
            }
            
            if (!مسجل) {
                نتيجة.نجح = false;
                نتيجة.رسالة = "تم الوصول للحد الأقصى من الأجهزة";
                return نتيجة;
            }
        }
        
        // تسجيل الجهاز
        bool موجود = false;
        for (const auto& جهاز : رخصة.أجهزة_مسجلة) {
            if (جهاز == طلب.معرف_جهاز) {
                موجود = true;
                break;
            }
        }
        if (!موجود) {
            رخصة.أجهزة_مسجلة.push_back(طلب.معرف_جهاز);
        }
        
        رخصة.عدد_التفعيلات++;
        
        نتيجة.نجح = true;
        نتيجة.رسالة = "تم التفعيل بنجاح";
        نتيجة.الرخصة = رخصة;
        نتيجة.انتهاء_صلاحية = رخصة.تاريخ_انتهاء;
        
        return نتيجة;
    }
    
    /**
     * التحقق من صلاحية الترخيص
     */
    bool التحقق_من_الصلاحية(const std::string& مفتاح,
                            const std::string& معرف_جهاز = "") {
        auto it = التراخيص_.find(مفتاح);
        if (it == التراخيص_.end()) return false;
        
        const auto& رخصة = it->second;
        
        // التحقق من الحالة
        if (رخصة.الحالة != حالة_رخصة::نشطة) return false;
        
        // التحقق من الانتهاء
        if (std::chrono::system_clock::now() > رخصة.تاريخ_انتهاء) {
            return false;
        }
        
        // التحقق من الجهاز إن وجد
        if (!معرف_جهاز.empty()) {
            bool موجود = false;
            for (const auto& جهاز : رخصة.أجهزة_مسجلة) {
                if (جهاز == معرف_جهاز) {
                    موجود = true;
                    break;
                }
            }
            if (!موجود) return false;
        }
        
        return true;
    }
    
    /**
     * إلغاء ترخيص
     */
    bool إلغاء_رخصة(const std::string& مفتاح) {
        auto it = التراخيص_.find(مفتاح);
        if (it == التراخيص_.end()) return false;
        
        it->second.الحالة = حالة_رخصة::ملغية;
        return true;
    }
    
    /**
     * تجديد ترخيص
     */
    bool تجديد_رخصة(const std::string& مفتاح, int أشهر_إضافية = 12) {
        auto it = التراخيص_.find(مفتاح);
        if (it == التراخيص_.end()) return false;
        
        auto& رخصة = it->second;
        
        // التمديد من الآن أو من تاريخ الانتهاء
        auto now = std::chrono::system_clock::now();
        auto بداية = (now > رخصة.تاريخ_انتهاء) ? now : رخصة.تاريخ_انتهاء;
        
        رخصة.تاريخ_انتهاء = بداية + 
            std::chrono::hours(24 * 30 * أشهر_إضافية);
        رخصة.الحالة = حالة_رخصة::نشطة;
        
        return true;
    }
    
    /**
     * إزالة جهاز من الترخيص
     */
    bool إزالة_جهاز(const std::string& مفتاح, const std::string& معرف_جهاز) {
        auto it = التراخيص_.find(مفتاح);
        if (it == التراخيص_.end()) return false;
        
        auto& أجهزة = it->second.أجهزة_مسجلة;
        auto جهاز_it = std::find(أجهزة.begin(), أجهزة.end(), معرف_جهاز);
        
        if (جهاز_it != أجهزة.end()) {
            أجهزة.erase(جهاز_it);
            return true;
        }
        
        return false;
    }
    
    /**
     * الحصول على معلومات الترخيص
     */
    std::optional<رخصة> الحصول_على_رخصة(const std::string& مفتاح) const {
        auto it = التراخيص_.find(مفتاح);
        if (it != التراخيص_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    /**
     * تراخيص المالك
     */
    std::vector<رخصة> تراخيص_المالك(const std::string& معرف_مالك) const {
        std::vector<رخصة> نتائج;
        for (const auto& [_, ر] : التراخيص_) {
            if (ر.معرف_مالك == معرف_مالك) {
                نتائج.push_back(ر);
            }
        }
        return نتائج;
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
        return "lic_" + std::to_string(ms) + "_" + std::to_string(++عداد);
    }
    
    /**
     * القيود حسب نوع الترخيص
     */
    قيود_رخصة القيود_حسب_النوع(نوع_رخصة نوع) {
        قيود_رخصة ق;
        
        switch (نوع) {
            case نوع_رخصة::شخصي:
                ق.أقصى_أجهزة = 1;
                ق.أقصى_مستخدمين = 1;
                ق.استخدام_تجاري = false;
                ق.دعم = false;
                break;
                
            case نوع_رخصة::احترافي:
                ق.أقصى_أجهزة = 5;
                ق.أقصى_مستخدمين = 1;
                ق.استخدام_تجاري = true;
                ق.دعم = true;
                break;
                
            case نوع_رخصة::فريق:
                ق.أقصى_أجهزة = 100;
                ق.أقصى_مستخدمين = 50;
                ق.استخدام_تجاري = true;
                ق.دعم = true;
                break;
                
            case نوع_رخصة::مؤسسي:
                ق.أقصى_أجهزة = 10000;
                ق.أقصى_مستخدمين = 5000;
                ق.استخدام_تجاري = true;
                ق.توزيع = true;
                ق.تعديل = true;
                ق.دعم = true;
                break;
                
            case نوع_رخصة::تعليمي:
                ق.أقصى_أجهزة = 2;
                ق.أقصى_مستخدمين = 1;
                ق.استخدام_تجاري = false;
                ق.دعم = false;
                break;
                
            case نوع_رخصة::تجريبي:
                ق.أقصى_أجهزة = 1;
                ق.أقصى_مستخدمين = 1;
                ق.استخدام_تجاري = false;
                ق.دعم = false;
                break;
                
            case نوع_رخصة::مجاني:
            default:
                ق.أقصى_أجهزة = 1000;
                ق.أقصى_مستخدمين = 1000;
                ق.استخدام_تجاري = true;
                ق.توزيع = true;
                ق.تعديل = true;
                ق.دعم = false;
                break;
        }
        
        return ق;
    }
};

}  // namespace ص::سوق

// ============== C API ==============

extern "C" {

using namespace ص::سوق;

typedef struct مدير_تراخيص_مقبض {
    std::unique_ptr<مدير_تراخيص> مدير;
} مدير_تراخيص_مقبض;

مدير_تراخيص_مقبض* sad_license_create() {
    auto مقبض = new مدير_تراخيص_مقبض();
    مقبض->مدير = std::make_unique<مدير_تراخيص>();
    return مقبض;
}

void sad_license_destroy(مدير_تراخيص_مقبض* مقبض) {
    delete مقبض;
}

const char* sad_license_generate(مدير_تراخيص_مقبض* مقبض,
                                 const char* package_id,
                                 const char* owner_id,
                                 int license_type) {
    static std::string result;
    if (مقبض && مقبض->مدير) {
        auto ر = مقبض->مدير->إنشاء_رخصة(
            package_id ? package_id : "",
            owner_id ? owner_id : "",
            "", "", 
            static_cast<نوع_رخصة>(license_type),
            12
        );
        result = ر.مفتاح;
        return result.c_str();
    }
    return "";
}

int sad_license_validate(مدير_تراخيص_مقبض* مقبض,
                         const char* key,
                         const char* device_id) {
    if (!مقبض || !مقبض->مدير) return 0;
    
    return مقبض->مدير->التحقق_من_الصلاحية(
        key ? key : "",
        device_id ? device_id : ""
    ) ? 1 : 0;
}

int sad_license_activate(مدير_تراخيص_مقبض* مقبض,
                         const char* key,
                         const char* device_id,
                         const char* device_name) {
    if (!مقبض || !مقبض->مدير) return 0;
    
    طلب_تفعيل طلب;
    طلب.مفتاح_رخصة = key ? key : "";
    طلب.معرف_جهاز = device_id ? device_id : "";
    طلب.اسم_جهاز = device_name ? device_name : "";
    
    auto نتيجة = مقبض->مدير->تفعيل(طلب);
    return نتيجة.نجح ? 1 : 0;
}

}  // extern "C"
