// (AR) تنفيذ سجل الأدوات
// (EN) Tool registry implementation

#include "hub/tool_registry.h"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <set>

namespace fs = std::filesystem;

namespace Sad
{
    namespace Hub
    {

        // ============================================================================
        // (AR) إعدادات البحث / (EN) Search configuration
        // ============================================================================

        namespace
        {

            // (AR) بادئة كل أدوات لغة ص: "sad-"
            // (EN) Prefix for all Sad tools: "sad-"
            constexpr const char *kToolPrefix = "sad-";

// (AR) اللاحقة على ويندوز
// (EN) Windows suffix
#ifdef _WIN32
            constexpr const char *kExeSuffix = ".exe";
            constexpr char kPathSep = ';';
#else
            constexpr const char *kExeSuffix = "";
            constexpr char kPathSep = ':';
#endif

            // (AR) أسماء محجوزة لا تُعتبر أدوات (Hub نفسه)
            // (EN) Reserved names that are not tools (Hub itself)
            const std::set<std::string> &reservedNames()
            {
                static const std::set<std::string> names = {"hub"};
                return names;
            }

            // (AR) فصل سلسلة على فاصل
            // (EN) Split string on separator
            std::vector<std::string> splitPath(const std::string &s, char sep)
            {
                std::vector<std::string> out;
                std::string cur;
                for (char c : s)
                {
                    if (c == sep)
                    {
                        if (!cur.empty())
                            out.push_back(cur);
                        cur.clear();
                    }
                    else
                        cur += c;
                }
                if (!cur.empty())
                    out.push_back(cur);
                return out;
            }

        } // anonymous namespace

        // ============================================================================
        ToolRegistry::ToolRegistry() = default;
        ToolRegistry::~ToolRegistry() = default;

        // ============================================================================
        bool ToolRegistry::isToolExecutable(const std::string &filename, std::string &outName) const
        {
            // (AR) الاسم يجب أن يبدأ بـ "sad-" وينتهي بـ ".exe" (على ويندوز)
            // (EN) Name must start with "sad-" and end with ".exe" (on Windows)
            std::string prefix = kToolPrefix;
            std::string suffix = kExeSuffix;

            if (filename.size() <= prefix.size() + suffix.size())
                return false;
            if (filename.compare(0, prefix.size(), prefix) != 0)
                return false;
            if (!suffix.empty() &&
                filename.compare(filename.size() - suffix.size(), suffix.size(), suffix) != 0)
                return false;

            // (AR) استخراج الاسم بين البادئة واللاحقة
            // (EN) Extract name between prefix and suffix
            size_t start = prefix.size();
            size_t len = filename.size() - prefix.size() - suffix.size();
            outName = filename.substr(start, len);

            if (reservedNames().count(outName))
                return false;
            if (outName.empty())
                return false;

            // (AR) لا نقبل أسماءً تحتوي محارف غريبة
            // (EN) Reject names with weird characters
            for (char c : outName)
            {
                if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-' && c != '_')
                {
                    return false;
                }
            }

            return true;
        }

        // ============================================================================
        void ToolRegistry::addToolFromPath(const std::string &path)
        {
            fs::path p(path);
            std::string filename = p.filename().string();
            std::string toolName;

            if (!isToolExecutable(filename, toolName))
                return;

            // (AR) لا تُكرّر إذا الأداة بنفس الاسم موجودة (أول من يُكتشَف يفوز)
            // (EN) Don't duplicate if same-name tool exists (first discovered wins)
            for (const auto &t : tools_)
            {
                if (t.name == toolName)
                    return;
            }

            ToolDescriptor td;
            td.name = toolName;
            td.executable = path;
            td.isBuiltin = false;
            tools_.push_back(td);
        }

