// بسم الله الرحمن الرحيم
// ==============================================================================
// cli_commands.hpp - واجهة أوامر سطر الأوامر العربية للغة ص
// Arabic CLI Commands Interface for SAD Language
// ==============================================================================
// 
// ملف رأسي يحتوي على تعريفات أوامر سطر الأوامر العربية
// هذا الملف هو جزء من مشروع "هيمنة لغة ص العالمية"
// 
// الأوامر المدعومة:
// ----------------
//   ص بناء      - ترجمة الكود المصدري إلى ملف تنفيذي
//   ص شغّل      - تشغيل برنامج ص مباشرة
//   ص اختبر     - تشغيل اختبارات المشروع
//   ص جديد      - إنشاء مشروع جديد
//   ص أضف       - إضافة اعتمادية جديدة
//   ص نشر       - نشر حزمة على مستودع الحزم
//   ص وثّق      - توليد التوثيق من التعليقات
//   ص نسّق      - تنسيق الكود تلقائياً
//   ص تحقق      - التحقق من صحة الكود بدون ترجمة
//   ص نسخة      - عرض معلومات الإصدار
//   ص مساعدة    - عرض المساعدة
//
// المؤلف: فريق تطوير لغة ص
// التاريخ: 2026-01-23
// الإصدار: 1.0.0
// ==============================================================================

#ifndef SAD_CLI_COMMANDS_HPP
#define SAD_CLI_COMMANDS_HPP

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <optional>
#include <memory>
#include <filesystem>
#include <iostream>

namespace sad {
namespace cli {

// ==============================================================================
// الثوابت والتعريفات الأساسية
// ==============================================================================

// إصدار أداة سطر الأوامر
constexpr const char* CLI_VERSION = "1.0.0";

// اسم البرنامج الرئيسي
constexpr const char* PROGRAM_NAME = "ص";
constexpr const char* PROGRAM_NAME_EN = "sad";

// ==============================================================================
// ألوان الطرفية للإخراج الجميل
// Terminal Colors for Beautiful Output
// ==============================================================================

namespace colors {
    // أكواد ANSI للألوان
    extern const char* RESET;
    extern const char* BOLD;
    extern const char* DIM;
    extern const char* UNDERLINE;
    extern const char* RED;
    extern const char* GREEN;
    extern const char* YELLOW;
    extern const char* BLUE;
    extern const char* MAGENTA;
    extern const char* CYAN;
    extern const char* WHITE;
    
    // تعطيل الألوان (لأنظمة لا تدعمها)
    void disable();
    
    // التحقق من دعم الألوان
    bool is_supported();
}

// ==============================================================================
// رموز الحالة العربية
// Arabic Status Symbols
// ==============================================================================

namespace symbols {
    // رموز النجاح والفشل
    extern const char* SUCCESS;     // ✓
    extern const char* FAILURE;     // ✗
    extern const char* WARNING;     // ⚠
    extern const char* INFO;        // ℹ
    extern const char* ARROW;       // →
    extern const char* BULLET;      // •
    extern const char* CHECK;       // ✔
    extern const char* CROSS;       // ✘
    extern const char* STAR;        // ★
    extern const char* PROGRESS;    // ◐
}

// ==============================================================================
// خيارات سطر الأوامر
// Command Line Options
// ==============================================================================

/**
 * @brief خيار سطر أوامر واحد
 * 
 * يمثل خياراً واحداً يمكن تمريره للأمر
 */
struct CommandOption {
    std::string short_name;      // الاسم القصير (مثل: -o)
    std::string long_name;       // الاسم الطويل (مثل: --output)
    std::string arabic_name;     // الاسم العربي (مثل: --إخراج)
    std::string description;     // الوصف بالإنجليزية
    std::string description_ar;  // الوصف بالعربية
    bool requires_value;         // هل يتطلب قيمة؟
    std::string default_value;   // القيمة الافتراضية
    bool is_flag;               // هل هو علم (flag) فقط؟
};

/**
 * @brief نتيجة تحليل خيارات سطر الأوامر
 */
struct ParsedOptions {
    std::map<std::string, std::string> options;  // الخيارات وقيمها
    std::vector<std::string> positional_args;    // الوسائط الموضعية
    std::vector<std::string> errors;             // أخطاء التحليل
    
