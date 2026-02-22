/**
 * ==========================================================
 * ملف: cluster.cpp
 * الوصف: إدارة عنقود Actors الموزع
 * المهمة: T363 - توزيع ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * العنقود (Cluster) هو مجموعة عقد متصلة:
 * - اكتشاف العقد تلقائياً
 * - موازنة الحمل بين العقد
 * - تجاوز الفشل تلقائياً
 * 
 * 💡 مثال:
 * ```
 * // إنشاء عنقود
 * متغير عنقود = عنقود.جديد("تطبيقي")
 * عنقود.انضم("192.168.1.100:5000")
 * 
 * // إنشاء Actor موزع
 * متغير خادم = عنقود.انشر(خادم_Actor)
 * ```
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <mutex>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>

namespace sad {
namespace runtime {
namespace actor {

// ==========================================================
// 📌 معلومات العقدة
// ==========================================================

/**
 * حالة العقدة
 */
enum class حالة_عقدة {
    منفصلة,        // غير متصلة
    تتصل,          // في طور الاتصال
    متصلة,         // متصلة وتعمل
    مريبة,         // لم ترد على النبضات
    غير_متاحة     // فشلت نهائياً
};

/**
 * معلومات العقدة
 */
struct معلومات_عقدة {
    std::string الاسم;
    std::string عنوان_IP;
    int المنفذ;
    حالة_عقدة الحالة = حالة_عقدة::منفصلة;
    std::chrono::system_clock::time_point آخر_نبضة;
    int عدد_actors = 0;
    double الحمل = 0.0;  // 0.0 - 1.0
    std::map<std::string, std::string> قدرات;
    
    std::string العنوان_الكامل() const {
        return عنوان_IP + ":" + std::to_string(المنفذ);
    }
};

// ==========================================================
// 📌 استراتيجية التوزيع
// ==========================================================

/**
 * استراتيجية توزيع Actors
 */
enum class استراتيجية_توزيع {
    دائري,          // round-robin
    أقل_حمل,        // least-loaded
    عشوائي,         // random
    محلي_أولاً,    // local-first
    بالقدرة         // by-capability
};

// ==========================================================
// 📌 إدارة العنقود
// ==========================================================

/**
 * مدير العنقود
 */
class مدير_العنقود {
private:
    std::string اسم_العنقود_;
    معلومات_عقدة العقدة_المحلية_;
    std::map<std::string, معلومات_عقدة> العقد_;
    mutable std::mutex قفل_;
    
    std::atomic<bool> يعمل_{false};
    std::thread خيط_النبضات_;
    استراتيجية_توزيع استراتيجية_ = استراتيجية_توزيع::أقل_حمل;
    
    // إعدادات
    int فترة_النبضة_ms_ = 1000;
    int حد_فقدان_النبضات_ = 3;
    
public:
    مدير_العنقود(const std::string& اسم, 
                 const std::string& ip,
                 int منفذ)
        : اسم_العنقود_(اسم) {
        العقدة_المحلية_.الاسم = اسم + "_" + std::to_string(منفذ);
        العقدة_المحلية_.عنوان_IP = ip;
        العقدة_المحلية_.المنفذ = منفذ;
        العقدة_المحلية_.الحالة = حالة_عقدة::متصلة;
    }
    
    ~مدير_العنقود() {
        أوقف();
    }
    
    /**
     * بدء العنقود
     */
    void ابدأ() {
        يعمل_ = true;
        خيط_النبضات_ = std::thread([this] { حلقة_النبضات(); });
    }
    
    /**
     * إيقاف العنقود
     */
    void أوقف() {
        يعمل_ = false;
        if (خيط_النبضات_.joinable()) {
            خيط_النبضات_.join();
        }
    }
    
