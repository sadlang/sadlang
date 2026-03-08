/**
 * @file module.cpp
 * @brief (AR) تنفيذ بنية الوحدة الأساسية
 *        (EN) Implementation of core module structure
 * 
 * @author Sad Language Development Team
 * @date December 2025
 */

#include "module.h"
#include "module_nodes.h"
#include "declarations.h"
#include "statements.h"
#include <sstream>
#include <fstream>
#include <filesystem>
#include <iomanip>

namespace Sad {
namespace Modules {

// =========================================================================
// ModuleMetadata Implementation
// =========================================================================

std::string ModuleMetadata::toString() const {
    std::ostringstream oss;
    oss << "Module: " << name << "\n"
        << "Version: " << version << "\n"
        << "File: " << filePath << "\n"
        << "Size: " << fileSize << " bytes\n"
        << "Checksum: " << checksum;
    return oss.str();
}

// =========================================================================
// ExportedSymbol Implementation
// =========================================================================

std::string ExportedSymbol::toString() const {
    std::string typeStr;
    switch (type) {
        case Type::FUNCTION: typeStr = "دالة / Function"; break;
        case Type::CLASS: typeStr = "صنف / Class"; break;
        case Type::VARIABLE: typeStr = "متغير / Variable"; break;
        case Type::CONSTANT: typeStr = "ثابت / Constant"; break;
    }
    return name + " (" + typeStr + ")";
}

// =========================================================================
// Module Implementation
// =========================================================================

Module::Module(const std::string& name, const std::string& filePath)
    : metadata_{}, ast_{}, exports_{}, dependencies_{} {
    metadata_.name = name;
    metadata_.filePath = filePath;
    metadata_.version = "1.0.0";
    metadata_.loadTime = std::chrono::system_clock::now();
    
    if (!filePath.empty() && std::filesystem::exists(filePath)) {
        metadata_.fileSize = std::filesystem::file_size(filePath);
        metadata_.fileModTime = std::chrono::system_clock::from_time_t(
            std::filesystem::last_write_time(filePath).time_since_epoch().count()
        );
        metadata_.checksum = calculateChecksum();
    }
}

void Module::setAST(std::vector<AST::StmtPtr> ast) {
    ast_ = std::move(ast);
    
    // تحليل تلقائي للاعتماديات والتصديرات
    // Automatic analysis of dependencies and exports
    analyzeDependencies();
    analyzeExports();
}

void Module::addExport(const std::string& name, ExportedSymbol::Type type, AST::StmtPtr declaration) {
    ExportedSymbol symbol;
    symbol.name = name;
    symbol.type = type;
    symbol.declaration = std::move(declaration);
    exports_[name] = std::move(symbol);
}

const ExportedSymbol* Module::findExport(const std::string& name) const {
    auto it = exports_.find(name);
    if (it != exports_.end()) {
        return &it->second;
    }
    return nullptr;
}

void Module::addDependency(const std::string& moduleName) {
    // تجنب التكرار / Avoid duplicates
    if (std::find(dependencies_.begin(), dependencies_.end(), moduleName) == dependencies_.end()) {
        dependencies_.push_back(moduleName);
    }
}

bool Module::isValid() const {
    // التحقق من صحة الوحدة
    // Check module validity
    
    // 1. يجب أن يكون هناك اسم
    if (metadata_.name.empty()) {
        return false;
    }
    
    // 2. إذا كان هناك مسار، يجب أن يكون الملف موجوداً
    if (!metadata_.filePath.empty() && !std::filesystem::exists(metadata_.filePath)) {
        return false;
    }
    
    // 3. التحقق من checksum إذا كان الملف موجوداً
    if (!metadata_.filePath.empty() && std::filesystem::exists(metadata_.filePath)) {
        std::string currentChecksum = calculateChecksum();
        if (currentChecksum != metadata_.checksum) {
            return false;  // الملف تغيّر / File changed
        }
    }
    
    return true;
}

void Module::updateMetadata() {
    if (!metadata_.filePath.empty() && std::filesystem::exists(metadata_.filePath)) {
        metadata_.fileSize = std::filesystem::file_size(metadata_.filePath);
        metadata_.fileModTime = std::chrono::system_clock::from_time_t(
            std::filesystem::last_write_time(metadata_.filePath).time_since_epoch().count()
        );
        metadata_.checksum = calculateChecksum();
    }
    metadata_.loadTime = std::chrono::system_clock::now();
}

void Module::analyzeDependencies() {
    // مسح الاعتماديات القديمة / Clear old dependencies
    dependencies_.clear();
    
    // المرور على AST والبحث عن جمل الاستيراد
    // Walk AST and find import statements
    for (const auto& stmt : ast_) {
        // استيراد بسيط: استورد module
        if (auto* importStmt = dynamic_cast<AST::ImportStmt*>(stmt.get())) {
            // بناء اسم الوحدة من المسار
            std::string moduleName;
            for (size_t i = 0; i < importStmt->modulePath.size(); ++i) {
                if (i > 0) moduleName += ".";
                moduleName += importStmt->modulePath[i];
            }
            addDependency(moduleName);
        }
        // استيراد انتقائي: من module استورد ...
        else if (auto* fromImportStmt = dynamic_cast<AST::FromImportStmt*>(stmt.get())) {
            // بناء اسم الوحدة من المسار
            std::string moduleName;
            for (size_t i = 0; i < fromImportStmt->modulePath.size(); ++i) {
                if (i > 0) moduleName += ".";
                moduleName += fromImportStmt->modulePath[i];
            }
            addDependency(moduleName);
        }
    }
}

void Module::analyzeExports() {
    // مسح التصديرات القديمة / Clear old exports
    exports_.clear();
    
    // المرور على AST والبحث عن جمل التصدير
    // Walk AST and find export declarations
    for (const auto& stmt : ast_) {
        if (auto* exportDecl = dynamic_cast<AST::ExportDecl*>(stmt.get())) {
            if (exportDecl->declaration) {
                // (AR) تحديد نوع التصدير واسمه من التصريح
                // (EN) Determine export type and name from declaration
                ExportedSymbol::Type type = ExportedSymbol::Type::VARIABLE;
                std::string name;
                
                if (auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(exportDecl->declaration.get())) {
                    type = ExportedSymbol::Type::FUNCTION;
                    name = funcDecl->name;
                } else if (auto* classDecl = dynamic_cast<AST::ClassDecl*>(exportDecl->declaration.get())) {
                    type = ExportedSymbol::Type::CLASS;
                    name = classDecl->name;
                } else if (auto* varDecl = dynamic_cast<AST::VarDeclStmt*>(exportDecl->declaration.get())) {
                    type = varDecl->isConst ? ExportedSymbol::Type::CONSTANT : ExportedSymbol::Type::VARIABLE;
                    name = varDecl->name;
                }
                
                if (!name.empty()) {
                    // (AR) نمرر nullptr لأن الملكية تبقى في AST
                    // (EN) Pass nullptr — ownership stays with the AST node
                    addExport(name, type, nullptr);
                }
            }
        }
    }
}

std::string Module::calculateChecksum() const {
    if (metadata_.filePath.empty() || !std::filesystem::exists(metadata_.filePath)) {
        return "";
    }
    
    // حساب checksum بسيط باستخدام hash
    // Simple checksum using hash
    std::ifstream file(metadata_.filePath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    
    std::hash<std::string> hasher;
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    
    size_t hash = hasher(content);
    
    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(16) << hash;
    return oss.str();
}

std::string Module::toString() const {
    std::ostringstream oss;
    oss << "═══════════════════════════════════════\n";
    oss << "  Module: " << metadata_.name << "\n";
    oss << "═══════════════════════════════════════\n";
    oss << "File: " << metadata_.filePath << "\n";
    oss << "Version: " << metadata_.version << "\n";
    oss << "AST Statements: " << ast_.size() << "\n";
    oss << "Dependencies: " << dependencies_.size() << "\n";
    for (const auto& dep : dependencies_) {
        oss << "  - " << dep << "\n";
    }
    oss << "Exports: " << exports_.size() << "\n";
    for (const auto& [name, symbol] : exports_) {
        oss << "  - " << symbol.toString() << "\n";
    }
    oss << "═══════════════════════════════════════\n";
    return oss.str();
}

} // namespace Modules
} // namespace Sad
