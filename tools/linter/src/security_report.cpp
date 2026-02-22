/**
 * =============================================================================
 * ملف: security_report.cpp
 * الوصف: نظام توليد تقارير الأمان
 * المهمة: T225 - Security report generation
 * المرحلة: Phase 22 - User Story 19 (Security Linting ص حارس)
 * =============================================================================
 * 
 * 📊 دليل المبتدئ لتقارير الأمان
 * ═════════════════════════════════
 * 
 * ما هو تقرير الأمان؟
 * ────────────────────
 * وثيقة تُظهر نتائج فحص كودك الأمني.
 * مثل تقرير طبي لبرنامجك!
 * 
 * أنواع التقارير:
 * ─────────────────
 * 
 * 1. تقرير نصي (Text)
 *    سريع وبسيط للقراءة في الطرفية
 * 
 * 2. تقرير HTML
 *    ملون وجميل للمشاركة مع الفريق
 * 
 * 3. تقرير JSON
 *    للأدوات الآلية وخطوط CI/CD
 * 
 * 4. تقرير PDF
 *    رسمي للتوثيق والأرشفة
 * 
 * 5. تقرير SARIF
 *    معيار صناعي لأدوات IDE
 * 
 * ماذا يتضمن التقرير؟
 * ─────────────────────
 * 
 * 1. ملخص تنفيذي
 *    - النتيجة الإجمالية
 *    - أهم المشاكل
 *    - التوصيات العاجلة
 * 
 * 2. الإحصائيات
 *    - عدد المشاكل حسب الخطورة
 *    - عدد المشاكل حسب النوع
 *    - الملفات الأكثر مشاكل
 * 
 * 3. تفاصيل المشاكل
 *    - وصف كل مشكلة
 *    - موقعها في الكود
 *    - كيفية إصلاحها
 * 
 * 4. رسومات بيانية
 *    - توزيع الخطورة
 *    - اتجاه المشاكل عبر الوقت
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <algorithm>

namespace sad::security::reporting {

// =============================================================================
// بنية البيانات
// =============================================================================

enum class Severity {
    Info = 1,
    Low = 3,
    Medium = 5,
    High = 7,
    Critical = 9
};

inline std::string severityToArabic(Severity sev) {
    switch (sev) {
        case Severity::Critical: return "حرج";
        case Severity::High:     return "عالي";
        case Severity::Medium:   return "متوسط";
        case Severity::Low:      return "منخفض";
        case Severity::Info:     return "معلومة";
    }
    return "غير معروف";
}

inline std::string severityToColor(Severity sev) {
    switch (sev) {
        case Severity::Critical: return "#ff4444";
        case Severity::High:     return "#ff8c00";
        case Severity::Medium:   return "#ffd700";
        case Severity::Low:      return "#4dabf7";
        case Severity::Info:     return "#aaaaaa";
    }
    return "#888888";
}

struct Finding {
    std::string id;
    std::string ruleId;
    std::string category;
    std::string message;
    std::string description;
    std::string file;
    int line = 0;
    int column = 0;
    int endLine = 0;
    int endColumn = 0;
    Severity severity = Severity::Medium;
    std::string code;
    std::string explanation;
    std::string recommendation;
    std::string cweId;
    std::string owaspId;
    std::vector<std::string> references;
    std::string fixedCode;
    bool canAutoFix = false;
};

struct ScanMetadata {
    std::string projectName;
    std::string version;
    std::string timestamp;
    int duration = 0; // milliseconds
    int filesScanned = 0;
    int linesScanned = 0;
    std::string tool = "ص حارس";
    std::string toolVersion = "1.0.0";
    std::vector<std::string> rules;
};

struct ScanResult {
    ScanMetadata metadata;
    std::vector<Finding> findings;
    
    int getCount(Severity sev) const {
        int count = 0;
        for (const auto& f : findings) {
            if (f.severity == sev) count++;
        }
        return count;
    }
    
    int getTotalCount() const {
        return static_cast<int>(findings.size());
    }
    
    std::map<std::string, int> getByCategory() const {
        std::map<std::string, int> counts;
        for (const auto& f : findings) {
            counts[f.category]++;
        }
        return counts;
    }
    
    std::map<std::string, int> getByFile() const {
        std::map<std::string, int> counts;
        for (const auto& f : findings) {
            counts[f.file]++;
        }
        return counts;
    }
};

// =============================================================================
// مولدات التقارير
// =============================================================================

/**
 * واجهة مولد التقارير
 */
