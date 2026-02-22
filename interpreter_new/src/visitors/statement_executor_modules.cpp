/**
 * @file statement_executor_modules.cpp
 * @brief (AR) تنفيذ نظام الاستيراد والتصدير في المفسر
 *        (EN) Implementation of import/export system in the interpreter
 * 
 * @details
 * (AR) هذا الملف يحتوي على تنفيذ جمل الاستيراد والتصدير:
 *      1. visitImportStmt - استيراد وحدة كاملة (مثل: استورد رياضيات)
 *      2. visitFromImportStmt - استيراد انتقائي (مثل: من رياضيات استورد جذر)
 *      3. visitExportDecl - تصدير تصريح (مثل: صدّر دالة حساب)
 *      4. visitExportStmt - تصدير تصريح (الإصدار القديم)
 * 
 *      آلية العمل:
 *      ═══════════
 *      عند مواجهة جملة استيراد:
 *      1. يُستخدم ModuleResolver للبحث عن ملف الوحدة
 *      2. يُحمَّل الملف ويُحلَّل إلى AST
 *      3. يُنفَّذ AST الوحدة في نطاق معزول (scope)
 *      4. تُستخرج الرموز المُصدَّرة (الدوال، الأصناف، المتغيرات)
 *      5. تُسجَّل الرموز في النطاق الحالي
 * 
 *      أنماط الاستيراد المدعومة:
 *      ═══════════════════════════
 *      - استورد رياضيات           → كل الصادرات كفضاء أسماء
 *      - استورد رياضيات كـ ر       → كل الصادرات كفضاء أسماء باسم مستعار
 *      - من رياضيات استورد جذر     → استيراد دالة محددة
 *      - من رياضيات استورد *       → استيراد كل الصادرات مباشرة
 *      - من رياضيات استورد جذر كـ ج → استيراد بإسم مستعار
 * 
 *      أنماط التصدير المدعومة:
 *      ═══════════════════════════
 *      - صدّر دالة حساب(...)       → تصدير دالة
 *      - صدّر صنف شخص(...)        → تصدير صنف
 *      - صدّر متغير س = 10         → تصدير متغير
 * 
 * @author فريق تطوير لغة ص / Sad Language Development Team
 * @date فبراير 2026 / February 2026
 * @version 1.0.0
 */

#include "statement_executor.h"
#include "module_nodes.h"
#include "declarations.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "class_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <set>

