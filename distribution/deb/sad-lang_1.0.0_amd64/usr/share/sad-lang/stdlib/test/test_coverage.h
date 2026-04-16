/**
 * =============================================================================
 * @file test_coverage.h
 * @brief (AR) أداة التغطية — Code Coverage Tool
 * @brief (EN) Code Coverage Tool
 * 
 * المهمة: T256 - Coverage Tool
 * المرحلة: سبتمبر 2026 — Test Framework
 * =============================================================================
 * 
 * 🎯 الميزات
 * ══════════
 * ✓ تتبع الأسطر المنفذة
 * ✓ تتبع الدوال المستدعاة
 * ✓ تتبع الفروع (if/else)
 * ✓ تقارير التغطية (HTML, console)
 * ✓ حساب نسبة التغطية
 * 
 * =============================================================================
 */

#ifndef SAD_STDLIB_TEST_COVERAGE_H
#define SAD_STDLIB_TEST_COVERAGE_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace sad {
namespace test {

namespace fs = std::filesystem;

// =============================================================================
// الأنواع
// =============================================================================

/// معلومات سطر
struct LineCoverage {
    int lineNumber;
    int hitCount = 0;       // عدد مرات التنفيذ
    bool executable = true; // هل يمكن تنفيذه؟
    std::string source;     // الكود المصدري
};

/// معلومات دالة
struct FunctionCoverage {
    std::string name;
    int startLine;
    int endLine;
    int hitCount = 0;
};

/// معلومات فرع
struct BranchCoverage {
    int line;
    int totalBranches;
    int coveredBranches = 0;
};

/// معلومات ملف
struct FileCoverage {
    std::string path;
    std::string relativePath;
    std::vector<LineCoverage> lines;
    std::vector<FunctionCoverage> functions;
    std::vector<BranchCoverage> branches;
    
    int totalLines() const {
        int count = 0;
        for (const auto& line : lines) {
            if (line.executable) count++;
        }
        return count;
    }
    
    int coveredLines() const {
        int count = 0;
        for (const auto& line : lines) {
            if (line.executable && line.hitCount > 0) count++;
        }
        return count;
    }
    
    double lineRate() const {
        int total = totalLines();
        return total > 0 ? (coveredLines() * 100.0 / total) : 100.0;
    }
    
    int totalFunctions() const { return functions.size(); }
    
    int coveredFunctions() const {
        int count = 0;
        for (const auto& fn : functions) {
            if (fn.hitCount > 0) count++;
        }
        return count;
    }
    
    double functionRate() const {
        int total = totalFunctions();
        return total > 0 ? (coveredFunctions() * 100.0 / total) : 100.0;
    }
    
    int totalBranches() const {
        int count = 0;
        for (const auto& br : branches) {
            count += br.totalBranches;
        }
        return count;
    }
    
    int coveredBranches() const {
        int count = 0;
        for (const auto& br : branches) {
            count += br.coveredBranches;
        }
        return count;
    }
    
    double branchRate() const {
        int total = totalBranches();
        return total > 0 ? (coveredBranches() * 100.0 / total) : 100.0;
    }
};

/// إحصائيات التغطية الكلية
struct CoverageStats {
    int totalFiles = 0;
    int totalLines = 0;
    int coveredLines = 0;
    int totalFunctions = 0;
    int coveredFunctions = 0;
    int totalBranches = 0;
    int coveredBranches = 0;
    
    double lineRate() const {
        return totalLines > 0 ? (coveredLines * 100.0 / totalLines) : 100.0;
    }
    
    double functionRate() const {
        return totalFunctions > 0 ? (coveredFunctions * 100.0 / totalFunctions) : 100.0;
    }
    
    double branchRate() const {
        return totalBranches > 0 ? (coveredBranches * 100.0 / totalBranches) : 100.0;
    }
};

// =============================================================================
// متتبع التغطية
// =============================================================================

class CoverageTracker {
public:
    // ─────────────────────────────────────────────
    // Singleton
    // ─────────────────────────────────────────────
    
