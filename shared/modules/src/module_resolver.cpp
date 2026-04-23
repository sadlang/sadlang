/**
 * @file module_resolver.cpp
 * @brief (AR) تنفيذ محلل الوحدات - يجد ويحمل ملفات الوحدات
 *        (EN) Implementation of Module Resolver - Finds and loads module files
 *
 * @author Sad Language Development Team / فريق تطوير لغة ص
 * @date December 2025 / ديسمبر 2025
 * @version 1.0.0 - Phase 1: Basic Module Resolution
 */

#include "module_resolver.h"
#include "parser_core.h"
#include "lexer_core.h"
#include "module_nodes.h"
#include "declarations.h"
#include "../../utils/include/utf8_utils.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <pwd.h>
#endif

namespace Sad
{
    namespace Modules
    {

        // =========================================================================
        // (AR) البناء والهدم / (EN) Constructor and Destructor
        // =========================================================================

        /**
         * @brief (AR) البناء - يُهيئ مسارات البحث الافتراضية
         *        (EN) Constructor - Initializes default search paths
         */
        ModuleResolver::ModuleResolver()
        {
            initializeDefaultPaths();
        }

        // =========================================================================
        // (AR) تهيئة مسارات البحث / (EN) Initialize Search Paths
        // =========================================================================

        /**
         * @brief (AR) تهيئة مسارات البحث الافتراضية
         *        (EN) Initialize default search paths
         *
         * @details
         * (AR) يضيف المسارات بالترتيب:
         *      1. المجلد الحالي (.)
         *      2. مجلد المكتبة القياسية (stdlib/)
         *      3. المسارات من متغير البيئة SAD_PATH
         *      4. مجلد المستخدم (~/.sad/modules)
         *
         * (EN) Adds paths in order:
         *      1. Current directory (.)
         *      2. Standard library directory (stdlib/)
         *      3. Paths from SAD_PATH environment variable
         *      4. User directory (~/.sad/modules)
         */
        void ModuleResolver::initializeDefaultPaths()
        {
            // (AR) 1. المجلد الحالي / (EN) 1. Current directory
            searchPaths_.push_back(std::filesystem::current_path());

            // (AR) 2. مجلد stdlib بجوار cwd (للحالات حين يُشغَّل من جذر المشروع)
            // (EN) stdlib next to cwd (when run from project root)
            {
                auto cwdStdlib = std::filesystem::current_path() / "stdlib";
                if (std::filesystem::exists(cwdStdlib))
                {
                    searchPaths_.push_back(cwdStdlib);
                    if (stdlibPath_.empty())
                        stdlibPath_ = cwdStdlib;
                }
            }

            // (AR) 3. مجلد stdlib بجوار الملف التنفيذي — يصعد المجلدات بحثاً عن stdlib/
            //      هذا يضمن العثور على المكتبة القياسية بغض النظر عن cwd عند الاستدعاء.
            //      نُضيف **كل** مجلدات stdlib المرشحة (وليس الأول فقط) لأن مجلدات
            //      البناء قد تحوي stdlib جزئياً (نسخة منشورة) بينما الجذر يحوي الكامل.
            // (EN) stdlib next to executable — climbs directories searching for stdlib/.
            //      We add **all** candidate stdlib dirs (not just the first) because
            //      build folders may contain a partial deployed stdlib while the
            //      project root holds the complete one.
            try
            {
                auto exeDir = sad::utf8::get_executable_dir();
                std::vector<std::filesystem::path> candidateRoots = {
                    exeDir,
                    exeDir.parent_path(),
                    exeDir.parent_path().parent_path(),
                    exeDir.parent_path().parent_path().parent_path(),
                    exeDir.parent_path().parent_path().parent_path().parent_path(),
                };
                for (const auto &root : candidateRoots)
                {
                    if (root.empty())
                        continue;
                    auto candidate = root / "stdlib";
                    if (std::filesystem::exists(candidate))
                    {
                        auto normalized = std::filesystem::absolute(candidate).lexically_normal();
                        auto it = std::find(searchPaths_.begin(), searchPaths_.end(), normalized);
                        if (it == searchPaths_.end())
                        {
                            searchPaths_.push_back(normalized);
                        }
                        if (stdlibPath_.empty())
                            stdlibPath_ = normalized;
                        // (AR) لا نتوقف — نتابع لإضافة جميع مجلدات stdlib المرشحة
                        // (EN) Don't break — keep adding all candidate stdlib dirs
                    }
                }
            }
            catch (...)
            {
                // (AR) فشل اكتشاف مسار executable — نتجاهل بصمت
                // (EN) Executable path discovery failed — silently ignore
            }

            // (AR) 3. متغير البيئة SAD_PATH / (EN) 3. SAD_PATH environment variable
            const char *sadPath = std::getenv("SAD_PATH");
            if (sadPath)
            {
                std::string pathStr(sadPath);
                std::istringstream iss(pathStr);
                std::string path;

#ifdef _WIN32
                const char delimiter = ';';
#else
                const char delimiter = ':';
#endif

                while (std::getline(iss, path, delimiter))
                {
                    if (!path.empty() && std::filesystem::exists(path))
                    {
                        searchPaths_.push_back(std::filesystem::path(path));
                    }
                }
            }

            // (AR) 4. مجلد المستخدم / (EN) 4. User directory
#ifdef _WIN32
            const char *userProfile = std::getenv("USERPROFILE");
            if (userProfile)
            {
                auto userModulesPath = std::filesystem::path(userProfile) / ".sad" / "modules";
                if (std::filesystem::exists(userModulesPath))
                {
                    searchPaths_.push_back(userModulesPath);
                }
            }
#else
            const char *homeDir = std::getenv("HOME");
            if (homeDir)
            {
                auto userModulesPath = std::filesystem::path(homeDir) / ".sad" / "modules";
                if (std::filesystem::exists(userModulesPath))
                {
                    searchPaths_.push_back(userModulesPath);
                }
            }
#endif
        }

