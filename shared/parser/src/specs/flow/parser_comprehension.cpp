/**
 * @file comprehension_parser.cpp
 * @brief (AR) محلل تعبيرات الفهم للقوائم والقواميس
 *        (EN) List and Dictionary Comprehension Parser
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║     ██████╗ ██████╗ ███╗   ███╗██████╗                                        ║
 * ║    ██╔════╝██╔═══██╗████╗ ████║██╔══██╗                                       ║
 * ║    ██║     ██║   ██║██╔████╔██║██████╔╝                                       ║
 * ║    ██║     ██║   ██║██║╚██╔╝██║██╔═══╝                                        ║
 * ║    ╚██████╗╚██████╔╝██║ ╚═╝ ██║██║                                            ║
 * ║     ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝                                            ║
 * ║                                                                               ║
 * ║              محلل تعبيرات الفهم بالعربية                                      ║
 * ║              Arabic Comprehension Expression Parser                           ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يحلل تعبيرات الفهم للقوائم والقواميس:
 * 
 *      ┌─────────────────────────────────────────────────────────────────────────┐
 *      │                    أنواع تعبيرات الفهم                                  │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │                                                                         │
 *      │    // فهم القوائم (List Comprehension)                                  │
 *      │    [س² لكل س في مدى(10)]                                               │
 *      │    [س لكل س في قائمة إذا س > 5]                                        │
 *      │    [س + ص لكل س في أ لكل ص في ب]                                       │
 *      │                                                                         │
 *      │    // فهم القواميس (Dict Comprehension)                                 │
 *      │    {مفتاح: قيمة لكل (مفتاح، قيمة) في أزواج}                            │
 *      │    {س: س² لكل س في مدى(5)}                                             │
 *      │                                                                         │
 *      │    // فهم المجموعات (Set Comprehension)                                 │
 *      │    {|س² لكل س في قائمة|}                                                │
 *      │                                                                         │
 *      │    // مولدات (Generator Expression)                                     │
 *      │    (س² لكل س في مدى(1000000))                                          │
 *      │                                                                         │
 *      │    // فهم غير متزامن (Async Comprehension)                              │
 *      │    [انتظر جلب(رابط) لكل رابط في روابط]                                  │
 *      │                                                                         │
 *      └─────────────────────────────────────────────────────────────────────────┘
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2024-01-24
 * @version 1.0.0
 */

#include "comprehension_parser.hpp"
#include "../include/parser.hpp"
#include "../../ast/include/ast.hpp"
#include "../../ast/include/comprehension_ast.hpp"
#include "../../lexer/include/token.hpp"
#include "../../errors/include/error_reporter.hpp"

#include <memory>
#include <vector>
#include <string>
#include <optional>

namespace sad::parser {

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) الكلمات المفتاحية
//                    (EN) Keywords
// ═══════════════════════════════════════════════════════════════════════════════

namespace comprehension_keywords {
    constexpr const char* FOR_EACH_AR = "لكل";
    constexpr const char* FOR_EACH_EN = "for";
    constexpr const char* IN_AR = "في";
    constexpr const char* IN_EN = "in";
    constexpr const char* IF_AR = "إذا";
    constexpr const char* IF_EN = "if";
    constexpr const char* ASYNC_AR = "غير_متزامن";
    constexpr const char* ASYNC_EN = "async";
    constexpr const char* AWAIT_AR = "انتظر";
    constexpr const char* AWAIT_EN = "await";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) أنواع تعبيرات الفهم
//                    (EN) Comprehension Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) نوع تعبير الفهم
 */
enum class ComprehensionKind {
    LIST,           // (AR) قائمة [...]
    DICT,           // (AR) قاموس {...}
    SET,            // (AR) مجموعة {|...|}
    GENERATOR       // (AR) مولد (...)
};

/**
 * (AR) حلقة فهم واحدة
 */
