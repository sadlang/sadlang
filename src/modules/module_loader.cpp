/**
 * @file module_loader.cpp
 * @brief (AR) تنفيذ محمل الوحدات الرئيسي
 *        (EN) Implementation of main module loader
 * 
 * @author Sad Language Development Team
 * @date December 2025
 */

#include "../../include/modules/module_loader.h"
#include <fstream>
#include <iostream>

namespace Sad {
namespace Modules {

// =========================================================================
// ModuleLoader Implementation
// =========================================================================

ModuleLoader& ModuleLoader::getInstance() {
    static ModuleLoader instance;
    return instance;
}

ModuleLoader::ModuleLoader() 
    : pathManager_(SearchPathManager::getInstance()),
      cache_(ModuleCache::getInstance()),
      dependencyGraph_{} {
}

Module* ModuleLoader::loadModule(const std::string& moduleName) {
    // 1. التحقق من الكاش / Check cache
    Module* cached = cache_.get(moduleName);
    if (cached != nullptr) {
        return cached;
    }
    
    // 2. التحقق من الحلقة الدائرية في مكدس التحميل / Check for cycle in loading stack
    if (loadingStack_.find(moduleName) != loadingStack_.end()) {
        std::cerr << "Error: Circular dependency detected while loading " << moduleName << std::endl;
        std::cerr << "Loading stack: ";
        for (const auto& name : loadingStack_) {
            std::cerr << name << " -> ";
        }
        std::cerr << moduleName << " (cycle!)" << std::endl;
        return nullptr;
    }
    
    // 3. إضافة إلى مكدس التحميل / Add to loading stack
    loadingStack_.insert(moduleName);
    
    // 4. البحث عن الملف / Find file
    std::string filePath = pathManager_.findModule(moduleName);
    if (filePath.empty()) {
        std::cerr << "Error: Module not found: " << moduleName << std::endl;
        loadingStack_.erase(moduleName);
        return nullptr;
    }
    
    // 5. تحليل الوحدة / Parse module
    auto module = parseModule(filePath, moduleName);
    if (!module) {
        std::cerr << "Error: Failed to parse module: " << moduleName << std::endl;
        loadingStack_.erase(moduleName);
        return nullptr;
    }
    
    // 6. حل الاعتماديات / Resolve dependencies
    if (!resolveDependencies(module.get())) {
        std::cerr << "Error: Failed to resolve dependencies for: " << moduleName << std::endl;
        loadingStack_.erase(moduleName);
        return nullptr;
    }
    
    // 7. إزالة من مكدس التحميل (التحميل اكتمل بنجاح) / Remove from loading stack (loaded successfully)
    loadingStack_.erase(moduleName);
    
    // 8. إضافة للكاش / Add to cache
    Module* modulePtr = module.get();
    cache_.put(moduleName, std::move(module));
    
    // 9. تعليم كمحمّل في رسم الاعتماديات / Mark as loaded in graph
    dependencyGraph_.markAsLoaded(moduleName);
    
    return modulePtr;
}

Module* ModuleLoader::loadModuleFromFile(const std::string& filePath, 
                                        const std::string& moduleName) {
    // استخراج اسم الوحدة من المسار إذا لم يُحدد
    // Extract module name from path if not specified
    std::string name = moduleName;
    if (name.empty()) {
        std::filesystem::path p(filePath);
        name = p.stem().string();  // اسم الملف بدون الامتداد / Filename without extension
    }
    
    // التحقق من الكاش / Check cache
    Module* cached = cache_.get(name);
    if (cached != nullptr) {
        return cached;
    }
    
    // تحليل / Parse
    auto module = parseModule(filePath, name);
    if (!module) {
        return nullptr;
    }
    
    // حل الاعتماديات / Resolve dependencies
    if (!resolveDependencies(module.get())) {
        return nullptr;
    }
    
    // إضافة للكاش / Add to cache
    Module* modulePtr = module.get();
    cache_.put(name, std::move(module));
    
    return modulePtr;
}

Module* ModuleLoader::getModule(const std::string& moduleName) {
    return cache_.get(moduleName);
}

bool ModuleLoader::isModuleLoaded(const std::string& moduleName) const {
    return cache_.has(moduleName);
}

Module* ModuleLoader::reloadModule(const std::string& moduleName, bool force) {
    // حذف من الكاش / Remove from cache
    if (force || !cache_.isValid(moduleName)) {
        cache_.clearModule(moduleName);
    }
    
    // إعادة التحميل / Reload
    return loadModule(moduleName);
}

void ModuleLoader::clear() {
    cache_.clear();
    dependencyGraph_.clear();
    loadingStack_.clear();
}

void ModuleLoader::printDebugInfo() const {
    std::cout << "\n╔════════════════════════════════════════╗\n";
    std::cout << "║   Module Loader Debug Info             ║\n";
    std::cout << "╚════════════════════════════════════════╝\n\n";
    
    // مسارات البحث / Search paths
    pathManager_.printPaths();
    
    // إحصائيات الكاش / Cache stats
    cache_.printStats();
    
    // رسم الاعتماديات / Dependency graph
    dependencyGraph_.print();
}

std::unique_ptr<Module> ModuleLoader::parseModule(const std::string& filePath, 
                                                  const std::string& moduleName) {
    // قراءة الملف / Read file
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file: " << filePath << std::endl;
        return nullptr;
    }
    
    std::string code((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
    file.close();
    
    // التحليل اللغوي / Lexical analysis
    Lexer::LexerCore lexer(code);
    
    // التحليل النحوي / Parsing
    Parser::ParserCore parser(lexer);
    auto ast = parser.parseProgram();
    
    if (parser.hasErrors()) {
        std::cerr << "Parser errors in " << moduleName << ":\n";
        parser.printErrors();
        return nullptr;
    }
    
    // إنشاء الوحدة / Create module
    auto module = std::make_unique<Module>(moduleName, filePath);
    module->setAST(std::move(ast));
    module->updateMetadata();
    
    return module;
}

bool ModuleLoader::resolveDependencies(Module* module) {
    if (!module) {
        return false;
    }
    
    const auto& deps = module->getDependencies();
    
    // إضافة للرسم / Add to graph
    dependencyGraph_.addModule(module->getName(), deps);
    
    // تحميل كل اعتمادية بشكل متكرر / Load each dependency recursively
    for (const auto& dep : deps) {
        if (!isModuleLoaded(dep)) {
            Module* depModule = loadModule(dep);
            if (!depModule) {
                return false;
            }
        }
    }
    
    // التحقق من التبعيات الدائرية / Check circular dependencies
    if (checkCircularDependencies()) {
        return false;
    }
    
    return true;
}

bool ModuleLoader::checkCircularDependencies() {
    if (dependencyGraph_.hasCircularDependency()) {
        std::cerr << "Error: Circular dependency detected!\n";
        std::cerr << "Circular path:\n";
        
        auto path = dependencyGraph_.getCircularPath();
        for (const auto& node : path) {
            std::cerr << "  -> " << node << "\n";
        }
        
        return true;  // وُجدت دورة / Cycle found
    }
    
    return false;  // لا توجد دورة / No cycle
}

} // namespace Modules
} // namespace Sad
