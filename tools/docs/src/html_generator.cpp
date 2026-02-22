/**
 * =============================================================================
 * ملف: html_generator.cpp
 * الوصف: مولد صفحات HTML للتوثيق
 * المهمة: T230 - HTML generator
 * المرحلة: Phase 23 - User Story 20 (Documentation System ص وثّق)
 * =============================================================================
 * 
 * 🌐 دليل المبتدئ لصفحات التوثيق
 * ══════════════════════════════════
 * 
 * ما هو HTML؟
 * ────────────
 * HTML = HyperText Markup Language
 * لغة صفحات الويب! كل موقع تراه مبني بـ HTML.
 * 
 * لماذا نولد HTML للتوثيق؟
 * ──────────────────────────
 * 1. يمكن فتحه في أي متصفح
 * 2. يدعم التنسيق والألوان
 * 3. يمكن البحث والتنقل
 * 4. يعمل بدون إنترنت
 * 
 * بنية موقع التوثيق:
 * ────────────────────
 * 
 * docs/
 * ├── index.html          # الصفحة الرئيسية
 * ├── api/
 * │   ├── functions.html  # قائمة الدوال
 * │   ├── classes.html    # قائمة الأصناف
 * │   └── modules.html    # قائمة الوحدات
 * ├── guides/
 * │   ├── getting-started.html
 * │   └── tutorials/
 * ├── search.html         # صفحة البحث
 * └── assets/
 *     ├── style.css
 *     └── script.js
 * 
 * ميزات صفحات التوثيق:
 * ──────────────────────
 * ✓ قائمة جانبية قابلة للطي
 * ✓ بحث فوري
 * ✓ تلوين الكود
 * ✓ روابط بين الصفحات
 * ✓ نسخ الكود بنقرة
 * ✓ وضع داكن/فاتح
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

namespace sad::docs {

// Forward declaration
struct ExtractedDoc;
enum class ASTNodeType;

// =============================================================================
// خيارات التوليد
// =============================================================================

struct HTMLGeneratorOptions {
    std::string title = "توثيق لغة ص";
    std::string description = "التوثيق الرسمي للغة ص البرمجية";
    std::string version = "1.0.0";
    std::string baseUrl = "";
    std::string theme = "dark";
    bool singlePage = false;
    bool includePrivate = false;
    bool searchEnabled = true;
    std::string logoUrl = "";
    std::string customCSS = "";
    std::vector<std::pair<std::string, std::string>> navLinks; // name, url
};

// =============================================================================
// عنصر توثيق مبسط (لتجنب الاعتماد الدائري)
// =============================================================================

struct SimpleDocItem {
    std::string name;
    std::string fullPath;
    std::string type;  // function, class, etc.
    std::string signature;
    std::string summary;
    std::string description;
    std::string returnType;
    std::string module;
    std::string file;
    int line = 0;
    std::vector<std::pair<std::string, std::string>> parameters; // name, type
    std::map<std::string, std::string> paramDocs;
    std::string returnDoc;
    std::vector<std::string> examples;
    std::vector<std::string> seeAlso;
    std::string since;
    std::string deprecated;
};

// =============================================================================
// مولد HTML
// =============================================================================

class HTMLGenerator {
public:
    HTMLGenerator(const HTMLGeneratorOptions& options = {})
        : options_(options) {}
    
    /**
     * توليد موقع التوثيق الكامل
     */
    void generateSite(
        const std::vector<SimpleDocItem>& docs,
        const std::string& outputDir
    ) {
        // إنشاء المجلدات
        fs::create_directories(outputDir);
        fs::create_directories(outputDir + "/api");
        fs::create_directories(outputDir + "/assets");
        
        // توليد الملفات
        generateIndexPage(docs, outputDir);
        generateAPIPages(docs, outputDir);
        generateSearchData(docs, outputDir);
        generateAssets(outputDir);
    }
    
    /**
     * توليد صفحة واحدة
     */
    std::string generatePage(const SimpleDocItem& doc) {
        std::ostringstream ss;
        
        ss << generateHeader(doc.name);
        ss << "<div class=\"content\">\n";
        ss << generateDocContent(doc);
        ss << "</div>\n";
        ss << generateFooter();
        
        return ss.str();
    }
    
    /**
     * توليد صفحة فهرس
     */
    std::string generateIndexHTML(const std::vector<SimpleDocItem>& docs) {
        std::ostringstream ss;
        
        ss << generateHeader(options_.title);
        
        ss << "<div class=\"hero\">\n";
        ss << "  <h1>🔤 " << options_.title << "</h1>\n";
        ss << "  <p>" << options_.description << "</p>\n";
        ss << "  <p class=\"version\">النسخة " << options_.version << "</p>\n";
        ss << "</div>\n";
        
        // إحصائيات
        int funcCount = 0, classCount = 0, moduleCount = 0;
        for (const auto& doc : docs) {
            if (doc.type == "function") funcCount++;
            else if (doc.type == "class") classCount++;
            else if (doc.type == "module") moduleCount++;
        }
        
        ss << "<div class=\"stats\">\n";
        ss << "  <div class=\"stat\"><span class=\"number\">" << funcCount << "</span><br>دالة</div>\n";
        ss << "  <div class=\"stat\"><span class=\"number\">" << classCount << "</span><br>صنف</div>\n";
        ss << "  <div class=\"stat\"><span class=\"number\">" << moduleCount << "</span><br>وحدة</div>\n";
        ss << "</div>\n";
        
        // روابط سريعة
        ss << "<div class=\"quick-links\">\n";
        ss << "  <h2>🚀 ابدأ من هنا</h2>\n";
        ss << "  <div class=\"links-grid\">\n";
        ss << "    <a href=\"api/functions.html\" class=\"link-card\">\n";
        ss << "      <span class=\"icon\">📝</span>\n";
        ss << "      <span class=\"title\">الدوال</span>\n";
        ss << "      <span class=\"desc\">جميع الدوال المتاحة</span>\n";
        ss << "    </a>\n";
        ss << "    <a href=\"api/classes.html\" class=\"link-card\">\n";
        ss << "      <span class=\"icon\">🏛️</span>\n";
        ss << "      <span class=\"title\">الأصناف</span>\n";
        ss << "      <span class=\"desc\">الأصناف والهياكل</span>\n";
        ss << "    </a>\n";
        ss << "    <a href=\"search.html\" class=\"link-card\">\n";
        ss << "      <span class=\"icon\">🔍</span>\n";
        ss << "      <span class=\"title\">البحث</span>\n";
        ss << "      <span class=\"desc\">بحث في التوثيق</span>\n";
        ss << "    </a>\n";
        ss << "  </div>\n";
        ss << "</div>\n";
        
        // أحدث الإضافات
        ss << "<div class=\"recent\">\n";
        ss << "  <h2>📌 أحدث الإضافات</h2>\n";
        ss << "  <ul>\n";
        int count = 0;
        for (const auto& doc : docs) {
            if (count++ >= 5) break;
            ss << "    <li><a href=\"api/" << doc.name << ".html\">" 
               << doc.name << "</a> - " << doc.summary << "</li>\n";
        }
        ss << "  </ul>\n";
        ss << "</div>\n";
        
        ss << generateFooter();
        
        return ss.str();
    }
    
