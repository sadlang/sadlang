/**
 * @file yield_parser.cpp
 * @brief (AR) محلل الكلمة المفتاحية أنتج (yield)
 *        (EN) Yield Keyword Parser
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║    ██╗   ██╗██╗███████╗██╗     ██████╗                                        ║
 * ║    ╚██╗ ██╔╝██║██╔════╝██║     ██╔══██╗                                       ║
 * ║     ╚████╔╝ ██║█████╗  ██║     ██║  ██║                                       ║
 * ║      ╚██╔╝  ██║██╔══╝  ██║     ██║  ██║                                       ║
 * ║       ██║   ██║███████╗███████╗██████╔╝                                       ║
 * ║       ╚═╝   ╚═╝╚══════╝╚══════╝╚═════╝                                        ║
 * ║                                                                               ║
 * ║              محلل تعبير الإنتاج - أنتج                                        ║
 * ║              Yield Expression Parser for Arabic                               ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يحلل تعبير 'أنتج' (yield) بصيغ متعددة:
 * 
 *      ┌─────────────────────────────────────────────────────────────────────────┐
 *      │                     صيغ استخدام 'أنتج'                                  │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │                                                                         │
 *      │    // الصيغة الأساسية                                                   │
 *      │    أنتج قيمة؛                                                           │
 *      │                                                                         │
 *      │    // إنتاج بدون قيمة (للمولدات التي تنتج ())                           │
 *      │    أنتج؛                                                                │
 *      │                                                                         │
 *      │    // إنتاج من مولد آخر (yield from / yield*)                           │
 *      │    أنتج* مولد_آخر؛                                                       │
 *      │                                                                         │
 *      │    // استخدام في مولد                                                    │
 *      │    مولد دالة فيبوناتشي() -> مولد<عدد> {                                 │
 *      │        دع متغير (أ، ب) = (0، 1)؛                                        │
 *      │        حلقة {                                                           │
 *      │            أنتج أ؛                                                       │
 *      │            (أ، ب) = (ب، أ + ب)؛                                         │
 *      │        }                                                                │
 *      │    }                                                                    │
 *      │                                                                         │
 *      │    // استخدام في تدفق غير متزامن                                        │
 *      │    غير_متزامن مولد دالة أحداث() -> تدفق<حدث> {                          │
 *      │        لكل حدث في انتظر مستمع.أحداث() {                                  │
 *      │            أنتج حدث؛                                                    │
 *      │        }                                                                │
 *      │    }                                                                    │
 *      │                                                                         │
 *      └─────────────────────────────────────────────────────────────────────────┘
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2024-01-24
 * @version 1.0.0
 */

#include "yield_parser.hpp"
#include "../include/parser.hpp"
#include "../../ast/include/ast.hpp"
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

namespace yield_keywords {
    constexpr const char* YIELD_AR = "أنتج";
    constexpr const char* YIELD_EN = "yield";
    constexpr const char* GENERATOR_AR = "مولد";
    constexpr const char* GENERATOR_EN = "gen";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) أنواع تعبير yield
//                    (EN) Yield Expression Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) نوع تعبير yield
 */
enum class YieldKind {
    SIMPLE,         // (AR) أنتج قيمة
    EMPTY,          // (AR) أنتج (بدون قيمة)
    FROM            // (AR) أنتج* مولد (تفويض)
};

/**
 * (AR) عقدة AST لتعبير yield
 */
struct YieldExprNode : public ast::ExpressionNode {
    YieldKind kind;
    std::unique_ptr<ast::ExpressionNode> value;     // (AR) القيمة المُنتجة
    lexer::SourceLocation location;
    
    YieldExprNode() : kind(YieldKind::EMPTY) {}
    
    ast::NodeKind getKind() const override {
        return ast::NodeKind::YieldExpr;
    }
    
    std::string toString() const override {
        std::string result = "Yield(";
        switch (kind) {
            case YieldKind::SIMPLE:
                result += "simple";
                break;
            case YieldKind::EMPTY:
                result += "empty";
                break;
            case YieldKind::FROM:
                result += "from";
                break;
        }
        if (value) {
            result += ", " + value->toString();
        }
        result += ")";
        return result;
    }
};

/**
 * (AR) عقدة AST لدالة المولد
 */
struct GeneratorFunctionNode : public ast::ASTNode {
    std::string name;
    std::vector<ast::Parameter> parameters;
    std::unique_ptr<ast::TypeNode> yield_type;      // (AR) نوع القيمة المُنتجة
    std::unique_ptr<ast::TypeNode> return_type;     // (AR) نوع الإرجاع النهائي
    std::unique_ptr<ast::BlockNode> body;
    bool is_async;                                  // (AR) هل غير متزامن (تدفق)؟
    bool is_public;
    lexer::SourceLocation location;
    
    GeneratorFunctionNode() : is_async(false), is_public(false) {}
    
    ast::NodeKind getKind() const override {
        return ast::NodeKind::GeneratorFunction;
    }
    
