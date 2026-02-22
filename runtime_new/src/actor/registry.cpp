/**
 * ==========================================================
 * ملف: registry.cpp
 * الوصف: سجل Actors الموزع
 * المهمة: T362 - توزيع ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * السجل الموزع يسمح بالعثور على Actors بالاسم:
 * - تسجيل Actor باسم فريد
 * - البحث عن Actor بالاسم
 * - مزامنة السجل عبر العقد
 * 
 * 💡 مثال:
 * ```
 * // تسجيل Actor
 * سجل.سجّل("خادم_ويب", خادم_actor)
 * 
 * // البحث والإرسال
 * متغير خادم = سجل.ابحث("خادم_ويب")
 * خادم.أرسل("طلب", بيانات)
 * ```
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <functional>

namespace sad {
namespace runtime {
namespace actor {

// ==========================================================
// 📌 معلومات Actor مسجل
// ==========================================================

/**
 * موقع Actor
 */
struct موقع_Actor {
    std::string عقدة;       // اسم العقدة
    std::string عنوان_IP;
    int منفذ = 0;
    bool محلي = true;
};

/**
 * معلومات Actor مسجل
 */
struct معلومات_مسجلة {
    std::string الاسم;
    std::string المعرف;
    موقع_Actor الموقع;
    std::chrono::system_clock::time_point وقت_التسجيل;
    std::chrono::system_clock::time_point آخر_نبضة;
    std::map<std::string, std::string> بيانات_وصفية;
    
    معلومات_مسجلة() {
        وقت_التسجيل = std::chrono::system_clock::now();
        آخر_نبضة = وقت_التسجيل;
    }
};

// ==========================================================
// 📌 السجل المحلي
// ==========================================================

/**
 * سجل Actors محلي
 */
class سجل_محلي {
private:
    std::map<std::string, معلومات_مسجلة> بالاسم_;
    std::map<std::string, std::string> معرف_إلى_اسم_;
    mutable std::shared_mutex قفل_;
    
public:
    /**
     * تسجيل Actor
     */
    bool سجّل(const std::string& اسم, const std::string& معرف,
              const موقع_Actor& موقع = {}) {
        std::unique_lock<std::shared_mutex> ق(قفل_);
        
        // التحقق من عدم وجود الاسم
        if (بالاسم_.find(اسم) != بالاسم_.end()) {
            return false;
        }
        
        معلومات_مسجلة معلومات;
        معلومات.الاسم = اسم;
        معلومات.المعرف = معرف;
        معلومات.الموقع = موقع;
        
        بالاسم_[اسم] = معلومات;
        معرف_إلى_اسم_[معرف] = اسم;
        
        return true;
    }
    
    /**
     * إلغاء التسجيل
     */
    bool ألغِ_تسجيل(const std::string& اسم) {
        std::unique_lock<std::shared_mutex> ق(قفل_);
        
        auto it = بالاسم_.find(اسم);
        if (it == بالاسم_.end()) return false;
        
        معرف_إلى_اسم_.erase(it->second.المعرف);
        بالاسم_.erase(it);
        
        return true;
    }
    
    /**
     * البحث بالاسم
     */
    const معلومات_مسجلة* ابحث(const std::string& اسم) const {
        std::shared_lock<std::shared_mutex> ق(قفل_);
        
        auto it = بالاسم_.find(اسم);
        if (it == بالاسم_.end()) return nullptr;
        return &it->second;
    }
    
    /**
     * البحث بالمعرف
     */
    const معلومات_مسجلة* ابحث_بالمعرف(const std::string& معرف) const {
        std::shared_lock<std::shared_mutex> ق(قفل_);
        
        auto it = معرف_إلى_اسم_.find(معرف);
        if (it == معرف_إلى_اسم_.end()) return nullptr;
        
        return ابحث(it->second);
    }
    
    /**
     * تحديث نبضة القلب
     */
    void حدّث_نبضة(const std::string& اسم) {
        std::unique_lock<std::shared_mutex> ق(قفل_);
        
        auto it = بالاسم_.find(اسم);
        if (it != بالاسم_.end()) {
            it->second.آخر_نبضة = std::chrono::system_clock::now();
        }
    }
    
    /**
     * قائمة جميع المسجلين
     */
    std::vector<std::string> قائمة() const {
        std::shared_lock<std::shared_mutex> ق(قفل_);
        
        std::vector<std::string> نتيجة;
        for (const auto& [اسم, _] : بالاسم_) {
            نتيجة.push_back(اسم);
        }
        return نتيجة;
    }
    
    /**
     * عدد المسجلين
     */
    size_t عدد() const {
        std::shared_lock<std::shared_mutex> ق(قفل_);
        return بالاسم_.size();
    }
    
    /**
     * إضافة بيانات وصفية
     */
    void أضف_بيانات(const std::string& اسم, 
                    const std::string& مفتاح,
                    const std::string& قيمة) {
        std::unique_lock<std::shared_mutex> ق(قفل_);
        
        auto it = بالاسم_.find(اسم);
        if (it != بالاسم_.end()) {
            it->second.بيانات_وصفية[مفتاح] = قيمة;
        }
    }
};

// ==========================================================
// 📌 السجل الموزع
// ==========================================================

/**
 * سجل Actors موزع
 */
class سجل_موزع {
private:
    سجل_محلي محلي_;
    std::string اسم_العقدة_;
    std::map<std::string, std::string> عقد_بعيدة_;  // عقدة -> عنوان
    mutable std::mutex قفل_عقد_;
    
public:
    explicit سجل_موزع(const std::string& اسم_عقدة = "local")
        : اسم_العقدة_(اسم_عقدة) {}
    
