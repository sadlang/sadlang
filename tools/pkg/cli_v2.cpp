/**
 * @file cli_v2.cpp
 * @brief sad-pkg CLI v2
 * @author Saleh Kadah
 * @date 2026
 */

#include "package.h"
#include "toml_parser.h"
#include "dependency_resolver.h"
#include "registry_client_v2.h"
#include "utf8_args.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdlib> // std::getenv
#include <cstring>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>

using namespace sad::pkg;

static const char *SAD_PKG_VERSION = "2.0.0";
// (AR) الثابت الموحَّد لعنوان السجلّ — مصدره registry_client_v2.h
// (EN) Unified registry URL constant — sourced from registry_client_v2.h
static const char *DEFAULT_REGISTRY = sad::pkg::DEFAULT_REGISTRY_URL;

#ifdef _WIN32
#include <windows.h>
static void init_console()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
}
#else
static void init_console() {}
#endif

static const char *RESET = "\033[0m";
static const char *BOLD = "\033[1m";
static const char *DIM = "\033[2m";
static const char *RED = "\033[31m";
static const char *GREEN = "\033[32m";
static const char *YELLOW = "\033[33m";
static const char *BLUE = "\033[34m";
static const char *MAGENTA = "\033[35m";
static const char *CYAN = "\033[36m";

static void print_header(const std::string &text)
{
    std::cout << BOLD << CYAN << text << RESET << "\n";
}
static void print_success(const std::string &text)
{
    std::cout << GREEN << "[OK] " << text << RESET << "\n";
}
static void print_error(const std::string &text)
{
    std::cerr << RED << "[X] " << text << RESET << "\n";
}
static void print_warning(const std::string &text)
{
    std::cout << YELLOW << "[!] " << text << RESET << "\n";
}
static void print_info(const std::string &text)
{
    std::cout << BLUE << "[i] " << text << RESET << "\n";
}
static void print_dim(const std::string &text)
{
    std::cout << DIM << text << RESET << "\n";
}
static void print_table_row(const std::string &key, const std::string &val,
                            const char *kc = nullptr)
{
    const char *c = kc ? kc : BOLD;
    std::cout << "  " << c << std::left << std::setw(22) << key << RESET << val << "\n";
}

// ============= Paths & Config =============

static std::filesystem::path get_config_dir()
{
#ifdef _WIN32
    const char *appdata = std::getenv("LOCALAPPDATA");
    if (appdata)
        return std::filesystem::path(appdata) / "sad-lang";
    return std::filesystem::path("C:\\ProgramData\\sad-lang");
#else
    const char *home = std::getenv("HOME");
    if (home)
        return std::filesystem::path(home) / ".config" / "sad-lang";
    return std::filesystem::path("/etc/sad-lang");
#endif
}

static std::filesystem::path get_cache_dir()
{
#ifdef _WIN32
    const char *appdata = std::getenv("LOCALAPPDATA");
    if (appdata)
        return std::filesystem::path(appdata) / "sad-lang" / "cache";
    return std::filesystem::path("C:\\ProgramData\\sad-lang\\cache");
#else
    const char *home = std::getenv("HOME");
    if (home)
        return std::filesystem::path(home) / ".cache" / "sad-lang";
    return std::filesystem::path("/tmp/sad-lang/cache");
#endif
}

