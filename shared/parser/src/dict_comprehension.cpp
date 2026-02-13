/**
 * @file dict_comprehension.cpp
 * @brief (AR) محلل تعبيرات فهم القواميس المتقدمة
 *        (EN) Advanced Dictionary Comprehension Parser
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║    ██████╗ ██╗ ██████╗████████╗                                               ║
 * ║    ██╔══██╗██║██╔════╝╚══██╔══╝                                               ║
 * ║    ██║  ██║██║██║        ██║                                                  ║
 * ║    ██║  ██║██║██║        ██║                                                  ║
 * ║    ██████╔╝██║╚██████╗   ██║                                                  ║
 * ║    ╚═════╝ ╚═╝ ╚═════╝   ╚═╝                                                  ║
 * ║                                                                               ║
 * ║              محلل فهم القواميس المتقدم                                        ║
 * ║              Advanced Dict Comprehension Parser                               ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يوفر ميزات متقدمة لفهم القواميس:
 * 
 *      ┌─────────────────────────────────────────────────────────────────────────┐
 *      │                    ميزات فهم القواميس المتقدمة                          │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │                                                                         │
 *      │    // تحويل مفاتيح (Key Transformation)                                 │
 *      │    {مفتاح.أسفل(): قيمة لكل (مفتاح، قيمة) في قاموس}                     │
 *      │                                                                         │
 *      │    // تجميع (Grouping)                                                  │
 *      │    {مفتاح: [ع لكل ع في مجموعة] لكل (مفتاح، مجموعة) في تجميع(قائمة)}    │
 *      │                                                                         │
 *      │    // دمج قواميس (Merging)                                              │
 *      │    {**ق1، **ق2، مفتاح_جديد: قيمة_جديدة}                                 │
 *      │                                                                         │
 *      │    // فهم مع قيمة افتراضية (Default Value)                              │
 *      │    {مفتاح: قاموس.احصل(مفتاح، افتراضي) لكل مفتاح في مفاتيح}             │
 *      │                                                                         │
 *      │    // قواميس متداخلة (Nested Dicts)                                     │
 *      │    {أ: {ب: قيمة لكل ب في ب_قائمة} لكل أ في أ_قائمة}                    │
 *      │                                                                         │
 *      │    // فهم مع تحويل أنواع (Type Conversion)                              │
 *      │    {نص(مفتاح): عدد(قيمة) لكل (مفتاح، قيمة) في أزواج}                  │
 *      │                                                                         │
 *      └─────────────────────────────────────────────────────────────────────────┘
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2024-01-24
 * @version 1.0.0
 */

#include "dict_comprehension.hpp"
#include "comprehension_parser.hpp"
#include "../include/parser.hpp"
#include "../../ast/include/ast.hpp"
#include "../../lexer/include/token.hpp"
#include "../../errors/include/error_reporter.hpp"

#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>

namespace sad::parser {

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) أنواع عمليات القواميس
//                    (EN) Dict Operation Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) نوع عملية القاموس
 */
enum class DictOperationKind {
    KEY_VALUE,          // (AR) مفتاح: قيمة
    SPREAD,             // (AR) **قاموس
    COMPUTED_KEY,       // (AR) [تعبير]: قيمة
    CONDITIONAL         // (AR) مفتاح: قيمة إذا شرط وإلا قيمة_أخرى
};

/**
 * (AR) عملية في تعبير القاموس
 */
struct DictOperation {
    DictOperationKind kind;
    std::unique_ptr<ast::ExpressionNode> key;
    std::unique_ptr<ast::ExpressionNode> value;
    std::unique_ptr<ast::ExpressionNode> spread_source;     // (AR) لـ **قاموس
    std::unique_ptr<ast::ExpressionNode> condition;         // (AR) للقيمة الشرطية
    std::unique_ptr<ast::ExpressionNode> else_value;        // (AR) القيمة البديلة
    bool is_computed_key;
    
    DictOperation() : kind(DictOperationKind::KEY_VALUE), is_computed_key(false) {}
};

/**
 * (AR) عقدة AST لفهم القاموس المتقدم
 */
struct AdvancedDictCompNode : public ast::ExpressionNode {
    std::vector<DictOperation> operations;
    std::vector<ComprehensionLoop> loops;
    std::vector<ComprehensionFilter> filters;
    bool has_spread;
    bool has_await;
    lexer::SourceLocation location;
    
    AdvancedDictCompNode() : has_spread(false), has_await(false) {}
    
