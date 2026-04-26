// بسم الله الرحمن الرحيم
// ============================================================================
// docs_emitter.h — مُصدِّر التوثيق المتقدم للغة ص
// Advanced Documentation Emitter for Sad Language
// ============================================================================
//
// @brief (AR) نظام توثيق شامل يحلل كود لغة ص ويولد توثيقاً احترافياً
//             بصيغ HTML وMarkdown وJSON مع بحث ومراجع متقاطعة وسمات متعددة.
// @brief (EN) Comprehensive documentation system that parses Sad language code
//             and generates professional docs in HTML, Markdown, JSON formats
//             with search, cross-references, and multiple themes.
//
// ============================================================================

#ifndef SAD_DOCS_EMITTER_H
#define SAD_DOCS_EMITTER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <optional>
#include <functional>
#include <chrono>
#include <filesystem>

namespace Sad {
namespace Meta {

// ============================================================================
// (AR) تنسيقات الإخراج المدعومة
// (EN) Supported output formats
// ============================================================================
enum class DocFormat {
    HTML,       ///< موقع HTML كامل مع CSS وبحث
    Markdown,   ///< ملفات Markdown
    JSON,       ///< بيانات JSON منظمة
    All         ///< جميع التنسيقات
};

// ============================================================================
// (AR) نوع عنصر التوثيق
// (EN) Documentation item kind
// ============================================================================
enum class DocItemKind {
    Function,       ///< دالة / function
    Class,          ///< صنف / class
    Struct,         ///< هيكل / struct
    Enum,           ///< تعداد / enum
    Trait,          ///< سمة / trait
    Interface,      ///< واجهة / interface
    Variable,       ///< متغير / variable
    Constant,       ///< ثابت / constant
    Module,         ///< وحدة / module
    Property,       ///< خاصية / property
    Constructor,    ///< باني / constructor
    Method,         ///< تابع / method
    TypeAlias       ///< اسم_بديل / type alias
};

// ============================================================================
// (AR) مستوى الوصول
// (EN) Access level
// ============================================================================
enum class AccessLevel {
    Public,     ///< عام
    Private,    ///< خاص
    Protected,  ///< محمي
    Internal    ///< داخلي
};

// ============================================================================
// (AR) وسم التوثيق المحلل
// (EN) Parsed documentation tag
// ============================================================================
struct DocTag {
    std::string name;       ///< اسم الوسم: @معطى, @أرجع, @مثال...
    std::string paramName;  ///< اسم المعطى (للوسوم ذات المعطيات)
    std::string content;    ///< محتوى الوسم
};

// ============================================================================
// (AR) معلومات المعطى
// (EN) Parameter info
// ============================================================================
struct ParamInfo {
    std::string name;           ///< اسم المعطى
    std::string type;           ///< نوع المعطى
    std::string description;    ///< وصف المعطى
    std::string defaultValue;   ///< القيمة الافتراضية
    bool isOptional = false;    ///< اختياري؟
};

// ============================================================================
// (AR) عنصر التوثيق الشامل
// (EN) Comprehensive documentation entry
// ============================================================================
struct DocEntry {
    // ---------- الهوية ----------
    std::string name;                   ///< الاسم
    std::string qualifiedName;          ///< الاسم المؤهل (مثلاً: وحدة.صنف.تابع)
    DocItemKind kind;                   ///< النوع
    AccessLevel access = AccessLevel::Public;
    std::string id;                     ///< معرف فريد للربط

    // ---------- التوقيع ----------
    std::string signature;              ///< التوقيع الكامل
    std::string declarationCode;        ///< كود الإعلان

    // ---------- الوصف ----------
    std::string summary;                ///< ملخص قصير (أول سطر)
    std::string description;            ///< وصف مفصل
    std::vector<std::string> notes;     ///< ملاحظات (@ملاحظة)
    std::vector<std::string> warnings;  ///< تحذيرات (@تحذير)

    // ---------- المعطيات والإرجاع ----------
    std::vector<ParamInfo> params;      ///< المعطيات
    std::string returnType;             ///< نوع الإرجاع
    std::string returnDescription;      ///< وصف الإرجاع

    // ---------- الأمثلة ----------
    std::vector<std::string> examples;  ///< أمثلة كود

    // ---------- المراجع ----------
    std::vector<std::string> seeAlso;   ///< انظر أيضاً
    std::vector<std::string> crossRefs; ///< مراجع متقاطعة (محللة تلقائياً)

    // ---------- الإصدار ----------
    std::string since;                  ///< منذ إصدار (@منذ)
    std::string deprecated;             ///< مهمل (@مهمل) — سبب الإهمال
    std::string version;                ///< إصدار التوثيق

