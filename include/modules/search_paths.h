/**
 * @file search_paths.h
 * @brief (AR) مدير مسارات البحث عن الوحدات
 *        (EN) Module search path manager
 * 
 * @details
 * (AR) يدير قائمة المسارات التي يبحث فيها النظام عن الوحدات المُستوردة.
 *      يدعم:
 *      - أولويات المسارات
 *      - التحميل من ملف تكوين
 *      - التحميل من متغيرات البيئة
 *      - مسارات النظام والمكتبة القياسية
 * 
 * (EN) Manages the list of paths where the system searches for imported modules.
 *      Supports:
 *      - Path priorities
 *      - Loading from config file
 *      - Loading from environment variables
 *      - System paths and standard library
 * 
 * @author Sad Language Development Team
 * @date December 2025
 * @version 2.0.0 - Phase 2: Module Loader
 */

#ifndef SAD_SEARCH_PATHS_H
#define SAD_SEARCH_PATHS_H

#include <string>
#include <vector>
#include <filesystem>

namespace Sad {
namespace Modules {

/**
 * @class SearchPathManager
 * @brief (AR) مدير مسارات البحث / (EN) Search path manager
 * 
 * @details
 * (AR) Singleton يدير جميع مسارات البحث عن الوحدات.
 *      ترتيب البحث:
 *      1. الدليل الحالي
 *      2. دليل المشروع (./lib, ./modules)
 *      3. المكتبة القياسية (SAD_HOME/stdlib)
 *      4. مسارات من التكوين
 *      5. متغير البيئة SAD_PATH
 *      6. مسارات النظام
 * 
 * (EN) Singleton managing all module search paths.
 *      Search order:
 *      1. Current directory
 *      2. Project directory (./lib, ./modules)
 *      3. Standard library (SAD_HOME/stdlib)
 *      4. Paths from config
 *      5. Environment variable SAD_PATH
 *      6. System paths
 */
class SearchPathManager {
public:
    /**
     * @brief (AR) الحصول على Instance (Singleton)
     *        (EN) Get instance (Singleton)
     */
    static SearchPathManager& getInstance();
    
    // Delete copy constructor and assignment
    SearchPathManager(const SearchPathManager&) = delete;
    SearchPathManager& operator=(const SearchPathManager&) = delete;
    
    /**
     * @brief (AR) إضافة مسار بحث / (EN) Add search path
     * @param path (AR) المسار المطلق / (EN) Absolute path
     * @param priority (AR) الأولوية (أعلى = يُفحص أولاً) / (EN) Priority (higher = checked first)
     */
    void addPath(const std::string& path, int priority = 50);
    
    /**
     * @brief (AR) إزالة مسار / (EN) Remove path
     */
    void removePath(const std::string& path);
    
    /**
     * @brief (AR) مسح جميع المسارات / (EN) Clear all paths
     */
    void clear();
    
    /**
     * @brief (AR) الحصول على جميع المسارات مرتبة حسب الأولوية
     *        (EN) Get all paths sorted by priority
     * @return (AR) قائمة المسارات / (EN) List of paths
     */
    std::vector<std::string> getPaths() const;
    
    /**
     * @brief (AR) البحث عن وحدة / (EN) Search for module
     * @param moduleName (AR) اسم الوحدة (قد يكون متداخلاً: "math.advanced")
     *                   (EN) Module name (may be nested: "math.advanced")
     * @return (AR) المسار الكامل للملف أو "" إذا لم يُوجد
     *         (EN) Full file path or "" if not found
     */
    std::string findModule(const std::string& moduleName) const;
    
    /**
     * @brief (AR) تحميل مسارات من ملف التكوين
     *        (EN) Load paths from config file
     * @param configFile (AR) مسار ملف sad.config.json / (EN) Path to sad.config.json
     */
    void loadFromConfig(const std::string& configFile);
    
    /**
     * @brief (AR) تحميل من متغير البيئة SAD_PATH
     *        (EN) Load from environment variable SAD_PATH
     */
    void loadFromEnvironment();
    
    /**
     * @brief (AR) إضافة المسارات الافتراضية (المكتبة القياسية، إلخ)
     *        (EN) Add default paths (standard library, etc.)
     */
    void addDefaultPaths();
    
    /**
     * @brief (AR) طباعة جميع المسارات (للتنقيح)
     *        (EN) Print all paths (for debugging)
     */
    void printPaths() const;

private:
    /**
     * @brief (AR) البناء الخاص / (EN) Private constructor
     */
    SearchPathManager();
    
    /**
     * @brief (AR) إدخال مسار / (EN) Path entry
     */
    struct PathEntry {
        std::string path;
        int priority;
        
        bool operator<(const PathEntry& other) const {
            // Higher priority first
            return priority > other.priority;
        }
    };
    
    std::vector<PathEntry> paths_;  ///< (AR) قائمة المسارات / (EN) List of paths
    
    /**
     * @brief (AR) التهيئة الأولية / (EN) Initial setup
     */
    void initialize();
    
    /**
     * @brief (AR) محاولة إيجاد الملف في مسار معين
     *        (EN) Try to find file in specific path
     * @param basePath (AR) المسار الأساسي / (EN) Base path
     * @param moduleName (AR) اسم الوحدة / (EN) Module name
     * @return (AR) المسار الكامل أو "" / (EN) Full path or ""
     */
    std::string tryFindInPath(const std::string& basePath, const std::string& moduleName) const;
    
    /**
     * @brief (AR) تحويل اسم الوحدة إلى مسار نسبي
     *        (EN) Convert module name to relative path
     * @param moduleName "math.advanced" → "math/advanced"
     */
    std::string moduleNameToPath(const std::string& moduleName) const;
};

} // namespace Modules
} // namespace Sad

#endif // SAD_SEARCH_PATHS_H