    ast::NodeKind getKind() const override {
        return ast::NodeKind::AdvancedDictComp;
    }
    
    std::string toString() const override {
        return "AdvDictComp(ops=" + std::to_string(operations.size()) + 
               ", loops=" + std::to_string(loops.size()) + ")";
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) محلل فهم القواميس المتقدم
//                    (EN) Advanced Dict Comprehension Parser
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) فئة محلل فهم القواميس المتقدم
 */
class DictComprehensionParser {
private:
    Parser& parser_;
    errors::ErrorReporter& error_reporter_;
    bool in_async_context_;
    
public:
    DictComprehensionParser(Parser& parser, errors::ErrorReporter& reporter)
        : parser_(parser)
        , error_reporter_(reporter)
        , in_async_context_(false)
    {}
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) تحليل عمليات القاموس
    //                    (EN) Dict Operation Parsing
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) تحليل عملية نشر (spread)
     * 
     * الصيغة: **قاموس
     */
    std::optional<DictOperation> parseSpreadOperation() {
        DictOperation op;
        op.kind = DictOperationKind::SPREAD;
        
        // (AR) توقع **
        if (!parser_.currentToken().is(lexer::TokenType::STAR)) {
            return std::nullopt;
        }
        parser_.advance();
        
        if (!parser_.currentToken().is(lexer::TokenType::STAR)) {
            // (AR) نجمة واحدة فقط، ليس spread
            parser_.retreat();
            return std::nullopt;
        }
        parser_.advance();
        
        // (AR) تحليل مصدر النشر
        op.spread_source = parser_.parseExpression();
        if (!op.spread_source) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت قاموساً بعد '**'",
                "Expected dict after '**'"
            );
            return std::nullopt;
        }
        
        return op;
    }
    
    /**
     * (AR) تحليل مفتاح محسوب
     * 
     * الصيغة: [تعبير]: قيمة
     */
    std::optional<DictOperation> parseComputedKeyOperation() {
        DictOperation op;
        op.kind = DictOperationKind::COMPUTED_KEY;
        op.is_computed_key = true;
        
        // (AR) توقع [
        if (!parser_.expectToken(lexer::TokenType::LBRACKET)) {
            return std::nullopt;
        }
        
        // (AR) تحليل تعبير المفتاح
        op.key = parser_.parseExpression();
        if (!op.key) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت تعبير المفتاح",
                "Expected key expression"
            );
            return std::nullopt;
        }
        
        // (AR) توقع ]
        if (!parser_.expectToken(lexer::TokenType::RBRACKET)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت ']'",
                "Expected ']'"
            );
            return std::nullopt;
        }
        
        // (AR) توقع :
        if (!parser_.expectToken(lexer::TokenType::COLON)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت ':'",
                "Expected ':'"
            );
            return std::nullopt;
        }
        
        // (AR) تحليل القيمة
        op.value = parser_.parseExpression();
        if (!op.value) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت قيمة",
                "Expected value"
            );
            return std::nullopt;
        }
        
        return op;
    }
    
    /**
     * (AR) تحليل قيمة شرطية
     * 
     * الصيغة: مفتاح: قيمة إذا شرط وإلا قيمة_بديلة
     */
    std::optional<DictOperation> parseConditionalValueOperation() {
        DictOperation op;
        op.kind = DictOperationKind::KEY_VALUE;
        
        // (AR) تحليل المفتاح
        op.key = parser_.parseExpression();
        if (!op.key) {
            return std::nullopt;
        }
        
        // (AR) توقع :
        if (!parser_.expectToken(lexer::TokenType::COLON)) {
            return std::nullopt;
        }
        
        // (AR) تحليل القيمة
        op.value = parser_.parseExpression();
        if (!op.value) {
            return std::nullopt;
        }
        
        // (AR) تحقق من وجود شرط
        if (parser_.currentToken().text == "إذا" || 
            parser_.currentToken().text == "if") {
            op.kind = DictOperationKind::CONDITIONAL;
            parser_.advance();
            
            // (AR) تحليل الشرط
            op.condition = parser_.parseExpression();
            if (!op.condition) {
                error_reporter_.error(
                    parser_.currentToken().location,
                    "توقعت شرطاً",
                    "Expected condition"
                );
                return std::nullopt;
            }
            
            // (AR) تحقق من وجود else
            if (parser_.currentToken().text == "وإلا" ||
                parser_.currentToken().text == "else") {
                parser_.advance();
                
                op.else_value = parser_.parseExpression();
                if (!op.else_value) {
                    error_reporter_.error(
                        parser_.currentToken().location,
                        "توقعت قيمة بديلة",
                        "Expected alternative value"
                    );
                    return std::nullopt;
                }
            }
        }
        
        return op;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) تحليل فهم القاموس الكامل
    //                    (EN) Full Dict Comprehension Parsing
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) تحليل تعبير فهم القاموس المتقدم
     */
    std::unique_ptr<AdvancedDictCompNode> parseAdvancedDictComprehension() {
        auto node = std::make_unique<AdvancedDictCompNode>();
        node->location = parser_.currentToken().location;
        
        // (AR) توقع {
        if (!parser_.expectToken(lexer::TokenType::LBRACE)) {
            return nullptr;
        }
        
        // (AR) تحليل العمليات
        while (!parser_.currentToken().is(lexer::TokenType::RBRACE) &&
               !parser_.currentToken().is(lexer::TokenType::EOF_TOKEN)) {
            
            std::optional<DictOperation> op;
            
            // (AR) تحقق من spread
            if (parser_.currentToken().is(lexer::TokenType::STAR)) {
                op = parseSpreadOperation();
                if (op) {
                    node->has_spread = true;
                }
            }
            // (AR) تحقق من مفتاح محسوب
            else if (parser_.currentToken().is(lexer::TokenType::LBRACKET)) {
                op = parseComputedKeyOperation();
            }
            // (AR) مفتاح: قيمة عادي أو شرطي
            else {
                op = parseConditionalValueOperation();
            }
            
            if (!op) {
                return nullptr;
            }
            
            node->operations.push_back(std::move(*op));
            
            // (AR) تحقق من وجود 'لكل' للفهم
            if (isForEachKeyword(parser_.currentToken().text)) {
                // (AR) هذا فهم قاموس
                if (!parseComprehensionClauses(node.get())) {
                    return nullptr;
                }
                break;
            }
            
            // (AR) فاصلة للعنصر التالي
            if (parser_.currentToken().is(lexer::TokenType::COMMA)) {
                parser_.advance();
            } else {
                break;
            }
        }
        
        // (AR) توقع }
        if (!parser_.expectToken(lexer::TokenType::RBRACE)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '}'",
                "Expected '}'"
            );
            return nullptr;
        }
        
        return node;
    }
    
