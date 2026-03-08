/**
 * =============================================================================
 * ملف: doc_comment_parser.cpp
 * الوصف: محلل تعليقات التوثيق (///)
 * المهمة: T227 - Doc comment parser
 * المرحلة: Phase 23 - User Story 20 (Documentation System ص وثّق)
 * =============================================================================
 * 
 * 📚 دليل المبتدئ لتعليقات التوثيق
 * ═════════════════════════════════════
 * 
 * ما هي تعليقات التوثيق؟
 * ──────────────────────────
 * تعليقات خاصة تبدأ بـ /// تُستخدم لشرح الكود.
 * يمكن تحويلها تلقائياً إلى صفحات توثيق!
 * 
 * الفرق عن التعليقات العادية:
 * ─────────────────────────────
 *   # تعليق عادي - للمبرمج فقط
 *   /// تعليق توثيقي - يظهر في الوثائق
 * 
 * بنية تعليق التوثيق:
 * ──────────────────────
 * 
 * /// # عنوان الدالة
 * /// 
 * /// وصف مختصر للدالة.
 * /// 
 * /// ## المعطيات
 * /// - `اسم`: وصف المعطى
 * /// - `عمر`: وصف آخر
 * /// 
 * /// ## القيمة المرجعة
 * /// وصف ما ترجعه الدالة
 * /// 
 * /// ## أمثلة
 * /// ```ص
 * /// نتيجة = دالتي(5)
 * /// ```
 * /// 
 * /// ## ملاحظات
 * /// - ملاحظة 1
 * /// - ملاحظة 2
 * 
 * علامات خاصة:
 * ──────────────
 * @معطى name - وصف معطى
 * @أرجع - وصف القيمة المرجعة
 * @مثال - مثال استخدام
 * @انظر - مرجع لدالة أخرى
 * @نسخة - نسخة الإضافة
 * @منذ - متى أُضيفت
 * @مهمل - إذا كان قديم
 * @تحذير - تحذير مهم
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <regex>
#include <sstream>
#include <iostream>
#include <optional>
#include <algorithm>

namespace sad::docs {

// =============================================================================
// أنواع العناصر الموثقة
// =============================================================================

enum class DocElementType {
    Function,       // دالة
    Class,          // صنف
    Struct,         // هيكل
    Enum,           // تعداد
    Constant,       // ثابت
    Variable,       // متغير
    Module,         // وحدة
    Property,       // خاصية
    Method,         // طريقة
    Interface,      // واجهة
    Type            // نوع
};

inline std::string elementTypeToArabic(DocElementType type) {
    switch (type) {
        case DocElementType::Function:  return "دالة";
        case DocElementType::Class:     return "صنف";
        case DocElementType::Struct:    return "هيكل";
        case DocElementType::Enum:      return "تعداد";
        case DocElementType::Constant:  return "ثابت";
        case DocElementType::Variable:  return "متغير";
        case DocElementType::Module:    return "وحدة";
        case DocElementType::Property:  return "خاصية";
        case DocElementType::Method:    return "طريقة";
        case DocElementType::Interface: return "واجهة";
        case DocElementType::Type:      return "نوع";
    }
    return "غير معروف";
}

// =============================================================================
// علامات التوثيق
// =============================================================================

struct DocTag {
    std::string name;       // اسم العلامة (معطى، أرجع، مثال...)
    std::string value;      // القيمة (اسم المعطى مثلاً)
    std::string description;// الوصف
    int line = 0;
};

// =============================================================================
// معطى موثق
// =============================================================================

struct DocParameter {
    std::string name;           // اسم المعطى
    std::string type;           // نوعه
    std::string description;    // وصفه
    bool optional = false;      // اختياري؟
    std::string defaultValue;   // قيمة افتراضية
};

// =============================================================================
// مثال
// =============================================================================

struct DocExample {
    std::string title;      // عنوان المثال
    std::string code;       // الكود
    std::string language;   // اللغة (ص، python، etc.)
    std::string output;     // الناتج المتوقع
    bool testable = false;  // قابل للاختبار؟
};

// =============================================================================
// كتلة التوثيق الكاملة
// =============================================================================

struct DocBlock {
    // معلومات أساسية
    std::string summary;        // ملخص مختصر
    std::string description;    // وصف مفصل
    DocElementType elementType = DocElementType::Function;
    std::string elementName;    // اسم العنصر
    
    // الموقع
    std::string file;
    int startLine = 0;
    int endLine = 0;
    
    // المعطيات والإرجاع
    std::vector<DocParameter> parameters;
    std::string returnType;
    std::string returnDescription;
    
    // الأمثلة
    std::vector<DocExample> examples;
    
    // الاستثناءات
    std::vector<std::pair<std::string, std::string>> exceptions; // نوع، وصف
    
    // العلامات الخاصة
    std::vector<std::string> seeAlso;   // انظر أيضاً
    std::string since;                   // منذ نسخة
    std::string deprecated;              // مهمل
    std::string author;                  // المؤلف
    std::vector<std::string> notes;      // ملاحظات
    std::vector<std::string> warnings;   // تحذيرات
    
    // Markdown خام
    std::string rawMarkdown;
    
    // الوسوم
    std::vector<DocTag> tags;
    
    /**
     * هل الكتلة فارغة؟
     */
    bool isEmpty() const {
        return summary.empty() && description.empty() && parameters.empty();
    }
    
    /**
     * تحويل إلى Markdown
     */
    std::string toMarkdown() const {
        std::ostringstream ss;
        
        // العنوان
        ss << "# " << elementTypeToArabic(elementType) << " `" << elementName << "`\n\n";
        
        // الملخص
        if (!summary.empty()) {
            ss << summary << "\n\n";
        }
        
        // الوصف
        if (!description.empty()) {
            ss << description << "\n\n";
        }
        
        // المعطيات
        if (!parameters.empty()) {
            ss << "## المعطيات\n\n";
            ss << "| الاسم | النوع | الوصف |\n";
            ss << "|-------|------|-------|\n";
            for (const auto& p : parameters) {
                ss << "| `" << p.name << "` | " << p.type << " | " << p.description;
                if (p.optional) ss << " (اختياري)";
                if (!p.defaultValue.empty()) ss << " [= " << p.defaultValue << "]";
                ss << " |\n";
            }
            ss << "\n";
        }
        
        // القيمة المرجعة
        if (!returnType.empty() || !returnDescription.empty()) {
            ss << "## القيمة المرجعة\n\n";
            if (!returnType.empty()) {
                ss << "**النوع**: `" << returnType << "`\n\n";
            }
            if (!returnDescription.empty()) {
                ss << returnDescription << "\n\n";
            }
        }
        
        // الاستثناءات
        if (!exceptions.empty()) {
            ss << "## الاستثناءات\n\n";
            for (const auto& [type, desc] : exceptions) {
                ss << "- **" << type << "**: " << desc << "\n";
            }
            ss << "\n";
        }
        
        // الأمثلة
        if (!examples.empty()) {
            ss << "## أمثلة\n\n";
            for (const auto& ex : examples) {
                if (!ex.title.empty()) {
                    ss << "### " << ex.title << "\n\n";
                }
                ss << "```" << ex.language << "\n";
                ss << ex.code << "\n";
                ss << "```\n\n";
                if (!ex.output.empty()) {
                    ss << "**الناتج**:\n```\n" << ex.output << "\n```\n\n";
                }
            }
        }
        
        // الملاحظات
        if (!notes.empty()) {
            ss << "## ملاحظات\n\n";
            for (const auto& note : notes) {
                ss << "- " << note << "\n";
            }
            ss << "\n";
        }
        
        // التحذيرات
        if (!warnings.empty()) {
            ss << "## ⚠️ تحذيرات\n\n";
            for (const auto& w : warnings) {
                ss << "> ⚠️ " << w << "\n";
            }
            ss << "\n";
        }
        
        // انظر أيضاً
        if (!seeAlso.empty()) {
            ss << "## انظر أيضاً\n\n";
            for (const auto& ref : seeAlso) {
                ss << "- [`" << ref << "`](" << ref << ".html)\n";
            }
            ss << "\n";
        }
        
        // معلومات إضافية
        if (!since.empty() || !deprecated.empty() || !author.empty()) {
            ss << "---\n\n";
            if (!since.empty()) ss << "**منذ نسخة**: " << since << "\n\n";
            if (!deprecated.empty()) ss << "**⚠️ مهمل**: " << deprecated << "\n\n";
            if (!author.empty()) ss << "**المؤلف**: " << author << "\n\n";
        }
        
        return ss.str();
    }
};

