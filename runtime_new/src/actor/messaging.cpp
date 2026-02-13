/**
 * ==========================================================
 * ملف: messaging.cpp
 * الوصف: نظام تمرير الرسائل بين Actors
 * المهمة: T360 - توزيع ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * تمرير الرسائل هو الطريقة الوحيدة للتواصل بين Actors:
 * - أرسل("رسالة") - إرسال بدون انتظار
 * - اطلب("رسالة") - إرسال مع انتظار الرد
 * - استمع() - الاستماع للرسائل الواردة
 * 
 * 💡 مثال:
 * ```
 * // إنشاء Actor
 * متغير عداد = عداد_Actor.جديد()
 * 
 * // إرسال رسالة
 * عداد.أرسل("زِد")
 * 
 * // طلب مع انتظار
 * متغير قيمة = عداد.اطلب("احصل")
 * ```
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <functional>

namespace sad {
namespace runtime {
namespace actor {

// ==========================================================
// 📌 الرسالة
// ==========================================================

/**
 * أولوية الرسالة
 */
enum class أولوية_رسالة {
    منخفضة = 0,
    عادية = 1,
    عالية = 2,
    حرجة = 3
};

/**
 * رسالة موسعة
 */
struct رسالة {
    uint64_t معرف;
    std::string مرسل;
    std::string مستلم;
    std::string اسم;
    std::vector<uint8_t> حمولة;
    أولوية_رسالة أولوية = أولوية_رسالة::عادية;
    uint64_t معرف_الطلب = 0;  // للربط بين الطلب والرد
    std::chrono::system_clock::time_point وقت_الإرسال;
    
    rسالة() : معرف(توليد_معرف()) {
        وقت_الإرسال = std::chrono::system_clock::now();
    }
    
    static uint64_t توليد_معرف() {
        static uint64_t عداد = 0;
        return ++عداد;
    }
};

// ==========================================================
// 📌 صندوق الرسائل
// ==========================================================

/**
 * صندوق رسائل Actor
 */
class صندوق_رسائل {
private:
    std::queue<رسالة> صندوق_;
    std::mutex قفل_;
    std::condition_variable شرط_;
    size_t حد_السعة_ = 10000;
    
public:
    /**
     * إضافة رسالة
     */
    bool ضع(const رسالة& ر) {
        std::lock_guard<std::mutex> ق(قفل_);
        if (صندوق_.size() >= حد_السعة_) {
            return false;  // الصندوق ممتلئ
        }
        صندوق_.push(ر);
        شرط_.notify_one();
        return true;
    }
    
    /**
     * أخذ رسالة (بانتظار)
     */
    رسالة خذ() {
        std::unique_lock<std::mutex> ق(قفل_);
        شرط_.wait(ق, [this] { return !صندوق_.empty(); });
        رسالة ر = صندوق_.front();
        صندوق_.pop();
        return ر;
    }
    
    /**
     * أخذ رسالة مع timeout
     */
    bool خذ_مع_مهلة(رسالة& ر, int ms) {
        std::unique_lock<std::mutex> ق(قفل_);
        if (!شرط_.wait_for(ق, std::chrono::milliseconds(ms),
                           [this] { return !صندوق_.empty(); })) {
            return false;
        }
        ر = صندوق_.front();
        صندوق_.pop();
        return true;
    }
    
    /**
     * محاولة أخذ بدون انتظار
     */
    bool حاول_خذ(رسالة& ر) {
        std::lock_guard<std::mutex> ق(قفل_);
        if (صندوق_.empty()) return false;
        ر = صندوق_.front();
        صندوق_.pop();
        return true;
    }
    
    /**
     * عدد الرسائل
     */
    size_t حجم() const {
        std::lock_guard<std::mutex> ق(const_cast<std::mutex&>(قفل_));
        return صندوق_.size();
    }
    
    bool فارغ() const { return حجم() == 0; }
    
