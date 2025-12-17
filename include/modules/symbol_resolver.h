/**
 * @file symbol_resolver.h
 * @brief (AR) محلل الرموز للوحدات
 *        (EN) Symbol resolver for modules
 * 
 * @details
 * (AR) يحلل الرموز (دوال، أصناف، متغيرات) من الوحدات المستوردة
 *      ويوفر وصول سريع للرموز المُصدّرة عبر الوحدات
 * 
 * (EN) Resolves symbols (functions, classes, variables) from imported modules
 *      and provides fast access to exported symbols across modules
 * 
 * @author Sad Language Development Team
 * @date December 2025
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <optional>
#include "modules/module.h"
#include "../parser/ast/declarations.h"
#include "../parser/ast/statements.h"

namespace Sad {
namespace Modules {

/**
 * @enum SymbolKind
 * @brief (AR) نوع الرمز / (EN) Symbol kind
 */
enum class SymbolKind {
    FUNCTION,       ///< دالة / Function
    CLASS,          ///< صنف / Class
    VARIABLE,       ///< متغير / Variable
    CONSTANT,       ///< ثابت / Constant
    MODULE          ///< وحدة / Module
};

/**
 * @struct SymbolInfo
 * @brief (AR) معلومات رمز / (EN) Symbol information
 */
struct SymbolInfo {
    std::string name;                       ///< اسم الرمز / Symbol name
    SymbolKind kind;                        ///< نوع الرمز / Symbol kind
    std::string type;                       ///< النوع (للمتغيرات والدوال) / Type
    std::string moduleName;                 ///< الوحدة التي يعود لها / Owning module
    std::string qualifiedName;              ///< الاسم المؤهل (module.symbol) / Qualified name
    AST::ASTNode* declaration;              ///< عقدة التعريف في AST / Declaration node
    bool isExported;                        ///< هل مُصدّر؟ / Is exported?
    std::string documentation;              ///< التوثيق / Documentation
    
    /**
     * @brief (AR) تحويل لنص / (EN) Convert to string
     */
    std::string toString() const;
};

/**
 * @struct ResolvedImport
 * @brief (AR) استيراد محلول / (EN) Resolved import
 */
struct ResolvedImport {
    std::string moduleName;                     ///< اسم الوحدة المستوردة
    std::string alias;                          ///< الاسم المستعار (إن وُجد)
    std::vector<std::string> importedSymbols;   ///< الرموز المستوردة (للاستيراد الجزئي)
    Module* module;                             ///< مؤشر للوحدة المحملة
    bool isWildcard;                            ///< استيراد كامل (from X import *)
};

/**
 * @class SymbolResolver
 * @brief (AR) محلل الرموز / (EN) Symbol resolver
 * 
 * @details
 * (AR) يحلل ويدير الرموز من الوحدات المستوردة.
 *      يوفر:
 *      - حل الرموز عبر الوحدات
 *      - الوصول السريع للرموز المُصدّرة
 *      - التحقق من صحة الاستيراد
 *      - اقتراحات الإكمال التلقائي
 * 
 * (EN) Resolves and manages symbols from imported modules.
 *      Provides:
 *      - Cross-module symbol resolution
 *      - Fast access to exported symbols
 *      - Import validation
 *      - Auto-completion suggestions
 */
class SymbolResolver {
public:
    SymbolResolver();
    ~SymbolResolver();
    
    /**
     * @brief (AR) تحليل رموز وحدة / (EN) Analyze module symbols
     * @param module الوحدة للتحليل / Module to analyze
     */
    void analyzeModule(Module* module);
    
    /**
     * @brief (AR) حل استيرادات وحدة / (EN) Resolve module imports
     * @param module الوحدة / Module
     * @param moduleLoader محمل الوحدات / Module loader
     */
    void resolveImports(Module* module, class ModuleLoader* moduleLoader);
    
