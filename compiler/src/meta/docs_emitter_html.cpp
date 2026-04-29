// ============================================================================
// docs_emitter_html.cpp — توليد HTML والتقارير + واجهة الإصدار C
// (AR) مولد التوثيق بصيغة HTML، واجهة الإصدار C
// (EN) HTML documentation generator, C emission interface, DocsEmitter class
// تم استخراج هذا الملف من docs_emitter.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include "meta/docs_emitter.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <regex>
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int

namespace fs = std::filesystem;

namespace Sad {
namespace Meta {
int HtmlDocGenerator::generate(
    const std::vector<DocEntry>& entries, DocsResult& result) {

    fs::create_directories(config_.outputDir);
    fs::create_directories(config_.outputDir + "/api");
    fs::create_directories(config_.outputDir + "/assets");

    std::map<std::string, const DocEntry*> nameIdx;
    for (const auto& e : entries) {
        nameIdx[e.name] = &e;
        if (!e.qualifiedName.empty() && e.qualifiedName != e.name) {
            nameIdx[e.qualifiedName] = &e;
        }
    }

    generateIndexPage(entries);
    result.pagesGenerated++;
    result.generatedFiles.push_back(config_.outputDir + "/index.html");

    for (const auto& entry : entries) {
        generateItemPage(entry, nameIdx);
        result.pagesGenerated++;
        result.generatedFiles.push_back(config_.outputDir + "/api/" + entry.id + ".html");
    }

    struct KindInfo { DocItemKind kind; std::string title; };
    std::vector<KindInfo> kinds = {
        {DocItemKind::Function, "״§„״¯ˆ״§„"}, {DocItemKind::Class, "״§„״£״µ†״§"},
        {DocItemKind::Struct, "״§„‡״§ƒ„"}, {DocItemKind::Enum, "״§„״×״¹״¯״§״¯״§״×"},
        {DocItemKind::Trait, "״§„״³…״§״×"}, {DocItemKind::Interface, "״§„ˆ״§״¬‡״§״×"},
        {DocItemKind::Constant, "״§„״«ˆ״§״¨״×"}, {DocItemKind::Module, "״§„ˆ״­״¯״§״×"},
    };
    for (const auto& ki : kinds) {
        std::vector<DocEntry> filtered;
        for (const auto& e : entries) {
            if (e.kind == ki.kind) filtered.push_back(e);
        }
        if (!filtered.empty()) {
            generateListPage(filtered, ki.kind, ki.title);
            result.pagesGenerated++;
        }
    }

    if (config_.searchEnabled) {
        generateSearchIndex(entries);
        generateSearchScript();
    }
    generateStylesheet();
    result.itemsDocumented = Sad::Security::SafeArithmetic::assertSafeCast<int>(entries.size(), "docs_emitter_html_size");
    return 0;
}

void HtmlDocGenerator::generateIndexPage(const std::vector<DocEntry>& entries) {
    std::ofstream f(config_.outputDir + "/index.html");
    f << "<!DOCTYPE html>\n<html dir=\"rtl\" lang=\"ar\">\n<head>\n"
      << "  <meta charset=\"UTF-8\">\n"
      << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
      << "  <title>" << escapeHtml(config_.title) << "</title>\n"
      << "  <link rel=\"stylesheet\" href=\"assets/style.css\">\n"
      << "</head>\n<body class=\"theme-" << config_.theme << "\">\n"
      << "<header class=\"site-header\">\n"
      << "  <h1>" << escapeHtml(config_.title) << "</h1>\n";
    if (!config_.description.empty())
        f << "  <p class=\"subtitle\">" << escapeHtml(config_.description) << "</p>\n";
    if (!config_.version.empty())
        f << "  <span class=\"version\">״§„״¥״µ״¯״§״± " << escapeHtml(config_.version) << "</span>\n";
    f << "</header>\n\n";

    if (config_.searchEnabled) {
        f << "<div class=\"search-container\">\n"
          << "  <input type=\"text\" id=\"search-input\" placeholder=\"״§״¨״­״«  ״§„״×ˆ״«‚...\">\n"
          << "  <div id=\"search-results\"></div>\n</div>\n\n";
    }

    // ״§„״¥״­״µ״§״¦״§״×
    std::map<DocItemKind, int> counts;
    for (const auto& e : entries) counts[e.kind]++;
    f << "<div class=\"stats-grid\">\n";
    struct SI { DocItemKind kind; std::string icon; std::string label; };
    for (auto& s : std::vector<SI>{
        {DocItemKind::Function,"ג¡","״¯ˆ״§„"},{DocItemKind::Class,"נ—ן¸","״£״µ†״§"},
        {DocItemKind::Struct,"נ“¦","‡״§ƒ„"},{DocItemKind::Constant,"נ”’","״«ˆ״§״¨״×"}}) {
        if (counts[s.kind] > 0) {
            f << "  <div class=\"stat-card\"><span class=\"stat-icon\">" << s.icon
              << "</span><span class=\"stat-number\">" << counts[s.kind]
              << "</span><span class=\"stat-label\">" << s.label << "</span></div>\n";
        }
    }
    f << "</div>\n\n";

    // ״§„״£‚״³״§…
    struct SD { DocItemKind kind; std::string title; std::string slug; };
    for (auto& sec : std::vector<SD>{
        {DocItemKind::Module,"״§„ˆ״­״¯״§״×","modules"},{DocItemKind::Class,"״§„״£״µ†״§","classes"},
        {DocItemKind::Function,"״§„״¯ˆ״§„","functions"},{DocItemKind::Struct,"״§„‡״§ƒ„","structs"},
        {DocItemKind::Constant,"״§„״«ˆ״§״¨״×","constants"}}) {
        bool has = false;
        for (const auto& e : entries) if (e.kind == sec.kind) { has = true; break; }
        if (!has) continue;
        f << "<section class=\"doc-section\">\n<h2 id=\"" << sec.slug << "\">"
          << sec.title << "</h2>\n<div class=\"item-grid\">\n";
        for (const auto& e : entries) {
            if (e.kind != sec.kind) continue;
            f << "  <div class=\"item-card\"><a href=\"api/" << e.id << ".html\">"
              << kindIcon(e.kind) << " " << escapeHtml(e.name) << "</a>";
            if (!e.summary.empty()) f << "<p class=\"item-summary\">" << escapeHtml(e.summary) << "</p>";
            f << "</div>\n";
        }
        f << "</div>\n</section>\n\n";
    }

    f << "<footer><p>״×… ״§„״×ˆ„״¯ ״¨ˆ״§״³״·״© <strong>״µ ˆ״«‘‚</strong></p></footer>\n";
    if (config_.searchEnabled) f << "<script src=\"assets/search.js\"></script>\n";
    f << "</body>\n</html>\n";
}

void HtmlDocGenerator::generateItemPage(
    const DocEntry& entry, const std::map<std::string, const DocEntry*>& index) {
    std::ofstream f(config_.outputDir + "/api/" + entry.id + ".html");
    f << "<!DOCTYPE html>\n<html dir=\"rtl\" lang=\"ar\">\n<head>\n"
      << "  <meta charset=\"UTF-8\">\n"
      << "  <title>" << escapeHtml(entry.name) << " ג€” " << escapeHtml(config_.title) << "</title>\n"
      << "  <link rel=\"stylesheet\" href=\"../assets/style.css\">\n"
      << "</head>\n<body class=\"theme-" << config_.theme << "\">\n"
      << "<nav class=\"breadcrumb\"><a href=\"../index.html\">נ  ״§„״±״¦״³״©</a> ג€÷ "
      << escapeHtml(entry.name) << "</nav>\n\n"
      << "<header class=\"item-header\">\n"
      << "  <span class=\"kind-badge\">" << kindIcon(entry.kind) << " "
      << entry.kindNameAr() << "</span> " << accessBadge(entry.access) << "\n";
    if (entry.isDeprecated()) f << "  <span class=\"deprecated-badge\">ג ן¸ …‡…„</span>\n";
    f << "  <h1>" << escapeHtml(entry.name) << "</h1>\n</header>\n\n"
      << "<div class=\"signature\"><pre><code>" << highlightSadCode(entry.signature)
      << "</code></pre></div>\n\n";

    if (entry.isDeprecated())
        f << "<div class=\"admonition warning\"><strong>ג ן¸ …‡…„:</strong> "
          << escapeHtml(entry.deprecated) << "</div>\n\n";
    if (!entry.summary.empty())
        f << "<div class=\"summary\">" << escapeHtml(entry.summary) << "</div>\n\n";
    if (!entry.description.empty())
        f << "<div class=\"description\">" << renderMarkdownToHtml(entry.description) << "</div>\n\n";

    for (const auto& n : entry.notes)
        f << "<div class=\"admonition note\"><strong>נ“ …„״§״­״¸״©:</strong> " << escapeHtml(n) << "</div>\n";
    for (const auto& w : entry.warnings)
        f << "<div class=\"admonition danger\"><strong>נ”´ ״×״­״°״±:</strong> " << escapeHtml(w) << "</div>\n";

    if (!entry.params.empty()) {
        f << "<section><h2>״§„…״¹״·״§״×</h2><table class=\"params-table\">\n"
          << "<thead><tr><th>״§„״§״³…</th><th>״§„†ˆ״¹</th><th>״§„ˆ״µ</th></tr></thead><tbody>\n";
        for (const auto& p : entry.params) {
            f << "<tr><td><code>" << escapeHtml(p.name) << "</code></td><td><code>"
              << escapeHtml(p.type) << "</code></td><td>" << escapeHtml(p.description) << "</td></tr>\n";
        }
        f << "</tbody></table></section>\n\n";
    }

    if (!entry.returnType.empty()) {
        f << "<section><h2>״§„‚…״© ״§„…״±״¬״¹״©</h2><p><code>" << escapeHtml(entry.returnType) << "</code></p>\n";
        if (!entry.returnDescription.empty())
            f << "<p>" << escapeHtml(entry.returnDescription) << "</p>\n";
        f << "</section>\n\n";
    }

    if (!entry.examples.empty()) {
        f << "<section><h2>״£…״«„״©</h2>\n";
        for (const auto& ex : entry.examples)
            f << "<pre><code class=\"lang-sad\">" << highlightSadCode(ex) << "</code></pre>\n";
        f << "</section>\n\n";
    }

    if (!entry.seeAlso.empty()) {
        f << "<section><h2>״§†״¸״± ״£״¶״§‹</h2><ul>\n";
        for (const auto& r : entry.seeAlso) {
            auto it = index.find(r);
            if (it != index.end())
                f << "<li><a href=\"" << it->second->id << ".html\">" << escapeHtml(r) << "</a></li>\n";
            else
                f << "<li>" << escapeHtml(r) << "</li>\n";
        }
        f << "</ul></section>\n\n";
    }

    f << "<footer class=\"source-info\">\n";
    if (!entry.sourceFile.empty())
        f << "  <p>נ“ " << escapeHtml(entry.sourceFile);
    if (entry.lineNumber > 0) f << " ג€” ״§„״³״·״± " << entry.lineNumber;
    f << "</p>\n";
    if (!entry.since.empty()) f << "  <p>נ“… …†״° " << escapeHtml(entry.since) << "</p>\n";
    f << "</footer>\n</body>\n</html>\n";
}

void HtmlDocGenerator::generateListPage(
    const std::vector<DocEntry>& entries, DocItemKind kind, const std::string& title) {
    std::string slug = entries.empty() ? "items" : entries[0].kindNameEn() + "s";
    std::ofstream f(config_.outputDir + "/api/" + slug + ".html");
    f << "<!DOCTYPE html>\n<html dir=\"rtl\" lang=\"ar\"><head><meta charset=\"UTF-8\">\n"
      << "<title>" << title << "</title><link rel=\"stylesheet\" href=\"../assets/style.css\">\n"
      << "</head><body class=\"theme-" << config_.theme << "\">\n"
      << "<nav><a href=\"../index.html\">נ  ״§„״±״¦״³״©</a></nav>\n"
      << "<h1>" << title << " (" << entries.size() << ")</h1>\n"
      << "<table class=\"items-table\"><thead><tr><th>״§„״§״³…</th><th>״§„…„״®״µ</th></tr></thead><tbody>\n";
    for (const auto& e : entries) {
        f << "<tr><td><a href=\"" << e.id << ".html\">" << escapeHtml(e.name) << "</a></td>"
          << "<td>" << escapeHtml(e.summary) << "</td></tr>\n";
    }
    f << "</tbody></table></body></html>\n";
}

void HtmlDocGenerator::generateSearchIndex(const std::vector<DocEntry>& entries) {
    std::ofstream f(config_.outputDir + "/assets/search-index.json");
    f << "[\n";
    bool first = true;
    for (const auto& e : entries) {
        if (!first) f << ",\n";
        first = false;
        f << "{\"name\":\"" << JsonDocGenerator::escapeJson(e.name)
          << "\",\"kind\":\"" << e.kindNameAr()
          << "\",\"summary\":\"" << JsonDocGenerator::escapeJson(e.summary)
          << "\",\"url\":\"api/" << e.id << ".html\"}";
    }
    f << "\n]\n";
}

void HtmlDocGenerator::generateSearchScript() {
    std::ofstream f(config_.outputDir + "/assets/search.js");
    f << R"((function(){let d=null;const i=document.getElementById('search-input'),r=document.getElementById('search-results');fetch('assets/search-index.json').then(x=>x.json()).then(x=>{d=x});i.addEventListener('input',function(){if(!d)return;const q=this.value.trim().toLowerCase();if(q.length<2){r.innerHTML='';return}const m=d.filter(x=>x.name.toLowerCase().includes(q)||x.summary.toLowerCase().includes(q)).slice(0,15);r.innerHTML=m.map(x=>'<a class="search-result" href="'+x.url+'"><span class="result-kind">'+x.kind+'</span> '+x.name+'<br><small>'+x.summary+'</small></a>').join('')})})();)";
}

void HtmlDocGenerator::generateModuleTree(const std::vector<DocEntry>&) {}

void HtmlDocGenerator::generateStylesheet() {
    std::ofstream f(config_.outputDir + "/assets/style.css");
    f << R"(:root{--bg:#0f0f23;--bg2:#16213e;--card:#1a1a3e;--text:#e8e8f0;--text2:#a0a0c0;--accent:#4dabf7;--success:#51cf66;--warn:#ffd43b;--danger:#ff6b6b;--border:#2a2a4a;--code-bg:#0a0a1a;--font:'Segoe UI',Tahoma,Arial,sans-serif;--mono:Consolas,monospace}
.theme-light{--bg:#f8f9fa;--bg2:#fff;--card:#fff;--text:#212529;--text2:#6c757d;--accent:#0d6efd;--border:#dee2e6;--code-bg:#f5f5f5}
.theme-ocean{--bg:#0a192f;--bg2:#112240;--card:#172a45;--accent:#64ffda;--text:#ccd6f6}
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:var(--font);background:var(--bg);color:var(--text);line-height:1.8;padding:0 20px 40px;max-width:1200px;margin:0 auto}
a{color:var(--accent);text-decoration:none}a:hover{text-decoration:underline}
.site-header{text-align:center;padding:40px 20px;background:linear-gradient(135deg,var(--bg2),var(--bg));border-radius:12px;margin:20px 0;border:1px solid var(--border)}
.site-header h1{font-size:2.2em;color:var(--accent)}.subtitle{color:var(--text2)}.version{display:inline-block;margin-top:10px;padding:4px 16px;background:var(--accent);color:var(--bg);border-radius:20px;font-size:.85em;font-weight:bold}
.search-container{position:relative;margin:20px 0}#search-input{width:100%;padding:14px 20px;font-size:1em;background:var(--bg2);color:var(--text);border:2px solid var(--border);border-radius:10px;outline:none}#search-input:focus{border-color:var(--accent)}
#search-results{position:absolute;top:100%;width:100%;z-index:100;background:var(--bg2);border-radius:0 0 10px 10px;border:1px solid var(--border);max-height:400px;overflow-y:auto}
.search-result{display:block;padding:10px 16px;border-bottom:1px solid var(--border)}.search-result:hover{background:var(--card)}.result-kind{padding:2px 8px;font-size:.75em;background:var(--accent);color:var(--bg);border-radius:4px;margin-left:8px}
.stats-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(120px,1fr));gap:15px;margin:20px 0}
.stat-card{background:var(--card);padding:20px;border-radius:10px;text-align:center;border:1px solid var(--border)}.stat-icon{font-size:1.5em;display:block}.stat-number{font-size:2em;font-weight:bold;color:var(--accent);display:block}.stat-label{color:var(--text2)}
.doc-section{margin:30px 0}.doc-section h2{color:var(--accent);border-bottom:2px solid var(--border);padding-bottom:8px;margin-bottom:15px}
.item-grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(300px,1fr));gap:12px}
.item-card{background:var(--card);padding:15px;border-radius:8px;border:1px solid var(--border)}.item-card:hover{border-color:var(--accent)}.item-summary{color:var(--text2);font-size:.9em;margin-top:5px}
.kind-badge{display:inline-block;padding:3px 10px;font-size:.8em;border-radius:5px;font-weight:bold;background:var(--accent);color:var(--bg)}
.access-badge{display:inline-block;padding:2px 8px;font-size:.75em;border-radius:4px;border:1px solid}.access-public{color:var(--success);border-color:var(--success)}.access-private{color:var(--danger);border-color:var(--danger)}
.deprecated-badge{display:inline-block;padding:2px 8px;font-size:.75em;background:var(--warn);color:#000;border-radius:4px}
.breadcrumb{padding:10px 0;color:var(--text2);font-size:.9em;margin-bottom:15px}.breadcrumb a{margin:0 4px}
.item-header{margin-bottom:20px}.item-header h1{font-size:2em;margin-top:8px}.item-info{display:flex;gap:8px;align-items:center}
.signature{background:var(--code-bg);padding:16px;border-radius:8px;overflow-x:auto;margin:15px 0;border:1px solid var(--border)}.signature pre{margin:0}.signature code{font-family:var(--mono);font-size:.95em}
.kw{color:#c678dd;font-weight:bold}.str{color:#98c379}.num{color:#d19a66}.cmt{color:#5c6370;font-style:italic}.fn{color:#61afef}.typ{color:#e5c07b}
.params-table,.items-table{width:100%;border-collapse:collapse;margin:10px 0}
.params-table th,.items-table th{background:var(--bg2);padding:10px;text-align:right;border-bottom:2px solid var(--accent)}
.params-table td,.items-table td{padding:10px;border-bottom:1px solid var(--border)}
.admonition{padding:15px 18px;border-radius:8px;margin:12px 0;border-right:4px solid}.admonition.note{background:rgba(77,171,247,.1);border-color:var(--accent)}.admonition.warning{background:rgba(255,212,59,.1);border-color:var(--warn)}.admonition.danger{background:rgba(255,107,107,.1);border-color:var(--danger)}
.source-info{margin-top:30px;padding-top:15px;border-top:1px solid var(--border);color:var(--text2);font-size:.85em}
footer{text-align:center;padding:20px;color:var(--text2);border-top:1px solid var(--border);margin-top:40px}
@media(max-width:768px){.stats-grid{grid-template-columns:repeat(3,1fr)}.item-grid{grid-template-columns:1fr}})";
}