    /**
     * الانضمام لعقدة
     */
    bool انضم(const std::string& عنوان) {
        معلومات_عقدة عقدة;
        
        // تحليل العنوان
        size_t نقطتين = عنوان.find(':');
        if (نقطتين == std::string::npos) return false;
        
        عقدة.عنوان_IP = عنوان.substr(0, نقطتين);
        عقدة.المنفذ = std::stoi(عنوان.substr(نقطتين + 1));
        عقدة.الاسم = عنوان;
        عقدة.الحالة = حالة_عقدة::تتصل;
        
        {
            std::lock_guard<std::mutex> ق(قفل_);
            العقد_[عنوان] = عقدة;
        }
        
        // محاولة الاتصال
        return اتصل_بعقدة(عنوان);
    }
    
    /**
     * مغادرة العنقود
     */
    void غادر() {
        // إشعار العقد الأخرى
        std::lock_guard<std::mutex> ق(قفل_);
        العقد_.clear();
    }
    
    /**
     * اختيار عقدة للنشر
     */
    معلومات_عقدة* اختر_عقدة() {
        std::lock_guard<std::mutex> ق(قفل_);
        
        switch (استراتيجية_) {
            case استراتيجية_توزيع::محلي_أولاً:
                return &العقدة_المحلية_;
                
            case استراتيجية_توزيع::أقل_حمل:
                return اختر_أقل_حمل();
                
            case استراتيجية_توزيع::دائري:
                return اختر_دائري();
                
            default:
                return &العقدة_المحلية_;
        }
    }
    
    /**
     * قائمة العقد
     */
    std::vector<معلومات_عقدة> العقد() const {
        std::lock_guard<std::mutex> ق(const_cast<std::mutex&>(قفل_));
        
        std::vector<معلومات_عقدة> نتيجة;
        نتيجة.push_back(العقدة_المحلية_);
        for (const auto& [_, عقدة] : العقد_) {
            نتيجة.push_back(عقدة);
        }
        return نتيجة;
    }
    
    /**
     * عدد العقد المتصلة
     */
    int عدد_العقد_المتصلة() const {
        std::lock_guard<std::mutex> ق(const_cast<std::mutex&>(قفل_));
        
        int عدد = 1;  // العقدة المحلية
        for (const auto& [_, عقدة] : العقد_) {
            if (عقدة.الحالة == حالة_عقدة::متصلة) {
                عدد++;
            }
        }
        return عدد;
    }
    
    /**
     * تحديث الحمل المحلي
     */
    void حدّث_الحمل(double حمل) {
        std::lock_guard<std::mutex> ق(قفل_);
        العقدة_المحلية_.الحمل = حمل;
    }
    
    /**
     * تعيين الاستراتيجية
     */
    void عيّن_استراتيجية(استراتيجية_توزيع س) {
        استراتيجية_ = س;
    }
    
