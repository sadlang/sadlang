/**
 * ===================================================================================
 *  نظام التقييمات والمراجعات - Reviews System (T408)
 * ===================================================================================
 * 
 *  الوصف: نظام تقييمات ومراجعات للحزم
 *  Description: Ratings and reviews system for packages
 *
 *  المميزات:
 *  - تقييمات من 1-5 نجوم
 *  - مراجعات نصية
 *  - التصويت على المراجعات
 *  - ردود المطورين
 * 
 * ===================================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <algorithm>
#include <optional>

namespace ص::سوق {

// ============== الأنواع ==============

/**
 * تقييم (نجوم فقط)
 */
struct تقييم {
    std::string معرف;
    std::string معرف_حزمة;
    std::string معرف_مستخدم;
    std::string اسم_مستخدم;
    
    int نجوم;  // 1-5
    std::chrono::system_clock::time_point تاريخ;
};

/**
 * مراجعة (نجوم + نص)
 */
struct مراجعة {
    std::string معرف;
    std::string معرف_حزمة;
    std::string معرف_مستخدم;
    std::string اسم_مستخدم;
    std::string صورة_مستخدم;
    
    int نجوم;  // 1-5
    std::string عنوان;
    std::string محتوى;
    
    // الإصدار عند المراجعة
    std::string إصدار_حزمة;
    
    // التصويتات
    int تصويت_إيجابي = 0;
    int تصويت_سلبي = 0;
    
    // رد المطور
    std::optional<std::string> رد_مطور;
    std::optional<std::chrono::system_clock::time_point> تاريخ_رد;
    
    // الحالة
    bool موثقة = false;    // المستخدم اشترى/حمّل الحزمة
    bool معدلة = false;
    bool محذوفة = false;
    
    std::chrono::system_clock::time_point تاريخ;
    std::optional<std::chrono::system_clock::time_point> تاريخ_تعديل;
};

/**
 * ملخص التقييمات
 */
struct ملخص_تقييمات {
    double متوسط = 0.0;
    uint32_t إجمالي = 0;
    
    uint32_t نجوم_5 = 0;
    uint32_t نجوم_4 = 0;
    uint32_t نجوم_3 = 0;
    uint32_t نجوم_2 = 0;
    uint32_t نجوم_1 = 0;
    
    // النسب المئوية
    double نسبة_5 = 0.0;
    double نسبة_4 = 0.0;
    double نسبة_3 = 0.0;
    double نسبة_2 = 0.0;
    double نسبة_1 = 0.0;
};

/**
 * خيارات ترتيب المراجعات
 */
enum class ترتيب_مراجعات {
    الأحدث,
    الأقدم,
    الأعلى_تقييماً,
    الأكثر_فائدة,
    الأكثر_تصويتاً
};

/**
 * فلتر المراجعات
 */
struct فلتر_مراجعات {
    std::optional<int> نجوم;
    bool موثقة_فقط = false;
    bool مع_رد_فقط = false;
    ترتيب_مراجعات ترتيب = ترتيب_مراجعات::الأحدث;
    int صفحة = 1;
    int حجم_صفحة = 10;
};

// ============== مدير التقييمات ==============

/**
 * مدير التقييمات والمراجعات
 */
class مدير_تقييمات {
private:
    std::map<std::string, std::vector<مراجعة>> مراجعات_الحزم_;
    std::map<std::string, std::map<std::string, int>> تصويتات_المستخدمين_;
    
    // كاش الملخصات
    std::map<std::string, ملخص_تقييمات> ملخصات_;
    
public:
    مدير_تقييمات() = default;
    
    // ============== المراجعات ==============
    
