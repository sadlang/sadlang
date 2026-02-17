// بسم الله الرحمن الرحيم
/**
 * @file cli_v2.cpp
 * @brief sad-pkg CLI v2 - أداة سطر الأوامر لمدير حزم لغة ص (الإصدار ٢)
 *
 * أداة محسّنة تعمل بدون تبعيات خارجية (بدون libcurl / nlohmann/json)
 * تدعم ملفات ص.toml العربية و sad.toml الإنجليزية
 *
 * الاستخدام / Usage:
 *   sad-pkg init                    - إنشاء حزمة جديدة / Initialize new package
 *   sad-pkg install [pkg]           - تثبيت التبعيات / Install dependencies
 *   sad-pkg add <pkg> [version]     - إضافة تبعية / Add dependency
 *   sad-pkg remove <pkg>            - إزالة تبعية / Remove dependency
 *   sad-pkg update                  - تحديث التبعيات / Update dependencies
 *   sad-pkg publish                 - نشر الحزمة / Publish package
 *   sad-pkg search <query>          - البحث / Search packages
 *   sad-pkg info <pkg>              - معلومات حزمة / Package info
 *   sad-pkg list                    - سرد المثبتة / List installed
 *   sad-pkg login                   - تسجيل الدخول / Login
 *   sad-pkg logout                  - تسجيل الخروج / Logout
 *   sad-pkg clean                   - تنظيف الكاش / Clean cache
 *   sad-pkg help                    - المساعدة / Help
 *
 * @author فريق لغة ص
 * @date فبراير 2026
 */

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

static const char* RESET  = "\033[0m";
static const char* BOLD   = "\033[1m";
static const char* RED    = "\033[31m";
static const char* GREEN  = "\033[32m";
static const char* YELLOW = "\033[33m";
static const char* BLUE   = "\033[34m";
static const char* CYAN   = "\033[36m";

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

// ============================================================================
// Config File Detection - اكتشاف ملف الإعداد
// ============================================================================

/**
 * @brief Helper: create a filesystem::path from UTF-8 string, handling Windows correctly
 */
static std::filesystem::path utf8_path(const std::string& s) {
#if defined(_MSC_VER) || (defined(_WIN32) && !defined(__GNUC__))
    // MSVC: char strings are interpreted as current code page, not UTF-8
    // Use the u8path factory (C++17) for correct UTF-8 → wide conversion
    return std::filesystem::u8path(s);
#else
    return std::filesystem::path(s);
#endif
}

// Arabic config filename: ص.toml (UTF-8 encoded)
static const std::string ARABIC_CONFIG = u8"\u0635.toml";

/**
 * @brief اكتشاف ملف الإعداد (يدعم العربي والإنجليزي)
 */