        // ============================================================================
        void ToolRegistry::scanDirectory(const std::string &dir)
        {
            searchedPaths_.push_back(dir);

            // (AR) كل العمليات هنا داخل try/catch لأن مسارات النظام قد تحتوي
            //      محارف غير قابلة للتحويل من UTF-8 إلى ANSI (مثل العربية في PATH)
            // (EN) Everything in try/catch — system paths may contain chars
            //      that can't convert UTF-8→ANSI (e.g. Arabic in PATH)
            try
            {
                std::error_code ec;
                if (!fs::exists(dir, ec) || ec)
                    return;
                if (!fs::is_directory(dir, ec) || ec)
                    return;

                // (AR) حد أقصى لعدد الملفات لتجنّب التجمّد على مجلدات ضخمة
                // (EN) Cap files to avoid hanging on huge dirs
                constexpr size_t kMaxFilesPerDir = 5000;
                size_t scanned = 0;

                fs::directory_iterator it(dir,
                                          fs::directory_options::skip_permission_denied, ec);
                if (ec)
                    return;

                for (; it != fs::directory_iterator(); it.increment(ec))
                {
                    if (ec)
                    {
                        ec.clear();
                        continue;
                    }
                    if (++scanned > kMaxFilesPerDir)
                        break;

                    try
                    {
                        std::string filename = it->path().filename().string();
                        std::string toolName;
                        if (!isToolExecutable(filename, toolName))
                            continue;

                        std::error_code ec2;
                        if (!it->is_regular_file(ec2) || ec2)
                            continue;

                        addToolFromPath(it->path().string());
                    }
                    catch (...)
                    {
                        // (AR) تجاوز أي ملف يفشل تحويل اسمه
                        // (EN) Skip any file whose name fails to convert
                        continue;
                    }
                }
            }
            catch (...)
            {
                // (AR) تجاوز المجلد بأكمله إذا فشل افتتاحه
                // (EN) Skip whole directory if opening fails
                return;
            }
        }

        // ============================================================================
        void ToolRegistry::discover(const std::string &hubExecutablePath)
        {
            tools_.clear();
            searchedPaths_.clear();

            // (AR) 1. مجلد التنفيذي نفسه
            // (EN) 1. Hub executable's own directory
            fs::path hubPath(hubExecutablePath);
            fs::path hubDir = hubPath.parent_path();
            if (!hubDir.empty())
            {
                scanDirectory(hubDir.string());

                // (AR) 2. <مجلد التنفيذي>/tools/
                // (EN) 2. <hub-dir>/tools/
                fs::path toolsSubdir = hubDir / "tools";
                scanDirectory(toolsSubdir.string());
            }

            // (AR) 3. متغير البيئة SAD_TOOLS_PATH
            // (EN) 3. SAD_TOOLS_PATH env var
            if (const char *envPath = std::getenv("SAD_TOOLS_PATH"))
            {
                for (const auto &dir : splitPath(envPath, kPathSep))
                {
                    scanDirectory(dir);
                }
            }

            // (AR) 4. PATH العام للنظام
            // (EN) 4. System PATH
            if (const char *envPath = std::getenv("PATH"))
            {
                for (const auto &dir : splitPath(envPath, kPathSep))
                {
                    scanDirectory(dir);
                }
            }

            // (AR) ترتيب أبجدي للعرض المُريح
            // (EN) Alphabetical sort for nicer display
            std::sort(tools_.begin(), tools_.end(),
                      [](const ToolDescriptor &a, const ToolDescriptor &b)
                      {
                          return a.name < b.name;
                      });
        }

        // ============================================================================
        std::optional<ToolDescriptor> ToolRegistry::find(const std::string &name) const
        {
            for (const auto &t : tools_)
            {
                if (t.name == name)
                    return t;
            }
            return std::nullopt;
        }

        // ============================================================================
        void ToolRegistry::addBuiltin(const std::string &name, const std::string &description)
        {
            ToolDescriptor td;
            td.name = name;
            td.executable = "<builtin>";
            td.description = description;
            td.isBuiltin = true;
            tools_.push_back(td);
            std::sort(tools_.begin(), tools_.end(),
                      [](const ToolDescriptor &a, const ToolDescriptor &b)
                      {
                          return a.name < b.name;
                      });
        }

    } // namespace Hub
} // namespace Sad
