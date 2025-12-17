// بسم الله الرحمن الرحيم
// Documentation Generator for Sad Language
// مولد التوثيق للغة Sad
// Phase 7.4: Documentation Generator

#ifndef SAD_DOCGEN_H
#define SAD_DOCGEN_H

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>
#include <fstream>

namespace sad {
namespace docgen {

// ============================================================================
// Forward Declarations
// ============================================================================

class Lexer;
class Parser;
class ASTNode;

// ============================================================================
// Documentation Types / أنواع التوثيق
// ============================================================================

/**
 * @brief DocType - نوع التوثيق
 */
enum class DocType {
    FUNCTION,      // دالة
    CLASS,         // صنف
    METHOD,        // وظيفة
    VARIABLE,      // متغير
    CONSTANT,      // ثابت
    PARAMETER,     // معامل
    RETURN_VALUE,  // قيمة الإرجاع
    MODULE,        // وحدة
    EXAMPLE        // مثال
};

/**
 * @brief DocComment - تعليق توثيقي
 * 
 * (AR) تعليق توثيقي مستخرج من الكود
 * (EN) Documentation comment extracted from code
 */
struct DocComment {
    DocType type;                           // نوع التوثيق
    std::string name;                       // الاسم
    std::string name_ar;                    // الاسم بالعربية
    std::string description;                // الوصف (English)
    std::string description_ar;             // الوصف (عربي)
    std::string signature;                  // التوقيع (e.g., "دالة جمع(رقم أ, رقم ب)")
    std::string return_type;                // نوع الإرجاع
    std::string return_desc;                // وصف الإرجاع
    std::vector<std::pair<std::string, std::string>> parameters;  // (name, desc)
    std::vector<std::string> examples;      // أمثلة الاستخدام
    std::vector<std::string> see_also;      // انظر أيضاً
    std::string file;                       // اسم الملف
    int line = -1;                          // رقم السطر
    std::string module;                     // الوحدة
    
    // Metadata
    bool deprecated = false;                // مهمل؟
    std::string since_version;              // منذ الإصدار
    std::string author;                     // المؤلف
};

/**
 * @brief OutputFormat - صيغة الإخراج
 */
enum class OutputFormat {
    HTML,          // HTML
    MARKDOWN,      // Markdown
    PDF,           // PDF (via HTML)
    JSON,          // JSON
    XML            // XML
};

// ============================================================================
// Documentation Extractor / مستخرج التوثيق
// ============================================================================

/**
 * @brief DocExtractor - مستخرج التوثيق من الكود
 * 
 * (AR) يستخرج التعليقات التوثيقية من ملفات الكود
 * (EN) Extracts documentation comments from source files
 * 
 * Supported comment formats:
 * 
 * 1. Single-line doc comment:
 *    # @brief وصف قصير
 *    # @param x وصف المعامل
 *    # @return وصف القيمة المرجعة
 * 
 * 2. Multi-line doc comment:
 *    ###
 *    # @brief وصف قصير
 *    # @description وصف طويل
 *    ###
 * 
 * 3. Arabic/English bilingual:
 *    # @brief Short description
 *    # @brief_ar وصف قصير
 */
class DocExtractor {
public:
    DocExtractor() = default;
    
    /**
     * @brief استخراج التوثيق من ملف / Extract docs from file
     */
    bool extract_from_file(const std::string& file_path);
    
    /**
     * @brief استخراج التوثيق من نص / Extract docs from source
     */
    bool extract_from_source(const std::string& source, const std::string& file_name = "");
    
    /**
     * @brief الحصول على جميع التعليقات / Get all doc comments
     */
    const std::vector<DocComment>& get_comments() const { return comments_; }
    
    /**
     * @brief الحصول على تعليقات حسب النوع / Get comments by type
     */
    std::vector<DocComment> get_comments_by_type(DocType type) const;
    
    /**
     * @brief الحصول على تعليقات حسب الوحدة / Get comments by module
     */
    std::vector<DocComment> get_comments_by_module(const std::string& module) const;
    
    /**
     * @brief مسح جميع التعليقات / Clear all comments
     */
    void clear() { comments_.clear(); }

private:
    std::vector<DocComment> comments_;
    
    /**
     * @brief تحليل تعليق توثيقي / Parse doc comment
     */
    std::optional<DocComment> parse_doc_comment(const std::vector<std::string>& lines,
                                                int start_line,
                                                const std::string& file);
    
    /**
     * @brief استخراج الوسوم من التعليق / Extract tags from comment
     */
    void extract_tags(DocComment& comment, const std::string& line);
    
    /**
     * @brief تحديد نوع التوثيق / Determine doc type
     */
    DocType determine_type(const std::string& next_line);
};

// ============================================================================
// Documentation Generator / مولد التوثيق
// ============================================================================

/**
 * @brief DocGenerator - مولد التوثيق
 * 
 * (AR) يولد التوثيق بصيغ مختلفة
 * (EN) Generates documentation in various formats
 */
class DocGenerator {
public:
    DocGenerator() = default;
    
    /**
     * @brief توليد التوثيق / Generate documentation
     * 
     * @param comments التعليقات التوثيقية
     * @param format صيغة الإخراج
     * @param output_dir مجلد الإخراج
     * @return true on success
     */
    bool generate(const std::vector<DocComment>& comments,
                 OutputFormat format,
                 const std::string& output_dir);
    
    /**
     * @brief توليد صفحة الفهرس / Generate index page
     */
    bool generate_index(const std::vector<DocComment>& comments,
                       const std::string& output_file);
    