std::string HtmlDocGenerator::highlightSadCode(const std::string& code) const {
    if (!config_.syntaxHighlight) return escapeHtml(code);
    std::string result = escapeHtml(code);
    static const std::vector<std::string> kws = {
        "״¯״§„״©","״µ†","‡ƒ„","״×״¹״¯״§״¯","״³…״©","ˆ״§״¬‡״©","ˆ״­״¯״©",
        "״¥״°״§","ˆ״¥„״§","״¨†…״§","„ƒ„","","״§״±״¬״¹","״§״®״±״¬","״×״§״¨״¹","†‡״§״©",
        "…״×״÷״±","״«״§״¨״×","״¬״¯״¯","‡״°״§","״¨״§†","״­״§ˆ„","״§…״³ƒ","״£״®״±״§‹",
        "״§״³״×ˆ״±״¯","…†","״µ״¯‘״±","״µ״­״­","״®״·״£","״¹״¯…",
        "״±‚…","״¹״´״±","…†״·‚","†״µ","…״µˆ״©","״®״±״·״©",
        "״­״§„״©","״¹†״¯…״§","״§״×״±״§״¶"
    };
    for (const auto& kw : kws) {
        std::string ek = escapeHtml(kw);
        std::string rep = "<span class=\"kw\">" + ek + "</span>";
        size_t pos = 0;
        while ((pos = result.find(ek, pos)) != std::string::npos) {
            result.replace(pos, ek.length(), rep);
            pos += rep.length();
        }
    }
    return result;
}