    // التحقق من وجود خيار
    bool has(const std::string& name) const;
    
    // الحصول على قيمة خيار
    std::string get(const std::string& name, const std::string& default_val = "") const;
    
    // الحصول على قيمة كعدد صحيح
    int get_int(const std::string& name, int default_val = 0) const;
    
    // الحصول على قيمة كعدد عشري
    double get_double(const std::string& name, double default_val = 0.0) const;
    
    // الحصول على قيمة كقيمة منطقية
    bool get_bool(const std::string& name, bool default_val = false) const;
};

// ==============================================================================
// الفئة الأساسية للأوامر
// Base Command Class
// ==============================================================================

/**
 * @brief الفئة الأساسية لجميع أوامر سطر الأوامر
 * 
 * جميع الأوامر (بناء، شغّل، اختبر، إلخ) ترث من هذه الفئة
 */
class Command {
public:
    virtual ~Command() = default;
    
    // تنفيذ الأمر - يجب تنفيذه في الفئات الفرعية
    virtual int execute(const ParsedOptions& options) = 0;
    
    // الحصول على اسم الأمر
    virtual std::string get_name() const = 0;
    virtual std::string get_arabic_name() const = 0;
    
    // الحصول على وصف الأمر
    virtual std::string get_description() const = 0;
    virtual std::string get_arabic_description() const = 0;
    
    // الحصول على الخيارات المتاحة
    virtual std::vector<CommandOption> get_options() const = 0;
    
    // طباعة المساعدة الخاصة بالأمر
    virtual void print_help(std::ostream& os) const;
    
    // طباعة أمثلة الاستخدام
    virtual void print_examples(std::ostream& os) const;
    
protected:
    // طباعة رسالة نجاح
    void print_success(const std::string& message) const;
    void print_success_ar(const std::string& message_ar) const;
    
    // طباعة رسالة خطأ
    void print_error(const std::string& message) const;
    void print_error_ar(const std::string& message_ar) const;
    
    // طباعة رسالة تحذير
    void print_warning(const std::string& message) const;
    void print_warning_ar(const std::string& message_ar) const;
    
    // طباعة رسالة معلومات
    void print_info(const std::string& message) const;
    void print_info_ar(const std::string& message_ar) const;
    
    // طباعة شريط التقدم
    void print_progress(const std::string& task, int percent) const;
};

// ==============================================================================
// أمر البناء - ص بناء
// Build Command - sad build
// ==============================================================================

/**
 * @brief أمر بناء المشروع
 * 
 * يترجم الكود المصدري إلى ملف تنفيذي أو مكتبة
 * 
 * الاستخدام:
 *   ص بناء [ملف.ص] [خيارات]
 *   sad build [file.s] [options]
 * 
 * أمثلة:
 *   ص بناء برنامج.ص
 *   ص بناء --إصدار
 *   ص بناء --هدف wasm32
 *   sad build program.s -o program
 */
class BuildCommand : public Command {
public:
    int execute(const ParsedOptions& options) override;
    
    std::string get_name() const override { return "build"; }
    std::string get_arabic_name() const override { return "بناء"; }
    
    std::string get_description() const override { 
        return "Compile source code to executable"; 
    }
    std::string get_arabic_description() const override { 
        return "ترجمة الكود المصدري إلى ملف تنفيذي"; 
    }
    
    std::vector<CommandOption> get_options() const override;
    void print_examples(std::ostream& os) const override;
    
private:
    // مستويات التحسين
    enum class OptLevel { O0, O1, O2, O3, Os, Oz };
    
