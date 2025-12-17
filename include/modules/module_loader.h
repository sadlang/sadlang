/**
 * @file module_loader.h
 * @brief (AR) محمل الوحدات الرئيسي
 *        (EN) Main module loader
 * 
 * @details
 * (AR) المحمل الرئيسي الذي يُنسّق بين جميع المكونات:
 *      - البحث عن الوحدات (SearchPathManager)
 *      - التخزين المؤقت (ModuleCache)
 *      - تحليل الاعتماديات (DependencyGraph)
 *      - التحليل النحوي (Parser)
 * 
 * (EN) Main loader coordinating all components:
 *      - Module search (SearchPathManager)
 *      - Caching (ModuleCache)
 *      - Dependency analysis (DependencyGraph)
 *      - Parsing (Parser)
 * 
 * @author Sad Language Development Team
 * @date December 2025
 * @version 2.0.0 - Phase 2: Module Loader
 */

#ifndef SAD_MODULE_LOADER_H
#define SAD_MODULE_LOADER_H

#include "module.h"
#include "search_paths.h"
#include "module_cache.h"
#include "dependency_graph.h"
#include "../parser/parser_core.h"
#include "../lexer/lexer_core.h"
#include <memory>
#include <vector>
#include <set>

namespace Sad {
namespace Modules {

/**
 * @class ModuleLoader
 * @brief (AR) محمل الوحدات الذكي / (EN) Smart module loader
 * 
 * @details
 * (AR) Singleton يُدير عملية تحميل الوحدات بالكامل:
 *      1. البحث عن ملف الوحدة
 *      2. التحقق من الكاش
 *      3. التحليل النحوي
 *      4. تحليل الاعتماديات
 *      5. حل ترتيب التحميل
 *      6. تحميل الوحدات بالترتيب الصحيح
 * 
 * (EN) Singleton managing the complete module loading process:
 *      1. Find module file
 *      2. Check cache
 *      3. Parse
 *      4. Analyze dependencies
 *      5. Resolve load order
 *      6. Load modules in correct order
 */
class ModuleLoader {
public:
    /**
     * @brief (AR) الحصول على Instance / (EN) Get instance
     */
    static ModuleLoader& getInstance();
    
    // Delete copy
    ModuleLoader(const ModuleLoader&) = delete;
    ModuleLoader& operator=(const ModuleLoader&) = delete;
    
    /**
     * @brief (AR) تحميل وحدة / (EN) Load module
     * @param moduleName (AR) اسم الوحدة / (EN) Module name
     * @return Module* أو nullptr إذا فشل التحميل
     */
    Module* loadModule(const std::string& moduleName);
    
    /**
     * @brief (AR) تحميل وحدة من مسار مباشر / (EN) Load module from direct path
     * @param filePath (AR) مسار الملف / (EN) File path
     * @param moduleName (AR) اسم الوحدة (اختياري) / (EN) Module name (optional)
     * @return Module* أو nullptr
     */
    Module* loadModuleFromFile(const std::string& filePath, 
                               const std::string& moduleName = "");
    
    /**
     * @brief (AR) الحصول على وحدة محمّلة / (EN) Get loaded module
     * @param moduleName (AR) اسم الوحدة / (EN) Module name
     * @return Module* أو nullptr
     */
    Module* getModule(const std::string& moduleName);
    
    /**
     * @brief (AR) هل الوحدة محمّلة؟ / (EN) Is module loaded?
     */
    bool isModuleLoaded(const std::string& moduleName) const;
    
    /**
     * @brief (AR) إعادة تحميل وحدة / (EN) Reload module
     * @param moduleName (AR) اسم الوحدة / (EN) Module name
     * @param force (AR) إجبار إعادة التحميل حتى لو لم يتغير / (EN) Force reload even if unchanged
     * @return Module* أو nullptr
     */
    Module* reloadModule(const std::string& moduleName, bool force = false);
    
    /**
     * @brief (AR) الحصول على رسم الاعتماديات / (EN) Get dependency graph
     */
    DependencyGraph& getDependencyGraph() { return dependencyGraph_; }
    
    /**
     * @brief (AR) مسح جميع الوحدات المحمّلة / (EN) Clear all loaded modules
     */
    void clear();
    
    /**
     * @brief (AR) طباعة معلومات التنقيح / (EN) Print debug info
     */
    void printDebugInfo() const;

private:
    ModuleLoader();
    
    SearchPathManager& pathManager_;    ///< (AR) مدير المسارات / (EN) Path manager
    ModuleCache& cache_;                ///< (AR) الكاش / (EN) Cache
    DependencyGraph dependencyGraph_;   ///< (AR) رسم الاعتماديات / (EN) Dependency graph
    std::set<std::string> loadingStack_;  ///< (AR) مكدس الوحدات قيد التحميل / (EN) Stack of modules being loaded
    
    /**
     * @brief (AR) تحليل وحدة من AST / (EN) Parse module from AST
     */
    std::unique_ptr<Module> parseModule(const std::string& filePath, 
                                       const std::string& moduleName);
    
    /**
     * @brief (AR) حل جميع الاعتماديات بشكل متكرر / (EN) Resolve all dependencies recursively
     */
    bool resolveDependencies(Module* module);
    
    /**
     * @brief (AR) التحقق من التبعيات الدائرية / (EN) Check for circular dependencies
     */
    bool checkCircularDependencies();
};

} // namespace Modules
} // namespace Sad

#endif // SAD_MODULE_LOADER_H
