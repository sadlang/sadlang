// بسم الله الرحمن الرحيم
// ==============================================================================
// cli_main.cpp - نقطة الدخول الرئيسية لأداة ص
// Main Entry Point for SAD CLI Tool
// ==============================================================================
//
// هذا الملف يحتوي على نقطة الدخول الرئيسية لأداة سطر الأوامر "ص"
// وهي الواجهة الرئيسية للتعامل مع لغة ص.
//
// الأداة تدعم الأوامر التالية:
// ----------------------------
//   ص بناء    - ترجمة الكود إلى ملف تنفيذي
//   ص شغّل    - ترجمة وتشغيل برنامج
//   ص اختبر   - تشغيل اختبارات المشروع
//   ص جديد    - إنشاء مشروع جديد
//   ص أضف     - إضافة اعتمادية
//   ص نشر     - نشر حزمة
//   ص وثّق    - توليد التوثيق
//   ص نسّق    - تنسيق الكود
//   ص تحقق    - التحقق من الكود
//   ص نسخة    - عرض الإصدار
//   ص مساعدة  - عرض المساعدة
//
// طريقة الاستخدام:
// ---------------
//   ص <أمر> [خيارات] [ملفات]
//   sad <command> [options] [files]
//
// المؤلف: فريق تطوير لغة ص
// التاريخ: 2026-01-23
// الإصدار: 1.0.0
// ==============================================================================

#include "cli_commands.hpp"
#include "mobile_project_gen.hpp"
#include "android_target.hpp"
#include <iostream>
#include <clocale>
#include <fstream>
#include <filesystem>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

namespace sad {
namespace cli {

// ==============================================================================
// تعريف ثوابت الألوان
// Color Constants Definition
// ==============================================================================

namespace colors {
    const char* RESET     = "\033[0m";
    const char* BOLD      = "\033[1m";
    const char* DIM       = "\033[2m";
    const char* UNDERLINE = "\033[4m";
    const char* RED       = "\033[31m";
    const char* GREEN     = "\033[32m";
    const char* YELLOW    = "\033[33m";
    const char* BLUE      = "\033[34m";
    const char* MAGENTA   = "\033[35m";
    const char* CYAN      = "\033[36m";
    const char* WHITE     = "\033[37m";
    
    void disable() {
        RESET = "";
        BOLD = "";
        DIM = "";
        UNDERLINE = "";
        RED = "";
        GREEN = "";
        YELLOW = "";
        BLUE = "";
        MAGENTA = "";
        CYAN = "";
        WHITE = "";
    }
    
    bool is_supported() {
#ifdef _WIN32
        // التحقق من دعم ويندوز لألوان ANSI
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) return false;
        
        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode)) return false;
        
        // محاولة تفعيل وضع ANSI
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        return SetConsoleMode(hOut, dwMode) != 0;
#else
        // يونكس عادة يدعم الألوان
        return true;
#endif
    }
}

// ==============================================================================
// تعريف الرموز
// Symbols Definition
// ==============================================================================

namespace symbols {
    const char* SUCCESS  = "✓";
    const char* FAILURE  = "✗";
    const char* WARNING  = "⚠";
    const char* INFO     = "ℹ";
    const char* ARROW    = "→";
    const char* BULLET   = "•";
    const char* CHECK    = "✔";
    const char* CROSS    = "✘";
    const char* STAR     = "★";
    const char* PROGRESS = "◐";
}

// ==============================================================================
// دوال الأمان - Security Functions
// ==============================================================================

namespace security {
    /**
     * @brief تطهير المسار لمنع Command Injection
     * @param path المسار المراد تطهيره
     * @return المسار المُطهَّر
     */
    std::string sanitize_path(const std::string& path) {
        std::string result;
        result.reserve(path.size());
        
        for (char c : path) {
            // السماح بأحرف آمنة فقط
            if ((c >= 'a' && c <= 'z') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') ||
                c == '/' || c == '\\' || c == '.' || c == '_' || c == '-' ||
                c == ':' ||  // للمسارات في Windows
                (c & 0x80)) {  // للأحرف العربية UTF-8
                result += c;
            }
        }
        
        return result;
    }
    
    /**
     * @brief التحقق من صحة المسار
     * @param path المسار
     * @return true إذا كان المسار آمناً
     */
    bool is_safe_path(const std::string& path) {
        // منع Path Traversal
        if (path.find("..") != std::string::npos) {
            return false;
        }
        
        // منع الأحرف الخطرة
        const std::string dangerous = ";|&`$(){}[]<>!";
        for (char c : path) {
            if (dangerous.find(c) != std::string::npos) {
                return false;
            }
        }
        
        return true;
    }
    
    /**
     * @brief تنفيذ أمر بشكل آمن
     * @param command الأمر
     * @return رمز الخروج
     */
    int safe_system(const std::string& command) {
        // في الإنتاج، استخدم CreateProcess/fork+exec بدلاً من system
        return std::system(command.c_str());
    }
}

// ==============================================================================
// تنفيذ ParsedOptions
// ==============================================================================

bool ParsedOptions::has(const std::string& name) const {
    return options.find(name) != options.end();
}

std::string ParsedOptions::get(const std::string& name, const std::string& default_val) const {
    auto it = options.find(name);
    return (it != options.end()) ? it->second : default_val;
}