    // أنواع الإخراج
    enum class OutputType { 
        Executable,      // ملف تنفيذي
        SharedLib,       // مكتبة مشتركة
        StaticLib,       // مكتبة ثابتة
        ObjectFile,      // ملف كائن
        LLVMIR,          // LLVM IR
        Assembly,        // كود التجميع
        WASM             // WebAssembly
    };
    
    // تحديد نوع الإخراج من الخيارات
    OutputType determine_output_type(const ParsedOptions& options) const;
    
    // تحديد مستوى التحسين
    OptLevel determine_opt_level(const ParsedOptions& options) const;
    
    // التحقق من صحة الملفات المدخلة
    bool validate_input_files(const std::vector<std::string>& files) const;
    
    // تنفيذ عملية البناء الفعلية
    int do_build(const std::vector<std::string>& input_files,
                 const std::string& output_file,
                 OutputType output_type,
                 OptLevel opt_level,
                 const ParsedOptions& options) const;
    
    // الحصول على امتداد ملف الإخراج
    std::string get_output_extension(OutputType type) const;
};

// ==============================================================================
// أمر التشغيل - ص شغّل
// Run Command - sad run
// ==============================================================================

/**
 * @brief أمر تشغيل البرنامج
 * 
 * يترجم ويشغل برنامج ص في خطوة واحدة
 * 
 * الاستخدام:
 *   ص شغّل [ملف.ص] [وسائط البرنامج]
 *   sad run [file.s] [program arguments]
 * 
 * أمثلة:
 *   ص شغّل برنامج.ص
 *   ص شغّل برنامج.ص -- وسيط1 وسيط2
 *   sad run program.s --release
 */
class RunCommand : public Command {
public:
    int execute(const ParsedOptions& options) override;
    
    std::string get_name() const override { return "run"; }
    std::string get_arabic_name() const override { return "شغّل"; }
    
    std::string get_description() const override { 
        return "Compile and run a program"; 
    }
    std::string get_arabic_description() const override { 
        return "ترجمة وتشغيل برنامج"; 
    }
    
    std::vector<CommandOption> get_options() const override;
    void print_examples(std::ostream& os) const override;
    
private:
    // تشغيل البرنامج بعد البناء
    int run_program(const std::string& executable,
                    const std::vector<std::string>& args) const;
};

// ==============================================================================
// أمر الاختبار - ص اختبر
// Test Command - sad test
// ==============================================================================

/**
 * @brief أمر تشغيل الاختبارات
 * 
 * يكتشف ويشغل جميع اختبارات المشروع
 * 
 * الاستخدام:
 *   ص اختبر [نمط] [خيارات]
 *   sad test [pattern] [options]
 * 
 * أمثلة:
 *   ص اختبر
 *   ص اختبر اختبار_*
 *   ص اختبر --متوازي
 *   sad test --verbose
 */
class TestCommand : public Command {
public:
    int execute(const ParsedOptions& options) override;
    
    std::string get_name() const override { return "test"; }
    std::string get_arabic_name() const override { return "اختبر"; }
    
    std::string get_description() const override { 
        return "Run project tests"; 
    }
    std::string get_arabic_description() const override { 
        return "تشغيل اختبارات المشروع"; 
    }
    
    std::vector<CommandOption> get_options() const override;
    void print_examples(std::ostream& os) const override;
    
private:
    // نتيجة اختبار واحد
    struct TestResult {
        std::string name;
        std::string name_ar;
        bool passed;
        double duration_ms;
        std::string error_message;
        std::string output;
    };
    
    // اكتشاف الاختبارات
    std::vector<std::string> discover_tests(const std::string& pattern) const;
    
    // تشغيل اختبار واحد
    TestResult run_single_test(const std::string& test_file) const;
    
    // طباعة ملخص النتائج
    void print_test_summary(const std::vector<TestResult>& results) const;
    
