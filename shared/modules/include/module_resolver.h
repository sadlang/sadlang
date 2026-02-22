/**
 * @file module_resolver.h
 * @brief (AR) محلل الوحدات - يجد ويحمل ملفات الوحدات
 *        (EN) Module Resolver - Finds and loads module files
 * 
 * @details
 * (AR) محلل الوحدات هو المسؤول عن:
 *      1. البحث عن ملفات الوحدات في مسارات متعددة
 *      2. تحميل وتحليل الوحدات
 *      3. إدارة الوحدات المحملة (تجنب التحميل المكرر)
 *      4. حل التبعيات بين الوحدات
 *      5. اكتشاف الاستيرادات الدائرية
 * 
 *      نظام البحث:
 *      - المجلد الحالي (.)
 *      - المكتبة القياسية (stdlib/)
 *      - المسارات المخصصة ($SAD_PATH)
 *      - مجلد المستخدم (~/.sad/modules)
 * 
 * (EN) Module Resolver is responsible for:
 *      1. Searching for module files in multiple paths
 *      2. Loading and parsing modules
 *      3. Managing loaded modules (avoid duplicate loading)
 *      4. Resolving dependencies between modules
 *      5. Detecting circular imports
 * 
 *      Search system:
 *      - Current directory (.)
 *      - Standard library (stdlib/)
 *      - Custom paths ($SAD_PATH)
 *      - User directory (~/.sad/modules)
 * 
 * @author Sad Language Development Team / فريق تطوير لغة ص
 * @date December 2025 / ديسمبر 2025
 * @version 1.0.0 - Phase 1: Basic Module Resolution
 * 
 * @example Basic Usage / الاستخدام الأساسي
 * @code{.cpp}
 * ModuleResolver resolver;
 * resolver.addSearchPath("./mylibs");
 * 
 * // حل وحدة / Resolve a module
 * auto module = resolver.resolveModule({"math"});
 * if (module) {
 *     std::cout << "Module found: " << module->name << std::endl;
 * }
 * @endcode
 */

#ifndef SAD_MODULE_RESOLVER_H
#define SAD_MODULE_RESOLVER_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <filesystem>

namespace Sad {

// Forward declarations
namespace AST {
    class Statement;
    class ImportStmt;
    class FromImportStmt;
    class ExportDecl;
}

namespace Lexer {
    class LexerCore;
}

namespace Parser {
    class ParserCore;
}

namespace Modules {

/**
 * @struct Module
 * @brief (AR) تمثيل لوحدة محملة
 *        (EN) Representation of a loaded module
 * 
 * @details
 * (AR) يحتوي على معلومات الوحدة المحملة:
 *      - الاسم الكامل (مثل: "mylib.utils.math")
 *      - مسار الملف على القرص
 *      - جدول الرموز المُصدَّرة
 *      - شجرة AST الخاصة بالوحدة
 * 
 * (EN) Contains information about a loaded module:
 *      - Full name (e.g., "mylib.utils.math")
 *      - File path on disk
 *      - Exported symbols table
 *      - Module's AST tree
 */
struct Module {
    std::string name;                                    ///< (AR) اسم الوحدة / (EN) Module name
    std::string fullName;                                ///< (AR) الاسم الكامل / (EN) Full name (with dots)
    std::filesystem::path filePath;                      ///< (AR) مسار الملف / (EN) File path
    std::vector<std::unique_ptr<AST::Statement>> ast;    ///< (AR) شجرة AST / (EN) AST tree
    std::unordered_map<std::string, void*> exports;      ///< (AR) الرموز المُصدَّرة / (EN) Exported symbols
    std::vector<std::string> dependencies;               ///< (AR) التبعيات / (EN) Dependencies
    bool isStdlib;                                       ///< (AR) هل مكتبة قياسية / (EN) Is standard library
    bool isLoaded;                                       ///< (AR) هل محملة / (EN) Is loaded
    
    /**
     * @brief (AR) البناء الافتراضي / (EN) Default constructor
     */
    Module() : isStdlib(false), isLoaded(false) {}
    
    /**
     * @brief (AR) بناء مع الاسم / (EN) Constructor with name
     * @param moduleName (AR) اسم الوحدة / (EN) Module name
     */
    explicit Module(const std::string& moduleName)
        : name(moduleName), fullName(moduleName), isStdlib(false), isLoaded(false) {}
    