int ParsedOptions::get_int(const std::string& name, int default_val) const {
    auto it = options.find(name);
    if (it == options.end()) return default_val;
    try {
        return std::stoi(it->second);
    } catch (...) {
        return default_val;
    }
}

double ParsedOptions::get_double(const std::string& name, double default_val) const {
    auto it = options.find(name);
    if (it == options.end()) return default_val;
    try {
        return std::stod(it->second);
    } catch (...) {
        return default_val;
    }
}

bool ParsedOptions::get_bool(const std::string& name, bool default_val) const {
    auto it = options.find(name);
    if (it == options.end()) return default_val;
    const std::string& val = it->second;
    return val == "true" || val == "1" || val == "yes" || val == "نعم" || val.empty();
}

// ==============================================================================
// تنفيذ دوال الفئة Command الأساسية
// ==============================================================================

void Command::print_help(std::ostream& os) const {
    os << colors::BOLD << colors::CYAN;
    os << get_arabic_name() << " - " << get_arabic_description() << std::endl;
    os << get_name() << " - " << get_description() << std::endl;
    os << colors::RESET << std::endl;
    
    os << colors::BOLD << "الاستخدام / Usage:" << colors::RESET << std::endl;
    os << "  ص " << get_arabic_name() << " [خيارات] [ملفات]" << std::endl;
    os << "  sad " << get_name() << " [options] [files]" << std::endl;
    os << std::endl;
    
    auto options = get_options();
    if (!options.empty()) {
        os << colors::BOLD << "الخيارات / Options:" << colors::RESET << std::endl;
        
        for (const auto& opt : options) {
            os << "  ";
            
            // الخيار القصير
            if (!opt.short_name.empty()) {
                os << colors::GREEN << opt.short_name << colors::RESET;
                if (!opt.long_name.empty()) os << ", ";
            }
            
            // الخيار الطويل
            if (!opt.long_name.empty()) {
                os << colors::GREEN << opt.long_name << colors::RESET;
            }
            
            // الخيار العربي
            if (!opt.arabic_name.empty()) {
                os << ", " << colors::GREEN << opt.arabic_name << colors::RESET;
            }
            
            // القيمة إذا مطلوبة
            if (opt.requires_value) {
                os << " <قيمة>";
            }
            
            os << std::endl;
            
            // الوصف
            os << "      " << colors::DIM << opt.description_ar << colors::RESET << std::endl;
            os << "      " << colors::DIM << opt.description << colors::RESET << std::endl;
        }
    }
    
    os << std::endl;
    print_examples(os);
}

void Command::print_examples(std::ostream& os) const {
    // التنفيذ الافتراضي - فارغ
    // يمكن للفئات الفرعية تجاوزه
}

void Command::print_success(const std::string& message) const {
    std::cout << colors::GREEN << symbols::SUCCESS << " " << message << colors::RESET << std::endl;
}

void Command::print_success_ar(const std::string& message_ar) const {
    std::cout << colors::GREEN << symbols::SUCCESS << " " << message_ar << colors::RESET << std::endl;
}

void Command::print_error(const std::string& message) const {
    std::cerr << colors::RED << symbols::FAILURE << " " << message << colors::RESET << std::endl;
}

void Command::print_error_ar(const std::string& message_ar) const {
    std::cerr << colors::RED << symbols::FAILURE << " " << message_ar << colors::RESET << std::endl;
}

void Command::print_warning(const std::string& message) const {
    std::cout << colors::YELLOW << symbols::WARNING << " " << message << colors::RESET << std::endl;
}

void Command::print_warning_ar(const std::string& message_ar) const {
    std::cout << colors::YELLOW << symbols::WARNING << " " << message_ar << colors::RESET << std::endl;
}

void Command::print_info(const std::string& message) const {
    std::cout << colors::CYAN << symbols::INFO << " " << message << colors::RESET << std::endl;
}

void Command::print_info_ar(const std::string& message_ar) const {
    std::cout << colors::CYAN << symbols::INFO << " " << message_ar << colors::RESET << std::endl;
}

void Command::print_progress(const std::string& task, int percent) const {
    // رسم شريط التقدم
    const int bar_width = 30;
    int filled = (percent * bar_width) / 100;
    
    std::cout << "\r" << colors::CYAN << symbols::PROGRESS << " " << task << " [";
    for (int i = 0; i < bar_width; ++i) {
        if (i < filled) {
            std::cout << colors::GREEN << "█";
        } else {
            std::cout << colors::DIM << "░";
        }
    }
    std::cout << colors::RESET << "] " << percent << "%" << std::flush;
    
    if (percent >= 100) {
        std::cout << std::endl;
    }
}

// ==============================================================================
// تنفيذ أمر المساعدة
// ==============================================================================

