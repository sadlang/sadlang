/**
 * =============================================================================
 * ملف: arabic_markdown.cpp
 * الوصف: معالج Markdown العربي
 * المهمة: T228 - Arabic markdown processor
 * المرحلة: Phase 23 - User Story 20 (Documentation System ص وثّق)
 * =============================================================================
 * 
 * 📝 دليل المبتدئ لـ Markdown
 * ════════════════════════════════
 * 
 * ما هو Markdown؟
 * ─────────────────
 * لغة تنسيق بسيطة لكتابة النصوص.
 * تكتب نصاً عادياً برموز بسيطة، ويتحول إلى صفحة جميلة!
 * 
 * رموز Markdown الأساسية:
 * ──────────────────────────
 * 
 * # عنوان كبير
 * ## عنوان أصغر
 * ### عنوان صغير
 * 
 * **نص عريض**
 * *نص مائل*
 * `كود`
 * 
 * - عنصر قائمة
 * - عنصر آخر
 * 
 * 1. قائمة مرقمة
 * 2. العنصر الثاني
 * 
 * [رابط](https://example.com)
 * ![صورة](image.png)
 * 
 * ```ص
 * كود متعدد الأسطر
 * ```
 * 
 * > اقتباس
 * 
 * ---
 * خط فاصل
 * 
 * ميزات خاصة بالعربية:
 * ──────────────────────
 * 
 * 1. اتجاه RTL تلقائي
 * 2. دعم الخطوط العربية
 * 3. أرقام عربية (٠١٢٣) أو هندية
 * 4. علامات ترقيم عربية
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <regex>
#include <sstream>
#include <iostream>
#include <functional>

namespace sad::docs {

// =============================================================================
// خيارات المعالج
// =============================================================================

struct MarkdownOptions {
    bool rtl = true;                    // اتجاه من اليمين لليسار
    bool arabicNumerals = false;        // أرقام عربية (٠١٢...)
    bool syntaxHighlight = true;        // تلوين الكود
    bool tableOfContents = true;        // جدول محتويات
    bool linkTargetBlank = true;        // روابط خارجية في تبويب جديد
    bool autoLinkify = true;            // تحويل URLs لروابط
    bool safeHTML = true;               // تنظيف HTML
    std::string codeTheme = "dark";     // سمة الكود
    std::string defaultLanguage = "ص";  // لغة افتراضية للكود
};

// =============================================================================
// عنصر AST
// =============================================================================

enum class MarkdownNodeType {
    Document,
    Heading,
    Paragraph,
    Text,
    Bold,
    Italic,
    Code,
    CodeBlock,
    Link,
    Image,
    List,
    ListItem,
    OrderedList,
    Blockquote,
    HorizontalRule,
    Table,
    TableRow,
    TableCell,
    LineBreak,
    HTML,
    Alert,          // تنبيه خاص
    Note,           // ملاحظة
    Warning,        // تحذير
    Tip,            // نصيحة
    CodeRef         // مرجع كود
};

struct MarkdownNode {
    MarkdownNodeType type;
    std::string content;
    std::map<std::string, std::string> attributes;
    std::vector<MarkdownNode> children;
    int level = 0;  // للعناوين
};

// =============================================================================
// معالج Markdown العربي
// =============================================================================

class ArabicMarkdownProcessor {
public:
    ArabicMarkdownProcessor(const MarkdownOptions& options = {})
        : options_(options) {}
    
    /**
     * تحويل Markdown إلى HTML
     */
    std::string toHTML(const std::string& markdown) {
        // مرحلة 1: تحليل إلى AST
        MarkdownNode document = parse(markdown);
        
        // مرحلة 2: توليد HTML
        std::string html = generateHTML(document);
        
        // مرحلة 3: إضافة غلاف
        return wrapHTML(html);
    }
    
    /**
     * استخراج جدول المحتويات
     */
    std::vector<std::pair<int, std::string>> extractTOC(const std::string& markdown) {
        std::vector<std::pair<int, std::string>> toc;
        std::istringstream stream(markdown);
        std::string line;
        
        while (std::getline(stream, line)) {
            // عناوين ATX (# ## ###)
            std::regex headingRegex(R"(^(#{1,6})\s+(.+)$)");
            std::smatch match;
            
            if (std::regex_search(line, match, headingRegex)) {
                int level = static_cast<int>(match[1].str().length());
                std::string title = match[2];
                toc.push_back({level, title});
            }
        }
        
        return toc;
    }
    