static std::string find_config_file() {
    // أولاً: sad.toml الإنجليزي (أكثر توافقية)
    if (std::filesystem::exists("sad.toml")) return "sad.toml";
    // ثانياً: ملف ص.toml العربي
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
// Commands - الأوامر
// ============================================================================

/**
 * @brief عرض المساعدة
 */
static void print_usage() {
    print_header(u8"sad-pkg - مدير حزم لغة ص / Sad Language Package Manager");
    std::cout << "\n";

    std::cout << BOLD << u8"الاستخدام / USAGE:" << RESET << "\n";
    std::cout << "  sad-pkg <command> [options]\n\n";

    std::cout << BOLD << u8"الأوامر / COMMANDS:" << RESET << "\n";
    std::cout << "  " << CYAN << "init" << RESET
              << u8"              إنشاء حزمة جديدة / Initialize new package\n";
    std::cout << "  " << CYAN << "install [pkg]" << RESET
              << u8"     تثبيت التبعيات / Install dependencies\n";
    std::cout << "  " << CYAN << "add <pkg>" << RESET
              << u8"         إضافة تبعية / Add dependency\n";
    std::cout << "  " << CYAN << "remove <pkg>" << RESET
              << u8"      إزالة تبعية / Remove dependency\n";
    std::cout << "  " << CYAN << "update" << RESET
              << u8"            تحديث الكل / Update all\n";
    std::cout << "  " << CYAN << "publish" << RESET
              << u8"           نشر الحزمة / Publish package\n";
    std::cout << "  " << CYAN << "publish --local" << RESET
              << u8"    نشر محلي / Publish locally\n";
    std::cout << "  " << CYAN << "search <query>" << RESET
              << u8"    بحث / Search\n";
    std::cout << "  " << CYAN << "info <pkg>" << RESET
              << u8"        معلومات حزمة / Package info\n";
    std::cout << "  " << CYAN << "list" << RESET
              << u8"              المثبتة / List installed\n";
    std::cout << "  " << CYAN << "login" << RESET
              << u8"             تسجيل دخول / Login\n";
    std::cout << "  " << CYAN << "logout" << RESET
              << u8"            تسجيل خروج / Logout\n";
    std::cout << "  " << CYAN << "clean" << RESET
              << u8"             تنظيف الكاش / Clean cache\n";
    std::cout << "  " << CYAN << "help" << RESET
              << u8"              المساعدة / Help\n";

    std::cout << "\n" << BOLD << u8"أمثلة / EXAMPLES:" << RESET << "\n";
    std::cout << "  sad-pkg init\n";
    std::cout << "  sad-pkg add http ^1.0.0\n";
    std::cout << "  sad-pkg install\n";
    std::cout << "  sad-pkg search http\n";
    std::cout << "  sad-pkg publish\n";
    std::cout << "\n";
}

/**
 * @brief أمر: init
 */
static int cmd_init() {
    print_header(u8"إنشاء حزمة جديدة... / Initializing new package...");

    if (!find_config_file().empty()) {
        print_error(u8"ملف الإعداد موجود بالفعل / Config file already exists");
        return 1;
    }

    std::string name, version_str, description, author;

    std::cout << u8"اسم الحزمة / Package name (default: my_package): ";
    std::getline(std::cin, name);
    if (name.empty()) name = "my_package";

    std::cout << u8"الإصدار / Version (default: 0.1.0): ";
    std::getline(std::cin, version_str);
    if (version_str.empty()) version_str = "0.1.0";

    std::cout << u8"الوصف / Description: ";
    std::getline(std::cin, description);

    std::cout << u8"المؤلف / Author: ";
    std::getline(std::cin, author);

    // إنشاء الحزمة
    Package pkg;
    pkg.name = name;
    pkg.version = Version::parse(version_str);
    pkg.description = description;
    if (!author.empty()) pkg.authors.push_back(author);
    pkg.main_file = "src/main.s";
    pkg.source_dirs.push_back("src");

    // حفظ كملف sad.toml (متوافق مع جميع الأنظمة)
    pkg.save_to_file("sad.toml");

    // إنشاء هيكل المجلدات
    std::filesystem::create_directories("src");

    // إنشاء الملف الرئيسي
    std::ofstream main_file("src/main.s");
    if (main_file.is_open()) {
        main_file << u8"// بسم الله الرحمن الرحيم\n";
        main_file << u8"// " << name << u8" - نقطة الدخول الرئيسية\n\n";
        main_file << u8"دالة رئيسية() {\n";
        main_file << u8"    اطبع(\"مرحباً من " << name << u8"!\")\n";
        main_file << u8"    أرجع 0\n";
        main_file << u8"}\n";
        main_file.close();
    }

    print_success(u8"تم إنشاء الحزمة بنجاح! / Package initialized!");
    print_info(u8"أُنشئ: sad.toml و src/main.s");
    print_info(u8"استخدم 'sad-pkg install' لتثبيت التبعيات");

    return 0;
}

/**
 * @brief أمر: install
 */
static int cmd_install(const std::string& package_name = "") {
    Package pkg;
    try {
        pkg = load_package_from_config();
    } catch (const std::exception& e) {
        print_error(e.what());
        return 1;
    }

    // إذا حُددت حزمة معينة، أضفها أولاً
    if (!package_name.empty()) {
        std::string name = package_name;
        std::string version_req = "*";
        size_t space = package_name.find(' ');
        if (space != std::string::npos) {
            name = package_name.substr(0, space);
            version_req = package_name.substr(space + 1);
        }

        pkg.dependencies.emplace_back(name, VersionRequirement::parse(version_req));
        save_package_config(pkg);
        print_info(u8"أُضيف " + name + " " + version_req + u8" إلى التبعيات");
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
            std::cout << "\n" << BOLD << u8"التعارضات:" << RESET << "\n";
            for (const auto& c : resolver.get_conflicts())
                std::cout << c.to_string() << "\n";
        }
        return 1;
    }

    if (resolved.empty()) {
        print_success(u8"لا توجد تبعيات للتثبيت / No dependencies to install");
        return 0;
    }

    // تثبيت كل تبعية
    std::cout << "\n" << BOLD << u8"تثبيت " << resolved.size()
              << u8" حزمة:" << RESET << "\n";

    int success_count = 0;
    for (const auto& dep : resolved) {
        std::cout << "  " << dep.name << " " << dep.version.to_string() << "\n";
        if (registry.download_and_install(dep.name, dep.version, "deps")) {
            success_count++;
        } else {
            print_warning(u8"  فشل تثبيت: " + dep.name);
        }
    }

    // إنشاء ملف القفل
    std::ofstream lockfile("sad.lock");
    if (lockfile.is_open()) {
        lockfile << u8"# ملف القفل - لا تعدله يدوياً\n";
        lockfile << u8"# تم التوليد بواسطة sad-pkg\n\n";
        for (const auto& dep : resolved) {
            lockfile << dep.name << " = \"" << dep.version.to_string() << "\"\n";
        }
        lockfile.close();
    }

    std::cout << "\n";
    print_success(u8"تم تثبيت " + std::to_string(success_count) + u8" / "
                  + std::to_string(resolved.size()) + u8" حزمة بنجاح!");
    return (success_count == static_cast<int>(resolved.size())) ? 0 : 1;
}

