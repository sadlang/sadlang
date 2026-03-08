/**
 * ==========================================================
 * ملف: preferences.cpp
 * الوصف: تتبع تفضيلات المبرمج
 * المهمة: T370 - رفيق ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * متتبع التفضيلات يراقب اختياراتك ويتذكرها:
 * - الاقتراحات التي قبلتها
 * - الاقتراحات التي رفضتها
 * - إعداداتك المفضلة
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <algorithm>

namespace sad {
namespace companion {

// ==========================================================
// 📌 أنواع التفضيلات
// ==========================================================

/**
 * نوع التفضيل
 */
enum class نوع_تفضيل {
    إكمال,          // اقتراحات الإكمال
    تنسيق,          // أسلوب التنسيق
    تسمية,          // أسماء المتغيرات
    بنية,           // هيكل الكود
    أداة,           // الأدوات المستخدمة
    اختصار          // اختصارات لوحة المفاتيح
};

/**
 * تفضيل مسجل
 */
struct تفضيل {
    نوع_تفضيل النوع;
    std::string المفتاح;
    std::string القيمة;
    int التكرارات = 1;
    std::chrono::system_clock::time_point آخر_استخدام;
    
    تفضيل() {
        آخر_استخدام = std::chrono::system_clock::now();
    }
};

// ==========================================================
// 📌 سجل القرارات
// ==========================================================

/**
 * قرار المستخدم
 */
enum class قرار {
    قبول,
    رفض,
    تجاهل
};

/**
 * سجل قرار
 */
struct سجل_قرار {
    std::string الاقتراح;
    قرار القرار;
    std::string السياق;
    std::chrono::system_clock::time_point الوقت;
};

// ==========================================================
// 📌 متتبع التفضيلات
// ==========================================================

/**
 * متتبع تفضيلات المبرمج
 */
class متتبع_التفضيلات {
private:
    std::map<std::string, تفضيل> تفضيلات_;
    std::vector<سجل_قرار> سجل_;
    
    // إحصائيات
    int اقتراحات_مقبولة_ = 0;
    int اقتراحات_مرفوضة_ = 0;
    int اقتراحات_متجاهلة_ = 0;
    
public:
    /**
     * تسجيل تفضيل جديد
     */
    void سجّل_تفضيل(نوع_تفضيل النوع, 
                     const std::string& مفتاح,
                     const std::string& قيمة) {
        std::string معرف = اسم_النوع(النوع) + ":" + مفتاح;
        
        auto it = تفضيلات_.find(معرف);
        if (it != تفضيلات_.end()) {
            it->second.القيمة = قيمة;
            it->second.التكرارات++;
            it->second.آخر_استخدام = std::chrono::system_clock::now();
        } else {
            تفضيل ت;
            ت.النوع = النوع;
            ت.المفتاح = مفتاح;
            ت.القيمة = قيمة;
            تفضيلات_[معرف] = ت;
        }
    }
    
    /**
     * الحصول على تفضيل
     */
    std::string احصل_تفضيل(نوع_تفضيل النوع, 
                           const std::string& مفتاح,
                           const std::string& افتراضي = "") const {
        std::string معرف = اسم_النوع(النوع) + ":" + مفتاح;
        
        auto it = تفضيلات_.find(معرف);
        if (it != تفضيلات_.end()) {
            return it->second.القيمة;
        }
        return افتراضي;
    }
    
    /**
     * تسجيل قرار اقتراح
     */
    void سجّل_قرار(const std::string& اقتراح, قرار ق,
                   const std::string& سياق = "") {
        سجل_قرار س;
        س.الاقتراح = اقتراح;
        س.القرار = ق;
        س.السياق = سياق;
        س.الوقت = std::chrono::system_clock::now();
        
        سجل_.push_back(س);
        
        // تحديث الإحصائيات
        switch (ق) {
            case قرار::قبول: اقتراحات_مقبولة_++; break;
            case قرار::رفض: اقتراحات_مرفوضة_++; break;
            case قرار::تجاهل: اقتراحات_متجاهلة_++; break;
        }
    }
    