        // =========================================================================
        // (AR) إدارة مسارات البحث / (EN) Search Path Management
        // =========================================================================

        /**
         * @brief (AR) إضافة مسار بحث جديد
         *        (EN) Add a new search path
         */
        void ModuleResolver::addSearchPath(const std::string &path)
        {
#ifdef _WIN32
            // (AR) على ويندوز، نحوّل UTF-8 إلى wstring لتجنب مشاكل الترميز
            // (EN) On Windows, convert UTF-8 to wstring to avoid encoding issues
            std::filesystem::path fsPath = sad::utf8::make_path(path);
#else
            std::filesystem::path fsPath(path);
#endif

            // (AR) تحقق من وجود المسار / (EN) Check if path exists
            if (!std::filesystem::exists(fsPath))
            {
                return;
            }

            // (AR) تجنب التكرار / (EN) Avoid duplicates
            auto it = std::find(searchPaths_.begin(), searchPaths_.end(), fsPath);
            if (it == searchPaths_.end())
            {
                searchPaths_.push_back(fsPath);
            }
        }

        /**
         * @brief (AR) إضافة مسار بحث مباشرة من filesystem::path
         *        (EN) Add search path directly from filesystem::path
         */
        void ModuleResolver::addSearchPathDirect(const std::filesystem::path &fsPath)
        {
            if (!std::filesystem::exists(fsPath))
            {
                return;
            }
            auto it = std::find(searchPaths_.begin(), searchPaths_.end(), fsPath);
            if (it == searchPaths_.end())
            {
                searchPaths_.push_back(fsPath);
            }
        }

        /**
         * @brief (AR) تعيين مسار المكتبة القياسية
         *        (EN) Set standard library path
         */
        void ModuleResolver::setStdlibPath(const std::string &path)
        {
            stdlibPath_ = std::filesystem::path(path);

            // (AR) إضافة للمسارات إن لم يكن موجوداً / (EN) Add to paths if not present
            auto it = std::find(searchPaths_.begin(), searchPaths_.end(), stdlibPath_);
            if (it == searchPaths_.end())
            {
                searchPaths_.push_back(stdlibPath_);
            }
        }

        // =========================================================================
        // (AR) حل الوحدات / (EN) Module Resolution
        // =========================================================================

