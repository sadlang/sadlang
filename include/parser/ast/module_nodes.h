/**
 * @file module_nodes.h
 * @brief (AR) عُقد AST لنظام الوحدات (الاستيراد والتصدير)
 *        (EN) AST nodes for module system (import and export)
 * 
 * @details
 * (AR) يحتوي هذا الملف على تعريفات عُقد شجرة AST الخاصة بنظام الوحدات في لغة ص.
 *      يدعم النظام الأنواع التالية من الاستيراد:
 *      1. استيراد كامل: استورد رياضيات
 *      2. استيراد مع اسم مستعار: استورد رياضيات كـ م
 *      3. استيراد انتقائي: من رياضيات استورد جذر، قوة
 *      4. استيراد انتقائي مع أسماء مستعارة: من رياضيات استورد جذر كـ ج
 *      5. استيراد كامل بـ *: من رياضيات استورد *
 * 
 *      ويدعم التصدير:
 *      - تصدير الدوال: صدّر دالة حساب(...)
 *      - تصدير المتغيرات: صدّر ثابت PI = 3.14
 *      - تصدير الأصناف: صدّر صنف شكل
 * 
 * (EN) This file contains AST node definitions for Sad language module system.
 *      The system supports the following import types:
 *      1. Full import: استورد رياضيات (import math)
 *      2. Import with alias: استورد رياضيات كـ م (import math as m)
 *      3. Selective import: من رياضيات استورد جذر، قوة (from math import sqrt, pow)
 *      4. Selective with alias: من رياضيات استورد جذر كـ ج (from math import sqrt as s)
 *      5. Wildcard import: من رياضيات استورد * (from math import *)
 * 
 *      And supports export:
 *      - Export functions: صدّر دالة حساب(...) (export function calc)
 *      - Export variables: صدّر ثابت PI = 3.14 (export const PI)
 *      - Export classes: صدّر صنف شكل (export class Shape)
 * 
 * @author Sad Language Development Team / فريق تطوير لغة ص
 * @date December 2025 / ديسمبر 2025
 * @version 1.0.0 - Phase 1: Import/Export System
 * 
 * @see docs/language_spec/rules/08_modules_and_ffi.md
 * @see docs/import_export_plan/phase1_import_export.md
 * 
 * @example استيراد كامل / Full Import
 * @code{.sad}
 * استورد رياضيات
 * اطبع(رياضيات.جذر(16))  // 4
 * @endcode
 * 
 * @example استيراد انتقائي / Selective Import
 * @code{.sad}
 * من رياضيات استورد جذر، قوة
 * اطبع(جذر(16))    // 4
 * اطبع(قوة(2، 3))   // 8
 * @endcode
 * 
 * @example تصدير / Export
 * @code{.sad}
 * صدّر دالة رقم مضاعفة(رقم س) {
 *     إرجاع س * 2
 * }
 * @endcode
 */

#ifndef SAD_AST_MODULE_NODES_H
#define SAD_AST_MODULE_NODES_H

#include "ast_node.h"
#include "ast_visitor.h"
#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace Sad {
namespace AST {

// Forward declarations
class Statement;

// =========================================================================
// (AR) هياكل مساعدة / (EN) Helper Structures
// =========================================================================

/**
 * @struct ImportItem
 * @brief (AR) عنصر مستورد واحد (مع اسم مستعار اختياري)
 *        (EN) Single imported item (with optional alias)
 * 
 * @details
 * (AR) يمثل رمز واحد يتم استيراده من وحدة، مع إمكانية تسميته باسم مختلف.
 *      مثال: من رياضيات استورد جذر كـ ج
 *      - name = "جذر"
 *      - alias = "ج"
 * 
 * (EN) Represents a single symbol imported from a module, with optional renaming.
 *      Example: from math import sqrt as s
 *      - name = "sqrt"
 *      - alias = "s"
 * 
 * @example
 * @code{.cpp}
 * ImportItem item;
 * item.name = "جذر";        // Original name
 * item.alias = "ج";         // Alias
 * item.position = token.position;
 * @endcode
 */
struct ImportItem {
    std::string name;                    ///< (AR) الاسم الأصلي / (EN) Original name
    std::optional<std::string> alias;    ///< (AR) الاسم المستعار (اختياري) / (EN) Alias (optional)
    Lexer::Position position;            ///< (AR) موقع العنصر في الكود / (EN) Position in source code
    
    /**
     * @brief (AR) البناء الافتراضي / (EN) Default constructor
     */
    ImportItem() = default;
    