std::string HtmlDocGenerator::renderMarkdownToHtml(const std::string& md) const {
    std::string html;
    std::istringstream stream(md);
    std::string line;
    while (std::getline(stream, line)) {
        html += line.empty() ? "<br>\n" : ("<p>" + escapeHtml(line) + "</p>\n");
    }
    return html;
}

std::string HtmlDocGenerator::escapeHtml(const std::string& s) const {
    std::string r; r.reserve(s.size() + s.size()/4);
    for (char c : s) {
        switch(c) { case '<':r+="&lt;";break; case '>':r+="&gt;";break; case '&':r+="&amp;";break;
        case '"':r+="&quot;";break; case '\'':r+="&#39;";break; default:r+=c; }
    }
    return r;
}

std::string HtmlDocGenerator::accessBadge(AccessLevel level) const {
    switch(level) {
        case AccessLevel::Public: return "<span class=\"access-badge access-public\">״¹״§…</span>";
        case AccessLevel::Private: return "<span class=\"access-badge access-private\">״®״§״µ</span>";
        case AccessLevel::Protected: return "<span class=\"access-badge access-protected\">…״­…</span>";
        case AccessLevel::Internal: return "<span class=\"access-badge access-protected\">״¯״§״®„</span>";
    }
    return "";
}

std::string HtmlDocGenerator::kindIcon(DocItemKind kind) const {
    switch(kind) {
        case DocItemKind::Function: return "ג¡"; case DocItemKind::Class: return "נ—ן¸";
        case DocItemKind::Struct: return "נ“¦"; case DocItemKind::Enum: return "נ“‹";
        case DocItemKind::Trait: return "נ”—"; case DocItemKind::Interface: return "נ”";
        case DocItemKind::Variable: return "נ“"; case DocItemKind::Constant: return "נ”’";
        case DocItemKind::Module: return "נ“"; case DocItemKind::Property: return "נ·ן¸";
        case DocItemKind::Constructor: return "נ”¨"; case DocItemKind::Method: return "ג™ן¸";
        case DocItemKind::TypeAlias: return "נ”„";
    }
    return "נ“„";
}