    /**
     * إضافة مراجعة
     */
    std::string إضافة_مراجعة(مراجعة& م) {
        م.معرف = توليد_معرف();
        م.تاريخ = std::chrono::system_clock::now();
        
        // التحقق من وجود مراجعة سابقة
        auto& مراجعات = مراجعات_الحزم_[م.معرف_حزمة];
        for (auto& موجودة : مراجعات) {
            if (موجودة.معرف_مستخدم == م.معرف_مستخدم && !موجودة.محذوفة) {
                // تحديث المراجعة الموجودة بدلاً من إضافة جديدة
                موجودة.نجوم = م.نجوم;
                موجودة.عنوان = م.عنوان;
                موجودة.محتوى = م.محتوى;
                موجودة.إصدار_حزمة = م.إصدار_حزمة;
                موجودة.معدلة = true;
                موجودة.تاريخ_تعديل = std::chrono::system_clock::now();
                تحديث_ملخص(م.معرف_حزمة);
                return موجودة.معرف;
            }
        }
        
        مراجعات.push_back(م);
        تحديث_ملخص(م.معرف_حزمة);
        
        return م.معرف;
    }
    
    /**
     * تعديل مراجعة
     */
    bool تعديل_مراجعة(const std::string& معرف,
                       const std::string& معرف_مستخدم,
                       int نجوم,
                       const std::string& عنوان,
                       const std::string& محتوى) {
        for (auto& [حزمة, مراجعات] : مراجعات_الحزم_) {
            for (auto& م : مراجعات) {
                if (م.معرف == معرف && م.معرف_مستخدم == معرف_مستخدم) {
                    م.نجوم = نجوم;
                    م.عنوان = عنوان;
                    م.محتوى = محتوى;
                    م.معدلة = true;
                    م.تاريخ_تعديل = std::chrono::system_clock::now();
                    تحديث_ملخص(حزمة);
                    return true;
                }
            }
        }
        return false;
    }
    
    /**
     * حذف مراجعة
     */
    bool حذف_مراجعة(const std::string& معرف, const std::string& معرف_مستخدم) {
        for (auto& [حزمة, مراجعات] : مراجعات_الحزم_) {
            for (auto& م : مراجعات) {
                if (م.معرف == معرف && م.معرف_مستخدم == معرف_مستخدم) {
                    م.محذوفة = true;
                    تحديث_ملخص(حزمة);
                    return true;
                }
            }
        }
        return false;
    }
    
    /**
     * الحصول على مراجعة
     */
    std::optional<مراجعة> الحصول_على_مراجعة(const std::string& معرف) const {
        for (const auto& [_, مراجعات] : مراجعات_الحزم_) {
            for (const auto& م : مراجعات) {
                if (م.معرف == معرف && !م.محذوفة) {
                    return م;
                }
            }
        }
        return std::nullopt;
    }
    
    /**
     * مراجعات حزمة
     */
    std::vector<مراجعة> مراجعات_حزمة(const std::string& معرف_حزمة,
                                     const فلتر_مراجعات& فلتر = {}) {
        std::vector<مراجعة> نتائج;
        
        auto it = مراجعات_الحزم_.find(معرف_حزمة);
        if (it == مراجعات_الحزم_.end()) return نتائج;
        
        for (const auto& م : it->second) {
            if (م.محذوفة) continue;
            
            // فلتر النجوم
            if (فلتر.نجوم && م.نجوم != *فلتر.نجوم) continue;
            
            // فلتر الموثقة
            if (فلتر.موثقة_فقط && !م.موثقة) continue;
            
            // فلتر مع رد
            if (فلتر.مع_رد_فقط && !م.رد_مطور) continue;
            
            نتائج.push_back(م);
        }
        
        // الترتيب
        switch (فلتر.ترتيب) {
            case ترتيب_مراجعات::الأحدث:
                std::sort(نتائج.begin(), نتائج.end(),
                    [](const مراجعة& أ, const مراجعة& ب) {
                        return أ.تاريخ > ب.تاريخ;
                    });
                break;
                
            case ترتيب_مراجعات::الأقدم:
                std::sort(نتائج.begin(), نتائج.end(),
                    [](const مراجعة& أ, const مراجعة& ب) {
                        return أ.تاريخ < ب.تاريخ;
                    });
                break;
                
            case ترتيب_مراجعات::الأعلى_تقييماً:
                std::sort(نتائج.begin(), نتائج.end(),
                    [](const مراجعة& أ, const مراجعة& ب) {
                        return أ.نجوم > ب.نجوم;
                    });
                break;
                
            case ترتيب_مراجعات::الأكثر_فائدة:
            case ترتيب_مراجعات::الأكثر_تصويتاً:
                std::sort(نتائج.begin(), نتائج.end(),
                    [](const مراجعة& أ, const مراجعة& ب) {
                        int صافي_أ = أ.تصويت_إيجابي - أ.تصويت_سلبي;
                        int صافي_ب = ب.تصويت_إيجابي - ب.تصويت_سلبي;
                        return صافي_أ > صافي_ب;
                    });
                break;
        }
        
        // التقسيم للصفحات
        int بداية = (فلتر.صفحة - 1) * فلتر.حجم_صفحة;
        int نهاية = std::min(بداية + فلتر.حجم_صفحة, (int)نتائج.size());
        
        if (بداية < نتائج.size()) {
            return std::vector<مراجعة>(نتائج.begin() + بداية, نتائج.begin() + نهاية);
        }
        
        return {};
    }
    