int HelpCommand::execute(const ParsedOptions& options) {
    // إذا طُلبت مساعدة لأمر محدد
    if (!options.positional_args.empty() && commands_) {
        std::string cmd_name = options.positional_args[0];
        auto it = commands_->find(cmd_name);
        if (it != commands_->end()) {
            it->second->print_help(std::cout);
            return 0;
        }
        
        // البحث بالاسم العربي
        for (const auto& [name, cmd] : *commands_) {
            if (cmd->get_arabic_name() == cmd_name) {
                cmd->print_help(std::cout);
                return 0;
            }
        }
        
        std::cerr << colors::RED;
        std::cerr << "أمر غير معروف: " << cmd_name << std::endl;
        std::cerr << "Unknown command: " << cmd_name << std::endl;
        std::cerr << colors::RESET;
        return 1;
    }
    
    // عرض المساعدة العامة
    std::cout << colors::BOLD << colors::CYAN;
    std::cout << R"(
    ╔═══════════════════════════════════════════════════╗
    ║                                                   ║
    ║           🌟 لغة ص - SAD Language 🌟             ║
    ║     أول لغة برمجة عربية كاملة المواصفات         ║
    ║                                                   ║
    ╚═══════════════════════════════════════════════════╝
)" << colors::RESET << std::endl;
    
    std::cout << colors::BOLD << "الاستخدام / Usage:" << colors::RESET << std::endl;
    std::cout << "  ص <أمر> [خيارات] [ملفات]" << std::endl;
    std::cout << "  sad <command> [options] [files]" << std::endl;
    std::cout << std::endl;
    
    std::cout << colors::BOLD << "الأوامر المتاحة / Available Commands:" << colors::RESET << std::endl;
    std::cout << std::endl;
    
    if (commands_) {
        // ترتيب الأوامر حسب الأهمية
        std::vector<std::pair<std::string, std::shared_ptr<Command>>> sorted_cmds(
            commands_->begin(), commands_->end()
        );
        
        for (const auto& [name, cmd] : sorted_cmds) {
            std::cout << "  " << colors::GREEN;
            std::cout << std::setw(12) << std::left << cmd->get_arabic_name();
            std::cout << colors::RESET << " / ";
            std::cout << colors::CYAN << std::setw(10) << std::left << name << colors::RESET;
            std::cout << "  " << cmd->get_arabic_description() << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << colors::BOLD << "أمثلة سريعة / Quick Examples:" << colors::RESET << std::endl;
    std::cout << std::endl;
    std::cout << "  " << colors::GREEN << "# إنشاء مشروع جديد" << colors::RESET << std::endl;
    std::cout << "  ص جديد مشروعي" << std::endl;
    std::cout << std::endl;
    std::cout << "  " << colors::GREEN << "# تشغيل برنامج" << colors::RESET << std::endl;
    std::cout << "  ص شغّل برنامج.ص" << std::endl;
    std::cout << std::endl;
    std::cout << "  " << colors::GREEN << "# بناء للإصدار" << colors::RESET << std::endl;
    std::cout << "  ص بناء --إصدار" << std::endl;
    std::cout << std::endl;
    
    std::cout << colors::DIM;
    std::cout << "للمزيد من المساعدة حول أمر محدد:" << std::endl;
    std::cout << "  ص مساعدة <أمر>   أو   sad help <command>" << std::endl;
    std::cout << colors::RESET << std::endl;
    
    return 0;
}

std::vector<CommandOption> HelpCommand::get_options() const {
    return {};
}

// ==============================================================================
// تنفيذ أمر الإصدار
// ==============================================================================

int VersionCommand::execute(const ParsedOptions& /*options*/) {
    std::cout << colors::BOLD << colors::CYAN;
    std::cout << R"(
    ╔═══════════════════════════════════════╗
    ║      لغة ص - SAD Language            ║
    ║      الإصدار / Version: )" << CLI_VERSION << R"(          ║
    ╚═══════════════════════════════════════╝
)" << colors::RESET << std::endl;
    
    std::cout << "  " << colors::DIM << "التاريخ / Date: " << __DATE__ << colors::RESET << std::endl;
    std::cout << "  " << colors::DIM << "المترجم / Compiler: ";
#ifdef _MSC_VER
    std::cout << "MSVC " << _MSC_VER;
#elif defined(__clang__)
    std::cout << "Clang " << __clang_major__ << "." << __clang_minor__;
#elif defined(__GNUC__)
    std::cout << "GCC " << __GNUC__ << "." << __GNUC_MINOR__;
#else
    std::cout << "Unknown";
#endif
    std::cout << colors::RESET << std::endl;
    
    std::cout << "  " << colors::DIM << "المنصة / Platform: ";
#ifdef _WIN32
    std::cout << "Windows";
#elif defined(__linux__)
    std::cout << "Linux";
#elif defined(__APPLE__)
    std::cout << "macOS";
#else
    std::cout << "Unknown";
#endif
    std::cout << colors::RESET << std::endl;
    
    std::cout << std::endl;
    std::cout << colors::DIM << "  الموقع / Website: https://sadlang.org" << colors::RESET << std::endl;
    std::cout << colors::DIM << "  المصدر / Source: https://github.com/sad-lang/sad" << colors::RESET << std::endl;
    std::cout << std::endl;
    
    return 0;
}

std::vector<CommandOption> VersionCommand::get_options() const {
    return {};
}

// ==============================================================================
// تنفيذ أمر الهاتف - ص هاتف
// Mobile Command Implementation
// ==============================================================================

int MobileCommand::execute(const ParsedOptions& options) {
    SubCommand subcmd = parse_subcommand(options);
    Platform platform = parse_platform(options);
    
    switch (subcmd) {
        case SubCommand::New:
            return execute_new(options, platform);
        case SubCommand::Build:
            return execute_build(options, platform);
        case SubCommand::Run:
            return execute_run(options, platform);
        case SubCommand::UI:
            return execute_ui(options, platform);
        case SubCommand::Info:
            return execute_info(options);
        case SubCommand::Help:
        default:
            print_help(std::cout);
            return 0;
    }
}

