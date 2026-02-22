/**
 * ==========================================================
 * ملف: security_report.cpp
 * الوصف: مولّد تقارير الأمان
 * المهمة: T349 - أمان ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * ينشئ تقارير أمان شاملة بصيغ مختلفة:
 * - نص عادي للقراءة
 * - JSON للتكامل مع أدوات أخرى
 * - HTML للعرض في المتصفح
 * - SARIF للتكامل مع CI/CD
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <fstream>
#include <chrono>
#include <iomanip>

namespace sad {
namespace security {

// ==========================================================
// 📌 بيانات التقرير
// ==========================================================

/**
 * ثغرة في التقرير
 */
struct ثغرة_التقرير {
    std::string معرّف;
    std::string نوع;
    std::string خطورة;     // حرجة، عالية، متوسطة، منخفضة
    double درجة_CVSS = 0.0;
    std::string وصف;
    std::string ملف;
    int سطر = 0;
    int عمود = 0;
    std::string كود;
    std::string إصلاح;
    std::string مثال_هجوم;
};

/**
 * ملخص التقرير
 */
struct ملخص_التقرير {
    int إجمالي_الثغرات = 0;
    int حرجة = 0;
    int عالية = 0;
    int متوسطة = 0;
    int منخفضة = 0;
    int معلومات = 0;
    int ملفات_مفحوصة = 0;
    double درجة_الأمان = 100.0;  // 0-100
    
    std::string الحالة() const {
        if (حرجة > 0) return "❌ حرج";
        if (عالية > 0) return "⚠️ يتطلب إجراء";
        if (متوسطة > 0) return "⚡ مقبول";
        return "✅ آمن";
    }
    
    void احسب_الدرجة() {
        درجة_الأمان = 100.0;
        درجة_الأمان -= حرجة * 25.0;
        درجة_الأمان -= عالية * 15.0;
        درجة_الأمان -= متوسطة * 5.0;
        درجة_الأمان -= منخفضة * 1.0;
        if (درجة_الأمان < 0) درجة_الأمان = 0;
    }
};

/**
 * تقرير الأمان
 */
struct تقرير_الأمان {
    std::string عنوان;
    std::string تاريخ;
    std::string إصدار = "1.0";
    ملخص_التقرير ملخص;
    std::vector<ثغرة_التقرير> ثغرات;
    std::vector<std::string> توصيات;
};

// ==========================================================
// 📌 صيغ التقرير
// ==========================================================

/**
 * صيغة التقرير
 */
enum class صيغة_التقرير {
    نص,
    JSON,
    HTML,
    SARIF,
    Markdown
};

// ==========================================================
// 📌 مولّد التقارير الأساسي
// ==========================================================

/**
 * واجهة مولّد التقارير
 */
class مولّد_تقرير_أساسي {
public:
    virtual ~مولّد_تقرير_أساسي() = default;
    virtual std::string أنشئ(const تقرير_الأمان& تقرير) = 0;
    virtual صيغة_التقرير الصيغة() const = 0;
};

// ==========================================================
// 📌 مولّد تقرير نصي
// ==========================================================

/**
 * مولّد تقرير نصي
 */