    // ---------- الموقع ----------
    std::string sourceFile;             ///< ملف المصدر
    int lineNumber = 0;                 ///< رقم السطر
    int endLine = 0;                    ///< رقم سطر النهاية

    // ---------- العلاقات ----------
    std::string parentName;             ///< العنصر الأب (للتوابع والخصائص)
    std::vector<std::string> children;  ///< العناصر الأبناء
    std::vector<std::string> inherits;  ///< السلاسل الموروثة
    std::vector<std::string> implements;///< الواجهات المنفذة

    // ---------- وسوم إضافية ----------
    std::vector<DocTag> tags;           ///< جميع الوسوم الأصلية
    std::map<std::string, std::string> metadata; ///< بيانات وصفية إضافية

    // ---------- المساعدات ----------
    std::string kindNameAr() const;     ///< اسم النوع بالعربية
    std::string kindNameEn() const;     ///< اسم النوع بالإنجليزية
    bool isDeprecated() const { return !deprecated.empty(); }
    bool isPublic() const { return access == AccessLevel::Public; }
};

// ============================================================================
// (AR) تكوين مولّد التوثيق
// (EN) Documentation generator configuration
// ============================================================================
struct DocsConfig {
    // --- التنسيقات ---
    bool htmlOutput = true;
    bool markdownOutput = false;
    bool jsonOutput = false;

    // --- المجلدات ---
    std::string outputDir = "docs";
    std::vector<std::string> inputFiles;
    std::vector<std::string> includePaths;

    // --- المعلومات ---
    std::string title = "توثيق لغة ص";
    std::string description;
    std::string version = "1.0.0";
    std::string author;
    std::string projectUrl;

    // --- المحتوى ---
    bool includePrivate = false;
    bool includeInternal = false;
    bool searchEnabled = true;
    bool crossRefsEnabled = true;
    bool syntaxHighlight = true;

    // --- التخصيص ---
    std::string theme = "dark";     ///< dark, light, ocean, desert
    std::string logoUrl;
    std::string customCss;
    std::string faviconUrl;

    // --- السلوك ---
    bool verbose = false;
    bool watchMode = false;
    std::string configFile;
};

// ============================================================================
// (AR) نتيجة التوليد
// (EN) Generation result
// ============================================================================
struct DocsResult {
    bool success = true;
    int filesProcessed = 0;
    int itemsDocumented = 0;
    int pagesGenerated = 0;
    int warnings = 0;
    int errors = 0;
    double durationSeconds = 0.0;
    std::vector<std::string> errorMessages;
    std::vector<std::string> warningMessages;
    std::vector<std::string> generatedFiles;
};

// ============================================================================
// (AR) محلل تعليقات التوثيق - يحلل تعليقات /// و #** **#
// (EN) Documentation comment parser - parses /// and #** **# comments
// ============================================================================
class DocCommentParser {
public:
    /// تحليل ملف كامل واستخراج عناصر التوثيق
    std::vector<DocEntry> parseFile(const std::string& filePath) const;

    /// تحليل نص مصدري واستخراج العناصر
    std::vector<DocEntry> parseSource(const std::string& source,
                                       const std::string& filename = "") const;

private:
    /// تحليل أسطر التعليقات إلى وسوم
    DocEntry parseDocBlock(const std::vector<std::string>& docLines) const;

    /// تحليل وسم واحد
    DocTag parseTag(const std::string& line) const;

    /// التعرف على إعلان بعد كتلة التوثيق
    void parseDeclaration(const std::string& line, DocEntry& entry) const;

    /// تحليل توقيع الدالة
    void parseFunctionSignature(const std::string& line, DocEntry& entry) const;

    /// تحليل إعلان الصنف
    void parseClassDeclaration(const std::string& line, DocEntry& entry) const;

    /// تحليل المعطيات من قوسين
    std::vector<ParamInfo> parseParamList(const std::string& paramStr) const;

    /// توليد معرف فريد
    static std::string generateId(const DocEntry& entry);
};

// ============================================================================
// (AR) جامع الملفات - يجمع ملفات .ص من المسارات المحددة
// (EN) File collector - collects .ص files from specified paths
// ============================================================================
class FileCollector {
public:
    std::vector<std::string> collect(
        const std::vector<std::string>& inputs,
        const std::vector<std::string>& excludePatterns = {}) const;

private:
    void collectFromDirectory(
        const std::string& dir,
        std::vector<std::string>& files,
        const std::vector<std::string>& excludePatterns) const;

    bool shouldExclude(const std::string& path,
                       const std::vector<std::string>& patterns) const;
};

// ============================================================================
// (AR) مولّد HTML - ينشئ موقع HTML كامل
// (EN) HTML generator - creates complete HTML website
// ============================================================================
class HtmlDocGenerator {
public:
    explicit HtmlDocGenerator(const DocsConfig& config) : config_(config) {}

