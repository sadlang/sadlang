// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════
// ملف: compiler_driver_pkg.cpp
// الوصف: تكامل sadc مع مدير الحزم sad-pkg
// المؤلف: مشروع لغة ص
// الإصدار: 1.0.0
// ═══════════════════════════════════════════════════════════════════════════
//
// نظرة عامة:
// ══════════
// هذا الملف يوفر التكامل بين المترجم (sadc) ومدير الحزم (sad-pkg)
// بحيث يمكن للمطور إدارة الحزم مباشرة من سطر أوامر المترجم.
//
// البنية المعمارية:
// ═════════════════
//
//   ┌─────────────────┐     HTTP/JSON      ┌─────────────────┐
//   │   sadc pkg      │ ◄─────────────────► │   sad-registry  │
//   │   (هذا الملف)   │                     │   (خادم الحزم)  │
//   └────────┬────────┘                     └─────────────────┘
//            │                                      ▲
//            │ قراءة/كتابة                          │
//            ▼                                      │
//   ┌─────────────────┐                    تحميل الحزم
//   │   ./حزم/        │ ◄───────────────────────────┘
//   │   (مجلد محلي)   │
//   └─────────────────┘
//
// الميزات:
// ════════
//   1. تثبيت الحزم من السجل المركزي (install)
//   2. قراءة ملف sad.toml لمعرفة التبعيات (sync)
//   3. سرد الحزم المثبتة محلياً (list)
//   4. البحث عن حزم في السجل (search)
//   5. استخراج Kotlin Bridges للأندرويد
//   6. استخراج اعتماديات Gradle
//
// الاستخدام:
// ══════════
//   sadc pkg install <اسم_الحزمة>     -- تثبيت حزمة
//   sadc pkg install حزمة -v 1.2.0   -- تثبيت إصدار محدد
//   sadc pkg list                     -- سرد الحزم المثبتة
//   sadc pkg sync                     -- مزامنة من sad.toml
//   sadc pkg search <نص>              -- البحث عن حزم
//
// ملف sad.toml:
// ═════════════
// ملف التكوين الرئيسي للمشروع. مثال:
//
//   [package]
//   name = "مشروعي"
//   version = "1.0.0"
//
//   [dependencies]
//   شبكة-http = "1.2.0"
//   رياضيات = "2.0.0"
//
// مجلد الحزم:
// ═══════════
// الحزم تُخزّن في ./حزم/ بالهيكل التالي:
//
//   حزم/
//   ├── شبكة-http/
//   │   ├── package.json      # بيانات الحزمة
//   │   ├── src/
//   │   │   └── http.ص        # ملفات المصدر
//   │   └── kotlin/
//   │       └── HttpBridge.kt # جسور Kotlin
//   └── رياضيات/
//       └── ...
//
// السجل المركزي:
// ═══════════════
// العنوان الافتراضي: http://185.47.174.39:3000
// يمكن تغييره عبر متغير البيئة: SAD_REGISTRY_URL
//
// API المتاح:
//   GET  /api/v1/packages/:name/:version/download
//   GET  /api/v1/search?q=query
//   GET  /api/v1/packages/:name
//
// ═══════════════════════════════════════════════════════════════════════════

#include "compiler_driver.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <map>
#include <cstring>

// تضمين عميل المستودع (الإصدار ٢ بدون تبعيات خارجية)
#include "../pkg/registry_client_v2.h"

#ifdef _WIN32
#include <windows.h>
// لحل مشكلة ترميز UTF-8 في Windows
#define WIN_UTF8_CONSOLE() SetConsoleOutputCP(CP_UTF8)
#else
#define WIN_UTF8_CONSOLE()
#endif

namespace fs = std::filesystem;

namespace sad
{
    namespace driver
    {

        // ═══════════════════════════════════════════════════════════════════════════
        // ثوابت التكوين
        // ═══════════════════════════════════════════════════════════════════════════

        // عنوان خادم السجل المركزي
        // يمكن تغييره عبر متغير البيئة SAD_REGISTRY_URL
        const std::string DEFAULT_REGISTRY_URL = "http://185.47.174.39:3000";