MobileCommand::SubCommand MobileCommand::parse_subcommand(const ParsedOptions& options) const {
    if (options.positional_args.empty()) {
        return SubCommand::Help;
    }
    
    const std::string& sub = options.positional_args[0];
    if (sub == "new" || sub == "جديد") return SubCommand::New;
    if (sub == "build" || sub == "بناء") return SubCommand::Build;
    if (sub == "run" || sub == "شغّل" || sub == "شغل") return SubCommand::Run;
    if (sub == "ui" || sub == "واجهة") return SubCommand::UI;
    if (sub == "info" || sub == "معلومات") return SubCommand::Info;
    if (sub == "help" || sub == "مساعدة") return SubCommand::Help;
    
    return SubCommand::Help;
}

MobileCommand::Platform MobileCommand::parse_platform(const ParsedOptions& options) const {
    if (options.has("android") || options.has("اندرويد")) return Platform::Android;
    if (options.has("ios") || options.has("آيفون") || options.has("ايفون")) return Platform::iOS;
    if (options.has("all") || options.has("الكل")) return Platform::Both;
    
    // الافتراضي: أندرويد
    return Platform::Android;
}

int MobileCommand::execute_new(const ParsedOptions& options, Platform platform) {
    // الحصول على اسم المشروع
    std::string project_name = "تطبيقي";
    if (options.positional_args.size() > 1) {
        project_name = options.positional_args[1];
    }
    
    // الحصول على معرف الحزمة
    std::string package_id = options.get("name", "com.example." + project_name);
    // إذا لم يحتوِ على نقطة، أضف بادئة
    if (package_id.find('.') == std::string::npos) {
        package_id = "com.example." + package_id;
    }
    
    print_info_ar("📱 إنشاء مشروع هاتف جديد: " + project_name);
    
    // استخدام MobileProjectGenerator الحقيقي
    using namespace Sad::Tools::Mobile;
    
    MobileProjectConfig config;
    config.metadata.name = project_name;
    config.metadata.arabicName = project_name;
    config.metadata.packageId = package_id;
    config.metadata.version = "1.0.0";
    config.metadata.description = "تطبيق هاتف بلغة ص";
    config.outputDirectory = std::filesystem::current_path();
    config.useKotlin = true;
    config.uiFramework = UIFramework::JetpackCompose;
    config.features.useRTLSupport = true;
    config.features.useNetworking = true;
    config.includeGitIgnore = true;
    
    switch (platform) {
        case Platform::Android:
            config.platform = MobilePlatform::Android;
            print_info_ar("المنصة: أندرويد (Android)");
            break;
        case Platform::iOS:
            config.platform = MobilePlatform::iOS;
            print_info_ar("المنصة: iOS (آيفون/آيباد)");
            break;
        case Platform::Both:
            config.platform = MobilePlatform::Both;
            print_info_ar("المنصة: متعدد المنصات (Android + iOS)");
            break;
    }
    
    MobileProjectGenerator generator;
    generator.setProgressCallback([this](const std::string& task, int percent) {
        print_progress(task, percent);
    });
    
    ProjectGenerationResult result;
    try {
        result = generator.generate(config);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cout << std::endl;
        print_error_ar("خطأ في نظام الملفات: " + std::string(e.what()));
        return 1;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        print_error_ar("خطأ: " + std::string(e.what()));
        return 1;
    }
    
    if (!result.success) {
        std::cout << std::endl;
        if (result.errors.empty()) {
            print_error_ar("فشل إنشاء المشروع (خطأ غير محدد)");
        }
        for (const auto& err : result.errors) {
            print_error_ar(err);
        }
        return 1;
    }
    
    // إنشاء ملف sad.toml في جذر المشروع
    auto projectDir = result.projectPath;
    {
        std::ostringstream toml;
        toml << "[مشروع]\n";
        toml << "اسم = \"" << project_name << "\"\n";
        toml << "نسخة = \"1.0.0\"\n";
        toml << "وصف = \"تطبيق هاتف بلغة ص\"\n";
        toml << "نوع = \"هاتف\"\n\n";
        toml << "[هاتف]\n";
        toml << "معرف = \"" << package_id << "\"\n";
        toml << "حد_أدنى_أندرويد = 21\n";
        toml << "حد_أدنى_آيفون = \"14.0\"\n";
        toml << "واجهة = \"compose\"\n\n";
        toml << "[اعتماديات]\n";
        
        std::ofstream out(projectDir / "sad.toml");
        out << toml.str();
    }
    
    // إنشاء ملف المصدر الرئيسي بلغة ص (واجهة مستخدم)
    std::filesystem::create_directories(projectDir / "src");
    {
        std::ostringstream mainSad;
        mainSad << "// بسم الله الرحمن الرحيم\n";
        mainSad << "// تطبيق هاتف - " << project_name << "\n";
        mainSad << "// تم إنشاؤه بأداة ص\n\n";
        mainSad << "استورد واجهة_هاتف\n\n";
        mainSad << "// الشاشة الرئيسية\n";
        mainSad << "واجهة شاشة_رئيسية {\n";
        mainSad << "    @حالة عداد: عدد = 0\n\n";
        mainSad << "    عرض() {\n";
        mainSad << "        عمود(توسيط: صحيح) {\n";
        mainSad << "            نص(\"مرحباً من لغة ص! 🌟\")\n";
        mainSad << "                .حجم_خط(24)\n";
        mainSad << "                .لون(.أزرق)\n\n";
        mainSad << "            نص(\"العداد: \" + نص(عداد))\n";
        mainSad << "                .حجم_خط(18)\n\n";
        mainSad << "            زر(\"+1\") {\n";
        mainSad << "                عداد = عداد + 1\n";
        mainSad << "            }\n";
        mainSad << "            .نمط(.مملوء)\n";
        mainSad << "            .لون_خلفية(.أخضر)\n";
        mainSad << "        }\n";
        mainSad << "    }\n";
        mainSad << "}\n\n";
        mainSad << "// نقطة الدخول\n";
        mainSad << "دالة رئيسية() {\n";
        mainSad << "    أعرض(شاشة_رئيسية())\n";
        mainSad << "}\n";
        
        std::ofstream out(projectDir / "src" / "main.ص");
        out << mainSad.str();
    }
    
    // توليد Gradle Wrapper إذا كان أندرويد
    if (platform == Platform::Android || platform == Platform::Both) {
        auto androidDir = projectDir / "android";
        generateGradleWrapper(androidDir);
    }
    
    // عرض النتيجة
    std::cout << std::endl;
    std::cout << result.toArabicSummary() << std::endl;
    
    print_success_ar("📱 تم إنشاء مشروع الهاتف: " + project_name);
    print_info_ar("  ابدأ بتحرير: " + (projectDir / "src" / "main.ص").string());
    print_info_ar("  للبناء: ص هاتف بناء --اندرويد");
    
    return 0;
}

