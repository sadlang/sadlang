/**
 * =============================================================================
 * ملف: report_generator.cpp
 * الوصف: مُولِّد تقارير اختبار الحمل
 * المهمة: T1141 - Phase 111
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

namespace sad {
namespace tools {

/**
 * نتائج الاختبار
 */
struct نتائج_اختبار {
    std::string اسم_الاختبار;
    std::string تاريخ_البدء;
    std::string تاريخ_الانتهاء;
    int إجمالي_الطلبات;
    int طلبات_ناجحة;
    int طلبات_فاشلة;
    double متوسط_الاستجابة_ms;
    double أقصى_استجابة_ms;
    double أدنى_استجابة_ms;
    double طلبات_في_الثانية;
    std::map<int, int> توزيع_حالات_HTTP;
    std::vector<std::pair<int, double>> أوقات_percentile;  // p50, p90, p99
};

/**
 * 📊 مُولِّد التقارير
 */
class مولد_تقارير {
public:
    /**
     * توليد تقرير نصي
     */
    static std::string ولد_نصي(const نتائج_اختبار& نتائج) {
        std::ostringstream تقرير;
        
        تقرير << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                    📊 تقرير اختبار الحمل                                     ║
╠══════════════════════════════════════════════════════════════════════════════╣
)";
        تقرير << "║ الاختبار: " << نتائج.اسم_الاختبار << "\n";
        تقرير << "║ البدء: " << نتائج.تاريخ_البدء << "\n";
        تقرير << "║ الانتهاء: " << نتائج.تاريخ_الانتهاء << "\n";
        تقرير << "╠══════════════════════════════════════════════════════════════════════════════╣\n";
        
        تقرير << "║ 📈 الإحصائيات العامة:\n";
        تقرير << "║   • إجمالي الطلبات: " << نتائج.إجمالي_الطلبات << "\n";
        تقرير << "║   • طلبات ناجحة: " << نتائج.طلبات_ناجحة 
               << " (" << (نتائج.إجمالي_الطلبات > 0 ? نتائج.طلبات_ناجحة * 100.0 / نتائج.إجمالي_الطلبات : 0) << "%)\n";
        تقرير << "║   • طلبات فاشلة: " << نتائج.طلبات_فاشلة << "\n";
        تقرير << "║   • طلبات/ثانية: " << std::fixed << std::setprecision(2) << نتائج.طلبات_في_الثانية << "\n";
        
        تقرير << "╠══════════════════════════════════════════════════════════════════════════════╣\n";
        تقرير << "║ ⏱️ أوقات الاستجابة:\n";
        تقرير << "║   • المتوسط: " << std::fixed << std::setprecision(2) << نتائج.متوسط_الاستجابة_ms << " ms\n";
        تقرير << "║   • الأدنى: " << نتائج.أدنى_استجابة_ms << " ms\n";
        تقرير << "║   • الأقصى: " << نتائج.أقصى_استجابة_ms << " ms\n";
        
        for (const auto& [percentile, قيمة] : نتائج.أوقات_percentile) {
            تقرير << "║   • P" << percentile << ": " << قيمة << " ms\n";
        }
        
        تقرير << "╠══════════════════════════════════════════════════════════════════════════════╣\n";
        تقرير << "║ 📊 توزيع حالات HTTP:\n";
        for (const auto& [حالة, عدد] : نتائج.توزيع_حالات_HTTP) {
            تقرير << "║   • " << حالة << ": " << عدد << "\n";
        }
        
        تقرير << "╚══════════════════════════════════════════════════════════════════════════════╝\n";
        