        // مجلد تخزين الحزم المحلية
        const std::string PACKAGES_DIR = "حزم";

        // ═══════════════════════════════════════════════════════════════════════════
        // هياكل البيانات
        // ═══════════════════════════════════════════════════════════════════════════

        /**
         * @brief معلومات حزمة محلية
         *
         * تمثل حزمة مثبتة محلياً مع بياناتها الأساسية
         */
        struct LocalPackage
        {
            std::string name;                      // اسم الحزمة (مثل: شبكة-http)
            std::string version;                   // الإصدار (مثل: 1.2.0)
            std::string path;                      // مسار الحزمة المحلي
            std::vector<std::string> dependencies; // التبعيات

            // ملفات الحزمة
            std::vector<std::string> source_files;   // ملفات .ص
            std::vector<std::string> kotlin_bridges; // ملفات .kt
            std::vector<std::string> gradle_deps;    // اعتماديات Gradle
        };

        /**
         * @brief نتيجة عملية الحزم
         */
        struct PackageResult
        {
            bool success = false;
            std::string message;
            std::vector<LocalPackage> packages;
            std::vector<std::string> errors;
            std::vector<std::string> warnings;
        };

        // ═══════════════════════════════════════════════════════════════════════════
        // دوال مساعدة
        // ═══════════════════════════════════════════════════════════════════════════

        /**
         * @brief الحصول على عنوان السجل من البيئة أو الافتراضي
         */
        std::string getRegistryUrl()
        {
            const char *env_url = std::getenv("SAD_REGISTRY_URL");
            if (env_url && std::strlen(env_url) > 0)
            {
                return std::string(env_url);
            }
            return DEFAULT_REGISTRY_URL;
        }

        /**
         * @brief طباعة مساعدة أوامر pkg
         */
        void printPkgHelp()
        {
            std::cout << u8R"(
═══════════════════════════════════════════════════════════════════════════
  sadc pkg — تكامل مدير الحزم مع المترجم
═══════════════════════════════════════════════════════════════════════════

الاستخدام:
  sadc pkg <أمر> [خيارات]

الأوامر المتاحة:
  install <حزمة>    تثبيت حزمة من السجل المركزي
  list              سرد الحزم المثبتة محلياً
  sync              مزامنة الحزم من ملف sad.toml
  info <حزمة>       عرض معلومات حزمة
  search <نص>       البحث عن حزم في السجل

أمثلة:
  sadc pkg install شبكة-http
  sadc pkg install sad-math --version 1.2.0
  sadc pkg list
  sadc pkg sync
  sadc pkg search رياضيات

ملاحظات:
  - الحزم تُخزّن في مجلد ./حزم/
  - التبعيات تُحل تلقائياً
  - يمكن تغيير عنوان السجل عبر متغير البيئة SAD_REGISTRY_URL

)" << std::endl;
        }

        /**
         * @brief قراءة ملف sad.toml للحصول على التبعيات
         *
         * @param toml_path مسار الملف
         * @return خريطة التبعيات (اسم -> إصدار)
         */
        std::map<std::string, std::string> parseSadToml(const std::string &toml_path)
        {
            std::map<std::string, std::string> dependencies;

            std::ifstream file(toml_path);
            if (!file.is_open())
            {
                return dependencies;
            }

            std::string line;
            bool in_deps_section = false;

            while (std::getline(file, line))
            {
                // إزالة المسافات البادئة والزائدة
                size_t start = line.find_first_not_of(" \t");
                size_t end = line.find_last_not_of(" \t\r\n");
                if (start == std::string::npos)
                    continue;
                line = line.substr(start, end - start + 1);

                // تجاهل التعليقات
                if (line.empty() || line[0] == '#')
                    continue;

                // البحث عن قسم [dependencies]
                if (line == "[dependencies]")
                {
                    in_deps_section = true;
                    continue;
                }

                // الخروج من القسم عند بداية قسم جديد
                if (line[0] == '[')
                {
                    in_deps_section = false;
                    continue;
                }

                // تحليل التبعية: name = "version"
                if (in_deps_section)
                {
                    size_t eq_pos = line.find('=');
                    if (eq_pos != std::string::npos)
                    {
                        std::string name = line.substr(0, eq_pos);
                        std::string version = line.substr(eq_pos + 1);

                        // تنظيف الاسم
                        name.erase(0, name.find_first_not_of(" \t"));
                        name.erase(name.find_last_not_of(" \t") + 1);

                        // تنظيف الإصدار وإزالة علامات الاقتباس
                        version.erase(0, version.find_first_not_of(" \t\"'"));
                        version.erase(version.find_last_not_of(" \t\"'") + 1);

                        if (!name.empty() && !version.empty())
                        {
                            dependencies[name] = version;
                        }
                    }
                }
            }

            return dependencies;
        }

