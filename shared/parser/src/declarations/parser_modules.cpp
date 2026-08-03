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
#include "declarations.h"
#include "expressions.h"
#include "statements.h"
#include <optional>

namespace Sad {
namespace Parser {

using namespace Lexer;
using namespace AST;
using TT = TokenType;

// =========================================================================
// (AR) دالة مساعدة: تحليل مسار وحدة / (EN) Helper: Parse module path
// =========================================================================
bool ParserCore::parseModulePath(std::vector<std::string>& modulePath) {
    if (check(TT::STRING_LITERAL)) {
        std::string quotedPath = current_.getValue();
        advance();
        size_t start = 0;
        for (size_t i = 0; i <= quotedPath.size(); ++i) {
            if (i == quotedPath.size() || quotedPath[i] == '.' || 
                quotedPath[i] == '/' || quotedPath[i] == '\\') {
                if (i > start) {
                    modulePath.push_back(quotedPath.substr(start, i - start));
                }
                start = i + 1;
            }
        }
        if (modulePath.empty()) modulePath.push_back(quotedPath);
        return true;
    }
    
    // (AR) دعم الاستيراد النسبي: .وحدة أو ..وحدة
    // (EN) Support relative import: .module or ..module
    if (check(TT::DOT) || check(TT::DOT_DOT)) {
        std::string relativePrefix;
        if (match(TT::DOT_DOT)) {
            relativePrefix = "..";
        } else {
            advance(); // consume DOT
            relativePrefix = ".";
        }
        modulePath.push_back(relativePrefix);
        
        if (!check(TT::IDENTIFIER) && !isTokenUsableAsName(current_.getType())) {
            error("(AR) متوقع اسم وحدة بعد '" + relativePrefix + "'. الصيغة: استورد X من .وحدة_محلية\n"
                  "(EN) Expected module name after '" + relativePrefix + "'. Syntax: import X from .local_module");
            return false;
        }
        modulePath.push_back(current_.getValue());
        advance();
        
        while (match(TT::DOT)) {
            if (!check(TT::IDENTIFIER) && !isTokenUsableAsName(current_.getType())) {
                error("(AR) متوقع اسم وحدة بعد '.'. (EN) Expected module name after '.'.");
                return false;
            }
            modulePath.push_back(current_.getValue());
            advance();
        }
        return true;
    }
    
    if (!check(TT::IDENTIFIER) && !isTokenUsableAsName(current_.getType())) {
        error("(AR) متوقع اسم وحدة. (EN) Expected module name.");
        return false;
    }
    
    modulePath.push_back(current_.getValue());
    advance();
    
    while (match(TT::DOT)) {
        if (!check(TT::IDENTIFIER) && !isTokenUsableAsName(current_.getType())) {
            error("(AR) متوقع اسم وحدة بعد '.'. (EN) Expected module name after '.'.");
            return false;
        }
        modulePath.push_back(current_.getValue());
        advance();
    }
    
    return true;
}

// =========================================================================
// (AR) تحليل جملة الاستيراد / (EN) Parse Import Statement
// =========================================================================

/**
 * @brief (AR) يحلل جملة استيراد بالصيغ المتعددة
 *        (EN) Parses import statement in multiple forms
 * 
 * @details
 * الصيغ المدعومة / Supported syntax:
 *   استورد وحدة                         ← استيراد بسيط
 *   استورد وحدة كـ اسم                  ← استيراد مع اسم مستعار
 *   استورد وحدة.فرعية.متداخلة           ← استيراد مسار متداخل
 *   استورد * من وحدة                    ← استيراد شامل (صيغة عربية)
 *   استورد عنصر من وحدة                 ← استيراد انتقائي (صيغة عربية)
 *   استورد عنصر1، عنصر2 من وحدة         ← استيراد متعدد (صيغة عربية)
 *   استورد عنصر كـ بديل من وحدة          ← استيراد مع اسم مستعار (صيغة عربية)
 */
StmtPtr ParserCore::parseImportStmt() {
    Position startPos = previous_.getPosition();
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) الحالة 1: استورد * من وحدة — استيراد شامل بالصيغة العربية
    // (EN) Case 1: import * from module — Arabic wildcard import
    // ═══════════════════════════════════════════════════════════════════
    if (match(TT::OP_MULTIPLY)) {
        if (!match(TT::KEYWORD_FROM)) {
            error("(AR) متوقع 'من' بعد 'استورد *'. الصيغة: استورد * من وحدة\n"
                  "(EN) Expected 'from' after 'import *'. Syntax: import * from module");
            return nullptr;
        }
        
        // (AR) تحليل مسار الوحدة / (EN) Parse module path
        std::vector<std::string> modulePath;
        if (!parseModulePath(modulePath)) return nullptr;
        
        return std::make_unique<FromImportStmt>(
            std::move(modulePath),
            std::vector<ImportItem>(),
            true,  // isWildcard
            startPos
        );
    }
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) تحليل مسار أو اسم عنصر / (EN) Parse path or item name
    // ═══════════════════════════════════════════════════════════════════
    