    /**
     * @brief (AR) إضافة رمز مُصدَّر / (EN) Add exported symbol
     * @param symbolName (AR) اسم الرمز / (EN) Symbol name
     * @param symbolPtr (AR) مؤشر للرمز / (EN) Symbol pointer
     */
    void addExport(const std::string& symbolName, void* symbolPtr) {
        exports[symbolName] = symbolPtr;
    }
    
    /**
     * @brief (AR) البحث عن رمز مُصدَّر / (EN) Find exported symbol
     * @param symbolName (AR) اسم الرمز / (EN) Symbol name
     * @return (AR) المؤشر للرمز أو nullptr / (EN) Pointer to symbol or nullptr
     */
    void* findExport(const std::string& symbolName) const {
        auto it = exports.find(symbolName);
        return (it != exports.end()) ? it->second : nullptr;
    }
    
    /**
     * @brief (AR) هل الرمز مُصدَّر / (EN) Is symbol exported
     * @param symbolName (AR) اسم الرمز / (EN) Symbol name
     * @return (AR) صحيح إن كان مُصدَّر / (EN) True if exported
     */
    bool isExported(const std::string& symbolName) const {
        return exports.find(symbolName) != exports.end();
    }
};

/**
 * @class ModuleResolver
 * @brief (AR) محلل الوحدات - يجد ويحمل الوحدات
 *        (EN) Module Resolver - Finds and loads modules
 * 
 * @details
 * (AR) المحلل النحوي الرئيسي لنظام الوحدات. يوفر:
 *      1. حل أسماء الوحدات إلى ملفات فعلية
 *      2. تحميل وتحليل الوحدات
 *      3. إدارة الذاكرة المخبئية للوحدات المحملة
 *      4. اكتشاف الاستيرادات الدائرية
 *      5. إدارة مسارات البحث
 * 
 * (EN) Main parser for module system. Provides:
 *      1. Resolving module names to actual files
 *      2. Loading and parsing modules
 *      3. Cache management for loaded modules
 *      4. Circular import detection
 *      5. Search path management
 * 
 * @thread_safety
 * (AR) هذا الصنف غير آمن للخيوط المتعددة حالياً. يجب استخدام mutex خارجي.
 * (EN) This class is not thread-safe currently. Use external mutex.
 * 
 * @example Usage Example
 * @code{.cpp}
 * ModuleResolver resolver;
 * 
 * // إضافة مسار بحث / Add search path
 * resolver.addSearchPath("./libs");
 * 
 * // حل وحدة / Resolve module
 * auto module = resolver.resolveModule({"mylib", "utils"});
 * if (module) {
 *     // استخدام الوحدة / Use module
 *     if (module->isExported("myFunction")) {
 *         std::cout << "Function is available!" << std::endl;
 *     }
 * }
 * @endcode
 */
class ModuleResolver {
public:
    /**
     * @brief (AR) البناء - ينشئ محلل وحدات جديد
     *        (EN) Constructor - Creates a new module resolver
     * 
     * @details
     * (AR) يُهيئ مسارات البحث الافتراضية:
     *      1. المجلد الحالي
     *      2. مجلد المكتبة القياسية
     *      3. المسارات من متغير البيئة SAD_PATH
     * 
     * (EN) Initializes default search paths:
     *      1. Current directory
     *      2. Standard library directory
     *      3. Paths from SAD_PATH environment variable
     */
    ModuleResolver();
    
    /**
     * @brief (AR) الهدم / (EN) Destructor
     */
    ~ModuleResolver() = default;
    
    // =====================================================================
    // (AR) إدارة مسارات البحث / (EN) Search Path Management
    // =====================================================================
    
    /**
     * @brief (AR) إضافة مسار بحث / (EN) Add search path
     * @param path (AR) المسار المطلوب إضافته / (EN) Path to add
     * 
     * @details
     * (AR) يضيف مسار جديد لقائمة مسارات البحث عن الوحدات.
     *      سيتم البحث في المسارات بنفس ترتيب إضافتها.
     * 
     * (EN) Adds a new path to module search paths list.
     *      Paths will be searched in the order they were added.
     * 
     * @example
     * @code{.cpp}
     * resolver.addSearchPath("./mylibs");
     * resolver.addSearchPath("/usr/local/sad/modules");
     * @endcode
     */
    void addSearchPath(const std::string& path);
    
    /**
     * @brief (AR) إضافة مسار بحث مباشرة من filesystem::path
     *        (EN) Add search path directly from filesystem::path
     */
    void addSearchPathDirect(const std::filesystem::path& path);
    
