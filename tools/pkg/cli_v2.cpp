// بسم الله الرحمن الرحيم
/**
 * @file cli_v2.cpp
 * @brief sad-pkg CLI v2 - أداة سطر الأوامر لمدير حزم لغة ص (الإصدار ٢)
 *
 * أداة احترافية تعمل بدون تبعيات خارجية (بدون libcurl / nlohmann/json)
 * تدعم ملفات ص.toml العربية و sad.toml الإنجليزية
 *
 * الأوامر الكاملة / Full Commands:
 *
 *   ─── إدارة المشروع ───
 *   sad-pkg init                     - إنشاء حزمة جديدة / Initialize
 *   sad-pkg install [pkg]            - تثبيت التبعيات / Install
 *   sad-pkg add <pkg> [version]      - إضافة تبعية / Add dependency
 *   sad-pkg remove <pkg>             - إزالة تبعية / Remove dependency
 *   sad-pkg update                   - تحديث التبعيات / Update
 *   sad-pkg outdated                 - حزم قديمة / Show outdated
 *   sad-pkg audit                    - تدقيق التبعيات / Audit
 *   sad-pkg verify                   - التحقق من السلامة / Verify
 *
 *   ─── النشر والمستودع ───
 *   sad-pkg publish                  - نشر الحزمة / Publish
 *   sad-pkg publish --local          - نشر محلي / Publish locally
 *   sad-pkg search <query>           - البحث / Search
 *   sad-pkg info <pkg>               - معلومات حزمة / Package info
 *   sad-pkg versions <pkg>           - عرض الإصدارات / Show versions
 *   sad-pkg list                     - المثبتة / List installed
 *   sad-pkg yank <pkg> <version>     - سحب إصدار / Yank version
 *   sad-pkg stats                    - إحصائيات المستودع / Stats
 *
 *   ─── الحساب والمصادقة ───
 *   sad-pkg register                 - تسجيل حساب جديد / Register
 *   sad-pkg login                    - تسجيل الدخول / Login
 *   sad-pkg logout                   - تسجيل الخروج / Logout
 *   sad-pkg whoami                   - من أنا / Who am I
 *   sad-pkg my-packages              - حزمي / My packages
 *   sad-pkg token <name>             - إنشاء مفتاح API / Create token
 *
 *   ─── الأدوات والإعدادات ───
 *   sad-pkg config <key> [value]     - إعدادات / Config
 *   sad-pkg doctor                   - تشخيص المشاكل / Diagnose
 *   sad-pkg clean                    - تنظيف الكاش / Clean cache
 *   sad-pkg help                     - المساعدة / Help
 *   sad-pkg --version                - رقم الإصدار / Version
 *
 * @author فريق لغة ص
 * @date فبراير 2026
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "package.h"
#include "toml_parser.h"
#include "dependency_resolver.h"
#include "registry_client_v2.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstring>

using namespace sad::pkg;

// ============================================================================
// Version Info - معلومات الإصدار
// ============================================================================

static const char* SAD_PKG_VERSION = "2.0.0";

// ============================================================================
// Console Colors - ألوان وحدة التحكم
// ============================================================================

#ifdef _WIN32
#include <windows.h>
static void init_console() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    // دعم UTF-8
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
}
#else
static void init_console() {}
#endif

static const char* RESET   = "\033[0m";
static const char* BOLD    = "\033[1m";
static const char* DIM     = "\033[2m";
static const char* RED     = "\033[31m";
static const char* GREEN   = "\033[32m";
static const char* YELLOW  = "\033[33m";
static const char* BLUE    = "\033[34m";
static const char* MAGENTA = "\033[35m";
static const char* CYAN    = "\033[36m";
static const char* WHITE   = "\033[37m";

static void print_header(const std::string& text) {
    std::cout << BOLD << CYAN << text << RESET << "\n";
}
static void print_success(const std::string& text) {
    std::cout << GREEN << "\xE2\x9C\x93 " << text << RESET << "\n";
}
static void print_error(const std::string& text) {
    std::cerr << RED << "\xE2\x9C\x97 " << text << RESET << "\n";
}
static void print_warning(const std::string& text) {
    std::cout << YELLOW << "\xE2\x9A\xA0 " << text << RESET << "\n";
}
static void print_info(const std::string& text) {
    std::cout << BLUE << "\xE2\x84\xB9 " << text << RESET << "\n";
}
static void print_dim(const std::string& text) {
    std::cout << DIM << text << RESET << "\n";
}

/**
 * @brief رسم شريط فاصل
 */
static void print_separator() {
    std::cout << DIM << u8"  ─────────────────────────────────────────────────" << RESET << "\n";
}

/**
 * @brief رسم صف جدول
 */
static void print_table_row(const std::string& key, const std::string& value,
                             const char* key_color = nullptr) {
    const char* kc = key_color ? key_color : BOLD;
    std::cout << "  " << kc << std::left << std::setw(22) << key << RESET << value << "\n";
}

/**
 * @brief قراءة كلمة مرور مع إخفاء الإدخال
 */
static std::string read_password(const std::string& prompt) {
    std::cout << prompt;
#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & ~ENABLE_ECHO_INPUT);
    std::string password;
    std::getline(std::cin, password);
    SetConsoleMode(hStdin, mode);
    std::cout << "\n";
    return password;
#else
    std::string password;
    std::getline(std::cin, password);
    return password;
#endif
}

// ============================================================================
// Config File Detection - اكتشاف ملف الإعداد
// ============================================================================

/**
 * @brief إنشاء مسار filesystem من نص UTF-8
 */
static std::filesystem::path utf8_path(const std::string& s) {
#if defined(_MSC_VER) || (defined(_WIN32) && !defined(__GNUC__))
    return std::filesystem::u8path(s);
#else
    return std::filesystem::path(s);
#endif
}

// اسم ملف الإعداد العربي
static const std::string ARABIC_CONFIG = u8"\u0635.toml";

/**
 * @brief اكتشاف ملف الإعداد (يدعم العربي والإنجليزي)
 */
static std::string find_config_file() {
    if (std::filesystem::exists("sad.toml")) return "sad.toml";
    if (std::filesystem::exists(utf8_path(ARABIC_CONFIG))) return ARABIC_CONFIG;
    return "";
}

/**
 * @brief تحميل الحزمة من ملف الإعداد
 */
static Package load_package_from_config() {
    std::string config = find_config_file();
    if (config.empty()) {
        throw std::runtime_error(
            u8"لم يُعثر على ملف الإعداد. استخدم 'sad-pkg init' أولاً\n"
            "Config file not found. Run 'sad-pkg init' first."
        );
    }
    return Package::from_file(utf8_path(config));
}

/**
 * @brief حفظ ملف الإعداد مع التعامل مع الترميز
 */
static void save_package_config(const Package& pkg) {
    std::string config = find_config_file();
    if (config.empty()) config = "sad.toml";
    pkg.save_to_file(utf8_path(config));
}

// ============================================================================
// ═══════════════════════════════════════════════════════════════════════
//                           أوامر المساعدة
// ═══════════════════════════════════════════════════════════════════════
// ============================================================================

/**
 * @brief عرض المساعدة الشاملة
 */
