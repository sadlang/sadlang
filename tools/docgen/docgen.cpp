// بسم الله الرحمن الرحيم
// Documentation Generator Implementation
// تنفيذ مولد التوثيق
// Phase 7.4: Documentation Generator

#include "docgen.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <regex>

namespace sad {
namespace docgen {

// ============================================================================
// الحمد لله - DocExtractor Implementation / تنفيذ مستخرج التوثيق
// ============================================================================

bool DocExtractor::extract_from_file(const std::string& file_path) {
    auto content = read_file(file_path);
    if (!content) {
        std::cerr << "Failed to read file: " << file_path << std::endl;
        return false;
    }
    
    return extract_from_source(*content, file_path);
}

bool DocExtractor::extract_from_source(const std::string& source, const std::string& file_name) {
    // Split into lines
    std::vector<std::string> lines;
    std::istringstream stream(source);
    std::string line;
    
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    
    // Find documentation comments
    for (size_t i = 0; i < lines.size(); ++i) {
        const std::string& current = lines[i];
        
        // Check for doc comment start
        if (current.find("###") != std::string::npos ||
            (current.find("#") == 0 && current.find("@") != std::string::npos)) {
            
            // Collect comment lines
            std::vector<std::string> comment_lines;
            size_t start = i;
            
            // Multi-line comment (###)
            if (current.find("###") != std::string::npos) {
                ++i;  // Skip ###
                while (i < lines.size() && lines[i].find("###") == std::string::npos) {
                    comment_lines.push_back(lines[i]);
                    ++i;
                }
            }
            // Single-line comments
            else {
                while (i < lines.size() && 
                       lines[i].find("#") == 0 && 
                       lines[i].find("@") != std::string::npos) {
                    comment_lines.push_back(lines[i]);
                    ++i;
                }
                --i;  // Back one line
            }
            
            // Parse comment
            auto comment_opt = parse_doc_comment(comment_lines, start, file_name);
            if (comment_opt) {
                // Determine type from next non-empty line
                for (size_t j = i + 1; j < lines.size(); ++j) {
                    std::string next = lines[j];
                    // Trim whitespace
                    next.erase(0, next.find_first_not_of(" \t"));
                    
                    if (!next.empty() && next[0] != '#') {
                        comment_opt->type = determine_type(next);
                        comment_opt->line = j + 1;
                        break;
                    }
                }
                
                comments_.push_back(*comment_opt);
            }
        }
    }
    
    return true;
}

std::optional<DocComment> DocExtractor::parse_doc_comment(
    const std::vector<std::string>& lines,
    int start_line,
    const std::string& file) {
    
    DocComment comment;
    comment.file = file;
    comment.line = start_line + 1;
    
    for (const auto& line : lines) {
        extract_tags(comment, line);
    }
    
    return comment;
}

void DocExtractor::extract_tags(DocComment& comment, const std::string& line) {
    // Remove leading # and whitespace
    std::string clean_line = line;
    size_t hash_pos = clean_line.find('#');
    if (hash_pos != std::string::npos) {
        clean_line = clean_line.substr(hash_pos + 1);
    }
    
    // Trim
    clean_line.erase(0, clean_line.find_first_not_of(" \t"));
    
    if (clean_line.empty()) return;
    
    // Parse tags
    if (clean_line.find("@brief") == 0) {
        comment.description = clean_line.substr(7);
    }
    else if (clean_line.find("@brief_ar") == 0) {
        comment.description_ar = clean_line.substr(10);
    }
    else if (clean_line.find("@description") == 0) {
        if (!comment.description.empty()) comment.description += " ";
        comment.description += clean_line.substr(13);
    }
    else if (clean_line.find("@description_ar") == 0) {
        if (!comment.description_ar.empty()) comment.description_ar += " ";
        comment.description_ar += clean_line.substr(16);
    }
    else if (clean_line.find("@param") == 0) {
        std::string param_text = clean_line.substr(7);
        size_t space = param_text.find(' ');
        if (space != std::string::npos) {
            std::string name = param_text.substr(0, space);
            std::string desc = param_text.substr(space + 1);
            comment.parameters.push_back({name, desc});
        }
    }
    else if (clean_line.find("@return") == 0) {
        comment.return_desc = clean_line.substr(8);
    }
    else if (clean_line.find("@example") == 0) {
        comment.examples.push_back(clean_line.substr(9));
    }
    else if (clean_line.find("@see") == 0) {
        comment.see_also.push_back(clean_line.substr(5));
    }
    else if (clean_line.find("@deprecated") == 0) {
        comment.deprecated = true;
    }
    else if (clean_line.find("@since") == 0) {
        comment.since_version = clean_line.substr(7);
    }
    else if (clean_line.find("@author") == 0) {
        comment.author = clean_line.substr(8);
    }
}

DocType DocExtractor::determine_type(const std::string& next_line) {
    if (next_line.find("دالة") != std::string::npos || 
        next_line.find("function") != std::string::npos) {
        return DocType::FUNCTION;
    }
    else if (next_line.find("صنف") != std::string::npos || 
             next_line.find("class") != std::string::npos) {
        return DocType::CLASS;
    }
    else if (next_line.find("رقم") != std::string::npos || 
             next_line.find("نص") != std::string::npos ||
             next_line.find("منطقي") != std::string::npos) {
        return DocType::VARIABLE;
    }
    else if (next_line.find("ثابت") != std::string::npos || 
             next_line.find("const") != std::string::npos) {
        return DocType::CONSTANT;
    }
    
    return DocType::FUNCTION;  // Default
}

std::vector<DocComment> DocExtractor::get_comments_by_type(DocType type) const {
    std::vector<DocComment> result;
    
    for (const auto& comment : comments_) {
        if (comment.type == type) {
            result.push_back(comment);
        }
    }
    
    return result;
}

std::vector<DocComment> DocExtractor::get_comments_by_module(const std::string& module) const {
    std::vector<DocComment> result;
    
    for (const auto& comment : comments_) {
        if (comment.module == module) {
            result.push_back(comment);
        }
    }
    
    return result;
}

// ============================================================================
// الحمد لله - DocGenerator Implementation / تنفيذ مولد التوثيق
// ============================================================================

bool DocGenerator::generate(const std::vector<DocComment>& comments,
                           OutputFormat format,
                           const std::string& output_dir) {
    // Ensure output directory exists
    if (!ensure_directory_exists(output_dir)) {
        std::cerr << "Failed to create output directory: " << output_dir << std::endl;
        return false;
    }
    
    switch (format) {
        case OutputFormat::HTML:
            return generate_html(comments, output_dir);
        
        case OutputFormat::MARKDOWN:
            return generate_markdown(comments, output_dir);
        
        case OutputFormat::JSON:
            return generate_json(comments, output_dir + "/api.json");
        
        default:
            std::cerr << "Unsupported format" << std::endl;
            return false;
    }
}

bool DocGenerator::generate_html(const std::vector<DocComment>& comments,
                                const std::string& output_dir) {
    // Generate index page
    std::string index_html = get_html_header("API Reference - " + settings_.project_name);
    
    index_html += R"(
<div class="container">
    <aside class="sidebar">
)";
    