    /**
     * @brief (AR) الحصول على جميع مسارات البحث / (EN) Get all search paths
     * @return (AR) قائمة المسارات / (EN) List of paths
     */
    const std::vector<std::filesystem::path>& getSearchPaths() const {
        return searchPaths_;
    }
    
    /**
     * @brief (AR) تعيين مسار المكتبة القياسية / (EN) Set standard library path
     * @param path (AR) مسار المكتبة القياسية / (EN) Standard library path
     */
    void setStdlibPath(const std::string& path);
    
    /**
     * @brief (AR) الحصول على مسار المكتبة القياسية / (EN) Get standard library path
     * @return (AR) المسار / (EN) Path
     */
    const std::filesystem::path& getStdlibPath() const {
        return stdlibPath_;
    }
    
    // =====================================================================
    // (AR) حل الوحدات / (EN) Module Resolution
    // =====================================================================
    
    /**
     * @brief (AR) حل وحدة - البحث عنها وتحميلها / (EN) Resolve module - Find and load it
     * @param modulePath (AR) مسار الوحدة (مثل: ["mylib", "utils"]) / (EN) Module path (e.g., ["mylib", "utils"])
     * @param currentFile (AR) الملف الحالي (للمسارات النسبية) / (EN) Current file (for relative paths)
     * @return (AR) مؤشر للوحدة أو nullptr / (EN) Pointer to module or nullptr
     * 
     * @details
     * (AR) يبحث عن الوحدة في المسارات التالية بالترتيب:
     *      1. نسبة للملف الحالي
     *      2. المجلد الحالي
     *      3. المكتبة القياسية
     *      4. مسارات البحث المخصصة
     * 
     * (EN) Searches for module in the following paths in order:
     *      1. Relative to current file
     *      2. Current directory
     *      3. Standard library
     *      4. Custom search paths
     * 
     * @throws ModuleNotFoundException إذا لم تُوجد الوحدة / if module not found
     * @throws CircularImportException إذا كان هناك استيراد دائري / if circular import detected
     * 
     * @example
     * @code{.cpp}
     * // حل "mylib.utils" / Resolve "mylib.utils"
     * auto module = resolver.resolveModule({"mylib", "utils"});
     * if (module) {
     *     std::cout << "Found: " << module->filePath << std::endl;
     * }
     * @endcode
     */
    Module* resolveModule(const std::vector<std::string>& modulePath,
                         const std::string& currentFile = "");
    
    /**
     * @brief (AR) البحث عن ملف وحدة / (EN) Find module file
     * @param modulePath (AR) مسار الوحدة / (EN) Module path
     * @param currentFile (AR) الملف الحالي / (EN) Current file
     * @return (AR) مسار الملف أو فارغ / (EN) File path or empty
     * 
     * @details
     * (AR) يبحث فقط عن الملف بدون تحميله.
     *      يتحقق من الامتدادات: .ص
     * 
     * (EN) Only searches for file without loading.
     *      Checks extensions: .ص
     */
    std::optional<std::filesystem::path> findModuleFile(
        const std::vector<std::string>& modulePath,
        const std::string& currentFile = ""
    );
    
    /**
     * @brief (AR) تحميل وحدة من ملف / (EN) Load module from file
     * @param filePath (AR) مسار الملف / (EN) File path
     * @param moduleName (AR) اسم الوحدة / (EN) Module name
     * @return (AR) مؤشر للوحدة المحملة / (EN) Pointer to loaded module
     * 
     * @details
     * (AR) يقوم بـ:
     *      1. قراءة الملف
     *      2. تحليل الكود (Lexing & Parsing)
     *      3. استخراج الرموز المُصدَّرة
     *      4. حفظ الوحدة في الذاكرة المخبئية
     * 
     * (EN) Performs:
     *      1. Read file
     *      2. Parse code (Lexing & Parsing)
     *      3. Extract exported symbols
     *      4. Save module in cache
     * 
     * @throws ModuleLoadException إذا فشل التحميل / if loading fails
     */
    Module* loadModule(const std::filesystem::path& filePath,
                      const std::string& moduleName);
    
    // =====================================================================
    // (AR) إدارة الذاكرة المخبئية / (EN) Cache Management
    // =====================================================================
    
    /**
     * @brief (AR) الحصول على وحدة محملة من الذاكرة المخبئية / (EN) Get loaded module from cache
     * @param fullName (AR) الاسم الكامل للوحدة / (EN) Full module name
     * @return (AR) مؤشر للوحدة أو nullptr / (EN) Pointer to module or nullptr
     */
    Module* getCachedModule(const std::string& fullName) const;
    