    /**
     * @brief (AR) بناء مع الاسم فقط / (EN) Constructor with name only
     * @param n (AR) الاسم / (EN) Name
     */
    explicit ImportItem(const std::string& n) 
        : name(n), alias(std::nullopt) {}
    
    /**
     * @brief (AR) بناء مع الاسم والاسم المستعار / (EN) Constructor with name and alias
     * @param n (AR) الاسم الأصلي / (EN) Original name
     * @param a (AR) الاسم المستعار / (EN) Alias
     */
    ImportItem(const std::string& n, const std::string& a) 
        : name(n), alias(a) {}
    
    /**
     * @brief (AR) الحصول على الاسم الفعال (المستعار إن وُجد، وإلا الأصلي)
     *        (EN) Get effective name (alias if present, otherwise original)
     * @return (AR) الاسم الذي سيُستخدم في الكود / (EN) Name to be used in code
     */
    std::string getEffectiveName() const {
        return alias.value_or(name);
    }
    
    /**
     * @brief (AR) تحويل لنص / (EN) Convert to string
     * @return (AR) تمثيل نصي / (EN) String representation
     * 
     * @example
     * "sqrt as s" or "جذر كـ ج"
     */
    std::string toString() const {
        if (alias.has_value()) {
            return name + " كـ " + alias.value();
        }
        return name;
    }
};

// =========================================================================
// (AR) جملة الاستيراد الأساسية / (EN) Basic Import Statement
// =========================================================================

/**
 * @class ImportStmt
 * @brief (AR) عقدة جملة الاستيراد - استورد [module] [كـ alias]
 *        (EN) Import statement node - import [module] [as alias]
 * 
 * @details
 * (AR) تمثل جملة استيراد كاملة لوحدة، مع إمكانية تسميتها باسم مختلف.
 *      الأمثلة:
 *      - استورد رياضيات              → modulePath=["رياضيات"], alias=nullopt
 *      - استورد رياضيات كـ م          → modulePath=["رياضيات"], alias="م"
 *      - استورد مكتبتي.أدوات.مساعدة  → modulePath=["مكتبتي", "أدوات", "مساعدة"]
 * 
 * (EN) Represents a full module import statement with optional alias.
 *      Examples:
 *      - import math                → modulePath=["math"], alias=nullopt
 *      - import math as m           → modulePath=["math"], alias="m"
 *      - import mylib.utils.helper  → modulePath=["mylib", "utils", "helper"]
 * 
 * @note
 * (AR) مسار الوحدة (modulePath) هو قائمة من السلاسل النصية لدعم الوحدات المتداخلة.
 *      مثال: "مكتبتي.أدوات" → ["مكتبتي", "أدوات"]
 * 
 * (EN) Module path is a vector of strings to support nested modules.
 *      Example: "mylib.utils" → ["mylib", "utils"]
 * 
 * @example AST Creation
 * @code{.cpp}
 * // استورد رياضيات كـ م
 * auto importStmt = std::make_unique<ImportStmt>(
 *     std::vector<std::string>{"رياضيات"},  // Module path
 *     "م",                                    // Alias
 *     token.position
 * );
 * @endcode
 */
class ImportStmt : public Statement {
public:
    std::vector<std::string> modulePath;     ///< (AR) مسار الوحدة / (EN) Module path (e.g., ["math", "advanced"])
    std::optional<std::string> alias;        ///< (AR) الاسم المستعار / (EN) Alias (optional)
    
    /**
     * @brief (AR) البناء / (EN) Constructor
     * @param path (AR) مسار الوحدة / (EN) Module path
     * @param moduleAlias (AR) الاسم المستعار (اختياري) / (EN) Alias (optional)
     * @param pos (AR) الموقع في الكود / (EN) Source position
     */
    ImportStmt(std::vector<std::string> path,
               std::optional<std::string> moduleAlias = std::nullopt,
               const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), 
          modulePath(std::move(path)), 
          alias(std::move(moduleAlias)) {}
    
    /**
     * @brief (AR) قبول الزائر / (EN) Accept visitor
     * @param visitor (AR) الزائر / (EN) Visitor
     */
    void accept(ASTVisitor& visitor) override {
        visitor.visitImportStmt(*this);
    }
    
    /**
     * @brief (AR) تحويل لنص / (EN) Convert to string
     * @return (AR) تمثيل نصي للجملة / (EN) String representation
     * 
     * @example
     * "import math as m" or "استورد رياضيات كـ م"
     */
    std::string toString() const override {
        std::string result = "استورد ";
        
        // Join module path with dots / دمج مسار الوحدة بنقاط
        for (size_t i = 0; i < modulePath.size(); ++i) {
            if (i > 0) result += ".";
            result += modulePath[i];
        }
        
        // Add alias if present / إضافة الاسم المستعار إن وُجد
        if (alias.has_value()) {
            result += " كـ " + alias.value();
        }
        
        return result;
    }
    