    index_html += generate_sidebar(comments);
    
    index_html += R"(
    </aside>
    <main class="content">
        <h1>)" + settings_.project_name + R"(</h1>
        <h2 style="direction: rtl;">)" + settings_.project_name_ar + R"(</h2>
        <p>Version: )" + settings_.version + R"(</p>
        
        <div class="section">
            <h2>الحمد لله - API Reference</h2>
            <p>Complete API documentation for )" + settings_.project_name + R"(</p>
        </div>
)";
    
    // Group by type
    std::unordered_map<DocType, std::vector<DocComment>> grouped;
    for (const auto& comment : comments) {
        grouped[comment.type].push_back(comment);
    }
    
    // Generate sections
    const std::vector<DocType> types = {
        DocType::FUNCTION,
        DocType::CLASS,
        DocType::VARIABLE,
        DocType::CONSTANT
    };
    
    for (auto type : types) {
        if (grouped.count(type) > 0) {
            index_html += R"(
        <div class="section">
            <h2>)" + doc_type_to_string(type) + " / " + doc_type_to_string_ar(type) + R"(</h2>
            <ul>
)";
            
            for (const auto& comment : grouped[type]) {
                std::string name = comment.name.empty() ? "Unnamed" : comment.name;
                index_html += "                <li><a href=\"#" + name + "\">" + name;
                if (!comment.name_ar.empty()) {
                    index_html += " / " + comment.name_ar;
                }
                index_html += "</a></li>\n";
            }
            
            index_html += R"(
            </ul>
        </div>
)";
        }
    }
    
    index_html += R"(
    </main>