static void print_usage() {
    std::cout << "\n";
    std::cout << BOLD << CYAN
              << u8"  ╭──────────────────────────────────────────────╮\n"
              << u8"  │       sad-pkg  مدير حزم لغة ص v" << SAD_PKG_VERSION << u8"        │\n"
              << u8"  ╰──────────────────────────────────────────────╯\n"
              << RESET << "\n";

    std::cout << BOLD << u8"  الاستخدام / USAGE:" << RESET << "\n";
    std::cout << "    sad-pkg <" << CYAN << u8"أمر" << RESET << "> [خيارات]\n\n";

    // إدارة المشروع
    std::cout << BOLD << MAGENTA << u8"  إدارة المشروع:" << RESET << "\n";
    std::cout << "    " << CYAN << "init" << RESET
              << u8"                     إنشاء حزمة جديدة\n";
    std::cout << "    " << CYAN << "install" << RESET << " [pkg]"
              << u8"           تثبيت التبعيات\n";
    std::cout << "    " << CYAN << "add" << RESET << " <pkg> [ver]"
              << u8"         إضافة تبعية\n";
    std::cout << "    " << CYAN << "remove" << RESET << " <pkg>"
              << u8"             إزالة تبعية\n";
    std::cout << "    " << CYAN << "update" << RESET
              << u8"                   تحديث الكل\n";
    std::cout << "    " << CYAN << "outdated" << RESET
              << u8"                 حزم قديمة\n";
    std::cout << "    " << CYAN << "audit" << RESET
              << u8"                    تدقيق التبعيات\n";
    std::cout << "    " << CYAN << "verify" << RESET
              << u8"                   التحقق من السلامة\n";
    std::cout << "\n";

    // النشر والمستودع
    std::cout << BOLD << MAGENTA << u8"  النشر والمستودع:" << RESET << "\n";
    std::cout << "    " << CYAN << "publish" << RESET
              << u8"                  نشر الحزمة\n";
    std::cout << "    " << CYAN << "publish --local" << RESET
              << u8"          نشر محلي\n";
    std::cout << "    " << CYAN << "search" << RESET << " <query>"
              << u8"           بحث\n";
    std::cout << "    " << CYAN << "info" << RESET << " <pkg>"
              << u8"               معلومات حزمة\n";
    std::cout << "    " << CYAN << "versions" << RESET << " <pkg>"
              << u8"           عرض الإصدارات\n";
    std::cout << "    " << CYAN << "list" << RESET
              << u8"                     المثبتة\n";
    std::cout << "    " << CYAN << "yank" << RESET << " <pkg> <ver>"
              << u8"         سحب إصدار\n";
    std::cout << "    " << CYAN << "stats" << RESET
              << u8"                    إحصائيات المستودع\n";
    std::cout << "\n";

    // الحساب والمصادقة
    std::cout << BOLD << MAGENTA << u8"  الحساب والمصادقة:" << RESET << "\n";
    std::cout << "    " << CYAN << "register" << RESET
              << u8"                 تسجيل حساب جديد\n";
    std::cout << "    " << CYAN << "login" << RESET
              << u8"                    تسجيل دخول\n";
    std::cout << "    " << CYAN << "logout" << RESET
              << u8"                   تسجيل خروج\n";
    std::cout << "    " << CYAN << "whoami" << RESET
              << u8"                   من أنا\n";
    std::cout << "    " << CYAN << "my-packages" << RESET
              << u8"              حزمي\n";
    std::cout << "    " << CYAN << "token" << RESET << " <name>"
              << u8"             إنشاء مفتاح API\n";
    std::cout << "\n";

    // الأدوات والإعدادات
    std::cout << BOLD << MAGENTA << u8"  الأدوات والإعدادات:" << RESET << "\n";
    std::cout << "    " << CYAN << "config" << RESET << " <key> [value]"
              << u8"    ضبط/عرض إعداد\n";
    std::cout << "    " << CYAN << "doctor" << RESET
              << u8"                   تشخيص المشاكل\n";
    std::cout << "    " << CYAN << "clean" << RESET
              << u8"                    تنظيف الكاش\n";
    std::cout << "    " << CYAN << "help" << RESET
              << u8"                     المساعدة\n";
    std::cout << "    " << CYAN << "--version" << RESET
              << u8"                رقم الإصدار\n";

    std::cout << "\n" << BOLD << u8"  أمثلة / EXAMPLES:" << RESET << "\n";
    std::cout << DIM;
    std::cout << "    sad-pkg init\n";
    std::cout << "    sad-pkg register\n";
    std::cout << "    sad-pkg login\n";
    std::cout << "    sad-pkg add http ^1.0.0\n";
    std::cout << "    sad-pkg install\n";
    std::cout << "    sad-pkg search http\n";
    std::cout << "    sad-pkg publish\n";
    std::cout << "    sad-pkg stats\n";
    std::cout << "    sad-pkg config registry https://my-registry.com\n";
    std::cout << RESET << "\n";

    std::cout << BOLD << u8"  الأوامر العربية:" << RESET << "\n";
    std::cout << DIM;
    std::cout << u8"    أنشئ، ثبّت، أضف، أزل، حدّث، انشر، ابحث، معلومات\n";
    std::cout << u8"    سجّل، دخول، اخرج، من_أنا، حزمي، مفتاح، إصدارات\n";
    std::cout << u8"    سحب، إحصائيات، إعدادات، تدقيق، قديمة، تحقق، طبيب\n";
    std::cout << u8"    قائمة، نظّف، مساعدة\n";
    std::cout << RESET << "\n";
}

// ============================================================================
// ═══════════════════════════════════════════════════════════════════════
//                        أوامر إدارة المشروع
// ═══════════════════════════════════════════════════════════════════════
// ============================================================================

/**
 * @brief أمر: init - إنشاء حزمة جديدة
 */
static int cmd_init() {
    print_header(u8"إنشاء حزمة جديدة... / Initializing new package...");

    if (!find_config_file().empty()) {
        print_error(u8"ملف الإعداد موجود بالفعل / Config file already exists");
        return 1;
    }

    std::string name, version_str, description, author, license;

    std::cout << "\n";
    std::cout << u8"  اسم الحزمة / Package name " << DIM << "(my_package)" << RESET << ": ";
    std::getline(std::cin, name);
    if (name.empty()) name = "my_package";

    std::cout << u8"  الإصدار / Version " << DIM << "(0.1.0)" << RESET << ": ";
    std::getline(std::cin, version_str);
    if (version_str.empty()) version_str = "0.1.0";

    std::cout << u8"  الوصف / Description: ";
    std::getline(std::cin, description);

    std::cout << u8"  المؤلف / Author: ";
    std::getline(std::cin, author);

    std::cout << u8"  الرخصة / License " << DIM << "(MIT)" << RESET << ": ";
    std::getline(std::cin, license);
    if (license.empty()) license = "MIT";

    // إنشاء الحزمة
    Package pkg;
    pkg.name = name;
    pkg.version = Version::parse(version_str);
    pkg.description = description;
    pkg.license = license;
    if (!author.empty()) pkg.authors.push_back(author);
    pkg.main_file = u8"src/main.ص";
    pkg.source_dirs.push_back("src");

    pkg.save_to_file("sad.toml");

    // إنشاء هيكل المجلدات
    std::filesystem::create_directories("src");
    std::filesystem::create_directories("tests");

    // إنشاء الملف الرئيسي
    auto main_path = utf8_path(u8"src/main.ص");
    std::ofstream main_file(main_path);
    if (main_file.is_open()) {
        main_file << u8"# بسم الله الرحمن الرحيم\n";
        main_file << u8"# " << name << u8" - نقطة الدخول الرئيسية\n\n";
        main_file << u8"دالة رئيسية()\n";
        main_file << u8"    اطبع_سطر(\"مرحباً من " << name << u8"!\")\n";
        main_file << u8"نهاية\n";
        main_file.close();
    }

    // إنشاء .gitignore
    std::ofstream gitignore(".gitignore");
    if (gitignore.is_open()) {
        gitignore << "deps/\nbuild/\n*.o\n*.exe\nsad.lock\n";
        gitignore.close();
    }

    std::cout << "\n";
    print_success(u8"تم إنشاء الحزمة بنجاح! / Package initialized!");
    print_info(u8"  أُنشئ: sad.toml و src/main.ص و .gitignore");
    std::cout << "\n";
    print_dim(u8"  الخطوات التالية:");
    print_dim(u8"    1. sad-pkg add <حزمة>    - إضافة تبعيات");
    print_dim(u8"    2. sad-pkg install       - تثبيت التبعيات");
    print_dim(u8"    3. sad-pkg publish       - نشر الحزمة");

    return 0;
}

/**
 * @brief أمر: install - تثبيت التبعيات
 */
static int cmd_install(const std::string& package_name = "") {
    Package pkg;
    try {
        pkg = load_package_from_config();
    } catch (const std::exception& e) {
        print_error(e.what());
        return 1;
    }

    // إذا حُددت حزمة معينة، أضفها مؤقتاً (يُحفظ بعد نجاح التثبيت)
    std::string install_pkg_name;
    if (!package_name.empty()) {
        install_pkg_name = package_name;
        std::string name = package_name;
        std::string version_req = "*";
        size_t space = package_name.find(' ');
        if (space != std::string::npos) {
            name = package_name.substr(0, space);
            version_req = package_name.substr(space + 1);
        }

        pkg.dependencies.emplace_back(name, VersionRequirement::parse(version_req));
        print_info(u8"إضافة " + name + " " + version_req + u8" إلى التبعيات...");
    }

    print_header(u8"تثبيت التبعيات... / Installing dependencies...");

    RegistryClientV2 registry;
    DependencyResolver resolver(registry);

    // حل التبعيات
    std::vector<ResolvedDependency> resolved;
    try {
        resolved = resolver.resolve(pkg, false);
    } catch (const std::exception& e) {
        print_error(u8"فشل حل التبعيات: " + std::string(e.what()));
        if (resolver.has_conflicts()) {
            std::cout << "\n" << BOLD << RED << u8"  التعارضات:" << RESET << "\n";
            for (const auto& c : resolver.get_conflicts())
                std::cout << "    " << YELLOW << c.to_string() << RESET << "\n";
        }
        return 1;
    }

    if (resolved.empty()) {
        print_success(u8"لا توجد تبعيات للتثبيت / No dependencies to install");
        return 0;
    }

    // تثبيت كل تبعية
    std::cout << "\n" << BOLD << u8"  تثبيت " << resolved.size()
              << u8" حزمة:" << RESET << "\n";

    int success_count = 0;
    for (const auto& dep : resolved) {
        std::cout << "    " << CYAN << dep.name << RESET << " "
                  << DIM << dep.version.to_string() << RESET << "\n";
        if (registry.download_and_install(dep.name, dep.version, "deps")) {
            success_count++;
        } else {
            print_warning(u8"    فشل تثبيت: " + dep.name);
        }
    }

    // إنشاء ملف القفل
    std::ofstream lockfile("sad.lock");
    if (lockfile.is_open()) {
        lockfile << u8"# ملف القفل - لا تعدله يدوياً / Lock file - do not edit\n";
        lockfile << u8"# تم التوليد بواسطة sad-pkg v" << SAD_PKG_VERSION << "\n\n";
        for (const auto& dep : resolved) {
            lockfile << dep.name << " = \"" << dep.version.to_string() << "\"\n";
        }
        lockfile.close();
    }

    std::cout << "\n";
    if (success_count == static_cast<int>(resolved.size())) {
        print_success(u8"تم تثبيت " + std::to_string(success_count) + u8" حزمة بنجاح!");
        // حفظ الإعداد فقط بعد نجاح التثبيت
        if (!install_pkg_name.empty()) {
            save_package_config(pkg);
        }
    } else {
        print_warning(u8"تم تثبيت " + std::to_string(success_count) + u8" / "
                      + std::to_string(resolved.size()) + u8" حزمة");
    }
    return (success_count == static_cast<int>(resolved.size())) ? 0 : 1;
}