    /**
     * @brief (AR) هل الوحدة محملة / (EN) Is module loaded
     * @param fullName (AR) الاسم الكامل / (EN) Full name
     * @return (AR) صحيح إن كانت محملة / (EN) True if loaded
     */
    bool isModuleLoaded(const std::string& fullName) const {
        return loadedModules_.find(fullName) != loadedModules_.end();
    }
    
    /**
     * @brief (AR) مسح الذاكرة المخبئية / (EN) Clear cache
     * 
     * @details
     * (AR) يمسح جميع الوحدات المحملة. مفيد للتطوير/الاختبار.
     * (EN) Clears all loaded modules. Useful for development/testing.
     */
    void clearCache();
    
    /**
     * @brief (AR) الحصول على جميع الوحدات المحملة / (EN) Get all loaded modules
     * @return (AR) خريطة الوحدات / (EN) Modules map
     */
    const std::unordered_map<std::string, std::unique_ptr<Module>>& getLoadedModules() const {
        return loadedModules_;
    }
    
    // =====================================================================
    // (AR) اكتشاف الاستيرادات الدائرية / (EN) Circular Import Detection
    // =====================================================================
    
    /**
     * @brief (AR) التحقق من وجود استيراد دائري / (EN) Check for circular import
     * @param moduleName (AR) اسم الوحدة / (EN) Module name
     * @return (AR) صحيح إن كان دائري / (EN) True if circular
     * 
     * @details
     * (AR) يفحص مكدس التحميل الحالي للكشف عن الاستيرادات الدائرية:
     *      A → B → C → A (دائري!)
     * 
     * (EN) Checks current loading stack to detect circular imports:
     *      A → B → C → A (circular!)
     */
    bool isCircularImport(const std::string& moduleName) const;
    
    /**
     * @brief (AR) الحصول على مكدس التحميل الحالي / (EN) Get current loading stack
     * @return (AR) مكدس أسماء الوحدات / (EN) Stack of module names
     */
    const std::vector<std::string>& getLoadingStack() const {
        return loadingStack_;
    }
    
    // =====================================================================
    // (AR) معلومات تشخيصية / (EN) Diagnostic Information
    // =====================================================================
    
    /**
     * @brief (AR) طباعة معلومات تشخيصية / (EN) Print diagnostic information
     * 
     * @details
     * (AR) يطبع:
     *      - مسارات البحث
     *      - الوحدات المحملة
     *      - مكدس التحميل
     * 
     * (EN) Prints:
     *      - Search paths
     *      - Loaded modules
     *      - Loading stack
     */
    void printDiagnostics() const;

private:
    // (AR) مسارات البحث / (EN) Search paths
    std::vector<std::filesystem::path> searchPaths_;
    std::filesystem::path stdlibPath_;
    
    // (AR) الوحدات المحملة / (EN) Loaded modules
    std::unordered_map<std::string, std::unique_ptr<Module>> loadedModules_;
    
    // (AR) مكدس التحميل (لاكتشاف الدوائر) / (EN) Loading stack (for circular detection)
    std::vector<std::string> loadingStack_;
    
    // (AR) الوحدات قيد التحميل / (EN) Modules currently loading
    std::unordered_set<std::string> currentlyLoading_;
    
    /**
     * @brief (AR) تهيئة مسارات البحث الافتراضية / (EN) Initialize default search paths
     */
    void initializeDefaultPaths();
    
    /**
     * @brief (AR) تحويل مسار الوحدة إلى اسم ملف / (EN) Convert module path to filename
     * @param modulePath (AR) مسار الوحدة / (EN) Module path
     * @return (AR) اسم الملف / (EN) Filename
     * 
     * @example
     * ["mylib", "utils"] → "mylib/utils.ص"
     */
    std::string modulePathToFilename(const std::vector<std::string>& modulePath) const;
    
    /**
     * @brief (AR) استخراج الرموز المُصدَّرة من AST / (EN) Extract exported symbols from AST
     * @param module (AR) الوحدة / (EN) Module
     * 
     * @details
     * (AR) يبحث في شجرة AST عن عقد ExportDecl ويستخرج الرموز.
     * (EN) Searches AST tree for ExportDecl nodes and extracts symbols.
     */
    void extractExports(Module* module);
};

} // namespace Modules
} // namespace Sad

#endif // SAD_MODULE_RESOLVER_H