int MobileCommand::execute_build(const ParsedOptions& options, Platform platform) {
    print_info_ar("📱 بناء تطبيق الهاتف...");
    
    // البحث عن جذر المشروع
    auto project_root = utils::find_project_root();
    if (!project_root) {
        project_root = utils::get_current_directory();
    }
    
    std::filesystem::path source_dir = *project_root;
    bool is_release = options.has("release") || options.has("إصدار");
    
    if (platform == Platform::Android || platform == Platform::Both) {
        print_info_ar("🤖 بناء لمنصة أندرويد...");
        int ret = build_android(source_dir, is_release, options);
        if (ret != 0) return ret;
    }
    
    if (platform == Platform::iOS || platform == Platform::Both) {
        print_info_ar("🍎 بناء لمنصة iOS...");
        int ret = build_ios(source_dir, is_release, options);
        if (ret != 0) return ret;
    }
    
    return 0;
}

int MobileCommand::execute_run(const ParsedOptions& options, Platform platform) {
    print_info_ar("📱 تشغيل تطبيق الهاتف...");
    
    // البحث عن جذر المشروع
    auto project_root = utils::find_project_root();
    if (!project_root) {
        project_root = utils::get_current_directory();
    }
    
    std::filesystem::path source_dir = *project_root;
    bool is_release = options.has("release") || options.has("إصدار");
    std::string build_type = is_release ? "release" : "debug";
    
    if (platform == Platform::Android || platform == Platform::Both) {
        print_info_ar("🤖 تشغيل على محاكي/جهاز أندرويد...");
        
        // التحقق من وجود ADB
        #ifdef _WIN32
        int adb_ret = std::system("adb devices > nul 2>&1");
        #else
        int adb_ret = std::system("adb devices > /dev/null 2>&1");
        #endif
        
        if (adb_ret != 0) {
            print_error_ar("⛔ ADB غير متوفر. تأكد من تثبيت Android SDK وإضافة platform-tools إلى PATH");
            print_info_ar("  تثبيت: sdkmanager \"platform-tools\"");
            return 1;
        }
        
        // البحث عن ملف APK
        std::filesystem::path android_dir = source_dir / "android";
        std::filesystem::path apk_path;
        
        if (is_release) {
            apk_path = android_dir / "app" / "build" / "outputs" / "apk" / "release" / "app-release.apk";
        } else {
            apk_path = android_dir / "app" / "build" / "outputs" / "apk" / "debug" / "app-debug.apk";
        }
        
        if (!std::filesystem::exists(apk_path)) {
            print_warning_ar("⚠ ملف APK غير موجود. جاري البناء أولاً...");
            int build_ret = build_android(source_dir, is_release, options);
            if (build_ret != 0) return build_ret;
        }
        
        if (!std::filesystem::exists(apk_path)) {
            print_error_ar("⛔ فشل العثور على ملف APK بعد البناء");
            return 1;
        }
        
        // تثبيت APK على الجهاز
        print_info_ar("📲 تثبيت APK على الجهاز...");
        std::string install_cmd = "adb install -r \"" + apk_path.string() + "\"";
        int install_ret = std::system(install_cmd.c_str());
        
        if (install_ret != 0) {
            print_error_ar("⛔ فشل تثبيت APK. تأكد من توصيل الجهاز وتفعيل وضع المطور.");
            return 1;
        }
        
        print_success_ar("✓ تم تثبيت APK بنجاح");
        
        // قراءة معرف الحزمة من sad.toml
        std::string package_name = "com.example.app";
        auto toml_content = utils::read_file((source_dir / "sad.toml").string());
        if (toml_content) {
            // بحث بسيط عن معرف الحزمة
            auto pos = toml_content->find("معرف = \"");
            if (pos != std::string::npos) {
                pos += std::string("معرف = \"").length();
                auto end_pos = toml_content->find("\"", pos);
                if (end_pos != std::string::npos) {
                    package_name = toml_content->substr(pos, end_pos - pos);
                }
            }
        }
        
        // تشغيل التطبيق
        print_info_ar("🚀 تشغيل التطبيق...");
        std::string launch_cmd = "adb shell am start -n " + package_name + "/.MainActivity";
        int launch_ret = std::system(launch_cmd.c_str());
        
        if (launch_ret != 0) {
            print_warning_ar("⚠ فشل تشغيل التطبيق تلقائياً. شغّله يدوياً من الجهاز.");
        } else {
            print_success_ar("✓ تم تشغيل التطبيق بنجاح! 🎉");
        }
    }
    
    if (platform == Platform::iOS || platform == Platform::Both) {
        print_info_ar("🍎 تشغيل على محاكي iOS...");
        
#ifdef __APPLE__
        std::filesystem::path ios_dir = source_dir / "ios";
        
        // تشغيل المحاكي
        print_info_ar("  جاري تشغيل محاكي iOS...");
        std::system("open -a Simulator");
        
        // بناء وتشغيل عبر xcodebuild
        std::string xcbuild_cmd = "cd \"" + ios_dir.string() + "\" && "
            "xcodebuild -scheme \"App\" -destination 'platform=iOS Simulator,name=iPhone 15' run 2>&1";
        int xc_ret = std::system(xcbuild_cmd.c_str());
        
        if (xc_ret != 0) {
            print_warning_ar("⚠ فشل التشغيل عبر xcodebuild. افتح المشروع في Xcode يدوياً.");
        } else {
            print_success_ar("✓ تم تشغيل التطبيق على محاكي iOS! 🎉");
        }
#else
        print_warning_ar("⚠ محاكي iOS متوفر فقط على macOS");
        print_info_ar("  يمكنك نقل مجلد ios/ إلى جهاز Mac وبناؤه هناك");
#endif
    }
    
    return 0;
}