</div>
)";
    
    index_html += get_html_footer();
    
    // Write index
    if (!write_file(output_dir + "/index.html", index_html)) {
        return false;
    }
    
    // Generate individual pages
    for (const auto& comment : comments) {
        std::string page_html = generate_html_page(comment);
        std::string filename = comment.name.empty() ? "unnamed" : comment.name;
        filename += ".html";
        
        if (!write_file(output_dir + "/" + filename, page_html)) {
            std::cerr << "Failed to write: " << filename << std::endl;
        }
    }
    
    // Copy CSS and JS
    write_file(output_dir + "/style.css", get_css_styles());
    write_file(output_dir + "/script.js", get_javascript());
    
    return true;
}

std::string DocGenerator::generate_html_page(const DocComment& comment) {
    std::string title = comment.name.empty() ? "Documentation" : comment.name;
    std::string html = get_html_header(title);
    
    html += R"(<div class="container"><main class="content">)";
    
    // Title
    html += "<h1>" + escape_html(comment.name);
    if (!comment.name_ar.empty()) {
        html += " / <span style='direction: rtl;'>" + escape_html(comment.name_ar) + "</span>";
    }
    html += "</h1>\n";
    
    // Type badge
    html += R"(<span class="badge badge-)" + doc_type_to_string(comment.type) + R"(">)";
    html += doc_type_to_string(comment.type) + " / " + doc_type_to_string_ar(comment.type);
    html += "</span>\n";
    
    // Deprecated warning
    if (comment.deprecated) {
        html += R"(<div class="warning">⚠️ Deprecated / مهمل</div>)";
    }
    
    // Description
    if (!comment.description.empty() || !comment.description_ar.empty()) {
        html += R"(<div class="section">)";
        if (!comment.description.empty()) {
            html += "<p>" + escape_html(comment.description) + "</p>";
        }
        if (!comment.description_ar.empty()) {
            html += "<p style='direction: rtl;'>" + escape_html(comment.description_ar) + "</p>";
        }
        html += "</div>\n";
    }
    
    // Signature
    if (!comment.signature.empty()) {
        html += R"(<div class="section"><h3>Signature / التوقيع</h3>)";
        html += "<pre><code>" + escape_html(comment.signature) + "</code></pre>";
        html += "</div>\n";
    }
    
    // Parameters
    if (!comment.parameters.empty()) {
        html += R"(<div class="section"><h3>Parameters / المعاملات</h3><ul>)";
        for (const auto& [name, desc] : comment.parameters) {
            html += "<li><strong>" + escape_html(name) + "</strong>: " + escape_html(desc) + "</li>";
        }
        html += "</ul></div>\n";
    }
    
    // Return value
    if (!comment.return_desc.empty()) {
        html += R"(<div class="section"><h3>Returns / القيمة المرجعة</h3>)";
        html += "<p>" + escape_html(comment.return_desc) + "</p>";
        html += "</div>\n";
    }
    
    // Examples
    if (!comment.examples.empty()) {
        html += R"(<div class="section"><h3>Examples / أمثلة</h3>)";
        for (const auto& example : comment.examples) {
            html += "<pre><code class='language-sad'>" + escape_html(example) + "</code></pre>";
        }
        html += "</div>\n";
    }
    
    // See also
    if (!comment.see_also.empty()) {
        html += R"(<div class="section"><h3>See Also / انظر أيضاً</h3><ul>)";
        for (const auto& link : comment.see_also) {
            html += "<li><a href='" + link + ".html'>" + escape_html(link) + "</a></li>";
        }
        html += "</ul></div>\n";
    }
    
    // Metadata
    html += R"(<div class="section metadata">)";
    if (!comment.file.empty()) {
        html += "<p><strong>File:</strong> " + escape_html(comment.file) + "</p>";
    }
    if (comment.line > 0) {
        html += "<p><strong>Line:</strong> " + std::to_string(comment.line) + "</p>";
    }
    if (!comment.since_version.empty()) {
        html += "<p><strong>Since:</strong> " + escape_html(comment.since_version) + "</p>";
    }
    if (!comment.author.empty()) {
        html += "<p><strong>Author:</strong> " + escape_html(comment.author) + "</p>";
    }
    html += "</div>\n";
    
    html += "</main></div>";
    html += get_html_footer();
    
    return html;
}