static std::string get_registry_url()
{
    // (AR) أولويّة التجاوز: متغيّر البيئة SAD_REGISTRY_URL ثم config.toml ثم الافتراضيّ
    //      — نفس متغيّر التجاوز الذي يقرأه sadc (compiler_driver_pkg.cpp)
    // (EN) Override precedence: SAD_REGISTRY_URL env var, then config.toml, then default
    //      — same override variable honored by sadc (compiler_driver_pkg.cpp)
    const char *env_url = std::getenv("SAD_REGISTRY_URL");
    if (env_url && *env_url)
        return std::string(env_url);

    auto cpath = get_config_dir() / "config.toml";
    if (std::filesystem::exists(cpath))
    {
        try
        {
            std::ifstream f(cpath);
            std::string line;
            while (std::getline(f, line))
            {
                auto eq = line.find('=');
                if (eq != std::string::npos)
                {
                    std::string k = line.substr(0, eq);
                    while (!k.empty() && k.back() == ' ')
                        k.pop_back();
                    if (k == "registry")
                    {
                        std::string v = line.substr(eq + 1);
                        while (!v.empty() && (v.front() == ' ' || v.front() == '"'))
                            v.erase(v.begin());
                        while (!v.empty() && (v.back() == ' ' || v.back() == '"'))
                            v.pop_back();
                        if (!v.empty())
                            return v;
                    }
                }
            }
        }
        catch (...)
        {
        }
    }
    return DEFAULT_REGISTRY;
}

static std::string get_saved_token()
{
    auto tp = get_config_dir() / "token";
    if (std::filesystem::exists(tp))
    {
        std::ifstream f(tp);
        std::string t;
        std::getline(f, t);
        while (!t.empty() && (t.back() == '\n' || t.back() == '\r'))
            t.pop_back();
        return t;
    }
    const char *env = std::getenv("SAD_TOKEN");
    return env ? std::string(env) : "";
}

static void save_token(const std::string &t)
{
    auto d = get_config_dir();
    std::filesystem::create_directories(d);
    std::ofstream f(d / "token");
    f << t << "\n";
}

static std::string find_config_file()
{
    if (std::filesystem::exists("sad.toml"))
        return "sad.toml";
    return "";
}

static Package load_package()
{
    std::string cf = find_config_file();
    if (cf.empty())
        throw std::runtime_error("Config file not found. Run 'sad-pkg init' first.");
    return Package::from_file(cf);
}

static void save_package(const Package &pkg)
{
    std::string cf = find_config_file();
    if (cf.empty())
        cf = "sad.toml";
    pkg.save_to_file(cf);
}

// ============= Registry Adapter =============

class RegistryAdapter : public IPackageRegistry
{
    RegistryClientV2 client_;

public:
    explicit RegistryAdapter(const std::string &url) : client_(url) {}
    RegistryClientV2 &raw() { return client_; }

    std::vector<Version> get_available_versions(const std::string &name) override
    {
        return client_.get_available_versions(name);
    }
    Package get_package_metadata(const std::string &name, const Version &ver) override
    {
        Package p;
        p.name = name;
        p.version = ver;
        return p;
    }
    bool package_exists(const std::string &name) override
    {
        return !client_.get_available_versions(name).empty();
    }
};

// ============= Help =============

static void print_usage()
{
    std::cout << "\n"
              << BOLD << CYAN
              << "  sad-pkg v" << SAD_PKG_VERSION << "\n"
              << RESET << "\n"
              << BOLD << "  Usage: " << RESET << "sad-pkg <command> [options]\n\n"
              << BOLD << MAGENTA << "  Project:\n"
              << RESET
              << "    " << CYAN << "init" << RESET << "                  Create new package\n"
              << "    " << CYAN << "install" << RESET << " [pkg]        Install dependencies\n"
              << "    " << CYAN << "add" << RESET << " <pkg> [ver]      Add dependency\n"
              << "    " << CYAN << "remove" << RESET << " <pkg>          Remove dependency\n"
              << "    " << CYAN << "update" << RESET << "                Update all\n"
              << "    " << CYAN << "list" << RESET << "                  List installed\n"
              << "    " << CYAN << "verify" << RESET << "                Verify integrity\n\n"
              << BOLD << MAGENTA << "  Registry:\n"
              << RESET
              << "    " << CYAN << "publish" << RESET << " [--local]     Publish package\n"
              << "    " << CYAN << "search" << RESET << " <query>        Search packages\n"
              << "    " << CYAN << "info" << RESET << " <pkg>            Package info\n\n"
              << BOLD << MAGENTA << "  Tools:\n"
              << RESET
              << "    " << CYAN << "config" << RESET << " <key> [val]    Get/set config\n"
              << "    " << CYAN << "doctor" << RESET << "                Diagnose environment\n"
              << "    " << CYAN << "clean" << RESET << "                 Clean cache\n"
              << "    " << CYAN << "help" << RESET << "                  Show help\n"
              << "    " << CYAN << "--version" << RESET << "             Show version\n\n";
}