        /**
         * @brief سرد الحزم المثبتة محلياً
         */
        std::vector<LocalPackage> listLocalPackages()
        {
            std::vector<LocalPackage> packages;

            fs::path packages_dir = fs::u8path(PACKAGES_DIR);
            if (!fs::exists(packages_dir))
            {
                return packages;
            }

            for (const auto &entry : fs::directory_iterator(packages_dir))
            {
                if (entry.is_directory())
                {
                    LocalPackage pkg;
                    pkg.name = entry.path().filename().u8string();
                    pkg.path = entry.path().u8string();

                    // البحث عن ملفات المصدر
                    for (const auto &file : fs::recursive_directory_iterator(entry.path()))
                    {
                        if (file.is_regular_file())
                        {
                            std::string ext = file.path().extension().u8string();
                            std::string filename = file.path().filename().u8string();

                            // ملفات .ص (UTF-8: D8 B5)
                            if (ext.size() >= 3 && ext.substr(ext.size() - 3) == u8".ص")
                            {
                                pkg.source_files.push_back(file.path().u8string());
                            }
                            // ملفات .kt (Kotlin bridges)
                            else if (ext == ".kt")
                            {
                                pkg.kotlin_bridges.push_back(file.path().u8string());
                            }
                            // قراءة package.json أو حزمة.صم للإصدار
                            else if (filename == "package.json" || filename == u8"حزمة.صم")
                            {
                                std::ifstream pf(file.path());
                                std::string content((std::istreambuf_iterator<char>(pf)),
                                                    std::istreambuf_iterator<char>());
                                // استخراج الإصدار بشكل بسيط
                                size_t ver_pos = content.find("\"version\"");
                                if (ver_pos != std::string::npos)
                                {
                                    size_t start = content.find("\"", ver_pos + 9);
                                    size_t end = content.find("\"", start + 1);
                                    if (start != std::string::npos && end != std::string::npos)
                                    {
                                        pkg.version = content.substr(start + 1, end - start - 1);
                                    }
                                }
                            }
                        }
                    }

                    packages.push_back(pkg);
                }
            }

            return packages;
        }

        // ═══════════════════════════════════════════════════════════════════════════
        // معالج الأوامر الرئيسي
        // ═══════════════════════════════════════════════════════════════════════════

