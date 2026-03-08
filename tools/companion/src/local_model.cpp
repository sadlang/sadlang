/**
 * ==========================================================
 * ملف: local_model.cpp
 * الوصف: نموذج التعلم المحلي (يعمل بدون إنترنت)
 * المهمة: T369 - رفيق ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * النموذج المحلي يتعلم من كودك بدون إرسال بيانات:
 * - يخزن الأنماط على جهازك
 * - يتحسن مع كل استخدام
 * - خصوصيتك محمية 100%
 * 
 * 💡 مثال:
 * ```
 * // كلما كتبت كود، النموذج يتعلم:
 * // - التراكيب التي تستخدمها
 * // - الأخطاء التي تصححها
 * // - الأنماط المفضلة لديك
 * ```
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <fstream>
#include <cmath>
#include <algorithm>

namespace sad {
namespace companion {

// ==========================================================
// 📌 نقطة البيانات
// ==========================================================

/**
 * نوع نقطة التعلم
 */
enum class نوع_نقطة {
    إكمال,         // اقتراح إكمال
    تصحيح,         // تصحيح خطأ
    إعادة_تسمية,   // تغيير اسم
    إعادة_هيكلة,   // تغيير بنية الكود
    نمط            // نمط متكرر
};

/**
 * نقطة بيانات للتعلم
 */
struct نقطة_تعلم {
    نوع_نقطة النوع;
    std::string السياق;      // ما قبل النقطة
    std::string المدخل;      // ما كتبه المستخدم
    std::string المخرج;      // النتيجة
    double الوزن = 1.0;      // أهمية النقطة
    int التكرارات = 1;
};

// ==========================================================
// 📌 خزانة النماذج
// ==========================================================

/**
 * خزانة أنماط محلية
 */
class خزانة_أنماط {
private:
    std::map<std::string, std::vector<std::string>> أنماط_;  // سياق -> اقتراحات
    std::map<std::string, int> تكرارات_;
    
public:
    /**
     * إضافة نمط
     */
    void أضف(const std::string& سياق, const std::string& اقتراح) {
        أنماط_[سياق].push_back(اقتراح);
        تكرارات_[سياق + "|" + اقتراح]++;
    }
    
    /**
     * البحث عن اقتراحات
     */
    std::vector<std::string> ابحث(const std::string& سياق, int حد = 5) {
        std::vector<std::string> نتائج;
        
        // البحث المطابق
        auto it = أنماط_.find(سياق);
        if (it != أنماط_.end()) {
            نتائج = it->second;
        }
        
        // البحث الجزئي
        if (نتائج.empty()) {
            for (const auto& [ن, اقتراحات] : أنماط_) {
                if (ن.find(سياق) != std::string::npos ||
                    سياق.find(ن) != std::string::npos) {
                    for (const auto& اقتراح : اقتراحات) {
                        نتائج.push_back(اقتراح);
                        if (نتائج.size() >= حد) break;
                    }
                }
            }
        }
        
        // ترتيب حسب التكرار
        // ...
        
        if (نتائج.size() > حد) {
            نتائج.resize(حد);
        }
        
        return نتائج;
    }
    
    /**
     * حجم الخزانة
     */
    size_t حجم() const { return أنماط_.size(); }
    
    /**
     * مسح الخزانة
     */
    void امسح() {
        أنماط_.clear();
        تكرارات_.clear();
    }
};

// ==========================================================
// 📌 النموذج المحلي
// ==========================================================

/**
 * نموذج التعلم المحلي
 */
class نموذج_محلي {
private:
    خزانة_أنماط خزانة_;
    std::vector<نقطة_تعلم> بيانات_;
    std::string مسار_الحفظ_;
    bool تم_التعديل_ = false;
    
    // إعدادات
    size_t حد_البيانات_ = 10000;
    double عتبة_الثقة_ = 0.3;
    
public:
    explicit نموذج_محلي(const std::string& مسار = "")
        : مسار_الحفظ_(مسار) {
        if (!مسار.empty()) {
            حمّل();
        }
    }
    
    ~نموذج_محلي() {
        if (تم_التعديل_ && !مسار_الحفظ_.empty()) {
            احفظ();
        }
    }
    
    /**
     * تعلم من نقطة جديدة
     */
    void تعلّم(const نقطة_تعلم& نقطة) {
        // إضافة للبيانات
        بيانات_.push_back(نقطة);
        
        // إضافة للخزانة
        خزانة_.أضف(نقطة.السياق, نقطة.المخرج);
        
        // تقليم البيانات إذا تجاوزت الحد
        if (بيانات_.size() > حد_البيانات_) {
            قلّم_البيانات();
        }
        
        تم_التعديل_ = true;
    }
    
