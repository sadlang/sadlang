/**
 * @file async_parser.cpp
 * @brief (AR) محلل الكلمة المفتاحية غير_متزامن (async)
 *        (EN) Async Keyword Parser
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║      ██████╗ ███████╗██╗   ██╗███╗   ██╗ ██████╗                              ║
 * ║     ██╔══██╗██╔════╝╚██╗ ██╔╝████╗  ██║██╔════╝                              ║
 * ║     ███████║███████╗ ╚████╔╝ ██╔██╗ ██║██║                                   ║
 * ║     ██╔══██║╚════██║  ╚██╔╝  ██║╚██╗██║██║                                   ║
 * ║     ██║  ██║███████║   ██║   ██║ ╚████║╚██████╗                              ║
 * ║     ╚═╝  ╚═╝╚══════╝   ╚═╝   ╚═╝  ╚═══╝ ╚═════╝                              ║
 * ║                                                                               ║
 * ║              محلل البرمجة غير المتزامنة بالعربية                              ║
 * ║              Asynchronous Programming Parser for Arabic                       ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يحلل صيغة async/await بالعربية:
 * 
 *      ┌─────────────────────────────────────────────────────────────────────────┐
 *      │                     نموذج البرمجة غير المتزامنة                          │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │                                                                         │
 *      │    // دالة غير متزامنة                                                  │
 *      │    غير_متزامن دالة جلب_بيانات(رابط: نص) -> نتيجة<بيانات، خطأ> {        │
 *      │        دع استجابة = انتظر http::اطلب(رابط)؟؛                           │
 *      │        دع محتوى = انتظر استجابة.نص()؟؛                                  │
 *      │        موافق(حلل_json(محتوى)؟)                                          │
 *      │    }                                                                    │
 *      │                                                                         │
 *      │    // استخدام                                                           │
 *      │    غير_متزامن دالة رئيسية() {                                           │
 *      │        دع بيانات = انتظر جلب_بيانات("https://api.example.com")؟؛       │
 *      │        اطبع(بيانات)؛                                                    │
 *      │    }                                                                    │
 *      │                                                                         │
 *      └─────────────────────────────────────────────────────────────────────────┘
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2024-01-24
 * @version 1.0.0
 */

#include "async_parser.hpp"
#include "../include/parser.hpp"
#include "../../ast/include/ast.hpp"
#include "../../lexer/include/token.hpp"
#include "../../errors/include/error_reporter.hpp"

#include <memory>
#include <vector>
#include <optional>
#include <string>
#include <unordered_map>

namespace sad::parser {

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) الثوابت والكلمات المفتاحية
//                    (EN) Constants and Keywords
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) الكلمات المفتاحية العربية للبرمجة غير المتزامنة
 */
namespace keywords {
    // غير_متزامن / async
    constexpr const char* ASYNC_AR = "غير_متزامن";
    constexpr const char* ASYNC_EN = "async";
    
    // انتظر / await
    constexpr const char* AWAIT_AR = "انتظر";
    constexpr const char* AWAIT_EN = "await";
    
    // مستقبل / Future
    constexpr const char* FUTURE_AR = "مستقبل";
    constexpr const char* FUTURE_EN = "Future";
    
    // مولد / Generator
    constexpr const char* GENERATOR_AR = "مولد";
    constexpr const char* GENERATOR_EN = "Generator";
    
    // تدفق / Stream
    constexpr const char* STREAM_AR = "تدفق";
    constexpr const char* STREAM_EN = "Stream";
    
    // أنتج / yield
    constexpr const char* YIELD_AR = "أنتج";
    constexpr const char* YIELD_EN = "yield";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) عقدة AST للدالة غير المتزامنة
//                    (EN) Async Function AST Node
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) نوع الدالة غير المتزامنة
 */
enum class AsyncFunctionKind {
    REGULAR_ASYNC,      // (AR) دالة غير متزامنة عادية
    ASYNC_GENERATOR,    // (AR) مولد غير متزامن
    ASYNC_CLOSURE       // (AR) إغلاق غير متزامن
};

/**
 * (AR) عقدة AST للدالة غير المتزامنة
 */
struct AsyncFunctionNode : public ast::ASTNode {
    std::string name;                           // (AR) اسم الدالة
    std::vector<ast::Parameter> parameters;     // (AR) المعاملات
    std::unique_ptr<ast::TypeNode> return_type; // (AR) نوع الإرجاع
    std::unique_ptr<ast::BlockNode> body;       // (AR) جسم الدالة
    AsyncFunctionKind kind;                     // (AR) نوع الدالة
    bool is_public;                             // (AR) هل عامة؟
    