// ============================================================================
// MarkdownDocGenerator
// ============================================================================

int MarkdownDocGenerator::generate(const std::vector<DocEntry>& entries, DocsResult& result) {
    fs::create_directories(config_.outputDir);
    fs::create_directories(config_.outputDir + "/api");
    {
        std::ofstream f(config_.outputDir + "/README.md");
        f << "# " << config_.title << "\n\n";
        if (!config_.description.empty()) f << config_.description << "\n\n";
        if (!config_.version.empty()) f << "**״§„״¥״µ״¯״§״±:** " << config_.version << "\n\n";
        f << generateTableOfContents(entries);
        result.pagesGenerated++;
        result.generatedFiles.push_back(config_.outputDir + "/README.md");
    }
    for (const auto& entry : entries) {
        std::string filename = entry.id + ".md";
        std::ofstream f(config_.outputDir + "/api/" + filename);
        f << generateEntryMarkdown(entry);
        result.pagesGenerated++;
        result.generatedFiles.push_back(config_.outputDir + "/api/" + filename);
    }
    result.itemsDocumented = Sad::Security::SafeArithmetic::assertSafeCast<int>(entries.size(), "docs_emitter_html_size");
    return 0;
}

std::string MarkdownDocGenerator::generateEntryMarkdown(const DocEntry& entry) const {
    std::ostringstream ss;
    ss << "# " << entry.name << "\n\n**״§„†ˆ״¹:** " << entry.kindNameAr() << "\n\n";
    if (entry.isDeprecated()) ss << "> ג ן¸ **…‡…„:** " << entry.deprecated << "\n\n";
    ss << "```sad\n" << entry.signature << "\n```\n\n";
    if (!entry.summary.empty()) ss << entry.summary << "\n\n";
    if (!entry.description.empty()) ss << entry.description << "\n\n";
    if (!entry.params.empty()) {
        ss << "## ״§„…״¹״·״§״×\n\n| ״§„״§״³… | ״§„†ˆ״¹ | ״§„ˆ״µ |\n|-------|------|-------|\n";
        for (const auto& p : entry.params)
            ss << "| `" << p.name << "` | `" << p.type << "` | " << p.description << " |\n";
        ss << "\n";
    }
    if (!entry.returnType.empty()) {
        ss << "## ״§„‚…״© ״§„…״±״¬״¹״©\n\n- **״§„†ˆ״¹:** `" << entry.returnType << "`\n";
        if (!entry.returnDescription.empty()) ss << "- " << entry.returnDescription << "\n";
        ss << "\n";
    }
    if (!entry.examples.empty()) {
        ss << "## ״£…״«„״©\n\n";
        for (const auto& ex : entry.examples) ss << "```sad\n" << ex << "\n```\n\n";
    }
    if (!entry.seeAlso.empty()) {
        ss << "## ״§†״¸״± ״£״¶״§‹\n\n";
        for (const auto& r : entry.seeAlso) ss << "- `" << r << "`\n";
        ss << "\n";
    }
    if (!entry.sourceFile.empty()) {
        ss << "---\nנ“ **״§„…„:** `" << entry.sourceFile << "`";
        if (entry.lineNumber > 0) ss << " ג€” ״§„״³״·״± " << entry.lineNumber;
        ss << "\n";
    }
    return ss.str();
}