    std::string toString() const override {
        std::string result = is_async ? "AsyncGenerator(" : "Generator(";
        result += "name=" + name;
        result += ", params=" + std::to_string(parameters.size());
        result += ")";
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) سياق المولد
//                    (EN) Generator Context
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) سياق تحليل المولد
 */
struct GeneratorContext {
    bool in_generator;              // (AR) داخل دالة مولد
    bool in_async_generator;        // (AR) داخل تدفق غير متزامن
    const types::Type* yield_type;  // (AR) نوع yield المتوقع
    int yield_count;                // (AR) عدد yield في الدالة
    
    GeneratorContext()
        : in_generator(false)
        , in_async_generator(false)
        , yield_type(nullptr)
        , yield_count(0) {}
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) محلل yield
//                    (EN) Yield Parser
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) فئة محلل yield
 */
class YieldParser {
private:
    Parser& parser_;
    errors::ErrorReporter& error_reporter_;
    std::vector<GeneratorContext> context_stack_;
    
public:
    YieldParser(Parser& parser, errors::ErrorReporter& reporter)
        : parser_(parser)
        , error_reporter_(reporter)
    {
        context_stack_.push_back(GeneratorContext());
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) إدارة السياق
    //                    (EN) Context Management
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) دخول سياق مولد جديد
     */
    void enterGeneratorContext(bool is_async, const types::Type* yield_type = nullptr) {
        GeneratorContext ctx;
        ctx.in_generator = true;
        ctx.in_async_generator = is_async;
        ctx.yield_type = yield_type;
        ctx.yield_count = 0;
        context_stack_.push_back(ctx);
    }
    
    /**
     * (AR) خروج من سياق المولد
     */
    int exitGeneratorContext() {
        int yield_count = 0;
        if (context_stack_.size() > 1) {
            yield_count = context_stack_.back().yield_count;
            context_stack_.pop_back();
        }
        return yield_count;
    }
    
    /**
     * (AR) الحصول على السياق الحالي
     */
    GeneratorContext& currentContext() {
        return context_stack_.back();
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) التحقق من الكلمات المفتاحية
    //                    (EN) Keyword Detection
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) هل الرمز الحالي 'أنتج'؟
     */
    bool isYieldKeyword() const {
        if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            return false;
        }
        const auto& text = parser_.currentToken().text;
        return text == yield_keywords::YIELD_AR || 
               text == yield_keywords::YIELD_EN;
    }
    
    /**
     * (AR) هل الرمز الحالي 'مولد'؟
     */
    bool isGeneratorKeyword() const {
        if (!parser_.currentToken().is(lexer::TokenType::IDENTIFIER)) {
            return false;
        }
        const auto& text = parser_.currentToken().text;
        return text == yield_keywords::GENERATOR_AR || 
               text == yield_keywords::GENERATOR_EN;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) تحليل تعبير yield
    //                    (EN) Yield Expression Parsing
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) تحليل تعبير yield
     */
    std::unique_ptr<YieldExprNode> parseYieldExpression() {
        auto node = std::make_unique<YieldExprNode>();
        node->location = parser_.currentToken().location;
        
        // (AR) التحقق من السياق
        if (!currentContext().in_generator) {
            error_reporter_.error(
                node->location,
                "'أنتج' يمكن استخدامها فقط داخل دالة مولد",
                "'yield' can only be used inside a generator function"
            );
            return nullptr;
        }
        
        // (AR) استهلك 'أنتج'
        parser_.advance();
        
        // (AR) تحقق من yield*
        if (parser_.currentToken().is(lexer::TokenType::STAR)) {
            parser_.advance();
            node->kind = YieldKind::FROM;
            
            // (AR) تحليل المولد المُفوَّض إليه
            node->value = parser_.parseExpression();
            if (!node->value) {
                error_reporter_.error(
                    parser_.currentToken().location,
                    "توقعت مولداً بعد 'أنتج*'",
                    "Expected generator after 'yield*'"
                );
                return nullptr;
            }
        } 
        // (AR) تحقق من yield بدون قيمة
        else if (isEndOfYield()) {
            node->kind = YieldKind::EMPTY;
        }
        // (AR) yield مع قيمة
        else {
            node->kind = YieldKind::SIMPLE;
            node->value = parser_.parseExpression();
            if (!node->value) {
                error_reporter_.error(
                    parser_.currentToken().location,
                    "توقعت تعبيراً بعد 'أنتج'",
                    "Expected expression after 'yield'"
                );
                return nullptr;
            }
        }
        
        // (AR) تحديث عداد yield
        currentContext().yield_count++;
        
        // (AR) تحقق من نوع القيمة المُنتجة
        validateYieldType(node.get());
        
        return node;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    (AR) تحليل دالة المولد
    //                    (EN) Generator Function Parsing
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) تحليل دالة مولد
     * 
     * الصيغة:
     *     مولد دالة اسم(معاملات) -> مولد<نوع> { جسم }
     *     غير_متزامن مولد دالة اسم(معاملات) -> تدفق<نوع> { جسم }
     */
    std::unique_ptr<GeneratorFunctionNode> parseGeneratorFunction(bool is_async) {
        auto node = std::make_unique<GeneratorFunctionNode>();
        node->location = parser_.currentToken().location;
        node->is_async = is_async;
        
        // (AR) استهلك 'مولد'
        if (!isGeneratorKeyword()) {
            error_reporter_.error(
                node->location,
                "توقعت 'مولد'",
                "Expected 'مولد'"
            );
            return nullptr;
        }
        parser_.advance();
        
        // (AR) توقع 'دالة'
        if (!parser_.expectKeyword("دالة") && !parser_.expectKeyword("function")) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت 'دالة' بعد 'مولد'",
                "Expected 'دالة' after 'مولد'"
            );
            return nullptr;
        }
        
        // (AR) اسم الدالة
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
        
        // (AR) المعاملات
        if (!parseGeneratorParameters(node->parameters)) {
            return nullptr;
        }
        
        // (AR) نوع الإرجاع
        if (parser_.currentToken().is(lexer::TokenType::ARROW)) {
            parser_.advance();
            
            // (AR) توقع مولد<T> أو تدفق<T>
            auto return_type = parser_.parseType();
            if (return_type) {
                extractYieldType(return_type.get(), node.get());
            }
        }
        
        // (AR) دخول سياق المولد
        enterGeneratorContext(is_async, getYieldType(node.get()));
        
        // (AR) جسم الدالة
        node->body = parser_.parseBlock();
        
        // (AR) خروج من سياق المولد
        int yield_count = exitGeneratorContext();
        
        // (AR) تحقق من وجود yield واحد على الأقل
        if (yield_count == 0) {
            error_reporter_.warning(
                node->location,
                "دالة المولد لا تحتوي على 'أنتج'",
                "Generator function does not contain 'yield'"
            );
        }
        
        return node;
    }
    
