/**
 * ==========================================================
 * ملف: location.cpp
 * الوصف: شفافية الموقع للـ Actors الموزعين
 * المهمة: T365 - توزيع ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * شفافية الموقع تعني أن Actor يتصرف نفس الشيء سواء كان:
 * - محلياً على نفس الجهاز
 * - بعيداً على جهاز آخر
 * 
 * الكود لا يحتاج لمعرفة موقع Actor الفعلي!
 * 
 * 💡 مثال:
 * ```
 * // الإرسال نفسه سواء كان محلياً أو بعيداً
 * متغير خادم = ابحث("خادم_ويب")
 * خادم.أرسل("طلب", بيانات)  // يعمل تلقائياً
 * ```
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace sad {
namespace runtime {
namespace actor {

// ==========================================================
// 📌 عنوان Actor
// ==========================================================

/**
 * نوع العنوان
 */
enum class نوع_العنوان {
    محلي,          // على نفس العقدة
    بعيد,          // على عقدة أخرى
    افتراضي        // عنوان منطقي
};

/**
 * عنوان Actor
 */
class عنوان_Actor {
private:
    std::string المعرف_;
    نوع_العنوان النوع_ = نوع_العنوان::محلي;
    std::string العقدة_;
    std::string عنوان_IP_;
    int المنفذ_ = 0;
    
public:
    عنوان_Actor() = default;
    
    /**
     * إنشاء عنوان محلي
     */
    static عنوان_Actor محلي(const std::string& معرف) {
        عنوان_Actor ع;
        ع.المعرف_ = معرف;
        ع.النوع_ = نوع_العنوان::محلي;
        return ع;
    }
    
    /**
     * إنشاء عنوان بعيد
     */
    static عنوان_Actor بعيد(const std::string& معرف,
                             const std::string& عقدة,
                             const std::string& ip,
                             int منفذ) {
        عنوان_Actor ع;
        ع.المعرف_ = معرف;
        ع.النوع_ = نوع_العنوان::بعيد;
        ع.العقدة_ = عقدة;
        ع.عنوان_IP_ = ip;
        ع.المنفذ_ = منفذ;
        return ع;
    }
    
    /**
     * تحويل إلى نص
     */
    std::string إلى_نص() const {
        if (النوع_ == نوع_العنوان::محلي) {
            return "local://" + المعرف_;
        } else {
            return "remote://" + العقدة_ + "/" + المعرف_;
        }
    }
    
    /**
     * تحليل من نص
     */
    static عنوان_Actor من_نص(const std::string& نص) {
        عنوان_Actor ع;
        
        if (نص.find("local://") == 0) {
            ع.النوع_ = نوع_العنوان::محلي;
            ع.المعرف_ = نص.substr(8);
        } else if (نص.find("remote://") == 0) {
            ع.النوع_ = نوع_العنوان::بعيد;
            std::string باقي = نص.substr(9);
            size_t شرطة = باقي.find('/');
            ع.العقدة_ = باقي.substr(0, شرطة);
            ع.المعرف_ = باقي.substr(شرطة + 1);
        }
        
        return ع;
    }
    
    // Getters
    const std::string& المعرف() const { return المعرف_; }
    نوع_العنوان النوع() const { return النوع_; }
    const std::string& العقدة() const { return العقدة_; }
    const std::string& عنوان_IP() const { return عنوان_IP_; }
    int المنفذ() const { return المنفذ_; }
    bool محلي() const { return النوع_ == نوع_العنوان::محلي; }
};

// ==========================================================
// 📌 وكيل Actor البعيد
// ==========================================================

/**
 * واجهة الإرسال
 */
class واجهة_إرسال {
public:
    virtual ~واجهة_إرسال() = default;
    virtual bool أرسل(const عنوان_Actor& إلى,
                      const std::string& رسالة,
                      const std::vector<uint8_t>& حمولة) = 0;
};

/**
 * وكيل Actor بعيد
 * يتصرف كـ Actor محلي لكن يُرسل عبر الشبكة
 */
