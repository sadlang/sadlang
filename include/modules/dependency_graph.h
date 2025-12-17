/**
 * @file dependency_graph.h
 * @brief (AR) رسم الاعتماديات بين الوحدات
 *        (EN) Dependency graph between modules
 * 
 * @author Sad Language Development Team
 * @date December 2025
 * @version 2.0.0 - Phase 2
 */

#ifndef SAD_DEPENDENCY_GRAPH_H
#define SAD_DEPENDENCY_GRAPH_H

#include <string>
#include <vector>
#include <set>
#include <map>

namespace Sad {
namespace Modules {

/**
 * @struct DependencyNode
 * @brief (AR) عقدة في رسم الاعتماديات / (EN) Node in dependency graph
 */
struct DependencyNode {
    std::string moduleName;                  ///< (AR) اسم الوحدة / (EN) Module name
    std::vector<std::string> dependencies;   ///< (AR) الاعتماديات / (EN) Dependencies
    int loadOrder;                           ///< (AR) ترتيب التحميل / (EN) Load order
    bool isLoaded;                           ///< (AR) هل تم التحميل؟ / (EN) Is loaded?
    
    DependencyNode() : loadOrder(-1), isLoaded(false) {}
};

/**
 * @class DependencyGraph
 * @brief (AR) رسم الاعتماديات / (EN) Dependency graph
 * 
 * @details
 * (AR) يحلل ويُدير الاعتماديات بين الوحدات:
 *      - حساب ترتيب التحميل (Topological Sort)
 *      - كشف التبعيات الدائرية
 *      - تتبع حالة التحميل
 * 
 * (EN) Analyzes and manages dependencies between modules:
 *      - Calculate load order (Topological Sort)
 *      - Detect circular dependencies
 *      - Track loading state
 */
class DependencyGraph {
public:
    /**
     * @brief (AR) البناء / (EN) Constructor
     */
    DependencyGraph();
    
    /**
     * @brief (AR) إضافة وحدة / (EN) Add module
     * @param moduleName (AR) اسم الوحدة / (EN) Module name
     * @param deps (AR) اعتماديات الوحدة / (EN) Module dependencies
     */
    void addModule(const std::string& moduleName, 
                   const std::vector<std::string>& deps);
    
    /**
     * @brief (AR) حل ترتيب التحميل (Topological Sort)
     *        (EN) Resolve loading order (Topological Sort)
     * @return (AR) قائمة الوحدات بالترتيب الصحيح / (EN) List of modules in correct order
     */
    std::vector<std::string> resolveLoadOrder();
    
    /**
     * @brief (AR) كشف التبعيات الدائرية / (EN) Detect circular dependencies
     * @return true إذا وُجدت تبعية دائرية
     */
    bool hasCircularDependency();
    
    /**
     * @brief (AR) الحصول على المسار الدائري / (EN) Get circular path
     */
    std::vector<std::string> getCircularPath();
    
    /**
     * @brief (AR) الحصول على اعتماديات وحدة / (EN) Get module dependencies
     */
    std::vector<std::string> getDependencies(const std::string& moduleName) const;
    
    /**
     * @brief (AR) الحصول على الوحدات التي تعتمد على وحدة معينة
     *        (EN) Get modules that depend on a specific module
     */
    std::vector<std::string> getDependents(const std::string& moduleName) const;
    
    /**
     * @brief (AR) وضع علامة أن الوحدة تم تحميلها / (EN) Mark module as loaded
     */
    void markAsLoaded(const std::string& moduleName);
    
    /**
     * @brief (AR) هل تم تحميل الوحدة؟ / (EN) Is module loaded?
     */
    bool isLoaded(const std::string& moduleName) const;
    
    /**
     * @brief (AR) مسح الرسم / (EN) Clear graph
     */
    void clear();
    
    /**
     * @brief (AR) طباعة الرسم (للتنقيح) / (EN) Print graph (for debugging)
     */
    void print() const;

private:
    std::map<std::string, DependencyNode> nodes_;  ///< (AR) العُقد / (EN) Nodes
    std::vector<std::string> circularPath_;        ///< (AR) المسار الدائري / (EN) Circular path
    
    /**
     * @brief (AR) خوارزمية DFS للكشف عن الدورات
     *        (EN) DFS algorithm for cycle detection
     */
    bool dfs(const std::string& node,
             std::set<std::string>& visited,
             std::set<std::string>& recStack,
             std::vector<std::string>& path);
    
    /**
     * @brief (AR) خوارزمية DFS للترتيب الطوبولوجي
     *        (EN) DFS algorithm for topological sort
     */
    void topologicalSortDFS(const std::string& node,
                           std::set<std::string>& visited,
                           std::vector<std::string>& stack);
};

} // namespace Modules
} // namespace Sad

#endif // SAD_DEPENDENCY_GRAPH_H