int MobileCommand::execute_ui(const ParsedOptions& options, Platform platform) {
    print_info_ar("🎨 توليد كود الواجهة...");
    
    // الحصول على ملف المصدر
    std::string source_file;
    if (options.positional_args.size() > 1) {
        source_file = options.positional_args[1];
    }
    
    if (source_file.empty()) {
        print_error_ar("يجب تحديد ملف المصدر");
        print_info_ar("  مثال: ص هاتف واجهة شاشتي.ص --اندرويد");
        return 1;
    }
    
    if (!utils::file_exists(source_file)) {
        print_error_ar("الملف غير موجود: " + source_file);
        return 1;
    }
    
    auto content = utils::read_file(source_file);
    if (!content) {
        print_error_ar("فشل قراءة الملف: " + source_file);
        return 1;
    }
    
    std::string base_name = utils::get_filename(source_file);
    // إزالة الامتداد
    auto dot_pos = base_name.rfind('.');
    if (dot_pos != std::string::npos) {
        base_name = base_name.substr(0, dot_pos);
    }
    
    if (platform == Platform::Android || platform == Platform::Both) {
        print_info_ar("  → توليد كود Jetpack Compose...");
        
        // توليد كود Kotlin/Compose مباشرة من محتوى ملف ص
        std::string kotlin_code = generateKotlinFromSadSource(*content, base_name);
        
        std::string output_file = utils::change_extension(source_file, ".kt");
        if (utils::write_file(output_file, kotlin_code)) {
            print_success_ar("  ✓ تم توليد: " + output_file);
            
            // عرض إحصائيات
            int line_count = 0;
            for (char c : kotlin_code) {
                if (c == '\n') line_count++;
            }
            print_info_ar("    📊 " + std::to_string(line_count) + " سطر من كود Kotlin");
        } else {
            print_error_ar("  ✗ فشل كتابة: " + output_file);
            return 1;
        }
    }
    
    if (platform == Platform::iOS || platform == Platform::Both) {
        print_info_ar("  → توليد كود SwiftUI...");
        
        // توليد كود SwiftUI مباشرة من محتوى ملف ص 
        std::string swift_code = generateSwiftUIFromSadSource(*content, base_name);
        
        std::string output_file = utils::change_extension(source_file, ".swift");
        if (utils::write_file(output_file, swift_code)) {
            print_success_ar("  ✓ تم توليد: " + output_file);
        } else {
            print_error_ar("  ✗ فشل كتابة: " + output_file);
            return 1;
        }
    }
    
    return 0;
}

// ==============================================================================
// دوال مساعدة لبناء التطبيقات - Helper methods for building apps
// ==============================================================================