// =============================================================================
// محلل تعليقات التوثيق
// =============================================================================

class DocCommentParser {
public:
    /**
     * تحليل تعليقات التوثيق من ملف
     */
    std::vector<DocBlock> parseFile(const std::string& content, const std::string& filename) {
        std::vector<DocBlock> blocks;
        std::vector<std::string> lines;
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            lines.push_back(line);
        }
        
        DocBlock currentBlock;
        bool inDocBlock = false;
        int blockStartLine = 0;
        std::vector<std::string> docLines;
        
        for (size_t i = 0; i < lines.size(); i++) {
            const std::string& line = lines[i];
            std::string trimmed = trim(line);
            
            // بداية تعليق توثيقي
            if (trimmed.substr(0, 3) == "///") {
                if (!inDocBlock) {
                    inDocBlock = true;
                    blockStartLine = static_cast<int>(i + 1);
                    docLines.clear();
                }
                
                // إزالة /// والمسافة
                std::string docContent = trimmed.substr(3);
                if (!docContent.empty() && docContent[0] == ' ') {
                    docContent = docContent.substr(1);
                }
                docLines.push_back(docContent);
            }
            // نهاية التعليق - بداية الكود
            else if (inDocBlock && !trimmed.empty() && trimmed[0] != '#') {
                currentBlock = parseDocLines(docLines);
                currentBlock.file = filename;
                currentBlock.startLine = blockStartLine;
                currentBlock.endLine = static_cast<int>(i);
                
                // استخراج اسم ونوع العنصر من السطر التالي
                extractElementInfo(trimmed, currentBlock);
                
                if (!currentBlock.isEmpty()) {
                    blocks.push_back(currentBlock);
                }
                
                inDocBlock = false;
                docLines.clear();
            }
            // استمرار بدون كود (ملف فارغ)
            else if (inDocBlock && trimmed.empty()) {
                // سطر فارغ داخل التوثيق
                docLines.push_back("");
            }
        }
        