    /**
     * @brief (AR) الحصول على الاسم الكامل للوحدة / (EN) Get full module name
     * @return (AR) اسم الوحدة الكامل / (EN) Full module name
     * 
     * @example
     * ["mylib", "utils", "math"] → "mylib.utils.math"
     */
    std::string getFullModuleName() const {
        std::string result;
        for (size_t i = 0; i < modulePath.size(); ++i) {
            if (i > 0) result += ".";
            result += modulePath[i];
        }
        return result;
    }
    
    /**
     * @brief (AR) الحصول على الاسم الفعال للوحدة في Namespace الحالي
     *        (EN) Get effective name of module in current namespace
     * @return (AR) الاسم الذي سيُستخدم / (EN) Name to be used
     * 
     * @details
     * (AR) يُرجع الاسم المستعار إن وُجد، وإلا آخر جزء من المسار.
     *      مثال: "مكتبتي.أدوات" بدون alias → "أدوات"
     *      مثال: "رياضيات" كـ "م" → "م"
     * 
     * (EN) Returns alias if present, otherwise last part of path.
     *      Example: "mylib.utils" without alias → "utils"
     *      Example: "math" as "m" → "m"
     */
    std::string getEffectiveName() const {
        return alias.value_or(modulePath.back());
    }
};

// =========================================================================
// (AR) جملة الاستيراد الانتقائي / (EN) Selective Import Statement (from...import)
// =========================================================================

/**
 * @class FromImportStmt
 * @brief (AR) عقدة جملة الاستيراد الانتقائي - من [module] استورد [items]
 *        (EN) From-import statement node - from [module] import [items]
 * 
 * @details
 * (AR) تمثل جملة استيراد انتقائي (استيراد رموز محددة من وحدة).
 *      الأمثلة:
 *      - من رياضيات استورد جذر، قوة
 *      - من رياضيات استورد جذر كـ ج
 *      - من رياضيات استورد *
 * 
 * (EN) Represents a selective import statement (import specific symbols from module).
 *      Examples:
 *      - from math import sqrt, pow
 *      - from math import sqrt as s
 *      - from math import *
 * 
 * @note
 * (AR) استيراد * (wildcard) يُمثَّل بقائمة فارغة من العناصر + علامة isWildcard=true
 * (EN) Wildcard import (*) is represented by empty items list + isWildcard=true flag
 * 
 * @example AST Creation
 * @code{.cpp}
 * // من رياضيات استورد جذر، قوة كـ أس
 * std::vector<ImportItem> items;
 * items.push_back(ImportItem("جذر"));
 * items.push_back(ImportItem("قوة", "أس"));
 * 
 * auto stmt = std::make_unique<FromImportStmt>(
 *     std::vector<std::string>{"رياضيات"},
 *     std::move(items),
 *     false,  // not wildcard
 *     token.position
 * );
 * @endcode
 */
class FromImportStmt : public Statement {
public:
    std::vector<std::string> modulePath;     ///< (AR) مسار الوحدة المصدر / (EN) Source module path
    std::vector<ImportItem> items;           ///< (AR) العناصر المستوردة / (EN) Imported items
    bool isWildcard;                         ///< (AR) هل استيراد * / (EN) Is wildcard import (*)
    
    /**
     * @brief (AR) البناء / (EN) Constructor
     * @param path (AR) مسار الوحدة / (EN) Module path
     * @param importItems (AR) العناصر المستوردة / (EN) Items to import
     * @param wildcard (AR) استيراد * / (EN) Wildcard import
     * @param pos (AR) الموقع في الكود / (EN) Source position
     */
    FromImportStmt(std::vector<std::string> path,
                   std::vector<ImportItem> importItems,
                   bool wildcard = false,
                   const Lexer::Position& pos = Lexer::Position())
        : Statement(pos),
          modulePath(std::move(path)),
          items(std::move(importItems)),
          isWildcard(wildcard) {}
    
    /**
     * @brief (AR) قبول الزائر / (EN) Accept visitor
     * @param visitor (AR) الزائر / (EN) Visitor
     */
    void accept(ASTVisitor& visitor) override {
        visitor.visitFromImportStmt(*this);
    }
    