    // طباعة نتيجة اختبار واحد
    void print_test_result_line(const TestResult& result, bool verbose) const;
    
    // كتابة التقرير إلى ملف
    void write_report(const std::vector<TestResult>& results,
                      const std::string& filename,
                      const std::string& format) const;
    
    // مطابقة النمط
    bool match_pattern(const std::string& str, const std::string& pattern) const;
};

// ==============================================================================
// أمر المشروع الجديد - ص جديد
// New Project Command - sad new
// ==============================================================================

/**
 * @brief أمر إنشاء مشروع جديد
 * 
 * ينشئ هيكل مشروع جديد مع الملفات الأساسية
 * 
 * الاستخدام:
 *   ص جديد [اسم_المشروع] [خيارات]
 *   sad new [project_name] [options]
 * 
 * أمثلة:
 *   ص جديد مشروعي
 *   ص جديد تطبيق_ويب --قالب ويب
 *   sad new my_project --template cli
 */
class NewProjectCommand : public Command {
public:
    int execute(const ParsedOptions& options) override;
    
    std::string get_name() const override { return "new"; }
    std::string get_arabic_name() const override { return "جديد"; }
    
    std::string get_description() const override { 
        return "Create a new project"; 
    }
    std::string get_arabic_description() const override { 
        return "إنشاء مشروع جديد"; 
    }
    
    std::vector<CommandOption> get_options() const override;
    void print_examples(std::ostream& os) const override;
    
private:
    // أنواع القوالب المتاحة
    enum class ProjectTemplate {
        Basic,       // مشروع أساسي
        Library,     // مكتبة
        CLI,         // تطبيق سطر أوامر
        Web,         // تطبيق ويب
        Mobile,      // تطبيق هاتف
        GUI          // تطبيق واجهة رسومية
    };
    
    // إنشاء هيكل المشروع
    bool create_project_structure(const std::string& name, 
                                   ProjectTemplate template_type) const;
    
    // إنشاء ملف ص.toml
    bool create_sad_toml(const std::string& name,
                         const std::filesystem::path& project_dir) const;
    
    // إنشاء الملف الرئيسي
    bool create_main_file(const std::string& name,
                          const std::filesystem::path& project_dir,
                          ProjectTemplate template_type) const;
};

// ==============================================================================
// أمر المساعدة - ص مساعدة
// Help Command - sad help
// ==============================================================================

/**
 * @brief أمر عرض المساعدة
 */
class HelpCommand : public Command {
public:
    int execute(const ParsedOptions& options) override;
    
    std::string get_name() const override { return "help"; }
    std::string get_arabic_name() const override { return "مساعدة"; }
    
    std::string get_description() const override { 
        return "Show help information"; 
    }
    std::string get_arabic_description() const override { 
        return "عرض معلومات المساعدة"; 
    }
    
    std::vector<CommandOption> get_options() const override;
    
    // تعيين مرجع لجميع الأوامر للمساعدة التفصيلية
    void set_commands(const std::map<std::string, std::shared_ptr<Command>>& cmds) {
        commands_ = &cmds;
    }
    
private:
    const std::map<std::string, std::shared_ptr<Command>>* commands_ = nullptr;
};

// ==============================================================================
// أمر الإصدار - ص نسخة
// Version Command - sad version
// ==============================================================================

/**
 * @brief أمر عرض معلومات الإصدار
 */
class VersionCommand : public Command {
public:
    int execute(const ParsedOptions& options) override;
    
    std::string get_name() const override { return "version"; }
    std::string get_arabic_name() const override { return "نسخة"; }
    
    std::string get_description() const override { 
        return "Show version information"; 
    }
    std::string get_arabic_description() const override { 
        return "عرض معلومات الإصدار"; 
    }
    