    /**
     * (AR) الموقع المصدري
     */
    lexer::SourceLocation location;
    
    AsyncFunctionNode() : kind(AsyncFunctionKind::REGULAR_ASYNC), is_public(false) {}
    
    /**
     * (AR) الحصول على نوع العقدة
     */
    ast::NodeKind getKind() const override {
        return ast::NodeKind::AsyncFunction;
    }
    
    /**
     * (AR) تحويل إلى سلسلة للتصحيح
     */
    std::string toString() const override {
        std::string result = "AsyncFunction(";
        result += "name=" + name;
        result += ", kind=" + std::to_string(static_cast<int>(kind));
        result += ", params=" + std::to_string(parameters.size());
        result += ")";
        return result;
    }
};

/**
 * (AR) عقدة AST لتعبير الانتظار
 */
struct AwaitExprNode : public ast::ExpressionNode {
    std::unique_ptr<ast::ExpressionNode> expression;    // (AR) التعبير المُنتَظَر
    lexer::SourceLocation location;
    
    ast::NodeKind getKind() const override {
        return ast::NodeKind::AwaitExpr;
    }
    
    std::string toString() const override {
        return "Await(" + (expression ? expression->toString() : "null") + ")";
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) محلل async
//                    (EN) Async Parser
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) فئة محلل البرمجة غير المتزامنة
 */
