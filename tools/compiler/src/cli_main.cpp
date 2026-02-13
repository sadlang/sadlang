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
        
        std::ofstream out(projectDir / "src" / "main.sad");
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
    print_info_ar("  ابدأ بتحرير: " + (projectDir / "src" / "main.sad").string());
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
    if (fs::exists(src_dir / "main.sad")) {
        main_sad = src_dir / "main.sad";
    } else if (fs::exists(shared_dir / "رئيسي.ص")) {
        main_sad = shared_dir / "رئيسي.ص";
    } else if (fs::exists(project_root / "main.sad")) {
        main_sad = project_root / "main.sad";
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
        std::system(("chmod +x \"" + (android_dir / "gradlew").string() + "\"").c_str());
        build_cmd = "cd \"" + android_dir.string() + "\" && ./gradlew " + build_task + " 2>&1";
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
    if (fs::exists(project_root / "src" / "main.sad")) {
        main_sad = project_root / "src" / "main.sad";
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

void MobileCommand::generateGradleWrapper(const std::filesystem::path& android_dir) {
    namespace fs = std::filesystem;
    
    // إنشاء مجلد gradle/wrapper
    fs::create_directories(android_dir / "gradle" / "wrapper");
    
    // إنشاء gradle-wrapper.properties
    {
        std::ofstream out(android_dir / "gradle" / "wrapper" / "gradle-wrapper.properties");
        out << "distributionBase=GRADLE_USER_HOME\n";
        out << "distributionPath=wrapper/dists\n";
        out << "distributionUrl=https\\://services.gradle.org/distributions/gradle-8.4-bin.zip\n";
        out << "networkTimeout=10000\n";
        out << "validateDistributionUrl=true\n";
        out << "zipStoreBase=GRADLE_USER_HOME\n";
        out << "zipStorePath=wrapper/dists\n";
    }
    
    // إنشاء gradlew.bat (Windows)
    {
        std::ofstream out(android_dir / "gradlew.bat");
        out << "@rem\n";
        out << "@rem Gradle startup script for Windows - Generated by SAD Language\n";
        out << "@rem\n\n";
        out << "@if \"%DEBUG%\"==\"\" @echo off\n";
        out << "@rem Set local scope for the variables with windows NT shell\n";
        out << "if \"%OS%\"==\"Windows_NT\" setlocal\n\n";
        out << "set DIRNAME=%~dp0\n";
        out << "if \"%DIRNAME%\"==\"\" set DIRNAME=.\n";
        out << "@rem This is normally unused\n";
        out << "set APP_BASE_NAME=%~n0\n";
        out << "set APP_HOME=%DIRNAME%\n\n";
        out << "@rem Resolve any \".\" and \"..\" in APP_HOME to make it shorter.\n";
        out << "for %%i in (\"%APP_HOME%\") do set APP_HOME=%%~fi\n\n";
        out << "@rem Add default JVM options here.\n";
        out << "set DEFAULT_JVM_OPTS=\"-Xmx64m\" \"-Xms64m\"\n\n";
        out << "@rem Find java.exe\n";
        out << "if defined JAVA_HOME goto findJavaFromJavaHome\n\n";
        out << "set JAVA_EXE=java.exe\n";
        out << "%JAVA_EXE% -version >NUL 2>&1\n";
        out << "if %ERRORLEVEL% equ 0 goto execute\n\n";
        out << "echo.\n";
        out << "echo ERROR: JAVA_HOME is not set and no 'java' command found.\n";
        out << "echo Please set the JAVA_HOME variable in your environment.\n";
        out << "goto fail\n\n";
        out << ":findJavaFromJavaHome\n";
        out << "set JAVA_HOME=%JAVA_HOME:\"=%\n";
        out << "set JAVA_EXE=%JAVA_HOME%/bin/java.exe\n\n";
        out << "if exist \"%JAVA_EXE%\" goto execute\n\n";
        out << "echo.\n";
        out << "echo ERROR: JAVA_HOME is set to an invalid directory: %JAVA_HOME%\n";
        out << "goto fail\n\n";
        out << ":execute\n";
        out << "@rem Setup the command line\n";
        out << "set CLASSPATH=%APP_HOME%\\gradle\\wrapper\\gradle-wrapper.jar\n\n";
        out << "@rem Execute Gradle\n";
        out << "\"%JAVA_EXE%\" %DEFAULT_JVM_OPTS% %JAVA_OPTS% %GRADLE_OPTS% "
            << "\"-Dorg.gradle.appname=%APP_BASE_NAME%\" "
            << "-classpath \"%CLASSPATH%\" "
            << "org.gradle.wrapper.GradleWrapperMain %*\n\n";
        out << ":end\n";
        out << "@rem End local scope for the variables with windows NT shell\n";
        out << "if \"%OS%\"==\"Windows_NT\" endlocal\n\n";
        out << ":omega\n";
        out << "exit /b %ERRORLEVEL%\n\n";
        out << ":fail\n";
        out << "exit /b 1\n";
    }
    
    // إنشاء gradlew (Unix/Mac)
    {
        std::ofstream out(android_dir / "gradlew");
        out << "#!/bin/sh\n";
        out << "# Gradle startup script for UNIX - Generated by SAD Language\n\n";
        out << "APP_NAME=\"Gradle\"\n";
        out << "APP_BASE_NAME=$(basename \"$0\")\n";
        out << "DEFAULT_JVM_OPTS='-Xmx64m -Xms64m'\n\n";
        out << "# Resolve the true real path of the script\n";
        out << "PRG=\"$0\"\n";
        out << "while [ -h \"$PRG\" ]; do\n";
        out << "    ls=$(ls -ld \"$PRG\")\n";
        out << "    link=$(expr \"$ls\" : '.*-> \\(.*\\)$')\n";
        out << "    if expr \"$link\" : '/.*' > /dev/null; then\n";
        out << "        PRG=\"$link\"\n";
        out << "    else\n";
        out << "        PRG=$(dirname \"$PRG\")/\"$link\"\n";
        out << "    fi\n";
        out << "done\n";
        out << "APP_HOME=$(cd \"$(dirname \"$PRG\")\" && pwd -P)\n\n";
        out << "CLASSPATH=$APP_HOME/gradle/wrapper/gradle-wrapper.jar\n\n";
        out << "# Determine the Java command to use\n";
        out << "if [ -n \"$JAVA_HOME\" ]; then\n";
        out << "    JAVACMD=\"$JAVA_HOME/bin/java\"\n";
        out << "else\n";
        out << "    JAVACMD=\"java\"\n";
        out << "fi\n\n";
        out << "# Execute Gradle\n";
        out << "exec \"$JAVACMD\" $DEFAULT_JVM_OPTS $JAVA_OPTS $GRADLE_OPTS "
            << "\"-Dorg.gradle.appname=$APP_BASE_NAME\" "
            << "-classpath \"$CLASSPATH\" "
            << "org.gradle.wrapper.GradleWrapperMain \"$@\"\n";
    }
    
    // ملاحظة: gradle-wrapper.jar يجب أن يكون موجوداً
    auto jar_path = android_dir / "gradle" / "wrapper" / "gradle-wrapper.jar";
    if (!fs::exists(jar_path)) {
        // 1. محاولة من مجلد resources بجوار الملف التنفيذي
        #ifdef _WIN32
        {
            // الحصول على مسار الملف التنفيذي
            char exe_path[MAX_PATH];
            GetModuleFileNameA(NULL, exe_path, MAX_PATH);
            fs::path exe_dir = fs::path(exe_path).parent_path();
            // البحث بالتسلسل: بجوار exe → في resources → في tools/compiler/resources
            for (auto& candidate : {
                exe_dir / "gradle-wrapper.jar",
                exe_dir / "resources" / "gradle-wrapper.jar",
                exe_dir / ".." / ".." / ".." / "tools" / "compiler" / "resources" / "gradle-wrapper.jar"
            }) {
                if (fs::exists(candidate)) {
                    fs::copy_file(candidate, jar_path, fs::copy_options::skip_existing);
                    break;
                }
            }
        }
        #endif
        
        // 2. محاولة نسخ jar من Gradle المثبت
        if (!fs::exists(jar_path)) {
            const char* gradle_home = std::getenv("GRADLE_HOME");
            if (gradle_home) {
                fs::path local_jar = fs::path(gradle_home) / "lib" / "gradle-wrapper.jar";
                if (fs::exists(local_jar)) {
                    fs::copy_file(local_jar, jar_path, fs::copy_options::skip_existing);
                }
            }
        }
        
        // 3. محاولة من مسار المستخدم ~/.gradle (بحث موسع)
        if (!fs::exists(jar_path)) {
            #ifdef _WIN32
            const char* user_home = std::getenv("USERPROFILE");
            #else
            const char* user_home = std::getenv("HOME");
            #endif
            
            if (user_home) {
                fs::path gradle_cache = fs::path(user_home) / ".gradle" / "wrapper" / "dists";
                if (fs::exists(gradle_cache)) {
                    // البحث عن أي ملف jar يحتوي على "gradle-wrapper" في الاسم
                    for (auto& entry : fs::recursive_directory_iterator(gradle_cache)) {
                        std::string fname = entry.path().filename().string();
                        if (fname == "gradle-wrapper.jar" || 
                            (fname.find("gradle-wrapper") != std::string::npos && 
                             fname.find(".jar") != std::string::npos &&
                             fname.find("shared") == std::string::npos)) {
                            fs::copy_file(entry.path(), jar_path, fs::copy_options::skip_existing);
                            break;
                        }
                    }
                }
            }
        }
        
        if (!fs::exists(jar_path)) {
            print_warning_ar("⚠ لم يُعثر على gradle-wrapper.jar");
            print_info_ar("  شغّل 'gradle wrapper' في مجلد android/ لتوليده");
        }
    }
}

std::string MobileCommand::generateKotlinFromSadSource(const std::string& sad_source,
                                                        const std::string& screen_name) {
    std::ostringstream kt;
    
    // تحليل بسيط لملف ص واستخراج المكونات
    // البحث عن تعريفات الواجهة: واجهة اسم_الشاشة { ... }
    
    // استخراج معلومات من المصدر
    struct UIComponent {
        std::string type;        // نص، زر، عمود، صف، صورة، حقل_نص
        std::string content;     // المحتوى النصي
        std::string modifier;    // المعدّلات
        std::vector<std::string> children;
    };
    
    // تحليل المكونات من كود ص
    std::vector<UIComponent> components;
    std::vector<std::pair<std::string, std::string>> state_vars; // اسم، نوع
    std::string view_name = screen_name;
    
    std::istringstream stream(sad_source);
    std::string line;
    bool in_view = false;
    bool in_display = false;
    
    while (std::getline(stream, line)) {
        // تنظيف
        std::string trimmed = line;
        size_t start = trimmed.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        trimmed = trimmed.substr(start);
        
        // تخطي التعليقات
        if (trimmed.substr(0, 2) == "//") continue;
        
        // اكتشاف واجهة
        if (trimmed.find("واجهة ") == 0) {
            auto space_pos = trimmed.find(' ');
            auto brace_pos = trimmed.find('{');
            if (space_pos != std::string::npos) {
                view_name = trimmed.substr(space_pos + 1);
                if (brace_pos != std::string::npos) {
                    view_name = trimmed.substr(space_pos + 1, brace_pos - space_pos - 1);
                }
                // إزالة مسافات
                while (!view_name.empty() && (view_name.back() == ' ' || view_name.back() == '{'))
                    view_name.pop_back();
                in_view = true;
            }
            continue;
        }
        
        // اكتشاف حالة (@حالة)
        if (in_view && trimmed.find("@حالة") != std::string::npos) {
            // @حالة اسم: نوع = قيمة 
            auto colon_pos = trimmed.find(':');
            auto eq_pos = trimmed.find('=');
            if (colon_pos != std::string::npos) {
                std::string var_part = trimmed.substr(trimmed.find("حالة") + 5, colon_pos - trimmed.find("حالة") - 5);
                // تنظيف
                while (!var_part.empty() && var_part.front() == ' ') var_part.erase(var_part.begin());
                while (!var_part.empty() && var_part.back() == ' ') var_part.pop_back();
                
                std::string type_part = "عدد"; // افتراضي
                if (colon_pos != std::string::npos) {
                    size_t end = (eq_pos != std::string::npos) ? eq_pos : trimmed.length();
                    type_part = trimmed.substr(colon_pos + 1, end - colon_pos - 1);
                    while (!type_part.empty() && type_part.front() == ' ') type_part.erase(type_part.begin());
                    while (!type_part.empty() && type_part.back() == ' ') type_part.pop_back();
                }
                
                state_vars.push_back({var_part, type_part});
            }
            continue;
        }
        
        // اكتشاف عرض
        if (in_view && trimmed.find("عرض()") != std::string::npos) {
            in_display = true;
            continue;
        }
        
        // اكتشاف المكونات
        if (in_display) {
            if (trimmed.find("نص(") == 0 || trimmed.find("نص(") != std::string::npos) {
                auto paren_start = trimmed.find("نص(") + 4;
                if (trimmed.find("نص(") == 0) paren_start = 3;
                // البحث بعد "نص("
                std::string text_start = trimmed.substr(trimmed.find("نص(") + 3);
                auto quote_start = text_start.find('"');
                auto quote_end = text_start.rfind('"');
                std::string text_content;
                if (quote_start != std::string::npos && quote_end > quote_start) {
                    text_content = text_start.substr(quote_start + 1, quote_end - quote_start - 1);
                } else {
                    // expression - e.g., "العداد: " + نص(عداد)
                    auto paren_end = text_start.rfind(')');
                    if (paren_end != std::string::npos) {
                        text_content = text_start.substr(0, paren_end);
                    }
                }
                components.push_back({"نص", text_content, "", {}});
            }
            else if (trimmed.find("زر(") != std::string::npos) {
                auto quote_start = trimmed.find('"');
                auto quote_end = trimmed.rfind('"');
                std::string btn_text;
                if (quote_start != std::string::npos && quote_end > quote_start) {
                    btn_text = trimmed.substr(quote_start + 1, quote_end - quote_start - 1);
                }
                components.push_back({"زر", btn_text, "", {}});
            }
        }
    }
    
    // إنشاء ملف Kotlin/Compose
    kt << "// ╔════════════════════════════════════════════════════════════════════╗\n";
    kt << "// ║    مُولَّد تلقائياً بواسطة مترجم لغة ص                            ║\n";
    kt << "// ║    Generated by SAD Language Compiler                             ║\n";
    kt << "// ╚════════════════════════════════════════════════════════════════════╝\n\n";
    
    kt << "package com.example.app\n\n";
    
    kt << "import android.os.Bundle\n";
    kt << "import androidx.activity.ComponentActivity\n";
    kt << "import androidx.activity.compose.setContent\n";
    kt << "import androidx.compose.foundation.layout.*\n";
    kt << "import androidx.compose.material3.*\n";
    kt << "import androidx.compose.runtime.*\n";
    kt << "import androidx.compose.ui.Alignment\n";
    kt << "import androidx.compose.ui.Modifier\n";
    kt << "import androidx.compose.ui.graphics.Color\n";
    kt << "import androidx.compose.ui.text.style.TextAlign\n";
    kt << "import androidx.compose.ui.unit.dp\n";
    kt << "import androidx.compose.ui.unit.sp\n";
    kt << "import androidx.compose.ui.platform.LocalLayoutDirection\n";
    kt << "import androidx.compose.ui.unit.LayoutDirection\n";
    kt << "import androidx.compose.runtime.CompositionLocalProvider\n\n";
    
    // توليد الشاشة الرئيسية
    kt << "/**\n";
    kt << " * الشاشة الرئيسية - مُولَّدة من كود لغة ص\n";
    kt << " */\n";
    kt << "@Composable\n";
    kt << "fun " << screen_name << "Screen() {\n";
    kt << "    // دعم الاتجاه من اليمين لليسار\n";
    kt << "    CompositionLocalProvider(LocalLayoutDirection provides LayoutDirection.Rtl) {\n";
    
    // متغيرات الحالة
    for (const auto& [name, type] : state_vars) {
        std::string kt_type = "Int";
        std::string kt_default = "0";
        if (type == "نص" || type == "String") {
            kt_type = "String";
            kt_default = "\"\"";
        } else if (type == "منطقي" || type == "Boolean") {
            kt_type = "Boolean";
            kt_default = "false";
        } else if (type == "عشري" || type == "Double") {
            kt_type = "Double";
            kt_default = "0.0";
        }
        
        kt << "        var " << name << " by remember { mutableStateOf(" << kt_default << ") }\n";
    }
    
    if (!state_vars.empty()) kt << "\n";
    
    kt << "        Surface(\n";
    kt << "            modifier = Modifier.fillMaxSize(),\n";
    kt << "            color = MaterialTheme.colorScheme.background\n";
    kt << "        ) {\n";
    kt << "            Column(\n";
    kt << "                modifier = Modifier\n";
    kt << "                    .fillMaxSize()\n";
    kt << "                    .padding(16.dp),\n";
    kt << "                verticalArrangement = Arrangement.Center,\n";
    kt << "                horizontalAlignment = Alignment.CenterHorizontally\n";
    kt << "            ) {\n";
    
    // المكونات
    for (const auto& comp : components) {
        if (comp.type == "نص") {
            kt << "                Text(\n";
            
            // فحص ما إذا كان تعبيراً أو نصاً ثابتاً
            if (comp.content.find('+') != std::string::npos || comp.content.find("نص(") != std::string::npos) {
                // تعبير ديناميكي - تحويل بسيط
                std::string expr = comp.content;
                // استبدال "نص(VARNAME)" بـ "$VARNAME"
                // بسيط: نترك كتعليق وقيمة ثابتة
                kt << "                    text = \"" << comp.content << "\", // TODO: dynamic expression\n";
            } else {
                kt << "                    text = \"" << comp.content << "\",\n";
            }
            
            kt << "                    style = MaterialTheme.typography.headlineMedium,\n";
            kt << "                    textAlign = TextAlign.Center\n";
            kt << "                )\n";
            kt << "                Spacer(modifier = Modifier.height(16.dp))\n";
        }
        else if (comp.type == "زر") {
            kt << "                Button(\n";
            kt << "                    onClick = { /* TODO: action */ },\n";
            kt << "                    modifier = Modifier.padding(8.dp)\n";
            kt << "                ) {\n";
            kt << "                    Text(\"" << comp.content << "\")\n";
            kt << "                }\n";
            kt << "                Spacer(modifier = Modifier.height(8.dp))\n";
        }
    }
    
    // إذا لم نجد مكونات، أضف شاشة افتراضية
    if (components.empty()) {
        kt << "                Text(\n";
        kt << "                    text = \"مرحباً من لغة ص! 🌟\",\n";
        kt << "                    style = MaterialTheme.typography.headlineLarge,\n";
        kt << "                    textAlign = TextAlign.Center\n";
        kt << "                )\n";
        kt << "                Spacer(modifier = Modifier.height(16.dp))\n";
        kt << "                Text(\n";
        kt << "                    text = \"تطبيق مُولَّد تلقائياً\",\n";
        kt << "                    style = MaterialTheme.typography.bodyLarge,\n";
        kt << "                    color = MaterialTheme.colorScheme.secondary\n";
        kt << "                )\n";
    }
    
    kt << "            }\n";
    kt << "        }\n";
    kt << "    }\n";
    kt << "}\n";
    
    return kt.str();
}

std::string MobileCommand::generateSwiftUIFromSadSource(const std::string& sad_source,
                                                         const std::string& screen_name) {
    std::ostringstream sw;
    
    sw << "// ╔════════════════════════════════════════════════════════════════════╗\n";
    sw << "// ║    مُولَّد تلقائياً بواسطة مترجم لغة ص                            ║\n";
    sw << "// ║    Generated by SAD Language Compiler                             ║\n";
    sw << "// ╚════════════════════════════════════════════════════════════════════╝\n\n";
    
    sw << "import SwiftUI\n\n";
    
    sw << "/// الشاشة الرئيسية - مُولَّدة من كود لغة ص\n";
    sw << "struct " << screen_name << "View: View {\n";
    
    // تحليل بسيط لاستخراج حالات
    std::istringstream stream(sad_source);
    std::string line;
    std::vector<std::pair<std::string, std::string>> state_vars;
    
    while (std::getline(stream, line)) {
        std::string trimmed = line;
        size_t start = trimmed.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        trimmed = trimmed.substr(start);
        
        if (trimmed.find("@حالة") != std::string::npos) {
            auto colon_pos = trimmed.find(':');
            if (colon_pos != std::string::npos) {
                std::string var_part = trimmed.substr(trimmed.find("حالة") + 5, colon_pos - trimmed.find("حالة") - 5);
                while (!var_part.empty() && var_part.front() == ' ') var_part.erase(var_part.begin());
                while (!var_part.empty() && var_part.back() == ' ') var_part.pop_back();
                
                auto eq_pos = trimmed.find('=');
                std::string type_part = trimmed.substr(colon_pos + 1, (eq_pos != std::string::npos ? eq_pos : trimmed.length()) - colon_pos - 1);
                while (!type_part.empty() && type_part.front() == ' ') type_part.erase(type_part.begin());
                while (!type_part.empty() && type_part.back() == ' ') type_part.pop_back();
                
                std::string swift_type = "Int";
                std::string swift_default = "0";
                if (type_part == "نص" || type_part == "String") {
                    swift_type = "String";
                    swift_default = "\"\"";
                } else if (type_part == "منطقي" || type_part == "Bool") {
                    swift_type = "Bool";
                    swift_default = "false";
                }
                
                sw << "    @State private var " << var_part << ": " << swift_type << " = " << swift_default << "\n";
                state_vars.push_back({var_part, swift_type});
            }
        }
    }
    
    if (!state_vars.empty()) sw << "\n";
    
    sw << "    var body: some View {\n";
    sw << "        NavigationView {\n";
    sw << "            VStack(spacing: 20) {\n";
    sw << "                Text(\"مرحباً من لغة ص! 🌟\")\n";
    sw << "                    .font(.largeTitle)\n";
    sw << "                    .fontWeight(.bold)\n";
    sw << "                    .multilineTextAlignment(.center)\n";
    sw << "\n";
    sw << "                Text(\"تطبيق مُولَّد تلقائياً\")\n";
    sw << "                    .font(.title2)\n";
    sw << "                    .foregroundColor(.secondary)\n";
    
    for (const auto& [name, type] : state_vars) {
        sw << "\n";
        sw << "                Text(\"" << name << ": \\(" << name << ")\")\n";
        sw << "                    .font(.title3)\n";
    }
    
    sw << "            }\n";
    sw << "            .padding()\n";
    sw << "            .environment(\\.layoutDirection, .rightToLeft)\n";
    sw << "        }\n";
    sw << "    }\n";
    sw << "}\n\n";
    
    sw << "#Preview {\n";
    sw << "    " << screen_name << "View()\n";
    sw << "}\n";
    
    return sw.str();
}

int MobileCommand::execute_info(const ParsedOptions& options) {
    std::cout << colors::BOLD << colors::CYAN;
    std::cout << R"(
    ╔═══════════════════════════════════════════════════╗
    ║     📱 نظام تطبيقات الهاتف بلغة ص                ║
    ║     Mobile Application System for SAD Language    ║
    ╚═══════════════════════════════════════════════════╝
)" << colors::RESET << std::endl;
    
    std::cout << colors::BOLD << "المنصات المدعومة:" << colors::RESET << std::endl;
    std::cout << "  🤖 أندرويد (Android 5.0+) - NDK + Jetpack Compose" << std::endl;
    std::cout << "  🍎 iOS (14.0+) - Swift + SwiftUI" << std::endl;
    std::cout << std::endl;
    
    std::cout << colors::BOLD << "الميزات:" << colors::RESET << std::endl;
    std::cout << "  • واجهة مستخدم تصريحية بالعربية" << std::endl;
    std::cout << "  • كود واحد لجميع المنصات" << std::endl;
    std::cout << "  • توليد كود أصلي (Kotlin/Swift)" << std::endl;
    std::cout << "  • بناء APK فعلي عبر Gradle" << std::endl;
    std::cout << "  • تثبيت وتشغيل عبر ADB" << std::endl;
    std::cout << "  • دعم الكاميرا والموقع والمستشعرات" << std::endl;
    std::cout << std::endl;
    
    std::cout << colors::BOLD << "حالة الأدوات:" << colors::RESET << std::endl;
    
    const char* ndk = std::getenv("ANDROID_NDK_HOME");
    if (ndk) {
        std::cout << "  " << colors::GREEN << "✓" << colors::RESET << " Android NDK: " << ndk << std::endl;
    } else {
        std::cout << "  " << colors::RED << "✗" << colors::RESET << " Android NDK: غير معين (ANDROID_NDK_HOME)" << std::endl;
    }
    
    const char* sdk = std::getenv("ANDROID_HOME");
    if (sdk) {
        std::cout << "  " << colors::GREEN << "✓" << colors::RESET << " Android SDK: " << sdk << std::endl;
    } else {
        std::cout << "  " << colors::RED << "✗" << colors::RESET << " Android SDK: غير معين (ANDROID_HOME)" << std::endl;
    }
    
    const char* java_home = std::getenv("JAVA_HOME");
    if (java_home) {
        std::cout << "  " << colors::GREEN << "✓" << colors::RESET << " JAVA_HOME: " << java_home << std::endl;
    } else {
        std::cout << "  " << colors::RED << "✗" << colors::RESET << " JAVA_HOME: غير معين" << std::endl;
    }
    
    // التحقق من ADB
    #ifdef _WIN32
    int adb_ret = std::system("adb version > nul 2>&1");
    #else
    int adb_ret = std::system("adb version > /dev/null 2>&1");
    #endif
    
    if (adb_ret == 0) {
        std::cout << "  " << colors::GREEN << "✓" << colors::RESET << " ADB: متوفر" << std::endl;
    } else {
        std::cout << "  " << colors::RED << "✗" << colors::RESET << " ADB: غير متوفر" << std::endl;
    }
    
    // التحقق من Gradle
    #ifdef _WIN32
    int gradle_ret = std::system("gradle --version > nul 2>&1");
    #else
    int gradle_ret = std::system("gradle --version > /dev/null 2>&1");
    #endif
    
    if (gradle_ret == 0) {
        std::cout << "  " << colors::GREEN << "✓" << colors::RESET << " Gradle: متوفر" << std::endl;
    } else {
        std::cout << "  " << colors::YELLOW << "⚠" << colors::RESET << " Gradle: غير متوفر (ليس مطلوباً مع Gradle Wrapper)" << std::endl;
    }
    
#ifdef __APPLE__
    std::cout << "  " << colors::GREEN << "✓" << colors::RESET << " Xcode: متوفر" << std::endl;
#else
    std::cout << "  " << colors::YELLOW << "⚠" << colors::RESET << " Xcode: غير متوفر (يتطلب macOS)" << std::endl;
#endif
    
    std::cout << std::endl;
    std::cout << colors::BOLD << "الأوامر:" << colors::RESET << std::endl;
    std::cout << "  ص هاتف جديد <اسم>     إنشاء مشروع هاتف جديد" << std::endl;
    std::cout << "  ص هاتف بناء            بناء APK (يتطلب JDK + Android SDK)" << std::endl;
    std::cout << "  ص هاتف شغّل             تثبيت وتشغيل على الجهاز (يتطلب ADB)" << std::endl;
    std::cout << "  ص هاتف واجهة <ملف>     توليد كود Kotlin/Swift من كود ص" << std::endl;
    std::cout << "  ص هاتف معلومات          عرض هذه المعلومات" << std::endl;
    std::cout << std::endl;
    
    return 0;
}

std::vector<CommandOption> MobileCommand::get_options() const {
    return {
        {"-a", "--android", "--اندرويد", "Target Android platform", "استهداف منصة أندرويد", false, "", true},
        {"-i", "--ios", "--آيفون", "Target iOS platform", "استهداف منصة iOS", false, "", true},
        {"", "--all", "--الكل", "Target all platforms", "استهداف جميع المنصات", false, "", true},
        {"-o", "--output", "--إخراج", "Output directory", "مجلد الإخراج", true, ".", false},
        {"-n", "--name", "--اسم", "Project/app name", "اسم المشروع/التطبيق", true, "", false},
        {"", "--release", "--إصدار", "Build in release mode", "بناء بوضع الإصدار", false, "", true},
    };
}

void MobileCommand::print_examples(std::ostream& os) const {
    os << colors::BOLD << "أمثلة الاستخدام / Usage Examples:" << colors::RESET << std::endl;
    os << std::endl;
    os << "  " << colors::GREEN << "# إنشاء مشروع هاتف جديد" << colors::RESET << std::endl;
    os << "  ص هاتف جديد تطبيقي --اندرويد" << std::endl;
    os << "  ص هاتف جديد تطبيقي --آيفون" << std::endl;
    os << "  ص هاتف جديد تطبيقي --الكل" << std::endl;
    os << std::endl;
    os << "  " << colors::GREEN << "# بناء التطبيق" << colors::RESET << std::endl;
    os << "  ص هاتف بناء --اندرويد" << std::endl;
    os << "  ص هاتف بناء --آيفون" << std::endl;
    os << std::endl;
    os << "  " << colors::GREEN << "# تشغيل على المحاكي" << colors::RESET << std::endl;
    os << "  ص هاتف شغّل --اندرويد" << std::endl;
    os << std::endl;
    os << "  " << colors::GREEN << "# توليد كود الواجهة" << colors::RESET << std::endl;
    os << "  ص هاتف واجهة شاشتي.ص --اندرويد" << std::endl;
    os << "  ص هاتف واجهة شاشتي.ص --آيفون" << std::endl;
    os << std::endl;
    os << "  " << colors::GREEN << "# معلومات النظام" << colors::RESET << std::endl;
    os << "  ص هاتف معلومات" << std::endl;
    os << std::endl;
}

// ==============================================================================
// تنفيذ CommandManager
// ==============================================================================

CommandManager::CommandManager() {
    register_default_commands();
}

void CommandManager::register_default_commands() {
    // تسجيل جميع الأوامر الافتراضية
    register_command(std::make_shared<BuildCommand>());
    register_command(std::make_shared<RunCommand>());
    register_command(std::make_shared<TestCommand>());
    register_command(std::make_shared<FormatCommand>());  // 📝 أمر التنسيق
    register_command(std::make_shared<NewProjectCommand>()); // 📁 أمر مشروع جديد
    register_command(std::make_shared<MobileCommand>());  // 📱 أمر الهاتف
    
    auto help_cmd = std::make_shared<HelpCommand>();
    register_command(help_cmd);
    
    register_command(std::make_shared<VersionCommand>());
    
    // تعيين مرجع الأوامر لأمر المساعدة
    help_cmd->set_commands(commands_);
}

void CommandManager::register_command(std::shared_ptr<Command> cmd) {
    std::string name = cmd->get_name();
    std::string arabic_name = cmd->get_arabic_name();
    
    commands_[name] = cmd;
    arabic_to_english_[arabic_name] = name;
}

std::shared_ptr<Command> CommandManager::get_command(const std::string& name) const {
    // البحث بالاسم الإنجليزي
    auto it = commands_.find(name);
    if (it != commands_.end()) {
        return it->second;
    }
    
    // البحث بالاسم العربي
    auto ar_it = arabic_to_english_.find(name);
    if (ar_it != arabic_to_english_.end()) {
        return commands_.at(ar_it->second);
    }
    
    return nullptr;
}

int CommandManager::run(int argc, char* argv[]) {
    // التحقق من دعم الألوان وتفعيلها
    if (!colors::is_supported()) {
        colors::disable();
    }
    
    // إذا لم تُعطَ وسائط، عرض المساعدة
    if (argc < 2) {
        auto help = get_command("help");
        if (help) {
            return help->execute({});
        }
        return 1;
    }
    
    // الحصول على اسم الأمر
    std::string cmd_name = argv[1];
    
    // التعامل مع خيارات المساعدة والإصدار السريعة
    if (cmd_name == "--help" || cmd_name == "-h" || cmd_name == "--مساعدة") {
        auto help = get_command("help");
        return help ? help->execute({}) : 1;
    }
    
    if (cmd_name == "--version" || cmd_name == "-V" || cmd_name == "--نسخة") {
        auto version = get_command("version");
        return version ? version->execute({}) : 1;
    }
    
    // البحث عن الأمر
    auto cmd = get_command(cmd_name);
    if (!cmd) {
        std::cerr << colors::RED;
        std::cerr << "أمر غير معروف: " << cmd_name << std::endl;
        std::cerr << "Unknown command: " << cmd_name << std::endl;
        std::cerr << colors::RESET;
        std::cerr << std::endl;
        std::cerr << "استخدم 'ص مساعدة' لعرض الأوامر المتاحة" << std::endl;
        std::cerr << "Use 'sad help' to see available commands" << std::endl;
        return 1;
    }
    
    // تحليل الخيارات
    ParsedOptions options = parse_args(argc - 2, argv + 2, cmd.get());
    
    // التحقق من أخطاء التحليل
    if (!options.errors.empty()) {
        for (const auto& error : options.errors) {
            std::cerr << colors::RED << symbols::FAILURE << " " << error << colors::RESET << std::endl;
        }
        return 1;
    }
    
    // تنفيذ الأمر
    return cmd->execute(options);
}

ParsedOptions CommandManager::parse_args(int argc, char* argv[], const Command* cmd) const {
    ParsedOptions result;
    
    auto cmd_options = cmd->get_options();
    
    for (int i = 0; i < argc; ++i) {
        std::string arg = argv[i];
        
        // التحقق مما إذا كان خياراً
        if (arg.length() > 1 && arg[0] == '-') {
            bool found = false;
            
            for (const auto& opt : cmd_options) {
                if (arg == opt.short_name || arg == opt.long_name || arg == opt.arabic_name) {
                    found = true;
                    
                    // اسم الخيار الموحد (بدون الشرطات)
                    std::string opt_name = opt.long_name;
                    if (opt_name.length() > 2 && opt_name[0] == '-' && opt_name[1] == '-') {
                        opt_name = opt_name.substr(2);
                    }
                    
                    if (opt.requires_value) {
                        if (i + 1 < argc) {
                            result.options[opt_name] = argv[++i];
                        } else {
                            result.errors.push_back("الخيار " + arg + " يتطلب قيمة");
                        }
                    } else {
                        result.options[opt_name] = "true";
                    }
                    break;
                }
            }
            
            // التعامل مع خيارات = مثل --output=file
            if (!found && arg.find('=') != std::string::npos) {
                auto pos = arg.find('=');
                std::string opt_part = arg.substr(0, pos);
                std::string val_part = arg.substr(pos + 1);
                
                for (const auto& opt : cmd_options) {
                    if (opt_part == opt.long_name || opt_part == opt.arabic_name) {
                        std::string opt_name = opt.long_name;
                        if (opt_name.length() > 2) opt_name = opt_name.substr(2);
                        result.options[opt_name] = val_part;
                        found = true;
                        break;
                    }
                }
            }
            
            if (!found) {
                result.errors.push_back("خيار غير معروف: " + arg);
            }
        } else {
            // وسيط موضعي
            result.positional_args.push_back(arg);
        }
    }
    
    return result;
}

void CommandManager::print_banner(std::ostream& os) const {
    os << colors::CYAN << colors::BOLD;
    os << R"(
      ___           ___           ___     
     /\__\         /\  \         /\  \    
    /:/ _/_       /::\  \       /::\  \   
   /:/ /\  \     /:/\:\  \     /:/\:\  \  
  /:/ /::\  \   /:/ /::\  \   /:/  \:\__\ 
 /:/_/:/\:\__\ /:/_/:/\:\__\ /:/__/ \:|__|
 \:\/:/ /:/  / \:\/:/  \/__/ \:\  \ /:/  /
  \::/ /:/  /   \::/__/       \:\  /:/  / 
   \/_/:/  /     \:\  \        \:\/:/  /  
     /:/  /       \:\__\        \::/__/   
     \/__/         \/__/         ~~       
    )" << colors::RESET << std::endl;
}