// ============= Project Commands =============

static int cmd_init()
{
    print_header("Initializing new package...");
    if (!find_config_file().empty())
    {
        print_error("Config file already exists.");
        return 1;
    }
    std::string name, ver, desc, author, lic;
    std::cout << "  Package name (my_package): ";
    std::getline(std::cin, name);
    if (name.empty())
        name = "my_package";
    std::cout << "  Version (0.1.0): ";
    std::getline(std::cin, ver);
    if (ver.empty())
        ver = "0.1.0";
    std::cout << "  Description: ";
    std::getline(std::cin, desc);
    std::cout << "  Author: ";
    std::getline(std::cin, author);
    std::cout << "  License (MIT): ";
    std::getline(std::cin, lic);
    if (lic.empty())
        lic = "MIT";

    Package pkg;
    pkg.name = name;
    pkg.version = Version::parse(ver);
    pkg.description = desc;
    pkg.license = lic;
    if (!author.empty())
        pkg.authors.push_back(author);
    pkg.main_file = "src/main.sad";
    pkg.source_dirs.push_back("src");
    pkg.save_to_file("sad.toml");

    std::filesystem::create_directories("src");
    std::filesystem::create_directories("tests");

    std::ofstream mf("src/main.sad");
    if (mf.is_open())
    {
        mf << "# " << name << "\n\n";
        mf.close();
    }
    std::ofstream gi(".gitignore");
    if (gi.is_open())
    {
        gi << "deps/\nbuild/\n*.o\n*.exe\nsad.lock\n";
        gi.close();
    }

    print_success("Package initialized! Created: sad.toml, src/, tests/");
    return 0;
}

static int cmd_install(const std::string &pkg_arg = "")
{
    Package pkg;
    try
    {
        pkg = load_package();
    }
    catch (const std::exception &e)
    {
        print_error(e.what());
        return 1;
    }

    if (!pkg_arg.empty())
    {
        std::string n = pkg_arg, vr = "*";
        auto sp = pkg_arg.find(' ');
        if (sp != std::string::npos)
        {
            n = pkg_arg.substr(0, sp);
            vr = pkg_arg.substr(sp + 1);
        }
        pkg.dependencies.emplace_back(n, VersionRequirement::parse(vr));
        print_info("Adding " + n + " " + vr + "...");
    }

    print_header("Installing dependencies...");
    std::string url = get_registry_url();
    RegistryAdapter adapter(url);
    DependencyResolver resolver(adapter);

    std::vector<ResolvedDependency> resolved;
    try
    {
        resolved = resolver.resolve(pkg, false);
    }
    catch (const std::exception &e)
    {
        print_error(std::string("Resolve failed: ") + e.what());
        if (resolver.has_conflicts())
        {
            for (const auto &c : resolver.get_conflicts())
                std::cout << "  " << YELLOW << c.to_string() << RESET << "\n";
        }
        return 1;
    }

    if (resolved.empty())
    {
        print_success("No dependencies to install.");
        return 0;
    }

    std::cout << "\n"
              << BOLD << "  Installing " << resolved.size()
              << " packages:\n"
              << RESET;
    int ok = 0;
    for (const auto &dep : resolved)
    {
        std::cout << "    " << CYAN << dep.name << RESET << " "
                  << DIM << dep.version.to_string() << RESET << "\n";
        if (adapter.raw().download_and_install(dep.name, dep.version, "deps"))
            ok++;
        else
            print_warning("  Failed: " + dep.name);
    }

    std::ofstream lf("sad.lock");
    if (lf.is_open())
    {
        lf << "# Lock file - do not edit\n";
        for (const auto &d : resolved)
            lf << d.name << " = \"" << d.version.to_string() << "\"\n";
        lf.close();
    }

    if (ok == static_cast<int>(resolved.size()))
    {
        print_success("Installed " + std::to_string(ok) + " packages!");
        if (!pkg_arg.empty())
            save_package(pkg);
    }
    else
    {
        print_warning("Installed " + std::to_string(ok) + "/" + std::to_string(resolved.size()));
    }
    return ok == static_cast<int>(resolved.size()) ? 0 : 1;
}

