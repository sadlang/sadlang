/**
 * ===================================================================================
 *  نظام التدريب المؤسسي - Enterprise Training System (T435)
 * ===================================================================================
 * 
 *  الوصف: نظام تدريب مخصص للمؤسسات
 *  Description: Custom training programs for enterprises
 *
 *  يدعم:
 *  - برامج تدريب مخصصة
 *  - ورش عمل
 *  - شهادات مؤسسية
 *  - تتبع تقدم الموظفين
 * 
 * ===================================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <ctime>

namespace ص::مؤسسة {

// ============== أنواع التدريب ==============

/**
 * نوع البرنامج
 */
enum class نوع_برنامج {
    تأهيلي,        // Onboarding
    مهاري,         // Skill building
    تخصصي,        // Specialized
    قيادي,         // Leadership
    تقني,          // Technical
    مخصص           // Custom
};

/**
 * تنسيق التدريب
 */
enum class تنسيق_تدريب {
    حضوري,         // In-person
    عن_بعد,        // Remote
    هجين,          // Hybrid
    ذاتي,          // Self-paced
    مباشر          // Live
};

/**
 * حالة البرنامج
 */
enum class حالة_برنامج {
    مسودة,
    قيد_التطوير,
    جاهز,
    نشط,
    مؤرشف
};

/**
 * مستوى البرنامج
 */
enum class مستوى_برنامج {
    مبتدئ,
    متوسط,
    متقدم,
    خبير
};

// ============== الوحدة التدريبية ==============

/**
 * وحدة تدريبية
 */
struct وحدة_تدريبية {
    std::string معرف;
    std::string الاسم;
    std::string الوصف;
    
    int المدة_دقائق = 60;
    int الترتيب = 0;
    
    // المحتوى
    std::vector<std::string> الشرائح;
    std::vector<std::string> الفيديوهات;
    std::vector<std::string> المراجع;
    
    // التمارين
    std::vector<std::string> تمارين_عملية;
    
    // الاختبار
    bool يتطلب_اختبار = false;
    int درجة_النجاح = 70;
};

// ============== البرنامج التدريبي ==============

/**
 * برنامج تدريبي
 */
struct برنامج_تدريب {
    std::string معرف;
    std::string الاسم;
    std::string الوصف;
    
    نوع_برنامج النوع;
    تنسيق_تدريب التنسيق;
    مستوى_برنامج المستوى;
    حالة_برنامج الحالة = حالة_برنامج::مسودة;
    
    // المالك
    std::string معرف_مؤسسة;
    std::string مدرب_رئيسي;
    
    // الوحدات
    std::vector<وحدة_تدريبية> الوحدات;
    
    // المدة
    int المدة_الكلية_ساعات = 0;
    int عدد_الجلسات = 0;
    
    // الشهادة
    bool يمنح_شهادة = false;
    std::string اسم_الشهادة;
    int ساعات_معتمدة = 0;
    
    // التسعير
    double السعر_للفرد = 0.0;
    double السعر_للمجموعة = 0.0;
    int الحد_الأقصى_للمشاركين = 20;
    
    // التواريخ
    std::string تاريخ_الإنشاء;
    std::string تاريخ_التحديث;
};

// ============== الجلسة ==============

/**
 * جلسة تدريب
 */
struct جلسة_تدريب {
    std::string معرف;
    std::string معرف_برنامج;
    
    std::string التاريخ;
    std::string وقت_البدء;
    std::string وقت_الانتهاء;
    
    std::string المدرب;
    std::string الموقع;  // أو رابط
    
    std::vector<std::string> المشاركون;
    int الحد_الأقصى = 20;
    
    bool ملغاة = false;
};

// ============== تقدم المتدرب ==============

/**
 * تقدم متدرب في برنامج
 */
struct تقدم_متدرب {
    std::string معرف_متدرب;
    std::string معرف_برنامج;
    
    // الوحدات المكتملة
    std::vector<std::string> وحدات_مكتملة;
    
    // الدرجات
    std::map<std::string, int> درجات_الاختبارات;
    
    // الحضور
    std::vector<std::string> جلسات_حضرها;
    