    // (AR) دعم الاستيراد بالنص المقتبس: استورد "وحدة"
    // (EN) Support string-quoted import: import "module"
    if (check(TT::STRING_LITERAL)) {
        std::vector<std::string> modulePath;
        std::string quotedPath = current_.getValue();
        advance();
        size_t start = 0;
        for (size_t i = 0; i <= quotedPath.size(); ++i) {
            if (i == quotedPath.size() || quotedPath[i] == '.' || 
                quotedPath[i] == '/' || quotedPath[i] == '\\') {
                if (i > start) {
                    modulePath.push_back(quotedPath.substr(start, i - start));
                }
                start = i + 1;
            }
        }
        if (modulePath.empty()) modulePath.push_back(quotedPath);
        
        // (AR) التحقق من 'من' بعد النص المقتبس
        if (match(TT::KEYWORD_FROM)) {
            // (AR) استورد "عنصر" من وحدة — نمط نادر لكنه مدعوم
            std::vector<std::string> fromPath;
            if (!parseModulePath(fromPath)) return nullptr;
            
            std::vector<ImportItem> items;
            for (auto& p : modulePath) {
                items.push_back(ImportItem(p));
            }
            return std::make_unique<FromImportStmt>(
                std::move(fromPath), std::move(items), false, startPos);
        }
        
        std::optional<std::string> alias = std::nullopt;
        if (match(TT::KEYWORD_AS)) {
            if (!check(TT::IDENTIFIER)) {
                error("(AR) متوقع اسم مستعار بعد 'كـ'. (EN) Expected alias name after 'as'.");
                return nullptr;
            }
            alias = current_.getValue();
            advance();
        }
        return std::make_unique<ImportStmt>(std::move(modulePath), std::move(alias), startPos);
    }
    
    if (!check(TT::IDENTIFIER) && !isTokenUsableAsName(current_.getType())) {
        error("(AR) متوقع اسم وحدة أو عنصر بعد 'استورد'.\n"
              "    الصيغ المدعومة:\n"
              "    • استورد * من وحدة           (استيراد شامل)\n"
              "    • استورد عنصر من وحدة         (استيراد انتقائي)\n"
              "    • استورد عنصر1، عنصر2 من وحدة (استيراد متعدد)\n"
              "    • استورد وحدة                  (استيراد بسيط)\n"
              "(EN) Expected module or item name after 'import'.");
        return nullptr;
    }
    