    // ============== التصويت ==============
    
    /**
     * التصويت على مراجعة
     */
    bool تصويت(const std::string& معرف_مراجعة,
               const std::string& معرف_مستخدم,
               bool إيجابي) {
        // البحث عن المراجعة
        مراجعة* م = nullptr;
        for (auto& [_, مراجعات] : مراجعات_الحزم_) {
            for (auto& مراجعة : مراجعات) {
                if (مراجعة.معرف == معرف_مراجعة) {
                    م = &مراجعة;
                    break;
                }
            }
            if (م) break;
        }
        
        if (!م) return false;
        
        // التحقق من التصويت السابق
        auto& تصويتات = تصويتات_المستخدمين_[معرف_مستخدم];
        auto it = تصويتات.find(معرف_مراجعة);
        
        if (it != تصويتات.end()) {
            // إزالة التصويت السابق
            if (it->second > 0) م->تصويت_إيجابي--;
            else م->تصويت_سلبي--;
            
            // إذا نفس التصويت، إلغاء فقط
            if ((إيجابي && it->second > 0) || (!إيجابي && it->second < 0)) {
                تصويتات.erase(it);
                return true;
            }
        }
        
        // إضافة التصويت الجديد
        if (إيجابي) {
            م->تصويت_إيجابي++;
            تصويتات[معرف_مراجعة] = 1;
        } else {
            م->تصويت_سلبي++;
            تصويتات[معرف_مراجعة] = -1;
        }
        
        return true;
    }
    
    // ============== رد المطور ==============
    
    /**
     * إضافة رد المطور
     */
    bool إضافة_رد_مطور(const std::string& معرف_مراجعة,
                        const std::string& رد) {
        for (auto& [_, مراجعات] : مراجعات_الحزم_) {
            for (auto& م : مراجعات) {
                if (م.معرف == معرف_مراجعة) {
                    م.رد_مطور = رد;
                    م.تاريخ_رد = std::chrono::system_clock::now();
                    return true;
                }
            }
        }
        return false;
    }
    
    // ============== الملخص ==============
    
    /**
     * ملخص تقييمات حزمة
     */
    ملخص_تقييمات ملخص_حزمة(const std::string& معرف_حزمة) {
        auto it = ملخصات_.find(معرف_حزمة);
        if (it != ملخصات_.end()) {
            return it->second;
        }
        
        // حساب الملخص
        تحديث_ملخص(معرف_حزمة);
        return ملخصات_[معرف_حزمة];
    }
    
    // ============== الإحصائيات ==============
    
