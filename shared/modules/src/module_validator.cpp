/**
 * @file module_validator.cpp
 * @brief (AR) تنفيذ محقق صحة الوحدات - Phase 4
 *        (EN) Module validator implementation - Phase 4
 * 
 * @author Sad Language Development Team
 * @date December 2025
 */

#include "module_validator.h"
#include "statements.h"
#include "declarations.h"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace Sad {
namespace Modules {

// =========================================================================
// ModuleDiagnostic Implementation
// =========================================================================

std::string ModuleDiagnostic::toString() const {
    std::stringstream ss;
    
    // Severity icon
    switch (severity) {
        case DiagnosticSeverity::ERROR:
            ss << "❌ خطأ / Error";
            break;
        case DiagnosticSeverity::WARNING:
            ss << "⚠️  تحذير / Warning";
            break;
        case DiagnosticSeverity::INFO:
            ss << "ℹ️  معلومة / Info";
            break;
    }
    
    ss << " [" << moduleName << "]";
    
    if (line >= 0) {
        ss << " (السطر / Line " << line;
        if (column >= 0) {
            ss << ":" << column;
        }
        ss << ")";
    }
    
    ss << "\n";
    if (!messageAr.empty()) {
        ss << "  (AR) " << messageAr << "\n";
    }
    if (!messageEn.empty()) {
        ss << "  (EN) " << messageEn;
    }
    
    return ss.str();
}

// =========================================================================
// ValidationResult Implementation
// =========================================================================

void ValidationResult::addError(const std::string& moduleName,
                                const std::string& messageEn,
                                const std::string& messageAr,
                                int line, int column) {
    success = false;
    ModuleDiagnostic diag;
    diag.severity = DiagnosticSeverity::ERROR;
    diag.moduleName = moduleName;
    diag.messageEn = messageEn;
    diag.messageAr = messageAr.empty() ? messageEn : messageAr;
    diag.line = line;
    diag.column = column;
    diagnostics.push_back(diag);
}

void ValidationResult::addWarning(const std::string& moduleName,
                                  const std::string& messageEn,
                                  const std::string& messageAr,
                                  int line, int column) {
    ModuleDiagnostic diag;
    diag.severity = DiagnosticSeverity::WARNING;
    diag.moduleName = moduleName;
    diag.messageEn = messageEn;
    diag.messageAr = messageAr.empty() ? messageEn : messageAr;
    diag.line = line;
    diag.column = column;
    diagnostics.push_back(diag);
}

void ValidationResult::addInfo(const std::string& moduleName,
                               const std::string& messageEn,
                               const std::string& messageAr) {
    ModuleDiagnostic diag;
    diag.severity = DiagnosticSeverity::INFO;
    diag.moduleName = moduleName;
    diag.messageEn = messageEn;
    diag.messageAr = messageAr.empty() ? messageEn : messageAr;
    diagnostics.push_back(diag);
}

bool ValidationResult::hasErrors() const {
    return std::any_of(diagnostics.begin(), diagnostics.end(),
                      [](const ModuleDiagnostic& d) { return d.isError(); });
}

size_t ValidationResult::errorCount() const {
    return std::count_if(diagnostics.begin(), diagnostics.end(),
                        [](const ModuleDiagnostic& d) { return d.isError(); });
}

size_t ValidationResult::warningCount() const {
    return std::count_if(diagnostics.begin(), diagnostics.end(),
                        [](const ModuleDiagnostic& d) { return d.isWarning(); });
}

void ValidationResult::print() const {
    std::cout << "\n========================================\n";
    std::cout << "  📋 نتيجة التحقق / Validation Result\n";
    std::cout << "========================================\n";
    
    if (diagnostics.empty()) {
        std::cout << "✅ لا توجد مشاكل / No issues found\n";
        return;
    }
    
    // Print all diagnostics
    for (const auto& diag : diagnostics) {
        std::cout << "\n" << diag.toString() << "\n";
    }
    
    // Summary
    std::cout << "\n========================================\n";
    std::cout << "📊 الملخص / Summary:\n";
    std::cout << "  - أخطاء / Errors: " << errorCount() << "\n";
    std::cout << "  - تحذيرات / Warnings: " << warningCount() << "\n";
    std::cout << "  - النتيجة / Result: " 
              << (success ? "✅ نجح / Passed" : "❌ فشل / Failed") << "\n";
    std::cout << "========================================\n\n";
}

// =========================================================================
// ModuleValidator Implementation
// =========================================================================

ModuleValidator::ModuleValidator() = default;
ModuleValidator::~ModuleValidator() = default;

ValidationResult ModuleValidator::validateModule(const Module* module) {
    ValidationResult result;
    
    if (!module) {
        result.addError("unknown", 
                       "Module pointer is null",
                       "مؤشر الوحدة فارغ");
        return result;
    }
    
    // 1. Validate module name
    if (!isValidModuleName(module->getName())) {
        result.addError(module->getName(),
                       "Invalid module name: " + module->getName(),
                       "اسم وحدة غير صالح: " + module->getName());
    }
    
    // 2. Validate AST
    auto astResult = validateAST(module);
    result.diagnostics.insert(result.diagnostics.end(),
                             astResult.diagnostics.begin(),
                             astResult.diagnostics.end());
    if (astResult.hasErrors()) result.success = false;
    
    // 3. Validate exports
    auto exportsResult = validateExports(module);
    result.diagnostics.insert(result.diagnostics.end(),
                             exportsResult.diagnostics.begin(),
                             exportsResult.diagnostics.end());
    if (exportsResult.hasErrors()) result.success = false;
    
    // 4. Check for duplicates
    auto dupsResult = checkDuplicates(module);
    result.diagnostics.insert(result.diagnostics.end(),
                             dupsResult.diagnostics.begin(),
                             dupsResult.diagnostics.end());
    if (dupsResult.hasErrors()) result.success = false;
    
    // 5. Validate imports (basic check without loader)
    auto importsResult = validateImports(module);
    result.diagnostics.insert(result.diagnostics.end(),
                             importsResult.diagnostics.begin(),
                             importsResult.diagnostics.end());
    if (importsResult.hasErrors()) result.success = false;
    
    return result;
}

ValidationResult ModuleValidator::validateModules(
    const std::vector<const Module*>& modules) {
    
    ValidationResult result;
    
    // Validate each module individually
    for (const auto* module : modules) {
        auto moduleResult = validateModule(module);
        result.diagnostics.insert(result.diagnostics.end(),
                                 moduleResult.diagnostics.begin(),
                                 moduleResult.diagnostics.end());
        if (moduleResult.hasErrors()) result.success = false;
    }
    
    // Check for cyclic dependencies across all modules
    std::set<std::string> visited;
    std::set<std::string> stack;
    
    for (const auto* module : modules) {
        if (visited.find(module->getName()) == visited.end()) {
            auto cycle = detectCycle(module, visited, stack);
            if (cycle.has_value()) {
                std::string cyclePath;
                for (size_t i = 0; i < cycle->size(); ++i) {
                    cyclePath += (*cycle)[i];
                    if (i < cycle->size() - 1) cyclePath += " -> ";
                }
                
                result.addError(module->getName(),
                               "Cyclic dependency detected: " + cyclePath,
                               "تبعية دائرية: " + cyclePath);
            }
        }
    }
    
    return result;
}

std::optional<std::vector<std::string>> ModuleValidator::detectCycle(
    const Module* module,
    std::set<std::string>& visited,
    std::set<std::string>& stack) {
    
    if (!module) return std::nullopt;
    
    std::vector<std::string> path;
    
    if (detectCycleHelper(module, visited, stack, path)) {
        return path;
    }
    
    return std::nullopt;
}

bool ModuleValidator::detectCycleHelper(
    const Module* module,
    std::set<std::string>& visited,
    std::set<std::string>& stack,
    std::vector<std::string>& path) {
    
    const std::string& name = module->getName();
    
    // Already in stack = cycle found
    if (stack.find(name) != stack.end()) {
        path.push_back(name);
        return true;
    }
    
    // Already visited (not in cycle)
    if (visited.find(name) != visited.end()) {
        return false;
    }
    
    // Mark as visited and add to stack
    visited.insert(name);
    stack.insert(name);
    path.push_back(name);
    
    // Check all dependencies
    const auto& deps = module->getDependencies();
    for (const auto& depName : deps) {
        // Try to load the dependency module
        Module* depModule = ModuleLoader::getInstance().loadModule(depName);
        if (depModule) {
            // Recursively check for cycles
            if (detectCycleHelper(depModule, visited, stack, path)) {
                return true;
            }
        }
    }
    
    // Remove from stack
    stack.erase(name);
    path.pop_back();
    
    return false;
}

ValidationResult ModuleValidator::validateImports(const Module* module) {
    ValidationResult result;
    
    const auto& deps = module->getDependencies();
    
    for (const auto& depName : deps) {
        // Check if module name is valid
        if (!isValidModuleName(depName)) {
            result.addError(module->getName(),
                           "Invalid imported module name: " + depName,
                           "اسم وحدة مستوردة غير صالح: " + depName);
        }
    }
    
    return result;
}

ValidationResult ModuleValidator::validateExports(const Module* module) {
    ValidationResult result;
    
    const auto& exports = module->getExports();
    
    for (const auto& [name, symbol] : exports) {
        // Check if symbol name is valid
        if (!isValidSymbolName(name)) {
            result.addError(module->getName(),
                           "Invalid exported symbol name: " + name,
                           "اسم رمز مُصدّر غير صالح: " + name);
        }
        
        // Check if declaration exists
        if (!symbol.declaration) {
            result.addWarning(module->getName(),
                            "Exported symbol '" + name + "' has no declaration",
                            "الرمز المُصدّر '" + name + "' ليس له إعلان");
        }
    }
    
    return result;
}

ValidationResult ModuleValidator::checkDuplicates(const Module* module) {
    ValidationResult result;
    
    const auto& exports = module->getExports();
    std::set<std::string> seen;
    
    for (const auto& [name, symbol] : exports) {
        if (seen.find(name) != seen.end()) {
            result.addError(module->getName(),
                           "Duplicate exported symbol: " + name,
                           "رمز مُصدّر مكرر: " + name);
        }
        seen.insert(name);
    }
    
    return result;
}

ValidationResult ModuleValidator::validateAST(const Module* module) {
    ValidationResult result;
    
    const auto& ast = module->getAST();
    
    if (ast.empty()) {
        if (strictMode_) {
            result.addWarning(module->getName(),
                            "Module has empty AST",
                            "الوحدة لها AST فارغ");
        }
        return result;
    }
    
    // Basic AST validity check
    for (const auto& stmt : ast) {
        if (!stmt) {
            result.addError(module->getName(),
                           "Module contains null statement in AST",
                           "الوحدة تحتوي على عبارة فارغة في AST");
        }
    }
    
    return result;
}

ValidationResult ModuleValidator::checkMissingDependencies(
    const Module* module,
    ModuleLoader& loader) {
    
    ValidationResult result;
    
    const auto& deps = module->getDependencies();
    
    for (const auto& depName : deps) {
        // Try to load it
        Module* loadedModule = loader.loadModule(depName);
        
        if (!loadedModule) {
            result.addError(module->getName(),
                           "Missing dependency: " + depName,
                           "تبعية مفقودة: " + depName);
        }
    }
    
    return result;
}

bool ModuleValidator::isValidModuleName(const std::string& name) const {
    if (name.empty()) return false;
    
    // Must start with letter or underscore
    char first = name[0];
    if (!std::isalpha(static_cast<unsigned char>(first)) && first != '_') {
        return false;
    }
    
    // Rest can be letters, digits, underscores
    for (char c : name) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
            return false;
        }
    }
    
    return true;
}

bool ModuleValidator::isValidSymbolName(const std::string& name) const {
    // Same rules as module names for now
    return isValidModuleName(name);
}

} // namespace Modules
} // namespace Sad