    // (AR) حفظ المعرّف الأول / (EN) Save first identifier
    std::string firstName = current_.getValue();
    Position firstPos = current_.getPosition();
    advance();
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) الحالة 2: استورد وحدة.فرعية — مسار متداخل (الصيغة القديمة)
    // (EN) Case 2: import module.sub — dotted path (old syntax)
    // ═══════════════════════════════════════════════════════════════════
    if (check(TT::DOT)) {
        std::vector<std::string> modulePath;
        modulePath.push_back(firstName);
        
        while (match(TT::DOT)) {
            if (!check(TT::IDENTIFIER) && !isTokenUsableAsName(current_.getType())) {
                error("(AR) متوقع اسم وحدة بعد '.'. (EN) Expected module name after '.'.");
                return nullptr;
            }
            modulePath.push_back(current_.getValue());
            advance();
        }
        
        std::optional<std::string> alias = std::nullopt;
        if (match(TT::KEYWORD_AS)) {
            if (!check(TT::IDENTIFIER)) {
                error("(AR) متوقع اسم مستعار بعد 'كـ'. (EN) Expected alias name after 'as'.");
                return nullptr;
            }
            alias = current_.getValue();
            advance();
        }
        
        return std::make_unique<ImportStmt>(std::move(modulePath), std::move(alias), startPos);
    }
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) الحالة 3: استورد عنصر من وحدة — صيغة عربية انتقائية
    // (EN) Case 3: import item from module — Arabic selective import
    // ═══════════════════════════════════════════════════════════════════
    
    // (AR) التحقق: هل يلي المعرّف فاصلة (عدة عناصر) أو 'كـ' أو 'من'
    // (EN) Check: does comma (multiple items), 'as', or 'from' follow?

    // (AR) [ز.٣٣] «من» في سطرٍ تالٍ **تبدأ جملةً جديدة** لا تُكمِل هذه. فبلا هذا
    //      الحارس تُقرأ سطران متجاوران:
    //          استورد رياضيات
    //          من نصوص استورد قص
    //      جملةً واحدةً `استورد رياضيات من نصوص` ⇒ «الرمز 'رياضيات' غير موجود في
    //      الوحدة 'نصوص'»، وتبقى `استورد قص` جملةً ثانيةً ⇒ «لم يُعثر على الوحدة».
    //      والسطرُ الفارغُ لا يفصل، لأنّ المُعجِميَّ لا يُصدِر رمزَ نهايةِ سطرٍ أصلًا؛
    //      فالفصلُ يُستنبَط من `Position::line` وحدَه. و`previous_` هنا هو المعرّفُ
    //      الأوّلُ المُستهلَكُ للتوّ، فالمقارنةُ به لا تحتاج حالةً إضافيّة.
    //      الفاصلةُ **لا** تُحرَس: `استورد جمع،⏎ ضرب من حساب` قائمةٌ مشروعةٌ متعدّدةُ
    //      الأسطر، والفاصلةُ ذاتُها علامةُ استمرارٍ صريحة بخلاف بدايةِ جملةٍ بـ«من».
    // (EN) [ز.٣٣] A `من` on a LATER line starts a new statement rather than
    //      continuing this one. Without this guard two adjacent lines merge into a
    //      single `import X from Y`. A blank line does not separate them: the lexer
    //      emits no newline token at all, so separation is derived from
    //      `Position::line`. `previous_` is the identifier just consumed, so no
    //      extra state is needed. The comma is deliberately NOT guarded — a
    //      multi-line item list is legitimate and the comma itself marks
    //      continuation explicitly, unlike a line that merely begins with `من`.
    const bool fromOnSameLine =
        check(TT::KEYWORD_FROM) &&
        current_.getPosition().line == previous_.getPosition().line;

    if (checkComma() || fromOnSameLine || check(TT::KEYWORD_AS)) {
        // (AR) جمع قائمة العناصر / (EN) Collect items list
        std::vector<ImportItem> items;
        
        ImportItem firstItem;
        firstItem.name = firstName;
        firstItem.position = firstPos;
        
        // (AR) فحص 'كـ' للعنصر الأول / (EN) Check 'as' for first item
        if (match(TT::KEYWORD_AS)) {
            if (!check(TT::IDENTIFIER)) {
                error("(AR) متوقع اسم مستعار بعد 'كـ'. (EN) Expected alias.");
                return nullptr;
            }
            firstItem.alias = current_.getValue();
            advance();
        }
        items.push_back(std::move(firstItem));
        
        // (AR) قراءة بقية العناصر / (EN) Read remaining items
        while (matchComma()) {
            if (!check(TT::IDENTIFIER) && !isTokenUsableAsName(current_.getType())) {
                error("(AR) متوقع اسم عنصر بعد الفاصلة. (EN) Expected item name after comma.");
                return nullptr;
            }
            
            ImportItem item;
            item.name = current_.getValue();
            item.position = current_.getPosition();
            advance();
            
            if (match(TT::KEYWORD_AS)) {
                if (!check(TT::IDENTIFIER)) {
                    error("(AR) متوقع اسم مستعار. (EN) Expected alias.");
                    return nullptr;
                }
                item.alias = current_.getValue();
                advance();
            }
            items.push_back(std::move(item));
        }
        
        // (AR) الآن يجب أن تأتي 'من' / (EN) Now 'from' must follow
        //      [ز.٣٣ — تتمّة] عنصرٌ واحدٌ بلا فاصلة (`استورد وحدة كـ اسم`) لا لبسَ فيه:
        //      «من» في سطرٍ تالٍ تبدأ جملةً جديدة، تمامًا كالحارسِ عند مدخلِ الحالة ٣.
        //      بلا هذا يبقى العيبُ نفسُه حيًّا في مسارِ الاسمِ المستعار:
        //          استورد نصوص كـ ن
        //          من رياضيات استورد إشارة   ⇒ «الرمز 'نصوص' غير موجود في 'رياضيات'»
        //      أمّا القائمةُ متعدّدةُ العناصر فمُلتبِسةٌ حقًّا (`استورد جمع،⏎ ضرب⏎ من حساب`
        //      مشروعةٌ و«من» فيها في سطرٍ تالٍ)، فلا تُحرَس — قيدٌ معلَنٌ لا سهوٌ.
        // (EN) [ز.٣٣ — follow-up] A single item with no comma (`import module as name`) is
        //      unambiguous: a `من` on a later line starts a new statement, exactly as at the
        //      case-3 entry guard. Without this the same defect survives on the alias path.
        //      A multi-item list IS genuinely ambiguous (a legitimate list may span lines with
        //      `من` on a later one), so it is deliberately left unguarded — a declared limit.
        const bool singleItemNoComma = items.size() == 1;
        const bool fromContinuesThisImport =
            check(TT::KEYWORD_FROM) &&
            (!singleItemNoComma ||
             current_.getPosition().line == previous_.getPosition().line);

        if (!fromContinuesThisImport || !match(TT::KEYWORD_FROM)) {
            // (AR) ربما هي الصيغة القديمة: استورد وحدة كـ اسم (بدون 'من')
            // (EN) Maybe old syntax: import module as alias (without 'from')
            if (items.size() == 1 && items[0].alias.has_value()) {
                // (AR) استورد وحدة كـ اسم — الصيغة القديمة
                std::vector<std::string> modulePath = {items[0].name};
                std::optional<std::string> alias = items[0].alias;
                return std::make_unique<ImportStmt>(
                    std::move(modulePath), std::move(alias), startPos);
            }
            error("(AR) متوقع 'من' بعد قائمة العناصر.\n"
                  "    مثال: استورد عنصر1، عنصر2 من وحدة.فرعية\n"
                  "(EN) Expected 'from' after items list.\n"
                  "    Example: import item1, item2 from module.sub");
            return nullptr;
        }
        
        // (AR) تحليل مسار الوحدة بعد 'من' / (EN) Parse module path after 'from'
        std::vector<std::string> modulePath;
        if (!parseModulePath(modulePath)) return nullptr;
        
        return std::make_unique<FromImportStmt>(
            std::move(modulePath), std::move(items), false, startPos);
    }
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) الحالة 4: استورد وحدة — استيراد بسيط (الصيغة القديمة)
    // (EN) Case 4: import module — simple import (old syntax)
    // ═══════════════════════════════════════════════════════════════════
    std::vector<std::string> modulePath = {firstName};
    std::optional<std::string> alias = std::nullopt;
    
    return std::make_unique<ImportStmt>(std::move(modulePath), std::move(alias), startPos);
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
 *   من "وحدة" استورد عنصر
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
    if (!parseModulePath(modulePath)) return nullptr;
    
    // (AR) يجب أن تتبعها كلمة 'استورد' / (EN) Must be followed by 'import'
    if (!match(TT::KEYWORD_IMPORT)) {
        error("(AR) متوقع 'استورد' بعد اسم الوحدة. (EN) Expected 'import' after module name.");
        return nullptr;
    }
    
    // (AR) فحص استيراد wildcard (*) / (EN) Check for wildcard import (*)
    if (match(TT::OP_MULTIPLY)) {
        return std::make_unique<FromImportStmt>(
            std::move(modulePath),
            std::vector<ImportItem>(),
            true,  // isWildcard
            startPos
        );
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
    } while (matchComma());
    
    if (items.empty()) {
        error("(AR) يجب استيراد عنصر واحد على الأقل. (EN) Must import at least one item.");
        return nullptr;
    }
    
    return std::make_unique<FromImportStmt>(
        std::move(modulePath), std::move(items), false, startPos);
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
 *   صدّر *                  // (AR) تصدير شامل مجرّد / (EN) bare wildcard export
 *   صدّر * من وحدة          // (AR) إعادة تصدير شاملة / (EN) wildcard re-export
 *
 * Grammar / القواعد:
 *   ExportDecl := 'صدّر' Declaration
 *   Declaration := FunctionDecl | ClassDecl | VarDecl
 */
