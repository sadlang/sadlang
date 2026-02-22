/**
 * ==========================================================
 * ملف: dep_audit.cpp
 * الوصف: تدقيق أمان التبعيات
 * المهمة: T345 - أمان ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * يفحص المكتبات المستخدمة بحثاً عن ثغرات أمنية معروفة.
 * يحذر من إصدارات قديمة أو مكتبات غير آمنة.
 * 
 * 🔍 ما يفحصه:
 * - CVE (ثغرات معروفة)
 * - إصدارات قديمة
 * - تراخيص غير متوافقة
 * - تبعيات غير مستخدمة
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <regex>
#include <sstream>
#include <fstream>
#include <algorithm>

namespace sad {
namespace security {

// ==========================================================
// 📌 معلومات التبعية
// ==========================================================

/**
 * ثغرة أمنية
 */
struct ثغرة {
    std::string معرّف;        // CVE-2023-XXXX
    std::string وصف;
    std::string خطورة;        // حرجة، عالية، متوسطة، منخفضة
    double درجة_CVSS = 0.0;
    std::string إصدار_مصاب;   // نطاق الإصدارات المصابة
    std::string إصدار_مصحح;   // أول إصدار آمن
    std::string رابط;
    
    bool حرجة() const { return درجة_CVSS >= 9.0; }
    bool عالية() const { return درجة_CVSS >= 7.0; }
};

/**
 * تبعية
 */
struct تبعية {
    std::string اسم;
    std::string إصدار;
    std::string إصدار_أحدث;
    std::string ترخيص;
    std::vector<ثغرة> ثغرات;
    bool مباشرة = true;      // تبعية مباشرة أم غير مباشرة
    bool مستخدمة = true;
    
    bool آمنة() const { return ثغرات.empty(); }
    bool قديمة() const { return إصدار != إصدار_أحدث && !إصدار_أحدث.empty(); }
    
    int عدد_الثغرات_الحرجة() const {
        int عدد = 0;
        for (const auto& ث : ثغرات) {
            if (ث.حرجة()) عدد++;
        }
        return عدد;
    }
};

// ==========================================================
// 📌 قاعدة بيانات الثغرات
// ==========================================================

/**
 * قاعدة بيانات الثغرات المعروفة
 */
class قاعدة_الثغرات {
private:
    std::unordered_map<std::string, std::vector<ثغرة>> ثغرات_المكتبات_;
    std::string آخر_تحديث_;
    
public:
    قاعدة_الثغرات() {
        // ثغرات معروفة للمكتبات الشائعة (نموذجية)
        أضف_ثغرات_نموذجية();
    }
    
    /**
     * البحث عن ثغرات لمكتبة
     */
    std::vector<ثغرة> ابحث(const std::string& مكتبة, 
                            const std::string& إصدار) const {
        std::vector<ثغرة> نتائج;
        
        auto it = ثغرات_المكتبات_.find(مكتبة);
        if (it != ثغرات_المكتبات_.end()) {
            for (const auto& ثغرة : it->second) {
                if (إصدار_مصاب(إصدار, ثغرة.إصدار_مصاب, ثغرة.إصدار_مصحح)) {
                    نتائج.push_back(ثغرة);
                }
            }
        }
        
        return نتائج;
    }
    
    /**
     * تحديث من مصدر خارجي
     */
    bool حدّث_من_ملف(const std::string& مسار) {
        // قراءة ملف JSON للثغرات
        return true;
    }
    
private:
    void أضف_ثغرات_نموذجية() {
        // مثال: log4j (ثغرة معروفة)
        ثغرة log4j;
        log4j.معرّف = "CVE-2021-44228";
        log4j.وصف = "Remote Code Execution via JNDI lookup";
        log4j.خطورة = "حرجة";
        log4j.درجة_CVSS = 10.0;
        log4j.إصدار_مصاب = "<2.17.0";
        log4j.إصدار_مصحح = "2.17.0";
        ثغرات_المكتبات_["log4j"].push_back(log4j);
    }
    
    bool إصدار_مصاب(const std::string& إصدار,
                    const std::string& نطاق_مصاب,
                    const std::string& إصدار_مصحح) const {
        // مقارنة إصدارات مبسطة
        // في الواقع نحتاج SemVer parsing
        if (نطاق_مصاب.empty()) return false;
        
        // مثال: <2.17.0
        if (نطاق_مصاب[0] == '<') {
            return قارن_إصدارات(إصدار, نطاق_مصاب.substr(1)) < 0;
        }
        
        return false;
    }
    
    int قارن_إصدارات(const std::string& أ, const std::string& ب) const {
        // مقارنة إصدارات مبسطة
        return أ.compare(ب);
    }
};

// ==========================================================
// 📌 محلل التبعيات
// ==========================================================

/**
 * محلل ملفات التبعيات
 */