struct ComprehensionLoop {
    std::string variable;                           // (AR) متغير التكرار
    std::unique_ptr<ast::PatternNode> pattern;      // (AR) نمط التفكيك (اختياري)
    std::unique_ptr<ast::ExpressionNode> iterable;  // (AR) المتكرر
    bool is_async;                                  // (AR) لكل غير متزامن
    
    ComprehensionLoop() : is_async(false) {}
};

/**
 * (AR) شرط تصفية
 */
struct ComprehensionFilter {
    std::unique_ptr<ast::ExpressionNode> condition;
};

/**
 * (AR) عقدة AST لتعبير الفهم
 */
struct ComprehensionNode : public ast::ExpressionNode {
    ComprehensionKind kind;
    std::unique_ptr<ast::ExpressionNode> element;   // (AR) العنصر المُنتج (للقوائم)
    std::unique_ptr<ast::ExpressionNode> key;       // (AR) المفتاح (للقواميس)
    std::unique_ptr<ast::ExpressionNode> value;     // (AR) القيمة (للقواميس)
    std::vector<ComprehensionLoop> loops;           // (AR) الحلقات المتداخلة
    std::vector<ComprehensionFilter> filters;       // (AR) شروط التصفية
    bool has_await;                                 // (AR) يحتوي على await
    lexer::SourceLocation location;
    
    ComprehensionNode() : kind(ComprehensionKind::LIST), has_await(false) {}
    
    ast::NodeKind getKind() const override {
        return ast::NodeKind::Comprehension;
    }
    
    std::string toString() const override {
        std::string result;
        switch (kind) {
            case ComprehensionKind::LIST: result = "ListComp("; break;
            case ComprehensionKind::DICT: result = "DictComp("; break;
            case ComprehensionKind::SET: result = "SetComp("; break;
            case ComprehensionKind::GENERATOR: result = "GenExpr("; break;
        }
        result += "loops=" + std::to_string(loops.size());
        result += ", filters=" + std::to_string(filters.size());
        result += ")";
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) محلل تعبيرات الفهم
//                    (EN) Comprehension Parser
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) فئة محلل تعبيرات الفهم
 */
