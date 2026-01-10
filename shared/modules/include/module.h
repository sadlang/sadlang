/**
 * @file module.h
 * @brief (AR) بنية الوحدة الأساسية في نظام الوحدات
 *        (EN) Core module structure for the module system
 * 
 * @details
 * (AR) يحتوي على تعريف الوحدة الأساسية التي تمثل ملف كود ص مُحلَّل.
 *      كل وحدة تحتوي على:
 *      - AST (الشجرة النحوية)
 *      - جدول الرموز (Symbols)
 *      - الاعتماديات (Dependencies)
 *      - معلومات التصدير (Exports)
 * 
 * (EN) Contains the core module definition representing a parsed Sad code file.
 *      Each module contains:
 *      - AST (Abstract Syntax Tree)
 *      - Symbol table
 *      - Dependencies
 *      - Export information
 * 
 * @author Sad Language Development Team
 * @date December 2025
 * @version 2.0.0 - Phase 2: Module Loader
 */

#ifndef SAD_MODULE_H
#define SAD_MODULE_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include "ast_node.h"
#include "module_nodes.h"

namespace Sad {
namespace Modules {

// Forward declarations
class ModuleLoader;

/**
 * @struct ModuleMetadata
 * @brief (AR) بيانات وصفية عن الوحدة / (EN) Module metadata
 */
struct ModuleMetadata {
    std::string name;                    ///< (AR) اسم الوحدة / (EN) Module name
    std::string version;                 ///< (AR) الإصدار / (EN) Version
    std::string filePath;                ///< (AR) المسار الكامل للملف / (EN) Full file path
    size_t fileSize;                     ///< (AR) حجم الملف / (EN) File size
    std::string checksum;                ///< (AR) المجموع الاختباري / (EN) Checksum
    
    std::chrono::system_clock::time_point loadTime;     ///< (AR) وقت التحميل / (EN) Load time
    std::chrono::system_clock::time_point fileModTime;  ///< (AR) وقت آخر تعديل / (EN) Last modification time
    
    /**
     * @brief (AR) تحويل لنص / (EN) Convert to string
     */
    std::string toString() const;
};

/**
 * @struct ExportedSymbol
 * @brief (AR) رمز مُصدَّر من الوحدة / (EN) Exported symbol from module
 */
struct ExportedSymbol {
    enum class Type {
        FUNCTION,    ///< دالة / Function
        CLASS,       ///< صنف / Class
        VARIABLE,    ///< متغير / Variable
        CONSTANT     ///< ثابت / Constant
    };
    
    std::string name;        ///< (AR) اسم الرمز / (EN) Symbol name
    Type type;               ///< (AR) نوع الرمز / (EN) Symbol type
    AST::StmtPtr declaration; ///< (AR) التصريح الأصلي / (EN) Original declaration
    
    /**
     * @brief (AR) تحويل لنص / (EN) Convert to string
     */
    std::string toString() const;
};

/**
 * @class Module
 * @brief (AR) وحدة كود ص مُحلَّلة / (EN) Parsed Sad code module
 * 
 * @details
 * (AR) تمثل ملف كود واحد مُحلَّل مع كل معلوماته:
 *      - الشجرة النحوية (AST)
 *      - الرموز المُصدَّرة
 *      - الوحدات المُستوردة (الاعتماديات)
 *      - معلومات وصفية
 * 
 * (EN) Represents a single parsed code file with all its information:
 *      - Abstract Syntax Tree (AST)
 *      - Exported symbols
 *      - Imported modules (dependencies)
 *      - Metadata
 */
class Module {
public:
    /**
     * @brief (AR) البناء / (EN) Constructor
     * @param name (AR) اسم الوحدة / (EN) Module name
     * @param filePath (AR) مسار الملف / (EN) File path
     */
    explicit Module(const std::string& name, const std::string& filePath = "");
    
    /**
     * @brief (AR) الحصول على الاسم / (EN) Get name
     */
    const std::string& getName() const { return metadata_.name; }
    
    /**
     * @brief (AR) الحصول على المسار / (EN) Get file path
     */
    const std::string& getFilePath() const { return metadata_.filePath; }
    
    /**
     * @brief (AR) الحصول على البيانات الوصفية / (EN) Get metadata
     */
    const ModuleMetadata& getMetadata() const { return metadata_; }
    
    /**
     * @brief (AR) تعيين AST / (EN) Set AST
     */
    void setAST(std::vector<AST::StmtPtr> ast);
    
    /**
     * @brief (AR) الحصول على AST / (EN) Get AST
     */
    const std::vector<AST::StmtPtr>& getAST() const { return ast_; }
    
    /**
     * @brief (AR) إضافة رمز مُصدَّر / (EN) Add exported symbol
     */
    void addExport(const std::string& name, ExportedSymbol::Type type, AST::StmtPtr declaration);
    
    /**
     * @brief (AR) الحصول على الرموز المُصدَّرة / (EN) Get exported symbols
     */
    const std::map<std::string, ExportedSymbol>& getExports() const { return exports_; }
    
    /**
     * @brief (AR) البحث عن رمز مُصدَّر / (EN) Find exported symbol
     */
    const ExportedSymbol* findExport(const std::string& name) const;
    
    /**
     * @brief (AR) إضافة اعتمادية (وحدة مُستوردة) / (EN) Add dependency (imported module)
     */
    void addDependency(const std::string& moduleName);
    
    /**
     * @brief (AR) الحصول على الاعتماديات / (EN) Get dependencies
     */
    const std::vector<std::string>& getDependencies() const { return dependencies_; }
    
    /**
     * @brief (AR) هل الوحدة صالحة؟ / (EN) Is module valid?
     */
    bool isValid() const;
    
    /**
     * @brief (AR) تحديث البيانات الوصفية / (EN) Update metadata
     */
    void updateMetadata();
    
    /**
     * @brief (AR) تحليل الاعتماديات من AST / (EN) Analyze dependencies from AST
     * 
     * @details
     * (AR) يمر على AST ويستخرج جميع جمل الاستيراد لبناء قائمة الاعتماديات
     * (EN) Walks the AST and extracts all import statements to build dependency list
     */
    void analyzeDependencies();
    
    /**
     * @brief (AR) تحليل التصديرات من AST / (EN) Analyze exports from AST
     */
    void analyzeExports();
    
    /**
     * @brief (AR) تحويل لنص / (EN) Convert to string
     */
    std::string toString() const;

private:
    ModuleMetadata metadata_;                        ///< (AR) البيانات الوصفية / (EN) Metadata
    std::vector<AST::StmtPtr> ast_;                  ///< (AR) الشجرة النحوية / (EN) AST
    std::map<std::string, ExportedSymbol> exports_;  ///< (AR) الرموز المُصدَّرة / (EN) Exported symbols
    std::vector<std::string> dependencies_;          ///< (AR) الاعتماديات / (EN) Dependencies
    
    /**
     * @brief (AR) حساب checksum للملف / (EN) Calculate file checksum
     */
    std::string calculateChecksum() const;
};

} // namespace Modules
} // namespace Sad

#endif // SAD_MODULE_H