    /**
     * @brief (AR) تحويل لنص / (EN) Convert to string
     * @return (AR) تمثيل نصي / (EN) String representation
     * 
     * @example
     * "from math import sqrt, pow as p" or "من رياضيات استورد جذر، قوة كـ أس"
     */
    std::string toString() const override {
        std::string result = "من ";
        
        // Join module path / دمج مسار الوحدة
        for (size_t i = 0; i < modulePath.size(); ++i) {
            if (i > 0) result += ".";
            result += modulePath[i];
        }
        
        result += " استورد ";
        
        // Wildcard import / استيراد *
        if (isWildcard) {
            result += "*";
        } else {
            // List items / عرض العناصر
            for (size_t i = 0; i < items.size(); ++i) {
                if (i > 0) result += "، ";
                result += items[i].toString();
            }
        }
        
        return result;
    }
    
    /**
     * @brief (AR) الحصول على الاسم الكامل للوحدة / (EN) Get full module name
     * @return (AR) اسم الوحدة الكامل / (EN) Full module name
     */
    std::string getFullModuleName() const {
        std::string result;
        for (size_t i = 0; i < modulePath.size(); ++i) {
            if (i > 0) result += ".";
            result += modulePath[i];
        }
        return result;
    }
};

// =========================================================================
// (AR) جملة التصدير / (EN) Export Statement
// =========================================================================

/**
 * @class ExportDecl
 * @brief (AR) عقدة تصريح التصدير - صدّر [declaration]
 *        (EN) Export declaration node - export [declaration]
 * 
 * @details
 * (AR) تمثل تصريح تصدير (جعل رمز متاحاً للوحدات الأخرى).
 *      الأمثلة:
 *      - صدّر دالة حساب(...)
 *      - صدّر صنف شكل { ... }
 *      - صدّر ثابت PI = 3.14
 * 
 * (EN) Represents an export declaration (make symbol available to other modules).
 *      Examples:
 *      - export function calc(...)
 *      - export class Shape { ... }
 *      - export const PI = 3.14
 * 
 * @note
 * (AR) التصريح الداخلي (declaration) يمكن أن يكون:
 *      - FunctionDecl (تصريح دالة)
 *      - ClassDecl (تصريح صنف)
 *      - VarDeclStmt (تصريح متغير/ثابت)
 * 
 * (EN) Inner declaration can be:
 *      - FunctionDecl (function declaration)
 *      - ClassDecl (class declaration)
 *      - VarDeclStmt (variable/const declaration)
 * 
 * @example AST Creation
 * @code{.cpp}
 * // صدّر دالة رقم مضاعفة(رقم س)
 * auto funcDecl = std::make_unique<FunctionDecl>(...);
 * auto exportDecl = std::make_unique<ExportDecl>(
 *     std::move(funcDecl),
 *     token.position
 * );
 * @endcode
 */
class ExportDecl : public Statement {
public:
    StmtPtr declaration;    ///< (AR) التصريح المُصدَّر / (EN) Exported declaration
    
    /**
     * @brief (AR) البناء / (EN) Constructor
     * @param decl (AR) التصريح المراد تصديره / (EN) Declaration to export
     * @param pos (AR) الموقع في الكود / (EN) Source position
     */
    ExportDecl(StmtPtr decl, const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), declaration(std::move(decl)) {}
    
    /**
     * @brief (AR) قبول الزائر / (EN) Accept visitor
     * @param visitor (AR) الزائر / (EN) Visitor
     */
    void accept(ASTVisitor& visitor) override {
        visitor.visitExportDecl(*this);
    }
    
    /**
     * @brief (AR) تحويل لنص / (EN) Convert to string
     * @return (AR) تمثيل نصي / (EN) String representation
     * 
     * @example
     * "export function calc(...)" or "صدّر دالة حساب(...)"
     */
    std::string toString() const override {
        return "صدّر " + (declaration ? declaration->toString() : "<null>");
    }
    
    /**
     * @brief (AR) الحصول على اسم الرمز المُصدَّر / (EN) Get exported symbol name
     * @return (AR) اسم الرمز / (EN) Symbol name
     * 
     * @details
     * (AR) يستخرج اسم الرمز من التصريح الداخلي (دالة/صنف/متغير).
     * (EN) Extracts symbol name from inner declaration (function/class/variable).
     */
    std::string getExportedName() const;
};

// =========================================================================
// (AR) أنواع مؤشرات ذكية / (EN) Smart Pointer Type Aliases
// =========================================================================

using ImportStmtPtr = std::unique_ptr<ImportStmt>;
using FromImportStmtPtr = std::unique_ptr<FromImportStmt>;
using ExportDeclPtr = std::unique_ptr<ExportDecl>;

} // namespace AST
} // namespace Sad

#endif // SAD_AST_MODULE_NODES_H
