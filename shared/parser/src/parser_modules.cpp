/**
 * @file parser_modules.cpp
 * @brief (AR) تنفيذ دوال تحليل نظام الوحدات (استيراد وتصدير)
 *        (EN) Implementation of module system parsing functions (import and export)
 * 
 * @author Sad Language Development Team / فريق تطوير لغة ص
 * @date December 2025 / ديسمبر 2025
 */

#include "parser_core.h"
#include "module_nodes.h"

namespace Sad {
namespace Parser {

using namespace Lexer;
using namespace AST;
using TT = TokenType;

// =========================================================================
// (AR) تحليل جملة الاستيراد / (EN) Parse Import Statement
// =========================================================================

/**
 * @brief (AR) يحلل جملة استيراد - استورد [وحدة] [كـ اسم]
 *        (EN) Parses import statement - import [module] [as name]
 * 
 * @details
 * Syntax / النحو:
 *   استورد وحدة
 *   استورد وحدة كـ اسم
 *   استورد وحدة.فرعية
 *   استورد وحدة.فرعية.متداخلة كـ اسم
 * 
 * Grammar / القواعد:
 *   ImportStmt := 'استورد' ModulePath ['كـ' IDENTIFIER]
 *   ModulePath := IDENTIFIER ('.' IDENTIFIER)*
 */
StmtPtr ParserCore::parseImportStmt() {
    Position startPos = previous_.getPosition();
    
    // (AR) تحليل مسار الوحدة / (EN) Parse module path
    std::vector<std::string> modulePath;
    
    if (!check(TT::IDENTIFIER)) {
        error("(AR) متوقع اسم وحدة بعد 'استورد'. (EN) Expected module name after 'import'.");
        return nullptr;
    }
    
    modulePath.push_back(current_.getValue());
    advance();
    
    // (AR) تحليل المسارات المتداخلة (وحدة.فرعية.متداخلة)
    // (EN) Parse nested paths (module.sub.nested)
    while (match(TT::DOT)) {
        if (!check(TT::IDENTIFIER)) {
            error("(AR) متوقع اسم وحدة بعد '.'. (EN) Expected module name after '.'.");
            return nullptr;
        }
        
        modulePath.push_back(current_.getValue());
        advance();
    }
    
    // (AR) تحليل الاسم المستعار الاختياري / (EN) Parse optional alias
    std::optional<std::string> alias = std::nullopt;
    
    if (match(TT::KEYWORD_AS)) {
        if (!check(TT::IDENTIFIER)) {
            error("(AR) متوقع اسم مستعار بعد 'كـ'. (EN) Expected alias name after 'as'.");
            return nullptr;
        }
        
        alias = current_.getValue();
        advance();
    }
    
    // (AR) إنشاء عقدة ImportStmt / (EN) Create ImportStmt node
    auto importStmt = std::make_unique<ImportStmt>(
        std::move(modulePath),
        std::move(alias),
        startPos
    );
    
    return importStmt;
}

// =========================================================================
// (AR) تحليل جملة الاستيراد الانتقائي / (EN) Parse From-Import Statement
// =========================================================================

/**
 * @brief (AR) يحلل جملة استيراد انتقائي - من [وحدة] استورد [عناصر]
 *        (EN) Parses from-import statement - from [module] import [items]
 * 
 * @details
 * Syntax / النحو:
 *   من وحدة استورد عنصر
 *   من وحدة استورد عنصر1، عنصر2
 *   من وحدة استورد عنصر كـ اسم
 *   من وحدة استورد *
 * 
 * Grammar / القواعد:
 *   FromImportStmt := 'من' ModulePath 'استورد' ImportItems
 *   ImportItems := '*' | ImportItem ('،' ImportItem)*
 *   ImportItem := IDENTIFIER ['كـ' IDENTIFIER]
 */
StmtPtr ParserCore::parseFromImportStmt() {
    Position startPos = previous_.getPosition();
    
    // (AR) تحليل مسار الوحدة / (EN) Parse module path
    std::vector<std::string> modulePath;
    
    if (!check(TT::IDENTIFIER)) {
        error("(AR) متوقع اسم وحدة بعد 'من'. (EN) Expected module name after 'from'.");
        return nullptr;
    }
    
    modulePath.push_back(current_.getValue());
    advance();
    
    // (AR) تحليل المسارات المتداخلة / (EN) Parse nested paths
    while (match(TT::DOT)) {
        if (!check(TT::IDENTIFIER)) {
            error("(AR) متوقع اسم وحدة بعد '.'. (EN) Expected module name after '.'.");
            return nullptr;
        }
        
        modulePath.push_back(current_.getValue());
        advance();
    }
    
    // (AR) يجب أن تتبعها كلمة 'استورد' / (EN) Must be followed by 'import'
    if (!match(TT::KEYWORD_IMPORT)) {
        error("(AR) متوقع 'استورد' بعد اسم الوحدة. (EN) Expected 'import' after module name.");
        return nullptr;
    }
    
    // (AR) فحص استيراد wildcard (*) / (EN) Check for wildcard import (*)
    if (match(TT::OP_MULTIPLY)) {
        // من وحدة استورد *
        auto fromImportStmt = std::make_unique<FromImportStmt>(
            std::move(modulePath),
            std::vector<ImportItem>(),  // قائمة فارغة / empty list
            true,  // isWildcard = true
            startPos
        );
        
        return fromImportStmt;
    }
    
    // (AR) تحليل قائمة العناصر / (EN) Parse items list
    std::vector<ImportItem> items;
    
    do {
        if (!check(TT::IDENTIFIER)) {
            error("(AR) متوقع اسم رمز بعد 'استورد'. (EN) Expected symbol name after 'import'.");
            return nullptr;
        }
        
        ImportItem item;
        item.name = current_.getValue();
        item.position = current_.getPosition();
        advance();
        
        // (AR) فحص الاسم المستعار / (EN) Check for alias
        if (match(TT::KEYWORD_AS)) {
            if (!check(TT::IDENTIFIER)) {
                error("(AR) متوقع اسم مستعار بعد 'كـ'. (EN) Expected alias name after 'as'.");
                return nullptr;
            }
            
            item.alias = current_.getValue();
            advance();
        }
        
        items.push_back(std::move(item));
        
        // (AR) فحص الفاصلة للعنصر التالي / (EN) Check for comma for next item
    } while (match(TT::COMMA) || match(TT::ARABIC_COMMA));
    
    // (AR) التحقق من وجود عناصر على الأقل / (EN) Verify at least one item
    if (items.empty()) {
        error("(AR) يجب استيراد عنصر واحد على الأقل. (EN) Must import at least one item.");
        return nullptr;
    }
    
    // (AR) إنشاء عقدة FromImportStmt / (EN) Create FromImportStmt node
    auto fromImportStmt = std::make_unique<FromImportStmt>(
        std::move(modulePath),
        std::move(items),
        false,  // isWildcard = false
        startPos
    );
    
    return fromImportStmt;
}

// =========================================================================
// (AR) تحليل تصريح التصدير / (EN) Parse Export Declaration
// =========================================================================

/**
 * @brief (AR) يحلل تصريح تصدير - صدّر [تصريح]
 *        (EN) Parses export declaration - export [declaration]
 * 
 * @details
 * Syntax / النحو:
 *   صدّر دالة اسم() { ... }
 *   صدّر صنف اسم { ... }
 *   صدّر ثابت PI = 3.14
 *   صدّر رقم س = 10
 * 
 * Grammar / القواعد:
 *   ExportDecl := 'صدّر' Declaration
 *   Declaration := FunctionDecl | ClassDecl | VarDecl
 */
StmtPtr ParserCore::parseExportDecl() {
    Position startPos = previous_.getPosition();
    
    // (AR) تحليل التصريح الداخلي / (EN) Parse inner declaration
    StmtPtr declaration = nullptr;
    
    // Parse the declaration that follows 'صدّر'
    // We need to advance past the keyword and call the appropriate parser
    if (match(TT::KEYWORD_FUNCTION)) {
        // صدّر دالة ... - Function keyword consumed, now parse the function
        declaration = parseFunctionDecl();
    }
    else if (match(TT::KEYWORD_CLASS)) {
        // صدّر صنف ... - Class keyword consumed, now parse the class
        declaration = parseClassDecl();
    }
    else if (match(TT::KEYWORD_CONST)) {
        // صدّر ثابت ... - Const keyword consumed, now parse variable
        declaration = parseVarDecl();
    }
    else if (isTypeToken(current_.getType())) {
        // صدّر رقم س = ... - Type token, parse as variable declaration
        declaration = parseVarDecl();
    }
    else {
        error(
            "(AR) متوقع تصريح بعد 'صدّر' (دالة، صنف، أو متغير).\n"
            "(EN) Expected declaration after 'export' (function, class, or variable)."
        );
        return nullptr;
    }
    
    // (AR) التحقق من نجاح التحليل / (EN) Verify parsing succeeded
    if (!declaration) {
        error("(AR) فشل تحليل التصريح المُصدَّر. (EN) Failed to parse exported declaration.");
        return nullptr;
    }
    
    // (AR) إنشاء عقدة ExportDecl / (EN) Create ExportDecl node
    auto exportDecl = std::make_unique<ExportDecl>(
        std::move(declaration),
        startPos
    );
    
    return exportDecl;
}

} // namespace Parser
} // namespace Sad