        /**
         * @brief معالجة أوامر pkg
         *
         * نقطة الدخول الرئيسية لأوامر إدارة الحزم من sadc
         *
         * @param argc عدد الوسائط
         * @param argv مصفوفة الوسائط
         * @return كود الخروج (0 = نجاح)
         */
        int handlePkgCommand(int argc, char *argv[])
        {
            WIN_UTF8_CONSOLE();

            if (argc < 3)
            {
                printPkgHelp();
                return 0;
            }

            std::string subCommand = argv[2];

            // ═══════════════════════════════════════════════════════════════════
            // أمر list — سرد الحزم المثبتة
            // ═══════════════════════════════════════════════════════════════════
            if (subCommand == "list" || subCommand == u8"سرد")
            {
                std::cout << u8"\n═══════════════════════════════════════════════════════════\n";
                std::cout << u8"  الحزم المثبتة محلياً\n";
                std::cout << u8"═══════════════════════════════════════════════════════════\n\n";

                auto packages = listLocalPackages();

                if (packages.empty())
                {
                    std::cout << u8"  لا توجد حزم مثبتة.\n";
                    std::cout << u8"  استخدم: sadc pkg install <اسم_الحزمة>\n\n";
                    return 0;
                }

                for (const auto &pkg : packages)
                {
                    std::cout << u8"  📦 " << pkg.name;
                    if (!pkg.version.empty())
                    {
                        std::cout << " (" << pkg.version << ")";
                    }
                    std::cout << "\n";
                    std::cout << u8"     المسار: " << pkg.path << "\n";
                    std::cout << u8"     ملفات .ص: " << pkg.source_files.size() << "\n";
                    if (!pkg.kotlin_bridges.empty())
                    {
                        std::cout << u8"     Kotlin Bridges: " << pkg.kotlin_bridges.size() << "\n";
                    }
                    std::cout << "\n";
                }

                std::cout << u8"  المجموع: " << packages.size() << u8" حزمة\n\n";
                return 0;
            }

            // ═══════════════════════════════════════════════════════════════════
            // أمر sync — مزامنة من sad.toml
            // ═══════════════════════════════════════════════════════════════════
            if (subCommand == "sync" || subCommand == u8"مزامنة")
            {
                std::string toml_path = "sad.toml";

                // البحث عن خيار --file
                for (int i = 3; i < argc; ++i)
                {
                    std::string arg = argv[i];
                    if ((arg == "-f" || arg == "--file") && i + 1 < argc)
                    {
                        toml_path = argv[++i];
                    }
                }

                if (!fs::exists(toml_path))
                {
                    std::cerr << u8"خطأ: ملف " << toml_path << u8" غير موجود\n";
                    std::cerr << u8"أنشئ الملف باستخدام: sad-pkg init\n";
                    return 1;
                }

                std::cout << u8"\n═══════════════════════════════════════════════════════════\n";
                std::cout << u8"  مزامنة الحزم من " << toml_path << "\n";
                std::cout << u8"═══════════════════════════════════════════════════════════\n\n";

                auto deps = parseSadToml(toml_path);

                if (deps.empty())
                {
                    std::cout << u8"  لا توجد تبعيات في الملف.\n\n";
                    return 0;
                }

                std::cout << u8"  التبعيات المطلوبة:\n";
                for (const auto &[name, version] : deps)
                {
                    std::cout << u8"    • " << name << " " << version << "\n";
                }
                std::cout << "\n";

                // تثبيت التبعيات
                std::string registry_url = getRegistryUrl();
                sad::pkg::RegistryClientV2 client(registry_url);
                fs::path dest_dir = fs::u8path(PACKAGES_DIR);
                fs::create_directories(dest_dir);

                int installed = 0;
                int failed = 0;

                for (const auto &[name, version_str] : deps)
                {
                    // التحقق إذا كانت مثبتة مسبقاً
                    if (fs::exists(dest_dir / fs::u8path(name)))
                    {
                        std::cout << u8"  ✓ " << name << u8" (موجود بالفعل)\n";
                        continue;
                    }

                    try
                    {
                        sad::pkg::Version ver = sad::pkg::Version::parse(version_str);
                        bool success = client.download_and_install(name, ver, dest_dir);
                        if (success)
                        {
                            std::cout << u8"  ✓ " << name << "@" << version_str << u8" (تم التثبيت)\n";
                            installed++;
                        }
                        else
                        {
                            std::cerr << u8"  ✗ " << name << u8" (فشل)\n";
                            failed++;
                        }
                    }
                    catch (const std::exception &e)
                    {
                        std::cerr << u8"  ✗ " << name << ": " << e.what() << "\n";
                        failed++;
                    }
                }

                std::cout << u8"\n  المجموع: " << installed << u8" تم تثبيتها";
                if (failed > 0)
                {
                    std::cout << u8", " << failed << u8" فشلت";
                }
                std::cout << "\n\n";

                return failed > 0 ? 1 : 0;
            }

            // ═══════════════════════════════════════════════════════════════════
            // أمر install — تثبيت حزمة
            // ═══════════════════════════════════════════════════════════════════
            if (subCommand == "install" || subCommand == u8"تثبيت")
            {
                if (argc < 4)
                {
                    std::cerr << u8"خطأ: يجب تحديد اسم الحزمة\n";
                    std::cerr << u8"الاستخدام: sadc pkg install <اسم_الحزمة>\n";
                    return 1;
                }

                std::string pkgName = argv[3];
                std::string version = ""; // فارغ = أحدث إصدار

                // البحث عن خيار --version
                for (int i = 4; i < argc; ++i)
                {
                    std::string arg = argv[i];
                    if ((arg == "-v" || arg == "--version") && i + 1 < argc)
                    {
                        version = argv[++i];
                    }
                }

                std::cout << u8"\n═══════════════════════════════════════════════════════════\n";
                std::cout << u8"  تثبيت حزمة: " << pkgName << "\n";
                std::cout << u8"═══════════════════════════════════════════════════════════\n\n";

                std::string registry_url = getRegistryUrl();
                std::cout << u8"  السجل: " << registry_url << "\n";

                try
                {
                    // إنشاء عميل المستودع
                    sad::pkg::RegistryClientV2 client(registry_url);

                    // تحديد الإصدار
                    sad::pkg::Version ver;
                    if (version.empty())
                    {
                        // الحصول على أحدث إصدار
                        auto versions = client.get_available_versions(pkgName);
                        if (versions.empty())
                        {
                            std::cerr << u8"\n  ✗ الحزمة غير موجودة: " << pkgName << "\n\n";
                            return 1;
                        }
                        ver = versions.front(); // أحدث إصدار
                        std::cout << u8"  أحدث إصدار: " << ver.to_string() << "\n\n";
                    }
                    else
                    {
                        ver = sad::pkg::Version::parse(version);
                        std::cout << u8"  الإصدار: " << ver.to_string() << "\n\n";
                    }

                    // مجلد الوجهة
                    fs::path dest_dir = fs::u8path(PACKAGES_DIR);
                    fs::create_directories(dest_dir);

                    // تنزيل وتثبيت
                    bool success = client.download_and_install(pkgName, ver, dest_dir);

                    if (success)
                    {
                        std::cout << u8"\n  ✓ تم تثبيت " << pkgName << "@" << ver.to_string() << " بنجاح\n";
                        std::cout << u8"  المسار: " << (dest_dir / pkgName).u8string() << "\n\n";
                        return 0;
                    }
                    else
                    {
                        std::cerr << u8"\n  ✗ فشل تثبيت الحزمة\n\n";
                        return 1;
                    }
                }
                catch (const std::exception &e)
                {
                    std::cerr << u8"\n  ✗ خطأ: " << e.what() << "\n\n";
                    return 1;
                }
            }

            // ═══════════════════════════════════════════════════════════════════
            // أمر search — البحث عن حزم
            // ═══════════════════════════════════════════════════════════════════
            if (subCommand == "search" || subCommand == u8"بحث")
            {
                if (argc < 4)
                {
                    std::cerr << u8"خطأ: يجب تحديد نص البحث\n";
                    std::cerr << u8"الاستخدام: sadc pkg search <نص>\n";
                    return 1;
                }

                std::string query = argv[3];
                std::string registry_url = getRegistryUrl();

                std::cout << u8"\n═══════════════════════════════════════════════════════════\n";
                std::cout << u8"  البحث عن: " << query << "\n";
                std::cout << u8"═══════════════════════════════════════════════════════════\n\n";

                std::cout << u8"  السجل: " << registry_url << "\n\n";

                try
                {
                    // إنشاء عميل المستودع
                    sad::pkg::RegistryClientV2 client(registry_url);

                    // البحث
                    auto results = client.search(query);

                    if (results.empty())
                    {
                        std::cout << u8"  لا توجد نتائج لـ \"" << query << "\"\n\n";
                        return 0;
                    }

                    std::cout << u8"  النتائج (" << results.size() << u8" حزمة):\n\n";

                    for (const auto &pkgName : results)
                    {
                        std::cout << u8"    📦 " << pkgName;

                        // محاولة الحصول على أحدث إصدار
                        try
                        {
                            auto versions = client.get_available_versions(pkgName);
                            if (!versions.empty())
                            {
                                std::cout << " (" << versions.front().to_string() << ")";
                            }
                        }
                        catch (...)
                        {
                        }

                        std::cout << "\n";
                    }

                    std::cout << u8"\n  استخدم: sadc pkg install <اسم_الحزمة>\n\n";
                    return 0;
                }
                catch (const std::exception &e)
                {
                    std::cerr << u8"\n  ✗ خطأ في الاتصال: " << e.what() << "\n\n";
                    return 1;
                }
            }

            // أمر غير معروف
            std::cerr << u8"خطأ: أمر غير معروف: " << subCommand << "\n";
            printPkgHelp();
            return 1;
        }