int MobileCommand::build_android(const std::filesystem::path& project_root, 
                                  bool is_release, const ParsedOptions& options) {
    namespace fs = std::filesystem;
    
    fs::path android_dir = project_root / "android";
    
    // الخطوة 1: التحقق من وجود مشروع أندرويد
    if (!fs::exists(android_dir / "app" / "build.gradle")) {
        print_error_ar("⛔ لم يُعثر على مشروع أندرويد في: " + android_dir.string());
        print_info_ar("  أنشئ مشروعاً أولاً: ص هاتف جديد اسم_المشروع --اندرويد");
        return 1;
    }
    
    print_progress("بناء أندرويد", 5);
    
    // الخطوة 2: توليد كود Kotlin من ملفات ص
    print_info_ar("📝 توليد كود Kotlin من ملفات لغة ص...");
    print_progress("بناء أندرويد", 10);
    
    fs::path src_dir = project_root / "src";
    fs::path shared_dir = project_root / "shared" / "src";
    
    // البحث عن ملف المصدر الرئيسي
    fs::path main_sad;
    if (fs::exists(src_dir / "main.ص")) {
        main_sad = src_dir / "main.ص";
    } else if (fs::exists(shared_dir / "رئيسي.ص")) {
        main_sad = shared_dir / "رئيسي.ص";
    } else if (fs::exists(project_root / "main.ص")) {
        main_sad = project_root / "main.ص";
    }
    
    if (!main_sad.empty() && fs::exists(main_sad)) {
        auto content = utils::read_file(main_sad.string());
        if (content) {
            // إيجاد مجلد كود Java/Kotlin في المشروع
            // البحث عن مجلد الحزمة
            std::string pkg_path;
            // بحث بسيط عن مجلد الحزمة الموجود
            for (auto& p : fs::recursive_directory_iterator(android_dir / "app" / "src" / "main" / "java")) {
                if (p.is_regular_file() && p.path().extension() == ".kt") {
                    pkg_path = p.path().parent_path().string();
                    break;
                }
            }
            
            if (pkg_path.empty()) {
                // مسار افتراضي
                pkg_path = (android_dir / "app" / "src" / "main" / "java" / "com" / "example" / "app").string();
                fs::create_directories(pkg_path);
            }
            
            // توليد ملف Kotlin للشاشات
            std::string kotlin_code = generateKotlinFromSadSource(*content, "MainScreen");
            std::string screens_path = pkg_path + "/SadScreens.kt";
            utils::write_file(screens_path, kotlin_code);
            
            print_success_ar("  ✓ تم توليد كود Kotlin: SadScreens.kt");
        }
    } else {
        print_warning_ar("  ⚠ لم يُعثر على ملف المصدر الرئيسي. سيُبنى المشروع كما هو.");
    }
    
    print_progress("بناء أندرويد", 30);
    
    // الخطوة 3: التحقق من المتطلبات
    print_info_ar("🔍 التحقق من متطلبات البناء...");
    
    // التحقق من Java/JDK
    const char* java_home = std::getenv("JAVA_HOME");
    #ifdef _WIN32
    int java_check = std::system("java -version > nul 2>&1");
    #else
    int java_check = std::system("java -version > /dev/null 2>&1");
    #endif
    
    if (java_check != 0 && !java_home) {
        print_error_ar("⛔ Java/JDK غير متوفر");
        print_info_ar("  ثبّت JDK 17 أو أحدث:");
        print_info_ar("  https://adoptium.net/");
        return 1;
    }
    
    if (java_home) {
        print_success_ar("  ✓ JAVA_HOME: " + std::string(java_home));
    }
    
    print_progress("بناء أندرويد", 40);
    
    // الخطوة 4: التأكد من وجود Gradle Wrapper
    fs::path gradlew;
    #ifdef _WIN32
    gradlew = android_dir / "gradlew.bat";
    #else
    gradlew = android_dir / "gradlew";
    #endif
    
    if (!fs::exists(gradlew)) {
        print_info_ar("📦 توليد Gradle Wrapper...");
        generateGradleWrapper(android_dir);
        
        if (!fs::exists(gradlew)) {
            // إذا فشل توليد الـ wrapper، حاول استخدام gradle المثبت عالمياً
            print_warning_ar("  ⚠ لم يُعثر على Gradle Wrapper");
            
            #ifdef _WIN32
            int gradle_check = std::system("gradle --version > nul 2>&1");
            #else
            int gradle_check = std::system("gradle --version > /dev/null 2>&1");
            #endif
            
            if (gradle_check == 0) {
                print_info_ar("  استخدام Gradle المثبت عالمياً...");
                
                // توليد wrapper عبر gradle
                std::string gen_wrapper_cmd = "cd /d \"" + android_dir.string() + "\" && gradle wrapper --gradle-version 8.4";
                std::system(gen_wrapper_cmd.c_str());
            } else {
                print_error_ar("⛔ Gradle غير متوفر. الخيارات:");
                print_info_ar("  1. ثبّت Gradle: https://gradle.org/install/");
                print_info_ar("  2. افتح المشروع في Android Studio (يتضمن Gradle)");
                print_info_ar("  3. مجلد المشروع: " + android_dir.string());
                return 1;
            }
        }
    }
    
    print_progress("بناء أندرويد", 50);
    
    // الخطوة 5: تنفيذ بناء Gradle الفعلي
    std::string build_task = is_release ? "assembleRelease" : "assembleDebug";
    print_info_ar("🔨 بناء APK (" + std::string(is_release ? "إصدار" : "تطوير") + ")...");
    
    // إصلاح: التحقق من صحة المسار قبل استخدامه
    std::string android_path = android_dir.string();
    if (!security::is_safe_path(android_path)) {
        print_error_ar("⛔ مسار المشروع يحتوي على أحرف غير آمنة");
        return 1;
    }
    
    std::string build_cmd;
    #ifdef _WIN32
    if (fs::exists(android_dir / "gradlew.bat")) {
        build_cmd = "cd /d \"" + android_dir.string() + "\" && gradlew.bat " + build_task + " 2>&1";
    } else {
        build_cmd = "cd /d \"" + android_dir.string() + "\" && gradle " + build_task + " 2>&1";
    }
    #else
    if (fs::exists(android_dir / "gradlew")) {
        // التأكد من أن gradlew قابل للتنفيذ
        // إصلاح: استخدام مسار مُطهَّر
        std::string gradlew_path = security::sanitize_path((android_dir / "gradlew").string());
        std::system(("chmod +x \"" + gradlew_path + "\"").c_str());
        build_cmd = "cd \"" + android_path + "\" && ./gradlew " + build_task + " 2>&1";
    } else {
        build_cmd = "cd \"" + android_dir.string() + "\" && gradle " + build_task + " 2>&1";
    }
    #endif
    
    print_progress("بناء أندرويد", 60);
    print_info_ar("  ⏳ جاري تنفيذ Gradle (قد يستغرق وقتاً في المرة الأولى)...");
    std::cout << std::endl;
    
    int build_ret = std::system(build_cmd.c_str());
    
    std::cout << std::endl;
    
    if (build_ret != 0) {
        print_error_ar("⛔ فشل بناء Gradle!");
        print_info_ar("  نصائح:");
        print_info_ar("  1. تحقق من أن JAVA_HOME يشير إلى JDK 17+");
        print_info_ar("  2. تحقق من أن Android SDK مثبت (ANDROID_HOME)");
        print_info_ar("  3. جرّب فتح المشروع في Android Studio");
        print_info_ar("  مجلد المشروع: " + android_dir.string());
        return 1;
    }
    
    print_progress("بناء أندرويد", 100);
    
    // البحث عن ملف APK المُنتج
    fs::path apk_path;
    if (is_release) {
        apk_path = android_dir / "app" / "build" / "outputs" / "apk" / "release" / "app-release.apk";
        if (!fs::exists(apk_path)) {
            apk_path = android_dir / "app" / "build" / "outputs" / "apk" / "release" / "app-release-unsigned.apk";
        }
    } else {
        apk_path = android_dir / "app" / "build" / "outputs" / "apk" / "debug" / "app-debug.apk";
    }
    
    if (fs::exists(apk_path)) {
        auto size = fs::file_size(apk_path);
        std::string size_str;
        if (size > 1024 * 1024) {
            size_str = std::to_string(size / (1024 * 1024)) + " MB";
        } else {
            size_str = std::to_string(size / 1024) + " KB";
        }
        
        print_success_ar("✅ تم بناء APK بنجاح!");
        print_info_ar("  📦 " + apk_path.string());
        print_info_ar("  📊 الحجم: " + size_str);
        print_info_ar("  🚀 للتثبيت: ص هاتف شغّل --اندرويد");
    } else {
        print_success_ar("✓ بناء Gradle مكتمل (تحقق من مجلد الإخراج يدوياً)");
        print_info_ar("  مجلد الإخراج: " + (android_dir / "app" / "build" / "outputs" / "apk").string());
    }
    
    return 0;
}

