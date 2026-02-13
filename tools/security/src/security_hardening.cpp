/**
 * ==========================================================
 * ملف: security_hardening.cpp
 * الوصف: تقوية الأمان
 * المهمة: T382 - مراجعة الأمان
 * ==========================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <map>

namespace sad {
namespace security {

/**
 * شدة الثغرة
 */
enum class شدة_ثغرة {
    حرجة,    // Critical
    عالية,   // High
    متوسطة,  // Medium
    منخفضة   // Low
};

/**
 * نوع الثغرة
 */
enum class نوع_ثغرة {
    حقن_كود,           // Code injection
    تجاوز_المخزن,       // Buffer overflow
    تسرب_ذاكرة,        // Memory leak
    سباق_بيانات,       // Data race
    مدخلات_غير_آمنة,    // Unsafe input
    اعتماديات_قديمة,    // Outdated dependencies
    أسرار_مكشوفة       // Exposed secrets
};

/**
 * ثغرة أمنية
 */
struct ثغرة {
    نوع_ثغرة نوع;
    شدة_ثغرة شدة;
    std::string ملف;
    int سطر;
    std::string وصف;
    std::string حل;
};

/**
 * فاحص الأمان
 */
class فاحص_الأمان {
public:
    /**
     * فحص ملف
     */
    std::vector<ثغرة> افحص(const std::string& مسار, const std::string& محتوى) {
        std::vector<ثغرة> ثغرات;
        
        // فحص الأسرار المكشوفة
        auto أسرار = افحص_الأسرار(مسار, محتوى);
        ثغرات.insert(ثغرات.end(), أسرار.begin(), أسرار.end());
        
        // فحص المدخلات غير الآمنة
        auto مدخلات = افحص_المدخلات(مسار, محتوى);
        ثغرات.insert(ثغرات.end(), مدخلات.begin(), مدخلات.end());
        
        // فحص التجاوزات
        auto تجاوزات = افحص_التجاوزات(مسار, محتوى);
        ثغرات.insert(ثغرات.end(), تجاوزات.begin(), تجاوزات.end());
        
        return ثغرات;
    }
    
    /**
     * تقرير الأمان
     */
    std::string تقرير(const std::vector<ثغرة>& ثغرات) {
        std::string ت;
        ت += "═══════════════════════════════════════════════════\n";
        ت += "🔒 تقرير الأمان\n";
        ت += "═══════════════════════════════════════════════════\n\n";
        
        if (ثغرات.empty()) {
            ت += "✅ لم يُكتشف أي ثغرات أمنية!\n";
            return ت;
        }
        
        // تصنيف حسب الشدة
        int حرجة = 0, عالية = 0, متوسطة = 0, منخفضة = 0;
        for (const auto& ث : ثغرات) {
            switch (ث.شدة) {
                case شدة_ثغرة::حرجة: حرجة++; break;
                case شدة_ثغرة::عالية: عالية++; break;
                case شدة_ثغرة::متوسطة: متوسطة++; break;
                case شدة_ثغرة::منخفضة: منخفضة++; break;
            }
        }
        
        ت += "📊 ملخص:\n";
        ت += "   🔴 حرجة: " + std::to_string(حرجة) + "\n";
        ت += "   🟠 عالية: " + std::to_string(عالية) + "\n";
        ت += "   🟡 متوسطة: " + std::to_string(متوسطة) + "\n";
        ت += "   🟢 منخفضة: " + std::to_string(منخفضة) + "\n\n";
        
        ت += "📋 التفاصيل:\n\n";
        for (const auto& ث : ثغرات) {
            std::string رمز;
            switch (ث.شدة) {
                case شدة_ثغرة::حرجة: رمز = "🔴"; break;
                case شدة_ثغرة::عالية: رمز = "🟠"; break;
                case شدة_ثغرة::متوسطة: رمز = "🟡"; break;
                case شدة_ثغرة::منخفضة: رمز = "🟢"; break;
            }
            
            ت += رمز + " " + ث.ملف + ":" + std::to_string(ث.سطر) + "\n";
            ت += "   " + ث.وصف + "\n";
            ت += "   💡 " + ث.حل + "\n\n";
        }
        
        return ت;
    }
    
private:
    std::vector<ثغرة> افحص_الأسرار(const std::string& مسار, const std::string& محتوى) {
        std::vector<ثغرة> نتيجة;
        
        // أنماط الأسرار الشائعة
        std::vector<std::pair<std::regex, std::string>> أنماط = {
            {std::regex(R"(password\s*=\s*['\"](.+?)['\"])"), "كلمة مرور مكشوفة"},
            {std::regex(R"(api[_-]?key\s*=\s*['\"](.+?)['\"])"), "مفتاح API مكشوف"},
            {std::regex(R"(secret\s*=\s*['\"](.+?)['\"])"), "سر مكشوف"},
            {std::regex(R"(token\s*=\s*['\"](.+?)['\"])"), "رمز مكشوف"}
        };
        
        for (const auto& [نمط, وصف] : أنماط) {
            std::smatch تطابق;
            if (std::regex_search(محتوى, تطابق, نمط)) {
                نتيجة.push_back({
                    .نوع = نوع_ثغرة::أسرار_مكشوفة,
                    .شدة = شدة_ثغرة::حرجة,
                    .ملف = مسار,
                    .سطر = 1,
                    .وصف = وصف,
                    .حل = "استخدم متغيرات البيئة أو ملف .env"
                });
            }
        }
        
        return نتيجة;
    }
    