static int cmd_add(const std::string &spec)
{
    if (spec.empty())
    {
        print_error("Package name required.");
        return 1;
    }
    Package pkg;
    try
    {
        pkg = load_package();
    }
    catch (const std::exception &e)
    {
        print_error(e.what());
        return 1;
    }
    std::string n = spec, vr = "*";
    auto sp = spec.find(' ');
    if (sp != std::string::npos)
    {
        n = spec.substr(0, sp);
        vr = spec.substr(sp + 1);
    }
    bool found = false;
    for (auto &d : pkg.dependencies)
    {
        if (d.name == n)
        {
            d.requirement = VersionRequirement::parse(vr);
            found = true;
            break;
        }
    }
    if (!found)
        pkg.dependencies.emplace_back(n, VersionRequirement::parse(vr));
    save_package(pkg);
    print_success("Added " + n + " " + vr);
    print_info("Run 'sad-pkg install' to install.");
    return 0;
}

static int cmd_remove(const std::string &name)
{
    if (name.empty())
    {
        print_error("Package name required.");
        return 1;
    }
    Package pkg;
    try
    {
        pkg = load_package();
    }
    catch (const std::exception &e)
    {
        print_error(e.what());
        return 1;
    }
    auto it = std::remove_if(pkg.dependencies.begin(), pkg.dependencies.end(),
                             [&](const Dependency &d)
                             { return d.name == name; });
    if (it == pkg.dependencies.end())
    {
        print_error("Not in dependencies: " + name);
        return 1;
    }
    pkg.dependencies.erase(it, pkg.dependencies.end());
    save_package(pkg);
    auto dp = std::filesystem::path("deps") / name;
    if (std::filesystem::exists(dp))
        std::filesystem::remove_all(dp);
    print_success("Removed " + name);
    return 0;
}

static int cmd_update()
{
    print_header("Updating...");
    if (std::filesystem::exists("sad.lock"))
        std::filesystem::remove("sad.lock");
    return cmd_install();
}

static int cmd_list()
{
    print_header("Installed Packages");
    if (!std::filesystem::exists("deps"))
    {
        print_info("No packages installed.");
        return 0;
    }
    int cnt = 0;
    for (const auto &entry : std::filesystem::directory_iterator("deps"))
    {
        if (!entry.is_directory())
            continue;
        std::cout << "  " << CYAN << entry.path().filename().string() << RESET << "\n";
        cnt++;
    }
    std::cout << "\n"
              << BOLD << "  Total: " << cnt << RESET << "\n";
    return 0;
}

static int cmd_verify()
{
    print_header("Verifying integrity...");
    Package pkg;
    try
    {
        pkg = load_package();
    }
    catch (const std::exception &e)
    {
        print_error(e.what());
        return 1;
    }
    int issues = 0;
    for (const auto &dep : pkg.dependencies)
    {
        auto dp = std::filesystem::path("deps") / dep.name;
        std::cout << "  " << CYAN << dep.name << RESET;
        if (!std::filesystem::exists(dp))
        {
            std::cout << RED << " MISSING\n"
                      << RESET;
            issues++;
        }
        else
        {
            std::cout << GREEN << " OK\n"
                      << RESET;
        }
    }
    if (issues == 0)
        print_success("All OK!");
    else
        print_warning(std::to_string(issues) + " issues");
    return issues > 0 ? 1 : 0;
}

// ============= Registry Commands =============