        /**
         * @brief (AR) حل وحدة - البحث عنها وتحميلها
         *        (EN) Resolve module - Find and load it
         */
        Module *ModuleResolver::resolveModule(
            const std::vector<std::string> &modulePath,
            const std::string &currentFile)
        {
            // (AR) التحقق من الاستيراد النسبي (يبدأ بـ "." أو "..")
            // (EN) Check for relative import (starts with "." or "..")
            bool isRelative = !modulePath.empty() &&
                              (modulePath[0] == "." || modulePath[0] == "..");

            // (AR) بناء الاسم الكامل / (EN) Build full name
            std::string fullName;
            for (size_t i = 0; i < modulePath.size(); ++i)
            {
                if (i > 0)
                    fullName += ".";
                fullName += modulePath[i];
            }

            // (AR) للاستيراد النسبي، نبني مفتاح cache فريداً بناءً على الملف الحالي
            // (EN) For relative import, build unique cache key based on current file
            std::string cacheKey = fullName;
            if (isRelative && !currentFile.empty())
            {
                cacheKey = currentFile + "::" + fullName;
            }

            // (AR) 1. التحقق من الذاكرة المخبئية / (EN) 1. Check cache
            if (isModuleLoaded(cacheKey))
            {
                return getCachedModule(cacheKey);
            }

            // (AR) 2. اكتشاف الاستيراد الدائري / (EN) 2. Detect circular import
            if (isCircularImport(cacheKey))
            {
                std::cerr << "(AR) خطأ: استيراد دائري! / (EN) Error: Circular import detected!" << std::endl;
                std::cerr << "(AR) المكدس / (EN) Stack: ";
                for (const auto &name : loadingStack_)
                {
                    std::cerr << name << " → ";
                }
                std::cerr << fullName << std::endl;
                return nullptr;
            }

            // (AR) 3. البحث عن ملف الوحدة / (EN) 3. Find module file
            auto filePathOpt = findModuleFile(modulePath, currentFile);
            if (!filePathOpt.has_value())
            {
                std::cerr << "(AR) خطأ: لم يُعثر على الوحدة / (EN) Error: Module not found: "
                          << fullName << std::endl;
                return nullptr;
            }

            // (AR) 4. تحميل الوحدة / (EN) 4. Load module
            return loadModule(filePathOpt.value(), cacheKey);
        }

        /**
         * @brief (AR) البحث عن ملف وحدة
         *        (EN) Find module file
         */
        std::optional<std::filesystem::path> ModuleResolver::findModuleFile(
            const std::vector<std::string> &modulePath,
            const std::string &currentFile)
        {
            // (AR) الامتداد الوحيد المدعوم هو .ص / (EN) Only supported extension is .ص
            std::vector<std::string> extensions = {".\xd8\xb5"};

            // ═══════════════════════════════════════════════════════════════════
            // (AR) استيراد نسبي: المسار يبدأ بـ "." أو ".."
            // (EN) Relative import: path starts with "." or ".."
            // ═══════════════════════════════════════════════════════════════════
            if (!modulePath.empty() && (modulePath[0] == "." || modulePath[0] == "..") && !currentFile.empty())
            {
                auto currentPath = sad::utf8::make_path(currentFile).parent_path();

                // (AR) إذا ".." نصعد مستوى / (EN) If ".." go up one level
                if (modulePath[0] == "..")
                {
                    currentPath = currentPath.parent_path();
                }

                // (AR) بناء المسار النسبي (نتخطى "." أو "..")
                // (EN) Build relative path (skip "." or "..")
                std::string relFilename;
                for (size_t i = 1; i < modulePath.size(); ++i)
                {
                    if (i > 1)
                    {
#ifdef _WIN32
                        relFilename += "\\";
#else
                        relFilename += "/";
#endif
                    }
                    relFilename += modulePath[i];
                }

                for (const auto &ext : extensions)
                {
                    auto filenamePath = sad::utf8::make_path(relFilename + ext);
                    auto fullPath = currentPath / filenamePath;

                    if (std::filesystem::exists(fullPath))
                    {
                        return fullPath;
                    }
                }

                // (AR) بحث في مجلد فرعي / (EN) Search in subdirectory
                if (modulePath.size() > 2)
                {
                    for (const auto &ext : extensions)
                    {
                        auto filenamePath = sad::utf8::make_path(relFilename + ext);
                        auto fullPath = currentPath / filenamePath;
                        if (std::filesystem::exists(fullPath))
                        {
                            return fullPath;
                        }
                    }
                }

                return std::nullopt;
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) استيراد عادي (مطلق) / (EN) Normal (absolute) import
            // ═══════════════════════════════════════════════════════════════════

            // (AR) تحويل مسار الوحدة إلى مسار ملف / (EN) Convert module path to file path
            std::string filename = modulePathToFilename(modulePath);

            // (AR) البحث نسبة للملف الحالي أولاً — الأولوية للملفات المحلية
            // (EN) Search relative to current file FIRST — local files take priority
            // (AR) هذا يتوافق مع سلوك معظم اللغات (Python, Rust, Go):
            //      الملفات في نفس المجلد لها الأولوية على المكتبة القياسية
            // (EN) This matches most languages' behavior (Python, Rust, Go):
            //      files in the same directory take priority over stdlib
            if (!currentFile.empty())
            {
                auto currentPath = sad::utf8::make_path(currentFile).parent_path();

                for (const auto &ext : extensions)
                {
                    auto filenamePath = sad::utf8::make_path(filename + ext);
                    auto fullPath = currentPath / filenamePath;

                    if (std::filesystem::exists(fullPath))
                    {
                        return fullPath;
                    }
                }
            }

            // (AR) البحث في المسارات المسجلة (المكتبة القياسية وغيرها)
            // (EN) Search in registered paths (stdlib and others)
            for (const auto &searchPath : searchPaths_)
            {
                for (const auto &ext : extensions)
                {
                    auto filenamePath = sad::utf8::make_path(filename + ext);
                    auto fullPath = searchPath / filenamePath;

                    if (std::filesystem::exists(fullPath))
                    {
                        return fullPath;
                    }
                }
            }

            return std::nullopt;
        }