    // الحالة
    std::string تاريخ_التسجيل;
    std::string تاريخ_الإكمال;
    bool مكتمل = false;
    
    // الشهادة
    bool حصل_على_شهادة = false;
    std::string رقم_الشهادة;
};

// ============== الشهادة ==============

/**
 * شهادة تدريب
 */
struct شهادة_تدريب {
    std::string الرقم;
    std::string معرف_متدرب;
    std::string اسم_المتدرب;
    
    std::string معرف_برنامج;
    std::string اسم_البرنامج;
    
    std::string تاريخ_الإصدار;
    std::string تاريخ_الانتهاء;  // إن وجد
    
    int الساعات_المعتمدة = 0;
    
    std::string رابط_التحقق;
};

// ============== نظام التدريب ==============

/**
 * نظام التدريب المؤسسي
 */
class نظام_تدريب {
private:
    std::map<std::string, برنامج_تدريب> البرامج_;
    std::map<std::string, جلسة_تدريب> الجلسات_;
    std::map<std::string, تقدم_متدرب> التقدم_;
    std::map<std::string, شهادة_تدريب> الشهادات_;
    
    int عداد_ = 0;
    
public:
    // ============== إدارة البرامج ==============
    
    std::string إنشاء_برنامج(const std::string& معرف_مؤسسة,
                              const std::string& الاسم,
                              نوع_برنامج النوع,
                              تنسيق_تدريب التنسيق,
                              مستوى_برنامج المستوى) {
        برنامج_تدريب ب;
        ب.معرف = "TRN-" + std::to_string(++عداد_);
        ب.الاسم = الاسم;
        ب.معرف_مؤسسة = معرف_مؤسسة;
        ب.النوع = النوع;
        ب.التنسيق = التنسيق;
        ب.المستوى = المستوى;
        
        time_t now = time(nullptr);
        struct tm* tm_info = localtime(&now);
        char buffer[20];
        strftime(buffer, 20, "%Y-%m-%d", tm_info);
        ب.تاريخ_الإنشاء = buffer;
        ب.تاريخ_التحديث = buffer;
        
        البرامج_[ب.معرف] = ب;
        return ب.معرف;
    }
    
    برنامج_تدريب* الحصول_على_برنامج(const std::string& معرف) {
        auto it = البرامج_.find(معرف);
        return it != البرامج_.end() ? &it->second : nullptr;
    }
    
    bool إضافة_وحدة(const std::string& معرف_برنامج,
                     const وحدة_تدريبية& وحدة) {
        auto* ب = الحصول_على_برنامج(معرف_برنامج);
        if (!ب) return false;
        
        وحدة_تدريبية و = وحدة;
        و.معرف = معرف_برنامج + "-U" + std::to_string(ب->الوحدات.size() + 1);
        و.الترتيب = ب->الوحدات.size();
        
        ب->الوحدات.push_back(و);
        ب->المدة_الكلية_ساعات += و.المدة_دقائق / 60;
        
        return true;
    }
    
    // ============== الجلسات ==============
    
    std::string جدولة_جلسة(const std::string& معرف_برنامج,
                            const std::string& التاريخ,
                            const std::string& وقت_البدء,
                            const std::string& المدرب) {
        جلسة_تدريب ج;
        ج.معرف = "SES-" + std::to_string(++عداد_);
        ج.معرف_برنامج = معرف_برنامج;
        ج.التاريخ = التاريخ;
        ج.وقت_البدء = وقت_البدء;
        ج.المدرب = المدرب;
        
        الجلسات_[ج.معرف] = ج;
        return ج.معرف;
    }
    
    bool تسجيل_في_جلسة(const std::string& معرف_جلسة,
                        const std::string& معرف_متدرب) {
        auto it = الجلسات_.find(معرف_جلسة);
        if (it == الجلسات_.end()) return false;
        
        auto& ج = it->second;
        if (ج.المشاركون.size() >= static_cast<size_t>(ج.الحد_الأقصى)) {
            return false;
        }
        
        ج.المشاركون.push_back(معرف_متدرب);
        return true;
    }
    
    // ============== التقدم ==============
    