private:
    HTMLGeneratorOptions options_;
    
    /**
     * توليد رأس الصفحة
     */
    std::string generateHeader(const std::string& pageTitle) {
        std::ostringstream ss;
        
        ss << R"(<!DOCTYPE html>
<html dir="rtl" lang="ar">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>)" << pageTitle << " - " << options_.title << R"(</title>
    <link rel="stylesheet" href="assets/style.css">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.8.0/styles/atom-one-dark.min.css">
    <script src="https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.8.0/highlight.min.js"></script>
</head>
<body>
    <nav class="navbar">
        <div class="nav-brand">
            <a href="index.html">🔤 )" << options_.title << R"(</a>
        </div>
        <div class="nav-links">
            <a href="api/functions.html">الدوال</a>
            <a href="api/classes.html">الأصناف</a>
            <a href="search.html">🔍 بحث</a>
        </div>
    </nav>
    
    <div class="container">
)";
        
        return ss.str();
    }
    
    /**
     * توليد ذيل الصفحة
     */
    std::string generateFooter() {
        std::ostringstream ss;
        
        ss << R"(
    </div>
    
    <footer>
        <p>توثيق )" << options_.title << R"( - النسخة )" << options_.version << R"(</p>
        <p>تم التوليد بواسطة ص وثّق</p>
    </footer>
    
    <script>
        hljs.highlightAll();
        
        // نسخ الكود
        document.querySelectorAll('pre code').forEach((block) => {
            const btn = document.createElement('button');
            btn.className = 'copy-btn';
            btn.textContent = 'نسخ';
            btn.onclick = () => {
                navigator.clipboard.writeText(block.textContent);
                btn.textContent = 'تم النسخ!';
                setTimeout(() => btn.textContent = 'نسخ', 2000);
            };
            block.parentNode.insertBefore(btn, block);
        });
    </script>
</body>
</html>
)";
        
        return ss.str();
    }
    
    /**
     * توليد محتوى التوثيق
     */
    std::string generateDocContent(const SimpleDocItem& doc) {
        std::ostringstream ss;
        
        // العنوان
        ss << "<div class=\"doc-header\">\n";
        ss << "  <span class=\"doc-type\">" << typeToArabic(doc.type) << "</span>\n";
        ss << "  <h1>" << doc.name << "</h1>\n";
        ss << "</div>\n";
        
        // مهمل
        if (!doc.deprecated.empty()) {
            ss << "<div class=\"alert alert-warning\">\n";
            ss << "  ⚠️ <strong>مهمل:</strong> " << doc.deprecated << "\n";
            ss << "</div>\n";
        }
        
        // التوقيع
        ss << "<div class=\"signature\">\n";
        ss << "  <pre><code class=\"language-sad\">" << escapeHTML(doc.signature) << "</code></pre>\n";
        ss << "</div>\n";
        
        // الملخص
        if (!doc.summary.empty()) {
            ss << "<div class=\"summary\">\n";
            ss << "  <p>" << doc.summary << "</p>\n";
            ss << "</div>\n";
        }
        
        // الوصف
        if (!doc.description.empty()) {
            ss << "<div class=\"description\">\n";
            ss << "  " << doc.description << "\n";
            ss << "</div>\n";
        }
        
        // المعطيات
        if (!doc.parameters.empty()) {
            ss << "<div class=\"section\">\n";
            ss << "  <h2>📥 المعطيات</h2>\n";
            ss << "  <table class=\"params-table\">\n";
            ss << "    <thead><tr><th>الاسم</th><th>النوع</th><th>الوصف</th></tr></thead>\n";
            ss << "    <tbody>\n";
            
            for (const auto& [name, type] : doc.parameters) {
                ss << "      <tr>\n";
                ss << "        <td><code>" << name << "</code></td>\n";
                ss << "        <td><code>" << type << "</code></td>\n";
                ss << "        <td>";
                auto it = doc.paramDocs.find(name);
                if (it != doc.paramDocs.end()) {
                    ss << it->second;
                }
                ss << "</td>\n";
                ss << "      </tr>\n";
            }
            
            ss << "    </tbody>\n";
            ss << "  </table>\n";
            ss << "</div>\n";
        }
        
        // القيمة المرجعة
        if (!doc.returnType.empty() || !doc.returnDoc.empty()) {
            ss << "<div class=\"section\">\n";
            ss << "  <h2>📤 القيمة المرجعة</h2>\n";
            if (!doc.returnType.empty()) {
                ss << "  <p><strong>النوع:</strong> <code>" << doc.returnType << "</code></p>\n";
            }
            if (!doc.returnDoc.empty()) {
                ss << "  <p>" << doc.returnDoc << "</p>\n";
            }
            ss << "</div>\n";
        }
        
        // الأمثلة
        if (!doc.examples.empty()) {
            ss << "<div class=\"section\">\n";
            ss << "  <h2>💡 أمثلة</h2>\n";
            for (const auto& ex : doc.examples) {
                ss << "  <pre><code class=\"language-sad\">" << escapeHTML(ex) << "</code></pre>\n";
            }
            ss << "</div>\n";
        }
        
        // انظر أيضاً
        if (!doc.seeAlso.empty()) {
            ss << "<div class=\"section\">\n";
            ss << "  <h2>🔗 انظر أيضاً</h2>\n";
            ss << "  <ul>\n";
            for (const auto& ref : doc.seeAlso) {
                ss << "    <li><a href=\"" << ref << ".html\">" << ref << "</a></li>\n";
            }
            ss << "  </ul>\n";
            ss << "</div>\n";
        }
        
        // معلومات الملف
        ss << "<div class=\"file-info\">\n";
        ss << "  <p>📄 <code>" << doc.file << ":" << doc.line << "</code></p>\n";
        if (!doc.since.empty()) {
            ss << "  <p>📅 منذ نسخة: " << doc.since << "</p>\n";
        }
        ss << "</div>\n";
        
        return ss.str();
    }
    
    /**
     * توليد صفحة الفهرس الرئيسية
     */
    void generateIndexPage(
        const std::vector<SimpleDocItem>& docs,
        const std::string& outputDir
    ) {
        std::string html = generateIndexHTML(docs);
        std::ofstream f(outputDir + "/index.html");
        f << html;
    }
    
    /**
     * توليد صفحات API
     */
    void generateAPIPages(
        const std::vector<SimpleDocItem>& docs,
        const std::string& outputDir
    ) {
        // صفحة لكل عنصر
        for (const auto& doc : docs) {
            std::string html = generatePage(doc);
            std::ofstream f(outputDir + "/api/" + doc.name + ".html");
            f << html;
        }
        
        // قائمة الدوال
        generateListPage(docs, "function", "الدوال", outputDir + "/api/functions.html");
        
        // قائمة الأصناف
        generateListPage(docs, "class", "الأصناف", outputDir + "/api/classes.html");
    }
    
    /**
     * توليد صفحة قائمة
     */
    void generateListPage(
        const std::vector<SimpleDocItem>& docs,
        const std::string& filterType,
        const std::string& title,
        const std::string& outputPath
    ) {
        std::ostringstream ss;
        
        ss << generateHeader(title);
        
        ss << "<h1>" << title << "</h1>\n";
        ss << "<div class=\"list-view\">\n";
        
        for (const auto& doc : docs) {
            if (doc.type == filterType) {
                ss << "<div class=\"list-item\">\n";
                ss << "  <a href=\"" << doc.name << ".html\" class=\"item-name\">" << doc.name << "</a>\n";
                ss << "  <p class=\"item-summary\">" << doc.summary << "</p>\n";
                ss << "  <code class=\"item-sig\">" << doc.signature << "</code>\n";
                ss << "</div>\n";
            }
        }
        
        ss << "</div>\n";
        ss << generateFooter();
        
        std::ofstream f(outputPath);
        f << ss.str();
    }
    
    /**
     * توليد بيانات البحث
     */
    void generateSearchData(
        const std::vector<SimpleDocItem>& docs,
        const std::string& outputDir
    ) {
        std::ostringstream json;
        json << "[\n";
        
        bool first = true;
        for (const auto& doc : docs) {
            if (!first) json << ",\n";
            first = false;
            
            json << "  {\"name\":\"" << escapeJSON(doc.name) << "\",";
            json << "\"type\":\"" << doc.type << "\",";
            json << "\"summary\":\"" << escapeJSON(doc.summary) << "\",";
            json << "\"url\":\"api/" << doc.name << ".html\"}";
        }
        
        json << "\n]\n";
        
        std::ofstream f(outputDir + "/assets/search-data.json");
        f << json.str();
        
        // صفحة البحث
        generateSearchPage(outputDir);
    }
    
    /**
     * توليد صفحة البحث
     */
    void generateSearchPage(const std::string& outputDir) {
        std::ostringstream ss;
        
        ss << generateHeader("البحث");
        
        ss << R"(
<div class="search-container">
    <h1>🔍 البحث في التوثيق</h1>
    <input type="text" id="searchInput" placeholder="ابحث عن دالة، صنف، أو موضوع..." autofocus>
    <div id="searchResults"></div>
</div>

<script>
let searchData = [];

fetch('assets/search-data.json')
    .then(r => r.json())
    .then(data => searchData = data);

document.getElementById('searchInput').addEventListener('input', function() {
    const query = this.value.toLowerCase();
    const results = document.getElementById('searchResults');
    
    if (query.length < 2) {
        results.innerHTML = '';
        return;
    }
    
    const matches = searchData.filter(item => 
        item.name.toLowerCase().includes(query) ||
        item.summary.toLowerCase().includes(query)
    ).slice(0, 20);
    
    results.innerHTML = matches.map(item => `
        <div class="search-result">
            <a href="${item.url}">
                <span class="result-type">${item.type}</span>
                <span class="result-name">${item.name}</span>
            </a>
            <p class="result-summary">${item.summary}</p>
        </div>
    `).join('');
});
</script>
)";
        
        ss << generateFooter();
        
        std::ofstream f(outputDir + "/search.html");
        f << ss.str();
    }
    
    /**
     * توليد الأصول (CSS, JS)
     */
    void generateAssets(const std::string& outputDir) {
        // CSS
        std::string css = R"(
:root {
    --bg-primary: #1a1a2e;
    --bg-secondary: #16213e;
    --bg-code: #0f0f23;
    --text-primary: #eee;
    --text-secondary: #aaa;
    --accent: #4dabf7;
    --accent-hover: #69b5ff;
    --success: #69db7c;
    --warning: #ffd43b;
    --danger: #ff6b6b;
    --border: #333;
}

* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: 'Segoe UI', Tahoma, Arial, sans-serif;
    background: var(--bg-primary);
    color: var(--text-primary);
    line-height: 1.8;
}