    void عيّن_السعة(size_t سعة) { حد_السعة_ = سعة; }
};

// ==========================================================
// 📌 ناقل الرسائل
// ==========================================================

/**
 * معالج رسالة
 */
using معالج_استلام = std::function<void(const رسالة&)>;

/**
 * ناقل الرسائل المركزي
 */
class ناقل_الرسائل {
private:
    std::map<std::string, صندوق_رسائل*> صناديق_;
    std::map<uint64_t, std::promise<رسالة>*> انتظار_الردود_;
    std::mutex قفل_;
    
public:
    /**
     * تسجيل صندوق رسائل
     */
    void سجّل(const std::string& معرف, صندوق_رسائل* صندوق) {
        std::lock_guard<std::mutex> ق(قفل_);
        صناديق_[معرف] = صندوق;
    }
    
    /**
     * إلغاء تسجيل
     */
    void ألغِ_تسجيل(const std::string& معرف) {
        std::lock_guard<std::mutex> ق(قفل_);
        صناديق_.erase(معرف);
    }
    
    /**
     * إرسال رسالة
     */
    bool أرسل(const رسالة& ر) {
        std::lock_guard<std::mutex> ق(قفل_);
        
        auto it = صناديق_.find(ر.مستلم);
        if (it == صناديق_.end()) {
            return false;  // المستلم غير موجود
        }
        
        return it->second->ضع(ر);
    }
    
    /**
     * إرسال وانتظار الرد
     */
    رسالة اطلب(const رسالة& طلب, int timeout_ms = 5000) {
        // إنشاء promise للرد
        std::promise<رسالة> وعد;
        auto مستقبل = وعد.get_future();
        
        {
            std::lock_guard<std::mutex> ق(قفل_);
            انتظار_الردود_[طلب.معرف] = &وعد;
        }
        
        // إرسال الطلب
        أرسل(طلب);
        
        // انتظار الرد
        if (مستقبل.wait_for(std::chrono::milliseconds(timeout_ms)) 
            == std::future_status::timeout) {
            // timeout
            std::lock_guard<std::mutex> ق(قفل_);
            انتظار_الردود_.erase(طلب.معرف);
            رسالة فارغ;
            return فارغ;
        }
        
        return مستقبل.get();
    }
    
    /**
     * إرسال رد
     */
    void رد(const رسالة& طلب_أصلي, const رسالة& الرد) {
        std::lock_guard<std::mutex> ق(قفل_);
        
        auto it = انتظار_الردود_.find(طلب_أصلي.معرف);
        if (it != انتظار_الردود_.end()) {
            it->second->set_value(الرد);
            انتظار_الردود_.erase(it);
        }
    }
    
    /**
     * بث رسالة للجميع
     */
    void بث(const رسالة& ر) {
        std::lock_guard<std::mutex> ق(قفل_);
        for (auto& [معرف, صندوق] : صناديق_) {
            if (معرف != ر.مرسل) {
                صندوق->ضع(ر);
            }
        }
    }
    
    /**
     * عدد الـ Actors المسجلين
     */
    size_t عدد_المسجلين() const {
        std::lock_guard<std::mutex> ق(const_cast<std::mutex&>(قفل_));
        return صناديق_.size();
    }
};

// ==========================================================
// 📌 مُرسل الرسائل
// ==========================================================

/**
 * واجهة لإرسال الرسائل
 */
class مُرسل {
private:
    std::string معرف_المرسل_;
    ناقل_الرسائل* ناقل_;
    
public:
    مُرسل(const std::string& معرف, ناقل_الرسائل* ناقل)
        : معرف_المرسل_(معرف), ناقل_(ناقل) {}
    
    /**
     * إرسال رسالة
     */
    void أرسل(const std::string& مستلم, const std::string& اسم,
              const std::vector<uint8_t>& حمولة = {}) {
        رسالة ر;
        ر.مرسل = معرف_المرسل_;
        ر.مستلم = مستلم;
        ر.اسم = اسم;
        ر.حمولة = حمولة;
        
        ناقل_->أرسل(ر);
    }
    