private:
    MarkdownOptions options_;
    
    /**
     * تحليل Markdown إلى AST
     */
    MarkdownNode parse(const std::string& markdown) {
        MarkdownNode document;
        document.type = MarkdownNodeType::Document;
        
        std::istringstream stream(markdown);
        std::string line;
        std::vector<std::string> buffer;
        
        bool inCodeBlock = false;
        std::string codeBlockLang;
        std::ostringstream codeContent;
        
        bool inList = false;
        bool orderedList = false;
        std::vector<MarkdownNode> listItems;
        
        while (std::getline(stream, line)) {
            // كتلة كود
            if (line.substr(0, 3) == "```") {
                if (!inCodeBlock) {
                    inCodeBlock = true;
                    codeBlockLang = line.substr(3);
                    codeContent.str("");
                } else {
                    inCodeBlock = false;
                    
                    MarkdownNode codeNode;
                    codeNode.type = MarkdownNodeType::CodeBlock;
                    codeNode.content = codeContent.str();
                    codeNode.attributes["language"] = codeBlockLang.empty() ? 
                        options_.defaultLanguage : codeBlockLang;
                    
                    document.children.push_back(codeNode);
                }
                continue;
            }
            
            if (inCodeBlock) {
                codeContent << line << "\n";
                continue;
            }
            
            // عناوين
            std::regex headingRegex(R"(^(#{1,6})\s+(.+)$)");
            std::smatch match;
            
            if (std::regex_search(line, match, headingRegex)) {
                // إنهاء القائمة السابقة
                finishList(document, inList, orderedList, listItems);
                // إنهاء الفقرة السابقة
                finishParagraph(document, buffer);
                
                MarkdownNode heading;
                heading.type = MarkdownNodeType::Heading;
                heading.level = static_cast<int>(match[1].str().length());
                heading.content = match[2];
                heading.attributes["id"] = generateId(match[2]);
                
                document.children.push_back(heading);
                continue;
            }
            
            // قوائم
            std::regex listRegex(R"(^[-*+]\s+(.+)$)");
            std::regex orderedListRegex(R"(^\d+\.\s+(.+)$)");
            
            if (std::regex_search(line, match, listRegex)) {
                if (!inList) {
                    finishParagraph(document, buffer);
                    inList = true;
                    orderedList = false;
                }
                
                MarkdownNode item;
                item.type = MarkdownNodeType::ListItem;
                item.content = match[1];
                listItems.push_back(item);
                continue;
            }
            
            if (std::regex_search(line, match, orderedListRegex)) {
                if (!inList) {
                    finishParagraph(document, buffer);
                    inList = true;
                    orderedList = true;
                }
                
                MarkdownNode item;
                item.type = MarkdownNodeType::ListItem;
                item.content = match[1];
                listItems.push_back(item);
                continue;
            }
            
            // نهاية القائمة
            if (inList && line.empty()) {
                finishList(document, inList, orderedList, listItems);
                continue;
            }
            
            // اقتباس
            if (line.substr(0, 2) == "> ") {
                finishParagraph(document, buffer);
                finishList(document, inList, orderedList, listItems);
                
                MarkdownNode quote;
                quote.type = MarkdownNodeType::Blockquote;
                quote.content = line.substr(2);
                
                document.children.push_back(quote);
                continue;
            }
            
            // خط فاصل
            if (line == "---" || line == "***" || line == "___") {
                finishParagraph(document, buffer);
                finishList(document, inList, orderedList, listItems);
                
                MarkdownNode hr;
                hr.type = MarkdownNodeType::HorizontalRule;
                document.children.push_back(hr);
                continue;
            }
            
            // تنبيهات خاصة
            std::regex alertRegex(R"(^:::(\w+)\s*(.*)$)");
            if (std::regex_search(line, match, alertRegex)) {
                finishParagraph(document, buffer);
                
                std::string alertType = match[1];
                std::string alertText = match[2];
                
                MarkdownNode alert;
                if (alertType == "tip" || alertType == "نصيحة") {
                    alert.type = MarkdownNodeType::Tip;
                } else if (alertType == "warning" || alertType == "تحذير") {
                    alert.type = MarkdownNodeType::Warning;
                } else if (alertType == "note" || alertType == "ملاحظة") {
                    alert.type = MarkdownNodeType::Note;
                } else {
                    alert.type = MarkdownNodeType::Alert;
                }
                alert.content = alertText;
                
                document.children.push_back(alert);
                continue;
            }
            
            // سطر فارغ
            if (line.empty()) {
                finishParagraph(document, buffer);
                continue;
            }
            
            // نص عادي
            buffer.push_back(line);
        }
        
        // إنهاء أي محتوى متبقي
        finishParagraph(document, buffer);
        finishList(document, inList, orderedList, listItems);
        
        return document;
    }
    
    /**
     * إنهاء فقرة
     */
    void finishParagraph(MarkdownNode& document, std::vector<std::string>& buffer) {
        if (buffer.empty()) return;
        
        std::ostringstream ss;
        for (const auto& line : buffer) {
            ss << line << " ";
        }
        
        MarkdownNode para;
        para.type = MarkdownNodeType::Paragraph;
        para.content = ss.str();
        
        document.children.push_back(para);
        buffer.clear();
    }
    
    /**
     * إنهاء قائمة
     */
    void finishList(MarkdownNode& document, bool& inList, bool ordered,
                    std::vector<MarkdownNode>& items) {
        if (!inList) return;
        
        MarkdownNode list;
        list.type = ordered ? MarkdownNodeType::OrderedList : MarkdownNodeType::List;
        list.children = items;
        
        document.children.push_back(list);
        
        inList = false;
        items.clear();
    }
    
    /**
     * توليد معرف فريد للعنوان
     */
    std::string generateId(const std::string& text) {
        std::string id = text;
        // استبدال المسافات بشرطات
        for (char& c : id) {
            if (c == ' ') c = '-';
        }
        return id;
    }
    
    /**
     * توليد HTML من AST
     */
    std::string generateHTML(const MarkdownNode& node) {
        std::ostringstream ss;
        
        switch (node.type) {
            case MarkdownNodeType::Document:
                for (const auto& child : node.children) {
                    ss << generateHTML(child);
                }
                break;
                
            case MarkdownNodeType::Heading: {
                std::string tag = "h" + std::to_string(node.level);
                std::string id = node.attributes.count("id") ? 
                    " id=\"" + node.attributes.at("id") + "\"" : "";
                ss << "<" << tag << id << ">" 
                   << processInline(node.content) 
                   << "</" << tag << ">\n";
                break;
            }
            
            case MarkdownNodeType::Paragraph:
                ss << "<p>" << processInline(node.content) << "</p>\n";
                break;
                
            case MarkdownNodeType::CodeBlock: {
                std::string lang = node.attributes.count("language") ? 
                    node.attributes.at("language") : "";
                ss << "<pre><code class=\"language-" << lang << "\">";
                ss << escapeHTML(node.content);
                ss << "</code></pre>\n";
                break;
            }
            
            case MarkdownNodeType::List:
                ss << "<ul>\n";
                for (const auto& item : node.children) {
                    ss << "<li>" << processInline(item.content) << "</li>\n";
                }
                ss << "</ul>\n";
                break;
                
            case MarkdownNodeType::OrderedList:
                ss << "<ol>\n";
                for (const auto& item : node.children) {
                    ss << "<li>" << processInline(item.content) << "</li>\n";
                }
                ss << "</ol>\n";
                break;
                
            case MarkdownNodeType::Blockquote:
                ss << "<blockquote>" << processInline(node.content) << "</blockquote>\n";
                break;
                
            case MarkdownNodeType::HorizontalRule:
                ss << "<hr>\n";
                break;
                
            case MarkdownNodeType::Tip:
                ss << "<div class=\"alert alert-tip\">💡 " 
                   << processInline(node.content) << "</div>\n";
                break;
                
            case MarkdownNodeType::Warning:
                ss << "<div class=\"alert alert-warning\">⚠️ " 
                   << processInline(node.content) << "</div>\n";
                break;
                
            case MarkdownNodeType::Note:
                ss << "<div class=\"alert alert-note\">📝 " 
                   << processInline(node.content) << "</div>\n";
                break;
                
            default:
                ss << node.content;
        }
        
        return ss.str();
    }
    
    /**
     * معالجة التنسيق السطري
     */
    std::string processInline(const std::string& text) {
        std::string result = text;
        
        // **عريض**
        result = std::regex_replace(result, 
            std::regex(R"(\*\*(.+?)\*\*)"), 
            "<strong>$1</strong>");
        
        // *مائل*
        result = std::regex_replace(result, 
            std::regex(R"(\*(.+?)\*)"), 
            "<em>$1</em>");
        
        // `كود`
        result = std::regex_replace(result, 
            std::regex(R"(`([^`]+)`)"), 
            "<code>$1</code>");
        
        // [رابط](url)
        result = std::regex_replace(result, 
            std::regex(R"(\[([^\]]+)\]\(([^\)]+)\))"), 
            "<a href=\"$2\">$1</a>");
        
        // ![صورة](url)
        result = std::regex_replace(result, 
            std::regex(R"(!\[([^\]]*)\]\(([^\)]+)\))"), 
            "<img src=\"$2\" alt=\"$1\">");
        
        // Auto-linkify URLs
        if (options_.autoLinkify) {
            result = std::regex_replace(result,
                std::regex(R"((https?://[^\s<>]+))"),
                "<a href=\"$1\">$1</a>");
        }
        
        return result;
    }
    
    /**
     * تغليف HTML
     */
    std::string wrapHTML(const std::string& content) {
        std::ostringstream ss;
        
        ss << R"(<!DOCTYPE html>
<html dir=")" << (options_.rtl ? "rtl" : "ltr") << R"(" lang="ar">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        :root {
            --bg: #1a1a2e;
            --text: #eee;
            --code-bg: #0f0f23;
            --heading: #4dabf7;
            --link: #69db7c;
            --border: #333;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Arial, sans-serif;
            background: var(--bg);
            color: var(--text);
            line-height: 1.8;
            max-width: 900px;
            margin: 0 auto;
            padding: 40px 20px;
        }
        
        h1, h2, h3, h4, h5, h6 {
            color: var(--heading);
            margin-top: 1.5em;
            margin-bottom: 0.5em;
        }
        
        h1 { font-size: 2.2em; border-bottom: 2px solid var(--border); padding-bottom: 10px; }
        h2 { font-size: 1.8em; }
        h3 { font-size: 1.4em; }
        
        a {
            color: var(--link);
            text-decoration: none;
        }
        
        a:hover {
            text-decoration: underline;
        }
        
        code {
            background: var(--code-bg);
            padding: 2px 6px;
            border-radius: 4px;
            font-family: 'Consolas', 'Monaco', monospace;
        }
        
        pre {
            background: var(--code-bg);
            padding: 15px;
            border-radius: 8px;
            overflow-x: auto;
        }
        
        pre code {
            padding: 0;
            background: none;
        }
        
        blockquote {
            border-right: 4px solid var(--heading);
            padding-right: 15px;
            margin-right: 0;
            color: #aaa;
            font-style: italic;
        }
        
        ul, ol {
            padding-right: 30px;
        }
        
        li {
            margin: 5px 0;
        }
        
        table {
            width: 100%;
            border-collapse: collapse;
            margin: 20px 0;
        }
        
        th, td {
            border: 1px solid var(--border);
            padding: 10px;
            text-align: right;
        }
        
        th {
            background: var(--code-bg);
        }
        
        .alert {
            padding: 15px;
            border-radius: 8px;
            margin: 15px 0;
        }
        
        .alert-tip {
            background: rgba(105, 219, 124, 0.1);
            border-right: 4px solid #69db7c;
        }
        
        .alert-warning {
            background: rgba(255, 212, 59, 0.1);
            border-right: 4px solid #ffd43b;
        }
        
        .alert-note {
            background: rgba(77, 171, 247, 0.1);
            border-right: 4px solid #4dabf7;
        }
        
        img {
            max-width: 100%;
            border-radius: 8px;
        }
        
        hr {
            border: none;
            border-top: 1px solid var(--border);
            margin: 30px 0;
        }
    </style>
</head>
<body>
)";
        
        ss << content;
        
        ss << R"(