    static CoverageTracker& instance() {
        static CoverageTracker tracker;
        return tracker;
    }
    
    // ─────────────────────────────────────────────
    // التتبع
    // ─────────────────────────────────────────────
    
    /**
     * تسجيل تنفيذ سطر
     */
    void recordLine(const std::string& file, int line) {
        lineHits_[file][line]++;
    }
    
    /**
     * تسجيل استدعاء دالة
     */
    void recordFunction(const std::string& file, const std::string& name) {
        functionHits_[file][name]++;
    }
    
    /**
     * تسجيل تنفيذ فرع
     */
    void recordBranch(const std::string& file, int line, int branchIndex) {
        branchHits_[file][line].insert(branchIndex);
    }
    
    /**
     * تسجيل معلومات الملف
     */
    void registerFile(const std::string& file, int totalLines) {
        if (fileInfo_.find(file) == fileInfo_.end()) {
            fileInfo_[file].totalLines = totalLines;
        }
    }
    
    /**
     * تسجيل دالة
     */
    void registerFunction(const std::string& file, const std::string& name,
                         int startLine, int endLine) {
        fileInfo_[file].functions.push_back({name, startLine, endLine, 0});
    }
    
    /**
     * تسجيل فرع
     */
    void registerBranch(const std::string& file, int line, int totalBranches) {
        fileInfo_[file].branches.push_back({line, totalBranches, 0});
    }
    
    // ─────────────────────────────────────────────
    // الحصول على النتائج
    // ─────────────────────────────────────────────
    
    /**
     * بناء تقرير التغطية
     */
    std::vector<FileCoverage> buildReport(const std::string& baseDir = ".") {
        std::vector<FileCoverage> report;
        
        for (const auto& [file, hits] : lineHits_) {
            FileCoverage coverage;
            coverage.path = file;
            coverage.relativePath = fs::relative(file, baseDir).string();
            
            // قراءة الملف المصدري
            std::ifstream source(file);
            std::string line;
            int lineNum = 1;
            
            while (std::getline(source, line)) {
                LineCoverage lineCov;
                lineCov.lineNumber = lineNum;
                lineCov.source = line;
                lineCov.executable = isExecutableLine(line);
                
                if (hits.count(lineNum)) {
                    lineCov.hitCount = hits.at(lineNum);
                }
                
                coverage.lines.push_back(lineCov);
                lineNum++;
            }
            
            // الدوال
            if (functionHits_.count(file)) {
                auto& info = fileInfo_[file];
                for (auto& fn : info.functions) {
                    if (functionHits_[file].count(fn.name)) {
                        fn.hitCount = functionHits_[file][fn.name];
                    }
                    coverage.functions.push_back(fn);
                }
            }
            
            // الفروع
            if (branchHits_.count(file)) {
                auto& info = fileInfo_[file];
                for (auto& br : info.branches) {
                    if (branchHits_[file].count(br.line)) {
                        br.coveredBranches = branchHits_[file][br.line].size();
                    }
                    coverage.branches.push_back(br);
                }
            }
            
            report.push_back(coverage);
        }
        
        return report;
    }
    
    /**
     * الحصول على الإحصائيات الكلية
     */
    CoverageStats getStats() {
        auto report = buildReport();
        
        CoverageStats stats;
        stats.totalFiles = report.size();
        
        for (const auto& file : report) {
            stats.totalLines += file.totalLines();
            stats.coveredLines += file.coveredLines();
            stats.totalFunctions += file.totalFunctions();
            stats.coveredFunctions += file.coveredFunctions();
            stats.totalBranches += file.totalBranches();
            stats.coveredBranches += file.coveredBranches();
        }
        
        return stats;
    }
    
    // ─────────────────────────────────────────────
    // التقارير
    // ─────────────────────────────────────────────
    