std::string MarkdownDocGenerator::generateTableOfContents(const std::vector<DocEntry>& entries) const {
    std::ostringstream ss;
    ss << "## ״§„‡״±״³\n\n";
    struct SD { DocItemKind kind; std::string title; };
    for (auto& sec : std::vector<SD>{
        {DocItemKind::Module,"״§„ˆ״­״¯״§״×"},{DocItemKind::Class,"״§„״£״µ†״§"},
        {DocItemKind::Function,"״§„״¯ˆ״§„"},{DocItemKind::Struct,"״§„‡״§ƒ„"},
        {DocItemKind::Constant,"״§„״«ˆ״§״¨״×"}}) {
        bool has = false;
        for (const auto& e : entries) if (e.kind == sec.kind) { has = true; break; }
        if (!has) continue;
        ss << "### " << sec.title << "\n\n";
        for (const auto& e : entries) {
            if (e.kind != sec.kind) continue;
            ss << "- [`" << e.name << "`](api/" << e.id << ".md)";
            if (!e.summary.empty()) ss << " ג€” " << e.summary;
            ss << "\n";
        }
        ss << "\n";
    }
    return ss.str();
}

// ============================================================================
// JsonDocGenerator
// ============================================================================

int JsonDocGenerator::generate(const std::vector<DocEntry>& entries, DocsResult& result) {
    fs::create_directories(config_.outputDir);
    std::ofstream f(config_.outputDir + "/docs.json");
    f << "{\n  \"title\": \"" << escapeJson(config_.title)
      << "\",\n  \"version\": \"" << escapeJson(config_.version)
      << "\",\n  \"totalItems\": " << entries.size() << ",\n  \"items\": [\n";
    for (size_t i = 0; i < entries.size(); ++i) {
        f << entryToJson(entries[i]);
        if (i < entries.size() - 1) f << ",";
        f << "\n";
    }
    f << "  ]\n}\n";
    result.pagesGenerated = 1;
    result.itemsDocumented = Sad::Security::SafeArithmetic::assertSafeCast<int>(entries.size(), "docs_emitter_html_size");
    result.generatedFiles.push_back(config_.outputDir + "/docs.json");
    return 0;
}