static int cmd_search(const std::string &query)
{
    if (query.empty())
    {
        print_error("Query required.");
        return 1;
    }
    print_header("Searching: " + query);
    RegistryClientV2 reg(get_registry_url());
    auto res = reg.search(query);
    if (res.empty())
    {
        print_info("No results.");
        return 0;
    }
    std::cout << "\n"
              << BOLD << "  Results (" << res.size() << "):\n"
              << RESET;
    for (const auto &n : res)
        std::cout << "    " << CYAN << n << RESET << "\n";
    std::cout << "\n";
    return 0;
}

static int cmd_info(const std::string &name)
{
    if (name.empty())
    {
        print_error("Package name required.");
        return 1;
    }
    RegistryClientV2 reg(get_registry_url());
    auto vers = reg.get_available_versions(name);
    if (vers.empty())
    {
        print_error("Not found: " + name);
        return 1;
    }
    print_header("Package: " + name);
    print_table_row("Latest:", vers[0].to_string());
    std::cout << "\n"
              << BOLD << "  Versions (" << vers.size() << "):\n"
              << RESET;
    int mx = std::min(static_cast<int>(vers.size()), 10);
    for (int i = 0; i < mx; i++)
    {
        std::cout << "    " << (i == 0 ? GREEN : DIM)
                  << vers[i].to_string();
        if (i == 0)
            std::cout << " (latest)";
        std::cout << RESET << "\n";
    }
    if (static_cast<int>(vers.size()) > mx)
        print_dim("    ... and " + std::to_string(vers.size() - mx) + " more");
    std::cout << "\n";
    return 0;
}

static int cmd_publish(bool local_mode)
{
    Package pkg;
    try
    {
        pkg = load_package();
    }
    catch (const std::exception &e)
    {
        print_error(e.what());
        return 1;
    }
    print_info("Package: " + pkg.name + " v" + pkg.version.to_string());

    if (local_mode)
    {
        print_header("Publishing locally...");
#ifdef _WIN32
        const char *ad = std::getenv("LOCALAPPDATA");
        auto dest = (ad ? std::filesystem::path(ad) : std::filesystem::path("C:\\ProgramData")) / "sad" / "packages" / pkg.name / pkg.version.to_string();
#else
        const char *hm = std::getenv("HOME");
        auto dest = (hm ? std::filesystem::path(hm) / ".sad" : std::filesystem::path("/usr/local/share/sad")) / "packages" / pkg.name / pkg.version.to_string();
#endif
        std::filesystem::create_directories(dest);
        auto cur = std::filesystem::current_path();
        int fc = 0;
        std::error_code ec;
        for (const auto &entry : std::filesystem::recursive_directory_iterator(cur, ec))
        {
            if (!entry.is_regular_file())
                continue;
            auto rel = std::filesystem::relative(entry.path(), cur, ec);
            if (ec)
                continue;
            auto rs = rel.string();
            if (rs.find(".git") == 0 || rs.find("build") == 0 || rs.find("deps") == 0)
                continue;
            auto df = dest / rel;
            std::filesystem::create_directories(df.parent_path(), ec);
            std::filesystem::copy_file(entry.path(), df,
                                       std::filesystem::copy_options::overwrite_existing, ec);
            if (!ec)
                fc++;
        }
        print_success("Published locally! " + std::to_string(fc) + " files");
        return 0;
    }

    print_header("Publishing to registry...");
    if (get_saved_token().empty())
    {
        print_error("Must set token first: sad-pkg config token <TOKEN>");
        return 1;
    }
    // (AR) لا نُصلّب اسم النطاق في الرسالة — نعرض العنوان الفعليّ المُستخدَم
    // (EN) Don't hardcode the domain in the message — show the effective URL
    print_warning("Remote publish requires an active registry at: " + get_registry_url());
    return 0;
}

// ============= Tool Commands =============