class مولّد_تقرير_نصي : public مولّد_تقرير_أساسي {
public:
    std::string أنشئ(const تقرير_الأمان& تقرير) override {
        std::ostringstream ss;
        
        // العنوان
        ss << "\n";
        ss << "╔═══════════════════════════════════════════════════════════════╗\n";
        ss << "║           🔒 تقرير أمان لغة ص                                 ║\n";
        ss << "╚═══════════════════════════════════════════════════════════════╝\n\n";
        
        // معلومات عامة
        ss << "📅 التاريخ: " << تقرير.تاريخ << "\n";
        ss << "📊 درجة الأمان: " << std::fixed << std::setprecision(1) 
           << تقرير.ملخص.درجة_الأمان << "/100\n";
        ss << "📋 الحالة: " << تقرير.ملخص.الحالة() << "\n\n";
        
        // الملخص
        ss << "┌───────────────────────────────────────────────────────────────┐\n";
        ss << "│ 📊 ملخص الثغرات                                              │\n";
        ss << "├───────────────────────────────────────────────────────────────┤\n";
        ss << "│  🔴 حرجة:   " << std::setw(5) << تقرير.ملخص.حرجة 
           << "                                           │\n";
        ss << "│  🟠 عالية:   " << std::setw(5) << تقرير.ملخص.عالية 
           << "                                           │\n";
        ss << "│  🟡 متوسطة: " << std::setw(5) << تقرير.ملخص.متوسطة 
           << "                                           │\n";
        ss << "│  🟢 منخفضة: " << std::setw(5) << تقرير.ملخص.منخفضة 
           << "                                           │\n";
        ss << "│  ───────────                                                 │\n";
        ss << "│  📝 المجموع: " << std::setw(5) << تقرير.ملخص.إجمالي_الثغرات 
           << "                                           │\n";
        ss << "└───────────────────────────────────────────────────────────────┘\n\n";
        
        // تفاصيل الثغرات
        if (!تقرير.ثغرات.empty()) {
            ss << "┌───────────────────────────────────────────────────────────────┐\n";
            ss << "│ 🔍 تفاصيل الثغرات                                            │\n";
            ss << "└───────────────────────────────────────────────────────────────┘\n\n";
            
            int رقم = 1;
            for (const auto& ث : تقرير.ثغرات) {
                ss << "╭───────────────────────────────────────────────────────────────╮\n";
                ss << "│ ثغرة #" << رقم++ << ": " << ث.نوع << "\n";
                ss << "├───────────────────────────────────────────────────────────────┤\n";
                ss << "│ 📍 المعرّف: " << ث.معرّف << "\n";
                ss << "│ ⚠️ الخطورة: " << ث.خطورة << " (CVSS: " << ث.درجة_CVSS << ")\n";
                ss << "│ 📁 الملف: " << ث.ملف << ":" << ث.سطر << "\n";
                ss << "│ 📝 الوصف: " << ث.وصف << "\n";
                if (!ث.كود.empty()) {
                    ss << "│ 💻 الكود: " << ث.كود << "\n";
                }
                if (!ث.إصلاح.empty()) {
                    ss << "│ ✏️ الإصلاح: " << ث.إصلاح << "\n";
                }
                ss << "╰───────────────────────────────────────────────────────────────╯\n\n";
            }
        }
        
        // التوصيات
        if (!تقرير.توصيات.empty()) {
            ss << "┌───────────────────────────────────────────────────────────────┐\n";
            ss << "│ 💡 التوصيات                                                  │\n";
            ss << "└───────────────────────────────────────────────────────────────┘\n\n";
            
            int رقم = 1;
            for (const auto& توصية : تقرير.توصيات) {
                ss << "  " << رقم++ << ". " << توصية << "\n";
            }
            ss << "\n";
        }
        
        // الخاتمة
        ss << "═══════════════════════════════════════════════════════════════════\n";
        ss << "             تم إنشاء التقرير بواسطة أدوات أمان ص\n";
        ss << "═══════════════════════════════════════════════════════════════════\n";
        
        return ss.str();
    }
    
    صيغة_التقرير الصيغة() const override { return صيغة_التقرير::نص; }
};

// ==========================================================
// 📌 مولّد تقرير JSON
// ==========================================================

/**
 * مولّد تقرير JSON
 */