std::string JsonDocGenerator::entryToJson(const DocEntry& entry, int indent) const {
    std::string p(indent, ' '), p2(indent+2, ' ');
    std::ostringstream ss;
    ss << p << "{\n"
       << p2 << "\"id\": \"" << escapeJson(entry.id) << "\",\n"
       << p2 << "\"name\": \"" << escapeJson(entry.name) << "\",\n"
       << p2 << "\"kind\": \"" << entry.kindNameEn() << "\",\n"
       << p2 << "\"kindAr\": \"" << entry.kindNameAr() << "\",\n"
       << p2 << "\"signature\": \"" << escapeJson(entry.signature) << "\",\n"
       << p2 << "\"summary\": \"" << escapeJson(entry.summary) << "\",\n"
       << p2 << "\"description\": \"" << escapeJson(entry.description) << "\",\n"
       << p2 << "\"params\": [";
    for (size_t j = 0; j < entry.params.size(); ++j) {
        const auto& pa = entry.params[j];
        ss << "{\"name\":\"" << escapeJson(pa.name)
           << "\",\"type\":\"" << escapeJson(pa.type)
           << "\",\"description\":\"" << escapeJson(pa.description) << "\"}";
        if (j < entry.params.size()-1) ss << ",";
    }
    ss << "],\n"
       << p2 << "\"returnType\": \"" << escapeJson(entry.returnType) << "\",\n"
       << p2 << "\"examples\": [";
    for (size_t j = 0; j < entry.examples.size(); ++j) {
        ss << "\"" << escapeJson(entry.examples[j]) << "\"";
        if (j < entry.examples.size()-1) ss << ",";
    }
    ss << "],\n"
       << p2 << "\"sourceFile\": \"" << escapeJson(entry.sourceFile) << "\",\n"
       << p2 << "\"lineNumber\": " << entry.lineNumber << "\n"
       << p << "}";
    return ss.str();
}