std::string DocGenerator::generate_sidebar(const std::vector<DocComment>& comments) {
    std::string sidebar = R"(<h3>الحمد لله - Navigation</h3><ul class="nav">)";
    
    // Group by type
    std::unordered_map<DocType, std::vector<DocComment>> grouped;
    for (const auto& comment : comments) {
        grouped[comment.type].push_back(comment);
    }
    
    const std::vector<DocType> types = {
        DocType::FUNCTION,
        DocType::CLASS,
        DocType::VARIABLE,
        DocType::CONSTANT
    };
    
    for (auto type : types) {
        if (grouped.count(type) > 0) {
            sidebar += "<li><strong>" + doc_type_to_string_ar(type) + "</strong><ul>";
            
            for (const auto& comment : grouped[type]) {
                std::string name = comment.name.empty() ? "Unnamed" : comment.name;
                sidebar += "<li><a href='" + name + ".html'>" + escape_html(name) + "</a></li>";
            }
            
            sidebar += "</ul></li>";
        }
    }
    
    sidebar += "</ul>";
    return sidebar;
}

std::string DocGenerator::get_html_header(const std::string& title) {
    return R"(<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>)" + escape_html(title) + R"(</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <header>
        <h1>)" + settings_.project_name + R"( - Documentation</h1>
    </header>
)";
}

std::string DocGenerator::get_html_footer() {
    return R"(
    <footer>
        <p>الحمد لله - Generated by Sad Documentation Generator</p>
        <p>Version: )" + settings_.version + R"(</p>
    </footer>
    <script src="script.js"></script>
</body>
</html>
)";
}

std::string DocGenerator::get_css_styles() {
    return R"(
/* بسم الله الرحمن الرحيم */
/* Sad Documentation CSS */

* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif, 'Amiri', 'Traditional Arabic';
    line-height: 1.6;
    color: #333;
    background: #f5f5f5;
}

header {
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    color: white;
    padding: 2rem;
    text-align: center;
    box-shadow: 0 2px 10px rgba(0,0,0,0.1);
}

.container {
    display: flex;
    max-width: 1400px;
    margin: 2rem auto;
    gap: 2rem;
    padding: 0 2rem;
}

.sidebar {
    flex: 0 0 300px;
    background: white;
    padding: 2rem;
    border-radius: 8px;
    box-shadow: 0 2px 10px rgba(0,0,0,0.1);
    position: sticky;
    top: 2rem;
    max-height: calc(100vh - 4rem);
    overflow-y: auto;
}

.content {
    flex: 1;
    background: white;
    padding: 3rem;
    border-radius: 8px;
    box-shadow: 0 2px 10px rgba(0,0,0,0.1);
}

h1 {
    color: #667eea;
    margin-bottom: 1rem;
    font-size: 2.5rem;
}