    // Getters
    const std::string& اسم_العنقود() const { return اسم_العنقود_; }
    const معلومات_عقدة& العقدة_المحلية() const { return العقدة_المحلية_; }
    
private:
    /**
     * حلقة النبضات
     */
    void حلقة_النبضات() {
        while (يعمل_) {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(فترة_النبضة_ms_));
            
            أرسل_نبضات();
            تحقق_من_العقد();
        }
    }
    
    /**
     * إرسال نبضات للعقد
     */
    void أرسل_نبضات() {
        std::lock_guard<std::mutex> ق(قفل_);
        
        for (auto& [عنوان, عقدة] : العقد_) {
            if (عقدة.الحالة == حالة_عقدة::متصلة) {
                // إرسال نبضة قلب
            }
        }
    }
    
    /**
     * التحقق من حالة العقد
     */
    void تحقق_من_العقد() {
        std::lock_guard<std::mutex> ق(قفل_);
        
        auto الآن = std::chrono::system_clock::now();
        
        for (auto& [عنوان, عقدة] : العقد_) {
            if (عقدة.الحالة == حالة_عقدة::متصلة) {
                auto فرق = std::chrono::duration_cast<std::chrono::milliseconds>(
                    الآن - عقدة.آخر_نبضة).count();
                
                if (فرق > فترة_النبضة_ms_ * حد_فقدان_النبضات_) {
                    عقدة.الحالة = حالة_عقدة::مريبة;
                }
            }
        }
    }
    
    /**
     * الاتصال بعقدة
     */
    bool اتصل_بعقدة(const std::string& عنوان) {
        // محاكاة الاتصال
        std::lock_guard<std::mutex> ق(قفل_);
        
        auto it = العقد_.find(عنوان);
        if (it != العقد_.end()) {
            it->second.الحالة = حالة_عقدة::متصلة;
            it->second.آخر_نبضة = std::chrono::system_clock::now();
            return true;
        }
        return false;
    }
    
    /**
     * اختيار العقدة الأقل حملاً
     */
    معلومات_عقدة* اختر_أقل_حمل() {
        معلومات_عقدة* أفضل = &العقدة_المحلية_;
        double أقل = العقدة_المحلية_.الحمل;
        
        for (auto& [_, عقدة] : العقد_) {
            if (عقدة.الحالة == حالة_عقدة::متصلة &&
                عقدة.الحمل < أقل) {
                أقل = عقدة.الحمل;
                أفضل = &عقدة;
            }
        }
        
        return أفضل;
    }
    
    /**
     * اختيار دائري
     */
    معلومات_عقدة* اختر_دائري() {
        static size_t فهرس = 0;
        
        std::vector<معلومات_عقدة*> متاحة;
        متاحة.push_back(&العقدة_المحلية_);
        
        for (auto& [_, عقدة] : العقد_) {
            if (عقدة.الحالة == حالة_عقدة::متصلة) {
                متاحة.push_back(&عقدة);
            }
        }
        
        if (متاحة.empty()) return &العقدة_المحلية_;
        
        فهرس = (فهرس + 1) % متاحة.size();
        return متاحة[فهرس];
    }
};

// ==========================================================
// 📌 العنقود العالمي
// ==========================================================

static std::unique_ptr<مدير_العنقود> العنقود_العالمي_;

/**
 * تهيئة العنقود العالمي
 */
void هيّئ_العنقود(const std::string& اسم,
                  const std::string& ip,
                  int منفذ) {
    العنقود_العالمي_ = std::make_unique<مدير_العنقود>(اسم, ip, منفذ);
    العنقود_العالمي_->ابدأ();
}

/**
 * الحصول على العنقود العالمي
 */
مدير_العنقود* العنقود() {
    return العنقود_العالمي_.get();
}

} // namespace actor
} // namespace runtime
} // namespace sad

// ==========================================================
// 📌 واجهة C
// ==========================================================

extern "C" {

using namespace sad::runtime::actor;

// مدير العنقود
void* sad_cluster_create(const char* name, const char* ip, int port) {
    return new مدير_العنقود(name, ip, port);
}

void sad_cluster_destroy(void* cluster) {
    delete static_cast<مدير_العنقود*>(cluster);
}

void sad_cluster_start(void* cluster) {
    static_cast<مدير_العنقود*>(cluster)->ابدأ();
}

void sad_cluster_stop(void* cluster) {
    static_cast<مدير_العنقود*>(cluster)->أوقف();
}

int sad_cluster_join(void* cluster, const char* address) {
    return static_cast<مدير_العنقود*>(cluster)->انضم(address) ? 1 : 0;
}

void sad_cluster_leave(void* cluster) {
    static_cast<مدير_العنقود*>(cluster)->غادر();
}

int sad_cluster_node_count(void* cluster) {
    return static_cast<مدير_العنقود*>(cluster)->عدد_العقد_المتصلة();
}

void sad_cluster_set_load(void* cluster, double load) {
    static_cast<مدير_العنقود*>(cluster)->حدّث_الحمل(load);
}

// العنقود العالمي
void sad_global_cluster_init(const char* name, const char* ip, int port) {
    هيّئ_العنقود(name, ip, port);
}

void* sad_global_cluster() {
    return العنقود();
}

} // extern "C"
