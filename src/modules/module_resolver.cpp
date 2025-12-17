/**
 * @file module_resolver.cpp
 * @brief (AR) تنفيذ محلل الوحدات - يجد ويحمل ملفات الوحدات
 *        (EN) Implementation of Module Resolver - Finds and loads module files
 * 
 * @author Sad Language Development Team / فريق تطوير لغة ص
 * @date December 2025 / ديسمبر 2025
 * @version 1.0.0 - Phase 1: Basic Module Resolution
 */

#include "../../include/modules/module_resolver.h"
#include "../../include/parser/parser_core.h"
#include "../../include/lexer/lexer_core.h"
#include "../../include/parser/ast/module_nodes.h"
#include "../../include/parser/ast/declarations.h"

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

namespace Sad {
namespace Modules {

// =========================================================================
// (AR) البناء والهدم / (EN) Constructor and Destructor
// =========================================================================

/**
 * @brief (AR) البناء - يُهيئ مسارات البحث الافتراضية
 *        (EN) Constructor - Initializes default search paths
 */
ModuleResolver::ModuleResolver() {
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
void ModuleResolver::initializeDefaultPaths() {
    // (AR) 1. المجلد الحالي / (EN) 1. Current directory
    searchPaths_.push_back(std::filesystem::current_path());
    
    // (AR) 2. المكتبة القياسية / (EN) 2. Standard library
    // افتراض أن المكتبة القياسية في مجلد stdlib بجوار البرنامج
    // Assume stdlib is in a stdlib folder next to the executable
    auto exePath = std::filesystem::current_path();
    stdlibPath_ = exePath / "stdlib";
    
    if (std::filesystem::exists(stdlibPath_)) {
        searchPaths_.push_back(stdlibPath_);
    }
    
    // (AR) 3. متغير البيئة SAD_PATH / (EN) 3. SAD_PATH environment variable
    const char* sadPath = std::getenv("SAD_PATH");
    if (sadPath) {
        std::string pathStr(sadPath);
        std::istringstream iss(pathStr);
        std::string path;
        
#ifdef _WIN32
        const char delimiter = ';';
#else
        const char delimiter = ':';
#endif
        
        while (std::getline(iss, path, delimiter)) {
            if (!path.empty() && std::filesystem::exists(path)) {
                searchPaths_.push_back(std::filesystem::path(path));
            }
        }
    }
    
    // (AR) 4. مجلد المستخدم / (EN) 4. User directory
#ifdef _WIN32
    const char* userProfile = std::getenv("USERPROFILE");
    if (userProfile) {
        auto userModulesPath = std::filesystem::path(userProfile) / ".sad" / "modules";
        if (std::filesystem::exists(userModulesPath)) {
            searchPaths_.push_back(userModulesPath);
        }
    }
#else
    const char* homeDir = std::getenv("HOME");
    if (homeDir) {
        auto userModulesPath = std::filesystem::path(homeDir) / ".sad" / "modules";
        if (std::filesystem::exists(userModulesPath)) {
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
void ModuleResolver::addSearchPath(const std::string& path) {
    std::filesystem::path fsPath(path);
    
    // (AR) تحقق من وجود المسار / (EN) Check if path exists
    if (!std::filesystem::exists(fsPath)) {
        std::cerr << "(AR) تحذير: المسار غير موجود: / (EN) Warning: Path does not exist: " 
                  << path << std::endl;
        return;
    }
    
    // (AR) تجنب التكرار / (EN) Avoid duplicates
    auto it = std::find(searchPaths_.begin(), searchPaths_.end(), fsPath);
    if (it == searchPaths_.end()) {
        searchPaths_.push_back(fsPath);
    }
}

/**
 * @brief (AR) تعيين مسار المكتبة القياسية
 *        (EN) Set standard library path
 */
void ModuleResolver::setStdlibPath(const std::string& path) {
    stdlibPath_ = std::filesystem::path(path);
    
    // (AR) إضافة للمسارات إن لم يكن موجوداً / (EN) Add to paths if not present
    auto it = std::find(searchPaths_.begin(), searchPaths_.end(), stdlibPath_);
    if (it == searchPaths_.end()) {
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
Module* ModuleResolver::resolveModule(
    const std::vector<std::string>& modulePath,
    const std::string& currentFile
) {
    // (AR) بناء الاسم الكامل / (EN) Build full name
    std::string fullName;
    for (size_t i = 0; i < modulePath.size(); ++i) {
        if (i > 0) fullName += ".";
        fullName += modulePath[i];
    }
    
    // (AR) 1. التحقق من الذاكرة المخبئية / (EN) 1. Check cache
    if (isModuleLoaded(fullName)) {
        return getCachedModule(fullName);
    }
    
    // (AR) 2. اكتشاف الاستيراد الدائري / (EN) 2. Detect circular import
    if (isCircularImport(fullName)) {
        std::cerr << "(AR) خطأ: استيراد دائري! / (EN) Error: Circular import detected!" << std::endl;
        std::cerr << "(AR) المكدس / (EN) Stack: ";
        for (const auto& name : loadingStack_) {
            std::cerr << name << " → ";
        }
        std::cerr << fullName << std::endl;
        return nullptr;
    }
    
    // (AR) 3. البحث عن ملف الوحدة / (EN) 3. Find module file
    auto filePathOpt = findModuleFile(modulePath, currentFile);
    if (!filePathOpt.has_value()) {
        std::cerr << "(AR) خطأ: لم يُعثر على الوحدة / (EN) Error: Module not found: " 
                  << fullName << std::endl;
        return nullptr;
    }
    
    // (AR) 4. تحميل الوحدة / (EN) 4. Load module
    return loadModule(filePathOpt.value(), fullName);
}

/**
 * @brief (AR) البحث عن ملف وحدة
 *        (EN) Find module file
 */
std::optional<std::filesystem::path> ModuleResolver::findModuleFile(
    const std::vector<std::string>& modulePath,
    const std::string& currentFile
) {
    // (AR) تحويل مسار الوحدة إلى مسار ملف / (EN) Convert module path to file path
    std::string filename = modulePathToFilename(modulePath);
    
    // (AR) الامتدادات المدعومة / (EN) Supported extensions
    std::vector<std::string> extensions = {".sad", ".sd"};
    
    // (AR) البحث في المسارات / (EN) Search in paths
    for (const auto& searchPath : searchPaths_) {
        for (const auto& ext : extensions) {
            auto fullPath = searchPath / (filename + ext);
            
            if (std::filesystem::exists(fullPath)) {
                return fullPath;
            }
        }
    }
    
    // (AR) البحث نسبة للملف الحالي / (EN) Search relative to current file
    if (!currentFile.empty()) {
        auto currentPath = std::filesystem::path(currentFile).parent_path();
        
        for (const auto& ext : extensions) {
            auto fullPath = currentPath / (filename + ext);
            
            if (std::filesystem::exists(fullPath)) {
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
Module* ModuleResolver::loadModule(
    const std::filesystem::path& filePath,
    const std::string& moduleName
) {
    // (AR) إضافة للمكدس / (EN) Add to stack
    loadingStack_.push_back(moduleName);
    currentlyLoading_.insert(moduleName);
    
    try {
        // (AR) قراءة الملف / (EN) Read file
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filePath.string());
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();
        file.close();
        
        // (AR) تحليل الكود / (EN) Parse code
        Lexer::LexerCore lexer(source, filePath.string());
        Parser::ParserCore parser(lexer);
        
        auto ast = parser.parseProgram();
        
        if (parser.hasErrors()) {
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
        module->isStdlib = (filePath.string().find("stdlib") != std::string::npos);
        module->isLoaded = true;
        
        // (AR) استخراج الرموز المُصدَّرة / (EN) Extract exported symbols
        extractExports(module.get());
        
        // (AR) حفظ في الذاكرة المخبئية / (EN) Save to cache
        Module* modulePtr = module.get();
        loadedModules_[moduleName] = std::move(module);
        
        // (AR) إزالة من المكدس / (EN) Remove from stack
        loadingStack_.pop_back();
        currentlyLoading_.erase(moduleName);
        
        return modulePtr;
        
    } catch (const std::exception& e) {
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
Module* ModuleResolver::getCachedModule(const std::string& fullName) const {
    auto it = loadedModules_.find(fullName);
    if (it != loadedModules_.end()) {
        return it->second.get();
    }
    return nullptr;
}

/**
 * @brief (AR) مسح الذاكرة المخبئية
 *        (EN) Clear cache
 */
void ModuleResolver::clearCache() {
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
bool ModuleResolver::isCircularImport(const std::string& moduleName) const {
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
    const std::vector<std::string>& modulePath
) const {
    std::string result;
    
    for (size_t i = 0; i < modulePath.size(); ++i) {
        if (i > 0) {
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
void ModuleResolver::extractExports(Module* module) {
    if (!module) return;
    
    // (AR) المرور على شجرة AST / (EN) Traverse AST tree
    for (const auto& stmt : module->ast) {
        // (AR) البحث عن عقد ExportDecl / (EN) Look for ExportDecl nodes
        if (auto* exportDecl = dynamic_cast<AST::ExportDecl*>(stmt.get())) {
            std::string symbolName = exportDecl->getExportedName();
            if (!symbolName.empty()) {
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
void ModuleResolver::printDiagnostics() const {
    std::cout << "\n=== (AR) معلومات محلل الوحدات / (EN) Module Resolver Diagnostics ===" << std::endl;
    
    // (AR) مسارات البحث / (EN) Search paths
    std::cout << "\n(AR) مسارات البحث / (EN) Search Paths:" << std::endl;
    for (const auto& path : searchPaths_) {
        std::cout << "  - " << path.string() << std::endl;
    }
    
    // (AR) الوحدات المحملة / (EN) Loaded modules
    std::cout << "\n(AR) الوحدات المحملة / (EN) Loaded Modules (" 
              << loadedModules_.size() << "):" << std::endl;
    for (const auto& [name, module] : loadedModules_) {
        std::cout << "  - " << name << " (" << module->filePath.string() << ")" << std::endl;
        std::cout << "    (AR) مُصدَّرة / (EN) Exported: " << module->exports.size() 
                  << " symbols" << std::endl;
    }
    
    // (AR) مكدس التحميل / (EN) Loading stack
    if (!loadingStack_.empty()) {
        std::cout << "\n(AR) مكدس التحميل الحالي / (EN) Current Loading Stack:" << std::endl;
        for (const auto& name : loadingStack_) {
            std::cout << "  - " << name << std::endl;
        }
    }
    
    std::cout << "\n=================================================\n" << std::endl;
}

} // namespace Modules
} // namespace Sad