StmtPtr ParserCore::parseExportDecl() {
    Position startPos = previous_.getPosition();
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) الحالة 1: إعادة تصدير شاملة — صدّر * من وحدة، أو تصدير شامل
    //      مجرّد — صدّر * (بلا 'من'، انظر الحالة 1-أ أدناه).
    // (EN) Case 1: Wildcard re-export — export * from module, or bare
    //      wildcard export — export * (no 'from', see Case 1a below).
    // ═══════════════════════════════════════════════════════════════
    if (match(TT::OP_MULTIPLY)) {
        // ═══════════════════════════════════════════════════════════════
        // (AR) الحالة 1-أ: تصدير شامل مجرّد — صدّر * (بلا 'من')
        //      دلالته: «صدّر كل رموز الوحدة الحاليّة العلنيّة». يُمثَّل بـ
        //      ReExportStmt بمسار وحدة فارغ + isWildcard=true (لا وحدة مصدرٍ
        //      خارجيّة). تتعامل معه زوّار SIR/المفسّر كلا-عمليّة لإعادة التصدير
        //      (لا تحميل وحدة)، إذ تبقى رموز الوحدة في نطاقها. تستعمله وحدات
        //      stdlib التوثيقيّة (مثل رسومات.ص)، وكان رفضه يحجب المترجم عن أيّ
        //      برنامج فيه «استورد رسومات» (RFC 0001 — الجذر الفعليّ لـP0-3).
        // (EN) Case 1a: Bare wildcard export — export * (without 'from')
        //      Means "export all public symbols of the current module".
        //      Represented as ReExportStmt with empty modulePath + isWildcard=true
        //      (no source module); SIR/interpreter visitors treat it as a
        //      re-export no-op since the symbols already live in this module.
        // ═══════════════════════════════════════════════════════════════
        if (!check(TT::KEYWORD_FROM)) {
            return std::make_unique<AST::ReExportStmt>(
                std::vector<std::string>{}, std::vector<AST::ImportItem>{}, true, startPos);
        }
        // (AR) أُكِّد وجود 'من' بـcheck أعلاه؛ نستهلكه هنا (match يُرجِع true حتمًا).
        // (EN) 'من' was confirmed by the check above; consume it here (match is sure to succeed).
        advance(); // (AR) استهلاك 'من' / (EN) consume 'من'
        std::vector<std::string> modulePath;
        if (!parseModulePath(modulePath)) {
            error("(AR) متوقع مسار الوحدة بعد 'من'.\n(EN) Expected module path after 'من'.");
            return nullptr;
        }
        return std::make_unique<AST::ReExportStmt>(
            std::move(modulePath), std::vector<AST::ImportItem>{}, true, startPos);
    }
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) الحالة 2: إعادة تصدير انتقائية — صدّر عنصر1، عنصر2 من وحدة
    //      IDENTIFIER ثم FROM → إعادة تصدير عنصر واحد
    //      IDENTIFIER ثم COMMA → قائمة إعادة تصدير
    //      IDENTIFIER ثم AS → عنصر واحد بإسم مستعار ثم FROM
    //      (القواعد غير ملتبسة — لا حاجة لـ backtracking)
    // (EN) Case 2: Selective re-export — export item1, item2 from module
    // ═══════════════════════════════════════════════════════════════
    if (check(TT::IDENTIFIER) && !check(TT::KEYWORD_FUNCTION) && !check(TT::KEYWORD_CLASS)
        && !check(TT::KEYWORD_CONST) && !check(TT::KEYWORD_VAR)) {
        
        auto nextType = peekNext().getType();
        
        // (AR) IDENTIFIER ثم FROM أو COMMA أو AS → إعادة تصدير
        if (nextType == TT::KEYWORD_FROM || nextType == TT::COMMA || nextType == TT::KEYWORD_AS) {
            std::vector<AST::ImportItem> items;
            
            // (AR) قراءة العنصر الأول
            AST::ImportItem firstItem;
            firstItem.name = current_.getValue();
            firstItem.position = current_.getPosition();
            advance();
            
            // (AR) دعم الأسماء المستعارة: صدّر عنصر كـ اسم_جديد من وحدة
            if (match(TT::KEYWORD_AS)) {
                if (!check(TT::IDENTIFIER)) {
                    error("(AR) متوقع اسم مستعار بعد 'كـ'.\n(EN) Expected alias after 'كـ'.");
                    return nullptr;
                }
                firstItem.alias = current_.getValue();
                advance();
            }
            items.push_back(firstItem);
            
            // (AR) قراءة عناصر إضافية مفصولة بفاصلة
            while (match(TT::COMMA)) {
                if (!check(TT::IDENTIFIER)) break;
                AST::ImportItem item;
                item.name = current_.getValue();
                item.position = current_.getPosition();
                advance();
                
                if (match(TT::KEYWORD_AS)) {
                    if (check(TT::IDENTIFIER)) {
                        item.alias = current_.getValue();
                        advance();
                    }
                }
                items.push_back(item);
            }
            
            // (AR) يجب أن يتبع 'من'
            if (!match(TT::KEYWORD_FROM)) {
                error("(AR) متوقع 'من' بعد قائمة العناصر في إعادة التصدير.\n"
                      "(EN) Expected 'من' (from) after item list in re-export.\n"
                      "مثال: صدّر عنصر1، عنصر2 من وحدة");
                return nullptr;
            }
            
            std::vector<std::string> modulePath;
            if (!parseModulePath(modulePath)) {
                error("(AR) متوقع مسار الوحدة بعد 'من'.\n(EN) Expected module path after 'من'.");
                return nullptr;
            }
            return std::make_unique<AST::ReExportStmt>(
                std::move(modulePath), std::move(items), false, startPos);
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) الحالات العادية: صدّر دالة|صنف|ثابت|متغير|خارجي|اسم
    // (EN) Regular cases: export function|class|const|var|extern|name
    // ═══════════════════════════════════════════════════════════════
    StmtPtr declaration = nullptr;
    
    // Parse the declaration that follows 'صدّر'
    // We need to advance past the keyword and call the appropriate parser
    // (AR) RFC 0034: «خارجي/خارجية» كلمة ناعمة — لا ندخل حاجز extern إلّا حين يكون
    //      ما بعدها شكل الصيغة القديمة فعلًا ('دالة' أو '("رمز")'). غير ذلك تسقط
    //      اللفظة لمسار «صدّر اسم» أدناه (إعادة تصدير دالة اسمها خارجية مثلًا).
    // (EN) RFC 0034: 'خارجي/خارجية' is a soft keyword — enter the extern barrier only
    //      when the continuation is actually legacy-shaped ('دالة' or '("sym")').
    //      Otherwise the lexeme falls through to the export-name path below
    //      (re-exporting a function named خارجية, for instance).
    if (check(TT::KEYWORD_EXTERN) &&
        (peekNext().getType() == TT::KEYWORD_FUNCTION ||
         peekNext().getType() == TT::PAREN_LEFT)) {
        advance(); // (AR) استهلاك 'خارجي'/'خارجية' / (EN) consume the extern lexeme
        // (AR) '(' يليه غير نصّ ليس صيغة قديمة — رسالة دقيقة (استدعاء لا يُصدَّر).
        // (EN) '(' + non-string is not the legacy form — accurate message (a call
        //      expression is not exportable).
        if (check(TT::PAREN_LEFT) && peekNext().getType() != TT::STRING_LITERAL) {
            errorCatalog(Errors::ErrorCode::SYN_INVALID_CONSTRUCT_FORM, {{"construct_ar", "التصريح المُصدَّر ('" + kw(TT::KEYWORD_EXPORT) + "' يتبعها تصريح لا استدعاء)"}, {"construct_en", "exported declaration ('" + kw(TT::KEYWORD_EXPORT) + "' must be followed by a declaration, not a call)"}, {"form", kw(TT::KEYWORD_EXPORT) + " " + kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + " printf(" + kw(TT::TYPE_STRING) + ")"}});
            return nullptr;
        }
        // (AR) RFC 0034: 'صدّر خارجي دالة' أُزيلت — الصيغة الجديدة 'صدّر دالة خارجية'
        //      تمرّ عبر فرع KEYWORD_FUNCTION أدناه (parseFunctionDecl). حاجز توجيهيّ.
        // (EN) RFC 0034: 'export extern function' removed — the new form
        //      'صدّر دالة خارجية' flows through the KEYWORD_FUNCTION branch below.
        errorCatalog(Errors::ErrorCode::SYN_REMOVED_SYNTAX, {{"old", kw(TT::KEYWORD_EXPORT) + " " + kw(TT::KEYWORD_EXTERN) + " " + kw(TT::KEYWORD_FUNCTION)}, {"new", kw(TT::KEYWORD_EXPORT) + " " + kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN)}, {"example", kw(TT::KEYWORD_EXPORT) + " " + kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + " printf(" + kw(TT::TYPE_STRING) + ")"}});
        // (AR) تعافٍ يمنع تعاقب الأخطاء: استهلاك '("رمز")' الاختياريّ ثم 'دالة' ثم التصريح.
        // (EN) Cascade-preventing recovery: consume optional '("sym")', 'function', then decl.
        std::string ffiLinkName;
        if (check(TT::PAREN_LEFT)) {
            advance();
            if (check(TT::STRING_LITERAL)) {
                ffiLinkName = current_.getValue();
                advance();
            }
            if (check(TT::PAREN_RIGHT))
                advance();
        }
        if (!match(TT::KEYWORD_FUNCTION)) {
            return nullptr;
        }
        declaration = parseExternFunctionDecl(ffiLinkName);
    }
    else if (match(TT::KEYWORD_FUNCTION)) {
        // صدّر دالة ... - Function keyword consumed, now parse the function
        declaration = parseFunctionDecl();
    }
    else if (match(TT::KEYWORD_CLASS)) {
        // صدّر صنف ... - Class keyword consumed, now parse the class
        declaration = parseClassDecl();
    }
    else if (match(TT::KEYWORD_ENUM)) {
        // صدّر تعداد ... - Enum keyword consumed, now parse the enum
        declaration = parseEnumDecl();
    }
    else if (match(TT::KEYWORD_STRUCT)) {
        // صدّر بنية ... - Struct keyword consumed, now parse the struct
        declaration = parseStructDecl();
    }
    else if (match(TT::KEYWORD_CONST)) {
        // صدّر ثابت ... - Const keyword consumed, now parse variable
        declaration = parseVarDecl();
    }
    else if (match(TT::KEYWORD_VAR)) {
        // صدّر متغير ... - Var keyword consumed, now parse variable
        declaration = parseVarDecl();
    }
    else if (isTypeToken(current_.getType())) {
        // صدّر رقم س = ... - Type token, parse as variable declaration
        declaration = parseVarDecl();
    }
    else if (check(TT::IDENTIFIER) || check(TT::KEYWORD_EXTERN)) {
        // (AR) RFC 0034: KEYWORD_EXTERN هنا اسم ناعم حتمًا (بوّابة الحاجز أعلاه
        //      التقطت الأشكال القديمة) — «صدّر خارجية» إعادة تصدير باسم مشروع.
        // (EN) RFC 0034: KEYWORD_EXTERN here is necessarily a soft name (the
        //      barrier gate above captured legacy shapes) — 'صدّر خارجية'
        //      re-exports a legal name.
        // صدّر اسم = قيمة - Export alias assignment
        // (AR) دعم تصدير الأسماء المستعارة: صدّر اسم = وحدة.عضو
        // (EN) Support export alias: export name = module.member
        Token nameToken = current_;
        advance();
        
        if (match(TT::OP_ASSIGN)) {
            // (AR) تحليل التعبير على اليمين
            // (EN) Parse the RHS expression
            ExprPtr value = parseExpression();
            if (!value) {
                error("(AR) متوقع قيمة بعد '=' في تصدير الإسناد. (EN) Expected value after '=' in export assignment.");
                return nullptr;
            }
            
            // (AR) إنشاء تصريح ثابت ضمني / (EN) Create implicit const declaration
            declaration = std::make_unique<VarDeclStmt>(
                nameToken.getValue(),
                Types::SadTypeKind::Unknown,  // (AR) سيُستنتج من القيمة
                std::move(value),
                true,  // isConst = true
                nameToken.getPosition()
            );
        } else {
            // (AR) صدّر اسم — بدون قيمة
            // (EN) export name — without value (re-export)
            // Create a variable reference to the identifier
            auto varRef = std::make_unique<VariableExpr>(nameToken.getValue(), nameToken.getPosition());
            declaration = std::make_unique<VarDeclStmt>(
                nameToken.getValue(),
                Types::SadTypeKind::Unknown,
                std::move(varRef),
                true,
                nameToken.getPosition()
            );
        }
    }
    else {
        error(
            "(AR) متوقع تصريح بعد 'صدّر' (دالة، صنف، تعداد، بنية، أو متغير).\n"
            "(EN) Expected declaration after 'export' (function, class, enum, struct, or variable)."
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
