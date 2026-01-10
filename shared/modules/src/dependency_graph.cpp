/**
 * @file dependency_graph.cpp
 * @brief (AR) تنفيذ رسم الاعتماديات
 *        (EN) Implementation of dependency graph
 * 
 * @author Sad Language Development Team
 * @date December 2025
 */

#include "dependency_graph.h"
#include <algorithm>
#include <iostream>

namespace Sad {
namespace Modules {

// =========================================================================
// DependencyGraph Implementation
// =========================================================================

DependencyGraph::DependencyGraph() : nodes_{}, circularPath_{} {
}

void DependencyGraph::addModule(const std::string& moduleName, 
                                const std::vector<std::string>& deps) {
    DependencyNode node;
    node.moduleName = moduleName;
    node.dependencies = deps;
    node.loadOrder = -1;
    node.isLoaded = false;
    
    nodes_[moduleName] = node;
}

std::vector<std::string> DependencyGraph::resolveLoadOrder() {
    std::vector<std::string> result;
    std::set<std::string> visited;
    
    // Topological sort using DFS
    for (const auto& [nodeName, node] : nodes_) {
        if (visited.find(nodeName) == visited.end()) {
            topologicalSortDFS(nodeName, visited, result);
        }
    }
    
    // Reverse to get correct order (dependencies first)
    std::reverse(result.begin(), result.end());
    
    // تعيين أرقام الترتيب / Assign load order numbers
    for (size_t i = 0; i < result.size(); ++i) {
        if (nodes_.find(result[i]) != nodes_.end()) {
            nodes_[result[i]].loadOrder = static_cast<int>(i);
        }
    }
    
    return result;
}

bool DependencyGraph::hasCircularDependency() {
    circularPath_.clear();
    
    std::set<std::string> visited;
    std::set<std::string> recStack;
    std::vector<std::string> path;
    
    for (const auto& [nodeName, node] : nodes_) {
        if (visited.find(nodeName) == visited.end()) {
            if (dfs(nodeName, visited, recStack, path)) {
                return true;  // وُجدت دورة / Cycle found
            }
        }
    }
    
    return false;
}

std::vector<std::string> DependencyGraph::getCircularPath() {
    return circularPath_;
}

std::vector<std::string> DependencyGraph::getDependencies(const std::string& moduleName) const {
    auto it = nodes_.find(moduleName);
    if (it != nodes_.end()) {
        return it->second.dependencies;
    }
    return {};
}

std::vector<std::string> DependencyGraph::getDependents(const std::string& moduleName) const {
    std::vector<std::string> result;
    
    // البحث عن جميع الوحدات التي تعتمد على moduleName
    // Find all modules that depend on moduleName
    for (const auto& [nodeName, node] : nodes_) {
        auto& deps = node.dependencies;
        if (std::find(deps.begin(), deps.end(), moduleName) != deps.end()) {
            result.push_back(nodeName);
        }
    }
    
    return result;
}

void DependencyGraph::markAsLoaded(const std::string& moduleName) {
    auto it = nodes_.find(moduleName);
    if (it != nodes_.end()) {
        it->second.isLoaded = true;
    }
}

bool DependencyGraph::isLoaded(const std::string& moduleName) const {
    auto it = nodes_.find(moduleName);
    if (it != nodes_.end()) {
        return it->second.isLoaded;
    }
    return false;
}

void DependencyGraph::clear() {
    nodes_.clear();
    circularPath_.clear();
}

void DependencyGraph::print() const {
    std::cout << "\n========================================\n";
    std::cout << "  Dependency Graph (رسم الاعتماديات)\n";
    std::cout << "========================================\n";
    
    for (const auto& [nodeName, node] : nodes_) {
        std::cout << "[" << node.loadOrder << "] " << nodeName;
        if (node.isLoaded) {
            std::cout << " ✓";
        }
        std::cout << "\n";
        
        if (!node.dependencies.empty()) {
            std::cout << "    Dependencies:\n";
            for (const auto& dep : node.dependencies) {
                std::cout << "      - " << dep << "\n";
            }
        }
    }
    
    std::cout << "========================================\n\n";
}

bool DependencyGraph::dfs(const std::string& node,
                         std::set<std::string>& visited,
                         std::set<std::string>& recStack,
                         std::vector<std::string>& path) {
    visited.insert(node);
    recStack.insert(node);
    path.push_back(node);
    
    auto it = nodes_.find(node);
    if (it != nodes_.end()) {
        for (const auto& dep : it->second.dependencies) {
            // إذا لم يُزر / If not visited
            if (visited.find(dep) == visited.end()) {
                if (dfs(dep, visited, recStack, path)) {
                    return true;  // وُجدت دورة / Cycle found
                }
            }
            // إذا كان في recursion stack / If in recursion stack
            else if (recStack.find(dep) != recStack.end()) {
                // وُجدت دورة! / Cycle found!
                path.push_back(dep);
                circularPath_ = path;
                return true;
            }
        }
    }
    
    recStack.erase(node);
    path.pop_back();
    return false;
}

void DependencyGraph::topologicalSortDFS(const std::string& node,
                                        std::set<std::string>& visited,
                                        std::vector<std::string>& stack) {
    visited.insert(node);
    
    auto it = nodes_.find(node);
    if (it != nodes_.end()) {
        for (const auto& dep : it->second.dependencies) {
            if (visited.find(dep) == visited.end()) {
                topologicalSortDFS(dep, visited, stack);
            }
        }
    }
    
    stack.push_back(node);
}

} // namespace Modules
} // namespace Sad