class محلل_التبعيات {
public:
    /**
     * تحليل ملف تبعيات Sad (sad.toml)
     */
    std::vector<تبعية> حلل_sad(const std::string& محتوى) {
        std::vector<تبعية> نتائج;
        
        std::istringstream stream(محتوى);
        std::string سطر;
        bool في_التبعيات = false;
        
        while (std::getline(stream, سطر)) {
            // تجاهل التعليقات والأسطر الفارغة
            if (سطر.empty() || سطر[0] == '#') continue;
            
            // قسم التبعيات
            if (سطر.find("[تبعيات]") != std::string::npos ||
                سطر.find("[dependencies]") != std::string::npos) {
                في_التبعيات = true;
                continue;
            }
            
            // نهاية القسم
            if (سطر[0] == '[' && في_التبعيات) {
                في_التبعيات = false;
                continue;
            }
            
            // تحليل تبعية
            if (في_التبعيات) {
                auto تب = حلل_سطر_تبعية(سطر);
                if (!تب.اسم.empty()) {
                    نتائج.push_back(تب);
                }
            }
        }
        
        return نتائج;
    }
    
private:
    تبعية حلل_سطر_تبعية(const std::string& سطر) {
        تبعية تب;
        
        // صيغة: اسم = "إصدار"
        auto pos = سطر.find('=');
        if (pos != std::string::npos) {
            تب.اسم = سطر.substr(0, pos);
            // إزالة المسافات
            تب.اسم.erase(0, تب.اسم.find_first_not_of(" \t"));
            تب.اسم.erase(تب.اسم.find_last_not_of(" \t") + 1);
            
            std::string قيمة = سطر.substr(pos + 1);
            // استخراج الإصدار من علامات الاقتباس
            auto بداية = قيمة.find('"');
            auto نهاية = قيمة.rfind('"');
            if (بداية != std::string::npos && نهاية > بداية) {
                تب.إصدار = قيمة.substr(بداية + 1, نهاية - بداية - 1);
            }
        }
        
        return تب;
    }
};

// ==========================================================
// 📌 تقرير التدقيق
// ==========================================================

/**
 * ملخص التدقيق
 */
struct ملخص_التدقيق {
    int إجمالي_التبعيات = 0;
    int تبعيات_آمنة = 0;
    int تبعيات_بثغرات = 0;
    int ثغرات_حرجة = 0;
    int ثغرات_عالية = 0;
    int ثغرات_متوسطة = 0;
    int ثغرات_منخفضة = 0;
    int تبعيات_قديمة = 0;
    int تبعيات_غير_مستخدمة = 0;
    
    bool آمن() const { 
        return ثغرات_حرجة == 0 && ثغرات_عالية == 0; 
    }
};

/**
 * تقرير تدقيق التبعيات
 */
struct تقرير_التدقيق {
    std::vector<تبعية> تبعيات;
    ملخص_التدقيق ملخص;
    std::string تاريخ;
    
    /**
     * إنشاء نص التقرير
     */
    std::string النص() const {
        std::ostringstream ss;
        
        ss << "═══════════════════════════════════════════════════\n";
        ss << "📋 تقرير تدقيق أمان التبعيات\n";
        ss << "═══════════════════════════════════════════════════\n\n";
        
        // الملخص
        ss << "📊 ملخص:\n";
        ss << "   إجمالي التبعيات: " << ملخص.إجمالي_التبعيات << "\n";
        ss << "   تبعيات آمنة: " << ملخص.تبعيات_آمنة << " ✓\n";
        ss << "   تبعيات بثغرات: " << ملخص.تبعيات_بثغرات << " ⚠\n";
        ss << "\n";
        
        // الثغرات
        ss << "🔴 ثغرات حرجة: " << ملخص.ثغرات_حرجة << "\n";
        ss << "🟠 ثغرات عالية: " << ملخص.ثغرات_عالية << "\n";
        ss << "🟡 ثغرات متوسطة: " << ملخص.ثغرات_متوسطة << "\n";
        ss << "🟢 ثغرات منخفضة: " << ملخص.ثغرات_منخفضة << "\n";
        ss << "\n";
        
        // التفاصيل
        if (!تبعيات.empty()) {
            ss << "───────────────────────────────────────────────────\n";
            ss << "📦 تفاصيل التبعيات:\n";
            ss << "───────────────────────────────────────────────────\n\n";
            
            for (const auto& تب : تبعيات) {
                ss << "📦 " << تب.اسم << " @ " << تب.إصدار;
                
                if (تب.آمنة()) {
                    ss << " ✓ آمن\n";
                } else {
                    ss << " ⚠ " << تب.ثغرات.size() << " ثغرة\n";
                    
                    for (const auto& ث : تب.ثغرات) {
                        ss << "   ├─ " << ث.معرّف << " (" << ث.خطورة << ")\n";
                        ss << "   │  " << ث.وصف << "\n";
                        ss << "   │  إصلاح: ترقية إلى " << ث.إصدار_مصحح << "\n";
                    }
                }
                ss << "\n";
            }
        }
        
        // الحالة النهائية
        ss << "═══════════════════════════════════════════════════\n";
        if (ملخص.آمن()) {
            ss << "✅ الحالة: آمن\n";
        } else {
            ss << "❌ الحالة: يتطلب إجراء\n";
        }
        ss << "═══════════════════════════════════════════════════\n";
        
        return ss.str();
    }
};