    bool تسجيل_في_برنامج(const std::string& معرف_متدرب,
                          const std::string& معرف_برنامج) {
        std::string مفتاح = معرف_متدرب + ":" + معرف_برنامج;
        
        if (التقدم_.find(مفتاح) != التقدم_.end()) {
            return false;  // مسجل مسبقاً
        }
        
        تقدم_متدرب ت;
        ت.معرف_متدرب = معرف_متدرب;
        ت.معرف_برنامج = معرف_برنامج;
        
        time_t now = time(nullptr);
        struct tm* tm_info = localtime(&now);
        char buffer[20];
        strftime(buffer, 20, "%Y-%m-%d", tm_info);
        ت.تاريخ_التسجيل = buffer;
        
        التقدم_[مفتاح] = ت;
        return true;
    }
    
    bool إكمال_وحدة(const std::string& معرف_متدرب,
                     const std::string& معرف_وحدة,
                     int درجة_الاختبار = -1) {
        // استخراج معرف البرنامج من معرف الوحدة
        size_t pos = معرف_وحدة.find("-U");
        if (pos == std::string::npos) return false;
        
        std::string معرف_برنامج = معرف_وحدة.substr(0, pos);
        std::string مفتاح = معرف_متدرب + ":" + معرف_برنامج;
        
        auto it = التقدم_.find(مفتاح);
        if (it == التقدم_.end()) return false;
        
        auto& ت = it->second;
        ت.وحدات_مكتملة.push_back(معرف_وحدة);
        
        if (درجة_الاختبار >= 0) {
            ت.درجات_الاختبارات[معرف_وحدة] = درجة_الاختبار;
        }
        
        // تحقق من إكمال البرنامج
        auto* ب = الحصول_على_برنامج(معرف_برنامج);
        if (ب && ت.وحدات_مكتملة.size() >= ب->الوحدات.size()) {
            ت.مكتمل = true;
            
            time_t now = time(nullptr);
            struct tm* tm_info = localtime(&now);
            char buffer[20];
            strftime(buffer, 20, "%Y-%m-%d", tm_info);
            ت.تاريخ_الإكمال = buffer;
            
            // إصدار شهادة إذا كان البرنامج يمنحها
            if (ب->يمنح_شهادة) {
                إصدار_شهادة(معرف_متدرب, معرف_برنامج, "متدرب");
            }
        }
        
        return true;
    }
    
    تقدم_متدرب* الحصول_على_تقدم(const std::string& معرف_متدرب,
                                   const std::string& معرف_برنامج) {
        std::string مفتاح = معرف_متدرب + ":" + معرف_برنامج;
        auto it = التقدم_.find(مفتاح);
        return it != التقدم_.end() ? &it->second : nullptr;
    }
    
    double نسبة_الإكمال(const std::string& معرف_متدرب,
                        const std::string& معرف_برنامج) {
        auto* ت = الحصول_على_تقدم(معرف_متدرب, معرف_برنامج);
        auto* ب = الحصول_على_برنامج(معرف_برنامج);
        
        if (!ت || !ب || ب->الوحدات.empty()) return 0.0;
        
        return static_cast<double>(ت->وحدات_مكتملة.size()) / 
               ب->الوحدات.size() * 100.0;
    }
    
    // ============== الشهادات ==============
    
    std::string إصدار_شهادة(const std::string& معرف_متدرب,
                             const std::string& معرف_برنامج,
                             const std::string& اسم_المتدرب) {
        auto* ب = الحصول_على_برنامج(معرف_برنامج);
        if (!ب) return "";
        
        شهادة_تدريب ش;
        ش.الرقم = "CERT-" + std::to_string(++عداد_);
        ش.معرف_متدرب = معرف_متدرب;
        ش.اسم_المتدرب = اسم_المتدرب;
        ش.معرف_برنامج = معرف_برنامج;
        ش.اسم_البرنامج = ب->الاسم;
        ش.الساعات_المعتمدة = ب->ساعات_معتمدة;
        
        time_t now = time(nullptr);
        struct tm* tm_info = localtime(&now);
        char buffer[20];
        strftime(buffer, 20, "%Y-%m-%d", tm_info);
        ش.تاريخ_الإصدار = buffer;
        
        ش.رابط_التحقق = "https://sad-lang.com/verify/" + ش.الرقم;
        
        الشهادات_[ش.الرقم] = ش;
        
        // تحديث تقدم المتدرب
        std::string مفتاح = معرف_متدرب + ":" + معرف_برنامج;
        auto it = التقدم_.find(مفتاح);
        if (it != التقدم_.end()) {
            it->second.حصل_على_شهادة = true;
            it->second.رقم_الشهادة = ش.الرقم;
        }
        
        return ش.الرقم;
    }
    