namespace Sad {
namespace Interpreter {

// =========================================================================
// (AR) تنفيذ وحدة مُحمَّلة واستخراج رموزها المُصدَّرة
// (EN) Execute a loaded module and extract its exported symbols
// =========================================================================

Data::Value StatementExecutor::executeModuleAndExtractExports(Modules::Module* module) {
    if (!module || module->ast.empty()) {
        return Data::Value(Data::Value::MapType());
    }
    
    // (AR) التحقق من الذاكرة المخبئية - إذا سبق تنفيذ هذه الوحدة نعيد الصادرات المخبأة
    // (EN) Check cache - if module was already executed, return cached exports
    // (AR) استخدام اسم الوحدة بدلاً من المسار لتجنب مشاكل الترميز العربي
    // (EN) Use module name instead of path to avoid Arabic encoding issues
    std::string moduleKey = module->fullName;
    auto cacheIt = executedModuleExports_.find(moduleKey);
    if (cacheIt != executedModuleExports_.end()) {
        return cacheIt->second;
    }
    
    // (AR) حفظ الحالة الحالية لمنفذ العبارات
    // (EN) Save current state of statement executor
    auto savedFlowControl = flowControl_;
    auto savedReturnValue = returnValue_;
    auto savedExportedSymbols = exportedSymbols_;
    auto savedCurrentFilePath = currentFilePath_;
    auto savedCurrentNamespace = currentNamespace_;
    
    // (AR) تعيين مسار الملف الجديد (مسار الوحدة)
    // (EN) Set new file path (module path)
    // (AR) استخدام u8string لدعم المسارات العربية
    // (EN) Use u8string to support Arabic paths
#ifdef _WIN32
    currentFilePath_ = module->filePath.u8string();
#else
    currentFilePath_ = module->filePath.string();
#endif
    
    // (AR) مسح الرموز المُصدَّرة للوحدة الجديدة
    // (EN) Clear exported symbols for new module
    exportedSymbols_.clear();
    
    // (AR) إنشاء نطاق جديد معزول للوحدة
    // (EN) Create isolated scope for module
    scopeManager_.pushScope(Data::ScopeType::BLOCK, "module:" + module->name);
    
    // (AR) حفظ أسماء الأصناف قبل تنفيذ الوحدة لاكتشاف الأصناف الجديدة
    // (EN) Snapshot class names before module execution to detect new classes
    std::set<std::string> classNamesBefore;
    {
        auto* classManager = Data::ClassManager::getInstance();
        if (classManager) {
            auto allNames = classManager->getAllClassNames();
            classNamesBefore.insert(allNames.begin(), allNames.end());
        }
    }
    
    // (AR) المرحلة 1: تنفيذ كل جمل الوحدة
    // (EN) Phase 1: Execute all module statements
    bool hasExplicitExports = false;
    
    for (auto& stmt : module->ast) {
        if (!stmt) continue;
        
        // (AR) التحقق إذا كانت الوحدة تحتوي على تصدير صريح
        // (EN) Check if module has explicit exports
        if (dynamic_cast<AST::ExportDecl*>(stmt.get()) != nullptr ||
            dynamic_cast<AST::ExportStmt*>(stmt.get()) != nullptr) {
            hasExplicitExports = true;
        }
        
        // (AR) تنفيذ الجملة
        // (EN) Execute statement
        stmt->accept(*this);
        
        // (AR) إعادة تعيين التحكم بالتدفق بعد كل جملة
        // (EN) Reset flow control after each statement
        if (flowControl_ == FlowControl::RETURN) {
            flowControl_ = FlowControl::NONE;
        }
    }
    
    // (AR) المرحلة 2: جمع الرموز المُصدَّرة
    // (EN) Phase 2: Collect exported symbols
    Data::Value::MapType moduleExports;
    
    if (hasExplicitExports) {
        // (AR) تصدير صريح: فقط الرموز المحددة بـ صدّر
        // (EN) Explicit export: only symbols marked with export
        for (const auto& symbolName : exportedSymbols_) {
            // (AR) البحث عن الرمز كمتغير
            // (EN) Look for symbol as variable
            if (variableManager_.exists(symbolName)) {
                moduleExports[symbolName] = variableManager_.get(symbolName);
                continue;
            }
            
            // (AR) البحث عن الرمز كدالة
            // (EN) Look for symbol as function
            if (functionManager_.hasFunction(symbolName)) {
                moduleExports[symbolName] = Data::Value(std::string("__func__:" + symbolName));
                continue;
            }
            
            // (AR) البحث عن الرمز كصنف
            // (EN) Look for symbol as class
            auto* classManager = Data::ClassManager::getInstance();
            if (classManager && classManager->hasClass(symbolName)) {
                moduleExports[symbolName] = Data::Value(std::string("__class__:" + symbolName));
                continue;
            }
        }
    } else {
        // (AR) لا يوجد تصدير صريح: نصدّر كل الدوال المُعرَّفة من المستخدم
        // (EN) No explicit export: export all user-defined functions
        auto allFunctions = functionManager_.getFunctionNames();
        for (const auto& funcName : allFunctions) {
            // (AR) تجاهل الدوال الداخلية والمُضمَّنة
            // (EN) Skip internal and built-in functions
            if (funcName.find("__") == 0) continue;
            
            // (AR) نتحقق أن الدالة ليست مُضمَّنة عبر getFunctionOverloads
            // (EN) Check that function is not built-in via getFunctionOverloads
            auto overloads = functionManager_.getFunctionOverloads(funcName);
            if (overloads.empty()) {
                continue;
            }
            // (AR) إذا كان أي إصدار غير مُضمَّن، نصدّره
            // (EN) If any version is non-native, export it
            bool hasUserDefined = false;
            for (const auto& funcDef : overloads) {
                if (funcDef && !funcDef->hasNativeImplementation()) {
                    hasUserDefined = true;
                    break;
                }
            }
            if (hasUserDefined) {
                moduleExports[funcName] = Data::Value(std::string("__func__:" + funcName));
            }
        }
        
        // (AR) تصدير ضمني للأصناف المعرّفة في الوحدة
        // (EN) Implicit export of classes defined in the module
        {
            auto* classManager = Data::ClassManager::getInstance();
            if (classManager) {
                auto allClassNames = classManager->getAllClassNames();
                for (const auto& className : allClassNames) {
                    // (AR) نصدّر فقط الأصناف الجديدة التي عُرِّفت أثناء تنفيذ الوحدة
                    // (EN) Only export classes that were defined during module execution
                    if (classNamesBefore.find(className) == classNamesBefore.end()) {
                        moduleExports[className] = Data::Value(std::string("__class__:" + className));
                    }
                }
            }
        }
    }
    
    // (AR) الخروج من نطاق الوحدة
    // (EN) Exit module scope
    scopeManager_.popScope();
    
    // (AR) استعادة الحالة السابقة
    // (EN) Restore previous state
    flowControl_ = savedFlowControl;
    returnValue_ = savedReturnValue;
    exportedSymbols_ = savedExportedSymbols;
    currentFilePath_ = savedCurrentFilePath;
    currentNamespace_ = savedCurrentNamespace;
    
    // (AR) تخزين النتيجة في الذاكرة المخبئية
    // (EN) Store result in cache
    Data::Value result(moduleExports);
    executedModuleExports_[moduleKey] = result;
    
    return result;
}

// =========================================================================
// (AR) جملة الاستيراد الكامل: استورد وحدة [كـ اسم_مستعار]
// (EN) Full import statement: import module [as alias]
// =========================================================================

void StatementExecutor::visitImportStmt(AST::ImportStmt& node) {
    // (AR) التحقق من وجود محلل الوحدات
    // (EN) Check module resolver exists
    if (!moduleResolver_) {
        throw ExecutionError(
            "خطأ: نظام الاستيراد غير مُهيَّأ. لا يوجد محلل وحدات.\n"
            "Error: Import system not initialized. No module resolver."
        );
    }
    
    // (AR) بناء الاسم الكامل للوحدة من المسار
    // (EN) Build full module name from path
    std::string fullModuleName = node.getFullModuleName();
    
    // (AR) تحديد الاسم الفعلي (الاسم المستعار أو اسم الوحدة)
    // (EN) Determine effective name (alias or module name)
    std::string effectiveName = node.getEffectiveName();
    
    // (AR) استخدام ModuleResolver للبحث عن الوحدة وتحميلها
    // (EN) Use ModuleResolver to find and load module
    Modules::Module* module = moduleResolver_->resolveModule(
        node.modulePath,
        currentFilePath_
    );
    
    if (!module) {
        // (AR) خطأ: الوحدة غير موجودة
        // (EN) Error: Module not found
        std::string searchedPaths;
        for (const auto& path : moduleResolver_->getSearchPaths()) {
            searchedPaths += "  - " + path.string() + "\n";
        }
        
        throw ExecutionError(
            "خطأ: لم يتم العثور على الوحدة '" + fullModuleName + "'\n"
            "المسارات التي تم البحث فيها:\n" + searchedPaths +
            "تأكد من أن الملف '" + fullModuleName + ".ص' موجود في أحد هذه المسارات."
        );
    }
    
    // (AR) تنفيذ AST الوحدة واستخراج الرموز المُصدَّرة
    // (EN) Execute module AST and extract exported symbols
    Data::Value moduleExports = executeModuleAndExtractExports(module);
    
    // (AR) تسجيل الوحدة كمتغير Map في النطاق الحالي
    // (EN) Register module as Map variable in current scope
    if (moduleExports.isMap()) {
        // (AR) حفظ فضاء أسماء الوحدة
        // (EN) Save module namespace
        loadedModuleNamespaces_[effectiveName] = moduleExports;
        
        // (AR) تسجيل كمتغير Map
        // (EN) Register as Map variable
        if (variableManager_.exists(effectiveName)) {
            variableManager_.assign(effectiveName, moduleExports);
        } else {
            variableManager_.define(effectiveName, moduleExports);
        }
    }
}

// =========================================================================
// (AR) جملة الاستيراد الانتقائي: من وحدة استورد رمز1، رمز2
// (EN) Selective import statement: from module import symbol1, symbol2
// =========================================================================

void StatementExecutor::visitFromImportStmt(AST::FromImportStmt& node) {
    // (AR) التحقق من وجود محلل الوحدات
    // (EN) Check module resolver exists
    if (!moduleResolver_) {
        throw ExecutionError(
            "خطأ: نظام الاستيراد غير مُهيَّأ. لا يوجد محلل وحدات.\n"
            "Error: Import system not initialized. No module resolver."
        );
    }
    
    // (AR) بناء الاسم الكامل للوحدة
    // (EN) Build full module name
    std::string fullModuleName = node.getFullModuleName();
    
    // (AR) تحميل الوحدة
    // (EN) Load module
    Modules::Module* module = moduleResolver_->resolveModule(
        node.modulePath,
        currentFilePath_
    );
    
    if (!module) {
        std::string searchedPaths;
        for (const auto& path : moduleResolver_->getSearchPaths()) {
            searchedPaths += "  - " + path.string() + "\n";
        }
        
        throw ExecutionError(
            "خطأ: لم يتم العثور على الوحدة '" + fullModuleName + "'\n"
            "المسارات التي تم البحث فيها:\n" + searchedPaths
        );
    }
    
    // (AR) تنفيذ AST الوحدة واستخراج الصادرات
    // (EN) Execute module AST and extract exports
    Data::Value moduleExports = executeModuleAndExtractExports(module);
    
    if (!moduleExports.isMap()) {
        throw ExecutionError(
            "خطأ: فشل في استخراج صادرات الوحدة '" + fullModuleName + "'"
        );
    }
    
    const auto& exportsMap = moduleExports.toMap();
    
    if (node.isWildcard) {
        // ═══════════════════════════════════════════════════════════════
        // (AR) استيراد عام: من وحدة استورد *
        // (EN) Wildcard import: from module import *
        // ═══════════════════════════════════════════════════════════════
        
        for (const auto& [name, value] : exportsMap) {
            if (value.isString() && value.toString().find("__func__:") == 0) {
                // (AR) الدالة مُسجَّلة بالفعل في FunctionManager
                // (EN) Function already registered in FunctionManager
                continue;
            }
            
            // (AR) تسجيل المتغير
            // (EN) Register variable
            if (variableManager_.exists(name)) {
                variableManager_.assign(name, value);
            } else {
                variableManager_.define(name, value);
            }
        }
        
    } else {
        // ═══════════════════════════════════════════════════════════════
        // (AR) استيراد انتقائي: من وحدة استورد رمز1، رمز2
        // (EN) Selective import: from module import sym1, sym2
        // ═══════════════════════════════════════════════════════════════
        
        for (const auto& item : node.items) {
            std::string originalName = item.name;
            std::string importedName = item.getEffectiveName();
            
            // (AR) البحث عن الرمز في صادرات الوحدة
            // (EN) Look for symbol in module exports
            auto it = exportsMap.find(originalName);
            
            if (it != exportsMap.end()) {
                const Data::Value& value = it->second;
                
                if (value.isString() && value.toString().find("__func__:") == 0) {
                    // (AR) دالة مُسجَّلة - إذا كان هناك اسم مستعار نسجل بالاسم الجديد
                    // (EN) Registered function - if alias, register with new name
                    if (item.alias.has_value() && item.alias.value() != originalName) {
                        // (AR) نستخدم getFunctionOverloads للحصول على كل إصدارات الدالة
                        // (EN) Use getFunctionOverloads to get all versions of the function
                        auto overloads = functionManager_.getFunctionOverloads(originalName);
                        for (const auto& funcDef : overloads) {
                            if (funcDef && funcDef->getBody()) {
                                functionManager_.defineFunction(
                                    importedName,
                                    funcDef->getParameters(),
                                    funcDef->getBody(),
                                    funcDef->getFunctionDecl()
                                );
                            }
                        }
                    }
                    continue;
                }
                
                // (AR) متغير - تسجيل بالاسم الفعلي
                // (EN) Variable - register with effective name
                if (variableManager_.exists(importedName)) {
                    variableManager_.assign(importedName, value);
                } else {
                    variableManager_.define(importedName, value);
                }
                
            } else {
                // (AR) غير موجود في الخريطة - نتحقق من FunctionManager
                // (EN) Not in map - check FunctionManager
                if (functionManager_.hasFunction(originalName)) {
                    if (item.alias.has_value() && item.alias.value() != originalName) {
                        auto overloads = functionManager_.getFunctionOverloads(originalName);
                        for (const auto& funcDef : overloads) {
                            if (funcDef && funcDef->getBody()) {
                                functionManager_.defineFunction(
                                    importedName,
                                    funcDef->getParameters(),
                                    funcDef->getBody(),
                                    funcDef->getFunctionDecl()
                                );
                            }
                        }
                    }
                } else {
                    // (AR) الرمز غير موجود
                    // (EN) Symbol not found
                    std::string availableSymbols;
                    for (const auto& [symName, _] : exportsMap) {
                        availableSymbols += "  - " + symName + "\n";
                    }
                    
                    throw ExecutionError(
                        "خطأ: الرمز '" + originalName + "' غير موجود في الوحدة '" + fullModuleName + "'\n"
                        "الرموز المتوفرة:\n" + availableSymbols
                    );
                }
            }
        }
    }
}

// =========================================================================
// (AR) جملة التصدير (الإصدار الجديد): صدّر دالة|صنف|متغير ...
// (EN) Export declaration (new version): export function|class|var ...
// =========================================================================

void StatementExecutor::visitExportDecl(AST::ExportDecl& node) {
    if (!node.declaration) {
        return;
    }
    
    // (AR) تنفيذ التصريح الداخلي (يُسجَّل في المدير المناسب)
    // (EN) Execute inner declaration (registers in appropriate manager)
    node.declaration->accept(*this);
    
    // (AR) استخراج اسم الرمز المُصدَّر وتمييزه
    // (EN) Extract exported symbol name and mark it
    std::string exportedName = node.getExportedName();
    
    if (!exportedName.empty()) {
        exportedSymbols_.insert(exportedName);
    }
}

// =========================================================================
// (AR) جملة التصدير (الإصدار القديم للتوافق)
// (EN) Export statement (legacy version for compatibility)
// =========================================================================

void StatementExecutor::visitExportStmt(AST::ExportStmt& node) {
    if (!node.declaration) {
        return;
    }
    
    // (AR) تنفيذ التصريح الداخلي
    // (EN) Execute inner declaration
    node.declaration->accept(*this);
    
    // (AR) استخراج اسم الرمز المُصدَّر
    // (EN) Extract exported symbol name
    std::string exportedName;
    
    if (auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(node.declaration.get())) {
        exportedName = funcDecl->name;
    } else if (auto* classDecl = dynamic_cast<AST::ClassDecl*>(node.declaration.get())) {
        exportedName = classDecl->name;
    } else if (auto* varDecl = dynamic_cast<AST::VarDeclStmt*>(node.declaration.get())) {
        exportedName = varDecl->name;
    }
    
    if (!exportedName.empty()) {
        exportedSymbols_.insert(exportedName);
    }
}

} // namespace Interpreter
} // namespace Sad