    /**
     * طباعة تقرير بسيط
     */
    void printSummary() {
        auto stats = getStats();
        
        std::cout << "\n═══════════════════════════════════════════════════\n";
        std::cout << "  📊 تقرير التغطية\n";
        std::cout << "═══════════════════════════════════════════════════\n";
        std::cout << "  📁 الملفات:   " << stats.totalFiles << "\n";
        std::cout << "  ───────────────────────────────────────────────\n";
        std::cout << "  📏 الأسطر:    " << stats.coveredLines << "/" << stats.totalLines
                  << " (" << std::fixed << std::setprecision(1) << stats.lineRate() << "%)\n";
        std::cout << "  📦 الدوال:    " << stats.coveredFunctions << "/" << stats.totalFunctions
                  << " (" << std::fixed << std::setprecision(1) << stats.functionRate() << "%)\n";
        std::cout << "  🔀 الفروع:    " << stats.coveredBranches << "/" << stats.totalBranches
                  << " (" << std::fixed << std::setprecision(1) << stats.branchRate() << "%)\n";
        std::cout << "═══════════════════════════════════════════════════\n\n";
    }
    
    /**
     * توليد تقرير HTML
     */
    void generateHtmlReport(const std::string& outputDir = "coverage") {
        auto report = buildReport();
        auto stats = getStats();
        
        // إنشاء المجلد
        fs::create_directories(outputDir);
        
        // الصفحة الرئيسية
        generateIndexHtml(outputDir, report, stats);
        
        // صفحة لكل ملف
        for (const auto& file : report) {
            generateFileHtml(outputDir, file);
        }
    }
    
    // ─────────────────────────────────────────────
    // تنظيف
    // ─────────────────────────────────────────────
    
    void reset() {
        lineHits_.clear();
        functionHits_.clear();
        branchHits_.clear();
        fileInfo_.clear();
    }

private:
    CoverageTracker() = default;
    
    // البيانات
    std::map<std::string, std::map<int, int>> lineHits_;
    std::map<std::string, std::map<std::string, int>> functionHits_;
    std::map<std::string, std::map<int, std::set<int>>> branchHits_;
    
    struct FileInfo {
        int totalLines = 0;
        std::vector<FunctionCoverage> functions;
        std::vector<BranchCoverage> branches;
    };
    std::map<std::string, FileInfo> fileInfo_;
    
    // ─────────────────────────────────────────────
    // مساعدات
    // ─────────────────────────────────────────────
    
    bool isExecutableLine(const std::string& line) {
        std::string trimmed = line;
        
        // إزالة المسافات
        size_t start = trimmed.find_first_not_of(" \t");
        if (start == std::string::npos) return false;
        trimmed = trimmed.substr(start);
        
        // تجاهل التعليقات
        if (trimmed[0] == '#') return false;
        
        // تجاهل نهاية
        if (trimmed.find("نهاية") == 0) return false;
        
        // تجاهل الأسطر الفارغة
        if (trimmed.empty()) return false;
        
        return true;
    }
    
    std::string getRateClass(double rate) {
        if (rate >= 80) return "high";
        if (rate >= 50) return "medium";
        return "low";
    }
    