    /**
     * @brief (AR) البحث عن رمز / (EN) Find symbol
     * @param name اسم الرمز / Symbol name
     * @param currentModule الوحدة الحالية / Current module
     * @return معلومات الرمز أو nullopt / Symbol info or nullopt
     */
    std::optional<SymbolInfo> findSymbol(
        const std::string& name,
        const Module* currentModule) const;
    
    /**
     * @brief (AR) البحث عن رمز مؤهل / (EN) Find qualified symbol
     * @param qualifiedName الاسم المؤهل (module.symbol) / Qualified name
     * @return معلومات الرمز أو nullopt / Symbol info or nullopt
     */
    std::optional<SymbolInfo> findQualifiedSymbol(
        const std::string& qualifiedName) const;
    
    /**
     * @brief (AR) الحصول على جميع رموز وحدة / (EN) Get all module symbols
     * @param moduleName اسم الوحدة / Module name
     * @return قائمة الرموز / Symbol list
     */
    std::vector<SymbolInfo> getModuleSymbols(
        const std::string& moduleName) const;
    
    /**
     * @brief (AR) الحصول على الرموز المُصدّرة فقط / (EN) Get exported symbols only
     * @param moduleName اسم الوحدة / Module name
     * @return قائمة الرموز المُصدّرة / Exported symbol list
     */
    std::vector<SymbolInfo> getExportedSymbols(
        const std::string& moduleName) const;
    
    /**
     * @brief (AR) الحصول على اقتراحات الإكمال / (EN) Get completion suggestions
     * @param prefix البادئة / Prefix
     * @param currentModule الوحدة الحالية / Current module
     * @return قائمة الاقتراحات / Suggestion list
     */
    std::vector<SymbolInfo> getCompletionSuggestions(
        const std::string& prefix,
        const Module* currentModule) const;
    
    /**
     * @brief (AR) التحقق من صحة الاستيراد / (EN) Validate import
     * @param importStmt جملة الاستيراد / Import statement
     * @param currentModule الوحدة الحالية / Current module
     * @return رسالة خطأ أو nullopt إذا صحيح / Error message or nullopt if valid
     */
    std::optional<std::string> validateImport(
        const AST::ImportStmt* importStmt,
        const Module* currentModule) const;
    
    /**
     * @brief (AR) مسح جدول الرموز / (EN) Clear symbol table
     */
    void clear();
    
    /**
     * @brief (AR) طباعة جدول الرموز / (EN) Print symbol table
     */
    void printSymbolTable() const;
    
private:
    // جدول الرموز: moduleName -> vector<SymbolInfo>
    // Symbol table: moduleName -> vector<SymbolInfo>
    std::map<std::string, std::vector<SymbolInfo>> symbolTable_;
    
    // الاستيرادات المحلولة: moduleName -> vector<ResolvedImport>
    // Resolved imports: moduleName -> vector<ResolvedImport>
    std::map<std::string, std::vector<ResolvedImport>> resolvedImports_;
    
    /**
     * @brief (AR) استخراج رموز من AST / (EN) Extract symbols from AST
     */
    std::vector<SymbolInfo> extractSymbols(
        const Module* module,
        const std::vector<AST::StmtPtr>& ast);
    
    /**
     * @brief (AR) استخراج رمز من تعريف / (EN) Extract symbol from declaration
     */
    std::optional<SymbolInfo> extractSymbolFromDecl(
        const Module* module,
        AST::ASTNode* decl);
    
    /**
     * @brief (AR) حل استيراد واحد / (EN) Resolve single import
     */
    ResolvedImport resolveImport(
        const AST::ImportStmt* importStmt,
        const Module* currentModule,
        ModuleLoader* moduleLoader);
    
    /**
     * @brief (AR) التحقق من تصدير رمز / (EN) Check if symbol is exported
     */
    bool isSymbolExported(
        const std::string& symbolName,
        const Module* module) const;
};

} // namespace Modules
} // namespace Sad