h2 {
    color: #764ba2;
    margin: 2rem 0 1rem;
    font-size: 2rem;
    border-bottom: 2px solid #667eea;
    padding-bottom: 0.5rem;
}

h3 {
    color: #555;
    margin: 1.5rem 0 0.75rem;
    font-size: 1.5rem;
}

.section {
    margin: 2rem 0;
    padding: 1.5rem;
    background: #f9f9f9;
    border-left: 4px solid #667eea;
    border-radius: 4px;
}

.badge {
    display: inline-block;
    padding: 0.25rem 0.75rem;
    background: #667eea;
    color: white;
    border-radius: 20px;
    font-size: 0.875rem;
    margin: 0.5rem 0;
}

.warning {
    background: #fff3cd;
    border: 1px solid #ffc107;
    padding: 1rem;
    border-radius: 4px;
    margin: 1rem 0;
}

pre {
    background: #282c34;
    color: #abb2bf;
    padding: 1.5rem;
    border-radius: 8px;
    overflow-x: auto;
    margin: 1rem 0;
}

code {
    font-family: 'Courier New', Courier, monospace;
    font-size: 0.95rem;
}

.nav {
    list-style: none;
}

.nav li {
    margin: 0.5rem 0;
}

.nav a {
    color: #667eea;
    text-decoration: none;
    transition: color 0.3s;
}

.nav a:hover {
    color: #764ba2;
}

.metadata {
    background: #e9ecef;
    font-size: 0.875rem;
}

footer {
    background: #333;
    color: white;
    text-align: center;
    padding: 2rem;
    margin-top: 4rem;
}
)";
}

std::string DocGenerator::get_javascript() {
    return R"(
// بسم الله الرحمن الرحيم
// Sad Documentation JavaScript

document.addEventListener('DOMContentLoaded', function() {
    console.log('الحمد لله - Documentation loaded');
    
    // Add syntax highlighting
    document.querySelectorAll('pre code').forEach(function(block) {
        // Simple syntax highlighting for Sad language
        let html = block.innerHTML;
        
        // Keywords
        const keywords = ['دالة', 'إرجاع', 'إذا', 'وإلا', 'بينما', 'لـ', 'صنف', 'جديد'];
        keywords.forEach(kw => {
            const regex = new RegExp('\\b' + kw + '\\b', 'g');
            html = html.replace(regex, '<span style="color: #c678dd;">' + kw + '</span>');
        });
        
        // Strings
        html = html.replace(/"([^"]*)"/g, '<span style="color: #98c379;">"$1"</span>');
        
        // Numbers
        html = html.replace(/\b(\d+)\b/g, '<span style="color: #d19a66;">$1</span>');
        
        // Comments
        html = html.replace(/#(.*)/g, '<span style="color: #5c6370;">#$1</span>');
        
        block.innerHTML = html;
    });
});
)";
}

bool DocGenerator::generate_markdown(const std::vector<DocComment>& comments,
                                    const std::string& output_dir) {
    std::string markdown = "# " + settings_.project_name + " - API Reference\n\n";
    markdown += "**الحمد لله - " + settings_.project_name_ar + "**\n\n";
    markdown += "Version: " + settings_.version + "\n\n";
    
    // Table of contents
    markdown += "## Table of Contents / جدول المحتويات\n\n";
    
    for (const auto& comment : comments) {
        std::string name = comment.name.empty() ? "Unnamed" : comment.name;
        markdown += "- [" + name + "](#" + name + ")\n";
    }
    
    markdown += "\n---\n\n";
    
    // Generate docs for each comment
    for (const auto& comment : comments) {
        markdown += generate_markdown_doc(comment);
        markdown += "\n---\n\n";
    }
    
    return write_file(output_dir + "/API.md", markdown);
}