    /**
     * مراجعات المستخدم
     */
    std::vector<مراجعة> مراجعات_مستخدم(const std::string& معرف_مستخدم) const {
        std::vector<مراجعة> نتائج;
        
        for (const auto& [_, مراجعات] : مراجعات_الحزم_) {
            for (const auto& م : مراجعات) {
                if (م.معرف_مستخدم == معرف_مستخدم && !م.محذوفة) {
                    نتائج.push_back(م);
                }
            }
        }
        
        return نتائج;
    }
    
private:
    /**
     * تحديث ملخص حزمة
     */
    void تحديث_ملخص(const std::string& معرف_حزمة) {
        ملخص_تقييمات ملخص;
        
        auto it = مراجعات_الحزم_.find(معرف_حزمة);
        if (it == مراجعات_الحزم_.end()) {
            ملخصات_[معرف_حزمة] = ملخص;
            return;
        }
        
        double مجموع = 0;
        for (const auto& م : it->second) {
            if (م.محذوفة) continue;
            
            ملخص.إجمالي++;
            مجموع += م.نجوم;
            
            switch (م.نجوم) {
                case 5: ملخص.نجوم_5++; break;
                case 4: ملخص.نجوم_4++; break;
                case 3: ملخص.نجوم_3++; break;
                case 2: ملخص.نجوم_2++; break;
                case 1: ملخص.نجوم_1++; break;
            }
        }
        
        if (ملخص.إجمالي > 0) {
            ملخص.متوسط = مجموع / ملخص.إجمالي;
            ملخص.نسبة_5 = 100.0 * ملخص.نجوم_5 / ملخص.إجمالي;
            ملخص.نسبة_4 = 100.0 * ملخص.نجوم_4 / ملخص.إجمالي;
            ملخص.نسبة_3 = 100.0 * ملخص.نجوم_3 / ملخص.إجمالي;
            ملخص.نسبة_2 = 100.0 * ملخص.نجوم_2 / ملخص.إجمالي;
            ملخص.نسبة_1 = 100.0 * ملخص.نجوم_1 / ملخص.إجمالي;
        }
        
        ملخصات_[معرف_حزمة] = ملخص;
    }
    
    std::string توليد_معرف() {
        static int عداد = 0;
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        return "rev_" + std::to_string(ms) + "_" + std::to_string(++عداد);
    }
};

}  // namespace ص::سوق

// ============== C API ==============

extern "C" {

using namespace ص::سوق;

typedef struct مدير_تقييمات_مقبض {
    std::unique_ptr<مدير_تقييمات> مدير;
} مدير_تقييمات_مقبض;

مدير_تقييمات_مقبض* sad_reviews_create() {
    auto مقبض = new مدير_تقييمات_مقبض();
    مقبض->مدير = std::make_unique<مدير_تقييمات>();
    return مقبض;
}

void sad_reviews_destroy(مدير_تقييمات_مقبض* مقبض) {
    delete مقبض;
}

const char* sad_reviews_add(مدير_تقييمات_مقبض* مقبض,
                            const char* package_id,
                            const char* user_id,
                            const char* user_name,
                            int stars,
                            const char* title,
                            const char* content) {
    static std::string result;
    if (مقبض && مقبض->مدير) {
        مراجعة م;
        م.معرف_حزمة = package_id ? package_id : "";
        م.معرف_مستخدم = user_id ? user_id : "";
        م.اسم_مستخدم = user_name ? user_name : "";
        م.نجوم = stars;
        م.عنوان = title ? title : "";
        م.محتوى = content ? content : "";
        
        result = مقبض->مدير->إضافة_مراجعة(م);
        return result.c_str();
    }
    return "";
}

double sad_reviews_average(مدير_تقييمات_مقبض* مقبض, const char* package_id) {
    if (مقبض && مقبض->مدير) {
        return مقبض->مدير->ملخص_حزمة(package_id ? package_id : "").متوسط;
    }
    return 0.0;
}

int sad_reviews_vote(مدير_تقييمات_مقبض* مقبض,
                     const char* review_id,
                     const char* user_id,
                     int positive) {
    if (مقبض && مقبض->مدير) {
        return مقبض->مدير->تصويت(
            review_id ? review_id : "",
            user_id ? user_id : "",
            positive != 0
        ) ? 1 : 0;
    }
    return 0;
}

}  // extern "C"