    std::vector<CommandOption> get_options() const override;
};

// ==============================================================================
// أمر التنسيق - ص نسّق
// Format Command - sad format
// ==============================================================================

/**
 * @brief أمر تنسيق الكود تلقائياً
 * 
 * ينسّق ملفات لغة ص وفقاً لقواعد التنسيق القياسية
 * 
 * الاستخدام:
 *   ص نسّق <ملف.ص>          - تنسيق ملف واحد
 *   ص نسّق <مجلد>            - تنسيق جميع الملفات في مجلد
 *   ص نسّق --تحقق <ملف>     - التحقق بدون تعديل
 */
class FormatCommand : public Command {
public:
    int execute(const ParsedOptions& options) override;
    
    std::string get_name() const override { return "format"; }
    std::string get_arabic_name() const override { return "نسّق"; }
    
    std::string get_description() const override { 
        return "Format source code files"; 
    }
    std::string get_arabic_description() const override { 
        return "تنسيق ملفات الكود المصدري"; 
    }
    
    std::vector<CommandOption> get_options() const override;
    void print_examples(std::ostream& os) const override;
};

// ==============================================================================
// أمر الهاتف - ص هاتف
// Mobile Command - sad mobile
// ==============================================================================

/**
 * @brief أمر إدارة تطبيقات الهاتف
 * 
 * يدير إنشاء وبناء وتشغيل تطبيقات الهاتف (أندرويد و iOS)
 * 
 * الاستخدام:
 *   ص هاتف جديد [اسم] [خيارات]     - إنشاء مشروع هاتف جديد
 *   ص هاتف بناء [خيارات]             - بناء تطبيق الهاتف
 *   ص هاتف شغّل [خيارات]             - تشغيل التطبيق على محاكي
 *   ص هاتف واجهة [ملف.ص] [خيارات]   - توليد كود الواجهة
 * 
 * أمثلة:
 *   ص هاتف جديد تطبيقي --اندرويد
 *   ص هاتف جديد تطبيقي --آيفون
 *   ص هاتف جديد تطبيقي --الكل
 *   ص هاتف بناء --اندرويد
 *   ص هاتف بناء --آيفون
 *   ص هاتف واجهة شاشتي.ص --اندرويد
 *   ص هاتف واجهة شاشتي.ص --آيفون
 *   sad mobile new myapp --android
 *   sad mobile build --ios
 *   sad mobile ui myscreen.s --android
 */
class MobileCommand : public Command {
public:
    int execute(const ParsedOptions& options) override;
    
    std::string get_name() const override { return "mobile"; }
    std::string get_arabic_name() const override { return "هاتف"; }
    
    std::string get_description() const override { 
        return "Manage mobile applications (Android & iOS)"; 
    }
    std::string get_arabic_description() const override { 
        return "إدارة تطبيقات الهاتف (أندرويد و iOS)"; 
    }
    
    std::vector<CommandOption> get_options() const override;
    void print_examples(std::ostream& os) const override;
    
private:
    // الأوامر الفرعية / Sub-commands
    enum class SubCommand {
        New,        // إنشاء مشروع جديد
        Build,      // بناء التطبيق
        Run,        // تشغيل التطبيق
        UI,         // توليد كود الواجهة
        Info,       // معلومات المشروع
        Help        // مساعدة
    };
    
    // المنصة المستهدفة / Target platform
    enum class Platform {
        Android,    // أندرويد
        iOS,        // iOS (آيفون/آيباد)
        Both        // كلاهما
    };
    
    // تحديد الأمر الفرعي
    SubCommand parse_subcommand(const ParsedOptions& options) const;
    
    // تحديد المنصة
    Platform parse_platform(const ParsedOptions& options) const;
    
    // الأوامر الفرعية
    int execute_new(const ParsedOptions& options, Platform platform);
    int execute_build(const ParsedOptions& options, Platform platform);
    int execute_run(const ParsedOptions& options, Platform platform);
    int execute_ui(const ParsedOptions& options, Platform platform);
    int execute_info(const ParsedOptions& options);
    