private:
    /**
     * (AR) تحليل حلقات وشروط الفهم
     */
    bool parseComprehensionClauses(AdvancedDictCompNode* node) {
        while (isForEachKeyword(parser_.currentToken().text) ||
               parser_.currentToken().text == "إذا" ||
               parser_.currentToken().text == "if") {
            
            if (isForEachKeyword(parser_.currentToken().text)) {
                auto loop = parseComprehensionLoop();
                if (!loop) {
                    return false;
                }
                if (loop->is_async) {
                    node->has_await = true;
                }
                node->loops.push_back(std::move(*loop));
            } else {
                auto filter = parseComprehensionFilter();
                if (!filter) {
                    return false;
                }
                node->filters.push_back(std::move(*filter));
            }
        }
        
        return true;
    }
    
    /**
     * (AR) تحليل حلقة فهم
     */
    std::optional<ComprehensionLoop> parseComprehensionLoop() {
        ComprehensionLoop loop;
        
        // (AR) تحقق من async
        if (parser_.currentToken().text == "غير_متزامن" ||
            parser_.currentToken().text == "async") {
            loop.is_async = true;
            parser_.advance();
        }
        
        // (AR) استهلك 'لكل'
        parser_.advance();
        
        // (AR) متغير أو نمط
        if (parser_.currentToken().is(lexer::TokenType::LPAREN)) {
            loop.pattern = parser_.parsePattern();
        } else {
            loop.variable = parser_.currentToken().text;
            parser_.advance();
        }
        
        // (AR) 'في'
        if (!isInKeyword(parser_.currentToken().text)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت 'في'",
                "Expected 'in'"
            );
            return std::nullopt;
        }
        parser_.advance();
        
        // (AR) المتكرر
        loop.iterable = parser_.parseExpression();
        if (!loop.iterable) {
            return std::nullopt;
        }
        
        return loop;
    }
    
    /**
     * (AR) تحليل شرط تصفية
     */
    std::optional<ComprehensionFilter> parseComprehensionFilter() {
        ComprehensionFilter filter;
        
        // (AR) استهلك 'إذا'
        parser_.advance();
        
        // (AR) الشرط
        filter.condition = parser_.parseExpression();
        if (!filter.condition) {
            return std::nullopt;
        }
        
        return filter;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) تحويل فهم القاموس إلى كود