static int cmd_config(const std::string &key, const std::string &val)
{
    if (key.empty())
    {
        print_header("Configuration");
        print_table_row("Registry:", get_registry_url());
        print_table_row("Cache:", get_cache_dir().string());
        print_table_row("Config:", get_config_dir().string());
        print_table_row("Token:", get_saved_token().empty() ? "(not set)" : "(set)");
        std::cout << "\n"
                  << DIM
                  << "  Keys: registry, token\n"
                  << "  Example: sad-pkg config registry https://my-reg.com\n"
                  << RESET << "\n";
        return 0;
    }
    if (val.empty())
    {
        if (key == "registry")
            std::cout << get_registry_url() << "\n";
        else if (key == "token")
            std::cout << (get_saved_token().empty() ? "(none)" : "(set)") << "\n";
        else
        {
            print_error("Unknown key: " + key);
            return 1;
        }
        return 0;
    }
    if (key == "token")
    {
        save_token(val);
        print_success("Token saved.");
        return 0;
    }
    auto cd = get_config_dir();
    std::filesystem::create_directories(cd);
    auto cp = cd / "config.toml";
    std::map<std::string, std::string> settings;
    if (std::filesystem::exists(cp))
    {
        std::ifstream f(cp);
        std::string line;
        while (std::getline(f, line))
        {
            auto eq = line.find('=');
            if (eq != std::string::npos)
            {
                std::string k2 = line.substr(0, eq);
                std::string v2 = line.substr(eq + 1);
                while (!k2.empty() && k2.back() == ' ')
                    k2.pop_back();
                while (!v2.empty() && (v2.front() == ' ' || v2.front() == '"'))
                    v2.erase(v2.begin());
                while (!v2.empty() && (v2.back() == ' ' || v2.back() == '"'))
                    v2.pop_back();
                settings[k2] = v2;
            }
        }
    }
    settings[key] = val;
    std::ofstream out(cp);
    for (const auto &kv : settings)
        out << kv.first << " = \"" << kv.second << "\"\n";
    print_success("Saved: " + key + " = " + val);
    return 0;
}

static int cmd_doctor()
{
    print_header("Environment Diagnosis");
    int issues = 0;

    std::cout << "  " << BOLD << "Config file: " << RESET;
    auto cf = find_config_file();
    if (!cf.empty())
    {
        std::cout << GREEN << cf << RESET << "\n";
        try
        {
            auto p = Package::from_file(cf);
            std::cout << "    " << DIM << p.name << " v"
                      << p.version.to_string() << RESET << "\n";
        }
        catch (const std::exception &e)
        {
            std::cout << "    " << RED << e.what() << RESET << "\n";
            issues++;
        }
    }
    else
    {
        std::cout << YELLOW << "(none)" << RESET << "\n";
    }

    std::cout << "  " << BOLD << "Deps dir: " << RESET;
    if (std::filesystem::exists("deps"))
    {
        int cnt = 0;
        for (const auto &entry : std::filesystem::directory_iterator("deps"))
            if (entry.is_directory())
                cnt++;
        std::cout << GREEN << cnt << " packages" << RESET << "\n";
    }
    else
    {
        std::cout << DIM << "(none)" << RESET << "\n";
    }

    std::cout << "  " << BOLD << "Lock file: " << RESET;
    if (std::filesystem::exists("sad.lock"))
        std::cout << GREEN << "sad.lock" << RESET << "\n";
    else
        std::cout << DIM << "(none)" << RESET << "\n";

    std::cout << "  " << BOLD << "Cache: " << RESET;
    auto cache = get_cache_dir();
    if (std::filesystem::exists(cache))
    {
        size_t total = 0;
        std::error_code ec;
        for (const auto &entry : std::filesystem::recursive_directory_iterator(cache, ec))
            if (entry.is_regular_file())
                total += entry.file_size();
        std::cout << std::fixed << std::setprecision(1)
                  << (total / (1024.0 * 1024.0)) << " MB\n";
    }
    else
    {
        std::cout << DIM << "(empty)" << RESET << "\n";
    }

    std::cout << "  " << BOLD << "Token: " << RESET;
    if (get_saved_token().empty())
        std::cout << YELLOW << "(not set)" << RESET << "\n";
    else
        std::cout << GREEN << "(set)" << RESET << "\n";

    std::cout << "  " << BOLD << "Registry: " << RESET
              << DIM << get_registry_url() << RESET << "\n";

    if (issues == 0)
        print_success("Environment OK!");
    else
        print_warning(std::to_string(issues) + " issues found.");
    return issues > 0 ? 1 : 0;
}