        return blocks;
    }
    
    /**
     * تحليل تعليق واحد
     */
    DocBlock parseDocComment(const std::string& comment) {
        std::vector<std::string> lines;
        std::istringstream stream(comment);
        std::string line;
        
        while (std::getline(stream, line)) {
            std::string trimmed = trim(line);
            if (trimmed.substr(0, 3) == "///") {
                lines.push_back(trimmed.substr(3));
            } else {
                lines.push_back(trimmed);
            }
        }
        
        return parseDocLines(lines);
    }
    
private:
    /**
     * تحليل أسطر التوثيق
     */
    DocBlock parseDocLines(const std::vector<std::string>& lines) {
        DocBlock block;
        std::ostringstream rawMarkdown;
        std::ostringstream currentSection;
        std::string currentSectionName;
        
        bool inCodeBlock = false;
        std::string codeBlockLang;
        std::ostringstream codeBlockContent;
        
        for (const auto& line : lines) {
            rawMarkdown << line << "\n";
            
            // كتلة كود
            if (line.substr(0, 3) == "```") {
                if (!inCodeBlock) {
                    inCodeBlock = true;
                    codeBlockLang = line.substr(3);
                    codeBlockContent.str("");
                } else {
                    inCodeBlock = false;
                    
                    DocExample example;
                    example.code = trim(codeBlockContent.str());
                    example.language = codeBlockLang.empty() ? "ص" : codeBlockLang;
                    block.examples.push_back(example);
                }
                continue;
            }
            
            if (inCodeBlock) {
                codeBlockContent << line << "\n";
                continue;
            }
            
            // علامات @
            if (!line.empty() && line[0] == '@') {
                parseTag(line, block);
                continue;
            }
            
            // عناوين Markdown
            if (line.substr(0, 2) == "# ") {
                block.summary = line.substr(2);
            }
            else if (line.substr(0, 3) == "## ") {
                // حفظ القسم السابق
                if (!currentSectionName.empty()) {
                    processSectionContent(currentSectionName, currentSection.str(), block);
                }
                currentSectionName = line.substr(3);
                currentSection.str("");
            }
            else if (line.substr(0, 4) == "### ") {
                // عنوان فرعي
                currentSection << line << "\n";
            }
            else {
                // محتوى
                if (block.summary.empty() && !line.empty()) {
                    block.summary = line;
                } else {
                    currentSection << line << "\n";
                }
            }
        }
        
        // معالجة آخر قسم
        if (!currentSectionName.empty()) {
            processSectionContent(currentSectionName, currentSection.str(), block);
        }
        
        // إذا لم يكن هناك وصف، استخدم المحتوى
        if (block.description.empty() && !currentSection.str().empty()) {
            block.description = trim(currentSection.str());
        }
        
        block.rawMarkdown = rawMarkdown.str();
        return block;
    }
    
    /**
     * تحليل علامة @
     */
    void parseTag(const std::string& line, DocBlock& block) {
        // @معطى name وصف
        std::regex paramRegex(R"(@(?:معطى|param|parameter)\s+(\S+)\s*(.*))");
        std::smatch match;
        
        if (std::regex_search(line, match, paramRegex)) {
            DocParameter param;
            param.name = match[1];
            param.description = match[2];
            block.parameters.push_back(param);
            return;
        }
        
        // @أرجع وصف
        std::regex returnRegex(R"(@(?:أرجع|return|returns)\s*(.*))");
        if (std::regex_search(line, match, returnRegex)) {
            block.returnDescription = match[1];
            return;
        }
        
        // @استثناء نوع وصف
        std::regex throwsRegex(R"(@(?:استثناء|throws|exception)\s+(\S+)\s*(.*))");
        if (std::regex_search(line, match, throwsRegex)) {
            block.exceptions.push_back({match[1], match[2]});
            return;
        }
        
        // @انظر مرجع
        std::regex seeRegex(R"(@(?:انظر|see)\s+(\S+))");
        if (std::regex_search(line, match, seeRegex)) {
            block.seeAlso.push_back(match[1]);
            return;
        }
        
        // @منذ نسخة
        std::regex sinceRegex(R"(@(?:منذ|since)\s+(\S+))");
        if (std::regex_search(line, match, sinceRegex)) {
            block.since = match[1];
            return;
        }
        
        // @مهمل سبب
        std::regex deprecatedRegex(R"(@(?:مهمل|deprecated)\s*(.*))");
        if (std::regex_search(line, match, deprecatedRegex)) {
            block.deprecated = match[1].str().empty() ? "مهمل" : match[1].str();
            return;
        }
        
        // @مؤلف اسم
        std::regex authorRegex(R"(@(?:مؤلف|author)\s*(.*))");
        if (std::regex_search(line, match, authorRegex)) {
            block.author = match[1];
            return;
        }
        
        // @ملاحظة نص
        std::regex noteRegex(R"(@(?:ملاحظة|note)\s*(.*))");
        if (std::regex_search(line, match, noteRegex)) {
            block.notes.push_back(match[1]);
            return;
        }
        
        // @تحذير نص
        std::regex warnRegex(R"(@(?:تحذير|warning)\s*(.*))");
        if (std::regex_search(line, match, warnRegex)) {
            block.warnings.push_back(match[1]);
            return;
        }
        
        // علامة عامة
        DocTag tag;
        std::regex genericRegex(R"(@(\S+)\s*(.*))");
        if (std::regex_search(line, match, genericRegex)) {
            tag.name = match[1];
            tag.description = match[2];
            block.tags.push_back(tag);
        }
    }
    
    /**
     * معالجة محتوى قسم
     */
    void processSectionContent(const std::string& sectionName, 
                               const std::string& content,
                               DocBlock& block) {
        std::string name = sectionName;
        // تحويل للمقارنة
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        
        if (name == "المعطيات" || name == "parameters" || name == "params") {
            parseParametersSection(content, block);
        }
        else if (name == "القيمة المرجعة" || name == "returns" || name == "return") {
            block.returnDescription = trim(content);
        }
        else if (name == "أمثلة" || name == "examples") {
            // تم معالجتها في كتل الكود
        }
        else if (name == "ملاحظات" || name == "notes") {
            parseListSection(content, block.notes);
        }
        else if (name == "تحذيرات" || name == "warnings") {
            parseListSection(content, block.warnings);
        }
        else if (name == "انظر أيضاً" || name == "see also") {
            parseListSection(content, block.seeAlso);
        }
        else {
            // قسم مخصص
            if (block.description.empty()) {
                block.description = trim(content);
            } else {
                block.description += "\n\n## " + sectionName + "\n\n" + content;
            }
        }
    }
    
    /**
     * تحليل قسم المعطيات
     */
    void parseParametersSection(const std::string& content, DocBlock& block) {
        std::istringstream stream(content);
        std::string line;
        
        // نمط: - `name`: وصف أو - name: وصف
        std::regex paramRegex(R"(-\s*`?(\w+)`?\s*[:-]?\s*(.*))");
        
        while (std::getline(stream, line)) {
            std::smatch match;
            if (std::regex_search(line, match, paramRegex)) {
                DocParameter param;
                param.name = match[1];
                param.description = match[2];
                
                // البحث عن نوع
                std::regex typeRegex(R"(\((\w+)\))");
                std::smatch typeMatch;
                if (std::regex_search(line, typeMatch, typeRegex)) {
                    param.type = typeMatch[1];
                }
                
                block.parameters.push_back(param);
            }
        }
    }
    
    /**
     * تحليل قسم قائمة
     */
    void parseListSection(const std::string& content, std::vector<std::string>& list) {
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            std::string trimmed = trim(line);
            if (trimmed.empty()) continue;
            
            // إزالة - أو * في البداية
            if (trimmed[0] == '-' || trimmed[0] == '*') {
                trimmed = trim(trimmed.substr(1));
            }
            
            if (!trimmed.empty()) {
                list.push_back(trimmed);
            }
        }
    }
    
    /**
     * استخراج معلومات العنصر من الكود
     */
    void extractElementInfo(const std::string& codeLine, DocBlock& block) {
        // دالة/function
        std::regex funcRegex(R"((?:دالة|function|def|fn)\s+(\w+))");
        std::smatch match;
        
        if (std::regex_search(codeLine, match, funcRegex)) {
            block.elementType = DocElementType::Function;
            block.elementName = match[1];
            return;
        }
        
        // صنف/class
        std::regex classRegex(R"((?:صنف|class|struct)\s+(\w+))");
        if (std::regex_search(codeLine, match, classRegex)) {
            block.elementType = DocElementType::Class;
            block.elementName = match[1];
            return;
        }
        
        // تعداد/enum
        std::regex enumRegex(R"((?:تعداد|enum)\s+(\w+))");
        if (std::regex_search(codeLine, match, enumRegex)) {
            block.elementType = DocElementType::Enum;
            block.elementName = match[1];
            return;
        }
        
        // ثابت/const
        std::regex constRegex(R"((?:ثابت|const)\s+(\w+))");
        if (std::regex_search(codeLine, match, constRegex)) {
            block.elementType = DocElementType::Constant;
            block.elementName = match[1];
            return;
        }
        
        // متغير
        std::regex varRegex(R"((\w+)\s*[=:])");
        if (std::regex_search(codeLine, match, varRegex)) {
            block.elementType = DocElementType::Variable;
            block.elementName = match[1];
        }
    }
    
    /**
     * إزالة المسافات
     */
    std::string trim(const std::string& s) {
        size_t start = s.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\n\r");
        return s.substr(start, end - start + 1);
    }
};

} // namespace sad::docs

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadDocParser {
    sad::docs::DocCommentParser* parser;
};