    /**
     * نسبة القبول
     */
    double نسبة_القبول() const {
        int إجمالي = اقتراحات_مقبولة_ + اقتراحات_مرفوضة_;
        if (إجمالي == 0) return 0.0;
        return (double)اقتراحات_مقبولة_ / إجمالي;
    }
    
    /**
     * التفضيلات الأكثر استخداماً
     */
    std::vector<تفضيل> الأكثر_استخداماً(int حد = 10) const {
        std::vector<تفضيل> نتائج;
        
        for (const auto& [_, ت] : تفضيلات_) {
            نتائج.push_back(ت);
        }
        
        // ترتيب حسب التكرارات
        std::sort(نتائج.begin(), نتائج.end(),
            [](const تفضيل& أ, const تفضيل& ب) {
                return أ.التكرارات > ب.التكرارات;
            });
        
        if (نتائج.size() > حد) {
            نتائج.resize(حد);
        }
        
        return نتائج;
    }
    
    /**
     * تقرير التفضيلات
     */
    std::string تقرير() const {
        std::string ت;
        
        ت += "═══════════════════════════════════════════════════\n";
        ت += "⚙️ تفضيلاتك\n";
        ت += "═══════════════════════════════════════════════════\n\n";
        
        ت += "📊 الإحصائيات:\n";
        ت += "   اقتراحات مقبولة: " + std::to_string(اقتراحات_مقبولة_) + "\n";
        ت += "   اقتراحات مرفوضة: " + std::to_string(اقتراحات_مرفوضة_) + "\n";
        ت += "   نسبة القبول: " + std::to_string((int)(نسبة_القبول() * 100)) + "%\n\n";
        
        ت += "🔝 التفضيلات الأكثر استخداماً:\n";
        auto أفضل = الأكثر_استخداماً(5);
        for (const auto& تف : أفضل) {
            ت += "   • " + تف.المفتاح + " = " + تف.القيمة + 
                 " (" + std::to_string(تف.التكرارات) + " مرة)\n";
        }
        
        return ت;
    }
    
private:
    std::string اسم_النوع(نوع_تفضيل ن) const {
        switch (ن) {
            case نوع_تفضيل::إكمال: return "completion";
            case نوع_تفضيل::تنسيق: return "format";
            case نوع_تفضيل::تسمية: return "naming";
            case نوع_تفضيل::بنية: return "structure";
            case نوع_تفضيل::أداة: return "tool";
            case نوع_تفضيل::اختصار: return "shortcut";
            default: return "other";
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

void* sad_preferences_create() {
    return new متتبع_التفضيلات();
}

void sad_preferences_destroy(void* prefs) {
    delete static_cast<متتبع_التفضيلات*>(prefs);
}

void sad_preferences_set(void* prefs, int type, const char* key, const char* value) {
    static_cast<متتبع_التفضيلات*>(prefs)->سجّل_تفضيل(
        static_cast<نوع_تفضيل>(type), key, value);
}

const char* sad_preferences_get(void* prefs, int type, const char* key, const char* def) {
    static std::string قيمة;
    قيمة = static_cast<متتبع_التفضيلات*>(prefs)->احصل_تفضيل(
        static_cast<نوع_تفضيل>(type), key, def);
    return قيمة.c_str();
}

void sad_preferences_record_decision(void* prefs, const char* suggestion, int decision) {
    static_cast<متتبع_التفضيلات*>(prefs)->سجّل_قرار(
        suggestion, static_cast<قرار>(decision));
}

double sad_preferences_acceptance_rate(void* prefs) {
    return static_cast<متتبع_التفضيلات*>(prefs)->نسبة_القبول();
}

const char* sad_preferences_report(void* prefs) {
    static std::string تقرير;
    تقرير = static_cast<متتبع_التفضيلات*>(prefs)->تقرير();
    return تقرير.c_str();
}

} // extern "C"