</body>
</html>)";
        
        return ss.str();
    }
    
    /**
     * هروب HTML
     */
    std::string escapeHTML(const std::string& s) {
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
};

// =============================================================================
// دالة مساعدة لتحويل Markdown إلى HTML
// =============================================================================

std::string markdownToHTML(const std::string& markdown, 
                           const MarkdownOptions& options = {}) {
    ArabicMarkdownProcessor processor(options);
    return processor.toHTML(markdown);
}

} // namespace sad::docs

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

char* sad_markdown_to_html(const char* markdown, int rtl) {
    sad::docs::MarkdownOptions options;
    options.rtl = rtl != 0;
    
    std::string html = sad::docs::markdownToHTML(markdown, options);
    
    char* result = new char[html.size() + 1];
    std::strcpy(result, html.c_str());
    return result;
}

void sad_markdown_free(char* str) {
    delete[] str;
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef ARABIC_MARKDOWN_TEST

#include <cassert>

void testBasicConversion() {
    sad::docs::ArabicMarkdownProcessor processor;
    
    std::string markdown = R"(
# مرحباً بالعالم

هذه **فقرة** مع *نص مائل*.

## قائمة

- عنصر 1
- عنصر 2

## كود

```ص
اطبع("مرحباً")
```
)";
    
    std::string html = processor.toHTML(markdown);
    
    assert(html.find("<h1") != std::string::npos);
    assert(html.find("<strong>فقرة</strong>") != std::string::npos);
    assert(html.find("<ul>") != std::string::npos);
    assert(html.find("<code") != std::string::npos);
    
    std::cout << "✅ اختبار التحويل الأساسي نجح\n";
}

void testTOC() {
    sad::docs::ArabicMarkdownProcessor processor;
    
    std::string markdown = R"(
# العنوان الرئيسي
## القسم الأول
### القسم الفرعي
## القسم الثاني
)";
    
    auto toc = processor.extractTOC(markdown);
    
    assert(toc.size() == 4);
    assert(toc[0].first == 1);
    assert(toc[1].first == 2);
    
    std::cout << "✅ اختبار جدول المحتويات نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات معالج Markdown العربي\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testBasicConversion();
    testTOC();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // ARABIC_MARKDOWN_TEST