class ReportGenerator {
public:
    virtual ~ReportGenerator() = default;
    virtual std::string generate(const ScanResult& result) = 0;
    virtual std::string getExtension() const = 0;
    virtual std::string getMimeType() const = 0;
};

/**
 * تقرير نصي
 */
class TextReportGenerator : public ReportGenerator {
public:
    std::string generate(const ScanResult& result) override {
        std::ostringstream ss;
        
        // العنوان
        ss << "\n";
        ss << "╔═══════════════════════════════════════════════════════════════════════════╗\n";
        ss << "║                    🛡️ تقرير الفحص الأمني - ص حارس                         ║\n";
        ss << "╚═══════════════════════════════════════════════════════════════════════════╝\n";
        
        // البيانات الوصفية
        ss << "\n📋 معلومات الفحص:\n";
        ss << "├─ المشروع: " << result.metadata.projectName << "\n";
        ss << "├─ التاريخ: " << result.metadata.timestamp << "\n";
        ss << "├─ الملفات: " << result.metadata.filesScanned << "\n";
        ss << "├─ الأسطر: " << result.metadata.linesScanned << "\n";
        ss << "└─ المدة: " << result.metadata.duration << " مللي ثانية\n";
        
        // الملخص
        ss << "\n📊 الملخص التنفيذي:\n";
        ss << "┌─────────────────────────────────────────────────┐\n";
        
        int critical = result.getCount(Severity::Critical);
        int high = result.getCount(Severity::High);
        int medium = result.getCount(Severity::Medium);
        int low = result.getCount(Severity::Low);
        int total = result.getTotalCount();
        
        ss << "│ 🔴 حرج:    " << std::setw(5) << critical << "  ";
        ss << generateBar(critical, total, 20) << " │\n";
        ss << "│ 🟠 عالي:   " << std::setw(5) << high << "  ";
        ss << generateBar(high, total, 20) << " │\n";
        ss << "│ 🟡 متوسط:  " << std::setw(5) << medium << "  ";
        ss << generateBar(medium, total, 20) << " │\n";
        ss << "│ 🔵 منخفض:  " << std::setw(5) << low << "  ";
        ss << generateBar(low, total, 20) << " │\n";
        ss << "├─────────────────────────────────────────────────┤\n";
        ss << "│ المجموع:  " << std::setw(5) << total << "                            │\n";
        ss << "└─────────────────────────────────────────────────┘\n";
        
        // النتيجة
        ss << "\n";
        if (critical > 0) {
            ss << "⚠️ تحذير: يوجد " << critical << " مشاكل حرجة يجب معالجتها فوراً!\n";
        } else if (high > 0) {
            ss << "⚡ تنبيه: يوجد " << high << " مشاكل عالية الخطورة.\n";
        } else if (total == 0) {
            ss << "✅ ممتاز! لم يُكتشف أي مشاكل أمنية.\n";
        } else {
            ss << "ℹ️ يوجد مشاكل بسيطة يُنصح بمعالجتها.\n";
        }
        
        // حسب الفئة
        ss << "\n📁 حسب الفئة:\n";
        auto byCategory = result.getByCategory();
        for (const auto& [cat, count] : byCategory) {
            ss << "  • " << cat << ": " << count << "\n";
        }
        
        // حسب الملف
        ss << "\n📄 الملفات الأكثر مشاكل:\n";
        auto byFile = result.getByFile();
        std::vector<std::pair<std::string, int>> sorted(byFile.begin(), byFile.end());
        std::sort(sorted.begin(), sorted.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        int shown = 0;
        for (const auto& [file, count] : sorted) {
            if (shown++ >= 5) break;
            ss << "  • " << file << " (" << count << ")\n";
        }
        
        // التفاصيل
        ss << "\n" << std::string(75, '═') << "\n";
        ss << "                        تفاصيل المشاكل\n";
        ss << std::string(75, '═') << "\n";
        
        int num = 0;
        for (const auto& f : result.findings) {
            num++;
            ss << "\n";
            ss << "┌─ #" << num << " ─────────────────────────────────────────────────────┐\n";
            ss << "│ " << getSeverityIcon(f.severity) << " [" << f.ruleId << "] " << f.message << "\n";
            ss << "│ 📍 " << f.file << ":" << f.line;
            if (f.column > 0) ss << ":" << f.column;
            ss << "\n";
            
            if (!f.cweId.empty()) {
                ss << "│ 🔗 " << f.cweId << "\n";
            }
            
            if (!f.code.empty()) {
                ss << "│ 📝 " << f.code << "\n";
            }
            
            if (!f.explanation.empty()) {
                ss << "│\n│ 📖 الشرح:\n";
                std::istringstream explStream(f.explanation);
                std::string line;
                while (std::getline(explStream, line)) {
                    ss << "│    " << line << "\n";
                }
            }
            
            if (!f.recommendation.empty()) {
                ss << "│\n│ 💡 التوصية:\n";
                ss << "│    " << f.recommendation << "\n";
            }
            
            ss << "└─────────────────────────────────────────────────────────────┘\n";
        }
        
        // التوصيات العامة
        ss << "\n" << std::string(75, '═') << "\n";
        ss << "                     التوصيات العامة\n";
        ss << std::string(75, '═') << "\n";
        
        if (critical > 0) {
            ss << "\n🔴 الإجراءات العاجلة:\n";
            ss << "   1. راجع كل المشاكل الحرجة فوراً\n";
            ss << "   2. أوقف النشر حتى الإصلاح\n";
            ss << "   3. أبلغ فريق الأمان\n";
        }
        
        ss << "\n📚 أفضل الممارسات:\n";
        ss << "   • استخدم الاستعلامات المعدة بدل SQL الديناميكي\n";
        ss << "   • تحقق من جميع المدخلات الخارجية\n";
        ss << "   • لا تخزن الأسرار في الكود\n";
        ss << "   • استخدم خوارزميات تشفير حديثة\n";
        ss << "   • افحص الكود دورياً\n";
        
        ss << "\n" << std::string(75, '═') << "\n";
        ss << "تم التوليد بواسطة: ص حارس v" << result.metadata.toolVersion << "\n";
        
        return ss.str();
    }
    
    std::string getExtension() const override { return ".txt"; }
    std::string getMimeType() const override { return "text/plain; charset=utf-8"; }
    
private:
    std::string getSeverityIcon(Severity sev) const {
        switch (sev) {
            case Severity::Critical: return "🔴";
            case Severity::High:     return "🟠";
            case Severity::Medium:   return "🟡";
            case Severity::Low:      return "🔵";
            default:                 return "⚪";
        }
    }
    
    std::string generateBar(int value, int total, int width) const {
        if (total == 0) return std::string(width, ' ');
        int filled = (value * width) / std::max(total, 1);
        return std::string(filled, '█') + std::string(width - filled, '░');
    }
};

/**
 * تقرير HTML
 */
class HTMLReportGenerator : public ReportGenerator {
public:
    std::string generate(const ScanResult& result) override {
        std::ostringstream ss;
        
        int critical = result.getCount(Severity::Critical);
        int high = result.getCount(Severity::High);
        int medium = result.getCount(Severity::Medium);
        int low = result.getCount(Severity::Low);
        int total = result.getTotalCount();
        
        ss << R"(<!DOCTYPE html>
<html dir="rtl" lang="ar">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>تقرير الفحص الأمني - )" << result.metadata.projectName << R"(</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <style>
        :root {
            --bg-dark: #1a1a2e;
            --bg-card: #16213e;
            --bg-input: #0f3460;
            --text-primary: #eee;
            --text-secondary: #aaa;
            --critical: #ff4444;
            --high: #ff8c00;
            --medium: #ffd700;
            --low: #4dabf7;
            --info: #aaa;
            --success: #00c853;
        }
        
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Arial, sans-serif;
            background: var(--bg-dark);
            color: var(--text-primary);
            line-height: 1.6;
        }
        