    /**
     * الاقتراح بناءً على السياق
     */
    std::vector<std::pair<std::string, double>> اقترح(
            const std::string& سياق, int حد = 5) {
        std::vector<std::pair<std::string, double>> نتائج;
        
        // البحث في الخزانة
        auto اقتراحات = خزانة_.ابحث(سياق, حد);
        
        for (const auto& اقتراح : اقتراحات) {
            double ثقة = احسب_الثقة(سياق, اقتراح);
            if (ثقة >= عتبة_الثقة_) {
                نتائج.push_back({اقتراح, ثقة});
            }
        }
        
        // ترتيب حسب الثقة
        std::sort(نتائج.begin(), نتائج.end(),
            [](const auto& أ, const auto& ب) {
                return أ.second > ب.second;
            });
        
        return نتائج;
    }
    
    /**
     * حفظ النموذج
     */
    bool احفظ() {
        if (مسار_الحفظ_.empty()) return false;
        
        std::ofstream ملف(مسار_الحفظ_, std::ios::binary);
        if (!ملف) return false;
        
        // حفظ البيانات
        size_t حجم = بيانات_.size();
        ملف.write(reinterpret_cast<char*>(&حجم), sizeof(حجم));
        
        for (const auto& نقطة : بيانات_) {
            // حفظ كل نقطة
            size_t طول = نقطة.السياق.size();
            ملف.write(reinterpret_cast<char*>(&طول), sizeof(طول));
            ملف.write(نقطة.السياق.data(), طول);
            
            طول = نقطة.المخرج.size();
            ملف.write(reinterpret_cast<char*>(&طول), sizeof(طول));
            ملف.write(نقطة.المخرج.data(), طول);
        }
        
        تم_التعديل_ = false;
        return true;
    }
    
    /**
     * تحميل النموذج
     */
    bool حمّل() {
        if (مسار_الحفظ_.empty()) return false;
        
        std::ifstream ملف(مسار_الحفظ_, std::ios::binary);
        if (!ملف) return false;
        
        // تحميل البيانات
        size_t حجم;
        ملف.read(reinterpret_cast<char*>(&حجم), sizeof(حجم));
        
        for (size_t i = 0; i < حجم; i++) {
            نقطة_تعلم نقطة;
            
            size_t طول;
            ملف.read(reinterpret_cast<char*>(&طول), sizeof(طول));
            نقطة.السياق.resize(طول);
            ملف.read(&نقطة.السياق[0], طول);
            
            ملف.read(reinterpret_cast<char*>(&طول), sizeof(طول));
            نقطة.المخرج.resize(طول);
            ملف.read(&نقطة.المخرج[0], طول);
            
            بيانات_.push_back(نقطة);
            خزانة_.أضف(نقطة.السياق, نقطة.المخرج);
        }
        
        return true;
    }
    
    /**
     * إحصائيات النموذج
     */
    std::string إحصائيات() const {
        std::string ت;
        ت += "📊 إحصائيات النموذج المحلي:\n";
        ت += "   نقاط التعلم: " + std::to_string(بيانات_.size()) + "\n";
        ت += "   الأنماط: " + std::to_string(خزانة_.حجم()) + "\n";
        return ت;
    }
    
    /**
     * مسح النموذج
     */
    void امسح() {
        بيانات_.clear();
        خزانة_.امسح();
        تم_التعديل_ = true;
    }
    
private:
    /**
     * حساب درجة الثقة
     */
    double احسب_الثقة(const std::string& سياق, const std::string& اقتراح) {
        // حساب بسيط للثقة
        int تطابق = 0;
        
        for (const auto& نقطة : بيانات_) {
            if (نقطة.السياق == سياق && نقطة.المخرج == اقتراح) {
                تطابق += نقطة.التكرارات;
            }
        }
        
        // تطبيع بين 0 و 1
        return std::min(1.0, تطابق / 10.0);
    }
    
    /**
     * تقليم البيانات القديمة
     */
    void قلّم_البيانات() {
        // حذف النصف الأقدم
        if (بيانات_.size() > حد_البيانات_ / 2) {
            بيانات_.erase(بيانات_.begin(), 
                          بيانات_.begin() + بيانات_.size() / 2);
        }
    }
};

} // namespace companion
} // namespace sad

// ==========================================================
// 📌 واجهة C
// ==========================================================

extern "C" {

using namespace sad::companion;

void* sad_local_model_create(const char* path) {
    return new نموذج_محلي(path ? path : "");
}

void sad_local_model_destroy(void* model) {
    delete static_cast<نموذج_محلي*>(model);
}

void sad_local_model_learn(void* model, const char* context, const char* output) {
    نقطة_تعلم نقطة;
    نقطة.السياق = context;
    نقطة.المخرج = output;
    static_cast<نموذج_محلي*>(model)->تعلّم(نقطة);
}

int sad_local_model_save(void* model) {
    return static_cast<نموذج_محلي*>(model)->احفظ() ? 1 : 0;
}

int sad_local_model_load(void* model) {
    return static_cast<نموذج_محلي*>(model)->حمّل() ? 1 : 0;
}

void sad_local_model_clear(void* model) {
    static_cast<نموذج_محلي*>(model)->امسح();
}

const char* sad_local_model_stats(void* model) {
    static std::string إحصائيات;
    إحصائيات = static_cast<نموذج_محلي*>(model)->إحصائيات();
    return إحصائيات.c_str();
}

} // extern "C"