std::string JsonDocGenerator::escapeJson(const std::string& s) {
    std::string r; r.reserve(s.size() + s.size()/8);
    for (char c : s) {
        switch(c) { case '"':r+="\\\"";break; case '\\':r+="\\\\";break;
        case '\n':r+="\\n";break; case '\r':r+="\\r";break; case '\t':r+="\\t";break;
        default: if(static_cast<unsigned char>(c)<0x20){char b[8];snprintf(b,sizeof(b),"\\u%04x",(unsigned char)c);r+=b;}else r+=c; }
    }
    return r;
}

// ============================================================================
// DocsEmitter ג€” ״§„…״­״±ƒ ״§„״±״¦״³
// ============================================================================

void DocsEmitter::addEntry(const DocEntry& entry) {
    entries_.push_back(entry);
    rebuildIndices();
}

int DocsEmitter::addFromFile(const std::string& filePath) {
    auto items = parser_.parseFile(filePath);
    int count = Sad::Security::SafeArithmetic::assertSafeCast<int>(items.size(), "docs_emitter_html_size");
    for (auto& item : items) entries_.push_back(std::move(item));
    if (count > 0) rebuildIndices();
    return count;
}

int DocsEmitter::addFromSource(const std::string& source, const std::string& filename) {
    auto items = parser_.parseSource(source, filename);
    int count = Sad::Security::SafeArithmetic::assertSafeCast<int>(items.size(), "docs_emitter_html_size");
    for (auto& item : items) entries_.push_back(std::move(item));
    if (count > 0) rebuildIndices();
    return count;
}

void DocsEmitter::buildCrossReferences() {
    std::map<std::string, size_t> lookup;
    for (size_t i = 0; i < entries_.size(); ++i) {
        lookup[entries_[i].name] = i;
        if (!entries_[i].qualifiedName.empty()) lookup[entries_[i].qualifiedName] = i;
    }
    for (auto& entry : entries_) {
        for (const auto& [name, idx] : lookup) {
            if (name == entry.name || name.size() < 2) continue;
            bool found = entry.description.find(name) != std::string::npos ||
                         entry.summary.find(name) != std::string::npos;
            if (found && std::find(entry.crossRefs.begin(), entry.crossRefs.end(), name) == entry.crossRefs.end()) {
                entry.crossRefs.push_back(name);
            }
        }
        if (!entry.parentName.empty()) {
            auto it = lookup.find(entry.parentName);
            if (it != lookup.end()) {
                auto& parent = entries_[it->second];
                if (std::find(parent.children.begin(), parent.children.end(), entry.name) == parent.children.end())
                    parent.children.push_back(entry.name);
            }
        }
    }
}

DocsResult DocsEmitter::generate() {
    DocsResult result;
    auto startTime = std::chrono::high_resolution_clock::now();

    if (entries_.empty() && !config_.inputFiles.empty()) {
        FileCollector collector;
        auto files = collector.collect(config_.inputFiles);
        for (const auto& file : files) {
            if (config_.verbose) std::cout << "  נ“„ " << file << "\n";
            addFromFile(file);
            result.filesProcessed++;
        }
    }

    filterByAccess();
    if (config_.crossRefsEnabled) buildCrossReferences();

    if (config_.htmlOutput) {
        if (config_.verbose) std::cout << "נ ״×ˆ„״¯ HTML...\n";
        HtmlDocGenerator gen(config_);
        gen.generate(entries_, result);
    }
    if (config_.markdownOutput) {
        if (config_.verbose) std::cout << "נ“ ״×ˆ„״¯ Markdown...\n";
        DocsConfig mc = config_;
        if (config_.htmlOutput) mc.outputDir = config_.outputDir + "/markdown";
        MarkdownDocGenerator gen(mc);
        gen.generate(entries_, result);
    }
    if (config_.jsonOutput) {
        if (config_.verbose) std::cout << "נ“ ״×ˆ„״¯ JSON...\n";
        JsonDocGenerator gen(config_);
        gen.generate(entries_, result);
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    result.durationSeconds = std::chrono::duration<double>(endTime - startTime).count();
    return result;
}

DocsResult DocsEmitter::generate(DocFormat format) {
    DocsConfig saved = config_;
    config_.htmlOutput = (format == DocFormat::HTML || format == DocFormat::All);
    config_.markdownOutput = (format == DocFormat::Markdown || format == DocFormat::All);
    config_.jsonOutput = (format == DocFormat::JSON || format == DocFormat::All);
    auto result = generate();
    config_ = saved;
    return result;
}

std::string DocsEmitter::emitJson() const {
    JsonDocGenerator gen(config_);
    std::ostringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < entries_.size(); ++i) {
        ss << gen.entryToJson(entries_[i], 2);
        if (i < entries_.size() - 1) ss << ",";
        ss << "\n";
    }
    ss << "]\n";
    return ss.str();
}