/**
 * @brief أمر: add
 */
static int cmd_add(const std::string& package_spec) {
    if (package_spec.empty()) {
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
            print_warning(u8"تم تحديث إصدار '" + name + u8"'");
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
 * @brief أمر: remove
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
 * @brief أمر: search
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

    std::cout << "\n" << BOLD << u8"نتائج البحث (" << results.size()
              << u8"):" << RESET << "\n\n";

    for (const auto& name : results) {
        std::cout << "  " << CYAN << name << RESET << "\n";
    }

    return 0;
}

/**
 * @brief أمر: info
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
    std::cout << BOLD << u8"الاسم / Name:        " << RESET << pkg.name << "\n";
    std::cout << BOLD << u8"الإصدار / Version:   " << RESET << pkg.version.to_string() << "\n";
    std::cout << BOLD << u8"الوصف / Description: " << RESET << pkg.description << "\n";

    if (!pkg.authors.empty()) {
        std::cout << BOLD << u8"المؤلفون / Authors:  " << RESET;
        for (size_t i = 0; i < pkg.authors.size(); i++) {
            std::cout << pkg.authors[i];
            if (i + 1 < pkg.authors.size()) std::cout << ", ";
        }
        std::cout << "\n";
    }

    if (!pkg.license.empty())
        std::cout << BOLD << u8"الرخصة / License:   " << RESET << pkg.license << "\n";
    if (!pkg.repository.empty())
        std::cout << BOLD << u8"المستودع / Repo:     " << RESET << pkg.repository << "\n";

    std::cout << "\n" << BOLD << u8"الإصدارات المتاحة / Available versions:" << RESET << "\n";
    for (const auto& v : versions)
        std::cout << "  " << v.to_string() << "\n";

    return 0;
}

/**
 * @brief أمر: list
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

        // البحث عن ملف الإعداد
        std::string version = "?";
        for (auto config_name : {u8"\u0635.toml", "sad.toml"}) {
            auto config_path = entry.path() / config_name;
            if (std::filesystem::exists(config_path)) {
                try {
                    auto pkg = Package::from_file(config_path);
                    version = pkg.version.to_string();
                } catch (...) {}
                break;
            }
        }

        std::cout << "  " << CYAN << name << RESET << " " << version << "\n";
        count++;
    }

    std::cout << "\n" << BOLD << u8"المجموع: " << RESET << count << u8" حزمة\n";
    return 0;
}

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
 * ينسخ الحزمة إلى مخزن الحزم المحلي حتى يمكن تثبيتها من مشاريع أخرى
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

    print_info(u8"الحزمة: " + pkg.name);
    print_info(u8"الإصدار: " + pkg.version.to_string());

    // تخطي التحقق المعقد للنشر المحلي - الملفات موجودة محلياً بالفعل
    if (pkg.name.empty()) {
        print_error(u8"اسم الحزمة مطلوب");
        return 1;
    }

    // إنشاء مجلد الحزمة في المخزن المحلي
    auto packages_dir = get_local_packages_dir();
    auto pkg_name_path = utf8_path(pkg.name);
    auto pkg_dest = packages_dir / pkg_name_path / pkg.version.to_string();

    try {
        std::filesystem::create_directories(pkg_dest);

        // نسخ الملفات
        auto current = std::filesystem::current_path();
        int file_count = 0;
        size_t total_size = 0;

        // استخدام error_code لتجنب الاستثناءات مع مسارات Unicode
        std::error_code ec;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(current, ec)) {
            if (!entry.is_regular_file()) continue;

            auto rel = std::filesystem::relative(entry.path(), current, ec);
            if (ec) continue;

            // استخدام wstring للمقارنة على ويندوز لتجنب مشاكل الترميز
            std::wstring rel_wstr = rel.wstring();

            // تخطي مجلد tests و examples و .git و build
            if (rel_wstr.find(L"tests") == 0 || rel_wstr.find(L"test") == 0 ||
                rel_wstr.find(L".git") == 0 || rel_wstr.find(L"build") == 0 ||
                rel_wstr.find(L"examples") == 0) {
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

        // إنشاء ملف metadata (باستخدام wstring لمسار الملف)
        auto meta_path = pkg_dest / ".sad-pkg-meta.json";
        std::ofstream meta(meta_path);
        meta << "{\n";
        meta << "  \"name\": \"" << pkg.name << "\",\n";
        meta << "  \"version\": \"" << pkg.version.to_string() << "\",\n";
        meta << "  \"description\": \"" << pkg.description << "\",\n";
        meta << "  \"files\": " << file_count << ",\n";
        meta << "  \"size\": " << total_size << ",\n";
        meta << "  \"published_locally\": true\n";
        meta << "}\n";
        meta.close();

        double size_kb = total_size / 1024.0;
        print_success(u8"تم النشر المحلي بنجاح! / Published locally!");

        // استخدام وظيفة آمنة لعرض المسار
        std::string dest_display;
        try { dest_display = pkg_dest.u8string(); }
        catch (...) { dest_display = "(unicode path)"; }
        print_info(u8"  المسار: " + dest_display);

        std::cout << "  " << BLUE << u8"الملفات: " << file_count
                  << u8" ملف (" << std::fixed << std::setprecision(1)
                  << size_kb << " KB)" << RESET << "\n";
        print_info(u8"  للتثبيت في مشروع آخر:");
        std::cout << "    sad-pkg add " << pkg.name << " " << pkg.version.to_string() << "\n";
        std::cout << "    sad-pkg install --local\n";

    } catch (const std::exception& e) {
        print_error(u8"فشل النشر المحلي: " + std::string(e.what()));
        return 1;
    }

    return 0;
}

/**
 * @brief أمر: publish
 */
static int cmd_publish(bool local_mode = false) {
    if (local_mode) {
        return cmd_publish_local();
    }

    print_header(u8"نشر الحزمة... / Publishing package...");

    Package pkg;
    try {
        pkg = load_package_from_config();
    } catch (const std::exception& e) {
        print_error(e.what());
        return 1;
    }

    print_info(u8"الحزمة: " + pkg.name);
    print_info(u8"الإصدار: " + pkg.version.to_string());

    RegistryClientV2 registry;

    if (!registry.is_logged_in()) {
        print_warning(u8"لم يتم تسجيل الدخول. استخدم 'sad-pkg publish --local' للنشر المحلي");
        print_error(u8"يجب تسجيل الدخول أولاً: sad-pkg login");
        return 1;
    }

    if (registry.publish(pkg)) {
        return 0;
    }

    return 1;
}

/**
 * @brief أمر: login
 */
static int cmd_login() {
    print_header(u8"تسجيل الدخول / Login");

    std::string username, password;
    std::cout << u8"اسم المستخدم / Username: ";
    std::getline(std::cin, username);
    std::cout << u8"كلمة المرور / Password: ";
    std::getline(std::cin, password);

    if (username.empty() || password.empty()) {
        print_error(u8"اسم المستخدم وكلمة المرور مطلوبان");
        return 1;
    }

    RegistryClientV2 registry;
    return registry.login(username, password) ? 0 : 1;
}

/**
 * @brief أمر: logout
 */
static int cmd_logout() {
    RegistryClientV2 registry;
    registry.logout();
    print_success(u8"تم تسجيل الخروج / Logged out");
    return 0;
}

/**
 * @brief أمر: clean
 */
static int cmd_clean() {
    print_header(u8"تنظيف الذاكرة المؤقتة / Cleaning cache...");

    RegistryClientV2 registry;
    size_t cache_size = registry.get_cache_size();
    double size_mb = cache_size / (1024.0 * 1024.0);

    std::cout << u8"حجم الكاش: " << std::fixed << std::setprecision(2)
              << size_mb << " MB\n";

    std::cout << u8"هل تريد المتابعة؟ / Continue? (y/N): ";
    std::string response;
    std::getline(std::cin, response);

    if (response != "y" && response != "Y" &&
        response != u8"ن" && response != u8"نعم") {
        print_info(u8"تم الإلغاء / Cancelled");
        return 0;
    }

    registry.clean_cache();
    print_success(u8"تم التنظيف! / Cache cleared!");
    return 0;
}

/**
 * @brief أمر: update
 */
static int cmd_update() {
    print_header(u8"تحديث التبعيات... / Updating dependencies...");

    // حذف ملف القفل لإعادة حل التبعيات
    for (auto lock_name : {u8"\u0635.\u0642\u0641\u0644", "sad.lock"}) {
        if (std::filesystem::exists(lock_name))
            std::filesystem::remove(lock_name);
    }

    return cmd_install();
}

// ============================================================================
// Main - الدالة الرئيسية
// ============================================================================

int main(int argc, char* argv[]) {
    init_console();

    if (argc < 2) {
        print_usage();
        return 0;
    }

    std::string command = argv[1];

    try {
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
        else if (command == "remove" || command == u8"أزل") {
            if (argc < 3) {
                print_error(u8"اسم الحزمة مطلوب / Package name required");
                return 1;
            }
            return cmd_remove(argv[2]);
        }
        else if (command == "update" || command == u8"حدّث") {
            return cmd_update();
        }
        else if (command == "search" || command == u8"ابحث") {
            if (argc < 3) {
                print_error(u8"نص البحث مطلوب / Search query required");
                return 1;
            }
            return cmd_search(argv[2]);
        }
        else if (command == "info" || command == u8"معلومات") {
            if (argc < 3) {
                print_error(u8"اسم الحزمة مطلوب / Package name required");
                return 1;
            }
            return cmd_info(argv[2]);
        }
        else if (command == "list" || command == u8"قائمة") {
            return cmd_list();
        }
        else if (command == "publish" || command == u8"انشر") {
            bool local = false;
            for (int i = 2; i < argc; i++) {
                std::string arg = argv[i];
                if (arg == "--local" || arg == u8"--محلي") local = true;
            }
            return cmd_publish(local);
        }
        else if (command == "login" || command == u8"سجّل") {
            return cmd_login();
        }
        else if (command == "logout" || command == u8"اخرج") {
            return cmd_logout();
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