.navbar {
    background: var(--bg-secondary);
    padding: 15px 30px;
    display: flex;
    justify-content: space-between;
    align-items: center;
    position: sticky;
    top: 0;
    z-index: 100;
    box-shadow: 0 2px 10px rgba(0,0,0,0.3);
}

.nav-brand a {
    font-size: 1.3em;
    font-weight: bold;
    color: var(--text-primary);
    text-decoration: none;
}

.nav-links a {
    color: var(--text-secondary);
    text-decoration: none;
    margin-right: 20px;
    transition: color 0.3s;
}

.nav-links a:hover {
    color: var(--accent);
}

.container {
    max-width: 1200px;
    margin: 0 auto;
    padding: 40px 20px;
}

.hero {
    text-align: center;
    padding: 60px 20px;
    background: linear-gradient(135deg, var(--bg-secondary), var(--bg-code));
    border-radius: 20px;
    margin-bottom: 40px;
}

.hero h1 {
    font-size: 3em;
    margin-bottom: 15px;
}

.hero p {
    font-size: 1.2em;
    color: var(--text-secondary);
}

.hero .version {
    margin-top: 20px;
    color: var(--accent);
}

.stats {
    display: flex;
    justify-content: center;
    gap: 40px;
    margin-bottom: 40px;
}