        .container {
            max-width: 1400px;
            margin: 0 auto;
            padding: 20px;
        }
        
        /* Header */
        .header {
            text-align: center;
            padding: 40px 20px;
            background: linear-gradient(135deg, #16213e, #0f3460);
            border-radius: 20px;
            margin-bottom: 30px;
            box-shadow: 0 10px 40px rgba(0,0,0,0.3);
        }
        
        .header h1 {
            font-size: 2.5em;
            margin-bottom: 10px;
        }
        
        .header .meta {
            color: var(--text-secondary);
        }
        
        /* Summary Cards */
        .summary-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        
        .stat-card {
            background: var(--bg-card);
            border-radius: 15px;
            padding: 25px;
            text-align: center;
            transition: transform 0.3s;
        }
        
        .stat-card:hover {
            transform: translateY(-5px);
        }
        
        .stat-card .number {
            font-size: 3em;
            font-weight: bold;
            line-height: 1;
        }
        
        .stat-card .label {
            margin-top: 10px;
            color: var(--text-secondary);
        }
        
        .stat-card.critical .number { color: var(--critical); }
        .stat-card.high .number { color: var(--high); }
        .stat-card.medium .number { color: var(--medium); }
        .stat-card.low .number { color: var(--low); }
        
        /* Charts */
        .charts-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(400px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        
        .chart-card {
            background: var(--bg-card);
            border-radius: 15px;
            padding: 25px;
        }
        
        .chart-card h3 {
            margin-bottom: 20px;
            text-align: center;
        }
        
        /* Findings */
        .findings-section h2 {
            margin-bottom: 20px;
            padding-bottom: 10px;
            border-bottom: 2px solid var(--bg-input);
        }
        
        .finding-card {
            background: var(--bg-card);
            border-radius: 15px;
            margin-bottom: 15px;
            overflow: hidden;
            border-right: 5px solid;
        }
        
        .finding-card.critical { border-color: var(--critical); }
        .finding-card.high { border-color: var(--high); }
        .finding-card.medium { border-color: var(--medium); }
        .finding-card.low { border-color: var(--low); }
        
        .finding-header {
            padding: 20px;
            cursor: pointer;
            display: flex;
            justify-content: space-between;
            align-items: center;
        }
        
        .finding-header:hover {
            background: var(--bg-input);
        }
        
        .finding-title {
            font-weight: bold;
        }
        
        .finding-meta {
            color: var(--text-secondary);
            font-size: 0.9em;
        }
        
        .severity-badge {
            padding: 5px 12px;
            border-radius: 20px;
            font-size: 0.8em;
            font-weight: bold;
        }
        
        .severity-badge.critical { background: var(--critical); color: white; }
        .severity-badge.high { background: var(--high); color: black; }
        .severity-badge.medium { background: var(--medium); color: black; }
        .severity-badge.low { background: var(--low); color: black; }
        
        .finding-body {
            padding: 20px;
            border-top: 1px solid var(--bg-input);
            display: none;
        }
        
        .finding-body.show {
            display: block;
        }
        
        .code-block {
            background: #0a0a0a;
            padding: 15px;
            border-radius: 8px;
            font-family: 'Consolas', monospace;
            overflow-x: auto;
            margin: 10px 0;
        }
        
        .recommendation {
            background: rgba(0, 200, 83, 0.1);
            border-right: 3px solid var(--success);
            padding: 15px;
            border-radius: 8px;
            margin-top: 15px;
        }
        
        /* Footer */
        .footer {
            text-align: center;
            padding: 30px;
            color: var(--text-secondary);
            border-top: 1px solid var(--bg-input);
            margin-top: 30px;
        }
        
        /* Filters */
        .filters {
            display: flex;
            gap: 10px;
            margin-bottom: 20px;
            flex-wrap: wrap;
        }
        
        .filter-btn {
            padding: 10px 20px;
            border: none;
            border-radius: 20px;
            cursor: pointer;
            background: var(--bg-input);
            color: var(--text-primary);
            transition: all 0.3s;
        }
        
        .filter-btn:hover, .filter-btn.active {
            background: var(--text-primary);
            color: var(--bg-dark);
        }
        
        /* Print */
        @media print {
            body { background: white; color: black; }
            .stat-card, .chart-card, .finding-card { 
                border: 1px solid #ddd; 
                break-inside: avoid;
            }
            .filters { display: none; }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🛡️ تقرير الفحص الأمني</h1>
            <p class="meta">
                <strong>)" << result.metadata.projectName << R"(</strong> | 
                )" << result.metadata.timestamp << R"( | 
                )" << result.metadata.filesScanned << R"( ملف
            </p>
        </div>
        
        <!-- Summary Cards -->
        <div class="summary-grid">
            <div class="stat-card critical">
                <div class="number">)" << critical << R"(</div>
                <div class="label">🔴 حرج</div>
            </div>
            <div class="stat-card high">
                <div class="number">)" << high << R"(</div>
                <div class="label">🟠 عالي</div>
            </div>
            <div class="stat-card medium">
                <div class="number">)" << medium << R"(</div>
                <div class="label">🟡 متوسط</div>
            </div>
            <div class="stat-card low">
                <div class="number">)" << low << R"(</div>
                <div class="label">🔵 منخفض</div>
            </div>
            <div class="stat-card">
                <div class="number">)" << total << R"(</div>
                <div class="label">المجموع</div>
            </div>
        </div>
        
        <!-- Charts -->
        <div class="charts-grid">
            <div class="chart-card">
                <h3>📊 توزيع الخطورة</h3>
                <canvas id="severityChart"></canvas>
            </div>
            <div class="chart-card">
                <h3>📁 حسب الفئة</h3>
                <canvas id="categoryChart"></canvas>
            </div>
        </div>
        
        <!-- Findings -->
        <div class="findings-section">
            <h2>📋 تفاصيل المشاكل</h2>
            
            <div class="filters">
                <button class="filter-btn active" onclick="filterFindings('all')">الكل</button>
                <button class="filter-btn" onclick="filterFindings('critical')">حرج</button>
                <button class="filter-btn" onclick="filterFindings('high')">عالي</button>
                <button class="filter-btn" onclick="filterFindings('medium')">متوسط</button>
                <button class="filter-btn" onclick="filterFindings('low')">منخفض</button>
            </div>
)";
        
        // المشاكل
        for (const auto& f : result.findings) {
            std::string sevClass;
            std::string sevText;
            switch (f.severity) {
                case Severity::Critical: sevClass = "critical"; sevText = "حرج"; break;
                case Severity::High: sevClass = "high"; sevText = "عالي"; break;
                case Severity::Medium: sevClass = "medium"; sevText = "متوسط"; break;
                case Severity::Low: sevClass = "low"; sevText = "منخفض"; break;
                default: sevClass = "info"; sevText = "معلومة"; break;
            }
            
            ss << R"(
            <div class="finding-card )" << sevClass << R"(" data-severity=")" << sevClass << R"(">
                <div class="finding-header" onclick="toggleFinding(this)">
                    <div>
                        <div class="finding-title">[)" << escapeHTML(f.ruleId) << R"(] )" << escapeHTML(f.message) << R"(</div>
                        <div class="finding-meta">📍 )" << escapeHTML(f.file) << R"(:)" << f.line << R"(</div>
                    </div>
                    <span class="severity-badge )" << sevClass << R"(">)" << sevText << R"(</span>
                </div>
                <div class="finding-body">)";
            
            if (!f.code.empty()) {
                ss << R"(<div class="code-block">)" << escapeHTML(f.code) << R"(</div>)";
            }
            
            if (!f.explanation.empty()) {
                ss << R"(<p><strong>📖 الشرح:</strong><br>)" << escapeHTML(f.explanation) << R"(</p>)";
            }
            
            if (!f.cweId.empty()) {
                ss << R"(<p><strong>🔗 المرجع:</strong> )" << escapeHTML(f.cweId) << R"(</p>)";
            }
            
            if (!f.recommendation.empty()) {
                ss << R"(<div class="recommendation"><strong>💡 التوصية:</strong><br>)" 
                   << escapeHTML(f.recommendation) << R"(</div>)";
            }
            
            ss << R"(
                </div>
            </div>)";
        }
        