/**
 * @brief أمر: add - إضافة تبعية
 */
static int cmd_add(const std::string& package_spec) {
    if (package_spec.empty()) {
        print_error(u8"اسم الحزمة مطلوب / Package name required");
        std::cout << DIM << "  sad-pkg add <pkg> [version]" << RESET << "\n";
        return 1;
    }

    Package pkg;
    try {
        pkg = load_package_from_config();
    } catch (const std::exception& e) {
        print_error(e.what());
        return 1;
    }

    std::string name = package_spec;
    std::string version_req = "*";
    size_t space = package_spec.find(' ');
    if (space != std::string::npos) {
        name = package_spec.substr(0, space);
        version_req = package_spec.substr(space + 1);
    }

    // التحقق من الوجود المسبق
    bool found = false;
    for (auto& dep : pkg.dependencies) {
        if (dep.name == name) {
            dep.requirement = VersionRequirement::parse(version_req);
            found = true;
            print_warning(u8"تم تحديث إصدار '" + name + u8"' إلى " + version_req);
            break;
        }
    }

    if (!found) {
        pkg.dependencies.emplace_back(name, VersionRequirement::parse(version_req));
    }

    save_package_config(pkg);
    print_success(u8"أُضيف " + name + " " + version_req + u8" إلى التبعيات");
    print_info(u8"استخدم 'sad-pkg install' للتثبيت");

    return 0;
}

/**
 * @brief أمر: remove - إزالة تبعية
 */
static int cmd_remove(const std::string& package_name) {
    if (package_name.empty()) {
        print_error(u8"اسم الحزمة مطلوب / Package name required");
        return 1;
    }

    Package pkg;
    try {
        pkg = load_package_from_config();
    } catch (const std::exception& e) {
        print_error(e.what());
        return 1;
    }

    auto it = std::remove_if(pkg.dependencies.begin(), pkg.dependencies.end(),
                              [&](const Dependency& d) { return d.name == package_name; });

    if (it == pkg.dependencies.end()) {
        print_error(u8"الحزمة غير موجودة في التبعيات: " + package_name);
        return 1;
    }

    pkg.dependencies.erase(it, pkg.dependencies.end());
    save_package_config(pkg);

    // حذف المجلد
    auto pkg_dir = std::filesystem::path("deps") / package_name;
    if (std::filesystem::exists(pkg_dir))
        std::filesystem::remove_all(pkg_dir);

    print_success(u8"تمت إزالة " + package_name);
    return 0;
}

/**
 * @brief أمر: update - تحديث التبعيات
 */
static int cmd_update() {
    print_header(u8"تحديث التبعيات... / Updating dependencies...");

    // حذف ملف القفل لإعادة حل التبعيات
    for (auto lock_name : {u8"\u0635.\u0642\u0641\u0644", "sad.lock"}) {
        auto lock_path = utf8_path(lock_name);
        if (std::filesystem::exists(lock_path))
            std::filesystem::remove(lock_path);
    }

    return cmd_install();
}

/**
 * @brief أمر: outdated - عرض الحزم التي لها تحديثات
 */
static int cmd_outdated() {
    print_header(u8"الحزم القديمة / Outdated Packages");

    Package pkg;
    try {
        pkg = load_package_from_config();
    } catch (const std::exception& e) {
        print_error(e.what());
        return 1;
    }

    if (pkg.dependencies.empty()) {
        print_info(u8"لا توجد تبعيات");
        return 0;
    }

    RegistryClientV2 registry;

    std::cout << "\n";
    std::cout << "  " << BOLD << std::left
              << std::setw(20) << u8"الحزمة"
              << std::setw(12) << u8"المطلوب"
              << std::setw(12) << u8"المثبت"
              << u8"الأحدث" << RESET << "\n";
    print_separator();

    int outdated_count = 0;

    for (const auto& dep : pkg.dependencies) {
        auto versions = registry.get_available_versions(dep.name);
        if (versions.empty()) continue;

        // الإصدار المثبت
        std::string installed_ver = "?";
        auto dep_config = std::filesystem::path("deps") / dep.name / "sad.toml";
        auto dep_config_ar = std::filesystem::path("deps") / dep.name / u8"\u0635.toml";
        if (std::filesystem::exists(dep_config)) {
            try { installed_ver = Package::from_file(dep_config).version.to_string(); }
            catch (...) {}
        } else if (std::filesystem::exists(dep_config_ar)) {
            try { installed_ver = Package::from_file(dep_config_ar).version.to_string(); }
            catch (...) {}
        }

        std::string latest_ver = versions[0].to_string();
        bool is_outdated = (installed_ver != "?" && installed_ver != latest_ver);

        std::cout << "  " << CYAN << std::left << std::setw(20) << dep.name << RESET
                  << std::setw(12) << dep.requirement.to_string()
                  << std::setw(12) << installed_ver;

        if (is_outdated) {
            std::cout << YELLOW << latest_ver << RESET;
            outdated_count++;
        } else if (installed_ver == latest_ver) {
            std::cout << GREEN << latest_ver << RESET;
        } else {
            std::cout << latest_ver;
        }
        std::cout << "\n";
    }

    std::cout << "\n";
    if (outdated_count > 0) {
        print_warning(std::to_string(outdated_count) + u8" حزمة تحتاج تحديث");
        print_info(u8"استخدم 'sad-pkg update' لتحديثها");
    } else {
        print_success(u8"جميع الحزم محدّثة!");
    }
    std::cout << "\n";

    return 0;
}

/**
 * @brief أمر: audit - تدقيق التبعيات
 */
static int cmd_audit() {
    print_header(u8"تدقيق التبعيات / Dependency Audit");

    Package pkg;
    try {
        pkg = load_package_from_config();
    } catch (const std::exception& e) {
        print_error(e.what());
        return 1;
    }

    if (pkg.dependencies.empty()) {
        print_success(u8"لا توجد تبعيات للتدقيق");
        return 0;
    }

    std::cout << "\n";
    print_info(u8"فحص " + std::to_string(pkg.dependencies.size()) + u8" تبعية...");
    std::cout << "\n";

    RegistryClientV2 registry;
    DependencyResolver resolver(registry);

    int issues = 0;
    int warnings = 0;

    for (const auto& dep : pkg.dependencies) {
        std::cout << "  " << CYAN << dep.name << RESET
                  << " " << DIM << dep.requirement.to_string() << RESET << " ";

        auto versions = registry.get_available_versions(dep.name);
        if (versions.empty()) {
            std::cout << RED << u8"  غير موجودة في المستودع" << RESET << "\n";
            issues++;
            continue;
        }

        bool found_match = false;
        for (const auto& v : versions) {
            if (dep.requirement.matches(v)) {
                found_match = true;
                break;
            }
        }

        if (!found_match) {
            std::cout << YELLOW << u8"  لا يوجد إصدار متوافق" << RESET << "\n";
            warnings++;
        } else {
            std::cout << GREEN << u8"  متوفرة" << RESET << "\n";
        }
    }

    // التحقق من تعارضات التبعيات
    std::cout << "\n";
    print_info(u8"فحص تعارضات التبعيات...");
    try {
        resolver.resolve(pkg, false);
        print_success(u8"لا توجد تعارضات في التبعيات");
    } catch (const std::exception& e) {
        print_error(u8"تعارض: " + std::string(e.what()));
        issues++;
        if (resolver.has_conflicts()) {
            for (const auto& c : resolver.get_conflicts())
                std::cout << "    " << YELLOW << c.to_string() << RESET << "\n";
        }
    }

    // التحقق من ملف القفل
    bool has_lockfile = std::filesystem::exists("sad.lock") ||
                        std::filesystem::exists(utf8_path(u8"\u0635.\u0642\u0641\u0644"));
    if (!has_lockfile) {
        print_warning(u8"لا يوجد ملف قفل (sad.lock) - شغّل 'sad-pkg install' لإنشائه");
        warnings++;
    }

    // النتيجة
    std::cout << "\n";
    print_separator();
    if (issues == 0 && warnings == 0) {
        print_success(u8"لم يُعثر على مشاكل!");
    } else {
        if (issues > 0) print_error(std::to_string(issues) + u8" مشكلة");
        if (warnings > 0) print_warning(std::to_string(warnings) + u8" تحذير");
    }
    std::cout << "\n";

    return issues > 0 ? 1 : 0;
}