SadDocParser* sad_doc_parser_new() {
    auto* ctx = new SadDocParser();
    ctx->parser = new sad::docs::DocCommentParser();
    return ctx;
}

void sad_doc_parser_free(SadDocParser* ctx) {
    if (ctx) {
        delete ctx->parser;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef DOC_COMMENT_PARSER_TEST

#include <cassert>

void testBasicParsing() {
    sad::docs::DocCommentParser parser;
    
    std::string code = R"(
/// # جمع عددين
/// 
/// تجمع هذه الدالة عددين وترجع النتيجة.
/// 
/// @معطى أ العدد الأول
/// @معطى ب العدد الثاني
/// @أرجع مجموع العددين
دالة جمع(أ: عدد, ب: عدد) -> عدد
    أرجع أ + ب
نهاية
)";
    
    auto blocks = parser.parseFile(code, "test.ص");
    
    assert(blocks.size() >= 1);
    assert(blocks[0].elementName == "جمع");
    assert(blocks[0].parameters.size() == 2);
    
    std::cout << "✅ اختبار التحليل الأساسي نجح\n";
}

void testMarkdownGeneration() {
    sad::docs::DocCommentParser parser;
    
    std::string code = R"(
/// # قراءة ملف
/// 
/// تقرأ محتويات ملف وترجعها كنص.
/// 
/// @معطى مسار مسار الملف
/// @أرجع محتوى الملف
/// @استثناء خطأ_ملف إذا لم يُعثر على الملف
/// @مثال
/// ```ص
/// محتوى = اقرأ_ملف("بيانات.txt")
/// اطبع(محتوى)
/// ```
دالة اقرأ_ملف(مسار: نص) -> نص
    # ...
نهاية
)";
    
    auto blocks = parser.parseFile(code, "test.ص");
    
    assert(blocks.size() >= 1);
    
    std::string markdown = blocks[0].toMarkdown();
    assert(markdown.find("قراءة ملف") != std::string::npos);
    assert(markdown.find("المعطيات") != std::string::npos);
    
    std::cout << "✅ اختبار توليد Markdown نجح\n";
    std::cout << "\n--- Markdown المولد ---\n" << markdown << "\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات محلل تعليقات التوثيق\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testBasicParsing();
    testMarkdownGeneration();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // DOC_COMMENT_PARSER_TEST