        // إغلاق HTML
        ss << R"(
        </div>
        
        <div class="footer">
            <p>تم التوليد بواسطة ص حارس v)" << result.metadata.toolVersion << R"(</p>
        </div>
    </div>
    
    <script>
        // Charts
        const severityCtx = document.getElementById('severityChart').getContext('2d');
        new Chart(severityCtx, {
            type: 'doughnut',
            data: {
                labels: ['حرج', 'عالي', 'متوسط', 'منخفض'],
                datasets: [{
                    data: [)" << critical << "," << high << "," << medium << "," << low << R"(],
                    backgroundColor: ['#ff4444', '#ff8c00', '#ffd700', '#4dabf7']
                }]
            },
            options: {
                plugins: {
                    legend: {
                        position: 'bottom',
                        labels: { color: '#eee' }
                    }
                }
            }
        });
        
        const categoryCtx = document.getElementById('categoryChart').getContext('2d');
        new Chart(categoryCtx, {
            type: 'bar',
            data: {
                labels: [)";
        
        auto byCategory = result.getByCategory();
        bool first = true;
        for (const auto& [cat, count] : byCategory) {
            if (!first) ss << ",";
            ss << "'" << cat << "'";
            first = false;
        }
        
        ss << R"(],
                datasets: [{
                    label: 'عدد المشاكل',
                    data: [)";
        
        first = true;
        for (const auto& [cat, count] : byCategory) {
            if (!first) ss << ",";
            ss << count;
            first = false;
        }
        
        ss << R"(],
                    backgroundColor: '#4dabf7'
                }]
            },
            options: {
                indexAxis: 'y',
                plugins: {
                    legend: { display: false }
                },
                scales: {
                    x: { ticks: { color: '#aaa' }, grid: { color: '#333' } },
                    y: { ticks: { color: '#aaa' }, grid: { color: '#333' } }
                }
            }
        });
        
        // Toggle findings
        function toggleFinding(header) {
            const body = header.nextElementSibling;
            body.classList.toggle('show');
        }
        
        // Filter findings
        function filterFindings(severity) {
            document.querySelectorAll('.filter-btn').forEach(btn => btn.classList.remove('active'));
            event.target.classList.add('active');
            
            document.querySelectorAll('.finding-card').forEach(card => {
                if (severity === 'all' || card.dataset.severity === severity) {
                    card.style.display = 'block';
                } else {
                    card.style.display = 'none';
                }
            });
        }
    </script>
</body>
</html>)";
        