    /**
     * @brief إعدادات التوليد / Generator settings
     */
    struct Settings {
        std::string project_name = "Sad Project";
        std::string project_name_ar = "مشروع Sad";
        std::string version = "1.0.0";
        std::string author = "";
        std::string logo_path = "";
        bool include_source_links = true;
        bool include_search = true;
        bool bilingual = true;
        std::string theme = "default";
        std::string language = "ar";  // ar, en, both
    };
    
    void set_settings(const Settings& settings) { settings_ = settings; }
    const Settings& get_settings() const { return settings_; }

private:
    Settings settings_;
    
    /**
     * @brief توليد HTML / Generate HTML
     */
    bool generate_html(const std::vector<DocComment>& comments,
                      const std::string& output_dir);
    
    /**
     * @brief توليد Markdown / Generate Markdown
     */
    bool generate_markdown(const std::vector<DocComment>& comments,
                          const std::string& output_dir);
    
    /**
     * @brief توليد JSON / Generate JSON
     */
    bool generate_json(const std::vector<DocComment>& comments,
                      const std::string& output_file);
    
    /**
     * @brief توليد صفحة HTML لتعليق واحد / Generate HTML page for comment
     */
    std::string generate_html_page(const DocComment& comment);
    
    /**
     * @brief توليد Markdown لتعليق واحد / Generate Markdown for comment
     */
    std::string generate_markdown_doc(const DocComment& comment);
    
    /**
     * @brief توليد قائمة جانبية / Generate sidebar
     */
    std::string generate_sidebar(const std::vector<DocComment>& comments);
    
    /**
     * @brief توليد قائمة التنقل / Generate navigation
     */
    std::string generate_navigation(const std::vector<DocComment>& comments);
    
    /**
     * @brief HTML header / رأس HTML
     */
    std::string get_html_header(const std::string& title);
    
    /**
     * @brief HTML footer / ذيل HTML
     */
    std::string get_html_footer();
    
    /**
     * @brief CSS styles / أنماط CSS
     */
    std::string get_css_styles();
    
    /**
     * @brief JavaScript code / كود JavaScript
     */
    std::string get_javascript();
};

// ============================================================================
// API Reference Generator / مولد مرجع API
// ============================================================================

/**
 * @brief APIReferenceGenerator - مولد مرجع API
 * 
 * (AR) يولد مرجع API كامل للمشروع
 * (EN) Generates complete API reference for the project
 */
class APIReferenceGenerator {
public:
    APIReferenceGenerator() = default;
    
    /**
     * @brief توليد مرجع API / Generate API reference
     * 
     * @param source_dirs مجلدات الكود المصدري
     * @param output_dir مجلد الإخراج
     * @param format صيغة الإخراج
     * @return true on success
     */
    bool generate(const std::vector<std::string>& source_dirs,
                 const std::string& output_dir,
                 OutputFormat format = OutputFormat::HTML);
    
    /**
     * @brief توليد شجرة الوحدات / Generate module tree
     */
    bool generate_module_tree(const std::string& output_file);
    
    /**
     * @brief توليد فهرس البحث / Generate search index
     */
    bool generate_search_index(const std::string& output_file);

private:
    DocExtractor extractor_;
    DocGenerator generator_;
    
    /**
     * @brief فحص مجلد ومعالجة الملفات / Scan directory and process files
     */
    bool scan_directory(const std::string& dir);
    
    /**
     * @brief هل الملف قابل للمعالجة / Is file processable
     */
    bool is_source_file(const std::string& path);
};

// ============================================================================
// Example Generator / مولد الأمثلة
// ============================================================================

/**
 * @brief ExampleGenerator - مولد الأمثلة
 * 
 * (AR) يستخرج ويولد أمثلة الاستخدام
 * (EN) Extracts and generates usage examples
 */
class ExampleGenerator {
public:
    ExampleGenerator() = default;
    
    /**
     * @brief استخراج الأمثلة من الكود / Extract examples from code
     */
    bool extract_examples(const std::string& file_path);
    
    /**
     * @brief توليد صفحة أمثلة / Generate examples page
     */
    bool generate_examples_page(const std::string& output_file,
                                OutputFormat format = OutputFormat::HTML);
    
    /**
     * @brief Example structure
     */
    struct Example {
        std::string title;          // العنوان
        std::string title_ar;       // العنوان بالعربية
        std::string description;    // الوصف
        std::string description_ar; // الوصف بالعربية
        std::string code;           // الكود
        std::string output;         // المخرجات المتوقعة
        std::string category;       // الفئة
    };
    
    const std::vector<Example>& get_examples() const { return examples_; }

private:
    std::vector<Example> examples_;
    
    /**
     * @brief تحليل مثال / Parse example
     */
    std::optional<Example> parse_example(const std::string& source);
};

// ============================================================================
// Utility Functions / دوال مساعدة
// ============================================================================

/**
 * @brief تحويل DocType إلى نص / Convert DocType to string
 */
std::string doc_type_to_string(DocType type);

/**
 * @brief تحويل DocType إلى نص عربي / Convert DocType to Arabic string
 */
std::string doc_type_to_string_ar(DocType type);

/**
 * @brief تنسيق الكود للعرض / Format code for display
 */
std::string format_code(const std::string& code, const std::string& language = "sad");

/**
 * @brief Escape HTML characters
 */
std::string escape_html(const std::string& text);

/**
 * @brief Convert Markdown to HTML
 */
std::string markdown_to_html(const std::string& markdown);

/**
 * @brief إنشاء مجلد إن لم يكن موجوداً / Create directory if not exists
 */
bool ensure_directory_exists(const std::string& path);

/**
 * @brief قراءة ملف كامل / Read entire file
 */
std::optional<std::string> read_file(const std::string& path);

/**
 * @brief كتابة نص إلى ملف / Write text to file
 */
bool write_file(const std::string& path, const std::string& content);

} // namespace docgen
} // namespace sad

#endif // SAD_DOCGEN_H