class AsyncParser {
private:
    Parser& parser_;                        // (AR) مرجع للمحلل الرئيسي
    errors::ErrorReporter& error_reporter_; // (AR) مُبلِّغ الأخطاء
    
public:
    /**
     * (AR) مُنشئ المحلل
     */
    AsyncParser(Parser& parser, errors::ErrorReporter& reporter)
        : parser_(parser), error_reporter_(reporter) {}
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) التحقق من الكلمات المفتاحية
    //                    (EN) Keyword Checking
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) هل الرمز الحالي هو 'غير_متزامن'؟
     */
    bool isAsyncKeyword() const {
        if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            return false;
        }
        
        const auto& text = parser_.currentToken().text;
        return text == keywords::ASYNC_AR || text == keywords::ASYNC_EN;
    }
    
    /**
     * (AR) هل الرمز الحالي هو 'انتظر'؟
     */
    bool isAwaitKeyword() const {
        if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            return false;
        }
        
        const auto& text = parser_.currentToken().text;
        return text == keywords::AWAIT_AR || text == keywords::AWAIT_EN;
    }
    
    /**
     * (AR) هل الرمز الحالي هو 'أنتج'؟
     */
    bool isYieldKeyword() const {
        if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            return false;
        }
        
        const auto& text = parser_.currentToken().text;
        return text == keywords::YIELD_AR || text == keywords::YIELD_EN;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) تحليل الدالة غير المتزامنة
    //                    (EN) Async Function Parsing
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) تحليل دالة غير متزامنة
     * 
     * الصيغة:
     *     غير_متزامن دالة اسم(معاملات) -> نوع_إرجاع { جسم }
     * 
     * @returns عقدة AST للدالة غير المتزامنة
     */
    std::unique_ptr<AsyncFunctionNode> parseAsyncFunction() {
        auto node = std::make_unique<AsyncFunctionNode>();
        node->location = parser_.currentToken().location;
        
        // (AR) استهلك 'غير_متزامن'
        if (!consumeAsyncKeyword()) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت 'غير_متزامن' في بداية الدالة",
                "Expected 'غير_متزامن' at the start of async function"
            );
            return nullptr;
        }
        
        // (AR) تحقق من وجود 'دالة'
        if (!parser_.expectKeyword("دالة") && !parser_.expectKeyword("function")) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت 'دالة' بعد 'غير_متزامن'",
                "Expected 'دالة' after 'غير_متزامن'"
            );
            return nullptr;
        }
        
        // (AR) تحليل اسم الدالة
        if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت اسم الدالة",
                "Expected function name"
            );
            return nullptr;
        }
        node->name = parser_.currentToken().text;
        parser_.advance();
        
        // (AR) تحليل المعاملات
        if (!parseAsyncParameters(node->parameters)) {
            return nullptr;
        }
        
        // (AR) تحليل نوع الإرجاع (اختياري)
        if (parser_.currentToken().is(lexer::TokenType::ARROW)) {
            parser_.advance(); // (AR) استهلك '->'
            node->return_type = parser_.parseType();
            if (!node->return_type) {
                error_reporter_.error(
                    parser_.currentToken().location,
                    "توقعت نوع الإرجاع بعد '->'",
                    "Expected return type after '->'"
                );
                return nullptr;
            }
        } else {
            // (AR) الإرجاع الافتراضي: مستقبل<()>
            node->return_type = createDefaultFutureType();
        }
        
        // (AR) تحليل جسم الدالة
        node->body = parser_.parseBlock();
        if (!node->body) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت جسم الدالة",
                "Expected function body"
            );
            return nullptr;
        }
        
        // (AR) تحقق من أن الجسم يحتوي على 'انتظر' أو 'أنتج'
        validateAsyncBody(*node);
        
        return node;
    }
    
    /**
     * (AR) تحليل تعبير الانتظار
     * 
     * الصيغة:
     *     انتظر تعبير
     * 
     * @returns عقدة AST لتعبير الانتظار
     */
    std::unique_ptr<AwaitExprNode> parseAwaitExpression() {
        auto node = std::make_unique<AwaitExprNode>();
        node->location = parser_.currentToken().location;
        
        // (AR) استهلك 'انتظر'
        if (!consumeAwaitKeyword()) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت 'انتظر'",
                "Expected 'انتظر'"
            );
            return nullptr;
        }
        
        // (AR) تحليل التعبير المُنتَظَر
        node->expression = parser_.parseExpression();
        if (!node->expression) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت تعبيراً بعد 'انتظر'",
                "Expected expression after 'انتظر'"
            );
            return nullptr;
        }
        
        return node;
    }
    
    /**
     * (AR) تحليل إغلاق غير متزامن (async closure)
     * 
     * الصيغة:
     *     غير_متزامن |معاملات| { جسم }
     * 
     * @returns عقدة AST للإغلاق غير المتزامن
     */
    std::unique_ptr<AsyncFunctionNode> parseAsyncClosure() {
        auto node = std::make_unique<AsyncFunctionNode>();
        node->location = parser_.currentToken().location;
        node->kind = AsyncFunctionKind::ASYNC_CLOSURE;
        node->name = ""; // (AR) الإغلاق بدون اسم
        
        // (AR) استهلك 'غير_متزامن'
        if (!consumeAsyncKeyword()) {
            return nullptr;
        }
        
        // (AR) توقع '|' لبداية المعاملات
        if (!parser_.currentToken().is(lexer::TokenType::PIPE)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '|' لبداية معاملات الإغلاق",
                "Expected '|' for closure parameters"
            );
            return nullptr;
        }
        parser_.advance();
        
        // (AR) تحليل معاملات الإغلاق
        parseClosureParameters(node->parameters);
        
        // (AR) توقع '|' لنهاية المعاملات
        if (!parser_.currentToken().is(lexer::TokenType::PIPE)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '|' لنهاية معاملات الإغلاق",
                "Expected '|' to close closure parameters"
            );
            return nullptr;
        }
        parser_.advance();
        
        // (AR) تحليل الجسم
        if (parser_.currentToken().is(lexer::TokenType::LBRACE)) {
            node->body = parser_.parseBlock();
        } else {
            // (AR) إغلاق بتعبير واحد
            auto expr = parser_.parseExpression();
            if (expr) {
                node->body = wrapExpressionInBlock(std::move(expr));
            }
        }
        
        return node;
    }
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) دوال مساعدة خاصة
    //                    (EN) Private Helper Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) استهلاك كلمة 'غير_متزامن'
     */
    bool consumeAsyncKeyword() {
        if (isAsyncKeyword()) {
            parser_.advance();
            return true;
        }
        return false;
    }
    
    /**
     * (AR) استهلاك كلمة 'انتظر'
     */
    bool consumeAwaitKeyword() {
        if (isAwaitKeyword()) {
            parser_.advance();
            return true;
        }
        return false;
    }
    
    /**
     * (AR) تحليل معاملات الدالة غير المتزامنة
     */
    bool parseAsyncParameters(std::vector<ast::Parameter>& params) {
        // (AR) توقع '('
        if (!parser_.currentToken().is(lexer::TokenType::LPAREN)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '(' لبداية المعاملات",
                "Expected '(' for parameters"
            );
            return false;
        }
        parser_.advance();
        
        // (AR) تحليل المعاملات
        while (!parser_.currentToken().is(lexer::TokenType::RPAREN) &&
               !parser_.currentToken().is(lexer::TokenType::EOF_TOKEN)) {
            
            ast::Parameter param;
            
            // (AR) اسم المعامل
            if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
                error_reporter_.error(
                    parser_.currentToken().location,
                    "توقعت اسم المعامل",
                    "Expected parameter name"
                );
                return false;
            }
            param.name = parser_.currentToken().text;
            parser_.advance();
            
            // (AR) توقع ':'
            if (!parser_.currentToken().is(lexer::TokenType::COLON)) {
                error_reporter_.error(
                    parser_.currentToken().location,
                    "توقعت ':' بعد اسم المعامل",
                    "Expected ':' after parameter name"
                );
                return false;
            }
            parser_.advance();
            
            // (AR) نوع المعامل
            param.type = parser_.parseType();
            if (!param.type) {
                return false;
            }
            
            params.push_back(std::move(param));
            
            // (AR) فاصلة للمعامل التالي
            if (parser_.currentToken().is(lexer::TokenType::COMMA)) {
                parser_.advance();
            } else {
                break;
            }
        }
        
        // (AR) توقع ')'
        if (!parser_.currentToken().is(lexer::TokenType::RPAREN)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت ')' لنهاية المعاملات",
                "Expected ')' for end of parameters"
            );
            return false;
        }
        parser_.advance();
        
        return true;
    }
    
    /**
     * (AR) تحليل معاملات الإغلاق
     */
    void parseClosureParameters(std::vector<ast::Parameter>& params) {
        while (!parser_.currentToken().is(lexer::TokenType::PIPE) &&
               !parser_.currentToken().is(lexer::TokenType::EOF_TOKEN)) {
            
            ast::Parameter param;
            
            if (parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
                param.name = parser_.currentToken().text;
                parser_.advance();
                
                // (AR) نوع اختياري
                if (parser_.currentToken().is(lexer::TokenType::COLON)) {
                    parser_.advance();
                    param.type = parser_.parseType();
                }
                
                params.push_back(std::move(param));
                
                if (parser_.currentToken().is(lexer::TokenType::COMMA)) {
                    parser_.advance();
                } else {
                    break;
                }
            } else {
                break;
            }
        }
    }
    
    /**
     * (AR) إنشاء نوع مستقبل افتراضي
     */
    std::unique_ptr<ast::TypeNode> createDefaultFutureType() {
        auto future_type = std::make_unique<ast::GenericTypeNode>();
        future_type->name = keywords::FUTURE_AR;
        future_type->type_arguments.push_back(std::make_unique<ast::UnitTypeNode>());
        return future_type;
    }
    
    /**
     * (AR) تغليف تعبير في كتلة
     */
    std::unique_ptr<ast::BlockNode> wrapExpressionInBlock(
        std::unique_ptr<ast::ExpressionNode> expr
    ) {
        auto block = std::make_unique<ast::BlockNode>();
        auto return_stmt = std::make_unique<ast::ReturnNode>();
        return_stmt->value = std::move(expr);
        block->statements.push_back(std::move(return_stmt));
        return block;
    }
    
    /**
     * (AR) التحقق من صحة جسم الدالة غير المتزامنة
     */
    void validateAsyncBody(const AsyncFunctionNode& node) {
        // (AR) تحقق من وجود 'انتظر' أو 'أنتج' في الجسم
        // هذا تحذير فقط، ليس خطأ
        bool hasAwaitOrYield = containsAwaitOrYield(node.body.get());
        
        if (!hasAwaitOrYield) {
            error_reporter_.warning(
                node.location,
                "الدالة غير المتزامنة لا تحتوي على 'انتظر' أو 'أنتج'",
                "Async function does not contain 'await' or 'yield'"
            );
        }
    }
    
    /**
     * (AR) هل تحتوي الكتلة على 'انتظر' أو 'أنتج'؟
     */
    bool containsAwaitOrYield(const ast::BlockNode* block) {
        if (!block) return false;
        
        // (AR) فحص جميع العبارات في الكتلة
        for (const auto& stmt : block->statements) {
            if (containsAwaitOrYieldInNode(stmt.get())) {
                return true;
            }
        }
        return false;
    }
    
    /**
     * (AR) هل تحتوي العقدة على 'انتظر' أو 'أنتج'؟
     */
    bool containsAwaitOrYieldInNode(const ast::ASTNode* node) {
        if (!node) return false;
        
        // (AR) تحقق من نوع العقدة
        if (node->getKind() == ast::NodeKind::AwaitExpr ||
            node->getKind() == ast::NodeKind::YieldExpr) {
            return true;
        }
        
        // (AR) فحص العقد الفرعية (يحتاج تنفيذ كامل لكل نوع عقدة)
        // هذا تنفيذ مبسط
        return false;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) محلل تعبير async المتقدم