    void generateIndexHtml(const std::string& outputDir,
                          const std::vector<FileCoverage>& report,
                          const CoverageStats& stats) {
        std::ofstream file(outputDir + "/index.html");
        if (!file) return;
        
        file << R"(<!DOCTYPE html>
<html dir="rtl" lang="ar">
<head>
    <meta charset="UTF-8">
    <title>تقرير التغطية — لغة ص</title>
    <style>
        :root {
            --high: #22c55e;
            --medium: #eab308;
            --low: #ef4444;
            --bg: #0f172a;
            --card: #1e293b;
            --text: #f1f5f9;
            --border: #334155;
        }
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body {
            font-family: 'Segoe UI', Tahoma, sans-serif;
            background: var(--bg);
            color: var(--text);
            padding: 2rem;
            line-height: 1.6;
        }
        .header {
            text-align: center;
            padding: 2rem;
            border-bottom: 1px solid var(--border);
            margin-bottom: 2rem;
        }
        .stats {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
            gap: 1rem;
            margin-bottom: 2rem;
        }
        .stat {
            background: var(--card);
            padding: 1.5rem;
            border-radius: 0.5rem;
            text-align: center;
        }
        .stat-value { font-size: 2.5rem; font-weight: bold; }
        .stat-label { color: #94a3b8; }
        .stat.high .stat-value { color: var(--high); }
        .stat.medium .stat-value { color: var(--medium); }
        .stat.low .stat-value { color: var(--low); }
        table {
            width: 100%;
            border-collapse: collapse;
            background: var(--card);
            border-radius: 0.5rem;
            overflow: hidden;
        }
        th, td { padding: 1rem; text-align: right; }
        th { background: #0f172a; }
        tr:hover { background: rgba(255,255,255,0.05); }
        .bar {
            height: 8px;
            background: var(--border);
            border-radius: 4px;
            overflow: hidden;
        }
        .bar-fill { height: 100%; }
        .bar-fill.high { background: var(--high); }
        .bar-fill.medium { background: var(--medium); }
        .bar-fill.low { background: var(--low); }
        a { color: #60a5fa; text-decoration: none; }
        a:hover { text-decoration: underline; }
    </style>
</head>
<body>
    <div class="header">
        <h1>📊 تقرير التغطية</h1>
        <p>لغة ص — )" << stats.totalFiles << R"( ملفات</p>
    </div>
    
    <div class="stats">
        <div class="stat )" << getRateClass(stats.lineRate()) << R"(">
            <div class="stat-value">)" << std::fixed << std::setprecision(1) << stats.lineRate() << R"(%</div>
            <div class="stat-label">تغطية الأسطر</div>
        </div>
        <div class="stat )" << getRateClass(stats.functionRate()) << R"(">
            <div class="stat-value">)" << std::fixed << std::setprecision(1) << stats.functionRate() << R"(%</div>
            <div class="stat-label">تغطية الدوال</div>
        </div>
        <div class="stat )" << getRateClass(stats.branchRate()) << R"(">
            <div class="stat-value">)" << std::fixed << std::setprecision(1) << stats.branchRate() << R"(%</div>
            <div class="stat-label">تغطية الفروع</div>
        </div>
    </div>
    
    <table>
        <thead>
            <tr>
                <th>الملف</th>
                <th>الأسطر</th>
                <th>التغطية</th>
                <th></th>
            </tr>
        </thead>
        <tbody>
)";
        
        for (const auto& f : report) {
            std::string fileHtml = fs::path(f.path).filename().string() + ".html";
            double rate = f.lineRate();
            
            file << R"(            <tr>
                <td><a href=")" << fileHtml << R"(">)" << f.relativePath << R"(</a></td>
                <td>)" << f.coveredLines() << "/" << f.totalLines() << R"(</td>
                <td>)" << std::fixed << std::setprecision(1) << rate << R"(%</td>
                <td style="width: 200px;">
                    <div class="bar">
                        <div class="bar-fill )" << getRateClass(rate) << R"(" style="width: )" << rate << R"(%"></div>
                    </div>
                </td>
            </tr>
)";
        }
        
        file << R"(        </tbody>
    </table>
</body>
</html>
)";
    }
    