private:
    /**
     * (AR) هل وصلنا لنهاية yield؟
     */
    bool isEndOfYield() const {
        auto& token = parser_.currentToken();
        return token.is(lexer::TokenType::SEMICOLON) ||
               token.is(lexer::TokenType::RBRACE) ||
               token.is(lexer::TokenType::RPAREN) ||
               token.is(lexer::TokenType::COMMA) ||
               token.is(lexer::TokenType::EOF_TOKEN);
    }
    
    /**
     * (AR) تحليل معاملات المولد
     */
    bool parseGeneratorParameters(std::vector<ast::Parameter>& params) {
        if (!parser_.currentToken().is(lexer::TokenType::LPAREN)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت '('",
                "Expected '('"
            );
            return false;
        }
        parser_.advance();
        
        while (!parser_.currentToken().is(lexer::TokenType::RPAREN) &&
               !parser_.currentToken().is(lexer::TokenType::EOF_TOKEN)) {
            
            ast::Parameter param;
            
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
        }
        
        if (!parser_.currentToken().is(lexer::TokenType::RPAREN)) {
            error_reporter_.error(
                parser_.currentToken().location,
                "توقعت ')'",
                "Expected ')'"
            );
            return false;
        }
        parser_.advance();
        
        return true;
    }
    
    /**
     * (AR) استخراج نوع yield من نوع الإرجاع
     */
    void extractYieldType(const ast::TypeNode* return_type, GeneratorFunctionNode* node) {
        // (AR) إذا كان مولد<T> أو تدفق<T>، استخرج T
        if (auto* generic = dynamic_cast<const ast::GenericTypeNode*>(return_type)) {
            if (generic->name == "مولد" || generic->name == "Generator" ||
                generic->name == "تدفق" || generic->name == "Stream") {
                if (!generic->type_arguments.empty()) {
                    node->yield_type = generic->type_arguments[0]->clone();
                }
                if (generic->type_arguments.size() > 1) {
                    node->return_type = generic->type_arguments[1]->clone();
                }
            }
        }
    }
    
    /**
     * (AR) الحصول على نوع yield
     */
    const types::Type* getYieldType(const GeneratorFunctionNode* node) {
        // (AR) تحويل TypeNode إلى Type
        // (يحتاج تنفيذ كامل)
        return nullptr;
    }
    
    /**
     * (AR) التحقق من نوع القيمة المُنتجة
     */
    void validateYieldType(const YieldExprNode* node) {
        auto& ctx = currentContext();
        
        if (!ctx.yield_type || !node->value) {
            return;
        }
        
        // (AR) مقارنة الأنواع
        // (يحتاج تنفيذ كامل مع نظام الأنواع)
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) نقاط الدخول العامة
//                    (EN) Public Entry Points
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) إنشاء محلل yield جديد
 */
std::unique_ptr<YieldParser> createYieldParser(
    Parser& parser,
    errors::ErrorReporter& reporter
) {
    return std::make_unique<YieldParser>(parser, reporter);
}

/**
 * (AR) هل النص كلمة yield؟
 */
bool isYieldKeyword(const std::string& text) {
    return text == yield_keywords::YIELD_AR || 
           text == yield_keywords::YIELD_EN;
}

/**
 * (AR) هل النص كلمة generator؟
 */
bool isGeneratorKeyword(const std::string& text) {
    return text == yield_keywords::GENERATOR_AR || 
           text == yield_keywords::GENERATOR_EN;
}

} // namespace sad::parser