// ==============================================================================
// (AR) التنفيذ الحقيقي للأوامر موجود في الملفات التالية:
//   - build_command.cpp → BuildCommand::execute(), get_options(), print_examples()
//   - run_command.cpp   → RunCommand::execute(), get_options(), print_examples()
//   - test_command.cpp  → TestCommand::execute(), get_options(), print_examples()
//   - formatter.cpp     → formatter_main()
//
// (EN) Real command implementations are in their respective files.
// ==============================================================================

// ==============================================================================
// FormatCommand - أمر التنسيق
// ==============================================================================

// Forward declaration of formatter_main from formatter.cpp (in ::sad::formatter namespace)

} // temporarily close cli namespace
} // temporarily close sad namespace

namespace sad { namespace formatter { int formatter_main(int argc, char* argv[]); } }

namespace sad {
namespace cli {

int FormatCommand::execute(const ParsedOptions& options) {
    // Build argv for formatter_main
    std::vector<std::string> args_storage;
    args_storage.push_back("sad-fmt");
    
    // Check for --check flag
    if (options.has("check") || options.has("تحقق")) {
        args_storage.push_back("--check");
    }
    
    // Check for --stdin flag
    if (options.has("stdin")) {
        args_storage.push_back("--stdin");
    }
    
    // Check for --config option
    if (options.has("config")) {
        args_storage.push_back("--config");
        args_storage.push_back(options.get("config"));
    }
    
    // Add positional arguments (files/directories)
    for (const auto& arg : options.positional_args) {
        args_storage.push_back(arg);
    }
    
    if (args_storage.size() < 2) {
        print_error_ar("يجب تحديد ملف أو مجلد للتنسيق");
        print_info_ar("استخدم: ص نسّق <ملف.ص> أو ص نسّق <مجلد>");
        return 1;
    }
    
    // Convert to char* argv
    std::vector<char*> c_args;
    for (auto& s : args_storage) {
        c_args.push_back(s.data());
    }
    
    return ::sad::formatter::formatter_main(static_cast<int>(c_args.size()), c_args.data());
}

std::vector<CommandOption> FormatCommand::get_options() const {
    return {
        {"", "--check",  "--تحقق",   "Check formatting without modifying", "التحقق من التنسيق بدون تعديل", false, "", true},
        {"", "--stdin",  "",          "Read from stdin",                    "القراءة من stdin",              false, "", true},
        {"", "--config", "--إعدادات", "Specify config file",               "تحديد ملف إعدادات التنسيق",    true,  "", false},
    };
}

void FormatCommand::print_examples(std::ostream& os) const {
    os << "أمثلة:\n";
    os << "  ص نسّق برنامج.ص          # تنسيق ملف\n";
    os << "  ص نسّق src/               # تنسيق مجلد\n";
    os << "  ص نسّق --تحقق برنامج.ص   # التحقق بدون تعديل\n";
    os << "\n";
    os << "Examples:\n";
    os << "  sad format program.sad     # Format a file\n";
    os << "  sad format src/            # Format a directory\n";
    os << "  sad format --check file.sad # Check without modifying\n";
}

// NewProjectCommand — أمر إنشاء مشروع جديد (stub حتى يُنقل لملف مستقل)
int NewProjectCommand::execute(const ParsedOptions& options) {
    if (options.positional_args.empty()) {
        print_error_ar("يجب تحديد اسم المشروع");
        print_info_ar("استخدم: ص جديد <اسم_المشروع>");
        return 1;
    }
    
    std::string project_name = options.positional_args[0];
    print_info_ar("📁 إنشاء مشروع جديد: " + project_name);
    
    // إنشاء هيكل المشروع
    namespace fs = std::filesystem;
    fs::path project_dir = fs::current_path() / project_name;
    
    if (fs::exists(project_dir)) {
        print_error_ar("المجلد موجود مسبقاً: " + project_name);
        return 1;
    }
    
    fs::create_directories(project_dir / "src");
    fs::create_directories(project_dir / "tests");
    
    // إنشاء الملف الرئيسي
    {
        std::ofstream main_file(project_dir / "src" / "main.sad");
        main_file << "# " << project_name << " - مشروع لغة ص\n\n"
                  << "دالة رئيسية()\n"
                  << "    اطبع_سطر(\"مرحباً من " << project_name << "!\")\n"
                  << "نهاية\n";
    }
    
    // إنشاء ملف اختبار
    {
        std::ofstream test_file(project_dir / "tests" / "test_main.sad");
        test_file << "# اختبارات " << project_name << "\n\n"
                  << "دالة رئيسية()\n"
                  << "    اطبع_سطر(\"الاختبارات...\")\n"
                  << "    اطبع_سطر(\"✅ نجح!\")\n"
                  << "نهاية\n";
    }
    
    print_success_ar("تم إنشاء المشروع: " + project_name);
    print_info_ar("  ابدأ بـ: cd " + project_name + " && ص شغّل src/main.sad");
    return 0;
}

std::vector<CommandOption> NewProjectCommand::get_options() const {
    return {
        {"", "--lib", "--مكتبة", "Create library project", "إنشاء مشروع مكتبة", false, "", true},
        {"", "--cli", "--سطر-أوامر", "Create CLI project", "إنشاء مشروع سطر أوامر", false, "", true},
    };
}

void NewProjectCommand::print_examples(std::ostream& os) const {
    os << "  ص جديد مشروعي          # إنشاء مشروع جديد\n";
    os << "  ص جديد مكتبتي --مكتبة  # إنشاء مشروع مكتبة\n";
    os << "  ص هاتف جديد تطبيقي --اندرويد  # مشروع هاتف\n";
}

// ==============================================================================
// تنفيذ الدوال المساعدة
// Utility Functions Implementation
// ==============================================================================

namespace utils {

bool file_exists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

bool directory_exists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

bool create_directory(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (...) {
        return false;
    }
}

std::optional<std::string> read_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return std::nullopt;
    
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

bool write_file(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::binary);
    if (!file) return false;
    