    std::vector<ثغرة> افحص_المدخلات(const std::string& مسار, const std::string& محتوى) {
        std::vector<ثغرة> نتيجة;
        
        // فحص استخدام eval
        if (محتوى.find("قيّم(") != std::string::npos ||
            محتوى.find("eval(") != std::string::npos) {
            نتيجة.push_back({
                .نوع = نوع_ثغرة::حقن_كود,
                .شدة = شدة_ثغرة::حرجة,
                .ملف = مسار,
                .سطر = 1,
                .وصف = "استخدام eval/قيّم خطير",
                .حل = "تجنب eval واستخدم طرق آمنة"
            });
        }
        
        // فحص SQL مباشر
        if (محتوى.find("استعلام(\"") != std::string::npos) {
            نتيجة.push_back({
                .نوع = نوع_ثغرة::حقن_كود,
                .شدة = شدة_ثغرة::عالية,
                .ملف = مسار,
                .سطر = 1,
                .وصف = "احتمال حقن SQL",
                .حل = "استخدم الاستعلامات المُعدّة (prepared statements)"
            });
        }
        
        return نتيجة;
    }
    
    std::vector<ثغرة> افحص_التجاوزات(const std::string& مسار, const std::string& محتوى) {
        std::vector<ثغرة> نتيجة;
        
        // فحص الوصول للمصفوفات بدون تحقق
        if (محتوى.find("[مدخل]") != std::string::npos ||
            محتوى.find("[input]") != std::string::npos) {
            نتيجة.push_back({
                .نوع = نوع_ثغرة::تجاوز_المخزن,
                .شدة = شدة_ثغرة::عالية,
                .ملف = مسار,
                .سطر = 1,
                .وصف = "وصول لمصفوفة بمدخل غير مُتحقق",
                .حل = "تحقق من حدود المصفوفة قبل الوصول"
            });
        }
        
        return نتيجة;
    }
};

/**
 * مُقوّي الأمان
 */
class مُقوّي_الأمان {
public:
    /**
     * إضافة تحقق المدخلات
     */
    std::string أضف_تحقق(const std::string& كود, const std::string& متغير) {
        std::string تحقق = "إذا " + متغير + " == فارغ ثم\n";
        تحقق += "    ارمِ خطأ(\"المدخل غير صالح\")\n";
        تحقق += "نهاية\n\n";
        return تحقق + كود;
    }
    
    /**
     * تشفير الأسرار
     */
    std::string شفّر_النص(const std::string& نص) {
        // تشفير بسيط (للتوضيح فقط)
        std::string مشفر;
        for (char c : نص) {
            مشفر += static_cast<char>(c + 1);
        }
        return مشفر;
    }
    
    /**
     * توليد توصيات الأمان
     */
    std::vector<std::string> توصيات() {
        return {
            "استخدم HTTPS دائماً",
            "شفّر البيانات الحساسة",
            "استخدم التحقق الثنائي",
            "حدّث الاعتماديات بانتظام",
            "استخدم CSP headers",
            "نظّف المدخلات دائماً",
            "استخدم prepared statements",
            "طبّق rate limiting"
        };
    }
};

} // namespace security
} // namespace sad

extern "C" {
using namespace sad::security;

void* sad_security_scanner_create() {
    return new فاحص_الأمان();
}

void sad_security_scanner_destroy(void* scanner) {
    delete static_cast<فاحص_الأمان*>(scanner);
}

void* sad_security_hardener_create() {
    return new مُقوّي_الأمان();
}

void sad_security_hardener_destroy(void* hardener) {
    delete static_cast<مُقوّي_الأمان*>(hardener);
}
}