    /// توليد الموقع كاملاً
    int generate(const std::vector<DocEntry>& entries,
                 DocsResult& result);

private:
    const DocsConfig& config_;

    void generateIndexPage(const std::vector<DocEntry>& entries);
    void generateItemPage(const DocEntry& entry,
                          const std::map<std::string, const DocEntry*>& index);
    void generateListPage(const std::vector<DocEntry>& entries,
                          DocItemKind kind,
                          const std::string& title);
    void generateSearchIndex(const std::vector<DocEntry>& entries);
    void generateModuleTree(const std::vector<DocEntry>& entries);
    void generateStylesheet();
    void generateSearchScript();

    /// تحويل نص ص → HTML مع تلوين بناء الجمل
    std::string highlightSadCode(const std::string& code) const;
    std::string renderMarkdownToHtml(const std::string& md) const;
    std::string escapeHtml(const std::string& s) const;
    std::string severityBadge(const std::string& dep) const;
    std::string breadcrumb(const DocEntry& entry) const;
    std::string accessBadge(AccessLevel level) const;
    std::string kindIcon(DocItemKind kind) const;
};

// ============================================================================
// (AR) مولّد Markdown
// (EN) Markdown generator
// ============================================================================
class MarkdownDocGenerator {
public:
    explicit MarkdownDocGenerator(const DocsConfig& config) : config_(config) {}

    int generate(const std::vector<DocEntry>& entries,
                 DocsResult& result);

private:
    const DocsConfig& config_;

    std::string generateEntryMarkdown(const DocEntry& entry) const;
    std::string generateTableOfContents(const std::vector<DocEntry>& entries) const;
};

// ============================================================================
// (AR) مولّد JSON
// (EN) JSON generator
// ============================================================================
class JsonDocGenerator {
public:
    explicit JsonDocGenerator(const DocsConfig& config) : config_(config) {}

    int generate(const std::vector<DocEntry>& entries,
                 DocsResult& result);

    std::string entryToJson(const DocEntry& entry, int indent = 4) const;
    static std::string escapeJson(const std::string& s);

private:
    const DocsConfig& config_;
};

// ============================================================================
// (AR) المحرك الرئيسي لتوليد التوثيق
// (EN) Main documentation generation engine
// ============================================================================
class DocsEmitter {
public:
    DocsEmitter() = default;
    explicit DocsEmitter(const DocsConfig& config) : config_(config) {}

    /// تعيين التكوين
    void setConfig(const DocsConfig& config) { config_ = config; }
    const DocsConfig& getConfig() const { return config_; }

    /// إضافة عنصر يدوياً (للتكامل مع AST)
    void addEntry(const DocEntry& entry);

    /// إضافة عناصر من تحليل ملف
    int addFromFile(const std::string& filePath);

    /// إضافة عناصر من نص مصدري
    int addFromSource(const std::string& source, const std::string& filename = "");

    /// بناء المراجع المتقاطعة
    void buildCrossReferences();

    /// توليد التوثيق بالتنسيق المحدد
    DocsResult generate();

    /// توليد بتنسيق معين
    DocsResult generate(DocFormat format);

    /// إخراج JSON (للتكامل مع IDE/LSP)
    std::string emitJson() const;

    /// إخراج Markdown
    std::string emitMarkdown() const;

    /// الحصول على جميع العناصر
    const std::vector<DocEntry>& getEntries() const { return entries_; }

    /// عدد العناصر
    size_t entryCount() const { return entries_.size(); }

    /// البحث في العناصر
    std::vector<const DocEntry*> search(const std::string& query) const;

    /// الحصول على عنصر بالاسم
    const DocEntry* findByName(const std::string& name) const;

    /// الحصول على عناصر بالنوع
    std::vector<const DocEntry*> findByKind(DocItemKind kind) const;

private:
    DocsConfig config_;
    std::vector<DocEntry> entries_;
    std::map<std::string, size_t> nameIndex_;    ///< فهرس بالاسم
    std::map<std::string, size_t> idIndex_;      ///< فهرس بالمعرف
    DocCommentParser parser_;

    void rebuildIndices();
    void filterByAccess();
};

// ============================================================================
// (AR) نقطة الدخول C المتوافقة — للاستخدام من sadc
// (EN) C-compatible entry point — for use from sadc
// ============================================================================
extern "C" {
    /// توليد التوثيق من ملف مصدري
    const char* sadc_emit_docs(const char* source_path, const char* format);

    /// توليد التوثيق من مصادر متعددة
    const char* sadc_emit_docs_batch(const char** source_paths, int count,
                                      const char* format, const char* output_dir);
}

} // namespace Meta
} // namespace Sad

#endif // SAD_DOCS_EMITTER_H