//                    (EN) Advanced Async Expression Parser
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) معلومات سياق async
 */
struct AsyncContext {
    bool in_async_function;     // (AR) داخل دالة غير متزامنة
    bool in_async_closure;      // (AR) داخل إغلاق غير متزامن
    bool in_generator;          // (AR) داخل مولد
    int await_depth;            // (AR) عمق تداخل الانتظار
    
    AsyncContext() 
        : in_async_function(false)
        , in_async_closure(false)
        , in_generator(false)
        , await_depth(0) {}
};

/**
 * (AR) محلل تعبيرات async المتقدمة
 */
class AdvancedAsyncParser {
private:
    Parser& parser_;
    errors::ErrorReporter& error_reporter_;
    std::vector<AsyncContext> context_stack_;
    
public:
    AdvancedAsyncParser(Parser& parser, errors::ErrorReporter& reporter)
        : parser_(parser), error_reporter_(reporter) {
        context_stack_.push_back(AsyncContext());
    }
    
    /**
     * (AR) دخول سياق async جديد
     */
    void enterAsyncContext(bool is_function, bool is_closure, bool is_generator) {
        AsyncContext ctx;
        ctx.in_async_function = is_function;
        ctx.in_async_closure = is_closure;
        ctx.in_generator = is_generator;
        ctx.await_depth = 0;
        context_stack_.push_back(ctx);
    }
    
