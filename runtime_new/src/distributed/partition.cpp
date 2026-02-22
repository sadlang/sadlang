/**
 * ===================================================================================
 *  تحمل التقسيم - Partition Tolerance (T445)
 * ===================================================================================
 * 
 *  الوصف: معالجة تقسيم الشبكة والتحمل
 *  Description: Network partition handling and tolerance
 *
 *  يدعم:
 *  - اكتشاف التقسيم
 *  - العمل المنفصل
 *  - دمج البيانات
 *  - حل التعارضات
 * 
 * ===================================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <mutex>
#include <atomic>
#include <cstdint>
#include <functional>
#include <algorithm>

namespace ص::موزع {

// ============== أنواع التقسيم ==============

/**
 * حالة التقسيم
 */
enum class حالة_تقسيم {
    طبيعي,          // Normal operation
    مشتبه,          // Suspected partition
    مؤكد,           // Confirmed partition
    متعافي          // Recovering
};

/**
 * استراتيجية حل التعارض
 */
enum class استراتيجية_حل {
    آخر_كتابة,       // Last-write-wins
    أول_كتابة,       // First-write-wins
    دمج,             // Merge
    إصدار_أعلى,     // Higher version wins
    مخصص             // Custom resolver
};

// ============== ساعة منطقية ==============

/**
 * ساعة لامبورت المنطقية
 */
class ساعة_لامبورت {
private:
    std::atomic<uint64_t> الوقت_{0};
    std::string معرف_العقدة_;
    
public:
    explicit ساعة_لامبورت(const std::string& معرف)
        : معرف_العقدة_(معرف) {}
    
    uint64_t تقدم() {
        return ++الوقت_;
    }
    
    void تحديث(uint64_t وقت_خارجي) {
        uint64_t حالي = الوقت_.load();
        while (حالي < وقت_خارجي && 
               !الوقت_.compare_exchange_weak(حالي, وقت_خارجي + 1)) {
        }
    }
    
    uint64_t الوقت_الحالي() const { return الوقت_.load(); }
};

/**
 * ساعة المتجهات
 */
class ساعة_متجهات {
private:
    std::map<std::string, uint64_t> المتجه_;
    std::string معرف_العقدة_;
    std::mutex قفل_;
    
public:
    explicit ساعة_متجهات(const std::string& معرف)
        : معرف_العقدة_(معرف) {
        المتجه_[معرف] = 0;
    }
    
    void تقدم() {
        std::lock_guard<std::mutex> قفل(قفل_);
        المتجه_[معرف_العقدة_]++;
    }
    
    void دمج(const std::map<std::string, uint64_t>& متجه_خارجي) {
        std::lock_guard<std::mutex> قفل(قفل_);
        
        for (const auto& [عقدة, وقت] : متجه_خارجي) {
            المتجه_[عقدة] = std::max(المتجه_[عقدة], وقت);
        }
        
        المتجه_[معرف_العقدة_]++;
    }
    
    std::map<std::string, uint64_t> الحصول_على_المتجه() const {
        return المتجه_;
    }
    
    // مقارنة: -1 = سبق، 0 = متزامن، 1 = لاحق، 2 = متعارض
    int مقارنة(const std::map<std::string, uint64_t>& أخر) const {
        bool سبق = false;
        bool لاحق = false;
        
        std::set<std::string> كل_العقد;
        for (const auto& [ع, _] : المتجه_) كل_العقد.insert(ع);
        for (const auto& [ع, _] : أخر) كل_العقد.insert(ع);
        
        for (const auto& عقدة : كل_العقد) {
            uint64_t محلي = 0;
            uint64_t خارجي = 0;
            
            auto it1 = المتجه_.find(عقدة);
            if (it1 != المتجه_.end()) محلي = it1->second;
            
            auto it2 = أخر.find(عقدة);
            if (it2 != أخر.end()) خارجي = it2->second;
            
            if (محلي < خارجي) سبق = true;
            if (محلي > خارجي) لاحق = true;
        }
        
        if (سبق && لاحق) return 2;  // متعارض
        if (سبق) return -1;
        if (لاحق) return 1;
        return 0;  // متساوي
    }
};

// ============== كشف التقسيم ==============

/**
 * كاشف تقسيم الشبكة
 */
class كاشف_تقسيم {
private:
    struct معلومات_عقدة {
        std::string المعرف;
        uint64_t آخر_اتصال = 0;
        int فشل_متتالي = 0;
    };
    
    std::map<std::string, معلومات_عقدة> العقد_;
    std::set<std::string> العقد_المعزولة_;
    
    حالة_تقسيم الحالة_ = حالة_تقسيم::طبيعي;
    