/**
 * @brief أمر: verify - التحقق من سلامة الحزم المثبتة
 */
static int cmd_verify() {
    print_header(u8"التحقق من سلامة الحزم / Verify Integrity");

    if (!std::filesystem::exists("deps")) {
        print_info(u8"لا توجد حزم مثبتة");
        return 0;
    }

    Package pkg;
    try {
        pkg = load_package_from_config();
    } catch (const std::exception& e) {
        print_error(e.what());
        return 1;
    }

    std::cout << "\n";
    int ok_count = 0, issue_count = 0;

    for (const auto& dep : pkg.dependencies) {
        auto dep_dir = std::filesystem::path("deps") / dep.name;
        std::cout << "  " << CYAN << dep.name << RESET << " ";

        if (!std::filesystem::exists(dep_dir)) {
            std::cout << RED << u8"  غير مثبتة" << RESET << "\n";
            issue_count++;
            continue;
        }

        bool has_config = std::filesystem::exists(dep_dir / "sad.toml") ||
                          std::filesystem::exists(dep_dir / u8"\u0635.toml");
        if (!has_config) {
            std::cout << YELLOW << u8"  بدون ملف إعداد" << RESET << "\n";
            issue_count++;
            continue;
        }

        int file_count = 0;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dep_dir)) {
            if (entry.is_regular_file()) file_count++;
        }

        if (file_count == 0) {
            std::cout << RED << u8"  مجلد فارغ" << RESET << "\n";
            issue_count++;
        } else {
            std::cout << GREEN << u8"  سليمة " << RESET << DIM
                      << "(" << file_count << u8" ملف)" << RESET << "\n";
            ok_count++;
        }
    }

    // ملف القفل
    std::cout << "\n  " << BOLD << u8"ملف القفل:" << RESET << " ";
    if (std::filesystem::exists("sad.lock")) {
        std::cout << GREEN << u8"موجود" << RESET << "\n";
    } else {
        std::cout << YELLOW << u8"غير موجود" << RESET << "\n";
        issue_count++;
    }

    std::cout << "\n";
    if (issue_count == 0) {
        print_success(u8"جميع الحزم سليمة!");
    } else {
        print_warning(std::to_string(issue_count) + u8" مشكلة مكتشفة");
        print_info(u8"استخدم 'sad-pkg install' لإصلاح المشاكل");
    }
    std::cout << "\n";

    return issue_count > 0 ? 1 : 0;
}

// ============================================================================
// ═══════════════════════════════════════════════════════════════════════
//                       أوامر النشر والمستودع
// ═══════════════════════════════════════════════════════════════════════
// ============================================================================

/**
 * @brief الحصول على مسار مخزن الحزم المحلي
 */
static std::filesystem::path get_local_packages_dir() {
#ifdef _WIN32
    const char* appdata = std::getenv("LOCALAPPDATA");
    if (appdata) return std::filesystem::path(appdata) / "sad" / "packages";
    return "C:\\ProgramData\\sad\\packages";
#else
    const char* home = std::getenv("HOME");
    if (home) return std::filesystem::path(home) / ".sad" / "packages";
    return "/usr/local/share/sad/packages";
#endif
}

/**
 * @brief أمر: publish --local (نشر محلي)
 */
static int cmd_publish_local() {
    print_header(u8"نشر محلي... / Publishing locally...");

    Package pkg;
    try {
        pkg = load_package_from_config();
    } catch (const std::exception& e) {
        print_error(e.what());
        return 1;
    }

    print_info(u8"الحزمة: " + pkg.name + " v" + pkg.version.to_string());

    if (pkg.name.empty()) {
        print_error(u8"اسم الحزمة مطلوب");
        return 1;
    }

    auto packages_dir = get_local_packages_dir();
    auto pkg_name_path = utf8_path(pkg.name);
    auto pkg_dest = packages_dir / pkg_name_path / pkg.version.to_string();

    try {
        std::filesystem::create_directories(pkg_dest);

        auto current = std::filesystem::current_path();
        int file_count = 0;
        size_t total_size = 0;

        std::error_code ec;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(current, ec)) {
            if (!entry.is_regular_file()) continue;

            auto rel = std::filesystem::relative(entry.path(), current, ec);
            if (ec) continue;

            std::wstring rel_wstr = rel.wstring();

            // تخطي مجلدات غير مطلوبة
            if (rel_wstr.find(L"tests") == 0 || rel_wstr.find(L"test") == 0 ||
                rel_wstr.find(L".git") == 0 || rel_wstr.find(L"build") == 0 ||
                rel_wstr.find(L"examples") == 0 || rel_wstr.find(L"deps") == 0 ||
                rel_wstr.find(L"node_modules") == 0) {
                continue;
            }

            auto dest_file = pkg_dest / rel;
            std::filesystem::create_directories(dest_file.parent_path(), ec);
            std::filesystem::copy_file(entry.path(), dest_file,
                std::filesystem::copy_options::overwrite_existing, ec);

            if (!ec) {
                file_count++;
                total_size += entry.file_size();
            }
        }

        // إنشاء ملف metadata
        auto meta_path = pkg_dest / ".sad-pkg-meta.json";
        std::ofstream meta(meta_path);
        if (!meta.is_open()) {
            print_warning(u8"تعذر كتابة ملف البيانات الوصفية");
        } else {
            // هروب النصوص لتجنب JSON غير صالح
            auto json_esc = [](const std::string& s) {
                std::string r;
                for (char c : s) {
                    switch (c) {
                        case '"': r += "\\\""; break;
                        case '\\': r += "\\\\"; break;
                        case '\n': r += "\\n"; break;
                        case '\r': r += "\\r"; break;
                        case '\t': r += "\\t"; break;
                        default: r += c;
                    }
                }
                return r;
            };
            meta << "{\n";
            meta << "  \"name\": \"" << json_esc(pkg.name) << "\",\n";
            meta << "  \"version\": \"" << pkg.version.to_string() << "\",\n";
            meta << "  \"description\": \"" << json_esc(pkg.description) << "\",\n";
            meta << "  \"files\": " << file_count << ",\n";
            meta << "  \"size\": " << total_size << ",\n";
            meta << "  \"published_locally\": true\n";
            meta << "}\n";
            meta.close();
        }

        double size_kb = total_size / 1024.0;
        std::cout << "\n";
        print_success(u8"تم النشر المحلي بنجاح! / Published locally!");

        std::string dest_display;
        try { dest_display = pkg_dest.u8string(); }
        catch (...) { dest_display = "(unicode path)"; }
        print_info(u8"  المسار: " + dest_display);
        std::cout << "  " << BLUE << u8"  الملفات: " << file_count
                  << u8" ملف (" << std::fixed << std::setprecision(1)
                  << size_kb << " KB)" << RESET << "\n";
        print_info(u8"  للتثبيت في مشروع آخر:");
        std::cout << DIM << "    sad-pkg add " << pkg.name << " " << pkg.version.to_string() << "\n";
        std::cout << "    sad-pkg install\n" << RESET;

    } catch (const std::exception& e) {
        print_error(u8"فشل النشر المحلي: " + std::string(e.what()));
        return 1;
    }

    return 0;
}

/**
 * @brief أمر: publish - نشر الحزمة
 */
static int cmd_publish(bool local_mode = false) {
    if (local_mode) return cmd_publish_local();

    print_header(u8"نشر الحزمة... / Publishing package...");

    Package pkg;
    try {
        pkg = load_package_from_config();
    } catch (const std::exception& e) {
        print_error(e.what());
        return 1;
    }

    print_info(u8"الحزمة: " + pkg.name + " v" + pkg.version.to_string());

    RegistryClientV2 registry;

    if (!registry.is_logged_in()) {
        print_error(u8"يجب تسجيل الدخول أولاً");
        std::cout << "\n";
        print_info(u8"  سجّل دخول:      sad-pkg login");
        print_info(u8"  أو أنشئ حساب:   sad-pkg register");
        print_info(u8"  أو انشر محلياً:  sad-pkg publish --local");
        return 1;
    }

    if (registry.publish(pkg)) {
        std::cout << "\n";
        print_success(u8"تم نشر " + pkg.name + " v" + pkg.version.to_string() + u8" بنجاح!");
        return 0;
    }

    print_error(u8"فشل نشر الحزمة. تحقق من اتصالك بالمستودع وبيانات الحزمة.");
    return 1;
}