    void generateFileHtml(const std::string& outputDir, const FileCoverage& coverage) {
        std::string filename = fs::path(coverage.path).filename().string() + ".html";
        std::ofstream file(outputDir + "/" + filename);
        if (!file) return;
        
        file << R"(<!DOCTYPE html>
<html dir="rtl" lang="ar">
<head>
    <meta charset="UTF-8">
    <title>)" << coverage.relativePath << R"( — تغطية</title>
    <style>
        :root {
            --covered: rgba(34, 197, 94, 0.2);
            --uncovered: rgba(239, 68, 68, 0.2);
            --bg: #0f172a;
            --card: #1e293b;
            --text: #f1f5f9;
        }
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body {
            font-family: 'Consolas', 'Monaco', monospace;
            background: var(--bg);
            color: var(--text);
            padding: 2rem;
        }
        .header {
            margin-bottom: 2rem;
            padding-bottom: 1rem;
            border-bottom: 1px solid #334155;
        }
        .line {
            display: flex;
            padding: 0 1rem;
        }
        .line-num {
            width: 60px;
            text-align: left;
            color: #64748b;
            user-select: none;
        }
        .line-hits {
            width: 50px;
            text-align: center;
            color: #64748b;
        }
        .line-code {
            flex: 1;
            white-space: pre;
            padding-right: 1rem;
        }
        .covered { background: var(--covered); }
        .uncovered { background: var(--uncovered); }
        a { color: #60a5fa; }
    </style>
</head>
<body>
    <div class="header">
        <a href="index.html">← العودة</a>
        <h2>)" << coverage.relativePath << R"(</h2>
        <p>التغطية: )" << std::fixed << std::setprecision(1) << coverage.lineRate() << R"(%</p>
    </div>
    <div class="code">
)";
        
        for (const auto& line : coverage.lines) {
            std::string cls = "";
            if (line.executable) {
                cls = line.hitCount > 0 ? "covered" : "uncovered";
            }
            
            // تحويل HTML entities
            std::string escaped = line.source;
            size_t pos = 0;
            while ((pos = escaped.find('&', pos)) != std::string::npos) {
                escaped.replace(pos, 1, "&amp;");
                pos += 5;
            }
            pos = 0;
            while ((pos = escaped.find('<', pos)) != std::string::npos) {
                escaped.replace(pos, 1, "&lt;");
                pos += 4;
            }
            pos = 0;
            while ((pos = escaped.find('>', pos)) != std::string::npos) {
                escaped.replace(pos, 1, "&gt;");
                pos += 4;
            }
            
            file << R"(        <div class="line )" << cls << R"(">
            <span class="line-num">)" << line.lineNumber << R"(</span>
            <span class="line-hits">)" << (line.executable ? std::to_string(line.hitCount) : "") << R"(</span>
            <span class="line-code">)" << escaped << R"(</span>
        </div>
)";
        }
        
        file << R"(    </div>
</body>
</html>
)";
    }
};

// =============================================================================
// ماكرو التتبع (يُضاف تلقائياً بواسطة المترجم)
// =============================================================================

#define SAD_COV_LINE(file, line) \
    sad::test::CoverageTracker::instance().recordLine(file, line)

#define SAD_COV_FUNC(file, name) \
    sad::test::CoverageTracker::instance().recordFunction(file, name)

#define SAD_COV_BRANCH(file, line, idx) \
    sad::test::CoverageTracker::instance().recordBranch(file, line, idx)

// =============================================================================
// دوال مساعدة
// =============================================================================

/**
 * طباعة ملخص التغطية
 */
inline void اطبع_التغطية() {
    CoverageTracker::instance().printSummary();
}

/**
 * توليد تقرير HTML
 */
inline void تقرير_تغطية_html(const std::string& مجلد = "coverage") {
    CoverageTracker::instance().generateHtmlReport(مجلد);
}

/**
 * نسبة التغطية
 */
inline double نسبة_التغطية() {
    return CoverageTracker::instance().getStats().lineRate();
}

/**
 * إعادة تعيين
 */
inline void امسح_التغطية() {
    CoverageTracker::instance().reset();
}

} // namespace test
} // namespace sad

#endif // SAD_STDLIB_TEST_COVERAGE_H