.stat {
    text-align: center;
    padding: 20px;
    background: var(--bg-secondary);
    border-radius: 15px;
    min-width: 120px;
}

.stat .number {
    font-size: 2.5em;
    font-weight: bold;
    color: var(--accent);
}

.quick-links h2 {
    margin-bottom: 20px;
}

.links-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
    gap: 20px;
}

.link-card {
    background: var(--bg-secondary);
    padding: 25px;
    border-radius: 15px;
    text-decoration: none;
    color: var(--text-primary);
    transition: transform 0.3s, box-shadow 0.3s;
    display: block;
}

.link-card:hover {
    transform: translateY(-5px);
    box-shadow: 0 10px 30px rgba(0,0,0,0.3);
}

.link-card .icon {
    font-size: 2em;
    display: block;
    margin-bottom: 10px;
}

.link-card .title {
    font-size: 1.3em;
    font-weight: bold;
    display: block;
    margin-bottom: 5px;
}

.link-card .desc {
    color: var(--text-secondary);
    font-size: 0.9em;
}

.doc-header {
    margin-bottom: 30px;
}

.doc-type {
    background: var(--accent);
    color: var(--bg-primary);
    padding: 5px 15px;
    border-radius: 20px;
    font-size: 0.9em;
    display: inline-block;
    margin-bottom: 10px;
}