std::string DocsEmitter::emitMarkdown() const {
    std::ostringstream out;
    out << "# ״×ˆ״«‚ „״÷״© ״µ\n\n";
    std::map<std::string, std::vector<const DocEntry*>> byKind;
    for (const auto& e : entries_) byKind[e.kindNameAr()].push_back(&e);
    for (const auto& [kind, items] : byKind) {
        out << "## " << kind << "\n\n";
        for (const auto* e : items) {
            out << "### `" << e->name << "`\n\n";
            if (!e->signature.empty()) out << "```sad\n" << e->signature << "\n```\n\n";
            if (!e->summary.empty()) out << e->summary << "\n\n";
            if (!e->description.empty()) out << e->description << "\n\n";
            if (!e->params.empty()) {
                out << "**…״¹״§…„״§״×:**\n\n";
                for (const auto& p : e->params) out << "- `" << p.name << "`: " << p.description << "\n";
                out << "\n";
            }
            if (!e->returnType.empty()) {
                out << "**״¥״±״¬״§״¹:** `" << e->returnType << "`";
                if (!e->returnDescription.empty()) out << " ג€” " << e->returnDescription;
                out << "\n\n";
            }
            for (const auto& ex : e->examples) out << "**…״«״§„:**\n```sad\n" << ex << "\n```\n\n";
        }
    }
    return out.str();
}

std::vector<const DocEntry*> DocsEmitter::search(const std::string& query) const {
    std::vector<const DocEntry*> results;
    for (const auto& e : entries_) {
        if (e.name.find(query) != std::string::npos ||
            e.qualifiedName.find(query) != std::string::npos ||
            e.summary.find(query) != std::string::npos ||
            e.description.find(query) != std::string::npos)
            results.push_back(&e);
    }
    return results;
}

const DocEntry* DocsEmitter::findByName(const std::string& name) const {
    auto it = nameIndex_.find(name);
    if (it != nameIndex_.end() && it->second < entries_.size()) return &entries_[it->second];
    return nullptr;
}

std::vector<const DocEntry*> DocsEmitter::findByKind(DocItemKind kind) const {
    std::vector<const DocEntry*> r;
    for (const auto& e : entries_) if (e.kind == kind) r.push_back(&e);
    return r;
}

void DocsEmitter::rebuildIndices() {
    nameIndex_.clear(); idIndex_.clear();
    for (size_t i = 0; i < entries_.size(); ++i) {
        nameIndex_[entries_[i].name] = i;
        if (!entries_[i].qualifiedName.empty()) nameIndex_[entries_[i].qualifiedName] = i;
        idIndex_[entries_[i].id] = i;
    }
}

void DocsEmitter::filterByAccess() {
    if (config_.includePrivate && config_.includeInternal) return;
    entries_.erase(std::remove_if(entries_.begin(), entries_.end(),
        [this](const DocEntry& e) {
            if (e.access == AccessLevel::Private && !config_.includePrivate) return true;
            if (e.access == AccessLevel::Internal && !config_.includeInternal) return true;
            return false;
        }), entries_.end());
    rebuildIndices();
}

// ============================================================================
// C entry points
// ============================================================================

static std::string g_docsResult;

extern "C" {

const char* sadc_emit_docs(const char* source_path, const char* format) {
    DocsEmitter emitter;
    if (source_path && source_path[0] != '\0') emitter.addFromFile(source_path);
    std::string fmt(format ? format : "json");
    if (fmt == "markdown" || fmt == "md") g_docsResult = emitter.emitMarkdown();
    else g_docsResult = emitter.emitJson();
    return g_docsResult.c_str();
}

const char* sadc_emit_docs_batch(const char** source_paths, int count,
                                  const char* format, const char* output_dir) {
    DocsConfig config;
    config.outputDir = output_dir ? output_dir : "docs";
    std::string fmt(format ? format : "html");
    config.htmlOutput = (fmt == "html" || fmt == "all");
    config.markdownOutput = (fmt == "markdown" || fmt == "md" || fmt == "all");
    config.jsonOutput = (fmt == "json" || fmt == "all");
    for (int i = 0; i < count; ++i) if (source_paths[i]) config.inputFiles.push_back(source_paths[i]);
    DocsEmitter emitter(config);
    auto result = emitter.generate();
    g_docsResult = "{\"success\":" + std::string(result.success ? "true" : "false")
        + ",\"pages\":" + std::to_string(result.pagesGenerated)
        + ",\"items\":" + std::to_string(result.itemsDocumented)
        + ",\"duration\":" + std::to_string(result.durationSeconds) + "}";
    return g_docsResult.c_str();
}

} // extern "C"


} // namespace Meta
} // namespace Sad