    // الإعدادات
    uint64_t مهلة_الاتصال_ = 5;      // ثواني
    int عتبة_الفشل_ = 3;             // فشل متتالي
    double عتبة_التقسيم_ = 0.5;      // نسبة العقد المعزولة
    
    std::mutex قفل_;
    
public:
    // ============== التكوين ==============
    
    void تعيين_مهلة(uint64_t ثواني) { مهلة_الاتصال_ = ثواني; }
    void تعيين_عتبة_الفشل(int عتبة) { عتبة_الفشل_ = عتبة; }
    
    // ============== إدارة العقد ==============
    
    void تسجيل_عقدة(const std::string& معرف) {
        std::lock_guard<std::mutex> قفل(قفل_);
        
        معلومات_عقدة معلومات;
        معلومات.المعرف = معرف;
        معلومات.آخر_اتصال = static_cast<uint64_t>(time(nullptr));
        
        العقد_[معرف] = معلومات;
    }
    
    void تسجيل_نجاح(const std::string& معرف) {
        std::lock_guard<std::mutex> قفل(قفل_);
        
        auto it = العقد_.find(معرف);
        if (it != العقد_.end()) {
            it->second.آخر_اتصال = static_cast<uint64_t>(time(nullptr));
            it->second.فشل_متتالي = 0;
            
            العقد_المعزولة_.erase(معرف);
        }
    }
    
    void تسجيل_فشل(const std::string& معرف) {
        std::lock_guard<std::mutex> قفل(قفل_);
        
        auto it = العقد_.find(معرف);
        if (it != العقد_.end()) {
            it->second.فشل_متتالي++;
            
            if (it->second.فشل_متتالي >= عتبة_الفشل_) {
                العقد_المعزولة_.insert(معرف);
            }
        }
    }
    
    // ============== الكشف ==============
    
    حالة_تقسيم فحص() {
        std::lock_guard<std::mutex> قفل(قفل_);
        
        if (العقد_.empty()) {
            الحالة_ = حالة_تقسيم::طبيعي;
            return الحالة_;
        }
        
        uint64_t الآن = static_cast<uint64_t>(time(nullptr));
        
        // فحص العقد المنتهية الوقت
        for (auto& [معرف, معلومات] : العقد_) {
            if (الآن - معلومات.آخر_اتصال > مهلة_الاتصال_) {
                معلومات.فشل_متتالي++;
                if (معلومات.فشل_متتالي >= عتبة_الفشل_) {
                    العقد_المعزولة_.insert(معرف);
                }
            }
        }
        
        // تحديد حالة التقسيم
        double نسبة_المعزولة = 
            static_cast<double>(العقد_المعزولة_.size()) / العقد_.size();
        
        if (نسبة_المعزولة == 0) {
            الحالة_ = حالة_تقسيم::طبيعي;
        } else if (نسبة_المعزولة < عتبة_التقسيم_) {
            الحالة_ = حالة_تقسيم::مشتبه;
        } else {
            الحالة_ = حالة_تقسيم::مؤكد;
        }
        
        return الحالة_;
    }
    
    حالة_تقسيم الحالة() const { return الحالة_; }
    
    std::set<std::string> العقد_المعزولة() const {
        return العقد_المعزولة_;
    }
    
    std::set<std::string> العقد_النشطة() const {
        std::set<std::string> نشطة;
        
        for (const auto& [معرف, _] : العقد_) {
            if (العقد_المعزولة_.find(معرف) == العقد_المعزولة_.end()) {
                نشطة.insert(معرف);
            }
        }
        
        return نشطة;
    }
};

// ============== حل التعارضات ==============

/**
 * قيمة متعارضة
 */
struct قيمة_متعارضة {
    std::vector<uint8_t> البيانات;
    std::map<std::string, uint64_t> الساعة;
    std::string مصدر;
    uint64_t الطابع_الزمني = 0;
};

/**
 * حالل التعارضات
 */
class حالل_تعارضات {
private:
    استراتيجية_حل الاستراتيجية_ = استراتيجية_حل::آخر_كتابة;
    
    std::function<قيمة_متعارضة(const std::vector<قيمة_متعارضة>&)>
        حالل_مخصص_;
    
public:
    void تعيين_استراتيجية(استراتيجية_حل استراتيجية) {
        الاستراتيجية_ = استراتيجية;
    }
    
    void تعيين_حالل_مخصص(
        std::function<قيمة_متعارضة(const std::vector<قيمة_متعارضة>&)> حالل) {
        حالل_مخصص_ = حالل;
    }
    