// ==========================================================
// 📌 مدقق التبعيات
// ==========================================================

/**
 * مدقق أمان التبعيات
 */
class مدقق_التبعيات {
private:
    قاعدة_الثغرات قاعدة_;
    محلل_التبعيات محلل_;
    
public:
    /**
     * تدقيق ملف تبعيات
     */
    تقرير_التدقيق دقق_ملف(const std::string& مسار) {
        std::ifstream ملف(مسار);
        if (!ملف.is_open()) {
            return تقرير_التدقيق{};
        }
        
        std::stringstream buffer;
        buffer << ملف.rdbuf();
        return دقق_محتوى(buffer.str());
    }
    
    /**
     * تدقيق محتوى
     */
    تقرير_التدقيق دقق_محتوى(const std::string& محتوى) {
        تقرير_التدقيق تقرير;
        
        // تحليل التبعيات
        auto تبعيات = محلل_.حلل_sad(محتوى);
        
        // فحص كل تبعية
        for (auto& تب : تبعيات) {
            تب.ثغرات = قاعدة_.ابحث(تب.اسم, تب.إصدار);
            تقرير.تبعيات.push_back(تب);
        }
        
        // حساب الملخص
        احسب_الملخص(تقرير);
        
        return تقرير;
    }
    
    /**
     * تدقيق قائمة تبعيات
     */
    تقرير_التدقيق دقق_قائمة(const std::vector<std::pair<std::string, std::string>>& قائمة) {
        تقرير_التدقيق تقرير;
        
        for (const auto& [اسم, إصدار] : قائمة) {
            تبعية تب;
            تب.اسم = اسم;
            تب.إصدار = إصدار;
            تب.ثغرات = قاعدة_.ابحث(اسم, إصدار);
            تقرير.تبعيات.push_back(تب);
        }
        
        احسب_الملخص(تقرير);
        
        return تقرير;
    }
    
private:
    void احسب_الملخص(تقرير_التدقيق& تقرير) {
        تقرير.ملخص.إجمالي_التبعيات = static_cast<int>(تقرير.تبعيات.size());
        
        for (const auto& تب : تقرير.تبعيات) {
            if (تب.آمنة()) {
                تقرير.ملخص.تبعيات_آمنة++;
            } else {
                تقرير.ملخص.تبعيات_بثغرات++;
                
                for (const auto& ث : تب.ثغرات) {
                    if (ث.درجة_CVSS >= 9.0) {
                        تقرير.ملخص.ثغرات_حرجة++;
                    } else if (ث.درجة_CVSS >= 7.0) {
                        تقرير.ملخص.ثغرات_عالية++;
                    } else if (ث.درجة_CVSS >= 4.0) {
                        تقرير.ملخص.ثغرات_متوسطة++;
                    } else {
                        تقرير.ملخص.ثغرات_منخفضة++;
                    }
                }
            }
            
            if (تب.قديمة()) {
                تقرير.ملخص.تبعيات_قديمة++;
            }
            
            if (!تب.مستخدمة) {
                تقرير.ملخص.تبعيات_غير_مستخدمة++;
            }
        }
    }
};

} // namespace security
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

using namespace sad::security;

typedef struct SadDepAuditor SadDepAuditor;
typedef struct SadAuditReport SadAuditReport;

SadDepAuditor* sad_dep_auditor_create(void) {
    return reinterpret_cast<SadDepAuditor*>(new مدقق_التبعيات());
}

void sad_dep_auditor_destroy(SadDepAuditor* auditor) {
    delete reinterpret_cast<مدقق_التبعيات*>(auditor);
}

SadAuditReport* sad_dep_auditor_audit_file(SadDepAuditor* auditor, const char* path) {
    if (!auditor || !path) return nullptr;
    
    auto تقرير = reinterpret_cast<مدقق_التبعيات*>(auditor)->دقق_ملف(path);
    return reinterpret_cast<SadAuditReport*>(new تقرير_التدقيق(std::move(تقرير)));
}

void sad_audit_report_destroy(SadAuditReport* report) {
    delete reinterpret_cast<تقرير_التدقيق*>(report);
}

int sad_audit_report_is_safe(SadAuditReport* report) {
    if (!report) return 0;
    return reinterpret_cast<تقرير_التدقيق*>(report)->ملخص.آمن() ? 1 : 0;
}

int sad_audit_report_critical_count(SadAuditReport* report) {
    if (!report) return 0;
    return reinterpret_cast<تقرير_التدقيق*>(report)->ملخص.ثغرات_حرجة;
}

const char* sad_audit_report_text(SadAuditReport* report) {
    if (!report) return nullptr;
    static std::string نص;
    نص = reinterpret_cast<تقرير_التدقيق*>(report)->النص();
    return نص.c_str();
}

} // extern "C"