.doc-header h1 {
    font-size: 2.5em;
}

.signature {
    background: var(--bg-code);
    border-radius: 10px;
    padding: 20px;
    margin-bottom: 30px;
}

.signature pre {
    margin: 0;
}

.section {
    margin-bottom: 40px;
}

.section h2 {
    font-size: 1.5em;
    margin-bottom: 15px;
    color: var(--accent);
}

.params-table {
    width: 100%;
    border-collapse: collapse;
}

.params-table th,
.params-table td {
    padding: 12px;
    text-align: right;
    border-bottom: 1px solid var(--border);
}

.params-table th {
    background: var(--bg-secondary);
}

.alert {
    padding: 15px 20px;
    border-radius: 10px;
    margin-bottom: 20px;
}

.alert-warning {
    background: rgba(255, 212, 59, 0.1);
    border-right: 4px solid var(--warning);
}

pre {
    background: var(--bg-code);
    padding: 15px;
    border-radius: 10px;
    overflow-x: auto;
    position: relative;
}

.copy-btn {
    position: absolute;
    top: 10px;
    left: 10px;
    background: var(--accent);
    color: var(--bg-primary);
    border: none;
    padding: 5px 10px;
    border-radius: 5px;
    cursor: pointer;
    font-size: 0.8em;
}

code {
    font-family: 'Consolas', 'Monaco', monospace;
}