        /**
         * @brief (AR) تحميل وحدة من ملف
         *        (EN) Load module from file
         */
        Module *ModuleResolver::loadModule(
            const std::filesystem::path &filePath,
            const std::string &moduleName)
        {
            // (AR) إضافة للمكدس / (EN) Add to stack
            loadingStack_.push_back(moduleName);
            currentlyLoading_.insert(moduleName);

            try
            {
                // (AR) قراءة الملف / (EN) Read file
                // (AR) فتح الملف باستخدام filesystem::path مباشرة لدعم الأسماء العربية
                // (EN) Open file using filesystem::path directly to support Arabic names
                std::ifstream file(filePath);
                if (!file.is_open())
                {
                    throw std::runtime_error("Cannot open module file: " + moduleName);
                }

                std::stringstream buffer;
                buffer << file.rdbuf();
                std::string source = buffer.str();
                file.close();

                // (AR) تحليل الكود / (EN) Parse code
                Lexer::LexerCore lexer(source);
                Parser::ParserCore parser(lexer);

                auto ast = parser.parseProgram();

                if (parser.hasErrors())
                {
                    std::cerr << "(AR) أخطاء في تحليل الوحدة / (EN) Errors parsing module: "
                              << moduleName << std::endl;
                    loadingStack_.pop_back();
                    currentlyLoading_.erase(moduleName);
                    return nullptr;
                }

                // (AR) إنشاء وحدة جديدة / (EN) Create new module
                auto module = std::make_unique<Module>(moduleName);
                module->fullName = moduleName;
                module->filePath = filePath;
                module->ast = std::move(ast);
                // (AR) استخدام wstring على ويندوز لتجنب مشاكل الترميز
                // (EN) Use wstring on Windows to avoid encoding issues
#ifdef _WIN32
                module->isStdlib = (filePath.wstring().find(L"stdlib") != std::wstring::npos);
#else
                module->isStdlib = (filePath.string().find("stdlib") != std::string::npos);
#endif
                module->isLoaded = true;

                // (AR) استخراج الرموز المُصدَّرة / (EN) Extract exported symbols
                extractExports(module.get());

                // (AR) حفظ في الذاكرة المخبئية / (EN) Save to cache
                Module *modulePtr = module.get();
                loadedModules_[moduleName] = std::move(module);

                // (AR) إزالة من المكدس / (EN) Remove from stack
                loadingStack_.pop_back();
                currentlyLoading_.erase(moduleName);

                return modulePtr;
            }
            catch (const std::exception &e)
            {
                std::cerr << "(AR) خطأ في تحميل الوحدة / (EN) Error loading module: "
                          << moduleName << " - " << e.what() << std::endl;

                loadingStack_.pop_back();
                currentlyLoading_.erase(moduleName);
                return nullptr;
            }
        }