    /**
     * إرسال وانتظار
     */
    رسالة اطلب(const std::string& مستلم, const std::string& اسم,
               const std::vector<uint8_t>& حمولة = {},
               int timeout_ms = 5000) {
        رسالة ر;
        ر.مرسل = معرف_المرسل_;
        ر.مستلم = مستلم;
        ر.اسم = اسم;
        ر.حمولة = حمولة;
        
        return ناقل_->اطلب(ر, timeout_ms);
    }
    
    /**
     * بث للجميع
     */
    void بث(const std::string& اسم, const std::vector<uint8_t>& حمولة = {}) {
        رسالة ر;
        ر.مرسل = معرف_المرسل_;
        ر.اسم = اسم;
        ر.حمولة = حمولة;
        
        ناقل_->بث(ر);
    }
};

// ==========================================================
// 📌 مستقبل الرسائل
// ==========================================================

/**
 * واجهة لاستقبال الرسائل
 */
class مُستقبل {
private:
    صندوق_رسائل صندوق_;
    std::map<std::string, معالج_استلام> معالجات_;
    
public:
    /**
     * تسجيل معالج
     */
    void عند(const std::string& اسم_الرسالة, معالج_استلام معالج) {
        معالجات_[اسم_الرسالة] = معالج;
    }
    
    /**
     * استقبال رسالة
     */
    رسالة استلم() {
        return صندوق_.خذ();
    }
    
    /**
     * استقبال مع مهلة
     */
    bool استلم_مع_مهلة(رسالة& ر, int ms) {
        return صندوق_.خذ_مع_مهلة(ر, ms);
    }
    
    /**
     * معالجة رسالة
     */
    void عالج(const رسالة& ر) {
        auto it = معالجات_.find(ر.اسم);
        if (it != معالجات_.end()) {
            it->second(ر);
        }
    }
    
    /**
     * حلقة المعالجة
     */
    void ابدأ_المعالجة() {
        while (true) {
            رسالة ر = استلم();
            if (ر.اسم == "__stop__") break;
            عالج(ر);
        }
    }
    
    صندوق_رسائل* صندوق() { return &صندوق_; }
};

// ==========================================================
// 📌 الناقل العالمي
// ==========================================================

/**
 * الحصول على الناقل العالمي
 */
ناقل_الرسائل& الناقل_العالمي() {
    static ناقل_الرسائل ناقل;
    return ناقل;
}

} // namespace actor
} // namespace runtime
} // namespace sad

// ==========================================================
// 📌 واجهة C
// ==========================================================

extern "C" {

using namespace sad::runtime::actor;

// صندوق الرسائل
void* sad_mailbox_create() {
    return new صندوق_رسائل();
}

void sad_mailbox_destroy(void* mailbox) {
    delete static_cast<صندوق_رسائل*>(mailbox);
}

int sad_mailbox_size(void* mailbox) {
    return static_cast<int>(static_cast<صندوق_رسائل*>(mailbox)->حجم());
}

// ناقل الرسائل
void* sad_message_bus_create() {
    return new ناقل_الرسائل();
}

void sad_message_bus_destroy(void* bus) {
    delete static_cast<ناقل_الرسائل*>(bus);
}

void sad_message_bus_register(void* bus, const char* id, void* mailbox) {
    static_cast<ناقل_الرسائل*>(bus)->سجّل(id, 
        static_cast<صندوق_رسائل*>(mailbox));
}

void sad_message_bus_unregister(void* bus, const char* id) {
    static_cast<ناقل_الرسائل*>(bus)->ألغِ_تسجيل(id);
}

// الناقل العالمي
void* sad_global_message_bus() {
    return &الناقل_العالمي();
}

} // extern "C"