int MobileCommand::build_ios(const std::filesystem::path& project_root,
                              bool is_release, const ParsedOptions& options) {
    namespace fs = std::filesystem;
    
    fs::path ios_dir = project_root / "ios";
    
    if (!fs::exists(ios_dir)) {
        print_error_ar("⛔ لم يُعثر على مشروع iOS في: " + ios_dir.string());
        return 1;
    }
    
#ifndef __APPLE__
    print_warning_ar("⚠ بناء iOS يتطلب نظام macOS مع Xcode");
    print_info_ar("  سيتم توليد كود SwiftUI فقط.");
    print_info_ar("  انقل مجلد ios/ إلى جهاز Mac لبنائه.");
    
    // توليد كود SwiftUI من ملفات ص
    fs::path main_sad;
    if (fs::exists(project_root / "src" / "main.ص")) {
        main_sad = project_root / "src" / "main.ص";
    } else if (fs::exists(project_root / "shared" / "src" / "رئيسي.ص")) {
        main_sad = project_root / "shared" / "src" / "رئيسي.ص";
    }
    
    if (!main_sad.empty() && fs::exists(main_sad)) {
        auto content = utils::read_file(main_sad.string());
        if (content) {
            std::string swift_code = generateSwiftUIFromSadSource(*content, "MainScreen");
            // البحث عن مجلد المصدر في iOS
            for (auto& p : fs::recursive_directory_iterator(ios_dir)) {
                if (p.is_regular_file() && p.path().extension() == ".swift") {
                    std::string swift_path = p.path().parent_path().string() + "/SadScreens.swift";
                    utils::write_file(swift_path, swift_code);
                    print_success_ar("  ✓ تم توليد: SadScreens.swift");
                    break;
                }
            }
        }
    }
    
    return 0;
#else
    // على macOS: استخدام xcodebuild
    print_info_ar("🍎 بناء مشروع iOS...");
    
    std::string config = is_release ? "Release" : "Debug";
    std::string xcbuild_cmd = "cd \"" + ios_dir.string() + "\" && "
        "xcodebuild -scheme \"App\" -configuration " + config + " build 2>&1";
    
    int ret = std::system(xcbuild_cmd.c_str());
    if (ret != 0) {
        print_error_ar("⛔ فشل بناء iOS");
        print_info_ar("  افتح المشروع في Xcode: " + ios_dir.string());
        return 1;
    }
    
    print_success_ar("✅ تم بناء مشروع iOS بنجاح!");
    return 0;
#endif
}

} // namespace cli
} // namespace sad