        // =========================================================================
        // (AR) إدارة الذاكرة المخبئية / (EN) Cache Management
        // =========================================================================

        /**
         * @brief (AR) الحصول على وحدة محملة
         *        (EN) Get loaded module
         */
        Module *ModuleResolver::getCachedModule(const std::string &fullName) const
        {
            auto it = loadedModules_.find(fullName);
            if (it != loadedModules_.end())
            {
                return it->second.get();
            }
            return nullptr;
        }

        /**
         * @brief (AR) مسح الذاكرة المخبئية
         *        (EN) Clear cache
         */
        void ModuleResolver::clearCache()
        {
            loadedModules_.clear();
            loadingStack_.clear();
            currentlyLoading_.clear();
        }

        // =========================================================================
        // (AR) اكتشاف الاستيرادات الدائرية / (EN) Circular Import Detection
        // =========================================================================

        /**
         * @brief (AR) التحقق من وجود استيراد دائري
         *        (EN) Check for circular import
         */
        bool ModuleResolver::isCircularImport(const std::string &moduleName) const
        {
            return currentlyLoading_.find(moduleName) != currentlyLoading_.end();
        }

        // =========================================================================
        // (AR) دوال مساعدة / (EN) Helper Functions
        // =========================================================================

        /**
         * @brief (AR) تحويل مسار الوحدة إلى اسم ملف
         *        (EN) Convert module path to filename
         */
        std::string ModuleResolver::modulePathToFilename(
            const std::vector<std::string> &modulePath) const
        {
            std::string result;

            for (size_t i = 0; i < modulePath.size(); ++i)
            {
                if (i > 0)
                {
#ifdef _WIN32
                    result += "\\";
#else
                    result += "/";
#endif
                }
                result += modulePath[i];
            }

            return result;
        }

        /**
         * @brief (AR) استخراج الرموز المُصدَّرة من AST
         *        (EN) Extract exported symbols from AST
         */
        void ModuleResolver::extractExports(Module *module)
        {
            if (!module)
                return;

            // (AR) المرور على شجرة AST / (EN) Traverse AST tree
            for (const auto &stmt : module->ast)
            {
                // (AR) البحث عن عقد ExportDecl / (EN) Look for ExportDecl nodes
                if (auto *exportDecl = dynamic_cast<AST::ExportDecl *>(stmt.get()))
                {
                    std::string symbolName = exportDecl->getExportedName();
                    if (!symbolName.empty())
                    {
                        // (AR) إضافة للرموز المُصدَّرة / (EN) Add to exported symbols
                        module->addExport(symbolName, exportDecl->declaration.get());
                    }
                }
            }
        }

        /**
         * @brief (AR) طباعة معلومات تشخيصية
         *        (EN) Print diagnostic information
         */
        void ModuleResolver::printDiagnostics() const
        {
            std::cout << "\n=== (AR) معلومات محلل الوحدات / (EN) Module Resolver Diagnostics ===" << std::endl;

            // (AR) مسارات البحث / (EN) Search paths
            std::cout << "\n(AR) مسارات البحث / (EN) Search Paths:" << std::endl;
            for (const auto &path : searchPaths_)
            {
                std::cout << "  - " << path.string() << std::endl;
            }

            // (AR) الوحدات المحملة / (EN) Loaded modules
            std::cout << "\n(AR) الوحدات المحملة / (EN) Loaded Modules ("
                      << loadedModules_.size() << "):" << std::endl;
            for (const auto &[name, module] : loadedModules_)
            {
                std::cout << "  - " << name << " (" << module->filePath.string() << ")" << std::endl;
                std::cout << "    (AR) مُصدَّرة / (EN) Exported: " << module->exports.size()
                          << " symbols" << std::endl;
            }

            // (AR) مكدس التحميل / (EN) Loading stack
            if (!loadingStack_.empty())
            {
                std::cout << "\n(AR) مكدس التحميل الحالي / (EN) Current Loading Stack:" << std::endl;
                for (const auto &name : loadingStack_)
                {
                    std::cout << "  - " << name << std::endl;
                }
            }

            std::cout << "\n=================================================\n"
                      << std::endl;
        }

    } // namespace Modules
} // namespace Sad