/**
 * @brief أمر: search - البحث عن حزم
 */
static int cmd_search(const std::string& query) {
    if (query.empty()) {
        print_error(u8"نص البحث مطلوب / Search query required");
        return 1;
    }

    print_header(u8"البحث عن: " + query + u8" ...");

    RegistryClientV2 registry;
    auto results = registry.search(query);

    if (results.empty()) {
        print_info(u8"لم يُعثر على نتائج / No results found");
        return 0;
    }

    std::cout << "\n" << BOLD << u8"  نتائج البحث (" << results.size()
              << u8"):" << RESET << "\n\n";

    for (const auto& name : results) {
        std::cout << "    " << CYAN << name << RESET << "\n";
    }
    std::cout << "\n";

    return 0;
}

/**
 * @brief أمر: info - معلومات حزمة
 */
static int cmd_info(const std::string& package_name) {
    if (package_name.empty()) {
        print_error(u8"اسم الحزمة مطلوب / Package name required");
        return 1;
    }

    print_header(u8"معلومات الحزمة / Package Information");

    RegistryClientV2 registry;

    auto versions = registry.get_available_versions(package_name);
    if (versions.empty()) {
        print_error(u8"الحزمة غير موجودة: " + package_name);
        return 1;
    }

    auto pkg = registry.get_package_metadata(package_name, versions[0]);

    std::cout << "\n";
    print_table_row(u8"الاسم / Name:", pkg.name);
    print_table_row(u8"الإصدار / Version:", pkg.version.to_string());
    print_table_row(u8"الوصف / Description:", pkg.description);

    if (!pkg.authors.empty()) {
        std::string authors_str;
        for (size_t i = 0; i < pkg.authors.size(); i++) {
            authors_str += pkg.authors[i];
            if (i + 1 < pkg.authors.size()) authors_str += ", ";
        }
        print_table_row(u8"المؤلفون / Authors:", authors_str);
    }

    if (!pkg.license.empty())
        print_table_row(u8"الرخصة / License:", pkg.license);
    if (!pkg.repository.empty())
        print_table_row(u8"المستودع / Repo:", pkg.repository);

    if (!pkg.dependencies.empty()) {
        std::cout << "\n" << BOLD << u8"  التبعيات / Dependencies:" << RESET << "\n";
        for (const auto& dep : pkg.dependencies)
            std::cout << "    " << CYAN << dep.name << RESET << " "
                      << DIM << dep.requirement.to_string() << RESET << "\n";
    }

    std::cout << "\n" << BOLD << u8"  الإصدارات المتاحة (" << versions.size()
              << u8"):" << RESET << "\n";
    int max_show = std::min(static_cast<int>(versions.size()), 10);
    for (int i = 0; i < max_show; i++) {
        std::cout << "    " << (i == 0 ? GREEN : WHITE)
                  << versions[i].to_string();
        if (i == 0) std::cout << " " << GREEN << u8"(أحدث)";
        std::cout << RESET << "\n";
    }
    if (static_cast<int>(versions.size()) > max_show) {
        print_dim(u8"    ... و " + std::to_string(versions.size() - max_show) + u8" إصدارات أخرى");
        print_dim(u8"    استخدم 'sad-pkg versions " + package_name + u8"' لعرض الكل");
    }

    std::cout << "\n";
    return 0;
}

/**
 * @brief أمر: versions - عرض جميع إصدارات حزمة
 */
static int cmd_versions(const std::string& package_name) {
    if (package_name.empty()) {
        print_error(u8"اسم الحزمة مطلوب / Package name required");
        return 1;
    }

    print_header(u8"إصدارات " + package_name);

    RegistryClientV2 registry;
    auto details = registry.get_package_details(package_name);

    // محاولة بديلة عبر API الإصدارات
    auto versions = registry.get_available_versions(package_name);
    if (versions.empty()) {
        print_error(u8"الحزمة غير موجودة: " + package_name);
        return 1;
    }

    // عرض مع تفاصيل إذا متاحة
    std::string latest = details.get_string("latest_version", versions[0].to_string());

    if (details.is_object() && details.contains("versions") && details["versions"].is_array()) {
        std::cout << "\n";
        for (const auto& v : details["versions"].arr_val) {
            std::string ver = v.get_string("version", "?");
            bool yanked = v.contains("is_yanked") && v["is_yanked"].type == SimpleJson::BOOL_VAL
                          && v["is_yanked"].bool_val;
            int downloads = v.get_int("downloads", 0);
            std::string created = v.get_string("created_at", "");

            std::cout << "    ";
            if (yanked) {
                std::cout << RED << ver << u8" (مسحوب)" << RESET;
            } else if (ver == latest) {
                std::cout << GREEN << ver << u8" (أحدث)" << RESET;
            } else {
                std::cout << WHITE << ver << RESET;
            }
            if (downloads > 0) std::cout << DIM << "  " << downloads << u8" تنزيل" << RESET;
            if (!created.empty()) std::cout << DIM << "  " << created.substr(0, 10) << RESET;
            std::cout << "\n";
        }
    } else {
        std::cout << "\n";
        for (size_t i = 0; i < versions.size(); i++) {
            bool is_latest = (versions[i].to_string() == latest);
            std::cout << "    ";
            if (is_latest) {
                std::cout << GREEN << versions[i].to_string() << u8"  (أحدث)" << RESET;
            } else {
                std::cout << WHITE << versions[i].to_string() << RESET;
            }
            std::cout << "\n";
        }
    }

    std::cout << "\n" << BOLD << u8"  المجموع: " << RESET
              << versions.size() << u8" إصدار\n\n";
    return 0;
}

/**
 * @brief أمر: list - الحزم المثبتة
 */
static int cmd_list() {
    print_header(u8"الحزم المثبتة / Installed Packages");

    if (!std::filesystem::exists("deps")) {
        print_info(u8"لا توجد حزم مثبتة / No packages installed");
        return 0;
    }

    int count = 0;
    std::cout << "\n";
    for (const auto& entry : std::filesystem::directory_iterator("deps")) {
        if (!entry.is_directory()) continue;
        std::string name = entry.path().filename().string();

        std::string version = "?";
        for (auto config_name : {u8"\u0635.toml", "sad.toml"}) {
            auto config_path = entry.path() / config_name;
            if (std::filesystem::exists(config_path)) {
                try { version = Package::from_file(config_path).version.to_string(); }
                catch (...) {}
                break;
            }
        }

        std::cout << "    " << CYAN << name << RESET << " " << DIM << version << RESET << "\n";
        count++;
    }

    std::cout << "\n" << BOLD << u8"  المجموع: " << RESET << count << u8" حزمة\n\n";
    return 0;
}

/**
 * @brief أمر: yank - سحب إصدار من المستودع
 */
static int cmd_yank(const std::string& package_name, const std::string& version) {
    if (package_name.empty() || version.empty()) {
        print_error(u8"اسم الحزمة والإصدار مطلوبان");
        std::cout << DIM << "  sad-pkg yank <pkg> <version>" << RESET << "\n";
        return 1;
    }

    print_header(u8"سحب إصدار / Yank Version");

    std::cout << "\n";
    print_warning(u8"سيتم سحب " + package_name + "@" + version + u8" من المستودع");
    print_warning(u8"لن يتمكن المستخدمون الجدد من تثبيت هذا الإصدار");
    print_info(u8"المشاريع الحالية التي تعتمد عليه ستبقى تعمل");

    std::cout << "\n" << u8"  هل أنت متأكد؟ اكتب اسم الحزمة للتأكيد: ";
    std::string confirm;
    std::getline(std::cin, confirm);

    if (confirm != package_name) {
        print_info(u8"تم الإلغاء / Cancelled");
        return 0;
    }

    RegistryClientV2 registry;

    if (!registry.is_logged_in()) {
        print_error(u8"يجب تسجيل الدخول أولاً");
        return 1;
    }

    std::cout << "\n";
    return registry.yank_version(package_name, version) ? 0 : 1;
}

/**
 * @brief أمر: stats - إحصائيات المستودع
 */