    file << content;
    return file.good();
}

std::string get_extension(const std::string& path) {
    auto pos = path.rfind('.');
    if (pos == std::string::npos) return "";
    return path.substr(pos);
}

std::string change_extension(const std::string& path, const std::string& new_ext) {
    auto pos = path.rfind('.');
    if (pos == std::string::npos) return path + new_ext;
    return path.substr(0, pos) + new_ext;
}

std::string get_filename(const std::string& path) {
    return std::filesystem::path(path).filename().string();
}

std::string get_current_directory() {
    return std::filesystem::current_path().string();
}

std::optional<std::string> find_project_root() {
    std::filesystem::path current = std::filesystem::current_path();
    
    while (true) {
        // البحث عن ص.toml أو sad.toml
        if (std::filesystem::exists(current / "ص.toml") ||
            std::filesystem::exists(current / "sad.toml")) {
            return current.string();
        }
        
        // الصعود للمجلد الأب
        auto parent = current.parent_path();
        if (parent == current) break;  // وصلنا للجذر
        current = parent;
    }
    
    return std::nullopt;
}

std::string to_lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> parts;
    std::istringstream ss(str);
    std::string part;
    while (std::getline(ss, part, delimiter)) {
        parts.push_back(part);
    }
    return parts;
}

std::string join(const std::vector<std::string>& parts, const std::string& delimiter) {
    std::string result;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) result += delimiter;
        result += parts[i];
    }
    return result;
}

std::optional<std::filesystem::file_time_type> get_modification_time(const std::string& path) {
    try {
        return std::filesystem::last_write_time(path);
    } catch (...) {
        return std::nullopt;
    }
}

} // namespace utils

} // namespace cli
} // namespace sad

// ==============================================================================
// نقطة الدخول الرئيسية
// Main Entry Point
// ==============================================================================

#ifdef SAD_CLI_MAIN

int main(int argc, char* argv[]) {
#ifdef _WIN32
    // تفعيل دعم Unicode على ويندوز
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // تفعيل وضع ANSI للألوان
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
    
    // تعيين الموقع للدعم العربي
    std::setlocale(LC_ALL, "");
    
    // إنشاء مدير الأوامر وتشغيله
    sad::cli::CommandManager manager;
    return manager.run(argc, argv);
}

#endif // SAD_CLI_MAIN