    /**
     * (AR) خروج من سياق async
     */
    void exitAsyncContext() {
        if (context_stack_.size() > 1) {
            context_stack_.pop_back();
        }
    }
    
    /**
     * (AR) الحصول على السياق الحالي
     */
    AsyncContext& currentContext() {
        return context_stack_.back();
    }
    
    /**
     * (AR) التحقق من صحة استخدام 'انتظر'
     */
    bool validateAwaitUsage(const lexer::SourceLocation& loc) {
        if (!currentContext().in_async_function && 
            !currentContext().in_async_closure) {
            error_reporter_.error(
                loc,
                "'انتظر' يمكن استخدامها فقط داخل دالة غير متزامنة",
                "'await' can only be used inside an async function"
            );
            return false;
        }
        return true;
    }
    
    /**
     * (AR) التحقق من صحة استخدام 'أنتج'
     */
    bool validateYieldUsage(const lexer::SourceLocation& loc) {
        if (!currentContext().in_generator) {
            error_reporter_.error(
                loc,
                "'أنتج' يمكن استخدامها فقط داخل مولد",
                "'yield' can only be used inside a generator"
            );
            return false;
        }
        return true;
    }
    
    /**
     * (AR) تحليل كتلة async
     */
    std::unique_ptr<ast::BlockNode> parseAsyncBlock() {
        auto ctx = currentContext();
        ctx.await_depth++;
        
        auto block = parser_.parseBlock();
        
        ctx.await_depth--;
        return block;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) نقاط الدخول العامة
//                    (EN) Public Entry Points
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) إنشاء محلل async جديد
 */
std::unique_ptr<AsyncParser> createAsyncParser(
    Parser& parser, 
    errors::ErrorReporter& reporter
) {
    return std::make_unique<AsyncParser>(parser, reporter);
}

/**
 * (AR) هل النص كلمة async؟
 */
bool isAsyncKeyword(const std::string& text) {
    return text == keywords::ASYNC_AR || text == keywords::ASYNC_EN;
}

/**
 * (AR) هل النص كلمة await؟
 */
bool isAwaitKeyword(const std::string& text) {
    return text == keywords::AWAIT_AR || text == keywords::AWAIT_EN;
}

/**
 * (AR) هل النص كلمة yield؟
 */
bool isYieldKeyword(const std::string& text) {
    return text == keywords::YIELD_AR || text == keywords::YIELD_EN;
}

} // namespace sad::parser