    // دوال البناء الفعلية
    int build_android(const std::filesystem::path& project_root, bool is_release, const ParsedOptions& options);
    int build_ios(const std::filesystem::path& project_root, bool is_release, const ParsedOptions& options);
    
    // توليد Gradle Wrapper
    void generateGradleWrapper(const std::filesystem::path& android_dir);
    
    // توليد كود من ملفات ص
    std::string generateKotlinFromSadSource(const std::string& sad_source, const std::string& screen_name);
    std::string generateSwiftUIFromSadSource(const std::string& sad_source, const std::string& screen_name);
};

// ==============================================================================
// مدير الأوامر الرئيسي
// Main Command Manager
// ==============================================================================

/**
 * @brief مدير أوامر سطر الأوامر
 * 
 * يدير تسجيل الأوامر وتحليل سطر الأوامر وتوجيه التنفيذ
 */
class CommandManager {
public:
    CommandManager();
    ~CommandManager() = default;
    
    // تشغيل الأداة مع وسائط سطر الأوامر
    int run(int argc, char* argv[]);
    
    // تسجيل أمر جديد
    void register_command(std::shared_ptr<Command> cmd);
    
    // الحصول على أمر بالاسم
    std::shared_ptr<Command> get_command(const std::string& name) const;
    
    // الحصول على جميع الأوامر
    const std::map<std::string, std::shared_ptr<Command>>& get_commands() const {
        return commands_;
    }
    
private:
    std::map<std::string, std::shared_ptr<Command>> commands_;
    std::map<std::string, std::string> arabic_to_english_;
    
    // تحليل سطر الأوامر
    ParsedOptions parse_args(int argc, char* argv[], const Command* cmd) const;
    
    // طباعة الاستخدام العام
    void print_usage(std::ostream& os) const;
    
    // طباعة شعار البرنامج
    void print_banner(std::ostream& os) const;
    
    // تحويل اسم الأمر العربي إلى الإنجليزي
    std::string arabic_to_english_cmd(const std::string& arabic_name) const;
    
    // تسجيل جميع الأوامر الافتراضية
    void register_default_commands();
};

// ==============================================================================
// دوال مساعدة
// Utility Functions
// ==============================================================================

namespace utils {
    // التحقق من وجود ملف
    bool file_exists(const std::string& path);
    
    // التحقق من وجود مجلد
    bool directory_exists(const std::string& path);
    
    // إنشاء مجلد (مع المجلدات الأب إن لزم)
    bool create_directory(const std::string& path);
    
    // قراءة محتوى ملف
    std::optional<std::string> read_file(const std::string& path);
    
    // كتابة محتوى إلى ملف
    bool write_file(const std::string& path, const std::string& content);
    
    // الحصول على امتداد الملف
    std::string get_extension(const std::string& path);
    
    // تغيير امتداد الملف
    std::string change_extension(const std::string& path, const std::string& new_ext);
    
    // الحصول على اسم الملف بدون المسار
    std::string get_filename(const std::string& path);
    
    // الحصول على المجلد الحالي
    std::string get_current_directory();
    
    // البحث عن ملف ص.toml في المجلد الحالي أو المجلدات الأب
    std::optional<std::string> find_project_root();
    
    // تحويل النص إلى أحرف صغيرة (للمقارنة)
    std::string to_lower(const std::string& str);
    
    // تقسيم نص بمحدد
    std::vector<std::string> split(const std::string& str, char delimiter);
    
    // دمج قائمة نصوص بمحدد
    std::string join(const std::vector<std::string>& parts, const std::string& delimiter);
    
    // الحصول على وقت التعديل للملف
    std::optional<std::filesystem::file_time_type> get_modification_time(const std::string& path);
}

} // namespace cli
} // namespace sad

#endif // SAD_CLI_COMMANDS_HPP