//                    (EN) Dict Comprehension Code Generation
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) تحويل فهم القاموس إلى حلقات عادية
 * 
 * مثال:
 *     {س: س² لكل س في مدى(5)}
 * 
 * يتحول إلى:
 *     دع _نتيجة = قاموس::جديد()؛
 *     لكل س في مدى(5) {
 *         _نتيجة.أدخل(س، س²)؛
 *     }
 *     _نتيجة
 */
class DictComprehensionDesugarer {
public:
    std::unique_ptr<ast::BlockNode> desugar(const AdvancedDictCompNode* comp) {
        auto block = std::make_unique<ast::BlockNode>();
        
        // (AR) إنشاء القاموس الناتج
        auto dict_decl = std::make_unique<ast::LetStmtNode>();
        dict_decl->name = "_نتيجة";
        dict_decl->initializer = std::make_unique<ast::CallExprNode>("قاموس::جديد");
        block->statements.push_back(std::move(dict_decl));
        
        // (AR) إنشاء الحلقات المتداخلة
        std::unique_ptr<ast::StatementNode> innermost = generateInsertion(comp);
        
        for (auto it = comp->loops.rbegin(); it != comp->loops.rend(); ++it) {
            auto for_loop = generateForLoop(*it, std::move(innermost));
            innermost = std::move(for_loop);
        }
        
        block->statements.push_back(std::move(innermost));
        
        // (AR) إرجاع النتيجة
        auto return_expr = std::make_unique<ast::IdentifierExprNode>("_نتيجة");
        block->statements.push_back(std::move(return_expr));
        
        return block;
    }
    
private:
    std::unique_ptr<ast::StatementNode> generateInsertion(const AdvancedDictCompNode* comp) {
        auto insert = std::make_unique<ast::CallStmtNode>();
        insert->callee = std::make_unique<ast::MemberAccessNode>(
            std::make_unique<ast::IdentifierExprNode>("_نتيجة"),
            "أدخل"
        );
        
        // (AR) أضف المفتاح والقيمة
        for (const auto& op : comp->operations) {
            if (op.kind == DictOperationKind::KEY_VALUE ||
                op.kind == DictOperationKind::COMPUTED_KEY) {
                insert->arguments.push_back(op.key->clone());
                insert->arguments.push_back(op.value->clone());
            }
        }
        
        // (AR) غلف بشروط التصفية
        std::unique_ptr<ast::StatementNode> result = std::move(insert);
        
        for (auto it = comp->filters.rbegin(); it != comp->filters.rend(); ++it) {
            auto if_stmt = std::make_unique<ast::IfStmtNode>();
            if_stmt->condition = it->condition->clone();
            if_stmt->then_block = std::make_unique<ast::BlockNode>();
            if_stmt->then_block->statements.push_back(std::move(result));
            result = std::move(if_stmt);
        }
        
        return result;
    }
    
    std::unique_ptr<ast::ForStmtNode> generateForLoop(
        const ComprehensionLoop& loop,
        std::unique_ptr<ast::StatementNode> body
    ) {
        auto for_stmt = std::make_unique<ast::ForStmtNode>();
        
        if (!loop.variable.empty()) {
            for_stmt->variable = loop.variable;
        } else if (loop.pattern) {
            for_stmt->pattern = loop.pattern->clone();
        }
        
        for_stmt->iterable = loop.iterable->clone();
        for_stmt->is_async = loop.is_async;
        
        for_stmt->body = std::make_unique<ast::BlockNode>();
        for_stmt->body->statements.push_back(std::move(body));
        
        return for_stmt;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) نقاط الدخول العامة
//                    (EN) Public Entry Points
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) إنشاء محلل فهم القواميس
 */
std::unique_ptr<DictComprehensionParser> createDictComprehensionParser(
    Parser& parser,
    errors::ErrorReporter& reporter
) {
    return std::make_unique<DictComprehensionParser>(parser, reporter);
}

/**
 * (AR) إنشاء محول فهم القواميس
 */
std::unique_ptr<DictComprehensionDesugarer> createDictComprehensionDesugarer() {
    return std::make_unique<DictComprehensionDesugarer>();
}

} // namespace sad::parser