class مولّد_تقرير_JSON : public مولّد_تقرير_أساسي {
public:
    std::string أنشئ(const تقرير_الأمان& تقرير) override {
        std::ostringstream ss;
        
        ss << "{\n";
        ss << "  \"title\": \"" << تقرير.عنوان << "\",\n";
        ss << "  \"date\": \"" << تقرير.تاريخ << "\",\n";
        ss << "  \"version\": \"" << تقرير.إصدار << "\",\n";
        ss << "  \"summary\": {\n";
        ss << "    \"total\": " << تقرير.ملخص.إجمالي_الثغرات << ",\n";
        ss << "    \"critical\": " << تقرير.ملخص.حرجة << ",\n";
        ss << "    \"high\": " << تقرير.ملخص.عالية << ",\n";
        ss << "    \"medium\": " << تقرير.ملخص.متوسطة << ",\n";
        ss << "    \"low\": " << تقرير.ملخص.منخفضة << ",\n";
        ss << "    \"score\": " << تقرير.ملخص.درجة_الأمان << "\n";
        ss << "  },\n";
        
        // الثغرات
        ss << "  \"vulnerabilities\": [\n";
        for (size_t i = 0; i < تقرير.ثغرات.size(); i++) {
            const auto& ث = تقرير.ثغرات[i];
            ss << "    {\n";
            ss << "      \"id\": \"" << ث.معرّف << "\",\n";
            ss << "      \"type\": \"" << ث.نوع << "\",\n";
            ss << "      \"severity\": \"" << ث.خطورة << "\",\n";
            ss << "      \"cvss\": " << ث.درجة_CVSS << ",\n";
            ss << "      \"description\": \"" << هروب_JSON(ث.وصف) << "\",\n";
            ss << "      \"file\": \"" << هروب_JSON(ث.ملف) << "\",\n";
            ss << "      \"line\": " << ث.سطر << ",\n";
            ss << "      \"fix\": \"" << هروب_JSON(ث.إصلاح) << "\"\n";
            ss << "    }";
            if (i < تقرير.ثغرات.size() - 1) ss << ",";
            ss << "\n";
        }
        ss << "  ],\n";
        
        // التوصيات
        ss << "  \"recommendations\": [\n";
        for (size_t i = 0; i < تقرير.توصيات.size(); i++) {
            ss << "    \"" << هروب_JSON(تقرير.توصيات[i]) << "\"";
            if (i < تقرير.توصيات.size() - 1) ss << ",";
            ss << "\n";
        }
        ss << "  ]\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    صيغة_التقرير الصيغة() const override { return صيغة_التقرير::JSON; }
    
private:
    std::string هروب_JSON(const std::string& نص) {
        std::string نتيجة;
        for (char c : نص) {
            switch (c) {
                case '"': نتيجة += "\\\""; break;
                case '\\': نتيجة += "\\\\"; break;
                case '\n': نتيجة += "\\n"; break;
                case '\r': نتيجة += "\\r"; break;
                case '\t': نتيجة += "\\t"; break;
                default: نتيجة += c;
            }
        }
        return نتيجة;
    }
};

// ==========================================================
// 📌 مولّد تقرير HTML
// ==========================================================

/**
 * مولّد تقرير HTML
 */
class مولّد_تقرير_HTML : public مولّد_تقرير_أساسي {
public:
    std::string أنشئ(const تقرير_الأمان& تقرير) override {
        std::ostringstream ss;
        
        ss << R"(<!DOCTYPE html>
<html dir="rtl" lang="ar">
<head>
    <meta charset="UTF-8">
    <title>تقرير أمان ص</title>
    <style>
        body { font-family: 'Segoe UI', Tahoma, sans-serif; max-width: 1200px; margin: 0 auto; padding: 20px; background: #f5f5f5; }
        .header { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; padding: 30px; border-radius: 10px; text-align: center; }
        .summary { display: flex; justify-content: space-around; margin: 20px 0; }
        .card { background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .critical { border-left: 5px solid #dc3545; }
        .high { border-left: 5px solid #fd7e14; }
        .medium { border-left: 5px solid #ffc107; }
        .low { border-left: 5px solid #28a745; }
        .score { font-size: 48px; font-weight: bold; }
        .score.good { color: #28a745; }
        .score.medium { color: #ffc107; }
        .score.bad { color: #dc3545; }
        table { width: 100%; border-collapse: collapse; }
        th, td { padding: 12px; text-align: right; border-bottom: 1px solid #ddd; }
        th { background: #f8f9fa; }
        .badge { padding: 5px 10px; border-radius: 5px; color: white; font-size: 12px; }
        .badge-critical { background: #dc3545; }
        .badge-high { background: #fd7e14; }
        .badge-medium { background: #ffc107; color: #333; }
        .badge-low { background: #28a745; }
    </style>
</head>
<body>
    <div class="header">
        <h1>🔒 تقرير أمان لغة ص</h1>
        <p>)" << تقرير.تاريخ << R"(</p>
    </div>
    
    <div class="summary">
        <div class="card" style="text-align: center;">
            <h3>درجة الأمان</h3>
            <div class="score )" << (تقرير.ملخص.درجة_الأمان >= 80 ? "good" : تقرير.ملخص.درجة_الأمان >= 50 ? "medium" : "bad") << R"(">
                )" << std::fixed << std::setprecision(0) << تقرير.ملخص.درجة_الأمان << R"(/100
            </div>
        </div>
        <div class="card">
            <h3>ملخص الثغرات</h3>
            <p>🔴 حرجة: )" << تقرير.ملخص.حرجة << R"(</p>
            <p>🟠 عالية: )" << تقرير.ملخص.عالية << R"(</p>
            <p>🟡 متوسطة: )" << تقرير.ملخص.متوسطة << R"(</p>
            <p>🟢 منخفضة: )" << تقرير.ملخص.منخفضة << R"(</p>
        </div>
    </div>
)";
        
        // جدول الثغرات
        if (!تقرير.ثغرات.empty()) {
            ss << R"(
    <div class="card">
        <h3>🔍 تفاصيل الثغرات</h3>
        <table>
            <tr>
                <th>الخطورة</th>
                <th>النوع</th>
                <th>الملف</th>
                <th>السطر</th>
                <th>الوصف</th>
            </tr>
)";
            for (const auto& ث : تقرير.ثغرات) {
                std::string badge_class = "badge-low";
                if (ث.خطورة == "حرجة") badge_class = "badge-critical";
                else if (ث.خطورة == "عالية") badge_class = "badge-high";
                else if (ث.خطورة == "متوسطة") badge_class = "badge-medium";
                
                ss << "            <tr>\n";
                ss << "                <td><span class=\"badge " << badge_class << "\">" << ث.خطورة << "</span></td>\n";
                ss << "                <td>" << ث.نوع << "</td>\n";
                ss << "                <td>" << ث.ملف << "</td>\n";
                ss << "                <td>" << ث.سطر << "</td>\n";
                ss << "                <td>" << ث.وصف << "</td>\n";
                ss << "            </tr>\n";
            }
            ss << "        </table>\n    </div>\n";
        }
        
        ss << R"(
</body>
</html>
)";
        
        return ss.str();
    }
    
    صيغة_التقرير الصيغة() const override { return صيغة_التقرير::HTML; }
};

// ==========================================================
// 📌 نظام التقارير
// ==========================================================

/**
 * نظام إنشاء التقارير
 */
class نظام_التقارير {
private:
    std::map<صيغة_التقرير, std::unique_ptr<مولّد_تقرير_أساسي>> مولّدات_;
    
public:
    نظام_التقارير() {
        مولّدات_[صيغة_التقرير::نص] = std::make_unique<مولّد_تقرير_نصي>();
        مولّدات_[صيغة_التقرير::JSON] = std::make_unique<مولّد_تقرير_JSON>();
        مولّدات_[صيغة_التقرير::HTML] = std::make_unique<مولّد_تقرير_HTML>();
    }
    
    /**
     * إنشاء تقرير
     */
    std::string أنشئ(const تقرير_الأمان& تقرير, صيغة_التقرير صيغة) {
        auto it = مولّدات_.find(صيغة);
        if (it != مولّدات_.end()) {
            return it->second->أنشئ(تقرير);
        }
        return "";
    }
    
    /**
     * حفظ تقرير
     */
    bool احفظ(const تقرير_الأمان& تقرير, 
              const std::string& مسار,
              صيغة_التقرير صيغة) {
        std::string محتوى = أنشئ(تقرير, صيغة);
        
        std::ofstream ملف(مسار);
        if (!ملف.is_open()) return false;
        
        ملف << محتوى;
        return true;
    }
};

} // namespace security
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

using namespace sad::security;

typedef struct SadSecurityReport SadSecurityReport;
typedef struct SadReportGenerator SadReportGenerator;

SadSecurityReport* sad_security_report_create(const char* title) {
    auto* تقرير = new تقرير_الأمان();
    تقرير->عنوان = title ? title : "تقرير أمان";
    
    // تعيين التاريخ
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    تقرير->تاريخ = ss.str();
    
    return reinterpret_cast<SadSecurityReport*>(تقرير);
}

void sad_security_report_destroy(SadSecurityReport* report) {
    delete reinterpret_cast<تقرير_الأمان*>(report);
}

void sad_security_report_add_vulnerability(SadSecurityReport* report,
                                            const char* id,
                                            const char* type,
                                            const char* severity,
                                            const char* description,
                                            const char* file,
                                            int line) {
    if (!report) return;
    
    ثغرة_التقرير ث;
    ث.معرّف = id ? id : "";
    ث.نوع = type ? type : "";
    ث.خطورة = severity ? severity : "";
    ث.وصف = description ? description : "";
    ث.ملف = file ? file : "";
    ث.سطر = line;
    
    auto* تقرير = reinterpret_cast<تقرير_الأمان*>(report);
    تقرير->ثغرات.push_back(ث);
    تقرير->ملخص.إجمالي_الثغرات++;
    
    if (ث.خطورة == "حرجة") تقرير->ملخص.حرجة++;
    else if (ث.خطورة == "عالية") تقرير->ملخص.عالية++;
    else if (ث.خطورة == "متوسطة") تقرير->ملخص.متوسطة++;
    else تقرير->ملخص.منخفضة++;
    
    تقرير->ملخص.احسب_الدرجة();
}

SadReportGenerator* sad_report_generator_create(void) {
    return reinterpret_cast<SadReportGenerator*>(new نظام_التقارير());
}

void sad_report_generator_destroy(SadReportGenerator* generator) {
    delete reinterpret_cast<نظام_التقارير*>(generator);
}

const char* sad_report_generator_generate(SadReportGenerator* generator,
                                          SadSecurityReport* report,
                                          int format) {
    if (!generator || !report) return nullptr;
    
    static std::string نتيجة;
    auto* نظام = reinterpret_cast<نظام_التقارير*>(generator);
    auto* تقرير = reinterpret_cast<تقرير_الأمان*>(report);
    
    نتيجة = نظام->أنشئ(*تقرير, static_cast<صيغة_التقرير>(format));
    return نتيجة.c_str();
}

} // extern "C"