        // ═══════════════════════════════════════════════════════════════════════════
        // دوال مساعدة للتكامل مع UI
        // ═══════════════════════════════════════════════════════════════════════════

        /**
         * @brief الحصول على Kotlin Bridges لحزمة معينة
         *
         * @param pkgName اسم الحزمة
         * @return قائمة مسارات ملفات Kotlin
         */
        std::vector<std::string> getPackageKotlinBridges(const std::string &pkgName)
        {
            std::vector<std::string> bridges;

            fs::path pkg_path = fs::u8path(PACKAGES_DIR) / fs::u8path(pkgName);
            if (!fs::exists(pkg_path))
            {
                return bridges;
            }

            // البحث في مجلد android/ أو kotlin/
            std::vector<fs::path> search_dirs = {
                pkg_path / "android",
                pkg_path / "kotlin",
                pkg_path // الجذر أيضاً
            };

            for (const auto &dir : search_dirs)
            {
                if (fs::exists(dir) && fs::is_directory(dir))
                {
                    for (const auto &file : fs::directory_iterator(dir))
                    {
                        if (file.is_regular_file() && file.path().extension() == ".kt")
                        {
                            bridges.push_back(file.path().u8string());
                        }
                    }
                }
            }

            return bridges;
        }

        /**
         * @brief الحصول على اعتماديات Gradle لحزمة معينة
         *
         * @param pkgName اسم الحزمة
         * @return قائمة اعتماديات Gradle (مثل: com.squareup.okhttp3:okhttp:4.12.0)
         */
        std::vector<std::string> getPackageGradleDeps(const std::string &pkgName)
        {
            std::vector<std::string> deps;

            // قراءة من ملف package.json أو حزمة.صم
            fs::path pkg_path = fs::u8path(PACKAGES_DIR) / fs::u8path(pkgName);

            std::vector<fs::path> metadata_files = {
                pkg_path / "package.json",
                pkg_path / u8"حزمة.صم"};

            for (const auto &meta_path : metadata_files)
            {
                if (fs::exists(meta_path))
                {
                    std::ifstream file(meta_path);
                    std::string content((std::istreambuf_iterator<char>(file)),
                                        std::istreambuf_iterator<char>());

                    // البحث عن android_deps
                    size_t deps_pos = content.find("\"android_deps\"");
                    if (deps_pos != std::string::npos)
                    {
                        size_t arr_start = content.find("[", deps_pos);
                        size_t arr_end = content.find("]", arr_start);
                        if (arr_start != std::string::npos && arr_end != std::string::npos)
                        {
                            std::string deps_str = content.substr(arr_start + 1, arr_end - arr_start - 1);

                            // تحليل القائمة البسيطة
                            size_t pos = 0;
                            while ((pos = deps_str.find("\"", pos)) != std::string::npos)
                            {
                                size_t end = deps_str.find("\"", pos + 1);
                                if (end != std::string::npos)
                                {
                                    std::string dep = deps_str.substr(pos + 1, end - pos - 1);
                                    if (!dep.empty() && dep.find(":") != std::string::npos)
                                    {
                                        deps.push_back(dep);
                                    }
                                    pos = end + 1;
                                }
                                else
                                {
                                    break;
                                }
                            }
                        }
                    }
                    break;
                }
            }

            return deps;
        }

    } // namespace driver
} // namespace sad