.file-info {
    margin-top: 40px;
    padding-top: 20px;
    border-top: 1px solid var(--border);
    color: var(--text-secondary);
    font-size: 0.9em;
}

.list-view {
    display: flex;
    flex-direction: column;
    gap: 15px;
}

.list-item {
    background: var(--bg-secondary);
    padding: 20px;
    border-radius: 10px;
}

.item-name {
    font-size: 1.3em;
    font-weight: bold;
    color: var(--accent);
    text-decoration: none;
}

.item-summary {
    color: var(--text-secondary);
    margin: 10px 0;
}

.item-sig {
    font-size: 0.9em;
    color: var(--text-secondary);
}

.search-container {
    max-width: 800px;
    margin: 0 auto;
}

#searchInput {
    width: 100%;
    padding: 15px 20px;
    font-size: 1.2em;
    border: none;
    border-radius: 10px;
    background: var(--bg-secondary);
    color: var(--text-primary);
    margin-bottom: 30px;
}

.search-result {
    background: var(--bg-secondary);
    padding: 15px;
    border-radius: 10px;
    margin-bottom: 10px;
}

.search-result a {
    text-decoration: none;
}

.result-type {
    background: var(--bg-code);
    padding: 2px 8px;
    border-radius: 5px;
    font-size: 0.8em;
    margin-left: 10px;
}

.result-name {
    color: var(--accent);
    font-weight: bold;
}

.result-summary {
    color: var(--text-secondary);
    margin-top: 5px;
    font-size: 0.9em;
}

footer {
    text-align: center;
    padding: 30px;
    color: var(--text-secondary);
    border-top: 1px solid var(--border);
    margin-top: 40px;
}

@media (max-width: 768px) {
    .navbar {
        flex-direction: column;
        gap: 15px;
    }
    
    .stats {
        flex-direction: column;
        align-items: center;
    }
    
    .hero h1 {
        font-size: 2em;
    }
}
)";
        
        std::ofstream cssFile(outputDir + "/assets/style.css");
        cssFile << css;
    }
    
    /**
     * نوع العنصر بالعربي
     */
    std::string typeToArabic(const std::string& type) const {
        if (type == "function") return "دالة";
        if (type == "class") return "صنف";
        if (type == "struct") return "هيكل";
        if (type == "enum") return "تعداد";
        if (type == "constant") return "ثابت";
        if (type == "variable") return "متغير";
        if (type == "module") return "وحدة";
        return type;
    }
    
    /**
     * هروب HTML
     */
    std::string escapeHTML(const std::string& s) const {
        std::string result;
        for (char c : s) {
            switch (c) {
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                case '&': result += "&amp;"; break;
                case '"': result += "&quot;"; break;
                default: result += c;
            }
        }
        return result;
    }
    
    /**
     * هروب JSON
     */
    std::string escapeJSON(const std::string& s) const {
        std::string result;
        for (char c : s) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += " "; break;
                default: result += c;
            }
        }
        return result;
    }
};

} // namespace sad::docs

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadHTMLGenerator {
    sad::docs::HTMLGenerator* generator;
};

SadHTMLGenerator* sad_html_generator_new(const char* title) {
    sad::docs::HTMLGeneratorOptions options;
    options.title = title;
    
    auto* ctx = new SadHTMLGenerator();
    ctx->generator = new sad::docs::HTMLGenerator(options);
    return ctx;
}

void sad_html_generator_free(SadHTMLGenerator* ctx) {
    if (ctx) {
        delete ctx->generator;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef HTML_GENERATOR_TEST

#include <cassert>

void testBasicGeneration() {
    sad::docs::HTMLGeneratorOptions options;
    options.title = "اختبار التوثيق";
    
    sad::docs::HTMLGenerator generator(options);
    
    sad::docs::SimpleDocItem doc;
    doc.name = "جمع";
    doc.type = "function";
    doc.signature = "دالة جمع(أ: عدد, ب: عدد) -> عدد";
    doc.summary = "تجمع عددين";
    doc.parameters = {{"أ", "عدد"}, {"ب", "عدد"}};
    doc.returnType = "عدد";
    
    std::string html = generator.generatePage(doc);
    
    assert(html.find("جمع") != std::string::npos);
    assert(html.find("<html") != std::string::npos);
    
    std::cout << "✅ اختبار التوليد الأساسي نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات مولد HTML\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testBasicGeneration();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // HTML_GENERATOR_TEST