static int cmd_stats() {
    print_header(u8"إحصائيات المستودع / Registry Statistics");

    RegistryClientV2 registry;
    auto stats = registry.get_stats();

    if (!stats.is_object()) {
        print_error(u8"فشل جلب الإحصائيات (الخادم غير متاح)");
        return 1;
    }

    std::cout << "\n";

    // الملخص
    if (stats.contains("summary") && stats["summary"].is_object()) {
        const auto& s = stats["summary"];
        std::cout << "  " << BOLD << u8"ملخص المستودع:" << RESET << "\n";
        std::cout << "    " << CYAN << u8"الحزم:      " << RESET
                  << s.get_int("total_packages", 0) << "\n";
        std::cout << "    " << CYAN << u8"الإصدارات:  " << RESET
                  << s.get_int("total_versions", 0) << "\n";
        std::cout << "    " << CYAN << u8"المستخدمون: " << RESET
                  << s.get_int("total_users", 0) << "\n";
        std::cout << "    " << CYAN << u8"التنزيلات:  " << RESET
                  << s.get_int("total_downloads", 0) << "\n";
    }

    // أكثر الحزم تنزيلاً
    if (stats.contains("popular_packages") && stats["popular_packages"].is_array()
        && !stats["popular_packages"].arr_val.empty()) {
        std::cout << "\n  " << BOLD << u8"الأكثر تنزيلاً:" << RESET << "\n";
        int i = 1;
        for (const auto& p : stats["popular_packages"].arr_val) {
            std::string name = p.get_string("name", "?");
            int dl = p.get_int("total_downloads", 0);
            std::string ver = p.get_string("latest_version", "");
            std::cout << "    " << YELLOW << i << "." << RESET
                      << " " << CYAN << name << RESET;
            if (!ver.empty()) std::cout << " " << DIM << ver << RESET;
            std::cout << " " << DIM << "(" << dl << u8" تنزيل)" << RESET << "\n";
            if (++i > 10) break;
        }
    }

    // أحدث الحزم
    if (stats.contains("newest_packages") && stats["newest_packages"].is_array()
        && !stats["newest_packages"].arr_val.empty()) {
        std::cout << "\n  " << BOLD << u8"الأحدث:" << RESET << "\n";
        for (const auto& p : stats["newest_packages"].arr_val) {
            std::string name = p.get_string("name", "?");
            std::string ver = p.get_string("latest_version", "");
            std::string desc = p.get_string("description_ar", p.get_string("description", ""));
            std::cout << "    " << CYAN << name << RESET;
            if (!ver.empty()) std::cout << " " << DIM << ver << RESET;
            if (!desc.empty()) std::cout << " - " << DIM << desc << RESET;
            std::cout << "\n";
        }
    }

    std::cout << "\n";
    return 0;
}

// ============================================================================
// ═══════════════════════════════════════════════════════════════════════
//                      أوامر الحساب والمصادقة
// ═══════════════════════════════════════════════════════════════════════
// ============================================================================

/**
 * @brief أمر: register - تسجيل حساب جديد في المستودع
 */
static int cmd_register() {
    print_header(u8"تسجيل حساب جديد / Register New Account");

    RegistryClientV2 registry;
    if (registry.is_logged_in()) {
        print_warning(u8"أنت مسجل الدخول بالفعل كـ " + registry.get_saved_username());
        std::cout << u8"  هل تريد إنشاء حساب جديد؟ (y/N): ";
        std::string response;
        std::getline(std::cin, response);
        if (response != "y" && response != "Y" &&
            response != u8"ن" && response != u8"نعم") {
            print_info(u8"تم الإلغاء");
            return 0;
        }
    }

    std::string username, email, password, password_confirm, display_name;

    std::cout << "\n";
    std::cout << u8"  اسم المستخدم / Username: ";
    std::getline(std::cin, username);

    if (username.empty()) {
        print_error(u8"اسم المستخدم مطلوب");
        return 1;
    }
    if (username.size() < 3) {
        print_error(u8"اسم المستخدم يجب أن يكون 3 أحرف على الأقل");
        return 1;
    }

    std::cout << u8"  البريد الإلكتروني / Email: ";
    std::getline(std::cin, email);

    if (email.empty() || email.find('@') == std::string::npos) {
        print_error(u8"بريد إلكتروني غير صالح");
        return 1;
    }

    password = read_password(u8"  كلمة المرور / Password: ");
    if (password.size() < 8) {
        print_error(u8"كلمة المرور يجب أن تكون 8 أحرف على الأقل");
        return 1;
    }

    password_confirm = read_password(u8"  تأكيد كلمة المرور / Confirm: ");
    if (password != password_confirm) {
        print_error(u8"كلمتا المرور غير متطابقتين");
        return 1;
    }

    std::cout << u8"  الاسم المعروض / Display name " << DIM << "(" << username << ")" << RESET << ": ";
    std::getline(std::cin, display_name);

    // ملخص التسجيل
    std::cout << "\n";
    std::cout << "  " << BOLD << u8"ملخص التسجيل:" << RESET << "\n";
    std::cout << "    " << DIM << u8"المستخدم:  " << RESET << username << "\n";
    std::cout << "    " << DIM << u8"البريد:    " << RESET << email << "\n";
    std::cout << "    " << DIM << u8"الاسم:     " << RESET
              << (display_name.empty() ? username : display_name) << "\n\n";

    std::cout << u8"  هل تريد المتابعة؟ (Y/n): ";
    std::string confirm;
    std::getline(std::cin, confirm);
    if (confirm == "n" || confirm == "N" || confirm == u8"لا") {
        print_info(u8"تم الإلغاء");
        return 0;
    }

    std::cout << "\n";
    if (registry.register_user(username, email, password, display_name)) {
        std::cout << "\n";
        print_success(u8"تم إنشاء الحساب وتسجيل الدخول بنجاح!");
        print_info(u8"يمكنك الآن نشر الحزم: sad-pkg publish");
        return 0;
    }

    return 1;
}

/**
 * @brief أمر: login - تسجيل الدخول
 */
static int cmd_login() {
    print_header(u8"تسجيل الدخول / Login");

    RegistryClientV2 registry;
    if (registry.is_logged_in()) {
        print_info(u8"أنت مسجل الدخول بالفعل كـ " + registry.get_saved_username());
        std::cout << u8"  هل تريد تسجيل الدخول بحساب آخر؟ (y/N): ";
        std::string response;
        std::getline(std::cin, response);
        if (response != "y" && response != "Y" &&
            response != u8"ن" && response != u8"نعم") {
            return 0;
        }
    }

    std::string username, password;

    std::cout << "\n";
    std::cout << u8"  اسم المستخدم أو البريد / Username or email: ";
    std::getline(std::cin, username);

    if (username.empty()) {
        print_error(u8"اسم المستخدم مطلوب");
        return 1;
    }

    password = read_password(u8"  كلمة المرور / Password: ");
    if (password.empty()) {
        print_error(u8"كلمة المرور مطلوبة");
        return 1;
    }

    std::cout << "\n";
    return registry.login(username, password) ? 0 : 1;
}

/**
 * @brief أمر: logout - تسجيل الخروج
 */
static int cmd_logout() {
    RegistryClientV2 registry;
    if (!registry.is_logged_in()) {
        print_info(u8"لست مسجلاً الدخول / Not logged in");
        return 0;
    }

    std::string username = registry.get_saved_username();
    registry.logout();
    print_success(u8"تم تسجيل خروج " + username + u8" / Logged out");
    return 0;
}

/**
 * @brief أمر: whoami - من أنا / عرض الملف الشخصي
 */
static int cmd_whoami() {
    RegistryClientV2 registry;

    if (!registry.is_logged_in()) {
        print_error(u8"لست مسجلاً الدخول");
        print_info(u8"  سجّل دخول:      sad-pkg login");
        print_info(u8"  أو أنشئ حساب:   sad-pkg register");
        return 1;
    }

    print_header(u8"الملف الشخصي / Profile");

    auto profile = registry.get_profile();

    std::cout << "\n";
    if (profile.is_object() && profile.contains("username")) {
        print_table_row(u8"المستخدم / Username:", profile.get_string("username", "?"));
        print_table_row(u8"البريد / Email:", profile.get_string("email", "?"));
        print_table_row(u8"الاسم / Display name:", profile.get_string("display_name", "?"));

        std::string bio = profile.get_string("bio", "");
        if (!bio.empty())
            print_table_row(u8"النبذة / Bio:", bio);

        std::string created = profile.get_string("created_at", "");
        if (!created.empty())
            print_table_row(u8"تاريخ الانضمام:", created.substr(0, 10));

        if (profile.contains("stats") && profile["stats"].is_object()) {
            std::cout << "\n  " << BOLD << u8"الإحصائيات:" << RESET << "\n";
            std::cout << "    " << CYAN << u8"الحزم:     " << RESET
                      << profile["stats"].get_int("packages", 0) << "\n";
            std::cout << "    " << CYAN << u8"التنزيلات: " << RESET
                      << profile["stats"].get_int("total_downloads", 0) << "\n";
        }
    } else {
        // بيانات محلية
        print_table_row(u8"المستخدم / Username:", registry.get_saved_username());
        print_table_row(u8"البريد / Email:", registry.get_saved_email());
        print_dim(u8"  (بيانات محلية - الخادم غير متاح)");
    }

    std::cout << "\n";
    return 0;
}