        return تقرير.str();
    }
    
    /**
     * توليد تقرير HTML
     */
    static std::string ولد_html(const نتائج_اختبار& نتائج) {
        std::ostringstream html;
        
        html << R"(<!DOCTYPE html>
<html dir="rtl" lang="ar">
<head>
    <meta charset="UTF-8">
    <title>تقرير اختبار الحمل</title>
    <style>
        body { font-family: 'Segoe UI', Tahoma, sans-serif; margin: 20px; background: #f5f5f5; }
        .card { background: white; border-radius: 8px; padding: 20px; margin: 10px 0; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        h1 { color: #2c3e50; }
        .stat { display: inline-block; margin: 10px 20px; text-align: center; }
        .stat-value { font-size: 2em; font-weight: bold; color: #3498db; }
        .stat-label { color: #7f8c8d; }
        .success { color: #27ae60; }
        .error { color: #e74c3c; }
    </style>
</head>
<body>
    <h1>📊 تقرير اختبار الحمل</h1>
    
    <div class="card">
        <h2>معلومات الاختبار</h2>
        <p><strong>الاسم:</strong> )" << نتائج.اسم_الاختبار << R"(</p>
        <p><strong>البدء:</strong> )" << نتائج.تاريخ_البدء << R"(</p>
        <p><strong>الانتهاء:</strong> )" << نتائج.تاريخ_الانتهاء << R"(</p>
    </div>
    
    <div class="card">
        <h2>الإحصائيات</h2>
        <div class="stat">
            <div class="stat-value">)" << نتائج.إجمالي_الطلبات << R"(</div>
            <div class="stat-label">إجمالي الطلبات</div>
        </div>
        <div class="stat">
            <div class="stat-value success">)" << نتائج.طلبات_ناجحة << R"(</div>
            <div class="stat-label">ناجحة</div>
        </div>
        <div class="stat">
            <div class="stat-value error">)" << نتائج.طلبات_فاشلة << R"(</div>
            <div class="stat-label">فاشلة</div>
        </div>
        <div class="stat">
            <div class="stat-value">)" << std::fixed << std::setprecision(2) << نتائج.طلبات_في_الثانية << R"(</div>
            <div class="stat-label">طلب/ثانية</div>
        </div>
    </div>
    
    <div class="card">
        <h2>أوقات الاستجابة</h2>
        <div class="stat">
            <div class="stat-value">)" << std::fixed << std::setprecision(0) << نتائج.متوسط_الاستجابة_ms << R"( ms</div>
            <div class="stat-label">المتوسط</div>
        </div>
        <div class="stat">
            <div class="stat-value">)" << نتائج.أدنى_استجابة_ms << R"( ms</div>
            <div class="stat-label">الأدنى</div>
        </div>
        <div class="stat">
            <div class="stat-value">)" << نتائج.أقصى_استجابة_ms << R"( ms</div>
            <div class="stat-label">الأقصى</div>
        </div>
    </div>
</body>
</html>)";
        
        return html.str();
    }
    
    /**
     * توليد تقرير JSON
     */
    static std::string ولد_json(const نتائج_اختبار& نتائج) {
        std::ostringstream json;
        
        json << "{\n";
        json << "  \"اسم_الاختبار\": \"" << نتائج.اسم_الاختبار << "\",\n";
        json << "  \"تاريخ_البدء\": \"" << نتائج.تاريخ_البدء << "\",\n";
        json << "  \"تاريخ_الانتهاء\": \"" << نتائج.تاريخ_الانتهاء << "\",\n";
        json << "  \"إجمالي_الطلبات\": " << نتائج.إجمالي_الطلبات << ",\n";
        json << "  \"طلبات_ناجحة\": " << نتائج.طلبات_ناجحة << ",\n";
        json << "  \"طلبات_فاشلة\": " << نتائج.طلبات_فاشلة << ",\n";
        json << "  \"متوسط_الاستجابة_ms\": " << نتائج.متوسط_الاستجابة_ms << ",\n";
        json << "  \"طلبات_في_الثانية\": " << نتائج.طلبات_في_الثانية << "\n";
        json << "}";
        
        return json.str();
    }
    
    /**
     * حفظ التقرير
     */
    static bool احفظ(const std::string& محتوى, const std::string& مسار) {
        std::ofstream ملف(مسار);
        if (!ملف.is_open()) return false;
        ملف << محتوى;
        return true;
    }
};

} // namespace tools
} // namespace sad

int main() {
    // بيانات تجريبية
    sad::tools::نتائج_اختبار نتائج;
    نتائج.اسم_الاختبار = "اختبار API الرئيسي";
    نتائج.تاريخ_البدء = "2026-01-25 10:00:00";
    نتائج.تاريخ_الانتهاء = "2026-01-25 10:05:00";
    نتائج.إجمالي_الطلبات = 10000;
    نتائج.طلبات_ناجحة = 9850;
    نتائج.طلبات_فاشلة = 150;
    نتائج.متوسط_الاستجابة_ms = 125.5;
    نتائج.أقصى_استجابة_ms = 2500;
    نتائج.أدنى_استجابة_ms = 15;
    نتائج.طلبات_في_الثانية = 33.3;
    نتائج.توزيع_حالات_HTTP = {{200, 9500}, {201, 350}, {400, 100}, {500, 50}};
    نتائج.أوقات_percentile = {{50, 100}, {90, 250}, {99, 800}};
    
    std::cout << sad::tools::مولد_تقارير::ولد_نصي(نتائج);
    
    return 0;
}
