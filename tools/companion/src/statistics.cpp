/**
 * ==========================================================
 * ملف: statistics.cpp
 * الوصف: إحصائيات البرمجة الشخصية
 * المهمة: T372 - رفيق ص
 * ==========================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>

namespace sad {
namespace companion {

/**
 * إحصائيات يومية
 */
struct إحصائيات_يومية {
    int سطور_مكتوبة = 0;
    int سطور_محذوفة = 0;
    int أخطاء_مصححة = 0;
    int دوال_منشأة = 0;
    int ملفات_معدلة = 0;
    double دقائق_برمجة = 0;
    std::map<std::string, int> لغات_مستخدمة;
};

/**
 * متتبع الإحصائيات
 */
class متتبع_الإحصائيات {
private:
    std::map<std::string, إحصائيات_يومية> بالتاريخ_;  // YYYY-MM-DD
    إحصائيات_يومية اليوم_;
    std::chrono::system_clock::time_point آخر_نشاط_;
    
public:
    متتبع_الإحصائيات() {
        آخر_نشاط_ = std::chrono::system_clock::now();
    }
    
    /**
     * تسجيل سطور مكتوبة
     */
    void سجّل_كتابة(int سطور) {
        اليوم_.سطور_مكتوبة += سطور;
        حدّث_الوقت();
    }
    
    /**
     * تسجيل سطور محذوفة
     */
    void سجّل_حذف(int سطور) {
        اليوم_.سطور_محذوفة += سطور;
    }
    
    /**
     * تسجيل خطأ مصحح
     */
    void سجّل_تصحيح() {
        اليوم_.أخطاء_مصححة++;
    }
    
    /**
     * تسجيل دالة جديدة
     */
    void سجّل_دالة() {
        اليوم_.دوال_منشأة++;
    }
    
    /**
     * تسجيل استخدام لغة
     */
    void سجّل_لغة(const std::string& لغة) {
        اليوم_.لغات_مستخدمة[لغة]++;
    }
    
    /**
     * حفظ إحصائيات اليوم
     */
    void احفظ_اليوم() {
        std::string تاريخ = تاريخ_اليوم();
        بالتاريخ_[تاريخ] = اليوم_;
        اليوم_ = إحصائيات_يومية();
    }
    
    /**
     * تقرير اليوم
     */
    std::string تقرير_اليوم() const {
        std::string ت;
        ت += "═══════════════════════════════════════════════════\n";
        ت += "📊 إحصائيات اليوم\n";
        ت += "═══════════════════════════════════════════════════\n\n";
        
        ت += "✍️ سطور مكتوبة: " + std::to_string(اليوم_.سطور_مكتوبة) + "\n";
        ت += "🗑️ سطور محذوفة: " + std::to_string(اليوم_.سطور_محذوفة) + "\n";
        ت += "🐛 أخطاء مصححة: " + std::to_string(اليوم_.أخطاء_مصححة) + "\n";
        ت += "🔧 دوال جديدة: " + std::to_string(اليوم_.دوال_منشأة) + "\n";
        ت += "⏱️ وقت البرمجة: " + std::to_string((int)اليوم_.دقائق_برمجة) + " دقيقة\n";
        
        // صافي الإنتاج
        int صافي = اليوم_.سطور_مكتوبة - اليوم_.سطور_محذوفة;
        ت += "\n📈 صافي الإنتاج: ";
        if (صافي >= 0) {
            ت += "+" + std::to_string(صافي) + " سطر\n";
        } else {
            ت += std::to_string(صافي) + " سطر\n";
        }
        
        return ت;
    }
    
    /**
     * تقرير الأسبوع
     */
    std::string تقرير_الأسبوع() const {
        std::string ت;
        ت += "═══════════════════════════════════════════════════\n";
        ت += "📊 إحصائيات الأسبوع\n";
        ت += "═══════════════════════════════════════════════════\n\n";
        
        // جمع آخر 7 أيام
        int مجموع_سطور = 0;
        int مجموع_أخطاء = 0;
        
        for (const auto& [تاريخ, إحصائيات] : بالتاريخ_) {
            مجموع_سطور += إحصائيات.سطور_مكتوبة - إحصائيات.سطور_محذوفة;
            مجموع_أخطاء += إحصائيات.أخطاء_مصححة;
        }
        
        ت += "📝 إجمالي السطور: " + std::to_string(مجموع_سطور) + "\n";
        ت += "🐛 إجمالي الأخطاء: " + std::to_string(مجموع_أخطاء) + "\n";
        
        return ت;
    }
    
private:
    void حدّث_الوقت() {
        auto الآن = std::chrono::system_clock::now();
        auto فرق = std::chrono::duration_cast<std::chrono::minutes>(
            الآن - آخر_نشاط_).count();
        
        if (فرق < 30) {  // أقل من 30 دقيقة
            اليوم_.دقائق_برمجة += فرق;
        }
        
        آخر_نشاط_ = الآن;
    }
    
    std::string تاريخ_اليوم() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        char buffer[11];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", std::localtime(&time));
        return buffer;
    }
};

} // namespace companion
} // namespace sad

extern "C" {
using namespace sad::companion;

void* sad_statistics_create() {
    return new متتبع_الإحصائيات();
}

void sad_statistics_destroy(void* stats) {
    delete static_cast<متتبع_الإحصائيات*>(stats);
}

void sad_statistics_record_write(void* stats, int lines) {
    static_cast<متتبع_الإحصائيات*>(stats)->سجّل_كتابة(lines);
}

void sad_statistics_record_delete(void* stats, int lines) {
    static_cast<متتبع_الإحصائيات*>(stats)->سجّل_حذف(lines);
}

void sad_statistics_record_fix(void* stats) {
    static_cast<متتبع_الإحصائيات*>(stats)->سجّل_تصحيح();
}

const char* sad_statistics_today_report(void* stats) {
    static std::string تقرير;
    تقرير = static_cast<متتبع_الإحصائيات*>(stats)->تقرير_اليوم();
    return تقرير.c_str();
}
}