    شهادة_تدريب* التحقق_من_شهادة(const std::string& رقم_شهادة) {
        auto it = الشهادات_.find(رقم_شهادة);
        return it != الشهادات_.end() ? &it->second : nullptr;
    }
    
    // ============== التقارير ==============
    
    std::vector<برنامج_تدريب> برامج_المؤسسة(const std::string& معرف_مؤسسة) const {
        std::vector<برنامج_تدريب> نتيجة;
        
        for (const auto& [_, ب] : البرامج_) {
            if (ب.معرف_مؤسسة == معرف_مؤسسة) {
                نتيجة.push_back(ب);
            }
        }
        
        return نتيجة;
    }
    
    int عدد_المتدربين_في_برنامج(const std::string& معرف_برنامج) const {
        int عدد = 0;
        
        for (const auto& [_, ت] : التقدم_) {
            if (ت.معرف_برنامج == معرف_برنامج) {
                عدد++;
            }
        }
        
        return عدد;
    }
    
    int عدد_الشهادات_الصادرة(const std::string& معرف_برنامج) const {
        int عدد = 0;
        
        for (const auto& [_, ش] : الشهادات_) {
            if (ش.معرف_برنامج == معرف_برنامج) {
                عدد++;
            }
        }
        
        return عدد;
    }
};

}  // namespace ص::مؤسسة

// ============== C API ==============

extern "C" {

using namespace ص::مؤسسة;

static نظام_تدريب* النظام_العام = nullptr;

void sad_training_init() {
    if (!النظام_العام) {
        النظام_العام = new نظام_تدريب();
    }
}

void sad_training_cleanup() {
    delete النظام_العام;
    النظام_العام = nullptr;
}

const char* sad_training_create_program(const char* معرف_مؤسسة,
                                         const char* الاسم,
                                         int النوع,
                                         int التنسيق,
                                         int المستوى) {
    if (!النظام_العام) return "";
    
    static std::string نتيجة;
    نتيجة = النظام_العام->إنشاء_برنامج(
        معرف_مؤسسة,
        الاسم,
        static_cast<نوع_برنامج>(النوع),
        static_cast<تنسيق_تدريب>(التنسيق),
        static_cast<مستوى_برنامج>(المستوى)
    );
    
    return نتيجة.c_str();
}

int sad_training_enroll(const char* معرف_متدرب, const char* معرف_برنامج) {
    if (!النظام_العام) return 0;
    return النظام_العام->تسجيل_في_برنامج(معرف_متدرب, معرف_برنامج) ? 1 : 0;
}

int sad_training_complete_unit(const char* معرف_متدرب,
                                const char* معرف_وحدة,
                                int درجة) {
    if (!النظام_العام) return 0;
    return النظام_العام->إكمال_وحدة(معرف_متدرب, معرف_وحدة, درجة) ? 1 : 0;
}

double sad_training_progress(const char* معرف_متدرب, const char* معرف_برنامج) {
    if (!النظام_العام) return 0.0;
    return النظام_العام->نسبة_الإكمال(معرف_متدرب, معرف_برنامج);
}

const char* sad_training_issue_certificate(const char* معرف_متدرب,
                                            const char* معرف_برنامج,
                                            const char* اسم_متدرب) {
    if (!النظام_العام) return "";
    
    static std::string نتيجة;
    نتيجة = النظام_العام->إصدار_شهادة(معرف_متدرب, معرف_برنامج, اسم_متدرب);
    
    return نتيجة.c_str();
}

int sad_training_verify_certificate(const char* رقم_شهادة) {
    if (!النظام_العام) return 0;
    return النظام_العام->التحقق_من_شهادة(رقم_شهادة) ? 1 : 0;
}

}  // extern "C"
