/**
 * @file module_nodes.cpp
 * @brief (AR) تنفيذ عُقد AST لنظام الوحدات
 *        (EN) Implementation of AST nodes for module system
 * 
 * @author Sad Language Development Team / فريق تطوير لغة ص
 * @date December 2025 / ديسمبر 2025
 */

#include "../../../include/parser/ast/module_nodes.h"
#include "../../../include/parser/ast/declarations.h"
#include "../../../include/parser/ast/statements.h"

namespace Sad {
namespace AST {

/**
 * @brief (AR) الحصول على اسم الرمز المُصدَّر من تصريح التصدير
 *        (EN) Get exported symbol name from export declaration
 * 
 * @details
 * (AR) يفحص نوع التصريح الداخلي ويستخرج اسم الرمز المُصدَّر:
 *      - إذا كان دالة → اسم الدالة
 *      - إذا كان صنف → اسم الصنف
 *      - إذا كان متغير → اسم المتغير
 * 
 * (EN) Examines inner declaration type and extracts exported symbol name:
 *      - If function → function name
 *      - If class → class name
 *      - If variable → variable name
 * 
 * @return (AR) اسم الرمز المُصدَّر، أو سلسلة فارغة إن لم يُعرف
 *         (EN) Exported symbol name, or empty string if unknown
 * 
 * @example
 * @code{.cpp}
 * // صدّر دالة حساب(...)
 * ExportDecl exportDecl(...);
 * std::string name = exportDecl.getExportedName(); // "حساب"
 * @endcode
 */
std::string ExportDecl::getExportedName() const {
    if (!declaration) {
        return "";
    }
    
    // (AR) محاولة تحويل إلى تصريح دالة / (EN) Try casting to function declaration
    if (auto* funcDecl = dynamic_cast<FunctionDecl*>(declaration.get())) {
        return funcDecl->name;
    }
    
    // (AR) محاولة تحويل إلى تصريح صنف / (EN) Try casting to class declaration
    if (auto* classDecl = dynamic_cast<ClassDecl*>(declaration.get())) {
        return classDecl->name;
    }
    
    // (AR) محاولة تحويل إلى تصريح متغير / (EN) Try casting to variable declaration
    if (auto* varDecl = dynamic_cast<VarDeclStmt*>(declaration.get())) {
        return varDecl->name;
    }
    
    // (AR) نوع غير معروف / (EN) Unknown type
    return "";
}

} // namespace AST
} // namespace Sad
