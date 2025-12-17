/**
 * @file symbol_resolver.cpp
 * @brief (AR) تنفيذ محلل الرموز
 *        (EN) Implementation of symbol resolver
 * 
 * @author Sad Language Development Team
 * @date December 2025
 */

#include "../../include/modules/symbol_resolver.h"
#include "../../include/modules/module_loader.h"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace Sad {
namespace Modules {

// =========================================================================
// SymbolInfo Implementation
// =========================================================================

std::string SymbolInfo::toString() const {
    std::stringstream ss;
    
    // نوع الرمز / Symbol kind
    switch (kind) {
        case SymbolKind::FUNCTION:
            ss << "دالة / Function: ";
            break;
        case SymbolKind::CLASS:
            ss << "صنف / Class: ";
            break;
        case SymbolKind::VARIABLE:
            ss << "متغير / Variable: ";
            break;
        case SymbolKind::CONSTANT:
            ss << "ثابت / Constant: ";
            break;
        case SymbolKind::MODULE:
            ss << "وحدة / Module: ";
            break;
    }
    
    ss << qualifiedName;
    
    if (!type.empty()) {
        ss << " : " << type;
    }
    
    if (isExported) {
        ss << " [exported]";
    }
    
    return ss.str();
}

// =========================================================================
// SymbolResolver Implementation
// =========================================================================

SymbolResolver::SymbolResolver() 
    : symbolTable_{}, resolvedImports_{} {
}

SymbolResolver::~SymbolResolver() {
    clear();
}

void SymbolResolver::analyzeModule(Module* module) {
    if (!module || module->getAST().empty()) {
        return;
    }
    
    const std::string& moduleName = module->getName();
    
    // استخراج الرموز من AST
    // Extract symbols from AST
    const auto& ast = module->getAST();
    auto symbols = extractSymbols(module, ast);
    
    // حفظ في جدول الرموز
    // Save to symbol table
    symbolTable_[moduleName] = symbols;
    
    std::cout << "Analyzed module '" << moduleName 
              << "': found " << symbols.size() << " symbols" << std::endl;
}

void SymbolResolver::resolveImports(Module* module, ModuleLoader* moduleLoader) {
    if (!module || !moduleLoader) {
        return;
    }
    
    const std::string& moduleName = module->getName();
    std::vector<ResolvedImport> imports;
    
    // المرور على جميع التبعيات
    // Iterate through all dependencies
    for (const auto& depName : module->getDependencies()) {
        // تحميل الوحدة التابعة
        // Load dependent module
        Module* depModule = moduleLoader->loadModule(depName);
        if (!depModule) {
            std::cerr << "Failed to load dependency: " << depName << std::endl;
            continue;
        }
        
        // تحليل رموز الوحدة التابعة إذا لم تُحلل بعد
        // Analyze dependent module symbols if not already analyzed
        if (symbolTable_.find(depName) == symbolTable_.end()) {
            analyzeModule(depModule);
        }
        
        // إنشاء ResolvedImport
        // Create ResolvedImport
        ResolvedImport resolved;
        resolved.moduleName = depName;
        resolved.module = depModule;
        resolved.isWildcard = false;  // سنحدده من AST لاحقاً
        
        imports.push_back(resolved);
    }
    
    resolvedImports_[moduleName] = imports;
    
    std::cout << "Resolved imports for '" << moduleName 
              << "': " << imports.size() << " imports" << std::endl;
}

std::optional<SymbolInfo> SymbolResolver::findSymbol(
    const std::string& name,
    const Module* currentModule) const 
{
    if (!currentModule) {
        return std::nullopt;
    }
    
    const std::string& moduleName = currentModule->getName();
    
    // 1. البحث في الوحدة الحالية أولاً
    // Search in current module first
    auto it = symbolTable_.find(moduleName);
    if (it != symbolTable_.end()) {
        for (const auto& symbol : it->second) {
            if (symbol.name == name) {
                return symbol;
            }
        }
    }
    
    // 2. البحث في الوحدات المستوردة
    // Search in imported modules
    auto importIt = resolvedImports_.find(moduleName);
    if (importIt != resolvedImports_.end()) {
        for (const auto& import : importIt->second) {
            auto symbolIt = symbolTable_.find(import.moduleName);
            if (symbolIt != symbolTable_.end()) {
                for (const auto& symbol : symbolIt->second) {
                    if (symbol.name == name && symbol.isExported) {
                        return symbol;
                    }
                }
            }
        }
    }
    
    return std::nullopt;
}

std::optional<SymbolInfo> SymbolResolver::findQualifiedSymbol(
    const std::string& qualifiedName) const 
{
    // تقسيم الاسم المؤهل: "module.symbol"
    // Split qualified name: "module.symbol"
    size_t dotPos = qualifiedName.find('.');
    if (dotPos == std::string::npos) {
        return std::nullopt;
    }
    
    std::string moduleName = qualifiedName.substr(0, dotPos);
    std::string symbolName = qualifiedName.substr(dotPos + 1);
    
    // البحث في جدول رموز الوحدة
    // Search in module's symbol table
    auto it = symbolTable_.find(moduleName);
    if (it != symbolTable_.end()) {
        for (const auto& symbol : it->second) {
            if (symbol.name == symbolName && symbol.isExported) {
                return symbol;
            }
        }
    }
    
    return std::nullopt;
}

std::vector<SymbolInfo> SymbolResolver::getModuleSymbols(
    const std::string& moduleName) const 
{
    auto it = symbolTable_.find(moduleName);
    if (it != symbolTable_.end()) {
        return it->second;
    }
    return {};
}

std::vector<SymbolInfo> SymbolResolver::getExportedSymbols(
    const std::string& moduleName) const 
{
    std::vector<SymbolInfo> exported;
    
    auto it = symbolTable_.find(moduleName);
    if (it != symbolTable_.end()) {
        for (const auto& symbol : it->second) {
            if (symbol.isExported) {
                exported.push_back(symbol);
            }
        }
    }
    
    return exported;
}

std::vector<SymbolInfo> SymbolResolver::getCompletionSuggestions(
    const std::string& prefix,
    const Module* currentModule) const 
{
    std::vector<SymbolInfo> suggestions;
    
    if (!currentModule) {
        return suggestions;
    }
    
    const std::string& moduleName = currentModule->getName();
    
    // البحث في الوحدة الحالية
    // Search in current module
    auto it = symbolTable_.find(moduleName);
    if (it != symbolTable_.end()) {
        for (const auto& symbol : it->second) {
            if (symbol.name.find(prefix) == 0) {  // يبدأ بـ prefix
                suggestions.push_back(symbol);
            }
        }
    }
    
    // البحث في الوحدات المستوردة
    // Search in imported modules
    auto importIt = resolvedImports_.find(moduleName);
    if (importIt != resolvedImports_.end()) {
        for (const auto& import : importIt->second) {
            auto exported = getExportedSymbols(import.moduleName);
            for (const auto& symbol : exported) {
                if (symbol.name.find(prefix) == 0) {
                    suggestions.push_back(symbol);
                }
            }
        }
    }
    
    return suggestions;
}

std::optional<std::string> SymbolResolver::validateImport(
    const AST::ImportStmt* importStmt,
    const Module* currentModule) const 
{
    if (!importStmt || !currentModule) {
        return "Invalid import statement";
    }
    
    // TODO: التحقق من صحة الاستيراد
    // - التحقق من وجود الوحدة
    // - التحقق من وجود الرموز المستوردة
    // - التحقق من تصدير الرموز
    
    return std::nullopt;  // صحيح / Valid
}

void SymbolResolver::clear() {
    symbolTable_.clear();
    resolvedImports_.clear();
}

void SymbolResolver::printSymbolTable() const {
    std::cout << "\n";
    std::cout << "========================================" << std::endl;
    std::cout << "  Symbol Table (جدول الرموز)" << std::endl;
    std::cout << "========================================" << std::endl;
    
    for (const auto& [moduleName, symbols] : symbolTable_) {
        std::cout << "\nModule: " << moduleName << std::endl;
        std::cout << "Symbols (" << symbols.size() << "):" << std::endl;
        
        for (const auto& symbol : symbols) {
            std::cout << "  - " << symbol.toString() << std::endl;
        }
    }
    
    std::cout << "\n========================================" << std::endl;
}

// =========================================================================
// Private Helper Methods
// =========================================================================

std::vector<SymbolInfo> SymbolResolver::extractSymbols(
    const Module* module,
    const std::vector<AST::StmtPtr>& ast) 
{
    std::vector<SymbolInfo> symbols;
    
    if (!module || ast.empty()) {
        return symbols;
    }
    
    // المرور على جميع العقد في البرنامج
    // Iterate through all nodes in program
    for (const auto& stmt : ast) {
        auto symbol = extractSymbolFromDecl(module, stmt.get());
        if (symbol) {
            symbols.push_back(*symbol);
        }
    }
    
    return symbols;
}

std::optional<SymbolInfo> SymbolResolver::extractSymbolFromDecl(
    const Module* module,
    AST::ASTNode* decl) 
{
    if (!module || !decl) {
        return std::nullopt;
    }
    
    SymbolInfo info;
    info.moduleName = module->getName();
    info.declaration = decl;
    info.isExported = false;
    
    // التحقق من نوع العقدة
    // Check node type
    if (auto exportDecl = dynamic_cast<AST::ExportDecl*>(decl)) {
        // تصدير / Export
        info.isExported = true;
        
        // الحصول على التعريف المُصدّر
        if (auto funcDecl = dynamic_cast<AST::FunctionDecl*>(exportDecl->declaration.get())) {
            info.name = funcDecl->name;
            info.kind = SymbolKind::FUNCTION;
            info.type = "function";  // TODO: استخراج توقيع الدالة الكامل
            info.qualifiedName = info.moduleName + "." + info.name;
            return info;
        }
        else if (auto classDecl = dynamic_cast<AST::ClassDecl*>(exportDecl->declaration.get())) {
            info.name = classDecl->name;
            info.kind = SymbolKind::CLASS;
            info.type = "class";
            info.qualifiedName = info.moduleName + "." + info.name;
            return info;
        }
        else if (auto varDecl = dynamic_cast<AST::VarDeclStmt*>(exportDecl->declaration.get())) {
            info.name = varDecl->name;
            info.kind = SymbolKind::VARIABLE;
            info.type = "variable";
            info.qualifiedName = info.moduleName + "." + info.name;
            return info;
        }
    }
    else if (auto funcDecl = dynamic_cast<AST::FunctionDecl*>(decl)) {
        // دالة عادية (غير مُصدّرة) / Regular function (not exported)
        info.name = funcDecl->name;
        info.kind = SymbolKind::FUNCTION;
        info.type = "function";
        info.qualifiedName = info.moduleName + "." + info.name;
        return info;
    }
    else if (auto classDecl = dynamic_cast<AST::ClassDecl*>(decl)) {
        // صنف عادي / Regular class
        info.name = classDecl->name;
        info.kind = SymbolKind::CLASS;
        info.type = "class";
        info.qualifiedName = info.moduleName + "." + info.name;
        return info;
    }
    else if (auto varDecl = dynamic_cast<AST::VarDeclStmt*>(decl)) {
        // متغير عادي / Regular variable
        info.name = varDecl->name;
        info.kind = SymbolKind::VARIABLE;
        info.type = "variable";
        info.qualifiedName = info.moduleName + "." + info.name;
        return info;
    }
    
    return std::nullopt;
}

ResolvedImport SymbolResolver::resolveImport(
    const AST::ImportStmt* importStmt,
    const Module* currentModule,
    ModuleLoader* moduleLoader) 
{
    ResolvedImport resolved;
    
    if (!importStmt || !currentModule || !moduleLoader) {
        return resolved;
    }
    
    // بناء اسم الوحدة من المسار
    // Build module name from path
    std::string moduleName;
    for (size_t i = 0; i < importStmt->modulePath.size(); ++i) {
        if (i > 0) moduleName += ".";
        moduleName += importStmt->modulePath[i];
    }
    
    resolved.moduleName = moduleName;
    resolved.alias = importStmt->alias.has_value() ? importStmt->alias.value() : moduleName;
    resolved.isWildcard = false;
    
    // تحميل الوحدة
    // Load module
    resolved.module = moduleLoader->loadModule(moduleName);
    
    return resolved;
}

bool SymbolResolver::isSymbolExported(
    const std::string& symbolName,
    const Module* module) const 
{
    if (!module) {
        return false;
    }
    
    const auto& exports = module->getExports();
    for (const auto& [name, exp] : exports) {
        if (name == symbolName) {
            return true;
        }
    }
    
    return false;
}

} // namespace Modules
} // namespace Sad