class ComprehensionParser {
private:
    Parser& parser_;
    errors::ErrorReporter& error_reporter_;
    bool in_async_context_;
    
public:
    ComprehensionParser(Parser& parser, errors::ErrorReporter& reporter)
        : parser_(parser)
        , error_reporter_(reporter)
        , in_async_context_(false)
    {}
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) إدارة السياق
    //                    (EN) Context Management
    // ═══════════════════════════════════════════════════════════════════════════
    
    void enterAsyncContext() { in_async_context_ = true; }
    void exitAsyncContext() { in_async_context_ = false; }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) التحقق من الكلمات المفتاحية
    //                    (EN) Keyword Detection
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) هل الرمز الحالي 'لكل'؟
     */
    bool isForEachKeyword() const {
        if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            return false;
        }
        const auto& text = parser_.currentToken().text;
        return text == comprehension_keywords::FOR_EACH_AR ||
               text == comprehension_keywords::FOR_EACH_EN;
    }
    
    /**
     * (AR) هل الرمز الحالي 'في'؟
     */
    bool isInKeyword() const {
        if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            return false;
        }
        const auto& text = parser_.currentToken().text;
        return text == comprehension_keywords::IN_AR ||
               text == comprehension_keywords::IN_EN;
    }
    
    /**
     * (AR) هل الرمز الحالي 'إذا'؟
     */
    bool isIfKeyword() const {
        if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            return false;
        }
        const auto& text = parser_.currentToken().text;
        return text == comprehension_keywords::IF_AR ||
               text == comprehension_keywords::IF_EN;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) تحليل فهم القوائم
    //                    (EN) List Comprehension Parsing
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) هل هذا تعبير فهم قائمة؟
     * 
     * نكتشف ذلك بالبحث عن "لكل" بعد التعبير الأول داخل []
     */
    bool isListComprehension() {
        // (AR) احفظ الموضع الحالي
        auto saved_pos = parser_.savePosition();
        
        // (AR) تخطى [ 
        if (!parser_.currentToken().is(lexer::TokenType::LBRACKET)) {
            return false;
        }
        parser_.advance();
        
        // (AR) حاول تحليل تعبير
        auto expr = parser_.parseExpression();
        
        // (AR) تحقق من وجود 'لكل'
        bool is_comp = isForEachKeyword();
        
        // (AR) استعد الموضع
        parser_.restorePosition(saved_pos);
        
        return is_comp;
    }
    
    /**
     * (AR) تحليل تعبير فهم القائمة
     * 
     * الصيغة: [تعبير لكل متغير في متكرر [إذا شرط]...]
     */
    std::unique_ptr<ComprehensionNode> parseListComprehension() {
        auto node = std::make_unique<ComprehensionNode>();
        node->kind = ComprehensionKind::LIST;
        node->location = parser_.currentToken().location;
        
        // (AR) استهلك [
        if (!parser_.expectToken(lexer::TokenType::LBRACKET)) {
            return nullptr;
        }
        
        // (AR) تحليل التعبير الناتج
        node->element = parseComprehensionElement();
        if (!node->element) {
            return nullptr;
        }
        
        // (AR) تحليل الحلقات والشروط
        if (!parseComprehensionClauses(node.get())) {
            return nullptr;
        }
        
        // (AR) استهلك ]
        if (!parser_.expectToken(lexer::TokenType::RBRACKET)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت ']' لإنهاء تعبير الفهم",
                "Expected ']' to close comprehension expression"
            );
            return nullptr;
        }
        
        return node;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) تحليل فهم القواميس
    //                    (EN) Dict Comprehension Parsing
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) هل هذا تعبير فهم قاموس؟
     */
    bool isDictComprehension() {
        auto saved_pos = parser_.savePosition();
        
        if (!parser_.currentToken().is(lexer::TokenType::LBRACE)) {
            return false;
        }
        parser_.advance();
        
        // (AR) حاول تحليل مفتاح: قيمة
        auto key = parser_.parseExpression();
        if (!key || !parser_.currentToken().is(lexer::TokenType::COLON)) {
            parser_.restorePosition(saved_pos);
            return false;
        }
        parser_.advance(); // :
        auto value = parser_.parseExpression();
        
        bool is_comp = isForEachKeyword();
        
        parser_.restorePosition(saved_pos);
        return is_comp;
    }
    
    /**
     * (AR) تحليل تعبير فهم القاموس
     * 
     * الصيغة: {مفتاح: قيمة لكل متغير في متكرر [إذا شرط]...}
     */
    std::unique_ptr<ComprehensionNode> parseDictComprehension() {
        auto node = std::make_unique<ComprehensionNode>();
        node->kind = ComprehensionKind::DICT;
        node->location = parser_.currentToken().location;
        
        // (AR) استهلك {
        if (!parser_.expectToken(lexer::TokenType::LBRACE)) {
            return nullptr;
        }
        
        // (AR) تحليل المفتاح
        node->key = parser_.parseExpression();
        if (!node->key) {
            return nullptr;
        }
        
        // (AR) استهلك :
        if (!parser_.expectToken(lexer::TokenType::COLON)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت ':' بين المفتاح والقيمة",
                "Expected ':' between key and value"
            );
            return nullptr;
        }
        
        // (AR) تحليل القيمة
        node->value = parser_.parseExpression();
        if (!node->value) {
            return nullptr;
        }
        
        // (AR) تحليل الحلقات والشروط
        if (!parseComprehensionClauses(node.get())) {
            return nullptr;
        }
        
        // (AR) استهلك }
        if (!parser_.expectToken(lexer::TokenType::RBRACE)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '}' لإنهاء تعبير الفهم",
                "Expected '}' to close comprehension expression"
            );
            return nullptr;
        }
        
        return node;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) تحليل المولدات
    //                    (EN) Generator Expression Parsing
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) هل هذا تعبير مولد؟
     */
    bool isGeneratorExpression() {
        auto saved_pos = parser_.savePosition();
        
        if (!parser_.currentToken().is(lexer::TokenType::LPAREN)) {
            return false;
        }
        parser_.advance();
        
        auto expr = parser_.parseExpression();
        bool is_gen = isForEachKeyword();
        
        parser_.restorePosition(saved_pos);
        return is_gen;
    }
    
    /**
     * (AR) تحليل تعبير المولد
     * 
     * الصيغة: (تعبير لكل متغير في متكرر [إذا شرط]...)
     */
    std::unique_ptr<ComprehensionNode> parseGeneratorExpression() {
        auto node = std::make_unique<ComprehensionNode>();
        node->kind = ComprehensionKind::GENERATOR;
        node->location = parser_.currentToken().location;
        
        // (AR) استهلك (
        if (!parser_.expectToken(lexer::TokenType::LPAREN)) {
            return nullptr;
        }
        
        // (AR) تحليل التعبير الناتج
        node->element = parseComprehensionElement();
        if (!node->element) {
            return nullptr;
        }
        
        // (AR) تحليل الحلقات والشروط
        if (!parseComprehensionClauses(node.get())) {
            return nullptr;
        }
        
        // (AR) استهلك )
        if (!parser_.expectToken(lexer::TokenType::RPAREN)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت ')' لإنهاء تعبير المولد",
                "Expected ')' to close generator expression"
            );
            return nullptr;
        }
        
        return node;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) تحليل فهم المجموعات
    //                    (EN) Set Comprehension Parsing
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) تحليل تعبير فهم المجموعة
     * 
     * الصيغة: {|تعبير لكل متغير في متكرر [إذا شرط]...|}
     */
    std::unique_ptr<ComprehensionNode> parseSetComprehension() {
        auto node = std::make_unique<ComprehensionNode>();
        node->kind = ComprehensionKind::SET;
        node->location = parser_.currentToken().location;
        
        // (AR) استهلك {|
        if (!parser_.expectToken(lexer::TokenType::LBRACE)) {
            return nullptr;
        }
        if (!parser_.expectToken(lexer::TokenType::PIPE)) {
            return nullptr;
        }
        
        // (AR) تحليل التعبير الناتج
        node->element = parseComprehensionElement();
        if (!node->element) {
            return nullptr;
        }
        
        // (AR) تحليل الحلقات والشروط
        if (!parseComprehensionClauses(node.get())) {
            return nullptr;
        }
        
        // (AR) استهلك |}
        if (!parser_.expectToken(lexer::TokenType::PIPE)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '|' قبل '}'",
                "Expected '|' before '}'"
            );
            return nullptr;
        }
        if (!parser_.expectToken(lexer::TokenType::RBRACE)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '}' لإنهاء تعبير الفهم",
                "Expected '}' to close set comprehension"
            );
            return nullptr;
        }
        
        return node;
    }
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) دوال مساعدة
    //                    (EN) Helper Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) تحليل العنصر الناتج (قد يحتوي await)
     */
    std::unique_ptr<ast::ExpressionNode> parseComprehensionElement() {
        // (AR) تحقق من وجود await
        bool has_await = false;
        if (parser_.currentToken().text == comprehension_keywords::AWAIT_AR ||
            parser_.currentToken().text == comprehension_keywords::AWAIT_EN) {
            has_await = true;
            parser_.advance();
            
            if (!in_async_context_) {
                error_reporter_.error(
                    parser_.currentToken().location,
                    "'انتظر' في تعبير الفهم يتطلب سياقاً غير متزامن",
                    "'await' in comprehension requires async context"
                );
            }
        }
        
        auto expr = parser_.parseExpression();
        
        if (has_await && expr) {
            // (AR) غلف التعبير بـ await
            auto await_node = std::make_unique<ast::AwaitExprNode>();
            await_node->inner = std::move(expr);
            return await_node;
        }
        
        return expr;
    }
    
    /**
     * (AR) تحليل حلقات وشروط الفهم
     */
    bool parseComprehensionClauses(ComprehensionNode* node) {
        // (AR) يجب أن يكون هناك على الأقل حلقة واحدة
        if (!isForEachKeyword()) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت 'لكل' في تعبير الفهم",
                "Expected 'for' in comprehension expression"
            );
            return false;
        }
        
        while (isForEachKeyword() || isIfKeyword()) {
            if (isForEachKeyword()) {
                // (AR) حلقة جديدة
                auto loop = parseComprehensionLoop();
                if (!loop) {
                    return false;
                }
                if (loop->is_async) {
                    node->has_await = true;
                }
                node->loops.push_back(std::move(*loop));
            } else if (isIfKeyword()) {
                // (AR) شرط تصفية
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
     * (AR) تحليل حلقة فهم واحدة
     */
    std::optional<ComprehensionLoop> parseComprehensionLoop() {
        ComprehensionLoop loop;
        
        // (AR) تحقق من async for
        if (parser_.currentToken().text == comprehension_keywords::ASYNC_AR ||
            parser_.currentToken().text == comprehension_keywords::ASYNC_EN) {
            loop.is_async = true;
            parser_.advance();
            
            if (!in_async_context_) {
                error_reporter_.error(
                    parser_.currentToken().location,
                    "'لكل غير_متزامن' يتطلب سياقاً غير متزامن",
                    "'async for' requires async context"
                );
            }
        }
        
        // (AR) استهلك 'لكل'
        if (!isForEachKeyword()) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت 'لكل'",
                "Expected 'for'"
            );
            return std::nullopt;
        }
        parser_.advance();
        
        // (AR) متغير التكرار أو نمط التفكيك
        if (parser_.currentToken().is(lexer::TokenType::LPAREN)) {
            // (AR) نمط تفكيك: (أ، ب)
            loop.pattern = parser_.parsePattern();
        } else if (parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            loop.variable = parser_.currentToken().text;
            parser_.advance();
        } else {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت متغيراً أو نمطاً",
                "Expected variable or pattern"
            );
            return std::nullopt;
        }
        
        // (AR) استهلك 'في'
        if (!isInKeyword()) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت 'في'",
                "Expected 'in'"
            );
            return std::nullopt;
        }
        parser_.advance();
        
        // (AR) تحليل المتكرر
        loop.iterable = parser_.parseExpression();
        if (!loop.iterable) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت تعبيراً قابلاً للتكرار",
                "Expected iterable expression"
            );
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
        if (!isIfKeyword()) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت 'إذا'",
                "Expected 'if'"
            );
            return std::nullopt;
        }
        parser_.advance();
        
        // (AR) تحليل الشرط
        filter.condition = parser_.parseExpression();
        if (!filter.condition) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت شرطاً",
                "Expected condition"
            );
            return std::nullopt;
        }
        
        return filter;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) نقاط الدخول العامة
//                    (EN) Public Entry Points
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) إنشاء محلل تعبيرات الفهم
 */
std::unique_ptr<ComprehensionParser> createComprehensionParser(
    Parser& parser,
    errors::ErrorReporter& reporter
) {
    return std::make_unique<ComprehensionParser>(parser, reporter);
}

/**
 * (AR) هل النص كلمة 'لكل'؟
 */
bool isForEachKeyword(const std::string& text) {
    return text == comprehension_keywords::FOR_EACH_AR ||
           text == comprehension_keywords::FOR_EACH_EN;
}

/**
 * (AR) هل النص كلمة 'في'؟
 */
bool isInKeyword(const std::string& text) {
    return text == comprehension_keywords::IN_AR ||
           text == comprehension_keywords::IN_EN;
}

} // namespace sad::parser