/**
 * @brief أمر: my-packages - عرض حزم المستخدم
 */
static int cmd_my_packages() {
    RegistryClientV2 registry;

    if (!registry.is_logged_in()) {
        print_error(u8"يجب تسجيل الدخول أولاً");
        return 1;
    }

    print_header(u8"حزمك المنشورة / Your Packages");

    auto result = registry.get_my_packages();

    if (!result.is_object() || !result.contains("packages") ||
        !result["packages"].is_array()) {
        print_error(u8"فشل جلب الحزم");
        return 1;
    }

    const auto& packages = result["packages"].arr_val;

    if (packages.empty()) {
        print_info(u8"لم تنشر أي حزم بعد");
        print_dim(u8"  استخدم 'sad-pkg publish' لنشر حزمتك الأولى!");
        return 0;
    }

    std::cout << "\n";
    std::cout << "  " << BOLD << std::left << std::setw(25) << u8"الاسم"
              << std::setw(12) << u8"الإصدار"
              << std::setw(12) << u8"التنزيلات"
              << u8"التحديث" << RESET << "\n";
    print_separator();

    for (const auto& p : packages) {
        std::string name = p.get_string("name", "?");
        std::string ver = p.get_string("latest_version", "?");
        int downloads = p.get_int("downloads", 0);
        std::string updated = p.get_string("updated_at", "");
        if (updated.size() > 10) updated = updated.substr(0, 10);

        std::cout << "  " << CYAN << std::left << std::setw(25) << name << RESET
                  << std::setw(12) << ver
                  << std::setw(12) << downloads
                  << DIM << updated << RESET << "\n";
    }

    std::cout << "\n" << BOLD << u8"  المجموع: " << RESET
              << packages.size() << u8" حزمة\n\n";

    return 0;
}

/**
 * @brief أمر: token - إنشاء مفتاح API جديد
 */
static int cmd_token(const std::string& token_name) {
    if (token_name.empty()) {
        print_error(u8"اسم المفتاح مطلوب / Token name required");
        std::cout << DIM << "  sad-pkg token <name>" << RESET << "\n";
        std::cout << "\n" << u8"  أمثلة:" << "\n";
        std::cout << DIM << "    sad-pkg token ci-deploy\n";
        std::cout << "    sad-pkg token github-actions\n";
        std::cout << "    sad-pkg token my-laptop\n" << RESET;
        return 1;
    }

    RegistryClientV2 registry;

    if (!registry.is_logged_in()) {
        print_error(u8"يجب تسجيل الدخول أولاً");
        return 1;
    }

    print_header(u8"إنشاء مفتاح API / Create API Token");

    // اختيار الصلاحيات
    std::cout << "\n" << u8"  الصلاحيات المتاحة:" << "\n";
    std::cout << "    " << CYAN << "1" << RESET << u8" - نشر فقط (publish)\n";
    std::cout << "    " << CYAN << "2" << RESET << u8" - قراءة فقط (read)\n";
    std::cout << "    " << CYAN << "3" << RESET << u8" - نشر + قراءة (publish, read)\n";
    std::cout << "    " << CYAN << "4" << RESET << u8" - كامل الصلاحيات (publish, read, admin)\n";
    std::cout << "\n  " << u8"اختر الصلاحيات " << DIM << "(1)" << RESET << ": ";

    std::string choice;
    std::getline(std::cin, choice);

    std::vector<std::string> scopes;
    if (choice == "2")      scopes = {"read"};
    else if (choice == "3") scopes = {"publish", "read"};
    else if (choice == "4") scopes = {"publish", "read", "admin"};
    else                    scopes = {"publish"};

    std::cout << "\n";
    std::string token = registry.create_api_token(token_name, scopes);

    if (token.empty()) {
        print_error(u8"فشل إنشاء المفتاح");
        return 1;
    }

    print_success(u8"تم إنشاء مفتاح API بنجاح!");
    std::cout << "\n";
    std::cout << "  " << BOLD << u8"الاسم:   " << RESET << token_name << "\n";
    std::cout << "  " << BOLD << u8"المفتاح: " << RESET << GREEN << token << RESET << "\n";
    std::cout << "\n";
    print_warning(u8"احفظ هذا المفتاح الآن! لن يظهر مرة أخرى.");
    std::cout << "\n" << DIM << u8"  للاستخدام في CI/CD:" << RESET << "\n";
    std::cout << DIM << "    SAD_TOKEN=" << token << " sad-pkg publish\n" << RESET;
    std::cout << "\n";

    return 0;
}

// ============================================================================
// ═══════════════════════════════════════════════════════════════════════
//                      أوامر الأدوات والإعدادات
// ═══════════════════════════════════════════════════════════════════════
// ============================================================================

/**
 * @brief أمر: config - ضبط/عرض الإعدادات
 */
static int cmd_config(const std::string& key, const std::string& value) {
    if (key.empty()) {
        // عرض جميع الإعدادات
        print_header(u8"الإعدادات / Configuration");
        std::cout << "\n";

        auto config = RegistryClientV2::load_user_config();
        RegistryClientV2 reg;

        print_table_row(u8"المستودع / Registry:", reg.get_registry_url());
        print_table_row(u8"الكاش / Cache dir:", reg.get_cache_dir().string());
        print_table_row(u8"الإعدادات / Config:", RegistryClientV2::get_config_dir_public().string());

        if (reg.is_logged_in()) {
            print_table_row(u8"المستخدم / User:", reg.get_saved_username());
        } else {
            print_table_row(u8"المستخدم / User:", u8"(غير مسجل)");
        }

        // إعدادات مخصصة
        if (config.is_object() && !config.obj_val.empty()) {
            std::cout << "\n  " << BOLD << u8"إعدادات مخصصة:" << RESET << "\n";
            for (const auto& [k, v] : config.obj_val) {
                std::cout << "    " << CYAN << k << RESET << " = ";
                if (!v.str_val.empty()) std::cout << v.str_val;
                else if (v.type == SimpleJson::Type::NUMBER) std::cout << v.num_val;
                else if (v.type == SimpleJson::Type::BOOL_VAL) std::cout << (v.bool_val ? "true" : "false");
                else std::cout << "(empty)";
                std::cout << "\n";
            }
        }

        std::cout << "\n" << BOLD << u8"  المفاتيح المتاحة:" << RESET << "\n";
        std::cout << DIM;
        std::cout << u8"    registry    - عنوان المستودع\n";
        std::cout << u8"    cache_dir   - مسار الكاش\n";
        std::cout << u8"    author      - المؤلف الافتراضي\n";
        std::cout << u8"    email       - البريد الافتراضي\n";
        std::cout << u8"    license     - الرخصة الافتراضية\n";
        std::cout << RESET << "\n";
        std::cout << DIM << u8"  مثال: sad-pkg config registry https://my-registry.com" << RESET << "\n\n";

        return 0;
    }

    if (value.empty()) {
        // قراءة إعداد واحد
        auto config = RegistryClientV2::load_user_config();
        if (config.contains(key)) {
            std::cout << config.get_string(key, "") << "\n";
        } else if (key == "registry") {
            RegistryClientV2 reg;
            std::cout << reg.get_registry_url() << "\n";
        } else if (key == "cache_dir") {
            RegistryClientV2 reg;
            std::cout << reg.get_cache_dir().string() << "\n";
        } else {
            print_error(u8"المفتاح غير موجود: " + key);
            return 1;
        }
        return 0;
    }

    // حفظ إعداد
    RegistryClientV2::save_user_config({{key, value}});
    print_success(u8"تم حفظ: " + key + " = " + value);
    return 0;
}

/**
 * @brief أمر: doctor - تشخيص شامل للبيئة
 */