        return ss.str();
    }
    
    std::string getExtension() const override { return ".html"; }
    std::string getMimeType() const override { return "text/html; charset=utf-8"; }
    
private:
    std::string escapeHTML(const std::string& s) const {
        std::string result;
        for (char c : s) {
            switch (c) {
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                case '&': result += "&amp;"; break;
                case '"': result += "&quot;"; break;
                case '\n': result += "<br>"; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

/**
 * تقرير JSON
 */
class JSONReportGenerator : public ReportGenerator {
public:
    std::string generate(const ScanResult& result) override {
        std::ostringstream ss;
        
        ss << "{\n";
        ss << "  \"metadata\": {\n";
        ss << "    \"projectName\": \"" << escape(result.metadata.projectName) << "\",\n";
        ss << "    \"version\": \"" << escape(result.metadata.version) << "\",\n";
        ss << "    \"timestamp\": \"" << escape(result.metadata.timestamp) << "\",\n";
        ss << "    \"duration\": " << result.metadata.duration << ",\n";
        ss << "    \"filesScanned\": " << result.metadata.filesScanned << ",\n";
        ss << "    \"linesScanned\": " << result.metadata.linesScanned << ",\n";
        ss << "    \"tool\": \"" << escape(result.metadata.tool) << "\",\n";
        ss << "    \"toolVersion\": \"" << escape(result.metadata.toolVersion) << "\"\n";
        ss << "  },\n";
        
        ss << "  \"summary\": {\n";
        ss << "    \"total\": " << result.getTotalCount() << ",\n";
        ss << "    \"critical\": " << result.getCount(Severity::Critical) << ",\n";
        ss << "    \"high\": " << result.getCount(Severity::High) << ",\n";
        ss << "    \"medium\": " << result.getCount(Severity::Medium) << ",\n";
        ss << "    \"low\": " << result.getCount(Severity::Low) << "\n";
        ss << "  },\n";
        
        ss << "  \"findings\": [\n";
        
        bool first = true;
        for (const auto& f : result.findings) {
            if (!first) ss << ",\n";
            first = false;
            
            ss << "    {\n";
            ss << "      \"id\": \"" << escape(f.id) << "\",\n";
            ss << "      \"ruleId\": \"" << escape(f.ruleId) << "\",\n";
            ss << "      \"category\": \"" << escape(f.category) << "\",\n";
            ss << "      \"message\": \"" << escape(f.message) << "\",\n";
            ss << "      \"file\": \"" << escape(f.file) << "\",\n";
            ss << "      \"line\": " << f.line << ",\n";
            ss << "      \"column\": " << f.column << ",\n";
            ss << "      \"severity\": " << static_cast<int>(f.severity) << ",\n";
            ss << "      \"severityText\": \"" << severityToArabic(f.severity) << "\",\n";
            ss << "      \"cweId\": \"" << escape(f.cweId) << "\",\n";
            ss << "      \"code\": \"" << escape(f.code) << "\",\n";
            ss << "      \"explanation\": \"" << escape(f.explanation) << "\",\n";
            ss << "      \"recommendation\": \"" << escape(f.recommendation) << "\",\n";
            ss << "      \"canAutoFix\": " << (f.canAutoFix ? "true" : "false") << "\n";
            ss << "    }";
        }
        
        ss << "\n  ]\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    std::string getExtension() const override { return ".json"; }
    std::string getMimeType() const override { return "application/json; charset=utf-8"; }
    
private:
    std::string escape(const std::string& s) const {
        std::string result;
        for (char c : s) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

/**
 * تقرير SARIF (Static Analysis Results Interchange Format)
 */
class SARIFReportGenerator : public ReportGenerator {
public:
    std::string generate(const ScanResult& result) override {
        std::ostringstream ss;
        
        ss << "{\n";
        ss << "  \"$schema\": \"https://json.schemastore.org/sarif-2.1.0.json\",\n";
        ss << "  \"version\": \"2.1.0\",\n";
        ss << "  \"runs\": [{\n";
        ss << "    \"tool\": {\n";
        ss << "      \"driver\": {\n";
        ss << "        \"name\": \"ص حارس\",\n";
        ss << "        \"version\": \"" << result.metadata.toolVersion << "\",\n";
        ss << "        \"informationUri\": \"https://sad-lang.org/guard\",\n";
        ss << "        \"rules\": []\n";
        ss << "      }\n";
        ss << "    },\n";
        ss << "    \"results\": [\n";
        
        bool first = true;
        for (const auto& f : result.findings) {
            if (!first) ss << ",\n";
            first = false;
            
            ss << "      {\n";
            ss << "        \"ruleId\": \"" << f.ruleId << "\",\n";
            ss << "        \"level\": \"" << sarifLevel(f.severity) << "\",\n";
            ss << "        \"message\": {\n";
            ss << "          \"text\": \"" << escape(f.message) << "\"\n";
            ss << "        },\n";
            ss << "        \"locations\": [{\n";
            ss << "          \"physicalLocation\": {\n";
            ss << "            \"artifactLocation\": {\n";
            ss << "              \"uri\": \"" << escape(f.file) << "\"\n";
            ss << "            },\n";
            ss << "            \"region\": {\n";
            ss << "              \"startLine\": " << f.line << ",\n";
            ss << "              \"startColumn\": " << f.column << "\n";
            ss << "            }\n";
            ss << "          }\n";
            ss << "        }]\n";
            ss << "      }";
        }
        
        ss << "\n    ]\n";
        ss << "  }]\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    std::string getExtension() const override { return ".sarif"; }
    std::string getMimeType() const override { return "application/sarif+json"; }
    
private:
    std::string sarifLevel(Severity sev) const {
        switch (sev) {
            case Severity::Critical:
            case Severity::High:
                return "error";
            case Severity::Medium:
                return "warning";
            default:
                return "note";
        }
    }
    
    std::string escape(const std::string& s) const {
        std::string result;
        for (char c : s) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                default: result += c; break;
            }
        }
        return result;
    }
};

// =============================================================================
// مصنع التقارير
// =============================================================================

class ReportFactory {
public:
    static std::unique_ptr<ReportGenerator> create(const std::string& format) {
        if (format == "html") {
            return std::make_unique<HTMLReportGenerator>();
        }
        if (format == "json") {
            return std::make_unique<JSONReportGenerator>();
        }
        if (format == "sarif") {
            return std::make_unique<SARIFReportGenerator>();
        }
        // Default: text
        return std::make_unique<TextReportGenerator>();
    }
};

} // namespace sad::security::reporting

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadSecurityReport {
    sad::security::reporting::ScanResult* result;
};

SadSecurityReport* sad_security_report_new() {
    auto* ctx = new SadSecurityReport();
    ctx->result = new sad::security::reporting::ScanResult();
    return ctx;
}

void sad_security_report_free(SadSecurityReport* ctx) {
    if (ctx) {
        delete ctx->result;
        delete ctx;
    }
}

void sad_security_report_set_metadata(
    SadSecurityReport* ctx,
    const char* project,
    const char* version,
    int files,
    int lines
) {
    if (!ctx) return;
    ctx->result->metadata.projectName = project;
    ctx->result->metadata.version = version;
    ctx->result->metadata.filesScanned = files;
    ctx->result->metadata.linesScanned = lines;
    
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    ctx->result->metadata.timestamp = std::ctime(&t);
}

void sad_security_report_add_finding(
    SadSecurityReport* ctx,
    const char* ruleId,
    const char* message,
    const char* file,
    int line,
    int severity
) {
    if (!ctx) return;
    sad::security::reporting::Finding f;
    f.ruleId = ruleId;
    f.message = message;
    f.file = file;
    f.line = line;
    f.severity = static_cast<sad::security::reporting::Severity>(severity);
    ctx->result->findings.push_back(f);
}

char* sad_security_report_generate(SadSecurityReport* ctx, const char* format) {
    if (!ctx) return nullptr;
    auto generator = sad::security::reporting::ReportFactory::create(format);
    std::string report = generator->generate(*ctx->result);
    char* result = new char[report.size() + 1];
    std::strcpy(result, report.c_str());
    return result;
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef SECURITY_REPORT_TEST

#include <cassert>

void testTextReport() {
    sad::security::reporting::ScanResult result;
    result.metadata.projectName = "مشروعي";
    result.metadata.filesScanned = 10;
    
    sad::security::reporting::Finding f;
    f.ruleId = "SQL001";
    f.message = "احتمال حقن SQL";
    f.file = "app.ص";
    f.line = 42;
    f.severity = sad::security::reporting::Severity::Critical;
    result.findings.push_back(f);
    
    auto gen = sad::security::reporting::ReportFactory::create("text");
    std::string report = gen->generate(result);
    
    assert(report.find("SQL001") != std::string::npos);
    assert(report.find("مشروعي") != std::string::npos);
    
    std::cout << "✅ اختبار التقرير النصي نجح\n";
}

void testHTMLReport() {
    sad::security::reporting::ScanResult result;
    result.metadata.projectName = "تطبيقي";
    
    auto gen = sad::security::reporting::ReportFactory::create("html");
    std::string report = gen->generate(result);
    
    assert(report.find("<!DOCTYPE html>") != std::string::npos);
    assert(report.find("تطبيقي") != std::string::npos);
    
    std::cout << "✅ اختبار تقرير HTML نجح\n";
}

void testJSONReport() {
    sad::security::reporting::ScanResult result;
    result.metadata.projectName = "test";
    
    auto gen = sad::security::reporting::ReportFactory::create("json");
    std::string report = gen->generate(result);
    
    assert(report.find("\"projectName\"") != std::string::npos);
    
    std::cout << "✅ اختبار تقرير JSON نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات نظام التقارير\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testTextReport();
    testHTMLReport();
    testJSONReport();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // SECURITY_REPORT_TEST