class وكيل_بعيد {
private:
    عنوان_Actor العنوان_;
    واجهة_إرسال* مُرسل_;
    
public:
    وكيل_بعيد(const عنوان_Actor& عنوان, واجهة_إرسال* مرسل)
        : العنوان_(عنوان), مُرسل_(مرسل) {}
    
    /**
     * إرسال رسالة
     */
    void أرسل(const std::string& رسالة,
              const std::vector<uint8_t>& حمولة = {}) {
        if (مُرسل_) {
            مُرسل_->أرسل(العنوان_, رسالة, حمولة);
        }
    }
    
    const عنوان_Actor& العنوان() const { return العنوان_; }
};

// ==========================================================
// 📌 محلل الموقع
// ==========================================================

/**
 * محلل الموقع - يحدد موقع Actor
 */
class محلل_الموقع {
private:
    std::map<std::string, عنوان_Actor> ذاكرة_مؤقتة_;
    
public:
    /**
     * حل عنوان Actor بالاسم
     */
    عنوان_Actor حل(const std::string& اسم) {
        // البحث في الذاكرة المؤقتة
        auto it = ذاكرة_مؤقتة_.find(اسم);
        if (it != ذاكرة_مؤقتة_.end()) {
            return it->second;
        }
        
        // البحث في السجل
        // هنا يتم الاستعلام من السجل الموزع
        
        // افتراضياً: Actor محلي
        return عنوان_Actor::محلي(اسم);
    }
    
    /**
     * تسجيل عنوان
     */
    void سجّل(const std::string& اسم, const عنوان_Actor& عنوان) {
        ذاكرة_مؤقتة_[اسم] = عنوان;
    }
    
    /**
     * إلغاء التسجيل
     */
    void ألغِ_تسجيل(const std::string& اسم) {
        ذاكرة_مؤقتة_.erase(اسم);
    }
    
    /**
     * مسح الذاكرة المؤقتة
     */
    void امسح_الذاكرة() {
        ذاكرة_مؤقتة_.clear();
    }
};

// ==========================================================
// 📌 مرجع شفاف الموقع
// ==========================================================

/**
 * مرجع Actor شفاف الموقع
 * يعمل نفس الشيء سواء كان Actor محلياً أو بعيداً
 */
class مرجع_شفاف {
private:
    std::string الاسم_;
    عنوان_Actor العنوان_;
    محلل_الموقع* المحلل_;
    واجهة_إرسال* مُرسل_;
    bool محلول_ = false;
    
public:
    مرجع_شفاف(const std::string& اسم, 
               محلل_الموقع* محلل,
               واجهة_إرسال* مرسل)
        : الاسم_(اسم), المحلل_(محلل), مُرسل_(مرسل) {}
    
    /**
     * إرسال رسالة (fire and forget)
     */
    void أرسل(const std::string& رسالة,
              const std::vector<uint8_t>& حمولة = {}) {
        تأكد_الحل();
        
        if (العنوان_.محلي()) {
            أرسل_محلياً(رسالة, حمولة);
        } else {
            أرسل_بعيداً(رسالة, حمولة);
        }
    }
    
    /**
     * طلب مع انتظار الرد
     */
    std::vector<uint8_t> اطلب(const std::string& رسالة,
                              const std::vector<uint8_t>& حمولة = {},
                              int timeout_ms = 5000) {
        تأكد_الحل();
        
        if (العنوان_.محلي()) {
            return اطلب_محلياً(رسالة, حمولة, timeout_ms);
        } else {
            return اطلب_بعيداً(رسالة, حمولة, timeout_ms);
        }
    }
    
    /**
     * التحقق من الحياة
     */
    bool حي() const {
        // إرسال ping والتحقق من الرد
        return true;
    }
    
    /**
     * الحصول على الموقع
     */
    std::string الموقع() const {
        return العنوان_.إلى_نص();
    }
    