static int cmd_doctor() {
    print_header(u8"تشخيص البيئة / Environment Diagnosis");

    std::cout << "\n";
    int issues = 0;

    // 1. ملف الإعداد
    std::cout << "  " << BOLD << u8"ملف الإعداد:" << RESET << " ";
    std::string config = find_config_file();
    if (!config.empty()) {
        std::cout << GREEN << u8"  " << config << RESET << "\n";
        try {
            auto pkg = Package::from_file(utf8_path(config));
            std::cout << "    " << DIM << u8"الاسم: " << pkg.name
                      << u8"  الإصدار: " << pkg.version.to_string() << RESET << "\n";
        } catch (const std::exception& e) {
            std::cout << "    " << RED << u8"خطأ: " << e.what() << RESET << "\n";
            issues++;
        }
    } else {
        std::cout << YELLOW << u8"  غير موجود (ليس مجلد مشروع)" << RESET << "\n";
    }

    // 2. مجلد التبعيات
    std::cout << "  " << BOLD << u8"مجلد التبعيات:" << RESET << " ";
    if (std::filesystem::exists("deps")) {
        int dep_count = 0;
        for (const auto& entry : std::filesystem::directory_iterator("deps"))
            if (entry.is_directory()) dep_count++;
        std::cout << GREEN << u8"  " << dep_count << u8" حزمة" << RESET << "\n";
    } else {
        std::cout << DIM << u8"  غير موجود" << RESET << "\n";
    }

    // 3. ملف القفل
    std::cout << "  " << BOLD << u8"ملف القفل:" << RESET << " ";
    if (std::filesystem::exists("sad.lock")) {
        std::cout << GREEN << u8"  sad.lock" << RESET << "\n";
    } else {
        std::cout << DIM << u8"  غير موجود" << RESET << "\n";
    }

    // 4. الكاش
    std::cout << "  " << BOLD << u8"الذاكرة المؤقتة:" << RESET << " ";
    RegistryClientV2 registry;
    size_t cache_size = registry.get_cache_size();
    double cache_mb = cache_size / (1024.0 * 1024.0);
    if (cache_size > 0) {
        std::cout << GREEN << "  " << RESET
                  << std::fixed << std::setprecision(1) << cache_mb << " MB\n";
    } else {
        std::cout << DIM << u8"  فارغة" << RESET << "\n";
    }
    std::cout << "    " << DIM << registry.get_cache_dir().string() << RESET << "\n";

    // 5. حالة المصادقة
    std::cout << "  " << BOLD << u8"حالة الدخول:" << RESET << " ";
    if (registry.is_logged_in()) {
        std::cout << GREEN << u8"  مسجل كـ " << RESET
                  << registry.get_saved_username() << "\n";
    } else {
        std::cout << YELLOW << u8"  غير مسجل" << RESET << "\n";
    }

    // 6. الاتصال
    std::cout << "  " << BOLD << u8"المستودع:" << RESET << " "
              << DIM << registry.get_registry_url() << RESET << "\n";
    std::cout << "    " << u8"الاتصال: ";
    auto test_stats = registry.get_stats();
    if (test_stats.is_object() && test_stats.contains("summary")) {
        std::cout << GREEN << u8"متصل" << RESET << "\n";
    } else {
        std::cout << YELLOW << u8"غير متصل" << RESET << "\n";
    }

    // 7. مجلد الإعدادات
    auto config_dir = RegistryClientV2::get_config_dir_public();
    std::cout << "  " << BOLD << u8"مجلد الإعدادات:" << RESET << " "
              << DIM << config_dir.string() << RESET << "\n";

    // النتيجة
    std::cout << "\n";
    if (issues == 0) {
        print_success(u8"البيئة سليمة! لا توجد مشاكل.");
    } else {
        print_warning(u8"وُجدت " + std::to_string(issues) + u8" مشكلة");
    }
    std::cout << "\n";

    return issues > 0 ? 1 : 0;
}

/**
 * @brief أمر: clean - تنظيف الكاش
 */
static int cmd_clean() {
    print_header(u8"تنظيف الذاكرة المؤقتة / Cleaning cache...");

    RegistryClientV2 registry;
    size_t cache_size = registry.get_cache_size();
    double size_mb = cache_size / (1024.0 * 1024.0);

    if (cache_size == 0) {
        print_info(u8"الذاكرة المؤقتة فارغة بالفعل");
        return 0;
    }

    std::cout << u8"\n  حجم الكاش: " << BOLD << std::fixed << std::setprecision(2)
              << size_mb << " MB" << RESET << "\n";
    std::cout << u8"  المسار: " << DIM << registry.get_cache_dir().string() << RESET << "\n\n";

    std::cout << u8"  هل تريد المتابعة؟ (y/N): ";
    std::string response;
    std::getline(std::cin, response);

    if (response != "y" && response != "Y" &&
        response != u8"ن" && response != u8"نعم") {
        print_info(u8"تم الإلغاء / Cancelled");
        return 0;
    }

    registry.clean_cache();
    print_success(u8"تم التنظيف!");
    return 0;
}

// ============================================================================
// ═══════════════════════════════════════════════════════════════════════
//                          الدالة الرئيسية
// ═══════════════════════════════════════════════════════════════════════
// ============================================================================

int main(int argc, char* argv[]) {
    init_console();

    if (argc < 2) {
        print_usage();
        return 0;
    }

    std::string command = argv[1];

    // إصدار الأداة
    if (command == "--version" || command == "-v" || command == u8"--إصدار") {
        std::cout << "sad-pkg v" << SAD_PKG_VERSION << "\n";
        return 0;
    }

    try {
        // ═══ إدارة المشروع ═══
        if (command == "init" || command == u8"أنشئ") {
            return cmd_init();
        }
        else if (command == "install" || command == u8"ثبّت") {
            std::string pkg = (argc > 2) ? argv[2] : "";
            if (argc > 3) pkg += " " + std::string(argv[3]);
            return cmd_install(pkg);
        }
        else if (command == "add" || command == u8"أضف") {
            if (argc < 3) {
                print_error(u8"اسم الحزمة مطلوب / Package name required");
                return 1;
            }
            std::string spec = argv[2];
            if (argc > 3) spec += " " + std::string(argv[3]);
            return cmd_add(spec);
        }
        else if (command == "remove" || command == u8"أزل" || command == "rm") {
            if (argc < 3) {
                print_error(u8"اسم الحزمة مطلوب / Package name required");
                return 1;
            }
            return cmd_remove(argv[2]);
        }
        else if (command == "update" || command == u8"حدّث" || command == "upgrade") {
            return cmd_update();
        }
        else if (command == "outdated" || command == u8"قديمة") {
            return cmd_outdated();
        }
        else if (command == "audit" || command == u8"تدقيق") {
            return cmd_audit();
        }
        else if (command == "verify" || command == u8"تحقق") {
            return cmd_verify();
        }

        // ═══ النشر والمستودع ═══
        else if (command == "publish" || command == u8"انشر") {
            bool local = false;
            for (int i = 2; i < argc; i++) {
                std::string arg = argv[i];
                if (arg == "--local" || arg == u8"--محلي") local = true;
            }
            return cmd_publish(local);
        }
        else if (command == "search" || command == u8"ابحث") {
            if (argc < 3) {
                print_error(u8"نص البحث مطلوب / Search query required");
                return 1;
            }
            // دمج كل الكلمات في البحث
            std::string query = argv[2];
            for (int i = 3; i < argc; i++) query += " " + std::string(argv[i]);
            return cmd_search(query);
        }
        else if (command == "info" || command == u8"معلومات") {
            if (argc < 3) {
                print_error(u8"اسم الحزمة مطلوب / Package name required");
                return 1;
            }
            return cmd_info(argv[2]);
        }
        else if (command == "versions" || command == u8"إصدارات") {
            if (argc < 3) {
                print_error(u8"اسم الحزمة مطلوب / Package name required");
                return 1;
            }
            return cmd_versions(argv[2]);
        }
        else if (command == "list" || command == "ls" || command == u8"قائمة") {
            return cmd_list();
        }
        else if (command == "yank" || command == u8"سحب") {
            if (argc < 4) {
                print_error(u8"اسم الحزمة والإصدار مطلوبان");
                std::cout << DIM << "  sad-pkg yank <pkg> <version>" << RESET << "\n";
                return 1;
            }
            return cmd_yank(argv[2], argv[3]);
        }
        else if (command == "stats" || command == u8"إحصائيات") {
            return cmd_stats();
        }

        // ═══ الحساب والمصادقة ═══
        else if (command == "register" || command == "signup" ||
                 command == u8"سجّل" || command == u8"تسجيل") {
            return cmd_register();
        }
        else if (command == "login" || command == u8"دخول") {
            return cmd_login();
        }
        else if (command == "logout" || command == u8"اخرج" || command == u8"خروج") {
            return cmd_logout();
        }
        else if (command == "whoami" || command == u8"من_أنا") {
            return cmd_whoami();
        }
        else if (command == "my-packages" || command == "my_packages" || command == u8"حزمي") {
            return cmd_my_packages();
        }
        else if (command == "token" || command == u8"مفتاح") {
            std::string name = (argc > 2) ? argv[2] : "";
            return cmd_token(name);
        }

        // ═══ الأدوات والإعدادات ═══
        else if (command == "config" || command == u8"إعدادات") {
            std::string key = (argc > 2) ? argv[2] : "";
            std::string value = (argc > 3) ? argv[3] : "";
            return cmd_config(key, value);
        }
        else if (command == "doctor" || command == u8"طبيب") {
            return cmd_doctor();
        }
        else if (command == "clean" || command == u8"نظّف") {
            return cmd_clean();
        }
        else if (command == "help" || command == "--help" ||
                 command == "-h" || command == u8"مساعدة") {
            print_usage();
            return 0;
        }
        else {
            print_error(u8"أمر غير معروف: " + command);
            print_info(u8"استخدم 'sad-pkg help' للمساعدة");
            return 1;
        }
    } catch (const std::exception& e) {
        print_error(u8"خطأ غير متوقع: " + std::string(e.what()));
        return 1;
    }

    return 0;
}