static int cmd_clean()
{
    print_header("Cleaning cache...");
    auto cache = get_cache_dir();
    if (!std::filesystem::exists(cache))
    {
        print_info("Cache already empty.");
        return 0;
    }
    size_t total = 0;
    std::error_code ec;
    for (const auto &entry : std::filesystem::recursive_directory_iterator(cache, ec))
        if (entry.is_regular_file())
            total += entry.file_size();
    if (total == 0)
    {
        print_info("Cache already empty.");
        return 0;
    }
    std::cout << "  Size: " << std::fixed << std::setprecision(2)
              << (total / (1024.0 * 1024.0)) << " MB\n";
    std::cout << "  Continue? (y/N): ";
    std::string r;
    std::getline(std::cin, r);
    if (r != "y" && r != "Y")
    {
        print_info("Cancelled.");
        return 0;
    }
    std::filesystem::remove_all(cache, ec);
    std::filesystem::create_directories(cache);
    print_success("Cache cleaned!");
    return 0;
}

// ============= Main =============

int main(int argc, char *argv[])
{
    init_console();
    // (AR) ربط argv بـUTF-8 لدعم أسماء الحزم/المسارات العربية
    Sad::Utils::Utf8ArgvHolder _argvHolder(argc, argv);
    if (argc < 2)
    {
        print_usage();
        return 0;
    }

    std::string cmd = argv[1];
    if (cmd == "--version" || cmd == "-v")
    {
        std::cout << "sad-pkg v" << SAD_PKG_VERSION << "\n";
        return 0;
    }

    try
    {
        if (cmd == "init")
            return cmd_init();

        if (cmd == "install")
        {
            std::string a;
            if (argc > 2)
            {
                a = argv[2];
                if (argc > 3)
                    a += " " + std::string(argv[3]);
            }
            return cmd_install(a);
        }

        if (cmd == "add")
        {
            if (argc < 3)
            {
                print_error("Package name required.");
                return 1;
            }
            std::string s = argv[2];
            if (argc > 3)
                s += " " + std::string(argv[3]);
            return cmd_add(s);
        }

        if (cmd == "remove" || cmd == "rm")
        {
            if (argc < 3)
            {
                print_error("Package name required.");
                return 1;
            }
            return cmd_remove(argv[2]);
        }

        if (cmd == "update" || cmd == "upgrade")
            return cmd_update();
        if (cmd == "list" || cmd == "ls")
            return cmd_list();
        if (cmd == "verify")
            return cmd_verify();

        if (cmd == "search")
        {
            if (argc < 3)
            {
                print_error("Query required.");
                return 1;
            }
            std::string q = argv[2];
            for (int i = 3; i < argc; i++)
                q += " " + std::string(argv[i]);
            return cmd_search(q);
        }

        if (cmd == "info")
        {
            if (argc < 3)
            {
                print_error("Package name required.");
                return 1;
            }
            return cmd_info(argv[2]);
        }

        if (cmd == "publish")
        {
            bool loc = false;
            for (int i = 2; i < argc; i++)
                if (std::string(argv[i]) == "--local")
                    loc = true;
            return cmd_publish(loc);
        }

        if (cmd == "config")
        {
            std::string k = (argc > 2) ? argv[2] : "";
            std::string v = (argc > 3) ? argv[3] : "";
            return cmd_config(k, v);
        }

        if (cmd == "doctor")
            return cmd_doctor();
        if (cmd == "clean")
            return cmd_clean();

        if (cmd == "help" || cmd == "--help" || cmd == "-h")
        {
            print_usage();
            return 0;
        }

        print_error("Unknown command: " + cmd);
        print_info("Use 'sad-pkg help' for help.");
        return 1;
    }
    catch (const std::exception &e)
    {
        print_error(std::string("Error: ") + e.what());
        return 1;
    }
}