    /**
     * تسجيل Actor محلي
     */
    bool سجّل(const std::string& اسم, const std::string& معرف) {
        موقع_Actor موقع;
        موقع.عقدة = اسم_العقدة_;
        موقع.محلي = true;
        
        bool نجح = محلي_.سجّل(اسم, معرف, موقع);
        
        if (نجح) {
            // إشعار العقد البعيدة
            أشعر_العقد_بالتسجيل(اسم, معرف);
        }
        
        return نجح;
    }
    
    /**
     * تسجيل Actor بعيد (من عقدة أخرى)
     */
    bool سجّل_بعيد(const std::string& اسم, const std::string& معرف,
                   const موقع_Actor& موقع) {
        return محلي_.سجّل(اسم, معرف, موقع);
    }
    
    /**
     * البحث
     */
    const معلومات_مسجلة* ابحث(const std::string& اسم) const {
        // البحث محلياً أولاً
        const معلومات_مسجلة* نتيجة = محلي_.ابحث(اسم);
        if (نتيجة) return نتيجة;
        
        // البحث في العقد البعيدة
        return ابحث_في_عقد_بعيدة(اسم);
    }
    
    /**
     * إضافة عقدة بعيدة
     */
    void أضف_عقدة(const std::string& اسم, const std::string& عنوان) {
        std::lock_guard<std::mutex> ق(قفل_عقد_);
        عقد_بعيدة_[اسم] = عنوان;
    }
    
    /**
     * إزالة عقدة
     */
    void أزل_عقدة(const std::string& اسم) {
        std::lock_guard<std::mutex> ق(قفل_عقد_);
        عقد_بعيدة_.erase(اسم);
    }
    
    /**
     * مزامنة مع عقدة
     */
    void زامن_مع(const std::string& عقدة) {
        // إرسال قائمة المسجلين المحليين
        // استقبال قائمة المسجلين البعيدين
    }
    
    // Getters
    سجل_محلي& محلي() { return محلي_; }
    const std::string& اسم_العقدة() const { return اسم_العقدة_; }
    
    std::vector<std::string> العقد() const {
        std::lock_guard<std::mutex> ق(const_cast<std::mutex&>(قفل_عقد_));
        std::vector<std::string> نتيجة;
        for (const auto& [اسم, _] : عقد_بعيدة_) {
            نتيجة.push_back(اسم);
        }
        return نتيجة;
    }
    
private:
    void أشعر_العقد_بالتسجيل(const std::string& اسم, const std::string& معرف) {
        // إرسال رسالة تسجيل لجميع العقد البعيدة
        std::lock_guard<std::mutex> ق(قفل_عقد_);
        for (const auto& [عقدة, عنوان] : عقد_بعيدة_) {
            // أرسل إشعار تسجيل
        }
    }
    
    const معلومات_مسجلة* ابحث_في_عقد_بعيدة(const std::string& اسم) const {
        // البحث في العقد البعيدة
        return nullptr;  // سيتم التنفيذ لاحقاً
    }
};

// ==========================================================
// 📌 السجل العالمي
// ==========================================================

/**
 * الحصول على السجل العالمي
 */
سجل_موزع& السجل_العالمي() {
    static سجل_موزع سجل("main");
    return سجل;
}

/**
 * تسجيل Actor بالاسم
 */
bool سجّل_عالمي(const std::string& اسم, const std::string& معرف) {
    return السجل_العالمي().سجّل(اسم, معرف);
}

/**
 * البحث عن Actor بالاسم
 */
const معلومات_مسجلة* ابحث_عالمي(const std::string& اسم) {
    return السجل_العالمي().ابحث(اسم);
}

} // namespace actor
} // namespace runtime
} // namespace sad

// ==========================================================
// 📌 واجهة C
// ==========================================================

extern "C" {

using namespace sad::runtime::actor;

// السجل المحلي
void* sad_registry_local_create() {
    return new سجل_محلي();
}

void sad_registry_local_destroy(void* reg) {
    delete static_cast<سجل_محلي*>(reg);
}

int sad_registry_local_register(void* reg, const char* name, const char* id) {
    return static_cast<سجل_محلي*>(reg)->سجّل(name, id) ? 1 : 0;
}

int sad_registry_local_unregister(void* reg, const char* name) {
    return static_cast<سجل_محلي*>(reg)->ألغِ_تسجيل(name) ? 1 : 0;
}

const char* sad_registry_local_lookup(void* reg, const char* name) {
    auto* معلومات = static_cast<سجل_محلي*>(reg)->ابحث(name);
    if (!معلومات) return nullptr;
    static std::string id;
    id = معلومات->المعرف;
    return id.c_str();
}

int sad_registry_local_count(void* reg) {
    return static_cast<int>(static_cast<سجل_محلي*>(reg)->عدد());
}

// السجل الموزع
void* sad_registry_distributed_create(const char* node_name) {
    return new سجل_موزع(node_name);
}

void sad_registry_distributed_destroy(void* reg) {
    delete static_cast<سجل_موزع*>(reg);
}

void sad_registry_add_node(void* reg, const char* name, const char* address) {
    static_cast<سجل_موزع*>(reg)->أضف_عقدة(name, address);
}

// السجل العالمي
int sad_global_register(const char* name, const char* id) {
    return سجّل_عالمي(name, id) ? 1 : 0;
}

const char* sad_global_lookup(const char* name) {
    auto* معلومات = ابحث_عالمي(name);
    if (!معلومات) return nullptr;
    static std::string id;
    id = معلومات->المعرف;
    return id.c_str();
}

} // extern "C"