std::string DocGenerator::generate_markdown_doc(const DocComment& comment) {
    std::string md = "## " + comment.name;
    if (!comment.name_ar.empty()) {
        md += " / " + comment.name_ar;
    }
    md += "\n\n";
    
    // Type
    md += "**Type:** " + doc_type_to_string(comment.type) + " / " + doc_type_to_string_ar(comment.type) + "\n\n";
    
    // Description
    if (!comment.description.empty()) {
        md += comment.description + "\n\n";
    }
    if (!comment.description_ar.empty()) {
        md += comment.description_ar + "\n\n";
    }
    
    // Parameters
    if (!comment.parameters.empty()) {
        md += "### Parameters / المعاملات\n\n";
        for (const auto& [name, desc] : comment.parameters) {
            md += "- **" + name + "**: " + desc + "\n";
        }
        md += "\n";
    }
    
    // Return
    if (!comment.return_desc.empty()) {
        md += "### Returns / القيمة المرجعة\n\n";
        md += comment.return_desc + "\n\n";
    }
    
    // Examples
    if (!comment.examples.empty()) {
        md += "### Examples / أمثلة\n\n";
        for (const auto& example : comment.examples) {
            md += "```sad\n" + example + "\n```\n\n";
        }
    }
    
    return md;
}

bool DocGenerator::generate_json(const std::vector<DocComment>& comments,
                                const std::string& output_file) {
    std::string json = "{\n  \"project\": \"" + settings_.project_name + "\",\n";
    json += "  \"version\": \"" + settings_.version + "\",\n";
    json += "  \"comments\": [\n";
    
    for (size_t i = 0; i < comments.size(); ++i) {
        const auto& c = comments[i];
        
        json += "    {\n";
        json += "      \"name\": \"" + escape_html(c.name) + "\",\n";
        json += "      \"type\": \"" + doc_type_to_string(c.type) + "\",\n";
        json += "      \"description\": \"" + escape_html(c.description) + "\",\n";
        json += "      \"description_ar\": \"" + escape_html(c.description_ar) + "\",\n";
        json += "      \"file\": \"" + escape_html(c.file) + "\",\n";
        json += "      \"line\": " + std::to_string(c.line) + "\n";
        json += "    }";
        
        if (i < comments.size() - 1) json += ",";
        json += "\n";
    }
    
    json += "  ]\n}\n";
    
    return write_file(output_file, json);
}

// ============================================================================
// الحمد لله - Utility Functions / دوال مساعدة
// ============================================================================

std::string doc_type_to_string(DocType type) {
    switch (type) {
        case DocType::FUNCTION: return "Function";
        case DocType::CLASS: return "Class";
        case DocType::METHOD: return "Method";
        case DocType::VARIABLE: return "Variable";
        case DocType::CONSTANT: return "Constant";
        case DocType::PARAMETER: return "Parameter";
        case DocType::RETURN_VALUE: return "Return Value";
        case DocType::MODULE: return "Module";
        case DocType::EXAMPLE: return "Example";
        default: return "Unknown";
    }
}

std::string doc_type_to_string_ar(DocType type) {
    switch (type) {
        case DocType::FUNCTION: return "دالة";
        case DocType::CLASS: return "صنف";
        case DocType::METHOD: return "وظيفة";
        case DocType::VARIABLE: return "متغير";
        case DocType::CONSTANT: return "ثابت";
        case DocType::PARAMETER: return "معامل";
        case DocType::RETURN_VALUE: return "قيمة الإرجاع";
        case DocType::MODULE: return "وحدة";
        case DocType::EXAMPLE: return "مثال";
        default: return "غير معروف";
    }
}

std::string escape_html(const std::string& text) {
    std::string result;
    result.reserve(text.size() * 1.1);
    
    for (char c : text) {
        switch (c) {
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '&': result += "&amp;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c;
        }
    }
    
    return result;
}

bool ensure_directory_exists(const std::string& path) {
    try {
        std::filesystem::create_directories(path);
        return true;
    } catch (...) {
        return false;
    }
}

std::optional<std::string> read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        return std::nullopt;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool write_file(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file) {
        return false;
    }
    
    file << content;
    return true;
}

} // namespace docgen
} // namespace sad