    قيمة_متعارضة حل(const std::vector<قيمة_متعارضة>& القيم) {
        if (القيم.empty()) {
            return {};
        }
        
        if (القيم.size() == 1) {
            return القيم[0];
        }
        
        switch (الاستراتيجية_) {
            case استراتيجية_حل::آخر_كتابة:
                return حل_آخر_كتابة(القيم);
                
            case استراتيجية_حل::أول_كتابة:
                return حل_أول_كتابة(القيم);
                
            case استراتيجية_حل::دمج:
                return حل_دمج(القيم);
                
            case استراتيجية_حل::إصدار_أعلى:
                return حل_إصدار_أعلى(القيم);
                
            case استراتيجية_حل::مخصص:
                if (حالل_مخصص_) {
                    return حالل_مخصص_(القيم);
                }
                return حل_آخر_كتابة(القيم);
                
            default:
                return حل_آخر_كتابة(القيم);
        }
    }
    
private:
    قيمة_متعارضة حل_آخر_كتابة(const std::vector<قيمة_متعارضة>& القيم) {
        auto أحدث = std::max_element(القيم.begin(), القيم.end(),
            [](const قيمة_متعارضة& أ, const قيمة_متعارضة& ب) {
                return أ.الطابع_الزمني < ب.الطابع_الزمني;
            });
        
        return *أحدث;
    }
    
    قيمة_متعارضة حل_أول_كتابة(const std::vector<قيمة_متعارضة>& القيم) {
        auto أقدم = std::min_element(القيم.begin(), القيم.end(),
            [](const قيمة_متعارضة& أ, const قيمة_متعارضة& ب) {
                return أ.الطابع_الزمني < ب.الطابع_الزمني;
            });
        
        return *أقدم;
    }
    
    قيمة_متعارضة حل_دمج(const std::vector<قيمة_متعارضة>& القيم) {
        // دمج بسيط: اتحاد البيانات
        قيمة_متعارضة نتيجة;
        
        for (const auto& ق : القيم) {
            // دمج البيانات (تبسيط)
            نتيجة.البيانات.insert(نتيجة.البيانات.end(),
                                   ق.البيانات.begin(), ق.البيانات.end());
            
            // دمج الساعات
            for (const auto& [عقدة, وقت] : ق.الساعة) {
                نتيجة.الساعة[عقدة] = std::max(نتيجة.الساعة[عقدة], وقت);
            }
            
            // أحدث طابع
            نتيجة.الطابع_الزمني = std::max(نتيجة.الطابع_الزمني, ق.الطابع_الزمني);
        }
        
        return نتيجة;
    }
    
    قيمة_متعارضة حل_إصدار_أعلى(const std::vector<قيمة_متعارضة>& القيم) {
        // اختيار القيمة ذات أعلى مجموع ساعات
        auto أعلى = std::max_element(القيم.begin(), القيم.end(),
            [](const قيمة_متعارضة& أ, const قيمة_متعارضة& ب) {
                uint64_t مجموع_أ = 0, مجموع_ب = 0;
                for (const auto& [_, و] : أ.الساعة) مجموع_أ += و;
                for (const auto& [_, و] : ب.الساعة) مجموع_ب += و;
                return مجموع_أ < مجموع_ب;
            });
        
        return *أعلى;
    }
};

}  // namespace ص::موزع

// ============== C API ==============

extern "C" {

using namespace ص::موزع;

static كاشف_تقسيم* الكاشف_العام = nullptr;
static حالل_تعارضات* الحالل_العام = nullptr;

void sad_partition_init() {
    if (!الكاشف_العام) {
        الكاشف_العام = new كاشف_تقسيم();
        الحالل_العام = new حالل_تعارضات();
    }
}

void sad_partition_cleanup() {
    delete الكاشف_العام;
    delete الحالل_العام;
    الكاشف_العام = nullptr;
    الحالل_العام = nullptr;
}

void sad_partition_register_node(const char* معرف) {
    if (الكاشف_العام) {
        الكاشف_العام->تسجيل_عقدة(معرف);
    }
}

void sad_partition_success(const char* معرف) {
    if (الكاشف_العام) {
        الكاشف_العام->تسجيل_نجاح(معرف);
    }
}

void sad_partition_failure(const char* معرف) {
    if (الكاشف_العام) {
        الكاشف_العام->تسجيل_فشل(معرف);
    }
}

int sad_partition_check() {
    if (!الكاشف_العام) return 0;
    return static_cast<int>(الكاشف_العام->فحص());
}

void sad_conflict_set_strategy(int استراتيجية) {
    if (الحالل_العام) {
        الحالل_العام->تعيين_استراتيجية(
            static_cast<استراتيجية_حل>(استراتيجية));
    }
}

}  // extern "C"