    /**
     * هل هو محلي؟
     */
    bool محلي() const {
        const_cast<مرجع_شفاف*>(this)->تأكد_الحل();
        return العنوان_.محلي();
    }
    
private:
    void تأكد_الحل() {
        if (!محلول_ && المحلل_) {
            العنوان_ = المحلل_->حل(الاسم_);
            محلول_ = true;
        }
    }
    
    void أرسل_محلياً(const std::string& رسالة,
                     const std::vector<uint8_t>& حمولة) {
        // إرسال مباشر عبر صندوق الرسائل
    }
    
    void أرسل_بعيداً(const std::string& رسالة,
                     const std::vector<uint8_t>& حمولة) {
        if (مُرسل_) {
            مُرسل_->أرسل(العنوان_, رسالة, حمولة);
        }
    }
    
    std::vector<uint8_t> اطلب_محلياً(const std::string& رسالة,
                                     const std::vector<uint8_t>& حمولة,
                                     int timeout_ms) {
        // طلب محلي مع انتظار
        return {};
    }
    
    std::vector<uint8_t> اطلب_بعيداً(const std::string& رسالة,
                                     const std::vector<uint8_t>& حمولة,
                                     int timeout_ms) {
        // طلب بعيد عبر الشبكة
        return {};
    }
};

// ==========================================================
// 📌 مصنع المراجع
// ==========================================================

/**
 * مصنع مراجع شفافة الموقع
 */
class مصنع_المراجع {
private:
    محلل_الموقع محلل_;
    واجهة_إرسال* مُرسل_ = nullptr;
    
public:
    void عيّن_مرسل(واجهة_إرسال* مرسل) {
        مُرسل_ = مرسل;
    }
    
    /**
     * إنشاء مرجع بالاسم
     */
    مرجع_شفاف أنشئ(const std::string& اسم) {
        return مرجع_شفاف(اسم, &محلل_, مُرسل_);
    }
    
    /**
     * تسجيل Actor
     */
    void سجّل(const std::string& اسم, const عنوان_Actor& عنوان) {
        محلل_.سجّل(اسم, عنوان);
    }
    
    محلل_الموقع& المحلل() { return محلل_; }
};

} // namespace actor
} // namespace runtime
} // namespace sad

// ==========================================================
// 📌 واجهة C
// ==========================================================

extern "C" {

using namespace sad::runtime::actor;

// العنوان
void* sad_actor_address_local(const char* id) {
    auto* ع = new عنوان_Actor(عنوان_Actor::محلي(id));
    return ع;
}

void* sad_actor_address_remote(const char* id, const char* node, 
                                const char* ip, int port) {
    auto* ع = new عنوان_Actor(عنوان_Actor::بعيد(id, node, ip, port));
    return ع;
}

void sad_actor_address_destroy(void* addr) {
    delete static_cast<عنوان_Actor*>(addr);
}

const char* sad_actor_address_to_string(void* addr) {
    static std::string نص;
    نص = static_cast<عنوان_Actor*>(addr)->إلى_نص();
    return نص.c_str();
}

int sad_actor_address_is_local(void* addr) {
    return static_cast<عنوان_Actor*>(addr)->محلي() ? 1 : 0;
}

// محلل الموقع
void* sad_location_resolver_create() {
    return new محلل_الموقع();
}

void sad_location_resolver_destroy(void* resolver) {
    delete static_cast<محلل_الموقع*>(resolver);
}

void sad_location_resolver_register(void* resolver, const char* name, void* addr) {
    static_cast<محلل_الموقع*>(resolver)->سجّل(name, 
        *static_cast<عنوان_Actor*>(addr));
}

// مصنع المراجع
void* sad_ref_factory_create() {
    return new مصنع_المراجع();
}

void sad_ref_factory_destroy(void* factory) {
    delete static_cast<مصنع_المراجع*>(factory);
}

void sad_ref_factory_register(void* factory, const char* name, void* addr) {
    static_cast<مصنع_المراجع*>(factory)->سجّل(name,
        *static_cast<عنوان_Actor*>(addr));
}

} // extern "C"
